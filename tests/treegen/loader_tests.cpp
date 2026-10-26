#include <doctest/doctest.h>

#include <cmath>
#include <filesystem>
#include <functional>
#include <map>
#include <string>
#include <vector>

#include <treegen/Error.h>
#include <treegen/TreeParams.h>

namespace fs = std::filesystem;
using treegen::LoadError;
using treegen::TreeParams;

namespace {

const fs::path kRepoRoot = fs::path(ESTREE_REPO_ROOT);
const fs::path kRulesDir = kRepoRoot / "Resources" / "LindenmayerRules";
const fs::path kDataDir = kRepoRoot / "tests" / "treegen" / "data";

// Runs `action`, requires a LoadError and returns its message.
std::string loadErrorOf(const std::function<void()>& action) {
	try {
		action();
	}
	catch (const LoadError& e) {
		return e.what();
	}
	catch (const std::exception& e) {
		FAIL("expected LoadError, got another exception: " << e.what());
	}
	FAIL("expected LoadError, nothing was thrown");
	return {};
}

bool contains(const std::string& text, const std::string& part) {
	return text.find(part) != std::string::npos;
}

std::string jsonErrorOf(const std::string& json) {
	return loadErrorOf([&] { treegen::parseTreeParamsJson(json, "probe.json"); });
}

std::string legacyErrorOf(const std::string& text) {
	return loadErrorOf([&] { treegen::parseTreeParamsLegacy(text, "probe.l"); });
}

const treegen::Rule& onlyRule(const TreeParams& p, char symbol) {
	auto it = p.rules.find(symbol);
	REQUIRE(it != p.rules.end());
	REQUIRE(it->second.size() == 1);
	return it->second.front();
}

}

// ---- substituteParameters ---------------------------------------------------------------------------------

TEST_CASE("substituteParameters: replaces a whole word with std::to_string(float)") {
	CHECK(treegen::substituteParameters("F(a)", { { "a", 0.5f } }) == "F(0.500000)");
	CHECK(treegen::substituteParameters("F(l*lr)", { { "lr", 1.109f } }) == "F(l*1.109000)");
}

TEST_CASE("substituteParameters: adjacent identical parameters are all replaced (was KNOWN BUG 1)") {
	CHECK(treegen::substituteParameters("F(a1,a1)", { { "a1", 35.0f } }) == "F(35.000000,35.000000)");
	CHECK(treegen::substituteParameters("a1a1", { { "a1", 35.0f } }) == "a1a1"); // one word, not two
	CHECK(treegen::substituteParameters("[a1][a1]", { { "a1", 35.0f } }) == "[35.000000][35.000000]");
}

TEST_CASE("substituteParameters: a parameter at the start or at the end is replaced (was KNOWN BUG 3)") {
	CHECK(treegen::substituteParameters("a1F", { { "a1", 35.0f } }) == "a1F"); // part of the word a1F
	CHECK(treegen::substituteParameters("a1+F", { { "a1", 35.0f } }) == "35.000000+F");
	CHECK(treegen::substituteParameters("F+a1", { { "a1", 35.0f } }) == "F+35.000000");
	CHECK(treegen::substituteParameters("a1", { { "a1", 35.0f } }) == "35.000000");
}

TEST_CASE("substituteParameters: never replaces a part of a longer word") {
	CHECK(treegen::substituteParameters("F(a10,ba1,a1x,_a1_)", { { "a1", 35.0f } }) == "F(a10,ba1,a1x,_35.000000_)");
}

TEST_CASE("substituteParameters: parameters apply in map order and the text after the last match is kept") {
	CHECK(treegen::substituteParameters("a+b", { { "a", 1.0f }, { "b", 2.0f } }) == "1.000000+2.000000");
	CHECK(treegen::substituteParameters("[v(a1)B]C", { { "a1", 35.0f } }) == "[v(35.000000)B]C");
	CHECK(treegen::substituteParameters("", { { "a", 1.0f } }) == "");
	CHECK(treegen::substituteParameters("F(x)", {}) == "F(x)");
}

// ---- JSON -------------------------------------------------------------------------------------------------

