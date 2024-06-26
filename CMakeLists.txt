# RESTinio CMake Project File
#
# This CMake file defines no targets on its own. It serves the following purposes:
#   * Setting compiler flags.
#   * Resolving dependencies (e.g., find_package for necessary libraries).
#   * Gathering meaningful targets from subdirectories (add_subdirectory).
#
# This file serves as the primary build entry for RESTinio's developers
# and is not intended for use when RESTinio is added as a dependency
# in another project.
#
# If you are not using a package manager (such as Conan or Vcpkg) and wish to use
# RESTinio as a submodule or any other approach that involves downloading a tarball
# of sources and building RESTinio as part of your CMake-based project,
# consider using the CMake file that defines RESTinio targets
# (found in <prj_root>/dev/destionio). That will simplify the process
# of incorporating RESTinio into your project, avoiding the need to cancel
# various build environment settings defined in this file (and this
# root cmake-file will not help with it in any way).


# Root CMake for RESTinio project.
cmake_minimum_required(VERSION 3.14)

set(RESTINIO_LIBRARY_NAME restinio)
project(${RESTINIO_LIBRARY_NAME}_root CXX)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
  # PROJECT_IS_TOP_LEVEL is defined automatically
elseif (CMAKE_CURRENT_BINARY_DIR STREQUAL CMAKE_BINARY_DIR)
  set(PROJECT_IS_TOP_LEVEL TRUE)
else()
  set(PROJECT_IS_TOP_LEVEL FALSE)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (RESTINIO_EXPLICIT_CPPSTD)
    # The C++ standard is set explicit by user.
    if ("${RESTINIO_EXPLICIT_CPPSTD}" STREQUAL "17" )
        message(STATUS "ENABLE C++17")
        set(CMAKE_CXX_STANDARD 17)
    elseif ("${RESTINIO_EXPLICIT_CPPSTD}" STREQUAL "20")
        message(STATUS "ENABLE C++20")
        set(CMAKE_CXX_STANDARD 20)
    elseif ("${RESTINIO_EXPLICIT_CPPSTD}" STREQUAL "23")
        message(STATUS "ENABLE C++23")
        set(CMAKE_CXX_STANDARD 23)
    else ()
        message(FATAL_ERROR
                "invalid RESTINIO_EXPLICIT_CPPSTD (value ='${RESTINIO_EXPLICIT_CPPSTD}')"
                "must be on of: [17, 20, 23]")
    endif ()
elseif(PROJECT_IS_TOP_LEVEL)
    message(STATUS "SET C++17 BY DEFAULT")
    set(CMAKE_CXX_STANDARD 17)
endif()

if (NOT RESTINIO_EXPLICIT_WIN32_WINNT)
	set(RESTINIO_EXPLICIT_WIN32_WINNT "0x0602") # Win8 by default
endif()

if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if (RESTINIO_EXPLICIT_LIBCXX)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libcxx.cmake)
    handle_explicit_libcxx_if_necessary(${RESTINIO_EXPLICIT_LIBCXX})
endif()

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/link_threads_if_necessary.cmake")

option(RESTINIO_INSTALL "Generate the install target." OFF)
option(RESTINIO_TEST "Build the tests." ${PROJECT_IS_TOP_LEVEL})
option(RESTINIO_SAMPLE "Build samples." ${PROJECT_IS_TOP_LEVEL})
option(RESTINIO_BENCHMARK "Build the benchmarks." ${PROJECT_IS_TOP_LEVEL})
option(RESTINIO_WITH_SOBJECTIZER "Add RESTinio sobjectizer integration" ${PROJECT_IS_TOP_LEVEL})

message(STATUS "RESTINIO_INSTALL:           ${RESTINIO_INSTALL}")
message(STATUS "RESTINIO_TEST:              ${RESTINIO_TEST}")
message(STATUS "RESTINIO_SAMPLE:            ${RESTINIO_SAMPLE}")
message(STATUS "RESTINIO_BENCHMARK:         ${RESTINIO_BENCHMARK}")
message(STATUS "RESTINIO_WITH_SOBJECTIZER:  ${RESTINIO_WITH_SOBJECTIZER}")

