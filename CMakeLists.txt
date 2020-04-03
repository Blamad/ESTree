cmake_minimum_required(VERSION 3.0)
project(ESTree)

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

set(RAPIDJSON_BUILD_DOC OFF)
set(RAPIDJSON_BUILD_EXAMPLES OFF)
set(RAPIDJSON_BUILD_TESTS OFF)
set(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF)
add_subdirectory(Vendor/rapidjson)

link_directories($ENV{FREETYPE2_LIB}/Libs/)
link_directories($ENV{BOOST_LIB}/Libs/)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
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
					$ENV{BOOST_LIB}/Includes/
					Vendor/freetype2/include/)

file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)
						 
file(GLOB VENDOR_SOURCES Vendor/glad/src/glad.c)

file(GLOB_RECURSE SOURCE_LIST ESEngine/*.cpp ESEngine/*.h ESEngine/*.hpp ESEngine/*.c Resources/*.comp Resources/*.frag Resources/*.geom Resources/*.vert Resources/*.json)
foreach(_source IN ITEMS ${SOURCE_LIST})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
source_group("Vendor" FILES ${VENDOR_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"ESEngine\")
add_executable(${PROJECT_NAME} ${SOURCE_LIST}
					${PROJECT_CONFIGS}	${VENDOR_SOURCES})
target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath freetype
					  libboost_date_time-vc140-mt-gd-x64-1_72 libboost_thread-vc140-mt-gd-x64-1_72
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})