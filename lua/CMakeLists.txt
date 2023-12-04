# libopensnlua
file(GLOB_RECURSE LIBOPENSN_LUA_SRCS *.cc)
add_library(libopensnlua SHARED ${LIBOPENSN_LUA_SRCS})

target_include_directories(libopensnlua
    PUBLIC
    ${LUA_INCLUDE_DIR}
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/external
)

target_link_libraries(libopensnlua
    PUBLIC
    ${LUA_LIBRARIES}
    PRIVATE
    ${PETSC_LIBRARY}
    MPI::MPI_CXX
    libopensn
)

target_compile_definitions(libopensnlua PUBLIC OPENSN_WITH_LUA)

target_compile_options(libopensnlua PRIVATE ${OPENSN_CXX_FLAGS})

if(NOT MSVC)
    set_target_properties(libopensnlua PROPERTIES OUTPUT_NAME opensnlua)
endif()


# opensn binary
add_executable(opensn "main.cc")

target_include_directories(opensn
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    ${PROJECT_SOURCE_DIR}/external
)

target_link_libraries(opensn
    PUBLIC
    ${LUA_LIBRARIES}
    PRIVATE
    libopensn
    libopensnlua
    MPI::MPI_CXX
)

target_compile_definitions(opensn PUBLIC OPENSN_WITH_LUA)

target_compile_options(opensn PRIVATE ${OPENSN_CXX_FLAGS})
