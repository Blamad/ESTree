#include <treegen/Rng.h>

namespace treegen {

Rng::Rng(uint32_t seed) : originalSeed(seed), engine(seed) {}

Rng Rng::derive(uint32_t streamId) const {
	std::seed_seq mixer{ originalSeed, streamId };
	uint32_t derived[1];
	mixer.generate(derived, derived + 1);
	return Rng(derived[0]);
}

float Rng::uniform01() {
	// 24 bits fit a float mantissa exactly, so the result is below 1.0f.
	return static_cast<float>(engine() >> 8) * (1.0f / 16777216.0f);
}

}
