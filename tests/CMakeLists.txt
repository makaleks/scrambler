cmake_minimum_required(VERSION 2.8)

project(overview_tests)

#enable_testing()

# C++ file list
file(GLOB TESTS_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.c") 
#set(CMAKE_BUILD_TYPE Debug)
foreach( t_source ${TESTS_SRC} )
    string( REPLACE ".c" "_test" t_name ${t_source} )
    add_executable( "${t_name}" "${t_source}" )
    target_link_libraries( "${t_name}" scrambler )
    # math.h
    target_link_libraries( "${t_name}" m )
    add_test(NAME "${t_name}" COMMAND "${t_name}")
endforeach( t_source ${TESTS_SRC} )
