// Test helper for the treegen_cli CTest entries (no shell scripts, so the tests run the same on Windows and Linux).
//
//   obj_check run <expectedExit> <substring|-> <logFile> -- <exe> [args...]
//       runs the command, stores stdout+stderr in logFile, fails when the exit code differs or the output does not
//       contain the substring ("-" = no output check)
//   obj_check validate <file.obj>   first line, v/vn/vt counts equal and > 0, f lines > 0, every face index in range
//   obj_check same <a> <b>          the files are byte-identical
//   obj_check differ <a> <b>        the files are not byte-identical
//
// Exit code 0 = check passed, 1 = check failed, 2 = bad usage.

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace {

int fail(const std::string& message) {
	std::cerr << "obj_check: FAIL: " << message << "\n";
	return 1;
}

bool readFile(const std::string& path, std::string& out) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return false;
	}
	out.assign(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
	return true;
}

std::string quote(const std::string& s) {
#ifdef _WIN32
	return "\"" + s + "\"";
#else
	std::string r = "'";
	for (char c : s) {
		if (c == '\'') {
			r += "'\\''";
		} else {
			r += c;
		}
	}
	return r + "'";
#endif
}

int runCommand(const std::vector<std::string>& args, const std::string& logFile) {
	std::string cmd;
	for (const std::string& a : args) {
		cmd += quote(a) + " ";
	}
	cmd += "> " + quote(logFile) + " 2>&1";
#ifdef _WIN32
	cmd = "\"" + cmd + "\"";   // cmd.exe /c strips the outer quotes
#endif
	int status = std::system(cmd.c_str());
#ifdef _WIN32
	return status;
#else
	return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
}

int cmdRun(const std::vector<std::string>& a) {
	// a: run <exit> <substring> <log> -- exe args...
	if (a.size() < 6 || a[5 - 1] != "--") {
		return 2;
	}
	int expected = std::atoi(a[1].c_str());
	const std::string& needle = a[2];
	const std::string& log = a[3];
	std::vector<std::string> command(a.begin() + 5, a.end());
	int code = runCommand(command, log);
	std::string output;
	readFile(log, output);
	std::cout << output;
	if (code != expected) {
		return fail("exit code " + std::to_string(code) + ", expected " + std::to_string(expected));
	}
	if (needle != "-" && output.find(needle) == std::string::npos) {
		return fail("output does not contain \"" + needle + "\"");
	}
	return 0;
}

int cmdValidate(const std::string& path) {
	std::ifstream in(path, std::ios::binary);
	if (!in) {
		return fail("cannot open " + path);
	}
	std::string line;
	if (!std::getline(in, line) || line.rfind("# treegen", 0) != 0) {
		return fail("first line is not the treegen header: " + line);
	}
	std::size_t v = 0, vn = 0, vt = 0;
	std::vector<std::string> faces;
	while (std::getline(in, line)) {
		if (line.find("nan") != std::string::npos || line.find("inf") != std::string::npos) {
			return fail("non-finite number: " + line);
		}
		if (line.rfind("v ", 0) == 0) {
			++v;
		} else if (line.rfind("vn ", 0) == 0) {
			++vn;
		} else if (line.rfind("vt ", 0) == 0) {
			++vt;
		} else if (line.rfind("f ", 0) == 0) {
			faces.push_back(line);
		}
	}
	if (v == 0 || faces.empty()) {
		return fail("no vertices or no faces");
	}
	if (v != vn || v != vt) {
		return fail("v/vn/vt counts differ: " + std::to_string(v) + "/" + std::to_string(vn) + "/" + std::to_string(vt));
	}
	for (const std::string& f : faces) {
		std::istringstream ls(f.substr(2));
		std::string token;
		int corners = 0;
		while (ls >> token) {
			++corners;
			std::istringstream ts(token);
			for (int part = 0; part < 3; ++part) {
				long idx = 0;
				char slash = 0;
				if (!(ts >> idx) || idx < 1 || static_cast<std::size_t>(idx) > v) {
					return fail("face index out of range in: " + f);
				}
				if (part < 2 && (!(ts >> slash) || slash != '/')) {
					return fail("malformed face: " + f);
				}
			}
		}
		if (corners != 3) {
			return fail("face is not a triangle: " + f);
		}
	}
	std::cout << "obj_check: " << path << ": " << v << " vertices, " << faces.size() << " faces\n";
	return 0;
}

int cmdCompare(const std::string& a, const std::string& b, bool expectSame) {
	std::string da, db;
	if (!readFile(a, da) || !readFile(b, db)) {
		return fail("cannot read " + a + " or " + b);
	}
	if (da.empty() || db.empty()) {
		return fail("an input file is empty");
	}
	bool same = da == db;
	if (same != expectSame) {
		return fail(std::string("files are ") + (same ? "identical" : "different") + ": " + a + " " + b);
	}
	return 0;
}

}

int main(int argc, char** argv) {
	std::vector<std::string> a(argv + 1, argv + argc);
	int code = 2;
	if (!a.empty() && a[0] == "run") {
		code = cmdRun(a);
	} else if (a.size() == 2 && a[0] == "validate") {
		code = cmdValidate(a[1]);
	} else if (a.size() == 3 && a[0] == "same") {
		code = cmdCompare(a[1], a[2], true);
	} else if (a.size() == 3 && a[0] == "differ") {
		code = cmdCompare(a[1], a[2], false);
	}
	if (code == 2) {
		std::cerr << "usage: obj_check run <exit> <substring|-> <log> -- <exe> [args...] | validate <obj> | same <a> <b> | differ <a> <b>\n";
	}
	return code;
}
