# drivers/generic/test/CMakeLists.txt

# Copyright (c) 2023 Omar Berrow

add_executable(initrdDriver "main.cpp"
	"${CMAKE_SOURCE_DIR}/src/oboskrnl/driverInterface/x86_64/call_interface.cpp" 
	"${CMAKE_SOURCE_DIR}/src/oboskrnl/arch/x86_64/syscall/syscall_interface.c" "connection_handler.cpp" "parse.cpp")

target_compile_options(initrdDriver
	PUBLIC "-ffreestanding"
	PUBLIC "-nostdlib"
)

set_target_properties(initrdDriver PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}")

target_include_directories(initrdDriver PUBLIC "${CMAKE_SOURCE_DIR}/src/oboskrnl")

target_link_options(initrdDriver
	PUBLIC "-ffreestanding"
	PUBLIC "-nostdlib"
)