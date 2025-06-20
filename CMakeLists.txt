cmake_minimum_required(VERSION 3.28)
set (PROJECT_NAME aos)
project(${PROJECT_NAME} C CXX)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_CLANG_TIDY "Enable clang-tidy checks" OFF)

if(ENABLE_CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY
        clang-tidy
        -format-style=file
        --extra-arg=-Wno-unknown-warning-option
        --extra-arg=-Wno-unused-command-line-argument
        -header-filter=aos.*\|aoe.*
    )
endif()


set(CLANG_TIDY_FOR_3RD_PARTY "" CACHE STRING "" FORCE)
function(disable_clang_tidy_for_target target)
    set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "")
endfunction()

# Запрещаем CMake добавлять флаги модулей
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP OFF)

# set lld as default linker if exist
find_program(LLD lld)
if(LLD)
    message(STATUS "Using lld linker: ${LLD}")
    set(CMAKE_LINKER ${LLD})
    # Также добавляем флаг для компилятора (если нужно)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")
else()
    message(WARNING "lld linker not found")
endif()


# Включение профилирования времени
set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

find_package(Threads REQUIRED)
find_package(OpenSSL)
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
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(tomlplusplus)


include(${CMAKE_SOURCE_DIR}/cmake/BuildLibsodium.cmake)
FetchContent_Declare(
  libsodium
  GIT_REPOSITORY https://github.com/jedisct1/libsodium.git
  GIT_TAG        stable 
)

FetchContent_MakeAvailable(libsodium)
build_libsodium(${libsodium_SOURCE_DIR})
include_directories(${libsodium_SOURCE_DIR}/src/libsodium/include)
link_directories(${LIBSODIUM_LIBRARY_DIR})

FetchContent_Declare(gtest_main
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
)
FetchContent_MakeAvailable(gtest_main)


option(USE_BENCHMARK_AOS "Enable Google Benchmark support" OFF)

if(USE_BENCHMARK_AOS)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)
    include(FetchContent)
    FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG main
    )
    FetchContent_MakeAvailable(benchmark)
    set_target_properties(benchmark PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
    set_target_properties(benchmark PROPERTIES FOLDER "Hide3rdPartyLibrary")
    set_target_properties(benchmark PROPERTIES EXCLUDE_FROM_ALL TRUE)
    set_target_properties(benchmark_main PROPERTIES FOLDER "Hide3rdPartyLibrary")
    set_target_properties(benchmark_main PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()


FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        master
)
FetchContent_MakeAvailable(magic_enum)

FetchContent_Declare(
    concurrentqueue
    GIT_REPOSITORY https://github.com/cameron314/concurrentqueue.git
    GIT_TAG        master
    OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(concurrentqueue)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
)
FetchContent_MakeAvailable(nlohmann_json)

# Fetch fork fmtlog
set(BUILD_TESTS_FMTLOG OFF CACHE BOOL "Suppressing fmtlog's tests" FORCE)
FetchContent_Declare(
  fmtlog
  GIT_REPOSITORY https://github.com/linoxoidunix/fmtlog.git
  GIT_TAG        main
)
FetchContent_MakeAvailable(fmtlog)
include_directories(${fmtlog_SOURCE_DIR})


message(STATUS "Starting FetchContent for simdjson...")
include(FetchContent)

FetchContent_Declare(
    simdjson
    GIT_REPOSITORY https://github.com/simdjson/simdjson.git
    GIT_TAG        master
)
message(STATUS "Declared simdjson FetchContent.")
FetchContent_MakeAvailable(simdjson)
message(STATUS "simdjson FetchContent is available.")

set_target_properties(fmt PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
set_target_properties(fmtlog-static PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
set_target_properties(gtest_main PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
set_target_properties(gtest PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
set_target_properties(simdjson PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
set_target_properties(gmock_main PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")
set_target_properties(gmock PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_FOR_3RD_PARTY}")


# set(DOWNLOAD_DEPENDENCIES ON CACHE BOOL "Download dependencies for mlpack")

set_target_properties(gtest PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(gtest_main PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(gmock PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(gmock_main PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(fmt PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(fmtlog-static PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(fmtlog-shared PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(simdjson PROPERTIES FOLDER "Hide3rdPartyLibrary")

set_target_properties(magic_enum PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(tomlplusplus_tomlplusplus PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(fmt PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(fmtlog-static PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(fmtlog-shared PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(simdjson PROPERTIES EXCLUDE_FROM_ALL TRUE)

file(GLOB LIST_H
     "aos/*h"
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
    # ${cereal_SOURCE_DIR}/include
    ${mlpack_SOURCE_DIR}/include
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
           $<INSTALL_INTERFACE:include>
           ${CMAKE_BINARY_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    OpenSSL::SSL OpenSSL::Crypto
    magic_enum
    concurrentqueue
    ${Boost_LIBRARIES}
    tomlplusplus::tomlplusplus
    nlohmann_json::nlohmann_json
    sodium
    fmtlog-static
    fmt
)


add_subdirectory(examples)


set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF
    CXX_MODULE_STD_ONLY OFF
)


option(BUILD_TESTS_AOS "Build tests" ON)
if(BUILD_TESTS_AOS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
else()
    message(STATUS "Tests of ${PROJECT_NAME} are skipped.")
endif()

add_custom_target(stats
    COMMAND ${CMAKE_COMMAND} -E echo "=== Build statistics ==="
    COMMAND ${CMAKE_COMMAND} -E echo "Total build time: $<TARGET_PROPERTY:${PROJECT_NAME},TIME_BUILD>"
)