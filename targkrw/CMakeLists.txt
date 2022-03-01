add_library(targkrw STATIC krw.cpp)
target_include_directories(targkrw PRIVATE
    ${ANDROID_TOOL_BASE}/common/local_util
    ${ANDROID_TOOL_BASE}/common/krw_util/krw_util
    ../include)
