#include <treegen/Module.h>

#include <charconv>
#include <system_error>

#include <treegen/Error.h>

namespace treegen {

namespace {

[[noreturn]] void fail(const std::string& reason, size_t offset) {
	throw RewriteError("parseModules: " + reason + " at offset " + std::to_string(offset));
}

bool isSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

float parseNumber(std::string_view text, size_t& pos) {
	float value = 0.0f;
	const char* begin = text.data() + pos;
	const char* end = text.data() + text.size();
	std::from_chars_result result = std::from_chars(begin, end, value);
	if (result.ec != std::errc())
		fail("expected a number", pos);
	pos += static_cast<size_t>(result.ptr - begin);
	return value;
}

}

ModuleString parseModules(std::string_view text) {
	ModuleString modules;
	size_t i = 0;
	while (i < text.size()) {
		char c = text[i];
		if (isSpace(c)) {
			++i;
			continue;
		}
		if (c == '(' || c == ')' || c == ',' || c == '~')
			fail(std::string("unexpected '") + c + "'", i);

		Module module;
		module.symbol = c;
		++i;
		if (i < text.size() && text[i] == '~') {
			module.randomized = true;
			++i;
		}
		if (i < text.size() && text[i] == '(') {
			++i;
			for (;;) {
				module.params.push_back(parseNumber(text, i));
				if (i >= text.size())
					fail("missing ')'", i);
				if (text[i] == ',') {
					++i;
					continue;
				}
				if (text[i] == ')') {
					++i;
					break;
				}
				fail("expected ',' or ')'", i);
			}
		}
		modules.push_back(std::move(module));
	}
	return modules;
}

std::string toString(const ModuleString& modules) {
	std::string text;
	for (const Module& module : modules) {
		text += module.symbol;
		if (module.randomized)
			text += '~';
		if (!module.params.empty()) {
			text += '(';
			for (size_t i = 0; i < module.params.size(); ++i) {
				if (i > 0)
					text += ',';
				text += std::to_string(module.params[i]);
			}
			text += ')';
		}
	}
	return text;
}

}
