# Copyright 2020 Google LLC

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.10)
project(beehive VERSION 0.0.1 DESCRIPTION "C++ threading library")
set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE HYVE_SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
add_library(beehive SHARED ${HYVE_SRC_FILES})
set_target_properties(beehive PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(beehive SYSTEM PRIVATE include)
find_package (Threads)
target_link_libraries (beehive ${CMAKE_THREAD_LIBS_INIT})

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

include(GoogleTest)
include_directories("${gtest_SOURCE_DIR}/include")

file(GLOB_RECURSE HYVE_TEST_FILES CONFIGURE_DEPENDS test/*.cpp)
add_executable(tests ${HYVE_TEST_FILES})
set_target_properties(tests PROPERTIES VERSION ${PROJECT_VERSION})
target_include_directories(tests SYSTEM PRIVATE include)
target_link_libraries(tests ${CMAKE_THREAD_LIBS_INIT} beehive gtest)
gtest_discover_tests(tests)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
