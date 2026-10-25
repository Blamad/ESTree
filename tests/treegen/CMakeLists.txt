# One executable for every treegen unit test: later tasks only add tests/treegen/<area>_tests.cpp.
file(GLOB TREEGEN_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*_tests.cpp)
estree_add_test(treegen_tests SOURCES main.cpp ${TREEGEN_TEST_SOURCES} LIBS treegen)

# Architecture boundary: treegen may include only the standard library, glm and rapidjson.
add_test(NAME treegen_boundary
	COMMAND ${CMAKE_COMMAND}
		-DTREEGEN_DIR=${PROJECT_SOURCE_DIR}/ESEngine/TreeGen
		-P ${CMAKE_CURRENT_SOURCE_DIR}/boundary_check.cmake)
# Negative self-test: the check above must fail on planted violations.
add_test(NAME treegen_boundary_selftest
	COMMAND ${CMAKE_COMMAND}
		-DCHECK_SCRIPT=${CMAKE_CURRENT_SOURCE_DIR}/boundary_check.cmake
		-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/boundary_selftest
		-P ${CMAKE_CURRENT_SOURCE_DIR}/boundary_selftest.cmake)
