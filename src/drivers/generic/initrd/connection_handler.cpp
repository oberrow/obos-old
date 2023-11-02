/*
	drivers/generic/initrd/main.cpp

	Copyright (c) 2023 Omar Berrow
*/

#include <int.h>
#include <error.h>

#include <driverInterface/struct.h>
#include <driverInterface/interface.h>
#include <driverInterface/x86_64/call_interface.h>

#include <arch/x86_64/syscall/syscall_interface.h>

#include "parse.h"

using namespace obos;

[[noreturn]] void kpanic(const char* format, ...);
extern size_t printf(const char* format, ...);

extern driverInterface::driverHeader g_driverHeader;

filesystemCache g_iterators;

extern void ConnectionHandler(uintptr_t _conn)
{
	driverInterface::DriverServer* conn = (driverInterface::DriverServer*)_conn;
	driverInterface::driverCommands command = driverInterface::OBOS_SERVICE_INVALID_SERVICE_COMMAND;
	while(1)
	{
		if (conn->IsConnectionClosed())
		{
			conn->CloseConnection();
			FreeDriverServer(conn);
			ExitThread(0);
		}
		if (!conn->RecvData(&command, sizeof(command)))
			continue;
		switch (command)
		{
		case obos::driverInterface::OBOS_SERVICE_GET_SERVICE_TYPE:
		{
			if (!conn->SendData(&g_driverHeader.driverType, sizeof(g_driverHeader.driverType)))
				break;
			break;
		}
		case obos::driverInterface::OBOS_SERVICE_QUERY_FILE_DATA:
		{
			size_t filepathSize = 0;
			if(!conn->RecvData(&filepathSize, sizeof(filepathSize)))
			{
				conn->SendData(nullptr, 24);
				break;
			}
			char* filepath = (char*)Malloc(filepathSize + 1);
			if (!conn->RecvData(filepath, filepathSize))
			{
				conn->SendData(nullptr, 24);
				Free(filepath);
				break;
			}
			conn->RecvData(nullptr, 9);
			uint64_t response[3] = {};
			GetFileAttribute(filepath, &response[0], (uint32_t*)(response + 2));
			if (!conn->SendData(&response, sizeof(response)))
				break;
			break;
		}
		case obos::driverInterface::OBOS_SERVICE_MAKE_FILE_ITERATOR:
		{
			ustarEntryCacheNode* iter = (ustarEntryCacheNode*)Malloc(sizeof(ustarEntryCacheNode*));
			iter->next = g_filesystemCache.head->next;
			iter->prev = g_filesystemCache.head->prev;
			iter->cache = g_filesystemCache.head->cache;
			conn->RecvData(nullptr, 9);
			conn->SendData(&iter, sizeof(*iter));
			if (g_iterators.tail)
				g_iterators.tail->next = iter;
			if (!g_iterators.head)
				g_iterators.head = iter;
			iter->prev = g_iterators.tail;
			g_iterators.tail = iter;
			g_iterators.size++;
			break;
		}
		case obos::driverInterface::OBOS_SERVICE_NEXT_FILE:
		{
			ustarEntryCacheNode* iter = nullptr;
			if (!conn->RecvData(&iter, sizeof(ustarEntryCacheNode*)))
			{
				conn->SendData(nullptr, 40);
				break;
			}
			{
				ustarEntryCacheNode* node = g_iterators.head;
				while (node)
				{
					if (node == iter)
						break;

					node = node->next;
				}
				if (node != iter)
				{
					conn->SendData(nullptr, 40);
					break;
				}
			}
			uintptr_t ret[4] = { iter->cache->entryFilesize, 0, iter->cache->entryAttributes, g_driverHeader.memoryManipFunctionsResponse.strlen(iter->cache->entry->path) };
			if (!conn->SendData(&ret[0], sizeof(ret))) 
				break;
			conn->SendData(&iter->cache->entry->path[0], ret[3]);
			break;
		}
		case obos::driverInterface::OBOS_SERVICE_CLOSE_FILE_ITERATOR:
		{
			ustarEntryCacheNode* iter = nullptr;
			uint8_t errCode = 0;
			if (!conn->RecvData(&iter, sizeof(ustarEntryCacheNode*)))
			{
				errCode = 1;
				conn->SendData(&errCode, sizeof(errCode));
				break;
			}
			{
				ustarEntryCacheNode* node = g_iterators.head;
				while (node)
				{
					if (node == iter)
						break;

					node = node->next;
				}
				if (node != iter)
				{
					errCode = 1;
					conn->SendData(&errCode, sizeof(errCode));
					break;
				}
			}
			iter->next->prev = iter->prev;
			iter->prev->next = iter->next;
			g_iterators.size--;
			Free(iter);
			break;
		}
		case obos::driverInterface::OBOS_SERVICE_READ_FILE:
		{
			size_t filepathSize = 0;
			if (!conn->RecvData(&filepathSize, sizeof(filepathSize)))
			{
				conn->SendData(nullptr, sizeof(size_t));
				break;
			}
			char* filepath = (char*)Malloc(filepathSize + 1);
			if (!conn->RecvData(filepath, filepathSize))
			{
				conn->SendData(nullptr, sizeof(size_t));
				Free(filepath);
				break;
			}
			if (!FileExists(filepath))
			{
				conn->SendData(nullptr, sizeof(size_t));
				Free(filepath);
				break;
			}
			size_t filesize = 0;
			ReadFile(filepath, filepathSize, &filesize, nullptr);
			char* data = (char*)Malloc(filesize + 1);
			ReadFile(filepath, filepathSize, nullptr, (char*)data);
			if (!conn->SendData(&filesize, sizeof(filesize))) 
			{
				Free(filepath);
				Free(data);
				break;
			}
			if (!conn->SendData(data, filesize)) 
			{
				Free(filepath);
				Free(data);
				break;
			}
			break;
		}
		default:
			break;
		}
	}
}