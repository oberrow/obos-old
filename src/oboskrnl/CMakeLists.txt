﻿# CMakeLists.txt
#
# Copyright (c) 2023 Omar Berrow

set_source_files_properties(SOURCE "crti/src/crti.S" ASM-ATT)
set_source_files_properties(SOURCE "crti/src/crte.S" ASM-ATT)

set(oboskrnl_cxxSources "boot/kmain.cpp" "console.cpp"             "klog.cpp"                "inline-asm.cpp"
                        "klog.cpp"       "descriptors/gdt/gdt.cpp" "descriptors/idt/idt.cpp" "memory_manager/physical.cpp"
                        "utils/bitfields.cpp")
set(oboskrnl_headers "types.h"      "boot/multiboot.h"      "console.h"             "klog.h"
                     "inline-asm.h" "descriptors/gdt/gdt.h" "descriptors/idt/idt.h" "memory_manager/physical.h"
                     "utils/bitfields.h")
set(oboskrnl_asmSources "boot/kernel_bootstrap-i686.asm" "descriptors/gdt/gdt.asm" "descriptors/idt/idt.asm" "descriptors/idt/handlers.asm")

set(oboskrnl_files ${oboskrnl_asmSources} ${oboskrnl_cxxSources} ${oboskrnl_headers})

add_executable(oboskrnl ${oboskrnl_files})
target_compile_options(oboskrnl PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wall> PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Werror>
                                PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=misleading-indentation> PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-builtin-declaration-mismatch> 
                                PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-Wno-error=unknown-pragmas>
                                PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=gnu++17> PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)
set_target_properties(oboskrnl PROPERTIES LINK_DEPENDS "${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT}")
target_link_options(oboskrnl 
    PUBLIC "-T" PUBLIC "${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT}"
    PUBLIC "${CMAKE_SOURCE_DIR}/src/oboskrnl/crti/src/crti.S" 
    PUBLIC "${CRTBEGIN_DIRECTORY}/crtbegin.o"
    PUBLIC "${CRTBEGIN_DIRECTORY}/crtend.o"
    PUBLIC "${CMAKE_SOURCE_DIR}/src/oboskrnl/crti/src/crte.S"
    PUBLIC "-Xlinker" PUBLIC "-Map" PUBLIC "${OUTPUT_DIR}/oboskrnl.map"
    PUBLIC "-ffreestanding"
    PUBLIC "-nostdlib"
    PUBLIC "-lgcc"
    PUBLIC "-fno-rtti"
    PUBLIC "-fno-exceptions")
set_target_properties(oboskrnl PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")
target_include_directories(oboskrnl PUBLIC "${CMAKE_SOURCE_DIR}/src/oboskrnl")

message("Using linker script at " ${CMAKE_SOURCE_DIR}/${LINKER_SCRIPT})
message("crtbegin.o is at ${CRTBEGIN_DIRECTORY}/crtbegin.o")
message("crtend.o is at ${CRTBEGIN_DIRECTORY}/crtend.o")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Debug configuration.")
    target_compile_definitions(oboskrnl PUBLIC _DEBUG=1)
    target_compile_options(oboskrnl PRIVATE $<$<COMPILE_LANGUAGE:ASM_NASM>:-g>)
else()
    message("Release configuration.")
    target_compile_definitions(oboskrnl PUBLIC NDEBUG=1)
endif()

set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CMAKE_C_COMPILER "i686-elf-gcc")
set(CMAKE_CXX_COMPILER "i686-elf-g++")
set(CMAKE_ASM-ATT_COMPILER "i686-elf-gcc")
set(CMAKE_ASM_NASM_COMPILER "nasm")
set(CMAKE_C_COMPILER_WORKS true)
set(CMAKE_CXX_COMPILER_WORKS true)
set(CMAKE_ASM-ATT_COMPILER_WORKS true)
set(CMAKE_ASM_NAME_COMPILER_WORKS true)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)