cmake_minimum_required(VERSION 3.0)

project(vvm)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# the -I flag in gcc
include_directories(
  ${PROJECT_SOURCE_DIR}/include/
)

set(APP_SOURCES src/main.cpp)
# set(LIB_SOURCES src/World.cpp)

# creates ./build/bin/runner
add_executable(${PROJECT_NAME} ${APP_SOURCES}
  src/container.cpp
  src/opcodes.cpp
  src/format.cc
  )

# shared library
# set(LIBRARY_NAME World)
# add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})
# target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})
target_link_libraries(${PROJECT_NAME})

