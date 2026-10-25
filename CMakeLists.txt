cmake_minimum_required(VERSION 3.21)
project(ESTree)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ESTREE_SANITIZE "Instrument ESTree and its tests with AddressSanitizer (plus UBSan on GCC/Clang)" OFF)
if(ESTREE_SANITIZE)
	if(MSVC)
		# Applies to every target defined below, the vendored libraries (Bullet, GLFW, FreeType) included, so that
		# ASan also sees accesses made inside Bullet.
		# ASan is incompatible with /RTC* and /ZI; the annotation macros avoid LNK2038 mismatches.
		add_compile_options(/fsanitize=address)
		add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
		add_link_options(/INCREMENTAL:NO)
		foreach(_lang C CXX)
			foreach(_cfg DEBUG RELWITHDEBINFO)
				string(REGEX REPLACE "/RTC[1csu]+" "" CMAKE_${_lang}_FLAGS_${_cfg} "${CMAKE_${_lang}_FLAGS_${_cfg}}")
				string(REPLACE "/ZI" "/Zi" CMAKE_${_lang}_FLAGS_${_cfg} "${CMAKE_${_lang}_FLAGS_${_cfg}}")
			endforeach()
		endforeach()

		# The ASan runtime of a toolset only initialises with the CRT DLLs of the same toolset; a newer
		# vcruntime140.dll in System32 makes the process die with 0xC0000142. So the matching CRT DLLs and the
		# ASan runtime are copied next to every executable (the exe directory wins over System32 and PATH).
		get_filename_component(_msvc_tools "${CMAKE_CXX_COMPILER}/../../../.." ABSOLUTE)
		get_filename_component(_msvc_ver "${_msvc_tools}" NAME)
		file(GLOB _asan_crt_release "${_msvc_tools}/../../../Redist/MSVC/${_msvc_ver}/x64/Microsoft.VC*.CRT")
		file(GLOB _asan_crt_debug "${_msvc_tools}/../../../Redist/MSVC/${_msvc_ver}/debug_nonredist/x64/Microsoft.VC*.DebugCRT")
		set(_asan_bin "${_msvc_tools}/bin/Hostx64/x64")
		# (release name, debug name) of every runtime DLL present in both redistributable folders
		set(_asan_dlls "clang_rt.asan_dynamic-x86_64.dll|clang_rt.asan_dbg_dynamic-x86_64.dll")
		foreach(_dll vcruntime140 vcruntime140_1 msvcp140 msvcp140_1 msvcp140_2 msvcp140_atomic_wait msvcp140_codecvt_ids concrt140)
			if(EXISTS "${_asan_crt_release}/${_dll}.dll" AND EXISTS "${_asan_crt_debug}/${_dll}d.dll")
				list(APPEND _asan_dlls "${_dll}.dll|${_dll}d.dll")
			endif()
		endforeach()
		# One link at a time: test executables share an output directory and copy the same DLLs into it.
		set_property(GLOBAL APPEND PROPERTY JOB_POOLS estree_asan_runtime=1)
		function(estree_asan_runtime target)
			set_property(TARGET ${target} PROPERTY JOB_POOL_LINK estree_asan_runtime)
			foreach(_pair ${_asan_dlls})
				string(REPLACE "|" ";" _pair "${_pair}")
				list(GET _pair 0 _release_dll)
				list(GET _pair 1 _debug_dll)
				if(_release_dll MATCHES "^clang_rt")
					set(_from_release "${_asan_bin}/${_release_dll}")
					set(_from_debug "${_asan_bin}/${_debug_dll}")
				else()
					set(_from_release "${_asan_crt_release}/${_release_dll}")
					set(_from_debug "${_asan_crt_debug}/${_debug_dll}")
				endif()
				add_custom_command(TARGET ${target} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<IF:$<CONFIG:Debug>,${_from_debug},${_from_release}>" "$<TARGET_FILE_DIR:${target}>")
			endforeach()
		endfunction()
	else()
		add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
		add_link_options(-fsanitize=address,undefined)
	endif()
endif()

# The engine executable (and everything it needs: GLFW, Bullet, FreeType, prebuilt Boost libs) is optional.
# OFF builds only the standalone targets and the tests that do not need those libraries; that is what the
# Linux CI job and any machine without the Windows Boost libs use.
if(WIN32)
	set(_estree_engine_default ON)
else()
	set(_estree_engine_default OFF)
endif()
option(ESTREE_BUILD_ENGINE "Build the ESTree engine executable and its dependencies (GLFW, Bullet, FreeType, Boost libs)" ${_estree_engine_default})

if(ESTREE_BUILD_ENGINE)
	option(GLFW_BUILD_DOCS OFF)
	option(GLFW_BUILD_EXAMPLES OFF)
	option(GLFW_BUILD_TESTS OFF)
	add_subdirectory(Vendor/glfw)

	option(BUILD_BULLET2_DEMOS OFF)
	option(BUILD_CPU_DEMOS OFF)
	option(BUILD_EXTRAS OFF)
	option(BUILD_OPENGL3_DEMOS OFF)
	option(BUILD_UNIT_TESTS OFF)
	add_subdirectory(Vendor/bullet3)

	add_subdirectory(Vendor/freetype2)
endif()

# Boost 1.72 is still used by the engine and the legacy tree pipeline (removed by T09/T13).
# Engine build: BOOST_LIB must point at a directory with Includes/ and Libs/ (prebuilt vc140 x64 static libs).
# BOOST_INCLUDEDIR (cache or env) overrides the header directory (default ${BOOST_LIB}/Includes), e.g. a
# header-only Boost on CI. Without the engine only the headers are needed, and only by tests that use them.
if(DEFINED ENV{BOOST_LIB})
	set(BOOST_LIB "$ENV{BOOST_LIB}" CACHE PATH "Boost root (Includes/, Libs/)")
else()
	set(BOOST_LIB "" CACHE PATH "Boost root (Includes/, Libs/)")
endif()
if(DEFINED ENV{BOOST_INCLUDEDIR})
	set(BOOST_INCLUDEDIR "$ENV{BOOST_INCLUDEDIR}" CACHE PATH "Directory containing boost/ headers (overrides BOOST_LIB/Includes)")
else()
	set(BOOST_INCLUDEDIR "" CACHE PATH "Directory containing boost/ headers (overrides BOOST_LIB/Includes)")
endif()
if(ESTREE_BUILD_ENGINE AND MSVC AND NOT BOOST_LIB)
	message(FATAL_ERROR "Set BOOST_LIB (env or -DBOOST_LIB=...) to a directory containing Includes/ and Libs/ of Boost 1.72, or configure with -DESTREE_BUILD_ENGINE=OFF")
endif()
if(BOOST_INCLUDEDIR)
	set(ESTREE_BOOST_INCLUDE_DIR "${BOOST_INCLUDEDIR}")
elseif(BOOST_LIB)
	set(ESTREE_BOOST_INCLUDE_DIR "${BOOST_LIB}/Includes")
else()
	set(ESTREE_BOOST_INCLUDE_DIR "")
endif()
if(ESTREE_BUILD_ENGINE AND MSVC)
	link_directories(${BOOST_LIB}/Libs/)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

# treegen is engine independent: it must be defined before the global include_directories()/add_definitions()
# below (a subdirectory inherits the directory properties set at the time it is added), and it is built with
# or without the engine.
add_subdirectory(ESEngine/TreeGen)

include_directories(ESEngine/
                    Vendor/bullet3/src/
                    Vendor/glad/include/
                    Vendor/glfw/include/
                    Vendor/glm/
                    Vendor/stb/
                    Vendor/rapidjson/include/
                    ${ESTREE_BOOST_INCLUDE_DIR}
                    Vendor/freetype2/include/)

# Global on purpose: the tests inherit them (BOOST_ALL_NO_LIB keeps MSVC from auto-linking Boost libs).
add_definitions(-DGLFW_INCLUDE_NONE
                -DBOOST_ALL_NO_LIB
                -DPROJECT_SOURCE_DIR=\"ESEngine\")

if(ESTREE_BUILD_ENGINE)
	file(GLOB PROJECT_CONFIGS CMakeLists.txt
	                          Readme.md
	                         .gitattributes
	                         .gitignore
	                         .gitmodules)

	file(GLOB VENDOR_SOURCES Vendor/glad/src/glad.c)

	#Scan sources
	file(GLOB_RECURSE SOURCE_LIST ESEngine/*.cpp ESEngine/*.h ESEngine/*.hpp ESEngine/*.c)
	# treegen is its own library target (linked below), not part of the executable's sources
	list(FILTER SOURCE_LIST EXCLUDE REGEX "/ESEngine/TreeGen/")
	foreach(_source IN ITEMS ${SOURCE_LIST})
	    get_filename_component(_source_path "${_source}" PATH)
	    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
	    string(REPLACE "/" "\\" _group_path "${_group_path}")
	    source_group("${_group_path}" FILES "${_source}")
	endforeach()

	#Scan resources
	file(GLOB_RECURSE RESOURCE_LIST Resources/*.comp Resources/*.frag Resources/*.geom Resources/*.vert Resources/*.json)
	foreach(_source IN ITEMS ${RESOURCE_LIST})
	    get_filename_component(_source_path "${_source}" PATH)
	    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
	    string(REPLACE "/" "\\" _group_path "${_group_path}")
	    source_group("${_group_path}" FILES "${_source}")
	endforeach()

	source_group("Vendor" FILES ${VENDOR_SOURCES})

	# Static Boost libs built with the vc140 toolset. Auto-linking is disabled because it derives the
	# toolset suffix from the compiler (vc142/vc145) and those libs do not exist; the vc140 builds are
	# ABI compatible with later MSVC 14.x toolsets.
	if(MSVC)
		set(BOOST_LIBRARIES "")
		foreach(_lib date_time thread chrono system atomic filesystem program_options)
			list(APPEND BOOST_LIBRARIES
				debug libboost_${_lib}-vc140-mt-gd-x64-1_72
				optimized libboost_${_lib}-vc140-mt-x64-1_72)
		endforeach()
	else()
		# Linux/other: system Boost (e.g. libboost-all-dev) instead of the prebuilt Windows libs
		find_package(Threads REQUIRED)
		set(BOOST_LIBRARIES boost_date_time boost_thread boost_chrono boost_system boost_atomic boost_filesystem boost_program_options Threads::Threads)
	endif()

	add_executable(${PROJECT_NAME} ${SOURCE_LIST} ${RESOURCE_LIST}
						${PROJECT_CONFIGS}	${VENDOR_SOURCES})
	target_link_libraries(${PROJECT_NAME} glfw
	                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
	                      BulletDynamics BulletCollision LinearMath freetype
						  treegen
						  ${BOOST_LIBRARIES}
	)
	if(WIN32)
		# boost::uuids (GameObject ids) uses the Windows CNG random provider
		target_link_libraries(${PROJECT_NAME} bcrypt)
	endif()
	set_target_properties(${PROJECT_NAME} PROPERTIES
	    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
	)
	if(COMMAND estree_asan_runtime)
		estree_asan_runtime(${PROJECT_NAME})
	endif()
endif()

option(ESTREE_BUILD_TESTS "Build unit tests (doctest)" ON)
if(ESTREE_BUILD_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif()

# T08: headless tools. Independent of the engine (they only need treegen), so outside the ESTREE_BUILD_ENGINE gate.
option(ESTREE_BUILD_TOOLS "Build command-line tools (treegen_cli)" ON)
if(ESTREE_BUILD_TOOLS)
	add_subdirectory(tools/treegen_cli)
endif()
