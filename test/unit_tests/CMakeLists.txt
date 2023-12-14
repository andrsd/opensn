add_executable(opensn-unit-tests
    byte_array_test.cc
    dynamic_vector_test.cc
    dynamic_matrix_test.cc
    ghosted_parallel_stl_vector_test.cc
    kba_graph_partitioner_test.cc
    nd_array_test.cc
    parallel_stl_vector_test.cc
    sparse_matrix_test.cc
    timing_block_test.cc
    main.cc
)

target_include_directories(
    opensn-unit-tests
    PUBLIC
        ${PROJECT_SOURCE_DIR}
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/external
    PRIVATE
        ${PETSC_INCLUDE_DIR}
)

target_link_libraries(
    opensn-unit-tests
    PUBLIC
        libopensn
        gtest_main
        gmock_main
    PRIVATE
        ${PETSC_LIBRARY}
        MPI::MPI_CXX
)

gtest_discover_tests(opensn-unit-tests)
