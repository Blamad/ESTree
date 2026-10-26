// treegen_cli: generates a tree from a Lindenmayer rule file and writes it as a Wavefront OBJ file.
// Links only the treegen library: no engine, no OpenGL, no Boost.

#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>

#include <treegen/ObjWriter.h>
#include <treegen/TreeGenerator.h>

namespace {

constexpr int kExitOk = 0;
constexpr int kExitUsage = 2;
constexpr int kExitLoad = 3;
constexpr int kExitGenerate = 4;

const char* const kUsage =
	"usage: treegen_cli <rules.json|rules.l> --out <tree.obj> [--seed N] [--depth D] [--ring R] [--threads T]\n"
	"\n"
	"  <rules>       Lindenmayer rule file (.json, or .l for the legacy format)\n"
	"  --out FILE    OBJ file to write (parent directories are created)\n"
	"  --seed N      random seed, 0..4294967295 (default 0 = the \"seed\" key of the rule file, if any)\n"
	"  --depth D     override the number of rewriting iterations of the rule file (D >= 0)\n"
	"  --ring R      vertices per trunk ring including the seam vertex, R >= 3 (default 9)\n"
	"  --threads T   geometry worker threads, 0 = automatic (default), 1 = none\n"
	"  --help        print this text and exit\n"
	"\n"
	"exit codes: 0 ok, 2 usage error, 3 rule file cannot be loaded, 4 generation or write error\n";

struct Arguments {
	std::string rulesFile;
	std::string outFile;
	uint32_t seed = 0;
	bool hasDepth = false;
	int depth = 0;
	int ring = 9;
	unsigned threads = 0;
	bool help = false;
};

template <typename T>
bool parseNumber(std::string_view text, T& value) {
	if (text.empty()) {
		return false;
	}
	T parsed{};
	std::from_chars_result r = std::from_chars(text.data(), text.data() + text.size(), parsed);
	if (r.ec != std::errc() || r.ptr != text.data() + text.size()) {
		return false;
	}
	value = parsed;
	return true;
}

// Returns an empty string on success, otherwise the complaint.
std::string parseArguments(int argc, char** argv, Arguments& args) {
	for (int i = 1; i < argc; ++i) {
		std::string_view arg = argv[i];
		if (arg == "--help" || arg == "-h") {
			args.help = true;
			return {};
		}
		if (arg.substr(0, 2) != "--") {
			if (!args.rulesFile.empty()) {
				return "unexpected argument '" + std::string(arg) + "'";
			}
			args.rulesFile = std::string(arg);
			continue;
		}
		std::string_view name = arg;
		std::string_view value;
		bool hasValue = false;
		std::size_t eq = arg.find('=');
		if (eq != std::string_view::npos) {
			name = arg.substr(0, eq);
			value = arg.substr(eq + 1);
			hasValue = true;
		}
		if (name != "--seed" && name != "--depth" && name != "--ring" && name != "--threads" && name != "--out") {
			return "unknown option '" + std::string(name) + "'";
		}
		if (!hasValue) {
			if (i + 1 >= argc) {
				return "option '" + std::string(name) + "' needs a value";
			}
			value = argv[++i];
		}
		bool ok = true;
		if (name == "--out") {
			args.outFile = std::string(value);
			ok = !value.empty();
		} else if (name == "--seed") {
			ok = parseNumber(value, args.seed);
		} else if (name == "--depth") {
			ok = parseNumber(value, args.depth) && args.depth >= 0;
			args.hasDepth = true;
		} else if (name == "--ring") {
			ok = parseNumber(value, args.ring) && args.ring >= 3;
		} else {
			ok = parseNumber(value, args.threads);
		}
		if (!ok) {
			return "invalid value '" + std::string(value) + "' for " + std::string(name);
		}
	}
	if (args.rulesFile.empty()) {
		return "missing rule file";
	}
	if (args.outFile.empty()) {
		return "missing --out";
	}
	return {};
}

}

int main(int argc, char** argv) {
	Arguments args;
	std::string complaint = parseArguments(argc, argv, args);
	if (args.help) {
		std::cout << kUsage;
		return kExitOk;
	}
	if (!complaint.empty()) {
		std::cerr << "treegen_cli: " << complaint << "\n\n" << kUsage;
		return kExitUsage;
	}

	auto start = std::chrono::steady_clock::now();

	treegen::TreeParams params;
	try {
		params = treegen::loadTreeParams(args.rulesFile);
	} catch (const std::exception& e) {
		std::cerr << "treegen_cli: cannot load rule file '" << args.rulesFile << "': " << e.what() << "\n";
		return kExitLoad;
	}
	if (args.hasDepth) {
		params.depth = args.depth;
	}

	treegen::GenerateOptions options;
	options.seed = args.seed;
	options.ringDensity = args.ring;
	options.threads = args.threads;

	treegen::TreeModel model;
	try {
		model = treegen::generateTree(params, options);
		treegen::writeObj(args.outFile, model.geometry.trunk, model.geometry.leafTransforms);
	} catch (const std::exception& e) {
		std::cerr << "treegen_cli: generation of '" << args.rulesFile << "' failed: " << e.what() << "\n";
		return kExitGenerate;
	}

	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "segments=" << model.geometry.stats.segments
	          << " vertices=" << model.geometry.trunk.vertices.size()
	          << " triangles=" << model.geometry.trunk.indices.size() / 3
	          << " leaves=" << model.geometry.leafTransforms.size()
	          << " seed=" << (args.seed != 0 ? args.seed : params.seed)
	          << " elapsed_ms=" << elapsed.count()
	          << " out=" << args.outFile << "\n";
	return kExitOk;
}
