
set (PROJECT_NAME bch_hash_and_unhash)
project(${PROJECT_NAME})


file(GLOB SRC
     "*.cpp"
)


add_executable (${PROJECT_NAME}
    ${SRC}
)

target_link_libraries(${PROJECT_NAME}
aos
fmt
fmtlog-static
benchmark::benchmark
magic_enum::magic_enum
)

target_include_directories(${PROJECT_NAME}
PRIVATE
${CMAKE_SOURCE_DIR}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
