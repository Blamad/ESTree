#include <doctest/doctest.h>

#include <clocale>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>

#include <treegen/Error.h>
#include <treegen/ObjWriter.h>

namespace fs = std::filesystem;

namespace {

fs::path scratchDir(const std::string& name) {
	fs::path dir = fs::temp_directory_path() / "treegen_obj_tests" / name;
	std::error_code ec;
	fs::remove_all(dir, ec);
	return dir;
}

std::string slurp(const fs::path& file) {
	std::ifstream in(file, std::ios::binary);
	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

treegen::MeshData triangle() {
	treegen::MeshData mesh;
	mesh.vertices = {
		{{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, 0.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
		{{0.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.25f, 1.0f}},
	};
	mesh.indices = {0, 1, 2};
	return mesh;
}

const char* const kTrunkText =
	"o trunk\n"
	"v 0 0 0\n"
	"v 1 0 0.5\n"
	"v 0 2 0\n"
	"vn 0 1 0\n"
	"vn 0 1 0\n"
	"vn 0 0 1\n"
	"vt 0 0\n"
	"vt 1 0\n"
	"vt 0.25 1\n"
	"f 1/1/1 2/2/2 3/3/3\n";

std::vector<std::string> linesStartingWith(const std::string& text, const std::string& prefix) {
	std::vector<std::string> result;
	std::istringstream in(text);
	for (std::string line; std::getline(in, line);) {
		if (line.compare(0, prefix.size(), prefix) == 0) {
			result.push_back(line);
		}
	}
	return result;
}

struct CommaDecimal : std::numpunct<char> {
	char do_decimal_point() const override { return ','; }
	char do_thousands_sep() const override { return '.'; }
	std::string do_grouping() const override { return "\3"; }
};

}

TEST_CASE("ObjWriter: trunk and one translated leaf give the exact expected text") {
	fs::path file = scratchDir("exact") / "tree.obj";
	treegen::writeObj(file, triangle(), {glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3))});

	std::string expected = std::string("# treegen OBJ export\n") + kTrunkText +
		"o leaves\n"
		"v 2 1 3\n"
		"v 2 3 3\n"
		"v 0 3 3\n"
		"v 0 1 3\n"
		"v 0 1 3\n"
		"v 0 3 3\n"
		"v 2 3 3\n"
		"v 2 1 3\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 1\n"
		"vn 0 0 -1\n"
		"vn 0 0 -1\n"
		"vn 0 0 -1\n"
		"vn 0 0 -1\n"
		"vt 1 1\n"
		"vt 1 0\n"
		"vt 0 0\n"
		"vt 0 1\n"
		"vt 0 1\n"
		"vt 0 0\n"
		"vt 1 0\n"
		"vt 1 1\n"
		"f 4/4/4 5/5/5 7/7/7\n"
		"f 5/5/5 6/6/6 7/7/7\n"
		"f 8/8/8 9/9/9 11/11/11\n"
		"f 9/9/9 10/10/10 11/11/11\n";
	CHECK(slurp(file) == expected);
}

TEST_CASE("ObjWriter: indices are 1-based and continue across leaves") {
	fs::path file = scratchDir("base") / "tree.obj";
	std::vector<glm::mat4> leaves(3, glm::mat4(1.0f));
	treegen::writeObj(file, triangle(), leaves);
	std::string text = slurp(file);

	CHECK(linesStartingWith(text, "v ").size() == 3 + 3 * 8);
	CHECK(linesStartingWith(text, "vn ").size() == 3 + 3 * 8);
	CHECK(linesStartingWith(text, "vt ").size() == 3 + 3 * 8);
	std::vector<std::string> faces = linesStartingWith(text, "f ");
	REQUIRE(faces.size() == 1 + 3 * 4);
	CHECK(faces.front() == "f 1/1/1 2/2/2 3/3/3");
	CHECK(faces[1] == "f 4/4/4 5/5/5 7/7/7");
	CHECK(faces[5] == "f 12/12/12 13/13/13 15/15/15");   // the second leaf starts after 3 + 8 vertices
	CHECK(faces.back() == "f 25/25/25 26/26/26 27/27/27");
}

TEST_CASE("ObjWriter: trunk only and leaves only") {
	fs::path dir = scratchDir("parts");
	treegen::writeObj(dir / "trunk.obj", triangle(), {});
	CHECK(slurp(dir / "trunk.obj") == std::string("# treegen OBJ export\n") + kTrunkText);

	treegen::writeObj(dir / "leaves.obj", treegen::MeshData{}, {glm::mat4(1.0f)});
	std::string text = slurp(dir / "leaves.obj");
	CHECK(text.find("o trunk") == std::string::npos);
	CHECK(text.find("o leaves\n") != std::string::npos);
	CHECK(linesStartingWith(text, "f ").front() == "f 1/1/1 2/2/2 4/4/4");

	treegen::writeObj(dir / "empty.obj", treegen::MeshData{}, {});
	CHECK(slurp(dir / "empty.obj") == "# treegen OBJ export\n");
}

TEST_CASE("ObjWriter: leaf normals follow the normal matrix") {
	fs::path file = scratchDir("normals") / "tree.obj";
	glm::mat4 scaled = glm::scale(glm::mat4(1.0f), glm::vec3(2, 3, 4));
	glm::mat4 mirrored = glm::scale(glm::mat4(1.0f), glm::vec3(-1, 1, 1));
	glm::mat4 degenerate(0.0f);
	treegen::writeObj(file, treegen::MeshData{}, {scaled, mirrored, degenerate});
	std::string text = slurp(file);
	std::vector<std::string> normals = linesStartingWith(text, "vn ");
	REQUIRE(normals.size() == 24);
	CHECK(normals[0] == "vn 0 0 1");    // non-uniform scale keeps +Z unit length
	CHECK(normals[4] == "vn 0 0 -1");
	CHECK(normals[8] == "vn 0 0 1");    // a mirror does not flip the facing
	CHECK(normals[12] == "vn 0 0 -1");
	CHECK(normals[16] == "vn 0 0 0");   // a singular matrix gives finite numbers, not NaN
	std::vector<std::string> positions = linesStartingWith(text, "v ");
	CHECK(positions[0] == "v 2 -3 0");
	CHECK(positions[8] == "v -1 -1 0");
}

TEST_CASE("ObjWriter: decimal point is a dot whatever the locale") {
	treegen::MeshData mesh;
	mesh.vertices = {{{1.5f, -0.25f, 1234567.0f}, {0.0f, 1.0f, 0.0f}, {0.125f, 0.0f}},
	                 {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	                 {{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}};
	mesh.indices = {0, 1, 2};

	fs::path dir = scratchDir("locale");
	treegen::writeObj(dir / "classic.obj", mesh, {});

	std::locale previous = std::locale::global(std::locale(std::locale::classic(), new CommaDecimal));
	treegen::writeObj(dir / "cpp_locale.obj", mesh, {});
	std::locale::global(previous);
	CHECK(slurp(dir / "cpp_locale.obj") == slurp(dir / "classic.obj"));

	std::string oldC = std::setlocale(LC_ALL, nullptr);
	const char* candidates[] = {"de_DE.UTF-8", "de_DE.utf8", "de_DE", "German_Germany.1252", "German"};
	bool switched = false;
	for (const char* name : candidates) {
		if (std::setlocale(LC_ALL, name) != nullptr) {
			switched = true;
			break;
		}
	}
	if (switched) {
		treegen::writeObj(dir / "c_locale.obj", mesh, {});
		std::setlocale(LC_ALL, oldC.c_str());
		CHECK(slurp(dir / "c_locale.obj") == slurp(dir / "classic.obj"));
	} else {
		MESSAGE("skipped the C-locale half: no German locale installed");
	}
	std::string text = slurp(dir / "classic.obj");
	CHECK(text.find("v 1.5 -0.25 1234567\n") != std::string::npos);
	CHECK(text.find("vt 0.125 0\n") != std::string::npos);
	CHECK(text.find(',') == std::string::npos);
}

TEST_CASE("ObjWriter: negative zero and non-finite values are written as 0") {
	treegen::MeshData mesh;
	mesh.vertices = {{{-0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	                 {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	                 {{0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}}};
	mesh.vertices[1].position.x = std::numeric_limits<float>::quiet_NaN();
	mesh.vertices[2].position.y = std::numeric_limits<float>::infinity();
	mesh.indices = {0, 1, 2};
	fs::path file = scratchDir("special") / "tree.obj";
	treegen::writeObj(file, mesh, {});
	std::vector<std::string> positions = linesStartingWith(slurp(file), "v ");
	CHECK(positions == std::vector<std::string>{"v 0 0 0", "v 0 0 0", "v 0 0 0"});
}

TEST_CASE("ObjWriter: creates missing parent directories and overwrites") {
	fs::path file = scratchDir("parents") / "a" / "b" / "tree.obj";
	treegen::writeObj(file, triangle(), {});
	CHECK(fs::exists(file));
	treegen::writeObj(file, treegen::MeshData{}, {});
	CHECK(slurp(file) == "# treegen OBJ export\n");   // truncated, not appended
}

TEST_CASE("ObjWriter: output is deterministic") {
	fs::path dir = scratchDir("determinism");
	std::vector<glm::mat4> leaves = {glm::rotate(glm::mat4(1.0f), 0.3f, glm::vec3(0, 1, 0)), glm::mat4(1.0f)};
	treegen::writeObj(dir / "a.obj", triangle(), leaves);
	treegen::writeObj(dir / "b.obj", triangle(), leaves);
	CHECK(slurp(dir / "a.obj") == slurp(dir / "b.obj"));
}

TEST_CASE("ObjWriter: I/O failures throw treegen Error") {
	fs::path dir = scratchDir("failure");
	fs::create_directories(dir);
	{
		std::ofstream blocker(dir / "file.txt");
		blocker << "x";
	}
	// the parent "directory" is a regular file
	CHECK_THROWS_AS(treegen::writeObj(dir / "file.txt" / "sub" / "tree.obj", triangle(), {}), treegen::Error);
	// the target itself is a directory
	CHECK_THROWS_AS(treegen::writeObj(dir, triangle(), {}), treegen::Error);
	CHECK_THROWS_AS(treegen::writeObj(fs::path(), triangle(), {}), treegen::Error);
}

TEST_CASE("ObjWriter: an out-of-range trunk index throws instead of writing a broken file") {
	treegen::MeshData mesh = triangle();
	mesh.indices = {0, 1, 3};
	fs::path file = scratchDir("badindex") / "tree.obj";
	CHECK_THROWS_AS(treegen::writeObj(file, mesh, {}), treegen::Error);
}
