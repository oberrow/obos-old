/*
	arch/x86_64/syscall/handle.cpp

	Copyright (c) 2023-2024 Omar Berrow
*/

#include <int.h>
#include <hashmap.h>

#include <arch/x86_64/syscall/handle.h>

#include <multitasking/process/process.h>

#include <multitasking/cpu_local.h>

namespace obos
{
	namespace syscalls
	{
		user_handle ProcessRegisterHandle(void* _proc, void* objectAddress, ProcessHandleType type)
		{
			if (!_proc)
				_proc = thread::GetCurrentCpuLocalPtr()->currentThread->owner;
			if (type == ProcessHandleType::INVALID ||
				!objectAddress)
				return 0xffffffffffffffff;
			process::Process* proc = (process::Process*)_proc;
			auto &handleTable = proc->context.handleTable;
			// Find the smallest handle value.
			user_handle handleValue = 0;
			for (auto iter = handleTable.begin(); iter; iter++)
				if (auto curr = (*(*iter).key); curr < handleValue)
					handleValue = curr;
			handleTable.emplace_at(handleValue, { objectAddress, type });
			return handleValue;
		}
		void* ProcessReleaseHandle(void* _proc, user_handle handle)
		{
			if (!_proc)
				_proc = thread::GetCurrentCpuLocalPtr()->currentThread->owner;
			if (!ProcessVerifyHandle(_proc, handle, ProcessHandleType::INVALID))
				return nullptr;
			void* ret = ProcessGetHandleObject(_proc, handle);
			process::Process* proc = (process::Process*)_proc;
			auto& handleTable = proc->context.handleTable;
			handleTable.remove(handle);
			return ret;
		}
		bool ProcessVerifyHandle(void* _proc, user_handle handle, ProcessHandleType type)
		{
			if (!_proc)
				_proc = thread::GetCurrentCpuLocalPtr()->currentThread->owner;
			process::Process* proc = (process::Process*)_proc;
			auto& handleTable = proc->context.handleTable;
			if (type == ProcessHandleType::INVALID)
				return handleTable.contains(handle);
			bool has = handleTable.contains(handle);
			if (!has)
				return false;
			return handleTable.at(handle).second == type;
		}
		void* ProcessGetHandleObject(void* _proc, user_handle handle)
		{
			if (!_proc)
				_proc = thread::GetCurrentCpuLocalPtr()->currentThread->owner;
			if (!ProcessVerifyHandle(_proc, handle, ProcessHandleType::INVALID))
				return nullptr;
			process::Process* proc = (process::Process*)_proc;
			auto& handleTable = proc->context.handleTable;
			return handleTable.at(handle).first;
		}
		ProcessHandleType ProcessGetHandleType(void* _proc, user_handle handle)
		{
			if (!_proc)
				_proc = thread::GetCurrentCpuLocalPtr()->currentThread->owner;
			if (!ProcessVerifyHandle(_proc, handle, ProcessHandleType::INVALID))
				return ProcessHandleType::INVALID;
			process::Process* proc = (process::Process*)_proc;
			auto& handleTable = proc->context.handleTable;
			return handleTable.at(handle).second;
		}
	}
}