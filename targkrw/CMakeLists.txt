project(targkrw)

set(INCLUDE_LIST
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/kern_img/common
    ${ANDROID_TOOL_BASE}/common/kern_img/${MY_HOST_COMPILER}
    ${ANDROID_TOOL_BASE}/common/krw_util/krw_util
    ${ANDROID_TOOL_BASE}/common/bgrep_c
    ../include
    )
    
add_library(${PROJECT_NAME} STATIC krw.cpp kernProc.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_LIST})
