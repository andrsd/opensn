# test binary
file(GLOB_RECURSE TEST_SRCS CONFIGURE_DEPENDS *.cc)

add_executable(opensn-test ${TEST_SRCS})

target_include_directories(opensn-test
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/external
)

target_link_libraries(opensn-test
    PRIVATE
    libopensn
    libopensnlua
    ${LUA_LIBRARIES}
    ${PETSC_LIBRARY}
    MPI::MPI_CXX
)

target_compile_definitions(opensn-test PRIVATE OPENSN_WITH_LUA)

target_compile_options(opensn-test PRIVATE ${OPENSN_CXX_FLAGS})

add_custom_target(test
    COMMAND
        ${CMAKE_SOURCE_DIR}/test/run_tests
            -d ${CMAKE_SOURCE_DIR}/test
            --exe ${CMAKE_BINARY_DIR}/test/opensn-test
            -v2
            -w3
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(test-tutorials
    COMMAND
        ${CMAKE_SOURCE_DIR}/test/run_tests
            -d ${CMAKE_SOURCE_DIR}/doc/tutorials
            --exe ${CMAKE_BINARY_DIR}/lua/opensn
            -v2
            -w3
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
