set(CMAKE_SYSTEM_NAME "Generic")
set(CMAKE_SYSTEM_PROCESSOR "x86-64")

set(CMAKE_C_COMPILER "x86_64-elf-gcc")
set(CMAKE_CXX_COMPILER "x86_64-elf-g++")
set(CMAKE_ASM-ATT_COMPILER "x86_64-elf-gcc")
set(CMAKE_ASM_NASM_COMPILER "nasm")
set(CMAKE_C_COMPILER_WORKS true)
set(CMAKE_CXX_COMPILER_WORKS true)
set(CMAKE_ASM-ATT_COMPILER_WORKS true)
set(CMAKE_ASM_NAME_COMPILER_WORKS true)

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

set(CMAKE_ASM_NASM_OBJECT_FORMAT "elf64 -g")

set(oboskrnl_asmSources "boot/x86-64/kernel_bootstrap.asm"		"x86-64/inline-asm.asm"				   "descriptors/idt/x86-64/handlers.asm" "descriptors/idt/x86-64/idt.asm" "utils/memcpy_x86-64.asm"
                        "memory_manager/paging/x86-64/init.asm" "multitasking/multitasking_x86-64.asm" "driver_api/interrupts.asm"			 "syscalls/interrupts.asm"		  "descriptors/gdt/gdt_x86-64.asm")

set(oboskrnl_platformSpecific "descriptors/idt/x86-64/idt.cpp" "x86-64/exception_handlers.cpp" "memory_manager/paging/x86-64/init.cpp" "memory_manager/paging/x86-64/allocate.cpp"
                              "memory_manager/x86-64/physical.cpp")

set (LINKER_SCRIPT "/src/oboskrnl/boot/x86-64/linker.ld")

execute_process(COMMAND x86_64-elf-g++ -print-file-name=crtbegin.o OUTPUT_VARIABLE CRTBEGIN_DIRECTORY)
execute_process(COMMAND x86_64-elf-g++ -print-file-name=crtend.o OUTPUT_VARIABLE CRTEND_DIRECTORY)
execute_process(COMMAND x86_64-elf-gcc --print-file-name=libgcc.a OUTPUT_VARIABLE LIBGCC)

set(TARGET_COMPILE_OPTIONS "PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mcmodel=kernel>" 
                           "PRIVATE $<$<COMPILE_LANGUAGE:C>:-mcmodel=kernel>"
                           "PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-mno-red-zone>"
                           "PRIVATE $<$<COMPILE_LANGUAGE:C>:-mno-red-zone>")

string(STRIP "${LIBGCC}" LIBGCC)
string(STRIP "${CRTBEGIN_DIRECTORY}" CRTBEGIN_DIRECTORY)
string(STRIP "${CRTEND_DIRECTORY}" CRTEND_DIRECTORY)

# set(CMAKE_ASM_NASM_LINK_EXECUTABLE "x86_64-elf-gcc -T ${CMAKE_SOURCE_DIR}${LINKER_SCRIPT} -g -Xlinker -Map ${OUTPUT_DIR}/testProgram.map -ffreestanding -nostdlib <OBJECTS> -o ${OUTPUT_DIR}/oboskrnl -mcmodel=kernel")