TEST_CASE("json: a full document is read field by field") {
	const char* json = R"json({
		"parameters": {
			"depth": 5, "initialLength": 2.5, "initialRadius": 0.5, "angle": 90,
			"leavesAngleDiversity": 180, "tropism": [0, -2, 0], "tropismBendingFactor": 0.12,
			"leavesGrowthProbability": 0.75, "lr": 1.109
		},
		"axiom": "F(lr)A",
		"rules": [
			{ "symbol": "A", "production": "F(1)[A]B", "probability": 0.5, "initialDepth": 2 },
			{ "symbol": "F(l,w)", "production": "F(l*lr)" }
		]
	})json";
	TreeParams p = treegen::parseTreeParamsJson(json, "full.json");
	CHECK(p.name == "full.json");
	CHECK(p.depth == 5);
	CHECK(p.initialLength == 2.5f);
	CHECK(p.initialRadius == 0.5f);
	CHECK(p.angle == doctest::Approx(1.5707963f));
	CHECK(p.leavesAngleDiversity == doctest::Approx(3.1415927f));
	CHECK(p.tropism.x == 0.0f);
	CHECK(p.tropism.y == -1.0f);
	CHECK(p.tropism.z == 0.0f);
	CHECK(p.tropismBendingFactor == 0.12f);
	CHECK(p.leafGrowthProb == 0.75f);
	REQUIRE(p.customParameters.size() == 1);
	CHECK(p.customParameters.at("lr") == 1.109f);
	CHECK(p.axiom == "F(1.109000)A");
	REQUIRE(p.rules.size() == 2);

	const treegen::Rule& a = onlyRule(p, 'A');
	CHECK(a.symbol == 'A');
	CHECK(a.production == "F(1)[A]B");
	CHECK(a.probability == 0.5f);
	CHECK(a.initialDepth == 2);
	CHECK(a.paramNames.empty());

	const treegen::Rule& f = onlyRule(p, 'F');
	CHECK(f.production == "F(l*1.109000)");
	CHECK(f.probability == 1.0f);
	CHECK(f.initialDepth == 0);
	REQUIRE(f.paramNames.size() == 2);
	CHECK(f.paramNames[0] == "l");
	CHECK(f.paramNames[1] == "w");
}

TEST_CASE("json: missing optional keys keep the defaults (angle is radians(15), tropism points down)") {
	TreeParams p = treegen::parseTreeParamsJson(R"({"axiom": "A", "rules": []})");
	CHECK(p.name.empty());
	CHECK(p.depth == 0);
	CHECK(p.initialLength == 1.0f);
	CHECK(p.initialRadius == 1.0f);
	CHECK(p.angle == doctest::Approx(0.2617994f));
	CHECK(p.leafGrowthProb == 1.0f);
	CHECK(p.leavesAngleDiversity == 0.0f);
	CHECK(p.tropism == glm::vec3(0, -1, 0));
	CHECK(p.tropismBendingFactor == 0.2f);
	CHECK(p.customParameters.empty());
	CHECK(p.rules.empty());
	CHECK(p.seed == 0);
	CHECK(p.maxModules == 5'000'000);
}

TEST_CASE("json: rules of one symbol keep their declaration order") {
	TreeParams p = treegen::parseTreeParamsJson(R"({"axiom": "A", "rules": [
		{"symbol": "A", "production": "one", "probability": 0.1},
		{"symbol": "B", "production": "other"},
		{"symbol": "A", "production": "two", "probability": 0.2},
		{"symbol": "A", "production": "three", "probability": 0.3}
	]})");
	REQUIRE(p.rules.at('A').size() == 3);
	CHECK(p.rules.at('A')[0].production == "one");
	CHECK(p.rules.at('A')[1].production == "two");
	CHECK(p.rules.at('A')[2].production == "three");
	CHECK(p.rules.at('B').size() == 1);
}

