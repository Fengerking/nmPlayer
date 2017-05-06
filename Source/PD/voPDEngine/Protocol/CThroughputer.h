#pragma once
#include "voPDPort.h"
typedef struct tagThroughputElem
{
	DWORD				bytes;		//the count of bytes, first bit is indicate if it is used
	DWORD				duration;	//<MS>, about one second
	tagThroughputElem*	next;

	tagThroughputElem()
		: bytes(0)
		, duration(0)
		, next(NULL)
	{
	}
} ThroughputElem, *PThroughputElem;

class CThroughputer:MEM_MANAGER
{
public:
	CThroughputer();
	virtual ~CThroughputer();

public:
	bool		AddElem(DWORD dwBytes, DWORD dwDuration);
	bool		GetThroughput(DWORD& dwThroughput);

	bool		HasElem();

protected:
	PThroughputElem		m_pElems;
	PThroughputElem		m_pHead;

	IVOMutex*	m_cs;
	bool				m_bFull;
};
