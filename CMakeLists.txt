cmake_minimum_required(VERSION 3.21)
project(ESTree)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Boost 1.72 is still used by the engine and the legacy tree pipeline (removed by T09/T13).
# BOOST_LIB must point at a directory with Includes/ and Libs/ (prebuilt vc140 x64 static libs).
if(DEFINED ENV{BOOST_LIB})
	set(BOOST_LIB "$ENV{BOOST_LIB}" CACHE PATH "Boost root (Includes/, Libs/)")
else()
	set(BOOST_LIB "" CACHE PATH "Boost root (Includes/, Libs/)")
endif()
if(NOT BOOST_LIB)
	message(FATAL_ERROR "Set BOOST_LIB (env or -DBOOST_LIB=...) to a directory containing Includes/ and Libs/ of Boost 1.72")
endif()
link_directories(${BOOST_LIB}/Libs/)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(ESEngine/
                    Vendor/bullet3/src/
                    Vendor/glad/include/
                    Vendor/glfw/include/
                    Vendor/glm/
                    Vendor/stb/
                    Vendor/rapidjson/include/
					${BOOST_LIB}/Includes/
					Vendor/freetype2/include/)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

file(GLOB VENDOR_SOURCES Vendor/glad/src/glad.c)

#Scan sources
file(GLOB_RECURSE SOURCE_LIST ESEngine/*.cpp ESEngine/*.h ESEngine/*.hpp ESEngine/*.c)
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
set(BOOST_LIBRARIES "")
foreach(_lib date_time thread chrono system atomic filesystem program_options)
	list(APPEND BOOST_LIBRARIES
		debug libboost_${_lib}-vc140-mt-gd-x64-1_72
		optimized libboost_${_lib}-vc140-mt-x64-1_72)
endforeach()

add_definitions(-DGLFW_INCLUDE_NONE
                -DBOOST_ALL_NO_LIB
                -DPROJECT_SOURCE_DIR=\"ESEngine\")
add_executable(${PROJECT_NAME} ${SOURCE_LIST} ${RESOURCE_LIST}
					${PROJECT_CONFIGS}	${VENDOR_SOURCES})
target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath freetype
					  ${BOOST_LIBRARIES}
)
if(WIN32)
	# boost::uuids (GameObject ids) uses the Windows CNG random provider
	target_link_libraries(${PROJECT_NAME} bcrypt)
endif()
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

option(ESTREE_BUILD_TESTS "Build unit tests (doctest)" ON)
if(ESTREE_BUILD_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif()
