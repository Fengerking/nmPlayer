#if defined(DMEMLEAK)

//DONOT include it!!! #include "dmemleak.h"
#ifndef _INC_STREAMS_H
#include <windows.h>
#else
#include <Streams.h>
#endif	//_INC_WINDOWS_H
#include <stdio.h>
#include <tchar.h>

#define TEXT_BUF_LEN 1024

struct AllocNode
{
	void* address;
	unsigned int size;
	char file[128];
	unsigned int line;
	AllocNode* prev;
	AllocNode* next;
};

class AllocList
{
private:
	AllocNode* _head;
	AllocNode* _tail;
	FILE* _flog;
	LONG _total; //total memory usage
	LONG _peak; //peak memory usage
	
public:
	AllocList()
	{
		_head = new AllocNode();
		_tail = new AllocNode();
		_head->prev = 0;
		_head->next = _tail;
		_tail->prev = _head;
		_tail->next = 0;
		_head->address = 0;
		_tail->address = 0;
		_flog = 0;
		_total = 0;
		_peak = 0;
	}
	
	~AllocList()
	{
		dumpLeak("Program Quit");
		AllocNode* node = _tail;
		while (node)
		{
			AllocNode* temp = node;
			node = node->prev;
			if (node)
				node->next = 0; //!!!
			delete temp; //for delete == overrided delete, thus will call removeByAddress
		}
		if (_flog)
		{
			fclose(_flog);
			_flog = 0;
		}
	}
	
protected:
	
	AllocNode* add()
	{
		AllocNode* node = new AllocNode();
		node->next = _head->next;
		node->prev = _head;
		node->next->prev = node;
		_head->next = node;
		return node;
	}
	
	bool remove(AllocNode* item)
	{
		AllocNode* node = _head->next;
		while (node)
		{
			if (node == item)
			{
				node->prev->next = node->next;
				node->next->prev = node->prev;
				delete node;
				return true;
			}
			node = node->next;
		}
		return false;
	}
	
	void outputString(const char* str)
	{
		if (_flog)
			fprintf(_flog, str);
		else
		{
#ifdef _UNICODE
			WCHAR buf[TEXT_BUF_LEN];
			MultiByteToWideChar(CP_ACP, 0, str, -1, buf, TEXT_BUF_LEN);
			OutputDebugString(buf);
#else //_UNICODE
			OutputDebugString(str);
#endif  //_UNICODE
		}
	}
	
public:
	
	void add(void* addr, unsigned int asize, const char *fname, unsigned int lnum)
	{
		AllocNode *node = add();
		node->address = addr;
		strcpy(node->file, fname);
		node->line = lnum;
		node->size = asize;
		_total += asize;
		if (_total > _peak)
			_peak = _total;
	}
	
	bool removeByAddress(void* addr)
	{
		AllocNode* node = _head;
		while (node)
		{
			if (node->address == addr)
			{
				_total -= node->size;
				node->prev->next = node->next;
				node->next->prev = node->prev;
				delete node;
				return true;
			}
			node = node->next;
		}
		return false;
	}
	
	void dumpLeak(const char* title)
	{
		unsigned int totalSize = 0;
		char buf[TEXT_BUF_LEN];
		
		sprintf(buf, "\r\nMemory Leak (%s)\r\n"
			"==============================================================\r\n", title);
		outputString(buf);
		
		_head->next->prev = 0; //_tail->prev->next = 0;
		AllocNode* node = _tail->prev; //_head->next;
		while (node)
		{
			sprintf(buf, "Memory Leak --> %s: LINE %d, MEM 0x%p, %d bytes\r\n",
				node->file, node->line, node->address, node->size);
			outputString(buf);
			totalSize += node->size;
			node = node->prev; //node->next;
		}
		sprintf(buf, "---------------------------------\r\n");
		outputString(buf);
		sprintf(buf, "Total Leaked: %d bytes \r\n", totalSize);
		outputString(buf);
#if 0
		// ASSERT(totalSize == _total);
		sprintf(buf, "Total Memory: %d bytes \r\n", _total);
		outputString(buf);
#endif
		sprintf(buf, "\r\nPeak Memory: %d bytes \r\n", _peak);
		outputString(buf);
		
		_head->next->prev = _head; //_tail->prev->next = _tail;
	}
	
	void setLogFile(const _TCHAR* filename)
	{
		if (_flog)
		{
			fclose(_flog);
			_flog = 0;
		}
		_TCHAR path[MAX_PATH];
		_stprintf(path, _T("%s.memleak.log"), filename);
		_flog = _tfopen(path, _T("wb"));
	}
};

AllocList allocList;

void __stdcall AddMemoryTrack(void* addr, unsigned int asize, const char *fname, unsigned int lnum)
{
	allocList.add(addr, asize, fname, lnum);
}

void __stdcall RemoveMemoryTrack(void* addr)
{
	allocList.removeByAddress(addr);
}

void __stdcall SetMemoryLogFile(const _TCHAR* filename)
{
	allocList.setLogFile(filename);
}

void __stdcall DumpMemoryLeak(const char* title)
{
	allocList.dumpLeak(title);
}

#endif //DEBUG
