cmake_minimum_required(VERSION 3.16)
project(gppBugAnonDestr CXX)

foreach(conf IN ITEMS ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
	string(TOUPPER ${conf} conf_upper)
	string(TOLOWER ${conf} conf_lower)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${conf_upper} "${CMAKE_BINARY_DIR}/${conf_lower}")
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${conf_upper} "${CMAKE_BINARY_DIR}/${conf_lower}")
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${conf_upper} "${CMAKE_BINARY_DIR}/${conf_lower}")
endforeach(conf)

set(CMAKE_CXX_STANDARD 17)

set(sources
  sources/func.h
  sources/func.cpp
)
add_library(func STATIC ${sources})

set(sources
  sources/common.h
  sources/func.h
  sources/main.cpp
  sources/aaa.cpp
  sources/bbb.cpp
)
add_executable(gppBugAnonDestr ${sources})
target_link_libraries(gppBugAnonDestr PUBLIC func)
