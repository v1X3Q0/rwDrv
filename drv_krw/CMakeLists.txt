add_library(drv_krw STATIC drv_krw.cpp)
target_include_directories(drv_krw PRIVATE
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/krw_util/krw_util
    ../include)