# The considered ways to get the dependencies (varies per dependency):
# * system - Assume dependency is installed as system library
# * find - Assume dependency is available with a call to find_package.
# * local - Rely on local sources of the dependency.

# By default we assume to use an embedded version of ASIO.
set(RESTINIO_DEP_STANDALONE_ASIO "find" CACHE STRING "How to get ASIO dependency)")
set_property(CACHE RESTINIO_DEP_STANDALONE_ASIO PROPERTY STRINGS "system;find")

# By default we assume to use Boost::asio other than system.
# to be more specific we will assume that find_package would do it.
set(RESTINIO_DEP_BOOST_ASIO "find" CACHE STRING "How to get Boost.ASIO dependency)")
set_property(CACHE RESTINIO_DEP_BOOST_ASIO PROPERTY STRINGS "system;find")

# By default we assume to use an embedded version of LLHTTP.
set(RESTINIO_DEP_LLHTTP "local" CACHE STRING "How to get llhttp (http parser) dependency)")
set_property(CACHE RESTINIO_DEP_LLHTTP PROPERTY STRINGS "system;find;local")

# By default we assume to use an embedded version of FMT.
set(RESTINIO_DEP_FMT "local" CACHE STRING "How to get FMT dependency)")
set_property(CACHE RESTINIO_DEP_FMT PROPERTY STRINGS "system;find;local")

# By default we assume to use an embedded version of expected-lite.
set(RESTINIO_DEP_EXPECTED_LITE "local" CACHE STRING "How to get expected-lite dependency)")
set_property(CACHE RESTINIO_DEP_EXPECTED_LITE PROPERTY STRINGS "system;find;local")

# By default we assume to use an embedded version of Catch2.
set(RESTINIO_DEP_CATCH2 "local" CACHE STRING "How to get Catch2 dependency)")
set_property(CACHE RESTINIO_DEP_CATCH2 PROPERTY STRINGS "find;local")

# By default we assume to use an embedded version of SObjectizer.
set(RESTINIO_DEP_SOBJECTIZER "local" CACHE STRING "How to get SObjectizer dependency)")
set_property(CACHE RESTINIO_DEP_SOBJECTIZER PROPERTY STRINGS "system;find;local")


message(STATUS "RESTINIO_DEP_STANDALONE_ASIO: ${RESTINIO_DEP_STANDALONE_ASIO}")
message(STATUS "RESTINIO_DEP_BOOST_ASIO:      ${RESTINIO_DEP_BOOST_ASIO}")
message(STATUS "RESTINIO_DEP_LLHTTP:          ${RESTINIO_DEP_LLHTTP}")
message(STATUS "RESTINIO_DEP_FMT:             ${RESTINIO_DEP_FMT}")
message(STATUS "RESTINIO_DEP_EXPECTED_LITE:   ${RESTINIO_DEP_EXPECTED_LITE}")
message(STATUS "RESTINIO_DEP_CATCH2:          ${RESTINIO_DEP_CATCH2}")
message(STATUS "RESTINIO_DEP_SOBJECTIZER:     ${RESTINIO_DEP_SOBJECTIZER}")

set(RESTINIO_ASIO_SOURCE "standalone" CACHE STRING "What source of ASIO to use (standalone, boost)")
set(RESTINIO_ASIO_SOURCE_VALUES "standalone;boost")
set_property(CACHE RESTINIO_ASIO_SOURCE PROPERTY STRINGS ${RESTINIO_ASIO_SOURCE_VALUES})

set(RESTINIO_FMT_TARGET "" CACHE STRING "What target of FMT to use (fmt::fmt-header-only, fmt::fmt)")
set(RESTINIO_FMT_TARGET_VALUES ";fmt::fmt-header-only;fmt::fmt")
set_property(CACHE RESTINIO_FMT_TARGET PROPERTY STRINGS ${RESTINIO_FMT_TARGET_VALUES})

