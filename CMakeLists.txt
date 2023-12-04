cmake_minimum_required(VERSION 3.12)

project(opensn
    VERSION 0.0.1
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CheckTypeSize)
include(CheckSymbolExists)

option(OPENSN_WITH_LUA "Build with lua support" ON)

# dependencies
find_package(MPI REQUIRED)

if(OPENSN_WITH_LUA)
    find_package(Lua 5.4 REQUIRED)
endif()

find_package(VTK QUIET)
if(VTK_VERSION VERSION_LESS "8.90.0")
    find_package(VTK COMPONENTS
        vtkCommonCore vtkCommonDataModel
        vtkIOLegacy vtkIOCore
        vtkIOXML vtkParallelCore vtkIOParallelXML
        vtkFiltersCore
        vtkIOEnSight
        vtkIOExodus
        REQUIRED)
else()
    find_package(VTK COMPONENTS
        CommonCore CommonDataModel
        IOLegacy IOCore
        IOXML ParallelCore IOParallelXML
        FiltersCore
        IOEnSight
        IOExodus
        REQUIRED)
endif()

find_package(PETSc 3.17 REQUIRED)
check_symbol_exists(PETSC_USE_64BIT_INDICES "${PETSC_INCLUDE_DIR}/petscconf.h" PETSC_USE_64BIT_INDICES)
if(NOT ${PETSC_USE_64BIT_INDICES} MATCHES 1)
    message(FATAL_ERROR "PETSc has not been configured with the flag --with-64-bit-indices\n")
endif()

# compile options
set(OPENSN_CXX_FLAGS)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    list(APPEND OPENSN_CXX_FLAGS "-pedantic")
    list(APPEND OPENSN_CXX_FLAGS "-Wall")
    list(APPEND OPENSN_CXX_FLAGS "-Wno-unused-variable")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    list(APPEND OPENSN_CXX_FLAGS "-pedantic")
    list(APPEND OPENSN_CXX_FLAGS "-Wall")
    list(APPEND OPENSN_CXX_FLAGS "-Wno-unused-variable")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    list(APPEND OPENSN_CXX_FLAGS "-pedantic")
    list(APPEND OPENSN_CXX_FLAGS "-Wall")
    list(APPEND OPENSN_CXX_FLAGS "-Wno-unused-variable")
    list(APPEND OPENSN_CXX_FLAGS "-Wno-sign-compare")
    list(APPEND OPENSN_CXX_FLAGS "-Wno-psabi")
else()
    message(WARNING "Untested compiler : ${CMAKE_CXX_COMPILER_ID}")
endif()


# libopensn
file(GLOB_RECURSE LIBOPENSN_SRCS
    framework/*.cc
    modules/*.cc
)
add_library(libopensn SHARED ${LIBOPENSN_SRCS})

target_include_directories(libopensn
    PRIVATE
    $<INSTALL_INTERFACE:include/opensn>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/external
    PUBLIC
    ${LUA_INCLUDE_DIR}
    ${PETSC_INCLUDE_DIR}
)

target_link_libraries(libopensn
    PRIVATE
    ${PETSC_LIBRARY}
    ${VTK_LIBRARIES}
    MPI::MPI_CXX
)
if(OPENSN_WITH_LUA)
    target_link_libraries(libopensn PRIVATE ${LUA_LIBRARIES})
    target_compile_definitions(libopensn PRIVATE OPENSN_WITH_LUA)
endif()

target_compile_options(libopensn PRIVATE ${OPENSN_CXX_FLAGS})

if(NOT MSVC)
    set_target_properties(libopensn PROPERTIES OUTPUT_NAME opensn)
endif()

set_target_properties(
    libopensn
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
)

if(OPENSN_WITH_LUA)
    add_subdirectory(lua)
    add_subdirectory(test)
endif()

#
configure_file(config.h.in config.h)

# install

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/OpenSnConfig.cmake.in
    OpenSnConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/opensn
    NO_SET_AND_CHECK_MACRO
)

write_basic_package_version_file(
    OpenSnConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(
    TARGETS libopensn
    EXPORT openSnTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/framework
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/opensn
    FILES_MATCHING PATTERN "*.h"
)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/modules
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/opensn
    FILES_MATCHING PATTERN "*.h"
)

install(
    EXPORT openSnTargets
    FILE OpenSnTargets.cmake
    NAMESPACE opensn::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/opensn
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/OpenSnConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/OpenSnConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/opensn
)

if(OPENSN_WITH_LUA)
    install(
        TARGETS opensn libopensnlua
        EXPORT openSnTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )
endif()
