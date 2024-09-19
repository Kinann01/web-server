cmake_minimum_required(VERSION 3.10)
project(web-server VERSION 1.0 DESCRIPTION "Cross-Platform Web Server")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

enable_testing()
add_subdirectory(tests)
add_subdirectory(src)