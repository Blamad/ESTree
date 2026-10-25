#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <fstream>
#include <string>

TEST_CASE("harness: runs") {
	CHECK(1 + 1 == 2);
}

TEST_CASE("harness: finds repo resources via ESTREE_REPO_ROOT") {
	const std::string path = std::string(ESTREE_REPO_ROOT) + "/Resources/LindenmayerRules/test.json";
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	REQUIRE_MESSAGE(file.is_open(), path);
	CHECK(file.tellg() > 0);
}