message(STATUS "RESTINIO_ASIO_SOURCE: ${RESTINIO_ASIO_SOURCE}")

if (RESTINIO_FMT_TARGET STREQUAL "" )
    message(STATUS "RESTINIO_FMT_TARGET:  will be decided dynamically")
else ()
    message(STATUS "RESTINIO_FMT_TARGET:  ${RESTINIO_FMT_TARGET}")
endif ()

# This effects llhttp and fmt.
option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so)" OFF)
option(BUILD_STATIC_LIBS "Build static libraries (.lib/.a)" ON)

if (WIN32)
    add_compile_definitions(_WIN32_WINNT=${RESTINIO_EXPLICIT_WIN32_WINNT})
endif ()

if (MSVC)
    add_compile_options(/W3)
    add_compile_definitions(_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
    add_compile_options(/utf-8)
endif ()

# ====================================================================
# Define dependencies.
message(STATUS "Defining RESTinio dependencies...")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(CPM)

cpmaddpackage(
      NAME
      asiocmake
      VERSION
      1.2.1
      GITHUB_REPOSITORY
      "maztheman/asio.cmake"
      GIT_TAG main
      OPTIONS
      "ASIO_USE_CPM ON"
      "ASIO_TAG asio-1-28-2")

cpmaddpackage(
    NAME llhttp
    VERSION 9.2.1
    URL https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.2.1.tar.gz
    OPTIONS
    "BUILD_SHARED_LIBS OFF"
    "BUILD_STATIC_LIBS ON"
)

cpmaddpackage("gh:fmtlib/fmt#10.2.1")

cpmaddpackage(
        NAME expected-lite
        VERSION 0.8.0
        GITHUB_REPOSITORY martinmoene/expected-lite
        OPTIONS
        "BUILD_SHARED_LIBS OFF"
        "BUILD_STATIC_LIBS ON"
)

cpmaddpackage(
        NAME so-5
        VERSION 5.8.2
        GITHUB_REPOSITORY maztheman/sobjectizer
        GIT_TAG master
        OPTIONS
        "SOBJECTIZER_BUILD_SHARED OFF"
        "SOBJECTIZER_BUILD_STATIC ON"
)

find_package(ZLIB)
if (ZLIB_FOUND)
  message(STATUS "ZLIB include dir: ${ZLIB_INCLUDE_DIRS}")
  message(STATUS "ZLIB libraries: ${ZLIB_LIBRARIES}")
endif ()

find_package(OpenSSL)
IF ( OPENSSL_FOUND )
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
ENDIF ( OPENSSL_FOUND )

# PCRE
find_package(PCRE)
IF (PCRE_FOUND)
  message( STATUS "PCRE_LIBRARIES='" ${PCRE_LIBRARIES} "'" )
  message( STATUS "PCRE_INCLUDE_DIRS='" ${PCRE_INCLUDE_DIRS} "'" )
ENDIF ()

# PCRE2
find_package(PCRE2)
IF (PCRE2_FOUND)
  message( STATUS "PCRE2_LIBRARIES='" ${PCRE2_LIBRARIES} "'" )
  message( STATUS "PCRE2_INCLUDE_DIRS='" ${PCRE2_INCLUDE_DIRS} "'" )
ENDIF ()

# PCRE2
find_package(PCRE2)
IF (PCRE2_FOUND)
  message( STATUS "PCRE2_LIBRARIES='" ${PCRE2_LIBRARIES} "'" )
  message( STATUS "PCRE2_INCLUDE_DIRS='" ${PCRE2_INCLUDE_DIRS} "'" )
ENDIF ()
# ====================================================================

option(RESTINIO_GCC_CODE_COVERAGE "Build code coverage targets" OFF)
if (RESTINIO_GCC_CODE_COVERAGE)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_program_required.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/code_coverage.cmake)

    if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug"
        AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo" )

        message(FATAL_ERROR "Codecoverage must be applied for debug builds")
    endif ()

    # Note 1: This code coverage approach is tested only for gcc.
    # Note 2: In case you have multipple gcc versions on the system
    #         you need to make sure that coverage commands (custom cmake commands)
    #         are executed with appropriate version of gcov.
    #         E.g. if you build with gcc-11/g++-11 then you should use
    #         gcov-11.
    #
    # You can use TOOLING_CODE_COVERAGE_GCOV for this:
    # cmake ... -DTOOLING_CODE_COVERAGE_GCOV=gcov-11 ...
    make_code_coverage_targets(
        NAME RESTINIO_CODE_COVERAGE
        FILTERS
            '${CMAKE_SOURCE_DIR}/restinio/.*'
        XML_REPORT_FILE ${CMAKE_SOURCE_DIR}/restinio_coverage.xml
        HTML_REPORT_DIR ${CMAKE_SOURCE_DIR}/restinio_coverage_html
        HTML_TITLE "Code coverage report for RESTinio"
    )

