# Fails (non-zero exit) when a source under TREEGEN_DIR includes something treegen must not depend on.
# treegen may use only the C++ standard library, glm and rapidjson (docs/architecture/treegen-design.md).
#
#   cmake -DTREEGEN_DIR=<ESEngine/TreeGen> -P boundary_check.cmake
#
# The target's include path already makes these includes fail to compile; this script is the second,
# build-independent line of defence and reports every offender as <file>:<line>.

if(NOT TREEGEN_DIR)
	message(FATAL_ERROR "boundary_check: pass -DTREEGEN_DIR=<directory to scan>")
endif()
if(NOT IS_DIRECTORY "${TREEGEN_DIR}")
	message(FATAL_ERROR "boundary_check: not a directory: ${TREEGEN_DIR}")
endif()

file(GLOB_RECURSE _sources
	"${TREEGEN_DIR}/*.h" "${TREEGEN_DIR}/*.hpp" "${TREEGEN_DIR}/*.inl"
	"${TREEGEN_DIR}/*.cpp" "${TREEGEN_DIR}/*.cc" "${TREEGEN_DIR}/*.c")
list(LENGTH _sources _source_count)
if(_source_count EQUAL 0)
	message(FATAL_ERROR "boundary_check: no sources found under ${TREEGEN_DIR} (wrong directory?)")
endif()

# An #include directive up to the point where the header name starts.
set(_include "#[ \t]*include[ \t]*[<\"]")
# Characters that can precede the forbidden part inside the header name.
set(_name "[^>\"\n]*")

set(_violations 0)

# Reports every match of <regex> in every scanned file as "<file>:<line>: <reason>".
function(forbid reason regex)
	set(_found 0)
	foreach(_file IN LISTS _sources)
		file(READ "${_file}" _content)
		set(_offset 0)
		while(TRUE)
			string(SUBSTRING "${_content}" ${_offset} -1 _tail)
			string(REGEX MATCH "${regex}" _match "${_tail}")
			if(_match STREQUAL "")
				break()
			endif()
			string(FIND "${_tail}" "${_match}" _position)
			math(EXPR _absolute "${_offset} + ${_position}")
			string(SUBSTRING "${_content}" 0 ${_absolute} _before)
			string(REGEX MATCHALL "\n" _newlines "${_before}")
			list(LENGTH _newlines _count)
			math(EXPR _line "${_count} + 1")
			string(LENGTH "${_match}" _match_length)
			math(EXPR _offset "${_absolute} + ${_match_length}")
			message("${_file}:${_line}: forbidden include (${reason}): ${_match}")
			math(EXPR _found "${_found} + 1")
		endwhile()
	endforeach()
	math(EXPR _total "${_violations} + ${_found}")
	set(_violations ${_total} PARENT_SCOPE)
endfunction()

forbid("engine layer"       "${_include}${_name}Engine/")        # Engine/ and GraphicEngine/
forbid("application layer"  "${_include}${_name}ESTree/")
forbid("glad"               "${_include}${_name}glad")
forbid("GLFW"               "${_include}${_name}[Gg][Ll][Ff][Ww]")
forbid("OpenGL"             "${_include}${_name}(GL/|OpenGL/|gl[.]h)")
forbid("Bullet"             "${_include}${_name}(bt[A-Z][A-Za-z]*[.]h|LinearMath|Bullet)")
forbid("FreeType"           "${_include}${_name}(ft2build|[Ff]ree[Tt]ype)")
forbid("stb"                "${_include}${_name}stb_")
forbid("Boost"              "${_include}${_name}boost/")
forbid("windows.h"          "${_include}${_name}[Ww]indows[.]h")
forbid("no console output in the library" "${_include}iostream[>\"]")

if(_violations GREATER 0)
	message(FATAL_ERROR "boundary_check: ${_violations} forbidden include(s) under ${TREEGEN_DIR}")
endif()
message(STATUS "boundary_check: ${_source_count} files under ${TREEGEN_DIR} include only the standard library, glm and rapidjson")
