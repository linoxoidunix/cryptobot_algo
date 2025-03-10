cmake_minimum_required(VERSION 3.28)
set (PROJECT_NAME aot_algo)
project(${PROJECT_NAME} C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

find_package(Boost 1.87.0 REQUIRED) 

if (Boost_FOUND)
    message(STATUS "Boost found: ${Boost_VERSION}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "Boost not found!")
endif()

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        master
)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)

FetchContent_MakeAvailable(magic_enum tomlplusplus)

file(GLOB LIST_H
     "aot_algo/*h"
)
file(GLOB LIST_CPP
     "src/*.cpp"
)

add_library (${PROJECT_NAME}
    ${LIST_CPP}
)

target_include_directories(
    ${PROJECT_NAME}
    ${BOOST_BEAST_FILES}
    PRIVATE
    ${Boost_INCLUDE_DIRS}
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
           $<INSTALL_INTERFACE:include>
           ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    magic_enum
    ${Boost_LIBRARIES}
    tomlplusplus::tomlplusplus
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(examples)
#if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # enable_testing()
    # include(GoogleTest)
    # add_subdirectory(tests)
#else()
#    message(STATUS "Tests will not be built in ${CMAKE_BUILD_TYPE} mode")
#endif()
