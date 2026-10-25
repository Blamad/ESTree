#pragma once

#include <cstdint>
#include <random>

namespace treegen {

// Seeded random stream. mt19937 and seed_seq are fully specified by the standard and uniform01 does not use
// std::uniform_real_distribution (whose results differ between standard libraries), so one seed gives the
// same numbers on every platform.
class Rng {
public:
	explicit Rng(uint32_t seed);

	// Independent stream for a pipeline stage (1 = rewrite, 2 = turtle, 3 = leaves). Depends only on the
	// seed and the id, not on how many numbers were drawn from this stream.
	Rng derive(uint32_t streamId) const;

	float uniform01(); // [0, 1)

private:
	uint32_t originalSeed;
	std::mt19937 engine;
};

}
