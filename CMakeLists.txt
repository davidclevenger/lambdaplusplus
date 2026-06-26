cmake_minimum_required(VERSION 3.21)

project(lambdaplusplus
    VERSION 0.1.0
    DESCRIPTION "A modern C++ library"
    LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
# PROJECT_IS_TOP_LEVEL is set by CMake (>= 3.21). Use it to default the build
# options to ON only when lambdaplusplus is the project being built directly,
# and OFF when it is pulled in as a subdirectory / dependency.
option(LAMBDAPLUSPLUS_BUILD_TESTS    "Build the unit tests"       ${PROJECT_IS_TOP_LEVEL})
option(LAMBDAPLUSPLUS_BUILD_EXAMPLES "Build the examples"         ${PROJECT_IS_TOP_LEVEL})
option(LAMBDAPLUSPLUS_INSTALL        "Generate the install rules" ${PROJECT_IS_TOP_LEVEL})

# ---------------------------------------------------------------------------
# Library target (header-only)
# ---------------------------------------------------------------------------
add_library(lambdaplusplus INTERFACE)
add_library(lambdaplusplus::lambdaplusplus ALIAS lambdaplusplus)

target_include_directories(lambdaplusplus
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(lambdaplusplus INTERFACE cxx_std_20)

# ---------------------------------------------------------------------------
# Tests / examples
# ---------------------------------------------------------------------------
if(LAMBDAPLUSPLUS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(LAMBDAPLUSPLUS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ---------------------------------------------------------------------------
# Install + package config
# ---------------------------------------------------------------------------
if(LAMBDAPLUSPLUS_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    install(TARGETS lambdaplusplus
        EXPORT lambdaplusplusTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT lambdaplusplusTargets
        FILE      lambdaplusplusTargets.cmake
        NAMESPACE lambdaplusplus::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lambdaplusplus)

    configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/lambdaplusplusConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/lambdaplusplusConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lambdaplusplus)

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/lambdaplusplusConfigVersion.cmake
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/lambdaplusplusConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/lambdaplusplusConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lambdaplusplus)
endif()
