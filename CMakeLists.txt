cmake_minimum_required(VERSION 3.0.0)
project(KeyboardListener VERSION 0.1.0 LANGUAGES C CXX)
include_directories(include)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



option(KEYBOARD_LIBRARY "KEYBOARD LIBRARY" OFF)


# set(KEYBOARD_LIBRARY ON)
if(KEYBOARD_LIBRARY)
    add_executable(KeyboardListenerExample examples/KeyboardListenerExample.cpp)
    add_library(KeyboardListenerlib SHARED src/KeyboardListener.cpp)
    target_link_libraries(KeyboardListenerExample -static  KeyboardListenerlib)
    target_link_libraries(KeyboardListenerlib)
endif(KEYBOARD_LIBRARY)
unset(KEYBOARD_LIBRARY CACHE) 

