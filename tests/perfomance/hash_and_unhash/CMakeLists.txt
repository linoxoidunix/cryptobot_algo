
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
fmtlog-static
fmt
benchmark::benchmark
magic_enum::magic_enum
)

target_include_directories(${PROJECT_NAME}
PRIVATE
${CMAKE_SOURCE_DIR}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF
    CXX_MODULE_STD_ONLY OFF
)
# Запрещаем CMake добавлять флаги модулей
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)
