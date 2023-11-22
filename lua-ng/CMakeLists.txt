set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

# opensnng binary
file(GLOB_RECURSE OPENSN_LUA_NG_SRCS *.cc)
add_executable(opensnng ${OPENSN_LUA_NG_SRCS})

target_include_directories(opensnng
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>/external
)

target_link_libraries(opensnng
    PRIVATE
    ${LUA_LIBRARIES}
    ${PETSC_LIBRARY}
    libopensn
    MPI::MPI_CXX
)

target_compile_definitions(opensnng PRIVATE OPENSN_WITH_LUA)

target_compile_options(opensnng PRIVATE ${OPENSN_CXX_FLAGS})
