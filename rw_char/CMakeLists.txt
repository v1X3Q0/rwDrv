if(NOT DEFINED ARCH)
    message(FATAL_ERROR "need ARCH")
endif()

if(NOT DEFINED KERNEL_HEADERS_PATH)
    message(FATAL_ERROR "need KERNEL_HEADERS_PATH")
endif()    

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

get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

foreach(dir ${dirs})
    string(APPEND INCLUDE_COMPILER_STRING "-I${dir} ")
endforeach()

FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "obj-m := rw_char.o")
FILE(APPEND ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild "\nccflags-y := ${INCLUDE_COMPILER_STRING}")

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/Makefile "obj-m := rw_char.o")
FILE(APPEND ${CMAKE_CURRENT_BINARY_DIR}/Makefile "\nccflags-y := ${INCLUDE_COMPILER_STRING}")

# FILE(COPY_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Kbuild ${CMAKE_CURRENT_SOURCE_DIR}/Makefile)

# set_target_properties(rw_char PROPERTIES DRV_BASE rw_char)

if(DEFINED KERNEL_RELEASE)
add_custom_command(
    OUTPUT rw_char.ko
    COMMAND python3 ${CMAKE_SOURCE_DIR}/linux/helpers/scripts/patch_kern_build.py patch ${KERNEL_HEADERS_PATH}/../../../../private/msm-google/Makefile ${CMAKE_CURRENT_BINARY_DIR}/Makefile.bkp_kern
    COMMAND ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} make -C ${KERNEL_HEADERS_PATH} M=${CMAKE_CURRENT_BINARY_DIR} modules src=${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/linux/helpers/scripts/patch_kern_build.py unpatch ${KERNEL_HEADERS_PATH}/../../../../private/msm-google/Makefile ${CMAKE_CURRENT_BINARY_DIR}/Makefile.bkp_kern
    COMMAND ${PATCH_CRC} -m ${CMAKE_CURRENT_BINARY_DIR}/rw_char.ko -k ${KERNEL_RELEASE} -i ${VENDOR_MOD}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rw_char.c
    )
else()
add_custom_command(
    OUTPUT rw_char.ko
    COMMAND python3 ${CMAKE_SOURCE_DIR}/linux/helpers/scripts/patch_kern_build.py patch ${KERNEL_HEADERS_PATH}/../../../../private/msm-google/Makefile ${CMAKE_CURRENT_BINARY_DIR}/Makefile.bkp_kern
    COMMAND ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} CROSS_COMPILE_ARM32=${CROSS_COMPILE_ARM32} make -C ${KERNEL_HEADERS_PATH} M=${CMAKE_CURRENT_BINARY_DIR} modules src=${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND python3 ${CMAKE_SOURCE_DIR}/linux/helpers/scripts/patch_kern_build.py unpatch ${KERNEL_HEADERS_PATH}/../../../../private/msm-google/Makefile ${CMAKE_CURRENT_BINARY_DIR}/Makefile.bkp_kern
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rw_char.c
    )
endif()

add_custom_target(rw_char ALL DEPENDS rw_char.ko)

add_dependencies(rw_char parseandroidkernel)