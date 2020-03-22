cmake_minimum_required(VERSION 3.0)
project(ESTree)

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(ESEngine/Vendor/glfw)

option(BUILD_BULLET2_DEMOS OFF)
option(BUILD_CPU_DEMOS OFF)
option(BUILD_EXTRAS OFF)
option(BUILD_OPENGL3_DEMOS OFF)
option(BUILD_UNIT_TESTS OFF)
add_subdirectory(ESEngine/Vendor/bullet3)

option(RAPIDJSON_BUILD_DOC OFF)
option(RAPIDJSON_BUILD_EXAMPLES OFF)
option(RAPIDJSON_BUILD_TESTS OFF)
option(RAPIDJSON_BUILD_THIRDPARTY_GTEST OFF)
add_subdirectory(ESEngine/Vendor/rapidjson)

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

include_directories(ESEngine/Headers/
                    ESEngine/Vendor/bullet3/src/
                    ESEngine/Vendor/glad/include/
                    ESEngine/Vendor/glfw/include/
                    ESEngine/Vendor/glm/
                    ESEngine/Vendor/stb/
                    ESEngine/Vendor/rapidjson/include/
					$ENV{BOOST_LIB}/Includes/
					ESEngine/Vendor/freetype2/include/)

file(GLOB VENDORS_SOURCES ESEngine/Vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS ESEngine/*.h)
file(GLOB PROJECT_SOURCES ESEngine/*.cpp)
file(GLOB PROJECT_SHADERS Shaders/*.comp
                          Shaders/*.frag
                          Shaders/*.geom
                          Shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"ESEngine\")
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})
target_link_libraries(${PROJECT_NAME} glfw
                      ${GLFW_LIBRARIES} ${GLAD_LIBRARIES}
                      BulletDynamics BulletCollision LinearMath freetype
					  libboost_date_time-vc140-mt-gd-x64-1_72 libboost_thread-vc140-mt-gd-x64-1_72
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
