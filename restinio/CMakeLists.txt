cmake_minimum_required(VERSION 3.8)

if(NOT RESTINIO_LIBRARY_NAME)
    # That must be the case we RESTinio is used as a dependency.
    set(RESTINIO_LIBRARY_NAME restinio)
endif()

# ====================================================================
# Version
include(${CMAKE_CURRENT_SOURCE_DIR}/version.cmake)
message(STATUS "RESTINIO_VERSION: ${RESTINIO_VERSION}")
# ====================================================================

project(${RESTINIO_LIBRARY_NAME}
        VERSION ${RESTINIO_VERSION}
        LANGUAGES CXX)

# ====================================================================
# RESTinio target and it's attributes
# ====================================================================

add_library(${RESTINIO_LIBRARY_NAME} INTERFACE)
add_library(${RESTINIO_LIBRARY_NAME}::${RESTINIO_LIBRARY_NAME} ALIAS ${RESTINIO_LIBRARY_NAME})

target_include_directories(
    ${RESTINIO_LIBRARY_NAME} INTERFACE
    # TODO: that might be a good idea to move
    #       headers to include dir so the next dir is not pointing to
    #       the parent dir.
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/..>
    $<INSTALL_INTERFACE:include>)

if (RESTINIO_EXPLICIT_CPPSTD)
    if ("${RESTINIO_EXPLICIT_CPPSTD}" STREQUAL "17" )
        target_compile_features(${RESTINIO_LIBRARY_NAME} INTERFACE cxx_std_17)
    elseif ("${RESTINIO_EXPLICIT_CPPSTD}" STREQUAL "20")
        target_compile_features(${RESTINIO_LIBRARY_NAME} INTERFACE cxx_std_20)
    elseif ("${RESTINIO_EXPLICIT_CPPSTD}" STREQUAL "23")
        target_compile_features(${RESTINIO_LIBRARY_NAME} INTERFACE cxx_std_23)
    else ()
        message(FATAL_ERROR
                "invalid RESTINIO_EXPLICIT_CPPSTD (value ='${RESTINIO_EXPLICIT_CPPSTD}')"
                "must be on of: [17, 20, 23]")
    endif ()
endif()

# --------------------------------------------------------------------
# Handle ASIO dependency.
target_link_libraries(${RESTINIO_LIBRARY_NAME} INTERFACE asio::asio)
message(STATUS "${RESTINIO_LIBRARY_NAME} target will depend on 'asio::asio' target")
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# Handle LLHTTP dependency.
target_link_libraries(${RESTINIO_LIBRARY_NAME} INTERFACE llhttp::llhttp)
message(STATUS "${RESTINIO_LIBRARY_NAME} target will depend on 'llhttp::llhttp' target")
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# Handle FMT dependency.
target_link_libraries(${RESTINIO_LIBRARY_NAME} INTERFACE fmt::fmt)
message(STATUS "${RESTINIO_LIBRARY_NAME} target will depend on 'fmt::fmt' target")
# --------------------------------------------------------------------

# --------------------------------------------------------------------
# Handle expected lite dependency.
target_link_libraries(${RESTINIO_LIBRARY_NAME} INTERFACE nonstd::expected-lite)
message(STATUS "${RESTINIO_LIBRARY_NAME} target will depend on 'nonstd::expected-lite' target")
# --------------------------------------------------------------------
target_link_libraries(${RESTINIO_LIBRARY_NAME} INTERFACE sobjectizer::StaticLib)
message(STATUS "${RESTINIO_LIBRARY_NAME} target will depend on 'sobjectizer::StaticLib' target")

if (RESTINIO_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

  set(RESTINIO_CMAKE_INSTALL_DEST_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/restinio CACHE STRING
      "Installation directory for cmake files, relative to ${CMAKE_INSTALL_PREFIX}.")

  set(restinio_version_config_tmp_file ${PROJECT_BINARY_DIR}/restinio-config-version.cmake)
  set(restinio_project_config_tmp_file ${PROJECT_BINARY_DIR}/restinio-config.cmake)

  SET(restinio_install_include_dir ${CMAKE_INSTALL_INCLUDEDIR}/restinio/ CACHE STRING
      "Installation directory for include files, relative to ${CMAKE_INSTALL_PREFIX}.")

  # Generate the version, config and target files into the build directory.
  write_basic_package_version_file(
      ${restinio_version_config_tmp_file}
      VERSION ${RESTINIO_VERSION}
      COMPATIBILITY SameMinorVersion )

  configure_package_config_file(
      "cmake/restinio-config.cmake.in"
      ${restinio_project_config_tmp_file}
      INSTALL_DESTINATION ${RESTINIO_CMAKE_INSTALL_DEST_DIR}
      PATH_VARS RESTINIO_VERSION)

    # Install version, config and target files.
    install(
        FILES ${restinio_project_config_tmp_file} ${restinio_version_config_tmp_file}
        DESTINATION ${RESTINIO_CMAKE_INSTALL_DEST_DIR})

    SET(RESTINIO_INSTALL_TARGETS ${RESTINIO_LIBRARY_NAME})

    install(
        TARGETS ${RESTINIO_INSTALL_TARGETS}
        EXPORT RESTINIO_ALL_TARGETS
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
    )

    install(
        EXPORT RESTINIO_ALL_TARGETS
        NAMESPACE restinio::
        FILE restinio-targets.cmake
        DESTINATION ${RESTINIO_CMAKE_INSTALL_DEST_DIR} )

    file(GLOB_RECURSE RESTINIO_HEADERS_ALL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.?pp)
    foreach (HEADER_FILE ${RESTINIO_HEADERS_ALL} )
      get_filename_component( DIR ${HEADER_FILE} PATH )
      install( FILES ${HEADER_FILE} DESTINATION ${restinio_install_include_dir}/${DIR} )
    endforeach ()
endif ()
