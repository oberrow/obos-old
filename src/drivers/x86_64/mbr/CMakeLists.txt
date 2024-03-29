# drivers/x86_64/mbr/CMakeLists.txt

# Copyright (c) 2023-2024 Omar Berrow

add_executable(mbrDriver "main.cpp" "../../generic/common/new.cpp")

target_compile_options(mbrDriver
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fno-stack-protector -fno-stack-check -fno-lto>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-use-cxa-atexit>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-nostdlib>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fno-exceptions>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-ffreestanding>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-Wall>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-Wextra>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX,C>:-fPIE>
	PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${TARGET_DRIVER_COMPILE_OPTIONS_CPP}>
	PRIVATE $<$<COMPILE_LANGUAGE:C>:${TARGET_DRIVER_COMPILE_OPTIONS_C}>
	PRIVATE "${DEBUG_SYMBOLS_OPT}"
)
set_property (TARGET mbrDriver PROPERTY CXX_STANDARD 20)

set_target_properties(mbrDriver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

target_compile_definitions(mbrDriver PRIVATE OBOS_DRIVER=1)

target_include_directories(mbrDriver PRIVATE "${CMAKE_SOURCE_DIR}/src/oboskrnl")

target_link_options(mbrDriver
	PRIVATE "-ffreestanding"
	PRIVATE "-nostdlib"
	PRIVATE "-pie"
)

add_dependencies(mbrDriver oboskrnl)