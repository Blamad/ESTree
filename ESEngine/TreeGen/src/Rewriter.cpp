#include <treegen/Rewriter.h>

#include <array>
#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include <treegen/Error.h>
#include <treegen/Expression.h>
#include <treegen/Rng.h>

namespace treegen {

namespace {

struct TemplateModule {
	char symbol = 0;
	bool randomized = false;
	std::vector<Expression> params;
};

struct CompiledRule {
	const Rule* rule = nullptr;
	std::string label;                       // "rule A(l,w) -> F(l*2)" for messages
	std::vector<TemplateModule> modules;
};

std::string ruleLabel(const Rule& rule) {
	std::string label = std::string("rule ") + rule.symbol;
	if (!rule.paramNames.empty()) {
		label += '(';
		for (size_t i = 0; i < rule.paramNames.size(); ++i) {
			if (i > 0)
				label += ',';
			label += rule.paramNames[i];
		}
		label += ')';
	}
	return label + " -> \"" + rule.production + "\"";
}

bool isSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

// Splits "a, b*(c+d)" at the commas outside parentheses.
std::vector<std::string_view> splitArguments(std::string_view text) {
	std::vector<std::string_view> parts;
	size_t start = 0;
	int level = 0;
	for (size_t i = 0; i < text.size(); ++i) {
		if (text[i] == '(')
			++level;
		else if (text[i] == ')')
			--level;
		else if (text[i] == ',' && level == 0) {
			parts.push_back(text.substr(start, i - start));
			start = i + 1;
		}
	}
	parts.push_back(text.substr(start));
	return parts;
}

// "F(l*lr)T[+(a)A(l/2,w)]" -> literal modules with expression parameters.
CompiledRule compileRule(const Rule& rule) {
	CompiledRule compiled;
	compiled.rule = &rule;
	compiled.label = ruleLabel(rule);
	const std::string& text = rule.production;

	auto fail = [&](const std::string& reason, size_t offset) {
		throw RewriteError(compiled.label + ": " + reason + " at offset " + std::to_string(offset) + " of the production");
	};

	size_t i = 0;
	while (i < text.size()) {
		char c = text[i];
		if (isSpace(c)) {
			++i;
			continue;
		}
		if (c == '(' || c == ')' || c == ',' || c == '~')
			fail(std::string("unexpected '") + c + "'", i);

		TemplateModule module;
		module.symbol = c;
		++i;
		if (i < text.size() && text[i] == '~') {
			module.randomized = true;
			++i;
		}
		if (i < text.size() && text[i] == '(') {
			size_t open = i;
			int level = 0;
			size_t close = std::string::npos;
			for (size_t j = i; j < text.size(); ++j) {
				if (text[j] == '(')
					++level;
				else if (text[j] == ')' && --level == 0) {
					close = j;
					break;
				}
			}
			if (close == std::string::npos)
				fail("missing ')'", open);
			for (std::string_view argument : splitArguments(std::string_view(text).substr(open + 1, close - open - 1))) {
				try {
					module.params.push_back(Expression::parse(argument, rule.paramNames));
				}
				catch (const RewriteError& e) {
					throw RewriteError(compiled.label + ": " + e.what());
				}
			}
			i = close + 1;
		}
		compiled.modules.push_back(std::move(module));
	}
	return compiled;
}

std::string describeModule(const Module& module) {
	return toString(ModuleString{ module });
}

[[noreturn]] void growthError(size_t iteration, size_t wouldBe, size_t limit) {
	throw RewriteError("rewrite: iteration " + std::to_string(iteration) + " exceeds maxModules = " +
		std::to_string(limit) + " (reached " + std::to_string(wouldBe) + " modules)");
}

float evaluateParameter(const Expression& expression, const CompiledRule& rule, const Module& module) {
	double value = expression.evaluate(module.params);
	float result = static_cast<float>(value);
	if (!std::isfinite(value) || !std::isfinite(result)) {
		throw RewriteError("rewrite: " + rule.label + ": expression \"" + expression.text() + "\" evaluated to " +
			(std::isnan(value) ? "NaN" : "an infinite or out-of-range value") + " for module " + describeModule(module));
	}
	return result;
}

void apply(const CompiledRule& rule, const Module& module, ModuleString& out) {
	const Rule& source = *rule.rule;
	if (module.params.size() != source.paramNames.size()) {
		throw RewriteError("rewrite: arity mismatch for symbol '" + std::string(1, module.symbol) + "': module " +
			describeModule(module) + " has " + std::to_string(module.params.size()) + " parameter(s), " + rule.label +
			" declares " + std::to_string(source.paramNames.size()));
	}
	for (const TemplateModule& t : rule.modules) {
		Module produced;
		produced.symbol = t.symbol;
		produced.randomized = t.randomized;
		produced.params.reserve(t.params.size());
		for (const Expression& expression : t.params)
			produced.params.push_back(evaluateParameter(expression, rule, module));
		out.push_back(std::move(produced));
	}
}

}

ModuleString rewrite(const TreeParams& params, const RewriteOptions& options) {
	ModuleString current = parseModules(params.axiom);
	if (current.size() > params.maxModules)
		growthError(0, current.size(), params.maxModules);

	Rng rng = Rng(options.seed).derive(1);
	std::map<const Rule*, std::unique_ptr<CompiledRule>> compiled;
	std::array<std::vector<const CompiledRule*>, 256> candidates;

	for (int iteration = 0; iteration < params.depth; ++iteration) {
		for (auto& list : candidates)
			list.clear();
		for (const auto& entry : params.rules) {
			for (const Rule& rule : entry.second) {
				if (rule.initialDepth > iteration)
					continue;
				std::unique_ptr<CompiledRule>& slot = compiled[&rule];
				if (!slot)
					slot = std::make_unique<CompiledRule>(compileRule(rule));
				candidates[static_cast<unsigned char>(rule.symbol)].push_back(slot.get());
			}
		}

		ModuleString next;
		next.reserve(current.size());
		for (Module& module : current) {
			const std::vector<const CompiledRule*>& applicable = candidates[static_cast<unsigned char>(module.symbol)];
			const CompiledRule* chosen = nullptr;
			if (!applicable.empty()) {
				double u = static_cast<double>(rng.uniform01());
				double cumulative = 0.0;
				for (const CompiledRule* rule : applicable) {
					cumulative += static_cast<double>(rule->rule->probability);
					if (u < cumulative) {
						chosen = rule;
						break;
					}
				}
			}

			size_t added = chosen ? chosen->modules.size() : 1;
			if (next.size() + added > params.maxModules)
				growthError(static_cast<size_t>(iteration), next.size() + added, params.maxModules);
			if (chosen)
				apply(*chosen, module, next);
			else
				next.push_back(std::move(module));
		}
		current = std::move(next);
	}
	return current;
}

}
