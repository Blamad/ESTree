include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
FetchContent_Declare(doctest
	GIT_REPOSITORY https://github.com/doctest/doctest.git
	GIT_TAG v2.4.11
	SYSTEM)
FetchContent_MakeAvailable(doctest)
include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)

# estree_add_test(<name> SOURCES <files...> [LIBS <targets...>])
function(estree_add_test name)
	cmake_parse_arguments(ARG "" "" "SOURCES;LIBS" ${ARGN})
	add_executable(${name} ${ARG_SOURCES})
	target_link_libraries(${name} PRIVATE doctest::doctest ${ARG_LIBS})
	target_compile_definitions(${name} PRIVATE ESTREE_REPO_ROOT="${PROJECT_SOURCE_DIR}")
	if(COMMAND estree_asan_runtime) # ESTREE_SANITIZE on MSVC; must precede test discovery, which runs the exe
		estree_asan_runtime(${name})
	endif()
	doctest_discover_tests(${name} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()

estree_add_test(smoke_tests SOURCES smoke/smoke_tests.cpp)
add_subdirectory(engine)

# T02: golden tests of the unchanged legacy params loader + solver (Boost headers only, no engine/GL code)
estree_add_test(legacy_characterization_tests
	SOURCES
		legacy_characterization/legacy_characterization_tests.cpp
		${PROJECT_SOURCE_DIR}/ESEngine/ESTree/Models/Tree/LindenmayerTreeParams.cpp
		${PROJECT_SOURCE_DIR}/ESEngine/ESTree/Models/Tree/LindenmayerTreeSolver.cpp)

