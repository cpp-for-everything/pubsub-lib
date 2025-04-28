cmake_minimum_required(VERSION 3.14)
project(pubsub_lib VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch Catch2 for tests
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

# Enable OpenMP
find_package(OpenMP REQUIRED)

# Create target for the library (header-only)
add_library(pubsub INTERFACE)

# Headers
target_include_directories(pubsub INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if(OpenMP_CXX_FOUND)
    add_definitions(-DWITH_OMP)
    target_link_libraries(pubsub INTERFACE OpenMP::OpenMP_CXX)
endif()

# Install the headers
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS pubsub EXPORT pubsubTargets)

# Export CMake package config
install(EXPORT pubsubTargets
    FILE pubsubTargets.cmake
    NAMESPACE pubsub::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pubsub
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  pubsubConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/pubsubConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/pubsubConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pubsub
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/pubsubConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/pubsubConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/pubsub
)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Optional: Add example
add_subdirectory(example)

# Optional: Add benchmarks
add_subdirectory(benchmarks)
