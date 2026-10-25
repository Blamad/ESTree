#ifndef ENGINE_UUID_H
#define ENGINE_UUID_H

#include <cstdint>
#include <random>
#include <string>

struct Uuid {
	uint64_t hi = 0;
	uint64_t lo = 0;

	bool operator==(const Uuid& other) const { return hi == other.hi && lo == other.lo; }
	bool operator<(const Uuid& other) const { return hi != other.hi ? hi < other.hi : lo < other.lo; }

	// Random (version 4 style) identifier.
	static Uuid generate() {
		static std::mt19937_64 engine{ std::random_device{}() };
		Uuid id;
		id.hi = (engine() & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
		id.lo = (engine() & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
		return id;
	}
};

inline std::string uuidToString(const Uuid& id) {
	static const char digits[] = "0123456789abcdef";
	std::string hex(32, '0');
	for (int i = 0; i < 16; i++) {
		hex[i] = digits[(id.hi >> (60 - 4 * i)) & 0xF];
		hex[16 + i] = digits[(id.lo >> (60 - 4 * i)) & 0xF];
	}
	return hex.substr(0, 8) + "-" + hex.substr(8, 4) + "-" + hex.substr(12, 4) + "-" + hex.substr(16, 4) + "-" + hex.substr(20);
}

#endif
