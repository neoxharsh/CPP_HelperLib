cmake_minimum_required(VERSION 3.0.0)
project(CPPHelperLibs VERSION 0.1.0 LANGUAGES C CXX)
include_directories(include)
include_directories(D:/msys64/usr/include/python3.11)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


option(KEYBOARD_LIBRARY "KEYBOARD LIBRARY" ON)
option(SCREEN_DRAWER_LIBRARY "SCREEN DRAWER LIBRARY" ON)

set(LIB_NAME KeyboardListener)
if(KEYBOARD_LIBRARY)
    add_library(${LIB_NAME}libShared SHARED src/${LIB_NAME}/KeyboardListener.cpp)
    target_link_libraries(${LIB_NAME}libShared)
    add_library(${LIB_NAME}libStatic STATIC src/${LIB_NAME}/KeyboardListener.cpp)
    target_link_libraries(${LIB_NAME}libStatic)

    add_executable(${LIB_NAME}Example examples/${LIB_NAME}/KeyboardListenerExample.cpp)
    target_link_libraries(${LIB_NAME}Example -static  ${LIB_NAME}libStatic)

    # add_library(${LIB_NAME}PLIB SHARED python/${LIB_NAME}/KLPythonHelper.cpp)
    # target_link_libraries(${LIB_NAME}PLIB -shared ${LIB_NAME}lib)
    # set_target_properties(${LIB_NAME}PLIB PROPERTIES
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/${LIB_NAME})
    # add_custom_command(TARGET ${LIB_NAME}PLIB POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #     $<TARGET_FILE:${LIB_NAME}lib>
    #     $<TARGET_FILE_DIR:${LIB_NAME}PLIB>
    # COMMENT "Copying DLLs to ${CMAKE_BINARY_DIR}/bin"
    # )
endif(KEYBOARD_LIBRARY)
unset(KEYBOARD_LIBRARY CACHE) 


set(LIB_NAME ScreenDrawer)
if(SCREEN_DRAWER_LIBRARY)
    add_library(${LIB_NAME}libShared SHARED src/${LIB_NAME}/ScreenDrawer.cpp)
    target_link_libraries(${LIB_NAME}libShared gdiplus)
    add_library(${LIB_NAME}libStatic STATIC src/${LIB_NAME}/ScreenDrawer.cpp)
    target_link_libraries(${LIB_NAME}libStatic gdiplus)
    

    add_executable(${LIB_NAME}Example examples/${LIB_NAME}/ScreenDrawerExample.cpp)
    target_link_libraries(${LIB_NAME}Example -static ${LIB_NAME}libStatic)

    add_executable(${LIB_NAME}ExampleSimple examples/${LIB_NAME}/ScreenDrawerExampleSimple.cpp)
    target_link_libraries(${LIB_NAME}ExampleSimple -static ${LIB_NAME}libStatic)

    add_executable(${LIB_NAME}ExampleSimpleTransparentToggle examples/${LIB_NAME}/ScreenDrawerTransparentToggle.cpp)
    target_link_libraries(${LIB_NAME}ExampleSimpleTransparentToggle -static ${LIB_NAME}libStatic)

    
    # add_library(${LIB_NAME}PLIB SHARED python/${LIB_NAME}/SDPythonHelper.cpp)
    # target_link_libraries(${LIB_NAME}PLIB  -shared ${LIB_NAME}lib)
    # set_target_properties(${LIB_NAME}PLIB PROPERTIES
    # RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python/${LIB_NAME})
    # add_custom_command(TARGET ${LIB_NAME}PLIB POST_BUILD
    # COMMAND ${CMAKE_COMMAND} -E copy_if_different
    #     $<TARGET_FILE:${LIB_NAME}lib>
    #     $<TARGET_FILE_DIR:${LIB_NAME}PLIB>
    # COMMENT "Copying DLLs to ${CMAKE_BINARY_DIR}/bin"
    # )
endif(SCREEN_DRAWER_LIBRARY)
unset(SCREEN_DRAWER_LIBRARY CACHE) 


