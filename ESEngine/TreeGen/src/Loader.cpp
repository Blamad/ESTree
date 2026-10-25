#include <treegen/TreeParams.h>

#include <cctype>
#include <charconv>
#include <climits>
#include <cmath>
#include <fstream>
#include <iterator>
#include <system_error>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <treegen/Error.h>

namespace treegen {

namespace {

// Same constant as the legacy loader: the params goldens depend on its exact value.
constexpr double kPi = 3.14159265359;

float degreesToRadians(double degrees) {
	return static_cast<float>((degrees * kPi) / 180.0);
}

[[noreturn]] void fail(const std::string& where, const std::string& reason) {
	throw LoadError(where + ": " + reason);
}

bool isAlnum(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

bool isBlank(char c) {
	return c == ' ' || c == '\t';
}

std::string_view skipBom(std::string_view text) {
	if (text.size() >= 3 && text.substr(0, 3) == "\xEF\xBB\xBF")
		text.remove_prefix(3);
	return text;
}

// Whole token must be a finite decimal number.
bool parseNumber(std::string_view token, double& value) {
	if (!token.empty() && token.front() == '+')
		token.remove_prefix(1);
	if (token.empty())
		return false;
	const char* begin = token.data();
	const char* end = token.data() + token.size();
	std::from_chars_result result = std::from_chars(begin, end, value);
	return result.ec == std::errc() && result.ptr == end && std::isfinite(value);
}

// ---- parameters shared by both formats ---------------------------------------------------------------------

[[noreturn]] void badParameter(const std::string& where, const std::string& key, const std::string& requirement) {
	fail(where, "parameter \"" + key + "\" " + requirement);
}

void setSeed(TreeParams& p, const std::string& where, const std::string& key, double value) {
	if (!(value >= 0.0 && value <= 4294967295.0 && value == std::floor(value)))
		badParameter(where, key, "must be an integer between 0 and 4294967295");
	p.seed = static_cast<uint32_t>(value);
}

void setMaxModules(TreeParams& p, const std::string& where, const std::string& key, double value) {
	if (!(value >= 1.0 && value <= 9007199254740992.0 && value == std::floor(value)))
		badParameter(where, key, "must be a positive integer");
	p.maxModules = static_cast<std::size_t>(value);
}

void setTropism(TreeParams& p, const std::string& where, double x, double y, double z) {
	glm::vec3 direction(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
	float length = glm::length(direction);
	if (!(length > 0.0f) || !std::isfinite(length))
		badParameter(where, "tropism", "must be a non-zero vector of three numbers");
	p.tropism = glm::normalize(direction);
}

// Applies one named number. `viaFloat` reproduces the legacy JSON path, which rounded every value to float
// before converting angles (the goldens are bit exact). Returns false when the key is not a known parameter.
bool applyKnownParameter(TreeParams& p, const std::string& where, const std::string& key, double value, bool viaFloat) {
	const double angleInput = viaFloat ? static_cast<double>(static_cast<float>(value)) : value;
	if (key == "initialLength") {
		p.initialLength = static_cast<float>(value);
	}
	else if (key == "depth") {
		if (!(value >= 0.0 && value <= static_cast<double>(INT_MAX)))
			badParameter(where, key, "must be a non-negative number");
		p.depth = static_cast<int>(value);
	}
	else if (key == "initialRadius") {
		p.initialRadius = static_cast<float>(value);
	}
	else if (key == "angle") {
		p.angle = degreesToRadians(angleInput);
	}
	else if (key == "leavesAngleDiversity") {
		p.leavesAngleDiversity = degreesToRadians(angleInput);
	}
	else if (key == "tropismBendingFactor") {
		p.tropismBendingFactor = static_cast<float>(value);
	}
	else if (key == "leavesGrowthProbability") {
		p.leafGrowthProb = static_cast<float>(value);
	}
	else if (key == "seed") {
		setSeed(p, where, key, value);
	}
	else if (key == "maxModules") {
		setMaxModules(p, where, key, value);
	}
	else {
		return false;
	}
	return true;
}

// ---- rule symbols: "A", "A(l,w)" and, in .l files, "A{2}" / "A{2}(l,w)" ---------------------------------------

struct SymbolSpec {
	char symbol = 0;
	std::vector<std::string> paramNames;
	int initialDepth = 0;
};

std::vector<std::string> splitNames(std::string_view list, const std::string& where) {
	std::vector<std::string> names;
	size_t start = 0;
	for (;;) {
		size_t comma = list.find(',', start);
		std::string_view item = list.substr(start, comma == std::string_view::npos ? std::string_view::npos : comma - start);
		while (!item.empty() && isBlank(item.front()))
			item.remove_prefix(1);
		while (!item.empty() && isBlank(item.back()))
			item.remove_suffix(1);
		if (item.empty())
			fail(where, "empty parameter name in rule symbol");
		names.emplace_back(item);
		if (comma == std::string_view::npos)
			break;
		start = comma + 1;
	}
	return names;
}

SymbolSpec parseSymbolSpec(std::string_view spec, bool allowDepth, const std::string& where) {
	const std::string quoted = "\"" + std::string(spec) + "\"";
	if (spec.empty())
		fail(where, "empty rule symbol");
	const char first = spec[0];
	if (first == '(' || first == ')' || first == ',' || first == '{' || first == '}')
		fail(where, "invalid rule symbol " + quoted);

	SymbolSpec out;
	out.symbol = first;
	bool haveParams = false, haveDepth = false;
	size_t i = 1;
	while (i < spec.size()) {
		if (spec[i] == '(' && !haveParams) {
			size_t close = spec.find(')', i + 1);
			if (close == std::string_view::npos)
				fail(where, "rule symbol " + quoted + " lacks the closing ')'");
			out.paramNames = splitNames(spec.substr(i + 1, close - i - 1), where);
			haveParams = true;
			i = close + 1;
		}
		else if (allowDepth && spec[i] == '{' && !haveDepth) {
			size_t close = spec.find('}', i + 1);
			if (close == std::string_view::npos || close == i + 1)
				fail(where, "rule symbol " + quoted + " has a malformed {depth}");
			int depth = 0;
			for (size_t k = i + 1; k < close; ++k) {
				if (spec[k] < '0' || spec[k] > '9' || depth > 100000)
					fail(where, "rule symbol " + quoted + " has a malformed {depth}");
				depth = depth * 10 + (spec[k] - '0');
			}
			out.initialDepth = depth;
			haveDepth = true;
			i = close + 1;
		}
		else {
			fail(where, "unexpected character '" + std::string(1, spec[i]) + "' in rule symbol " + quoted);
		}
	}
	return out;
}

void addRule(TreeParams& p, const SymbolSpec& spec, std::string production, float probability) {
	Rule rule;
	rule.symbol = spec.symbol;
	rule.paramNames = spec.paramNames;
	rule.production = std::move(production);
	rule.probability = probability;
	rule.initialDepth = spec.initialDepth;
	p.rules[spec.symbol].push_back(std::move(rule));
}

// Custom parameters become text in the axiom and in every production, once the whole file is known.
void substituteEverywhere(TreeParams& p) {
	if (p.customParameters.empty())
		return;
	p.axiom = substituteParameters(p.axiom, p.customParameters);
	for (auto& symbolRules : p.rules)
		for (Rule& rule : symbolRules.second)
			rule.production = substituteParameters(rule.production, p.customParameters);
}

// ---- JSON ------------------------------------------------------------------------------------------------

using JsonValue = rapidjson::Value;

std::string describeOffset(std::string_view text, size_t offset) {
	size_t line = 1, column = 1;
	for (size_t i = 0; i < offset && i < text.size(); ++i) {
		if (text[i] == '\n') {
			++line;
			column = 1;
		}
		else {
			++column;
		}
	}
	return std::to_string(line) + ":" + std::to_string(column);
}

double jsonNumber(const JsonValue& value, const std::string& where, const std::string& key) {
	if (!value.IsNumber())
		fail(where, "\"" + key + "\" must be a number");
	double number = value.GetDouble();
	if (!std::isfinite(number))
		fail(where, "\"" + key + "\" is out of range");
	return number;
}

const JsonValue* findMember(const JsonValue& object, const char* key) {
	auto it = object.FindMember(key);
	return it == object.MemberEnd() ? nullptr : &it->value;
}

void readJsonTropism(TreeParams& p, const JsonValue& value, const std::string& where) {
	if (!value.IsArray() || value.Size() != 3)
		badParameter(where, "tropism", "must be an array of three numbers");
	double c[3];
	for (rapidjson::SizeType i = 0; i < 3; ++i) {
		if (!value[i].IsNumber())
			badParameter(where, "tropism", "must be an array of three numbers");
		c[i] = value[i].GetDouble();
	}
	setTropism(p, where, c[0], c[1], c[2]);
}

void readJsonParameters(TreeParams& p, const JsonValue& parameters, const std::string& where) {
	for (auto it = parameters.MemberBegin(); it != parameters.MemberEnd(); ++it) {
		const std::string key = it->name.GetString();
		if (key == "tropism") {
			readJsonTropism(p, it->value, where);
			continue;
		}
		const double number = jsonNumber(it->value, where, key);
		if (!applyKnownParameter(p, where, key, number, true))
			p.customParameters[key] = static_cast<float>(number);
	}
}

void readJsonRules(TreeParams& p, const JsonValue& rules, const std::string& where) {
	for (rapidjson::SizeType i = 0; i < rules.Size(); ++i) {
		const std::string here = where + ": rules[" + std::to_string(i) + "]";
		const JsonValue& rule = rules[i];
		if (!rule.IsObject())
			fail(here, "a rule must be an object");

		const JsonValue* symbol = findMember(rule, "symbol");
		if (!symbol)
			fail(here, "missing \"symbol\"");
		if (!symbol->IsString())
			fail(here, "\"symbol\" must be a string");
		const JsonValue* production = findMember(rule, "production");
		if (!production)
			fail(here, "missing \"production\"");
		if (!production->IsString())
			fail(here, "\"production\" must be a string");

		float probability = 1.0f;
		if (const JsonValue* value = findMember(rule, "probability"))
			probability = static_cast<float>(jsonNumber(*value, here, "probability"));
		SymbolSpec spec = parseSymbolSpec(std::string_view(symbol->GetString(), symbol->GetStringLength()), false, here);
		if (const JsonValue* value = findMember(rule, "initialDepth")) {
			if (!value->IsInt())
				fail(here, "\"initialDepth\" must be an integer");
			spec.initialDepth = value->GetInt();
		}
		addRule(p, spec, std::string(production->GetString(), production->GetStringLength()), probability);
	}
}

// ---- legacy .l ---------------------------------------------------------------------------------------------

std::vector<std::string_view> splitTokens(std::string_view line) {
	std::vector<std::string_view> tokens;
	size_t i = 0;
	while (i < line.size()) {
		while (i < line.size() && isBlank(line[i]))
			++i;
		size_t start = i;
		while (i < line.size() && !isBlank(line[i]))
			++i;
		if (i > start)
			tokens.push_back(line.substr(start, i - start));
	}
	return tokens;
}

double legacyNumber(std::string_view token, const std::string& where, const std::string& what) {
	double value = 0.0;
	if (!parseNumber(token, value))
		fail(where, what + " \"" + std::string(token) + "\" is not a number");
	return value;
}

void readLegacyDefine(TreeParams& p, const std::vector<std::string_view>& tokens, const std::string& where) {
	if (tokens.size() < 3)
		fail(where, "define needs a name and a value");
	const std::string key(tokens[1]);
	if (key == "tropism") {
		if (tokens.size() < 5)
			badParameter(where, key, "needs three numbers");
		setTropism(p, where,
			legacyNumber(tokens[2], where, "tropism component"),
			legacyNumber(tokens[3], where, "tropism component"),
			legacyNumber(tokens[4], where, "tropism component"));
		return;
	}
	const double value = legacyNumber(tokens[2], where, "value of define " + key + ":");
	if (!applyKnownParameter(p, where, key, value, false))
		p.customParameters[key] = static_cast<float>(value);
}

}

std::string substituteParameters(std::string_view text, const std::map<std::string, float>& values) {
	std::string result(text);
	for (const auto& entry : values) {
		const std::string& name = entry.first;
		if (name.empty())
			continue;
		const std::string replacement = std::to_string(entry.second);
		std::string next;
		next.reserve(result.size());
		size_t i = 0;
		while (i < result.size()) {
			const bool starts = result.compare(i, name.size(), name) == 0;
			const bool leftBound = i == 0 || !isAlnum(result[i - 1]);
			const size_t after = i + name.size();
			const bool rightBound = after >= result.size() || !isAlnum(result[after]);
			if (starts && leftBound && rightBound) {
				next += replacement;
				i = after;
			}
			else {
				next += result[i];
				++i;
			}
		}
		result = std::move(next);
	}
	return result;
}

TreeParams parseTreeParamsJson(std::string_view json, std::string name) {
	const std::string where = name.empty() ? std::string("<json>") : name;
	json = skipBom(json);
	if (json.empty())
		fail(where, "file is empty");

	const std::string text(json);
	rapidjson::Document document;
	document.Parse(text.c_str(), text.size());
	if (document.HasParseError())
		fail(where + ":" + describeOffset(text, document.GetErrorOffset()),
			std::string("invalid JSON: ") + rapidjson::GetParseError_En(document.GetParseError()));
	if (!document.IsObject())
		fail(where, "the root must be a JSON object with \"axiom\" and \"rules\"");

	TreeParams p;
	p.name = name;

	const JsonValue* parameters = findMember(document, "parameters");
	if (parameters) {
		if (!parameters->IsObject())
			fail(where, "\"parameters\" must be an object");
		readJsonParameters(p, *parameters, where);
	}
	if (const JsonValue* seed = findMember(document, "seed"))
		setSeed(p, where, "seed", jsonNumber(*seed, where, "seed"));
	if (const JsonValue* maxModules = findMember(document, "maxModules"))
		setMaxModules(p, where, "maxModules", jsonNumber(*maxModules, where, "maxModules"));

	const JsonValue* axiom = findMember(document, "axiom");
	if (!axiom)
		fail(where, "missing required key \"axiom\"");
	if (!axiom->IsString())
		fail(where, "\"axiom\" must be a string");
	p.axiom.assign(axiom->GetString(), axiom->GetStringLength());

	const JsonValue* rules = findMember(document, "rules");
	if (!rules)
		fail(where, "missing required key \"rules\"");
	if (!rules->IsArray())
		fail(where, "\"rules\" must be an array");
	readJsonRules(p, *rules, where);

	substituteEverywhere(p);
	return p;
}

TreeParams parseTreeParamsLegacy(std::string_view text, std::string name) {
	const std::string where = name.empty() ? std::string("<l-file>") : name;
	text = skipBom(text);

	TreeParams p;
	p.name = name;

	// Old files start with bare-number lines: depth, initialLength, initialRadius, angle (degrees). They are
	// only recognised before the first axiom line.
	int headerLines = 0;
	bool haveAxiom = false;
	size_t lineNumber = 0;
	size_t start = 0;
	while (start < text.size()) {
		const size_t newline = text.find('\n', start);
		std::string_view line = text.substr(start, newline == std::string_view::npos ? std::string_view::npos : newline - start);
		start = newline == std::string_view::npos ? text.size() : newline + 1;
		++lineNumber;
		if (!line.empty() && line.back() == '\r')
			line.remove_suffix(1);
		if (line.empty() || line.front() == '#')
			continue;

		const std::string here = where + ":" + std::to_string(lineNumber);
		const std::vector<std::string_view> tokens = splitTokens(line);
		if (tokens.empty())
			continue;

		if (tokens[0] == "define") {
			readLegacyDefine(p, tokens, here);
			continue;
		}

		if (tokens.size() == 1) {
			double number = 0.0;
			if (!haveAxiom && headerLines < 4 && parseNumber(tokens[0], number)) {
				static const char* const headerKeys[4] = { "depth", "initialLength", "initialRadius", "angle" };
				applyKnownParameter(p, here, headerKeys[headerLines], number, false);
				++headerLines;
			}
			else {
				p.axiom.assign(tokens[0]);
				haveAxiom = true;
			}
			continue;
		}

		const SymbolSpec spec = parseSymbolSpec(tokens[0], true, here);
		float probability = 1.0f;
		if (tokens.size() >= 3)
			probability = static_cast<float>(legacyNumber(tokens[2], here, "probability"));
		addRule(p, spec, std::string(tokens[1]), probability);
	}

	if (!haveAxiom)
		fail(where, "no axiom line (a line with a single token)");

	substituteEverywhere(p);
	return p;
}

TreeParams loadTreeParams(const std::filesystem::path& file) {
	const std::string name = file.string();

	std::error_code ec;
	if (std::filesystem::is_directory(file, ec))
		fail(name, "is a directory, not a rule file");

	std::ifstream in(file, std::ios::binary);
	if (!in)
		fail(name, "cannot open file");
	const std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	if (in.bad())
		fail(name, "cannot read file");

	std::string extension = file.extension().string();
	for (char& c : extension)
		c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
	if (extension == ".l")
		return parseTreeParamsLegacy(content, name);
	return parseTreeParamsJson(content, name);
}

}
