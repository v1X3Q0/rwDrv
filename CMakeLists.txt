cmake_minimum_required(VERSION 3.0.0)
project(rwDrv VERSION 0.1.0)

if(NOT DEFINED ANDROID_TOOL_BASE)
    message(FATAL_ERROR "need ANDROID_TOOL_BASE")
endif()

add_subdirectory(drv_krw)
add_subdirectory(targkrw)
# add_subdirectory(rw_char)
