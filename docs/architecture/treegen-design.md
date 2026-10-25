# `treegen` — target design and API contract

`treegen` is the engine-independent tree generator. It is the part of ESTree that may someday be
shipped on its own (CLI, plugin core, WASM). This document is the **contract between tasks
T03–T08**: when two tasks run in parallel they code against these names. If a task must change a
signature, it edits this file in the same commit and appends a `docs/DECISIONS.md` entry.

## Boundaries

- Location: `ESEngine/TreeGen/` — headers `include/treegen/*.h`, sources `src/*.cpp`, own `CMakeLists.txt`.
- CMake target: `treegen` (STATIC), alias `treegen::treegen`. Public include dir: `ESEngine/TreeGen/include`.
- Namespace: `treegen`. Consumers write `#include <treegen/Xxx.h>`.
- Allowed dependencies: C++17 standard library, `glm`, `rapidjson` (header-only, via `Vendor/`).
- Forbidden: anything under `Engine/`, `GraphicEngine/`, `ESTree/`, `glad`, `GLFW`, Bullet, FreeType, stb, `boost/`, `<windows.h>`.
- The target's include path contains only `treegen`'s own include dir, `glm`, `rapidjson`, so a forbidden include fails to compile; a CTest (`treegen_boundary`) additionally greps the sources.
- Errors: exceptions derived from `treegen::Error : std::runtime_error` (`LoadError`, `RewriteError`, `InterpretError`). No `exit()`, no `std::cout` in the library; diagnostics go through return values/exceptions.
- Determinism: same `TreeParams` + same `seed` ⇒ bit-identical output on one platform. RNG is `std::mt19937` seeded from the explicit seed; draw order is part of the contract (rewriter draws first, then turtle `~` randomness, then leaf selection — each from its own derived stream, see `Rng`).
- Thread safety: all free functions are reentrant; no hidden statics.

## Pipeline

```
loadTreeParams(path)            → TreeParams
rewrite(params, opts)           → ModuleString
interpret(params, modules, opts)→ Skeleton
buildGeometry(skel, params, opts)→ TreeGeometry { MeshData trunk; vector<glm::mat4> leaves; }
generateTree(params, opts)      → TreeModel   (facade = the three steps above)
```

## Headers and signatures (v1 — T03 creates the first three, later tasks add the rest)

```cpp
// treegen/Error.h  (T03)
struct Error      : std::runtime_error { using std::runtime_error::runtime_error; };
struct LoadError      : Error { using Error::Error; };
struct RewriteError   : Error { using Error::Error; };
struct InterpretError : Error { using Error::Error; };

// treegen/Rng.h  (T03)
class Rng {                       // thin wrapper over std::mt19937
public:
	explicit Rng(uint32_t seed);
	Rng derive(uint32_t streamId) const;     // independent stream, e.g. 1=rewrite, 2=turtle, 3=leaves
	float uniform01();                       // [0,1)
};

// treegen/Module.h  (T03)
struct Module {                   // one symbol + its numeric parameters
	char symbol = 0;
	std::vector<float> params;
	bool randomized = false;      // symbol followed by '~' : first param is an upper bound
};
using ModuleString = std::vector<Module>;
ModuleString parseModules(std::string_view text);   // "F(2)T+(45)A" ; throws RewriteError on malformed text
std::string  toString(const ModuleString&);         // canonical: params via std::to_string(float), joined by ','

// treegen/TreeParams.h  (T03)
struct Rule {
	char symbol;                              // single character
	std::vector<std::string> paramNames;      // formal parameters, e.g. {"l","w"}
	std::string production;                   // production text, custom parameters already substituted
	float probability = 1.0f;
	int   initialDepth = 0;                   // rule applies when iteration index >= initialDepth
};
struct TreeParams {
	std::string name;                         // source path / label
	std::string axiom;
	int   depth = 0;
	float initialLength = 1.0f, initialRadius = 1.0f;
	float angle = glm::radians(15.0f);        // radians
	float leafGrowthProb = 1.0f;
	float leavesAngleDiversity = 0.0f;        // radians
	glm::vec3 tropism{0, -1, 0};              // normalized when loaded from JSON
	float tropismBendingFactor = 0.2f;
	std::map<std::string, float> customParameters;
	std::map<char, std::vector<Rule>> rules;  // declaration order preserved per symbol
	uint32_t seed = 0;                        // optional "seed" key; 0 = caller decides
	std::size_t maxModules = 5'000'000;       // optional "maxModules" key
};
TreeParams loadTreeParams(const std::filesystem::path& file);   // .json or .l ; throws LoadError
TreeParams parseTreeParamsJson(std::string_view json, std::string name = {});   // for tests

// treegen/Rewriter.h  (T04)
struct RewriteOptions { uint32_t seed = 0; };
ModuleString rewrite(const TreeParams&, const RewriteOptions& = {});   // throws RewriteError

// treegen/Skeleton.h + Turtle.h  (T05)
struct SkeletonSegment {
	int       parent;                 // index into Skeleton::segments, -1 for the root
	glm::mat4 model;                  // frame at the segment's END (same as legacy Segment::modelMatrix)
	float     radius;                 // legacy "radius" semantics (a DIAMETER in geometry, see overview.md)
	float     roll;                   // accumulated roll, radians
	int       childCount = 0;
};
struct Skeleton { std::vector<SkeletonSegment> segments; };   // segments[0] is the root (zero-length)
struct InterpretOptions { uint32_t seed = 0; };
Skeleton interpret(const TreeParams&, const ModuleString&, const InterpretOptions& = {});  // throws InterpretError
inline bool isTip(const SkeletonSegment& s) { return s.childCount == 0; }

// treegen/MeshData.h + Geometry.h  (T06)
struct MeshVertex { glm::vec3 position, normal; glm::vec2 uv; };
struct MeshData {
	std::vector<MeshVertex> vertices;
	std::vector<uint32_t>   indices;           // triangle list
};
struct GeometryOptions { int ringDensity = 9; uint32_t seed = 0; };
struct TreeGeometry { MeshData trunk; std::vector<glm::mat4> leafTransforms; };
TreeGeometry buildGeometry(const Skeleton&, const TreeParams&, const GeometryOptions& = {});

// treegen/TreeGenerator.h  (T06)
struct GenerateOptions { uint32_t seed = 0; int ringDensity = 9; };
struct TreeModel { Skeleton skeleton; TreeGeometry geometry; ModuleString modules; };
TreeModel generateTree(const TreeParams&, const GenerateOptions& = {});

// treegen/ObjWriter.h  (T08)
void writeObj(const std::filesystem::path& file, const MeshData& trunk, const std::vector<glm::mat4>& leaves);
```

## Compatibility with the legacy pipeline

- `ModuleString` text form must reproduce the legacy production string for deterministic rules
  (T02 golden tests). Float formatting is `std::to_string(float)` (six decimals), same as legacy.
- Interpreter semantics are the table in `docs/architecture/overview.md`; the only intentional
  differences are listed in `docs/DECISIONS.md` (bug fixes: literal `-1`, leading-minus
  expressions, unbalanced brackets → error).
- Engine adapter (T07) converts `MeshVertex` → engine `Vertex` (color left default) and
  `uint32_t` indices → `int` indices for the existing `Mesh`, and `leafTransforms` →
  `InstancedTransform::modelMatrix` (premultiplied by the tree's world transform, as today).

## Not in v1 (see `BACKLOG.md`)

Context-sensitive rules, conditions (`A(x) : x > 3`), glTF export, LOD, shared-joint smooth meshes, wind data.
