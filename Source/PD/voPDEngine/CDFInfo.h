/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CDFInfo.h

Contains:	CDFInfo header file

Written by:	East Zhou

Change History (most recent first):
2007-10-17		East		Create file

Contain Download File Information!!!
*******************************************************************************/
#pragma once

#include <voPDPort.h>

#define RESERVE_BUFFER_SIZE		102400	//100K
#define  LIVE_SIZE 0x7fffffff 
typedef struct tagDFFrag:MEM_MANAGER 
{
	DWORD		start;	//start position
	DWORD		len;	//length

	tagDFFrag*	next;	//next fragment

	tagDFFrag()
		: start(0)
		, len(0)
		, next(NULL)
	{
	}

	tagDFFrag(DWORD _start, DWORD _len)
		: start(_start)
		, len(_len)
		, next(NULL)
	{
	}
} DFFrag, *PDFFrag;

class CDFInfo:MEM_MANAGER
{
public:
	CDFInfo();
	virtual ~CDFInfo();

public:
	bool		SetFileSize(DWORD dwFileSize) {m_dwFileSize = dwFileSize; return true;}
	DWORD		GetFileSize() {return m_dwFileSize;}

	bool		AddFragment(DWORD dwStart, DWORD dwLen, bool bLock = true);
	bool		CanRead(DWORD dwStart, DWORD dwLen, bool bLock = true);

	bool		GetBlankFragment(DWORD& dwStart, DWORD& dwLen);
	bool		GetBlankFragmentByStart(DWORD& dwStart, DWORD& dwLen);

	IVOMutex*	GetCritSec() {return m_csFrag;}

	DWORD		GetFragmentNum();
	bool		ReadIn(PBYTE pSrc, DWORD dwFrags);
	bool		WriteOut(PBYTE pDst);

	DWORD		GetCurrPlayDFEnd();

protected:
	void		ClearFragments();

protected:
	PDFFrag		m_pFragHead;
	IVOMutex*	m_csFrag;
	DWORD		m_dwFileSize;
	PDFFrag		m_pFragCurr;

	DWORD		m_dwCurrPlayPos;
};

//for big size file
class CDFInfo2:MEM_MANAGER
{
public:
	CDFInfo2();
	virtual ~CDFInfo2();

public:
	bool		SetContSize(DWORD dwContSize);
	DWORD		GetContSize() {return m_dwContSize;}

	bool		CanRestartDownloadThread(DWORD& dwFilePos);

	bool		RemoveHeader(DWORD dwFileHeaderSize);

	bool		SetFileSize(DWORD dwFileSize) {m_dwFileSize = dwFileSize; return true;}
	DWORD		GetFileSize() {return m_dwFileSize;}

	bool		AddFragment(DWORD dwLen);
	//将该文件位置之前的内容置为废弃数据
	void		RemoveFragment(DWORD dwFilePos);
	//该文件位置是否在content中
	bool		InContent(DWORD dwFilePos);
	void		Reset(DWORD dwFilePos);

	bool		CanRead(DWORD dwFilePos, DWORD dwLen, DWORD& dwContStart, bool bLock = true);
	bool		CanWrite(DWORD dwLen, DWORD* pdwContStart);

	DWORD		ContPos2FilePos(DWORD dwContPos);

	IVOMutex*	GetCritSec() {return m_csFrag;}

	bool		ReadIn(PBYTE pSrc);
	bool		WriteOut(PBYTE pDst);

protected:
	DWORD		GetContLenB();

protected:
	IVOMutex*	m_csFrag;

	bool		m_bOrdinal;
	DWORD		m_dwContStartPos;	//content start position
	DWORD		m_dwContEndPos;		//content end position
	DWORD		m_dwContSize;		//content size
	DWORD		m_dwFileOffset;		//对应源文件位置的offset

	DWORD		m_dwFileSize;
};
