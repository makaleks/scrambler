cmake_minimum_required(VERSION 2.8)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
project("scrambler")

include_directories(${CMAKE_SOURCE_DIR}/include)
message(STATUS ${CMAKE_SOURCE_DIR})

#set(CMAKE_BUILD_TYPE Debug)

if(MSVC)
  # Force to always compile with W4
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # Update if necessary
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

# Note: output directories must be passed before targets (all_library,
# add_executable) mentioned
set(PROJECT_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
# Executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY})
# Dinamic libs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY})
# Static libs
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_OUTPUT_DIRECTORY})

# C++ file list
file(GLOB_RECURSE SCRAMBLER_SRC "src/*.c" "src/*.h") 
set(LIB_SOURCE_LIST ${SCRAMBLER_SRC})
#message(STATUS "List is: " ${LIB_SOURCE_LIST})
add_library(scrambler STATIC ${LIB_SOURCE_LIST})

# Compilation info to JSON
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

enable_testing()
#add_subdirectory(tests)
