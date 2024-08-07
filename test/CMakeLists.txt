cmake_minimum_required(VERSION 3.21)
include(CTest)
CPMAddPackage(NAME unity
              VERSION 2.6.0
              GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
              EXCLUDE_FROM_ALL TRUE
              OPTIONS "UNITY_EXTENSION_MEMORY ON")

set(STONN_COV_OK $<AND:$<BOOL:${STONN_BUILD_TESTS}>,$<BOOL:${STONN_BUILD_COV}>>)
find_program(GCOV NAMES gcov)
find_program(LCOV NAMES lcov lcov.bat lcov.exe lcov.perl)
find_program(GENHTML NAMES genhtml genhtml.perl genhtml.bat)
if (STONN_COV_OK AND NOT GCOV)
    message(WARNING "gcov not found; not running code coverage.")
    set(STONN_COV_OK 0)
endif()
if (STONN_COV_OK AND NOT LCOV)
    message(WARNING "lcov not found; not running code coverage.")
    set(STONN_COV_OK 0)
endif()
if (STONN_COV_OK AND NOT GENHTML)
    message(WARNING "genhtml not found; not running code coverage.")
    set(STONN_COV_OK 0)
endif()

if (STONN_COV_OK AND CMAKE_C_COMPILER_ID MATCHES "(Apple)?[Cc]lang")
    if (CMAKE_C_COMPILER_VERSION VERSION_LESS 3)
        message(FATAL_ERROR "clang version must be 3.0.0 or greater; not running code coverage.")
        set(STONN_COV_OK 0)
    endif()
elseif(STONN_COV_OK AND NOT CMAKE_C_COMPILER_ID MATCHES "GNU")
    message(WARNING "code coverage requires GNU or Clang compiler; not running code coverage.")
    set(STONN_COV_OK 0)
endif()

get_property(GENERATOR_IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (STONN_COV_OK AND NOT(CMAKE_BUILD_TYPE STREQUAL "Debug" OR GENERATOR_IS_MULTI_CONFIG))
    message(WARNING "code coverage results with a non-Debug build may be misleading")
endif()

add_executable(stonn.test
    ${CMAKE_CURRENT_SOURCE_DIR}/stonn.test.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mt19937-64.c)
add_test(NAME stonn COMMAND $<TARGET_FILE:stonn.test>)
target_link_libraries(stonn.test stonn unity m)
if (STONN_COV_OK)
    add_library(envcoverage INTERFACE)
    target_compile_options(envcoverage INTERFACE "-fprofile-arcs" "-ftest-coverage")
    target_link_options(envcoverage INTERFACE "--coverage")
    target_link_libraries(stonn envcoverage)
    target_link_libraries(stonn.test envcoverage)

    set(covinfo ${CMAKE_BINARY_DIR}/.covinfo)
    add_custom_target(cov DEPENDS missingno stonn.test)
    add_custom_command(OUTPUT ${covinfo} missingno
        COMMAND ${LCOV} -c -i -o ${covinfo} -b ${CMAKE_BINARY_DIR} -d ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_CTEST_COMMAND}
        COMMAND ${LCOV} -c -o ${covinfo} -d ${CMAKE_BINARY_DIR} -b ${CMAKE_BINARY_DIR} --gcov-tool ${GCOV}
        COMMAND ${LCOV} -r ${covinfo} -o ${covinfo} ${CMAKE_SOURCE_DIR}/test/*.c
        COMMAND ${GENHTML} ${covinfo} -output ${CMAKE_BINARY_DIR}/coverage)
endif()

