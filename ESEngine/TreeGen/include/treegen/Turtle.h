#pragma once

#include <cstdint>

#include <treegen/Module.h>
#include <treegen/Skeleton.h>
#include <treegen/TreeParams.h>

namespace treegen {

struct InterpretOptions {
	uint32_t seed = 0;   // drives the `X~(v)` modules, from Rng(seed).derive(2)
};

// Runs the turtle over `modules` (alphabet: F f [ ] ^ v < > + - ; ` = $ T, see docs/architecture/overview.md).
// A module's first parameter is its argument; no parameter means the documented default. Unknown symbols are
// ignored. Throws InterpretError when a ']' has no matching '[' (the message names the module index); an unmatched
// '[' at the end of the string is allowed.
Skeleton interpret(const TreeParams& params, const ModuleString& modules, const InterpretOptions& options = {});

}