TEST_CASE("json: seed and maxModules are read from the top level and from parameters") {
	TreeParams top = treegen::parseTreeParamsJson(R"({"seed": 4000000000, "maxModules": 1234, "axiom": "A", "rules": []})");
	CHECK(top.seed == 4000000000u);
	CHECK(top.maxModules == 1234);

	TreeParams inner = treegen::parseTreeParamsJson(R"({"parameters": {"seed": 7, "maxModules": 99}, "axiom": "A", "rules": []})");
	CHECK(inner.seed == 7);
	CHECK(inner.maxModules == 99);
	CHECK(inner.customParameters.empty()); // reserved names are not substitution parameters

	TreeParams both = treegen::parseTreeParamsJson(R"({"seed": 1, "parameters": {"seed": 2}, "axiom": "A", "rules": []})");
	CHECK(both.seed == 1); // the top level wins
}

TEST_CASE("json: parameters is optional and a BOM is skipped") {
	TreeParams p = treegen::parseTreeParamsJson("\xEF\xBB\xBF{\"axiom\": \"A\", \"rules\": [{\"symbol\": \"A\", \"production\": \"AA\"}]}");
	CHECK(p.axiom == "A");
	CHECK(onlyRule(p, 'A').production == "AA");
}

// ---- Review Focus 2: bad input is a typed LoadError, never a crash ------------------------------------------

TEST_CASE("Review Focus 2: a missing file throws LoadError naming the path") {
	const std::string path = (kDataDir / "does_not_exist.json").string();
	std::string message = loadErrorOf([&] { treegen::loadTreeParams(path); });
	CHECK(contains(message, path));
	CHECK(contains(message, "cannot open"));

	CHECK_THROWS_AS(treegen::loadTreeParams("no/such/dir/tree.l"), LoadError);
}

TEST_CASE("Review Focus 2: a directory is not a rule file") {
	std::string message = loadErrorOf([&] { treegen::loadTreeParams(kDataDir); });
	CHECK(contains(message, kDataDir.string()));
}

TEST_CASE("Review Focus 2: an empty file throws LoadError with path and reason") {
	const std::string path = (kDataDir / "empty.json").string();
	std::string message = loadErrorOf([&] { treegen::loadTreeParams(path); });
	CHECK(contains(message, path));
	CHECK(contains(message, "empty"));
}

TEST_CASE("Review Focus 2: truncated JSON throws LoadError with path, position and reason") {
	const std::string path = (kDataDir / "truncated.json").string();
	std::string message = loadErrorOf([&] { treegen::loadTreeParams(path); });
	CHECK(contains(message, path));
	CHECK(contains(message, "invalid JSON"));
	CHECK(contains(message, ":4:")); // the file ends on line 4
}

TEST_CASE("Review Focus 2: syntactically valid JSON without rules throws LoadError naming the key") {
	const std::string path = (kDataDir / "missing_rules.json").string();
	std::string message = loadErrorOf([&] { treegen::loadTreeParams(path); });
	CHECK(contains(message, path));
	CHECK(contains(message, "\"rules\""));
}

TEST_CASE("Review Focus 2: garbage and wrong root types") {
	CHECK(contains(jsonErrorOf(""), "empty"));
	CHECK(contains(jsonErrorOf("not json"), "invalid JSON"));
	CHECK(contains(jsonErrorOf("[]"), "root"));
	CHECK(contains(jsonErrorOf("42"), "root"));
	CHECK(contains(jsonErrorOf("{\"axiom\": \"A\", \"rules\": []} trailing"), "invalid JSON"));
}

TEST_CASE("Review Focus 2: an empty object lacks the axiom") {
	std::string message = jsonErrorOf("{}");
	CHECK(contains(message, "probe.json"));
	CHECK(contains(message, "\"axiom\""));
}

TEST_CASE("Review Focus 2: axiom and rules are required and typed") {
	CHECK(contains(jsonErrorOf(R"({"rules": []})"), "\"axiom\""));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A"})"), "\"rules\""));
	CHECK(contains(jsonErrorOf(R"({"axiom": 5, "rules": []})"), "\"axiom\" must be a string"));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": {}})"), "\"rules\" must be an array"));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": "A"})"), "\"rules\" must be an array"));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [], "parameters": []})"), "\"parameters\" must be an object"));
}

TEST_CASE("Review Focus 2: a rule needs an object with symbol and production strings") {
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [1]})"), "rules[0]"));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"production": "AA"}]})"), "\"symbol\""));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": "A"}]})"), "\"production\""));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": 1, "production": "AA"}]})"), "\"symbol\" must be a string"));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": "A", "production": 1}]})"), "\"production\" must be a string"));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": "A", "production": "A"}, {"symbol": "B"}]})"), "rules[1]"));
}

