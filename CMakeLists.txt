cmake_minimum_required(VERSION 3.16)
project(jira-lab)

IF( NOT CMAKE_BUILD_TYPE )
   message ("Release build")
   SET( CMAKE_BUILD_TYPE Release ... FORCE )
ENDIF()

set(CMAKE_CXX_STANDARD 11)

find_package(fmt CONFIG REQUIRED)

add_subdirectory(src)

# enable_testing()
include_directories(src)
# add_subdirectory(tests)
