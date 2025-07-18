
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
  set_target_properties(${prj} PROPERTIES
    CXX_SCAN_FOR_MODULES OFF
    CXX_MODULE_STD_ONLY OFF
  )
  # Запрещаем CMake добавлять флаги модулей
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API OFF)
  set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 0)
  gtest_add_tests(TARGET ${prj})
endmacro()

macro(cxx_executable name dir libs)
cxx_executable_extended(${name} ${dir}/${name}.cc ${libs}  ${ARGN})
endmacro()

cxx_executable(executor_provider ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main concurrentqueue tomlplusplus::tomlplusplus sodium)
cxx_executable(hedge_position_strategy ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium)
cxx_executable(hedge_position_strategy_default ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium)
cxx_executable(hedge_position_storage_by_pair ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium)
cxx_executable(net_position_strategy ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium) 
cxx_executable(net_position_strategy_default ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium) 
cxx_executable(net_position_storage_by_pair ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium) 
cxx_executable(net_position_storage_by_pair_use_external_event_bybit ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue tomlplusplus::tomlplusplus sodium) 
cxx_executable(bybit_execution_event_parser ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(bybit_order_storage ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main magic_enum concurrentqueue simdjson::simdjson tomlplusplus::tomlplusplus sodium)
cxx_executable(bybit_spot_buy_limit_order ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(bybit_place_order_request_maker ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(bybit_cancel_order_request_maker ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(bybit_min_qty_calculator ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(order_book ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(bybit_order_book_event_parser ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(converter_trading_pair_to_big_string_view ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(ed25519_signer ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(binance_order_event_parser ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
cxx_executable(binance_execution_event_parser ${CMAKE_CURRENT_LIST_DIR} aos fmtlog-static fmt gtest_main gmock_main magic_enum concurrentqueue simdjson::simdjson nlohmann_json::nlohmann_json tomlplusplus::tomlplusplus sodium)
