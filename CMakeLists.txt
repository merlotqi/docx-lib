cmake_minimum_required(VERSION 3.16)

project(
  docx-lib
  VERSION 0.1.0
  DESCRIPTION "A C++ library for reading and writing .docx (Office Open XML) files"
  HOMEPAGE_URL "https://github.com/merlotqi/docx-lib"
  LANGUAGES CXX)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(DOCX_IS_TOP_LEVEL ON)
else()
  set(DOCX_IS_TOP_LEVEL OFF)
endif()

option(DOCX_BUILD_TESTS "Build the docx-lib test suite" ${DOCX_IS_TOP_LEVEL})
option(DOCX_INSTALL "Generate install rules" ${DOCX_IS_TOP_LEVEL})
option(DOCX_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# ---------------------------------------------------------------------------
# Global settings (only when built standalone)
# ---------------------------------------------------------------------------
if(DOCX_IS_TOP_LEVEL)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE
        "Release"
        CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
  endif()
endif()

# ---------------------------------------------------------------------------
# Helper: standard warning flags for a target
# ---------------------------------------------------------------------------
function(docx_set_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4)
    if(DOCX_WARNINGS_AS_ERRORS)
      target_compile_options(${target} PRIVATE /WX)
    endif()
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
    if(DOCX_WARNINGS_AS_ERRORS)
      target_compile_options(${target} PRIVATE -Werror)
    endif()
  endif()
endfunction()

# ---------------------------------------------------------------------------
# Library
# ---------------------------------------------------------------------------
add_library(docx src/docx.cpp)
add_library(docx::docx ALIAS docx)

target_compile_features(docx PUBLIC cxx_std_20)
target_include_directories(
  docx
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include>)
set_target_properties(docx PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
docx_set_warnings(docx)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(DOCX_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------
# Install / package
# ---------------------------------------------------------------------------
if(DOCX_INSTALL)
  include(GNUInstallDirs)
  include(CMakePackageConfigHelpers)

  install(
    TARGETS docx
    EXPORT docxTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  configure_package_config_file(
    cmake/docxConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/docxConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/docx)
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/docxConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/docxConfig.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/docxConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/docx)
  install(
    EXPORT docxTargets
    NAMESPACE docx::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/docx)
endif()
