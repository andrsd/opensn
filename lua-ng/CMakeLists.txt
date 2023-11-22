# libopensnluang
file(GLOB_RECURSE LIBOPENSN_LUA_NG_SRCS *.cc)
add_library(libopensnluang SHARED ${LIBOPENSN_LUA_NG_SRCS})

target_include_directories(libopensnluang
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LUA_INCLUDE_DIR}
)

target_link_libraries(libopensnluang
    PRIVATE
    ${LUA_LIBRARIES}
    ${PETSC_LIBRARY}
    MPI::MPI_CXX
    libopensn
)

target_compile_definitions(libopensnluang PRIVATE OPENSN_WITH_LUA)

target_compile_options(libopensnluang PRIVATE ${OPENSN_CXX_FLAGS})

if(NOT MSVC)
    set_target_properties(libopensnluang PROPERTIES OUTPUT_NAME opensnlua)
endif()


# opensnng binary
add_executable(opensnng "main.cc")

target_include_directories(opensnng
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

target_link_libraries(opensnng
    PRIVATE
    ${LUA_LIBRARIES}
    libopensn
    libopensnluang
    MPI::MPI_CXX
)

target_compile_definitions(opensn PRIVATE OPENSN_WITH_LUA)

target_compile_options(opensn PRIVATE ${OPENSN_CXX_FLAGS})
