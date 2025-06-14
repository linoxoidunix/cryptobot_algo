cmake_minimum_required(VERSION 3.28)
set (PROJECT_NAME aos)
project(${PROJECT_NAME} C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

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

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Suppressing benchmark's tests" FORCE)#ignore tests for benchmark
set(BENCHMARK_ENABLE_EXAMPLES OFF CACHE BOOL "Disable benchmark examples")
FetchContent_Declare(
    benchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG main
)

FetchContent_MakeAvailable(benchmark)

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

# # Fetch fmt
# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
#   GIT_TAG        master  # or a specific commit/tag
# )
# FetchContent_MakeAvailable(fmt)

# Fetch fmtlog
FetchContent_Declare(
  fmtlog
  GIT_REPOSITORY https://github.com/MengRao/fmtlog.git
  GIT_TAG        main  # or use a stable tag/commit
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


# # Загружаем Dlib
# FetchContent_Declare(
#     dlib
#     GIT_REPOSITORY https://github.com/davisking/dlib.git
#     GIT_TAG master  # Укажите нужную версию
# )
# FetchContent_MakeAvailable(dlib)

# # Загружаем Cereal
# FetchContent_Declare(
#     cereal
#     GIT_REPOSITORY https://github.com/USCiLab/cereal.git
#     GIT_TAG v1.3.2  # или укажите нужную версию
# )

# # Загружаем mlpack
# FetchContent_Declare(
#     mlpack
#     GIT_REPOSITORY https://github.com/mlpack/mlpack.git
#     GIT_TAG master  # или укажите конкретную версию, например v4.2.0
# )

# # Загружаем зависимости для mlpack
# set(DOWNLOAD_DEPENDENCIES ON CACHE BOOL "Download dependencies for mlpack")

# Загружаем все зависимости
# FetchContent_MakeAvailable(cereal)
# FetchContent_MakeAvailable(mlpack)

set_target_properties(gtest PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(gtest_main PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(gmock PROPERTIES FOLDER "Hide3rdPartyLibrary")
set_target_properties(gmock_main PROPERTIES FOLDER "Hide3rdPartyLibrary")

set_target_properties(magic_enum PROPERTIES EXCLUDE_FROM_ALL TRUE)
set_target_properties(tomlplusplus_tomlplusplus PROPERTIES EXCLUDE_FROM_ALL TRUE)

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
    fmt
    fmtlog-static
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_subdirectory(examples)

option(BUILD_TESTS_AOS "Build tests" ON)
if(BUILD_TESTS_AOS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
else()
    message(STATUS "Tests of ${PROJECT_NAME} are skipped.")
endif()
