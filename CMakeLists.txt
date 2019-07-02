cmake_minimum_required(VERSION 3.11 FATAL_ERROR) 

project(EvaTemplateLibrary)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -g3 -O0 -std=c++17")

set(SRC_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LIBS_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(BUILD_DIR   "${CMAKE_CURRENT_SOURCE_DIR}/build")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BUILD_DIR}/bin")

file(GLOB SOURCES "${SRC_DIR}/*.cpp")
file(GLOB HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR}) 
