cmake_minimum_required(VERSION 3.14)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug
    CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()
# (you can also set it on the command line: -D CMAKE_BUILD_TYPE=Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

project(vvm)
set(vvm_VERSION_MAJOR 0)
set(vvm_VERSION_MINOR 5)
set(vvm_VERSION_PATCH 0)

set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB SOURCE "${PROJECT_PATH}/src/*.cpp" "${PROJECT_PATH}/include/vvm/*.h")
file(COPY "${PROJECT_PATH}/font.ttf" DESTINATION "${PROJECT_PATH}/bin")

# Third party sources
set(M3RDPARTY_DIR "${PROJECT_PATH}/include/zep/m3rdparty")
set(M3RDPARTY_INCLUDE "")


# INCLUDE("${M3RDPARTY_DIR}/cmake/copy_files.cmake")
# INCLUDE("${M3RDPARTY_DIR}/list.cmake")

include(ExternalProject)
externalproject_add(
  sdl2
  PREFIX "${M3RDPARTY_DIR}"
  CMAKE_ARGS -DCMAKE_DEBUG_POSTFIX='' -DCMAKE_STATIC=ON
  SOURCE_DIR "${M3RDPARTY_DIR}/sdl"
  TEST_COMMAND ""
  INSTALL_COMMAND ""
  INSTALL_DIR ""
)
list(APPEND M3RDPARTY_INCLUDE
    ${M3RDPARTY_DIR}
    ${CMAKE_BINARY_DIR}
    "${M3RDPARTY_DIR}/sdl"
    "${M3RDPARTY_DIR}/sdl/include"
    "${PROJECT_PATH}/imgui/imgui/examples/libs/gl3w"
    # "${PROJECT_PATH}/include/zep/src"
    # "${PROJECT_PATH}/include/zep/include"
    # "${PROJECT_PATH}/include/zep/src/imgui"
    "${PROJECT_PATH}/include/vvm-core/include"
    # "${PROJECT_PATH}/include/yaml-cpp/include"
    )

set(SDL_LINKLIBS
    GL
    GLU
    dl
    SDL2
    SDL2main
    # SDL2-static
)
link_directories(${M3RDPARTY_DIR}/src/sdl2-build)
# target_link_libraries(sdl2 sndio)

add_subdirectory(imgui)
include_directories("imgui")
include_directories(
  ${PROJECT_PATH}
  ${M3RDPARTY_DIR}
  "${PROJECT_PATH}/include"
  ${M3RDPARTY_INCLUDE}
  ${OPENGL_INCLUDE_DIR}
  )

file(GLOB ZEP_SOURCE
  "${PROJECT_PATH}/include/zep/src/*.cpp"
  "${PROJECT_PATH}/include/zep/src/*.h"
  "${PROJECT_PATH}/include/zep/src/utils/*.cpp"
  "${PROJECT_PATH}/include/zep/src/utils/*.h"
  "${PROJECT_PATH}/include/zep/src/imgui/*.cpp"
  "${PROJECT_PATH}/include/zep/src/imgui/*.h")
# add_library(zep ${ZEP_SOURCE})

add_subdirectory(backward-cpp)
add_subdirectory("include/yaml-cpp")

set(EXECUTABLE_NAME "vvm")
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_executable(${EXECUTABLE_NAME}
  # ${ZEP_SOURCE}
  ${BACKWARD_ENABLE}

  "${PROJECT_PATH}/imgui/imgui/examples/libs/gl3w/GL/gl3w.c"
  "${PROJECT_PATH}/imgui/imgui/examples/imgui_impl_sdl.cpp"
  "${PROJECT_PATH}/imgui/imgui/examples/imgui_impl_opengl3.cpp"

  include/vvm-core/src/memory_container.cpp
  include/vvm-core/src/core.cpp
  include/vvm-core/src/analyzer.cpp
  include/vvm-core/src/functions.cpp
  include/vvm-core/src/debug.cpp
  src/format.cc
  src/application.cpp
  src/main.cpp
)

add_backward(vvm)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if(NOT WIN32)
  # set(CMAKE_C_COMPILER /usr/bin/gcc-7)
  # set(CMAKE_CXX_COMPILER /usr/bin/g++-7)

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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-catch-value")
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS ON)

  add_definitions("-Wall")
  add_definitions("-Werror")
else()
  add_compile_options("/std:c++17")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE
    "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /SAFESEH:NO")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_PATH}/bin)

# OpenGL
find_package(OpenGL REQUIRED)

include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})
if(OPENGL_FOUND)
  target_link_libraries(${EXECUTABLE_NAME} ${OPENGL_LIBRARIES})
endif()

add_dependencies(${PROJECT_NAME} sdl2)
if(NOT WIN32)
  target_link_libraries(${EXECUTABLE_NAME}
      imgui
      Threads::Threads
      stdc++fs
      sndio
      ${SDL_LINKLIBS}
      ${OPENGL_LIBRARIES}
      yaml-cpp
      # zep
      )
else()
  target_link_libraries(${EXECUTABLE_NAME} imgui Threads::Threads)
endif()

target_compile_features(vvm PRIVATE cxx_delegating_constructors)

# Install target
install(TARGETS ${EXECUTABLE_NAME} DESTINATION bin)