# Example (Ubuntu with gcc-11):
#     cmake -B_build -DCMAKE_BUILD_TYPE=Debug -DRESTINIO_GCC_CODE_COVERAGE=ON -DTOOLING_CODE_COVERAGE_GCOV=gcov-11 .
#     cmake --build _build
#     cmake --build _build --target RESTINIO_CODE_COVERAGE_all
endif()

option(RESTINIO_CLANG_TIDY "Scan build with cllang-tidy" OFF)
if (RESTINIO_CLANG_TIDY)
    message("========================================")
    message(STATUS "Enabling clang-tidy")

    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/find_program_required.cmake)
    find_program_required(
        NAME clang-tidy
        SEARCH_NAME ${TOOLING_CLANG_TIDY_PATH} clang-tidy
        PATH clang_tidy_exe )

    if (NOT TOOLING_CLANG_TIDY_PARAMS)
        include(${CMAKE_CURRENT_SOURCE_DIR}/default-clang-tidy-flags.cmake)
    endif ()

    set(RESTINIO_CLANG_TIDY_ARGS
        "--header-filter='${CMAKE_SOURCE_DIR}/restinio/.*';${TOOLING_CLANG_TIDY_PARAMS}")

    # Rely on CMake feature:
    # https://cmake.org/cmake/help/v3.10/variable/CMAKE_LANG_CLANG_TIDY.html#variable:CMAKE_%3CLANG%3E_CLANG_TIDY
    set(CMAKE_CXX_CLANG_TIDY "${clang_tidy_exe};${RESTINIO_CLANG_TIDY_ARGS}")
    message(STATUS "CMAKE_CXX_CLANG_TIDY: ${CMAKE_CXX_CLANG_TIDY}")
    message("========================================")

# Example (Ubuntu with clang-tidy-12):
#     cmake -B_build -DCMAKE_BUILD_TYPE=Debug -DRESTINIO_CLANG_TIDY=ON -DTOOLING_CLANG_TIDY_PATH=clang-tidy-12 .
#     cmake --build _build
endif ()

message("========================================")
message(STATUS "Defining restinio target...")
add_subdirectory(restinio)
message("========================================")

if(NOT PROJECT_IS_TOP_LEVEL)
    return()
endif()

if (RESTINIO_TEST OR RESTINIO_SAMPLE OR RESTINIO_BENCHMARK)
    message("========================================")
    message("adding restinio-helpers")
    add_subdirectory(restinio-helpers)
    message("========================================")
endif()

if (RESTINIO_TEST)
    CPMAddPackage("gh:catchorg/Catch2@3.5.0")
    list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")
    include(Catch)

    enable_testing()
    add_subdirectory(test)
endif ()

if (RESTINIO_SAMPLE)
    include_directories("${CMAKE_SOURCE_DIR}/clara")
    add_subdirectory(sample)
endif ()

if (RESTINIO_BENCHMARK)
    include_directories("${CMAKE_SOURCE_DIR}/clara")
    add_subdirectory(benches)
endif ()

# vim:ts=4:sts=4:sw=4:expandtab:
