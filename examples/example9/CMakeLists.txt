
set (PROJECT_NAME example9)
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
magic_enum::magic_enum
tomlplusplus::tomlplusplus
sodium
concurrentqueue
nlohmann_json::nlohmann_json
fmtlog-static
fmt
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)