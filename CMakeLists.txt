cmake_minimum_required(VERSION 3.0.0)
project(rwDrv VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message("no ANDROID_TOOL_BASE, setting to working directory")
    set(ANDROID_TOOL_BASE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_subdirectory(drv_krw)
add_subdirectory(targkrw)
# add_subdirectory(rw_char)
