cmake_minimum_required(VERSION 3.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

project(vvm)
set (vvm_VERSION_MAJOR 0)
set (vvm_VERSION_MINOR 4)
set (vvm_VERSION_PATCH 1)

file(GLOB SOURCE "src/*.cpp" "include/vvm/*.h")
file(COPY "font.ttf" DESTINATION "bin")

include_directories(
  ${PROJECT_BINARY_DIR}
  "${PROJECT_BINARY_DIR}/include"
  "${PROJECT_BINARY_DIR}/include/imgui")

file(GLOB IMLIB_SOURCE "${PROJECT_BINARY_DIR}/include/imgui/*.cpp" "${PROJECT_BINARY_DIR}/include/imgui/*.h")
add_library(imgui ${IMLIB_SOURCE})

add_subdirectory(backward-cpp)

set(EXECUTABLE_NAME "vvm")
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(${EXECUTABLE_NAME}
  include/imgui/imgui.cpp
  include/imgui/imgui_draw.cpp
  include/imgui/imgui-SFML.cpp
  #src/ImguiWindowsFileIO.cpp
  ${BACKWARD_ENABLE}

  src/container.cpp
  src/opcodes.cpp
  src/functions.cpp
  src/debug.cpp
  src/format.cc
  #src/keys.cpp
  src/application.cpp
  src/main.cpp
)

add_backward(vvm)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

IF(NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-function")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-narrowing")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
	set(CMAKE_CXX_STANDARD 14)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)

	add_definitions("-Wall")
	add_definitions("-Werror")
else()
	add_compile_options("/std:c++17")
	SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)


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
	target_link_libraries(${EXECUTABLE_NAME} imgui Threads::Threads stdc++fs)
else()
	target_link_libraries(${EXECUTABLE_NAME} imgui Threads::Threads)
endif()

target_compile_features(vvm PRIVATE cxx_delegating_constructors)

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
