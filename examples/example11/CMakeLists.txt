
set (PROJECT_NAME example11)
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
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF
    CXX_MODULE_STD_ONLY OFF
)
# Запрещаем CMake добавлять флаги модулей
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API OFF)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)

option(ENABLE_SANITIZERS "Enable AddressSanitizer and LeakSanitizer" ON)

if (ENABLE_SANITIZERS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,leak -fno-omit-frame-pointer -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,leak")
endif()