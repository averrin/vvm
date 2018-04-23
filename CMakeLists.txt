cmake_minimum_required(VERSION 3.5)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

project(vvm)
set (vvm_VERSION_MAJOR 0)
set (vvm_VERSION_MINOR 4)
set (vvm_VERSION_PATCH 2)

set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB SOURCE "${PROJECT_PATH}/src/*.cpp" "${PROJECT_PATH}/include/vvm/*.h")
file(COPY "${PROJECT_PATH}/font.ttf" DESTINATION "${PROJECT_PATH}/bin")

include_directories(
  ${PROJECT_PATH}
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/include/imgui"
  "${PROJECT_PATH}/include/imgui-sfml"
  "${PROJECT_PATH}/include/zep/src"
  "${PROJECT_PATH}/include/zep/src/imgui"
  "${PROJECT_PATH}/include/zep/m3rdparty"
  )

file(GLOB IMLIB_SOURCE
  "${PROJECT_PATH}/include/imgui/*.cpp"
  "${PROJECT_PATH}/include/imgui/*.h"
  "${PROJECT_PATH}/include/imgui-sfml/*.cpp"
  "${PROJECT_PATH}/include/imgui-sfml/*.h"
  )
add_library(imgui ${IMLIB_SOURCE})

file(GLOB ZEP_SOURCE "${PROJECT_PATH}/include/zep/src/*.cpp" "${PROJECT_PATH}/include/zep/src/*.h"
 "${PROJECT_PATH}/include/zep/src/utils/*.cpp" "${PROJECT_PATH}/include/zep/src/utils/*.h"
"${PROJECT_PATH}/include/zep/src/imgui/*.cpp" "${PROJECT_PATH}/include/zep/src/imgui/*.h" )
add_library(zep ${ZEP_SOURCE})

add_subdirectory(backward-cpp)


set(EXECUTABLE_NAME "vvm")
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(${EXECUTABLE_NAME}
  ${IMLIB_SOURCE}
  "${PROJECT_PATH}/include/zep/src/list.cmake"
  ${ZEP_SOURCE}
  #src/ImguiWindowsFileIO.cpp
  ${BACKWARD_ENABLE}

  src/container.cpp
  src/opcodes.cpp
  src/functions.cpp
  src/debug.cpp
  src/format.cc
  #src/keys.cpp
  src/controls.cpp
  src/application.cpp
  src/main.cpp
)

add_backward(vvm)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

IF(NOT WIN32)
  set(CMAKE_C_COMPILER /usr/bin/gcc-7)
  set(CMAKE_CXX_COMPILER /usr/bin/g++-7)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-zero-length")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-parentheses")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)

	add_definitions("-Wall")
	add_definitions("-Werror")
else()
	add_compile_options("/std:c++17")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_PATH}/bin)


# Detect and add SFML
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 REQUIRED system window graphics network audio)
if(SFML_FOUND)
  include_directories(${SFML_INCLUDE_DIR})
  target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})
endif()

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
if (OPENGL_FOUND)
  target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
endif()

IF(NOT WIN32)
	target_link_libraries(${EXECUTABLE_NAME} imgui Threads::Threads stdc++fs zep)
else()
	target_link_libraries(${EXECUTABLE_NAME} imgui Threads::Threads)
endif()

target_compile_features(vvm PRIVATE cxx_delegating_constructors)

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
