#include <treegen/Turtle.h>

#include <cmath>
#include <string>
#include <utility>
#include <vector>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <treegen/Error.h>
#include <treegen/Rng.h>

namespace treegen {

namespace {

constexpr float kDefaultScale = 0.625f;   // default parameter of ; ` =

// Same constant and operation order as the legacy interpreter, so equivalence tests can use tight tolerances.
float toRadians(float degrees) {
	return degrees * 3.14159265359f / 180.f;
}

const glm::quat kIdentity(1.0f, 0.0f, 0.0f, 0.0f);

struct TurtleState {
	glm::quat rotation = kIdentity;        // since the last F, relative to the parent segment's frame
	glm::quat totalRotation = kIdentity;   // product of all rotations of the segments so far
	float length = 0.0f;
	float radius = 0.0f;
	float angle = 0.0f;
	float lengthScale = 1.0f;
	float roll = 0.0f;
};

// The turtle's heading (image of +Y) of a unit quaternion.
glm::vec3 frontVector(const glm::quat& q) {
	return glm::vec3(2 * (q.x * q.y - q.w * q.z), 1 - 2 * (q.x * q.x + q.z * q.z), 2 * (q.y * q.z + q.w * q.x));
}

// Rotates the heading towards `tropismDir`. The axis is deliberately not normalised: that is what the legacy
// interpreter did, and it makes the effective bend angle ~sin(theta) smaller than the nominal one.
glm::quat applyTropism(const TurtleState& state, const glm::vec3& tropismDir, float bendingFactor) {
	const glm::vec3 yAxis(0, 1, 0);
	glm::vec3 front = frontVector(state.totalRotation * state.rotation);
	glm::vec3 crossVec = glm::cross(front, tropismDir);
	float angleVal = std::acos(glm::clamp(glm::dot(front, tropismDir), -1.0f, 1.0f));
	angleVal *= std::sin(angleVal) * bendingFactor;

	glm::quat transformQuat = glm::angleAxis(-state.roll, yAxis) * glm::angleAxis(angleVal, crossVec) * glm::angleAxis(state.roll, yAxis);
	return state.rotation * transformQuat;
}

// Legacy `$`: builds a matrix whose rows are (left, front, up) from the heading and the downwards world axis and
// converts it to a quaternion. When the heading is parallel to that axis (a freshly reset turtle points straight up)
// the legacy code normalised a zero vector and produced NaN; here the rotation is left unchanged instead.
glm::quat restoreHorizontalOrientation(const glm::quat& rotation) {
	const glm::vec3 worldUp(0, -1, 0);
	glm::vec3 front = frontVector(rotation);
	glm::vec3 leftRaw = glm::cross(front, worldUp);
	if (glm::dot(leftRaw, leftRaw) < 1e-12f)
		return rotation;
	glm::vec3 left = glm::normalize(leftRaw);
	glm::vec3 up = glm::normalize(glm::cross(front, left));

	glm::mat3 basis(
		left.x, front.x, up.x,
		left.y, front.y, up.y,
		left.z, front.z, up.z);
	return glm::quat_cast(basis);
}

}

Skeleton interpret(const TreeParams& params, const ModuleString& modules, const InterpretOptions& options) {
	Skeleton skeleton;

	std::size_t drawing = 0;
	for (const Module& module : modules)
		if (module.symbol == 'F' || module.symbol == 'f')
			++drawing;
	skeleton.segments.reserve(drawing + 1);

	skeleton.segments.push_back(SkeletonSegment{ -1, glm::mat4(1.0f), params.initialRadius, 0.0f, 0 });
	int current = 0;

	TurtleState state;
	state.length = params.initialLength;
	state.radius = params.initialRadius;
	state.angle = params.angle;

	Rng rng = Rng(options.seed).derive(2);
	std::vector<std::pair<int, TurtleState>> stack;

	// The module's first parameter, or `fallback` when it has none. `X~(v)` draws uniform01() * v.
	auto argument = [&rng](const Module& module, float fallback) {
		if (module.params.empty())
			return fallback;
		float value = module.params[0];
		if (module.randomized)
			value = rng.uniform01() * value;
		return value;
	};
	// Rotation arguments are degrees; the default is the (already scaled) turtle angle in radians.
	auto angleArgument = [&](const Module& module) {
		return module.params.empty() ? state.angle : toRadians(argument(module, 0.0f));
	};

	const glm::vec3 xAxis(1, 0, 0), yAxis(0, 1, 0), zAxis(0, 0, 1);

	for (std::size_t i = 0; i < modules.size(); ++i) {
		const Module& module = modules[i];
		switch (module.symbol) {
		case '[':
			stack.emplace_back(current, state);
			break;
		case ']':
			if (stack.empty())
				throw InterpretError("interpret: ']' without matching '[' at module " + std::to_string(i));
			current = stack.back().first;
			state = stack.back().second;
			stack.pop_back();
			break;
		case '^': // pitch about X, +
			state.rotation *= glm::angleAxis(angleArgument(module), xAxis);
			break;
		case 'v': // pitch about X, -
			state.rotation *= glm::angleAxis(-angleArgument(module), xAxis);
			break;
		case '<': // yaw about Z, +
			state.rotation *= glm::angleAxis(angleArgument(module), zAxis);
			break;
		case '>': // yaw about Z, -
			state.rotation *= glm::angleAxis(-angleArgument(module), zAxis);
			break;
		case '+': { // roll about Y, +
			float value = angleArgument(module);
			state.roll += value;
			state.rotation *= glm::angleAxis(value, yAxis);
			break;
		}
		case '-': { // roll about Y, -
			float value = angleArgument(module);
			state.roll -= value;
			state.rotation *= glm::angleAxis(-value, yAxis);
			break;
		}
		case ';': // scale the angle
			state.angle *= argument(module, kDefaultScale);
			break;
		case '`': { // scale length and radius
			float value = argument(module, kDefaultScale);
			state.lengthScale *= value;
			state.radius *= value;
			break;
		}
		case '=': // scale the radius only
			state.radius *= argument(module, kDefaultScale);
			break;
		case '$':
			state.rotation = restoreHorizontalOrientation(state.rotation);
			state.roll = glm::roll(state.rotation);
			break;
		case 'T':
			state.rotation = applyTropism(state, params.tropism, argument(module, params.tropismBendingFactor));
			break;
		case 'f':
		case 'F': {
			state.length = argument(module, state.length) * state.lengthScale;

			// The frame uses the raw local rotation like the legacy code: `$` produces quaternions of norm < 1 whose
			// mat4_cast the shipped trees were built with, normalising here would change them.
			const glm::quat& rotation = state.rotation;
			glm::mat4 frame = glm::translate(glm::mat4_cast(rotation), glm::vec3(0, state.length, 0));

			SkeletonSegment segment{ current, skeleton.segments[current].model * frame, state.radius, state.roll, 0 };
			skeleton.segments[current].childCount++;
			skeleton.segments.push_back(segment);
			current = static_cast<int>(skeleton.segments.size()) - 1;

			TurtleState next = state;
			next.rotation = kIdentity;
			// Only the accumulated rotation is renormalised (the legacy code never did): it is a product over the whole
			// path and the heading used by `T` is read from it.
			next.totalRotation = glm::normalize(state.totalRotation * rotation);
			next.length = params.initialLength;
			state = next;
			break;
		}
		default: // unknown symbols (rule variables such as A, B) do nothing
			break;
		}
	}

	return skeleton;
}

}
