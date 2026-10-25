#include <treegen/ObjWriter.h>

#include <array>
#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <system_error>

#include <treegen/Error.h>

namespace treegen {

namespace {

constexpr std::size_t kFlushThreshold = std::size_t(1) << 20;

// Buffered writer: numbers go through std::to_chars (locale independent, shortest round-trip, fixed notation).
class ObjOutput {
public:
	explicit ObjOutput(const std::filesystem::path& file) : path(file) {
		std::error_code ec;
		if (file.has_parent_path()) {
			std::filesystem::create_directories(file.parent_path(), ec);
			if (ec) {
				throw Error(file.string() + ": cannot create directory " + file.parent_path().string() + ": " + ec.message());
			}
		}
		stream.open(file, std::ios::binary | std::ios::trunc);
		if (!stream) {
			throw Error(file.string() + ": cannot open for writing");
		}
		buffer.reserve(kFlushThreshold + 4096);
	}

	void text(const char* s) { buffer += s; }

	void number(float value) {
		if (!std::isfinite(value)) {
			value = 0.0f;
		}
		value += 0.0f;   // -0 + 0 = +0
		char tmp[160];
		std::to_chars_result r = std::to_chars(tmp, tmp + sizeof tmp, value, std::chars_format::fixed);
		buffer.append(tmp, r.ptr);
	}

	void index(std::size_t value) {
		char tmp[24];
		std::to_chars_result r = std::to_chars(tmp, tmp + sizeof tmp, value);
		buffer.append(tmp, r.ptr);
	}

	void vec(const char* tag, float x, float y, float z) {
		text(tag);
		buffer += ' ';
		number(x);
		buffer += ' ';
		number(y);
		buffer += ' ';
		number(z);
		endLine();
	}

	void vec(const char* tag, float x, float y) {
		text(tag);
		buffer += ' ';
		number(x);
		buffer += ' ';
		number(y);
		endLine();
	}

	// "f a/a/a b/b/b c/c/c" with 1-based indices
	void face(std::size_t a, std::size_t b, std::size_t c) {
		buffer += 'f';
		for (std::size_t i : {a, b, c}) {
			buffer += ' ';
			index(i);
			buffer += '/';
			index(i);
			buffer += '/';
			index(i);
		}
		endLine();
	}

	void endLine() {
		buffer += '\n';
		if (buffer.size() >= kFlushThreshold) {
			flush();
		}
	}

	void finish() {
		flush();
		stream.close();
		if (stream.fail()) {
			throw Error(path.string() + ": write failed");
		}
	}

private:
	void flush() {
		stream.write(buffer.data(), static_cast<std::streamsize>(buffer.size()));
		buffer.clear();
		if (!stream) {
			throw Error(path.string() + ": write failed");
		}
	}

	std::filesystem::path path;
	std::ofstream stream;
	std::string buffer;
};

struct LeafCorner {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;
};

// Same corners as LindenmayerTree::createInstancedLeavesPanelMesh: front quad, then back quad.
const std::array<LeafCorner, 8> kLeafPanel = {{
	{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
	{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
	{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
	{{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
	{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
}};

constexpr std::array<std::size_t, 12> kLeafTriangles = {0, 1, 3, 1, 2, 3, 4, 5, 7, 5, 6, 7};

// Inverse-transpose of the upper 3x3 up to a positive factor: the cofactor matrix, sign-corrected for mirrors.
// Unlike the inverse it stays finite for a singular matrix.
glm::mat3 normalMatrix(const glm::mat4& m) {
	glm::vec3 a(m[0]), b(m[1]), c(m[2]);
	glm::mat3 cofactor(glm::cross(b, c), glm::cross(c, a), glm::cross(a, b));
	float det = glm::dot(a, glm::cross(b, c));
	return det < 0.0f ? -cofactor : cofactor;
}

glm::vec3 safeNormalize(const glm::vec3& v) {
	float len = glm::length(v);
	if (!(len > 0.0f) || !std::isfinite(len)) {
		return glm::vec3(0.0f);
	}
	return v / len;
}

}

void writeObj(const std::filesystem::path& file, const MeshData& trunk, const std::vector<glm::mat4>& leaves) {
	if (trunk.indices.size() % 3 != 0) {
		throw Error(file.string() + ": the trunk index count " + std::to_string(trunk.indices.size()) + " is not a multiple of 3");
	}
	for (uint32_t i : trunk.indices) {
		if (i >= trunk.vertices.size()) {
			throw Error(file.string() + ": trunk index " + std::to_string(i) + " is out of range (" +
				std::to_string(trunk.vertices.size()) + " vertices)");
		}
	}

	ObjOutput out(file);
	out.text("# treegen OBJ export\n");

	if (!trunk.vertices.empty()) {
		out.text("o trunk\n");
		for (const MeshVertex& v : trunk.vertices) {
			out.vec("v", v.position.x, v.position.y, v.position.z);
		}
		for (const MeshVertex& v : trunk.vertices) {
			out.vec("vn", v.normal.x, v.normal.y, v.normal.z);
		}
		for (const MeshVertex& v : trunk.vertices) {
			out.vec("vt", v.uv.x, v.uv.y);
		}
		for (std::size_t i = 0; i < trunk.indices.size(); i += 3) {
			out.face(std::size_t(trunk.indices[i]) + 1, std::size_t(trunk.indices[i + 1]) + 1, std::size_t(trunk.indices[i + 2]) + 1);
		}
	}

	if (!leaves.empty()) {
		out.text("o leaves\n");
		for (const glm::mat4& m : leaves) {
			for (const LeafCorner& corner : kLeafPanel) {
				glm::vec3 p(m * glm::vec4(corner.position, 1.0f));
				out.vec("v", p.x, p.y, p.z);
			}
		}
		for (const glm::mat4& m : leaves) {
			glm::mat3 nm = normalMatrix(m);
			for (const LeafCorner& corner : kLeafPanel) {
				glm::vec3 n = safeNormalize(nm * corner.normal);
				out.vec("vn", n.x, n.y, n.z);
			}
		}
		for (std::size_t l = 0; l < leaves.size(); ++l) {
			for (const LeafCorner& corner : kLeafPanel) {
				out.vec("vt", corner.uv.x, corner.uv.y);
			}
		}
		std::size_t base = trunk.vertices.size() + 1;
		for (std::size_t l = 0; l < leaves.size(); ++l, base += kLeafPanel.size()) {
			for (std::size_t t = 0; t < kLeafTriangles.size(); t += 3) {
				out.face(base + kLeafTriangles[t], base + kLeafTriangles[t + 1], base + kLeafTriangles[t + 2]);
			}
		}
	}

	out.finish();
}

}
