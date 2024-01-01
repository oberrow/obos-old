# drivers/x86_64/sata/CMakeLists.txt

# Copyright (c) 2023-2024 Omar Berrow

add_executable(sataDriver "main.cpp" "new.cpp" "command.cpp" "interface.cpp" ${CMAKE_SOURCE_DIR}/src/drivers/x86_64/common/enumerate_pci.cpp)

target_compile_options(sataDriver
	PRIVATE "-ffreestanding"
	PRIVATE "-nostdlib"
	PRIVATE "-fPIE"
	PRIVATE "-Wall"
	PRIVATE "-Wextra"
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fno-stack-protector -fno-stack-check -fno-lto>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fno-use-cxa-atexit>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fno-rtti>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fno-exceptions>
	PRIVATE "${DEBUG_SYMBOLS_OPT}"
)
set_property (TARGET sataDriver PROPERTY CXX_STANDARD 20)

set_target_properties(sataDriver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

target_compile_definitions(sataDriver PRIVATE OBOS_DRIVER=1)

target_include_directories(sataDriver PRIVATE "${CMAKE_SOURCE_DIR}/src/oboskrnl")
target_include_directories(sataDriver PRIVATE "${CMAKE_SOURCE_DIR}/src/drivers/x86_64/common/")

target_link_options(sataDriver
	PRIVATE "-ffreestanding"
	PRIVATE "-nostdlib"
	PRIVATE "-pie"
)

add_dependencies(sataDriver oboskrnl)