# treegen: the engine-independent tree generator (see docs/architecture/treegen-design.md).
# Allowed dependencies: C++17 standard library, glm, rapidjson. Nothing from Engine/, GraphicEngine/, ESTree/.
#
# The root CMakeLists adds this directory BEFORE its global include_directories()/add_definitions(), and the
# target below additionally resets its include path, so a forbidden include fails to compile (and
# tests/treegen/boundary_check.cmake greps for it as a second line of defence).

set(TREEGEN_GLM_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Vendor/glm" CACHE PATH "Directory containing glm/")
set(TREEGEN_RAPIDJSON_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Vendor/rapidjson/include" CACHE PATH "Directory containing rapidjson/")

file(GLOB_RECURSE TREEGEN_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
file(GLOB_RECURSE TREEGEN_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/include/*.h)

add_library(treegen STATIC ${TREEGEN_SOURCES} ${TREEGEN_HEADERS})
add_library(treegen::treegen ALIAS treegen)

# Drop whatever include directories the parent directory had collected.
set_property(TARGET treegen PROPERTY INCLUDE_DIRECTORIES "")

find_package(Threads REQUIRED)   # buildGeometry computes large meshes with std::thread

target_compile_features(treegen PUBLIC cxx_std_17)
target_link_libraries(treegen PUBLIC Threads::Threads)
target_include_directories(treegen PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(treegen SYSTEM PUBLIC ${TREEGEN_GLM_DIR})
target_include_directories(treegen SYSTEM PRIVATE ${TREEGEN_RAPIDJSON_DIR})

if(MSVC)
	target_compile_options(treegen PRIVATE /W4)
else()
	target_compile_options(treegen PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${TREEGEN_SOURCES} ${TREEGEN_HEADERS})