TEST_CASE("Review Focus 2: rule fields of the wrong type") {
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": "A", "production": "A", "probability": "high"}]})"), "\"probability\""));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": "A", "production": "A", "initialDepth": 1.5}]})"), "\"initialDepth\""));
	CHECK(contains(jsonErrorOf(R"({"axiom": "A", "rules": [{"symbol": "A", "production": "A", "initialDepth": "1"}]})"), "\"initialDepth\""));
}

TEST_CASE("Review Focus 2: rule symbols must be one character with an optional parameter list") {
	for (const char* symbol : { "", "AB", "A(", "A(l", "A()", "A(l,)", "(l)", "A(l)x", "A(l)(w)" }) {
		CAPTURE(symbol);
		std::string json = std::string(R"({"axiom": "A", "rules": [{"symbol": ")") + symbol + R"(", "production": "A"}]})";
		std::string message = jsonErrorOf(json);
		CHECK(contains(message, "rules[0]"));
		CHECK(contains(message, "symbol"));
	}
}

TEST_CASE("Review Focus 2: parameter values of the wrong type") {
	CHECK(contains(jsonErrorOf(R"({"parameters": {"angle": "ninety"}, "axiom": "A", "rules": []})"), "\"angle\" must be a number"));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"depth": true}, "axiom": "A", "rules": []})"), "\"depth\" must be a number"));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"lr": null}, "axiom": "A", "rules": []})"), "\"lr\" must be a number"));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"lr": [1]}, "axiom": "A", "rules": []})"), "\"lr\" must be a number"));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"depth": -1}, "axiom": "A", "rules": []})"), "\"depth\""));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"initialLength": 1e999}, "axiom": "A", "rules": []})"), "invalid JSON")); // rapidjson refuses the literal
}

TEST_CASE("Review Focus 2: tropism must be a non-zero vector of three numbers") {
	CHECK(contains(jsonErrorOf(R"({"parameters": {"tropism": 1}, "axiom": "A", "rules": []})"), "\"tropism\""));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"tropism": [0, 1]}, "axiom": "A", "rules": []})"), "\"tropism\""));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"tropism": [0, "x", 0]}, "axiom": "A", "rules": []})"), "\"tropism\""));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"tropism": [0, 0, 0]}, "axiom": "A", "rules": []})"), "\"tropism\""));
}

TEST_CASE("Review Focus 2: seed and maxModules must be non-negative integers") {
	CHECK(contains(jsonErrorOf(R"({"seed": -1, "axiom": "A", "rules": []})"), "\"seed\""));
	CHECK(contains(jsonErrorOf(R"({"seed": 4294967296, "axiom": "A", "rules": []})"), "\"seed\""));
	CHECK(contains(jsonErrorOf(R"({"seed": 1.5, "axiom": "A", "rules": []})"), "\"seed\""));
	CHECK(contains(jsonErrorOf(R"({"seed": "x", "axiom": "A", "rules": []})"), "\"seed\""));
	CHECK(contains(jsonErrorOf(R"({"maxModules": 0, "axiom": "A", "rules": []})"), "\"maxModules\""));
	CHECK(contains(jsonErrorOf(R"({"maxModules": -5, "axiom": "A", "rules": []})"), "\"maxModules\""));
	CHECK(contains(jsonErrorOf(R"({"parameters": {"maxModules": "many"}, "axiom": "A", "rules": []})"), "\"maxModules\""));
}

// ---- legacy .l --------------------------------------------------------------------------------------------

