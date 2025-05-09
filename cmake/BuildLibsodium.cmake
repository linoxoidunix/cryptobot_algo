function(build_libsodium libsodium_SOURCE_DIR)
    set(LIBSODIUM_INSTALL_DIR "${CMAKE_BINARY_DIR}/_deps/libsodium-build")

    if(NOT EXISTS "${LIBSODIUM_INSTALL_DIR}/lib/libsodium.a")
        message(STATUS "Building libsodium manually...")

        execute_process(
            COMMAND ./configure --prefix=${LIBSODIUM_INSTALL_DIR}
            WORKING_DIRECTORY ${libsodium_SOURCE_DIR}
            RESULT_VARIABLE configure_result
            OUTPUT_FILE /dev/null
            ERROR_VARIABLE configure_error
            ERROR_STRIP_TRAILING_WHITESPACE
        )
        if(NOT configure_result EQUAL 0)
            message(FATAL_ERROR "libsodium configure failed:\n${configure_error}")
        endif()

        execute_process(
            COMMAND make -j
            WORKING_DIRECTORY ${libsodium_SOURCE_DIR}
            RESULT_VARIABLE make_result
            OUTPUT_FILE /dev/null
            ERROR_VARIABLE make_error
            ERROR_STRIP_TRAILING_WHITESPACE
        )
        if(NOT make_result EQUAL 0)
            message(FATAL_ERROR "libsodium make failed:\n${make_error}")
        endif()

        execute_process(
            COMMAND make install
            WORKING_DIRECTORY ${libsodium_SOURCE_DIR}
            RESULT_VARIABLE install_result
            OUTPUT_FILE /dev/null
            ERROR_VARIABLE install_error
            ERROR_STRIP_TRAILING_WHITESPACE
        )
        if(NOT install_result EQUAL 0)
            message(FATAL_ERROR "libsodium install failed:\n${install_error}")
        endif()
    endif()

    set(LIBSODIUM_INCLUDE_DIR "${LIBSODIUM_INSTALL_DIR}/include" PARENT_SCOPE)
    set(LIBSODIUM_LIBRARY_DIR "${LIBSODIUM_INSTALL_DIR}/lib" PARENT_SCOPE)
endfunction()
