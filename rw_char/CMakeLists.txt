if(NOT DEFINED ARCH)
    message(FATAL_ERROR "need ARCH")
endif()

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${ANDROID_TOOL_BASE}/common/krw_util/krw_util
    )

if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
    if(NOT DEFINED KERNEL_HEADERS_PATH)
        message(WARNING "need KERNEL_HEADERS_PATH, skipping driver")
    else()
        if(NOT DEFINED CROSS_COMPILE)
            message(FATAL_ERROR "need CROSS_COMPILE")
        endif()

        if(NOT DEFINED CROSS_COMPILE_ARM32)
            message(FATAL_ERROR "need CROSS_COMPILE_ARM32")
        endif()

        if(NOT DEFINED VENDOR_MOD)
            message(WARNING "need VENDOR_MOD")
        endif()

        if(NOT DEFINED KERNEL_RELEASE)
            message(WARNING "need KERNEL_RELEASE, gonna build for debug kernel")
        endif()

        set(CROSS_COMPILE_FORCE
            CROSS_COMPILE=${CROSS_COMPILE} CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32}
        )        
    endif()
else()
    set(CROSS_COMPILE_FORCE)
    if (NOT DEFINED KERNEL_HEADERS_PATH)
        # Find the kernel release
        execute_process(
            COMMAND uname -r
            OUTPUT_VARIABLE KERNEL_RELEASE_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(KERNEL_HEADERS_PATH "/usr/src/linux-headers-${KERNEL_RELEASE_VERSION}")
    endif()
endif()

# we have something for kernel headers, then we're building the driver
if (DEFINED KERNEL_HEADERS_PATH)
    get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
    list(APPEND dirs ${INCLUDE_DIRS})
    foreach(dir ${dirs})
        string(APPEND INCLUDE_COMPILER_STRING "-I${dir} ")
    endforeach()
    
    # check, if we are oem or generic. if generic, 
    #     # found it, which means that we need the "_" before "printk"
    if (LINUX_PRINTK_PREFIX MATCHES 1)
        set(LINUX_PRINTK_PREFIX=1)
        string(APPEND INCLUDE_COMPILER_STRING "-DLINUX_PRINTK_PREFIX=${LINUX_PRINTK_PREFIX} ")
    endif()

    FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := rw_char.o")
    FILE(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "\nccflags-y := ${INCLUDE_COMPILER_STRING}")

    FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Makefile "obj-m := rw_char.o")
    FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/Makefile "\nccflags-y := ${INCLUDE_COMPILER_STRING}")

    set(KARCH ${ARCH})
    if(${KARCH} STREQUAL "aarch64")
        set(KARCH arm64)
    endif()

    if(DEFINED KERNEL_RELEASE)
        add_custom_command(
            OUTPUT rw_char.ko
            COMMAND ARCH=${KARCH} ${CROSS_COMPILE_FORCE} make -C ${KERNEL_HEADERS_PATH} M=${CMAKE_CURRENT_BINARY_DIR} modules src=${CMAKE_CURRENT_SOURCE_DIR}
            # should we be building for an off version, will have to patch the binary in post
            COMMAND ${PATCH_CRC} -m ${CMAKE_CURRENT_BINARY_DIR}/rw_char.ko -k ${KERNEL_RELEASE} -i ${VENDOR_MOD}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rw_char.c
            )
    else()
        add_custom_command(
            OUTPUT rw_char.ko
            COMMAND ARCH=${KARCH} ${CROSS_COMPILE_FORCE} make -C ${KERNEL_HEADERS_PATH} M=${CMAKE_CURRENT_BINARY_DIR} modules src=${CMAKE_CURRENT_SOURCE_DIR}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rw_char.c
            )
    endif()

    add_custom_target(rw_char ALL DEPENDS rw_char.ko)

    if (${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_dependencies(rw_char parseandroidkernel)
    endif()
endif()
