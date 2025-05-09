
set (PROJECT_NAME example27)
project(${PROJECT_NAME})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

file(GLOB SRC
     "*.cpp"
)


add_executable (${PROJECT_NAME}
    ${SRC}
)

target_link_libraries(${PROJECT_NAME}
aos
aot
magic_enum::magic_enum
tomlplusplus::tomlplusplus
sodium
concurrentqueue
nlohmann_json::nlohmann_json
simdjson::simdjson
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and LeakSanitizer" ON)

# if (ENABLE_SANITIZERS)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,leak -fno-omit-frame-pointer -g")
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,leak")
# endif()