TEST_CASE("legacy l: define lines, comments and rules") {
	const char* text =
		"# a comment\n"
		"define depth 3\n"
		"define initialLength 1.5\n"
		"define initialRadius 0.6\n"
		"define angle 90\n"
		"define leavesAngleDiversity 180\n"
		"define tropismBendingFactor 0.3\n"
		"define lr 1.109\t\t/* elongation rate */\n"
		"\n"
		"B(1)F(lr)A\n"
		"A F(lr)[+A]\n"
		"F(l) F(l*lr) 0.5\n"
		"B{2}(w,x) B(w*lr)\n";
	TreeParams p = treegen::parseTreeParamsLegacy(text, "probe.l");
	CHECK(p.name == "probe.l");
	CHECK(p.depth == 3);
	CHECK(p.initialLength == 1.5f);
	CHECK(p.initialRadius == 0.6f);
	CHECK(p.angle == doctest::Approx(1.5707963f));
	CHECK(p.leavesAngleDiversity == doctest::Approx(3.1415927f));
	CHECK(p.tropismBendingFactor == 0.3f);
	REQUIRE(p.customParameters.size() == 1);
	CHECK(p.customParameters.at("lr") == 1.109f);
	CHECK(p.axiom == "B(1)F(1.109000)A");

	CHECK(onlyRule(p, 'A').production == "F(1.109000)[+A]");
	const treegen::Rule& f = onlyRule(p, 'F');
	CHECK(f.production == "F(l*1.109000)");
	CHECK(f.probability == 0.5f);
	REQUIRE(f.paramNames.size() == 1);
	CHECK(f.paramNames[0] == "l");
	const treegen::Rule& b = onlyRule(p, 'B');
	CHECK(b.initialDepth == 2);
	REQUIRE(b.paramNames.size() == 2);
	CHECK(b.paramNames[1] == "x");
}

TEST_CASE("legacy l: bare-number header lines are depth, length, radius and angle (was KNOWN BUG 11)") {
	const char* text =
		"#depth, initialLength, initialRaidus, angle, axiom, rules\n"
		"4\n0.5\n0.25\n7\n"
		"F\n"
		"F `Y[^M]\n";
	TreeParams p = treegen::parseTreeParamsLegacy(text, "header.l");
	CHECK(p.depth == 4);
	CHECK(p.initialLength == 0.5f);
	CHECK(p.initialRadius == 0.25f);
	CHECK(p.angle == doctest::Approx(0.12217305f)); // 7 degrees
	CHECK(p.axiom == "F");
	CHECK(onlyRule(p, 'F').production == "`Y[^M]");
}

TEST_CASE("legacy l: a header axiom that looks like a call is not mistaken for a number") {
	TreeParams p = treegen::parseTreeParamsLegacy("1\n1.5\n0.6\n15\nf(4.5)ab\na [f(4.5)a]\n");
	CHECK(p.depth == 1);
	CHECK(p.axiom == "f(4.5)ab");
}

TEST_CASE("legacy l: numbers after the axiom are not header lines") {
	TreeParams p = treegen::parseTreeParamsLegacy("A\n5\nA AA\n");
	CHECK(p.axiom == "5"); // the last single-token line is the axiom, as before
	CHECK(p.depth == 0);
}

TEST_CASE("legacy l: the default angle is radians(15) (was KNOWN BUG 12)") {
	TreeParams p = treegen::parseTreeParamsLegacy("A\n");
	CHECK(p.angle == doctest::Approx(0.2617994f));
	CHECK(p.depth == 0); // deterministic default instead of an uninitialized int (KNOWN BUG 11)
}

TEST_CASE("legacy l: define tropism is parsed and normalized (was KNOWN BUG 12)") {
	TreeParams p = treegen::parseTreeParamsLegacy("define tropism 0 0 -4\nA\n");
	CHECK(p.tropism == glm::vec3(0, 0, -1));
	CHECK(contains(legacyErrorOf("define tropism 0 0\nA\n"), "tropism"));
	CHECK(contains(legacyErrorOf("define tropism 0 0 0\nA\n"), "tropism"));
	CHECK(contains(legacyErrorOf("define tropism 0 x 1\nA\n"), "tropism"));
}

TEST_CASE("legacy l: custom parameters are substituted into the axiom and into rules defined earlier (was KNOWN BUG 10)") {
	TreeParams p = treegen::parseTreeParamsLegacy("B(1)F(startLength)+(45)A\nA F(startLength)\ndefine startLength 1.2\n");
	CHECK(p.axiom == "B(1)F(1.200000)+(45)A");
	CHECK(onlyRule(p, 'A').production == "F(1.200000)");
}

