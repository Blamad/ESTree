#include <treegen/Expression.h>

#include <charconv>
#include <system_error>

#include <treegen/Error.h>

namespace treegen {

namespace {

bool isSpace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool isDigit(char c) {
	return c >= '0' && c <= '9';
}

bool isIdentifierStart(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool isIdentifierPart(char c) {
	return isIdentifierStart(c) || isDigit(c);
}

}

// Recursive descent:  sum := term (('+'|'-') term)*   term := unary (('*'|'/') unary)*
//                     unary := ('-'|'+') unary | primary   primary := number | identifier | '(' sum ')'
class ExpressionParser {
public:
	ExpressionParser(Expression& target, std::string_view text, const std::vector<std::string>& names)
		: out(target), text(text), names(names) {}

	void run() {
		skipSpace();
		if (pos >= text.size())
			fail("empty expression");
		sum(0);
		skipSpace();
		if (pos < text.size())
			fail(std::string("unexpected '") + text[pos] + "'");
		out.constant = true;
		for (const Expression::Instruction& instruction : out.program)
			if (instruction.op == Expression::Op::Param)
				out.constant = false;
	}

private:
	Expression& out;
	std::string_view text;
	const std::vector<std::string>& names;
	std::size_t pos = 0;
	std::size_t stack = 0;

	[[noreturn]] void fail(const std::string& reason) const {
		throw RewriteError("expression \"" + std::string(text) + "\": " + reason + " at offset " + std::to_string(pos));
	}

	void skipSpace() {
		while (pos < text.size() && isSpace(text[pos]))
			++pos;
	}

	void emit(Expression::Op op, std::size_t index = 0, double value = 0.0) {
		out.program.push_back({ op, index, value });
		switch (op) {
		case Expression::Op::Literal:
		case Expression::Op::Param:
			if (++stack > Expression::maxDepth + 2)
				fail("expression is too complex");
			break;
		case Expression::Op::Negate:
			break;
		default:
			--stack;
			break;
		}
	}

	void checkDepth(std::size_t depth) const {
		if (depth > Expression::maxDepth)
			fail("expression is nested too deeply");
	}

	void sum(std::size_t depth) {
		checkDepth(depth);
		term(depth);
		for (;;) {
			skipSpace();
			if (pos >= text.size() || (text[pos] != '+' && text[pos] != '-'))
				return;
			Expression::Op op = text[pos] == '+' ? Expression::Op::Add : Expression::Op::Sub;
			++pos;
			term(depth);
			emit(op);
		}
	}

	void term(std::size_t depth) {
		unary(depth);
		for (;;) {
			skipSpace();
			if (pos >= text.size() || (text[pos] != '*' && text[pos] != '/'))
				return;
			Expression::Op op = text[pos] == '*' ? Expression::Op::Mul : Expression::Op::Div;
			++pos;
			unary(depth);
			emit(op);
		}
	}

	void unary(std::size_t depth) {
		checkDepth(depth);
		skipSpace();
		if (pos < text.size() && (text[pos] == '-' || text[pos] == '+')) {
			bool negate = text[pos] == '-';
			++pos;
			unary(depth + 1);
			if (negate)
				emit(Expression::Op::Negate);
			return;
		}
		primary(depth);
	}

	void primary(std::size_t depth) {
		skipSpace();
		if (pos >= text.size())
			fail("unexpected end of expression");
		char c = text[pos];
		if (c == '(') {
			++pos;
			sum(depth + 1);
			skipSpace();
			if (pos >= text.size() || text[pos] != ')')
				fail("missing ')'");
			++pos;
			return;
		}
		if (isDigit(c) || c == '.') {
			number();
			return;
		}
		if (isIdentifierStart(c)) {
			identifier();
			return;
		}
		fail(std::string("unexpected '") + c + "'");
	}

	void number() {
		float value = 0.0f;
		const char* begin = text.data() + pos;
		const char* end = text.data() + text.size();
		std::from_chars_result result = std::from_chars(begin, end, value);
		if (result.ec != std::errc() || result.ptr == begin)
			fail("malformed number");
		pos += static_cast<std::size_t>(result.ptr - begin);
		emit(Expression::Op::Literal, 0, static_cast<double>(value));
	}

	void identifier() {
		std::size_t start = pos;
		while (pos < text.size() && isIdentifierPart(text[pos]))
			++pos;
		std::string_view name = text.substr(start, pos - start);
		for (std::size_t i = 0; i < names.size(); ++i) {
			if (names[i] == name) {
				emit(Expression::Op::Param, i);
				return;
			}
		}
		pos = start;
		fail("unknown identifier '" + std::string(name) + "'");
	}
};

Expression Expression::parse(std::string_view text, const std::vector<std::string>& names) {
	Expression expression;
	expression.source = std::string(text);
	ExpressionParser(expression, text, names).run();
	expression.paramCount = names.size();
	return expression;
}

double Expression::evaluate(const float* args, std::size_t count) const {
	if (count < paramCount)
		throw RewriteError("expression \"" + source + "\": " + std::to_string(paramCount) + " argument(s) expected, got " + std::to_string(count));

	double values[maxDepth + 2];
	std::size_t top = 0;
	for (const Instruction& instruction : program) {
		switch (instruction.op) {
		case Op::Literal:
			values[top++] = instruction.value;
			break;
		case Op::Param:
			values[top++] = static_cast<double>(args[instruction.index]);
			break;
		case Op::Negate:
			values[top - 1] = -values[top - 1];
			break;
		case Op::Add:
			--top;
			values[top - 1] += values[top];
			break;
		case Op::Sub:
			--top;
			values[top - 1] -= values[top];
			break;
		case Op::Mul:
			--top;
			values[top - 1] *= values[top];
			break;
		case Op::Div:
			--top;
			values[top - 1] /= values[top];
			break;
		}
	}
	return values[0];
}

}
