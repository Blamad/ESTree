# These tests compile engine sources and need Bullet.
if(NOT ESTREE_BUILD_ENGINE)
	return()
endif()

# Headless tests of the engine's ownership code (no window, no OpenGL context).
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/ESEngine)

estree_add_test(engine_gameobject_tests
	SOURCES
		gameobject_tests.cpp
		${ENGINE_DIR}/Engine/GameObject/GameObject.cpp
		${ENGINE_DIR}/Engine/GameObject/Transform.cpp
	# GNU ld (unlike MSVC) needs Bullet's out-of-line symbols that GameObject.h's includes reference
	LIBS BulletDynamics BulletCollision LinearMath Threads::Threads)

estree_add_test(engine_input_tests
	SOURCES
		inputstate_tests.cpp
		${ENGINE_DIR}/GraphicEngine/Input/InputState.cpp)

# Deviation from tests/README.md ("do not link Bullet"): the physics ownership code cannot be exercised
# without the Bullet libraries. Still no GLFW/glad/FreeType, no window, no GL context.
estree_add_test(engine_physics_tests
	SOURCES
		physics_tests.cpp
		${ENGINE_DIR}/Engine/GameObject/GameObject.cpp
		${ENGINE_DIR}/Engine/GameObject/Transform.cpp
		${ENGINE_DIR}/Engine/GameObject/RigidBody.cpp
		${ENGINE_DIR}/Engine/Manager/PhysicsManager.cpp
		${ENGINE_DIR}/Engine/Manager/Context.cpp
		${ENGINE_DIR}/Engine/Core/Logger.cpp
	LIBS BulletDynamics BulletCollision LinearMath Threads::Threads)
