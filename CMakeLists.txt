cmake_minimum_required(VERSION 3.10)
project(oh)

include(cmake/pkg.cmake)

add_library(oh src/parser.cc src/contains.cc)
target_compile_features(oh PUBLIC cxx_std_23)
target_link_libraries(oh PRIVATE taocpp::pegtl cista::cista utl)
target_link_libraries(oh PUBLIC date::date)
target_include_directories(oh PUBLIC include)

add_executable(oh-test test/test.cc)
target_link_libraries(oh-test PRIVATE oh gtest_main)