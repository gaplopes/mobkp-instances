cmake_minimum_required(VERSION 3.10)

project(mobkp-instances)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Warning flags
set(MOBKP_CXX_WARN_FLAGS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  list(APPEND MOBKP_CXX_WARN_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    # -pedantic-errors
    # -Wold-style-cast
    # -Wundef
    # -Wredundant-decls
    # -Wcast-qual
    # -Wmissing-include-dirs
    # -Wdisabled-optimization
    # -Wconversion
    # -Wdouble-promotion
    # -Wshadow
    # -Wnull-dereference
  )
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    list(APPEND MOBKP_CXX_WARN_FLAGS
      -Wvector-operation-performance
      -Wduplicated-cond
    )
  endif()
else()
  message(WARNING "Unknown compiler (not GNU, Clang, or MSVC), not setting any warning flags.")
  set(MOBKP_CXX_WARN_FLAGS "")
endif()

# Find dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake/modules")

# Include FetchContent module
include(FetchContent)

# Find dependencies
find_package(mobkp QUIET)
# If mobkp not found fallback to github
if (NOT mobkp)
  include(FetchContent)
  FetchContent_Declare(
    mobkp
    GIT_REPOSITORY https://github.com/adbjesus/mobkp.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(mobkp)
  if (NOT TARGET mobkp::mobkp)
    add_library(mobkp::mobkp ALIAS mobkp)
  endif()
endif()

find_package(mooutils QUIET)
# If mooutils not found fallback to github
if (NOT mooutils_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    mooutils
    GIT_REPOSITORY https://github.com/adbjesus/mooutils.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(mooutils)
  add_library(mooutils::mooutils ALIAS mooutils)
endif()

find_package(apm QUIET)
# If mooutils not found fallback to github
if (NOT apm_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    apm
    GIT_REPOSITORY https://github.com/adbjesus/apm.git
    GIT_TAG main
  )
  FetchContent_MakeAvailable(apm)
  add_library(apm::apm ALIAS apm)
endif()

find_package(CLI11 QUIET)
if (NOT CLI11_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG        v2.4.2
  )
  FetchContent_MakeAvailable(cli11)
endif()

find_package(glpk REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost REQUIRED)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/apps/include)

# Add the executable and specify the source files
add_executable(mobkp-instances
  ${CMAKE_SOURCE_DIR}/apps/mobkp_instances.cpp
  # Add other source files if necessary
)

# Link the libraries
target_link_libraries(mobkp-instances 
    mobkp::mobkp 
    mooutils::mooutils 
    fmt::fmt 
    Boost::headers
    CLI11::CLI11
)

# Set compile options
target_compile_options(mobkp-instances PRIVATE ${MOBKP_CXX_WARN_FLAGS})

# Set the output name of the executable
set_target_properties(mobkp-instances PROPERTIES OUTPUT_NAME mobkp-instances)

# Install the target
install(TARGETS mobkp-instances)
