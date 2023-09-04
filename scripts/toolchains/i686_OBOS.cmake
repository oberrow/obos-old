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

set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf32")

set(oboskrnl_asmSources "boot/i686/kernel_bootstrap.asm" "descriptors/gdt/gdt.asm"       "descriptors/idt/i686/idt.asm"  "descriptors/idt/i686/handlers.asm"
                        "memory_manager/paging/i686/init.asm" "multitasking/multitasking-i686.asm" "utils/memcpy_i686.asm"         "i686/inline-asm.asm"
                        "driver_api/interrupts.asm"      "syscalls/interrupts.asm"     
)

set (LINKER_SCRIPT "/src/oboskrnl/boot/i686/linker.ld")

set(TARGET_COMPILE_OPTIONS)
set(oboskrnl_platformSpecific "memory_manager/i686/physical.cpp" "memory_manager/paging/i686/init.cpp" "memory_manager/paging/i686/allocate.cpp" "descriptors/idt/i686/idt.cpp")

execute_process(COMMAND i686-elf-g++ -print-file-name=crtbegin.o OUTPUT_VARIABLE CRTBEGIN_DIRECTORY)
execute_process(COMMAND i686-elf-g++ -print-file-name=crtend.o OUTPUT_VARIABLE CRTEND_DIRECTORY)
execute_process(COMMAND i686-elf-gcc --print-file-name=libgcc.a OUTPUT_VARIABLE LIBGCC)

string(STRIP "${LIBGCC}" LIBGCC)
string(STRIP "${CRTBEGIN_DIRECTORY}" CRTBEGIN_DIRECTORY)
string(STRIP "${CRTEND_DIRECTORY}" CRTEND_DIRECTORY)