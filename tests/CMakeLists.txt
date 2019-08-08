cmake_minimum_required(VERSION 3.11 FATAL_ERROR) 

project(EvaTemplateLibrary_UnitTests)

# This module will be used for downloading third party code.
include(ExternalProject)

# Holds directory where third party libraries are going to be downloaded.
set(THIRDPARTY "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty")

########################################################################################################################
# Catch2 unit testing framework.
########################################################################################################################

ExternalProject_Add(
    Catch2

    GIT_REPOSITORY		"https://github.com/catchorg/Catch2.git"
    GIT_TAG 			"master"

    SOURCE_DIR 			"${THIRDPARTY}/Catch2-master"

    # Don't update and patch.
    UPDATE_COMMAND		""
    PATCH_COMMAND 		""

    # Don't configure and build.
    CONFIGURE_COMMAND 	""
    BUILD_COMMAND     	""

    # Don't test and install.
    TEST_COMMAND		""
    INSTALL_COMMAND		"")


set(Catch2_INCLUDE_DIR "${THIRDPARTY}/Catch2-master/single_include")

########################################################################################################################
# Static library, built from project source code.
########################################################################################################################

add_library(EvaTemplateLibrary 
    STATIC 
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/main.cpp")

target_include_directories(EvaTemplateLibrary
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../src")

target_compile_features(EvaTemplateLibrary PUBLIC cxx_std_17)

########################################################################################################################
# Unit tests executable.
########################################################################################################################

add_executable(${PROJECT_NAME}

    "${CMAKE_CURRENT_SOURCE_DIR}/vector_test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE ${Catch2_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} EvaTemplateLibrary)

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND "$<TARGET_FILE:${PROJECT_NAME}>")

add_dependencies(${PROJECT_NAME} Catch2)