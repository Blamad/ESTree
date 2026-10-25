# Headless tests of the engine's ownership code (no window, no OpenGL context).
set(ENGINE_DIR ${PROJECT_SOURCE_DIR}/ESEngine)
set(ENGINE_SYSTEM_LIBS "")
if(WIN32)
	# boost::uuids (GameObject ids) uses the Windows CNG random provider
	set(ENGINE_SYSTEM_LIBS bcrypt)
endif()

estree_add_test(engine_gameobject_tests
	SOURCES
		gameobject_tests.cpp
		${ENGINE_DIR}/Engine/GameObject/GameObject.cpp
		${ENGINE_DIR}/Engine/GameObject/Transform.cpp
	LIBS ${ENGINE_SYSTEM_LIBS})

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
	LIBS BulletDynamics BulletCollision LinearMath ${BOOST_LIBRARIES} ${ENGINE_SYSTEM_LIBS})
