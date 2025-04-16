
set (PROJECT_NAME unit_test)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 23)

macro(cxx_executable_extended prjname src)
  set(prj ${PROJECT_NAME}_${prjname})
  add_executable(${prj} ${src})
  foreach (lib "${ARGN}")
    target_link_libraries(${prj} ${lib})
  endforeach()
  target_include_directories(${prj} PRIVATE ${CMAKE_SOURCE_DIR})
  set_property(TARGET ${prj} PROPERTY CXX_STANDARD 23)
  gtest_add_tests(TARGET ${prj})
endmacro()

macro(cxx_executable name dir libs)
cxx_executable_extended(${name} ${dir}/${name}.cc ${libs}  ${ARGN})
endmacro()

cxx_executable(executor_provider ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main )
cxx_executable(hedge_position_strategy ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(hedge_position_strategy_default ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(hedge_position_storage_by_pair ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(net_position_strategy ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(net_position_strategy_default ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(net_position_storage_by_pair ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(net_position_storage_by_pair_use_external_event_bybit ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum)
cxx_executable(bybit_execution_event_parser ${CMAKE_CURRENT_LIST_DIR} aos aot gtest_main magic_enum simdjson::simdjson)