TEST_CASE("legacy l: CRLF line ends and a UTF-8 BOM are ignored (was a junk rule named by the BOM)") {
	TreeParams p = treegen::parseTreeParamsLegacy("\xEF\xBB\xBF#depth, initialLength, angle\r\ndefine depth 2\r\nA(10)\r\nA(s) F(s)\r\n");
	CHECK(p.depth == 2);
	CHECK(p.axiom == "A(10)");
	REQUIRE(p.rules.size() == 1);
	CHECK(onlyRule(p, 'A').production == "F(s)");
}

TEST_CASE("legacy l: only the first of several defines on one line counts, tabs and spaces separate tokens") {
	TreeParams p = treegen::parseTreeParamsLegacy("define vr 1.132\t\tdefine vr 1.732\t\t/* width */\ndefine d1  94.74\nA\n");
	CHECK(p.customParameters.at("vr") == 1.132f);
	CHECK(p.customParameters.at("d1") == 94.74f);
}

TEST_CASE("legacy l: seed and maxModules defines") {
	TreeParams p = treegen::parseTreeParamsLegacy("define seed 42\ndefine maxModules 1000\nA\n");
	CHECK(p.seed == 42);
	CHECK(p.maxModules == 1000);
	CHECK(p.customParameters.empty());
}

TEST_CASE("legacy l: errors carry the file and the line") {
	CHECK(contains(legacyErrorOf(""), "axiom"));
	CHECK(contains(legacyErrorOf("# only a comment\n"), "axiom"));
	std::string message = legacyErrorOf("A\ndefine depth\n");
	CHECK(contains(message, "probe.l:2"));
	CHECK(contains(message, "define"));
	CHECK(contains(legacyErrorOf("define depth many\nA\n"), "probe.l:1"));
	CHECK(contains(legacyErrorOf("define depth many\nA\n"), "many"));
	CHECK(contains(legacyErrorOf("A\nA( AA\n"), "probe.l:2"));
	CHECK(contains(legacyErrorOf("A\nA{x} AA\n"), "probe.l:2"));
	CHECK(contains(legacyErrorOf("A\nAB CD\n"), "probe.l:2"));
	CHECK(contains(legacyErrorOf("A\nA AA often\n"), "probability"));
	CHECK(contains(legacyErrorOf("define depth -2\nA\n"), "depth"));
	CHECK(contains(legacyErrorOf("define seed 1.5\nA\n"), "seed"));
	CHECK(contains(legacyErrorOf("define maxModules 0\nA\n"), "maxModules"));
}

TEST_CASE("legacy l: loadTreeParams picks the parser by extension") {
	TreeParams fromL = treegen::loadTreeParams(kRulesDir / "legacy" / "test.l");
	CHECK(fromL.axiom == "A(10)");
	TreeParams fromJson = treegen::loadTreeParams(kRulesDir / "test.json");
	CHECK_FALSE(fromJson.axiom.empty());
}

// ---- the shipped rule files -------------------------------------------------------------------------------

TEST_CASE("Every file in Resources/LindenmayerRules loads through loadTreeParams") {
	size_t loaded = 0;
	for (const fs::path& dir : { kRulesDir, kRulesDir / "legacy" }) {
		for (const auto& entry : fs::directory_iterator(dir)) {
			if (!entry.is_regular_file())
				continue;
			const fs::path path = entry.path();
			CAPTURE(path.string());
			TreeParams p;
			CHECK_NOTHROW(p = treegen::loadTreeParams(path));
			CHECK(p.name == path.string());
			CHECK_FALSE(p.axiom.empty());
			CHECK_FALSE(p.rules.empty());
			CHECK(p.depth >= 0);
			CHECK(std::isfinite(p.angle));
			CHECK(std::abs(glm::length(p.tropism) - 1.0f) < 1e-5f);
			for (const auto& symbolRules : p.rules)
				for (const treegen::Rule& rule : symbolRules.second) {
					CHECK(rule.symbol == symbolRules.first);
					CHECK_FALSE(rule.production.empty());
				}
			++loaded;
		}
	}
	CHECK(loaded == 43); // 21 json + 22 legacy .l
}
