# Proves that boundary_check.cmake really catches violations (a check that never fails proves nothing).
#
#   cmake -DCHECK_SCRIPT=<boundary_check.cmake> -DWORK_DIR=<scratch dir> -P boundary_selftest.cmake

if(NOT CHECK_SCRIPT OR NOT WORK_DIR)
	message(FATAL_ERROR "boundary_selftest: pass -DCHECK_SCRIPT and -DWORK_DIR")
endif()

file(REMOVE_RECURSE "${WORK_DIR}")

set(_failures 0)

# Runs the check on a directory holding one source whose second line is <include_line>.
function(run_case name include_line expect_violation)
	set(_dir "${WORK_DIR}/${name}")
	file(WRITE "${_dir}/src/probe.cpp" "#include <vector>\n${include_line}\nint probe() { return 0; }\n")
	execute_process(
		COMMAND "${CMAKE_COMMAND}" "-DTREEGEN_DIR=${_dir}" -P "${CHECK_SCRIPT}"
		RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
	set(_text "${_out}${_err}")
	set(_ok TRUE)
	if(expect_violation)
		if(_rc EQUAL 0)
			set(_ok FALSE)
			message("FAIL ${name}: '${include_line}' was not reported")
		elseif(NOT _text MATCHES "probe[.]cpp:2: forbidden include")
			set(_ok FALSE)
			message("FAIL ${name}: no <file>:<line> in the report:\n${_text}")
		endif()
	elseif(NOT _rc EQUAL 0)
		set(_ok FALSE)
		message("FAIL ${name}: '${include_line}' is allowed but was reported:\n${_text}")
	endif()
	if(NOT _ok)
		math(EXPR _next "${_failures} + 1")
		set(_failures ${_next} PARENT_SCOPE)
	else()
		message(STATUS "ok   ${name}")
	endif()
endfunction()

# allowed
run_case(allow_std        "#include <string_view>"            FALSE)
run_case(allow_glm        "#include <glm/glm.hpp>"            FALSE)
run_case(allow_rapidjson  "#include <rapidjson/document.h>"   FALSE)
run_case(allow_treegen    "#include <treegen/Module.h>"       FALSE)
run_case(allow_comment    "// Engine/Core/Logger.h is not included here" FALSE)

# forbidden
run_case(engine           "#include \"Engine/Core/Logger.h\""            TRUE)
run_case(graphic_engine   "#include \"GraphicEngine/GLWindow.h\""        TRUE)
run_case(esTree           "#include \"ESTree/Models/Tree/LindenmayerTreeParams.h\"" TRUE)
run_case(glad             "#include <glad/glad.h>"                       TRUE)
run_case(glfw             "#include <GLFW/glfw3.h>"                      TRUE)
run_case(opengl           "#include <GL/gl.h>"                           TRUE)
run_case(bullet           "#include <btBulletDynamicsCommon.h>"          TRUE)
run_case(freetype         "#include <ft2build.h>"                        TRUE)
run_case(stb              "#include \"stb_image.h\""                     TRUE)
run_case(boost            "#include <boost/random.hpp>"                  TRUE)
run_case(windows          "#include <windows.h>"                         TRUE)
run_case(windows_spaced   "#  include   <Windows.h>"                    TRUE)
run_case(iostream         "#include <iostream>"                          TRUE)

# two offenders in one file are both listed with their own line numbers
file(WRITE "${WORK_DIR}/two/probe.cpp" "#include <vector>\n#include <windows.h>\n\n#include <boost/foreach.hpp>\n")
execute_process(COMMAND "${CMAKE_COMMAND}" "-DTREEGEN_DIR=${WORK_DIR}/two" -P "${CHECK_SCRIPT}"
	RESULT_VARIABLE _rc OUTPUT_VARIABLE _out ERROR_VARIABLE _err)
set(_both "${_out}${_err}")
if(_rc EQUAL 0 OR NOT _both MATCHES "probe[.]cpp:2: forbidden include" OR NOT _both MATCHES "probe[.]cpp:4: forbidden include")
	message("FAIL two: expected both probe.cpp:2 and probe.cpp:4:\n${_out}${_err}")
	math(EXPR _failures "${_failures} + 1")
endif()

# an empty directory is an error, not a pass
file(MAKE_DIRECTORY "${WORK_DIR}/empty")
execute_process(COMMAND "${CMAKE_COMMAND}" "-DTREEGEN_DIR=${WORK_DIR}/empty" -P "${CHECK_SCRIPT}" RESULT_VARIABLE _rc OUTPUT_QUIET ERROR_QUIET)
if(_rc EQUAL 0)
	message("FAIL empty: a directory without sources must not pass")
	math(EXPR _failures "${_failures} + 1")
endif()

if(_failures GREATER 0)
	message(FATAL_ERROR "boundary_selftest: ${_failures} case(s) failed")
endif()
message(STATUS "boundary_selftest: all cases behave as expected")
