#ifndef _CSAMPLE_DATA_H
#define _CSAMPLE_DATA_H

#include "voType.h"
#include "voCMutex.h"

class CSampleData
{
public:
	CSampleData(void);
public:
	virtual ~CSampleData(void);

public:
	bool			AllocBuf (int nSize);
	bool			FreeData (void);

	int				GetBuffer (VO_PBYTE * ppBuffer);
	int				GetBufSize (void) {return m_nDataSize;}

	bool			SetData (VO_PBYTE pData, int nLen);
	bool			AddData (VO_PBYTE pData, int nLen);
	int				GetData (VO_PBYTE * ppData);

	int				GetDataLen (void) {return m_nDataLen;}
	void			SetDataLen (int nDataLen) {m_nDataLen = nDataLen;}
	VO_PBYTE		GetDataPoint (void) {return m_pData;}

	bool			SetSample (VO_PBYTE pData, int nLen, VO_U32 llStart, VO_U32 llEnd, bool bSync);

	CSampleData *	GetNext (void);
	bool			SetNext (CSampleData * pSampleData);

	bool			GetTime (VO_U32 & dwStart, VO_U32 & llEnd);
	bool			SetTime (VO_U32 dwStart, VO_U32 dwEnd);

	bool			SetSyncPoint (bool bSync);
	bool			GetSyncPoint (void);

	bool			SetExtData (VO_PBYTE pExtData, int nExtLen);

protected:
	VO_PBYTE	m_pData;
	int			m_nDataSize;
	int			m_nDataLen;

	VO_PBYTE	m_pExtData;
	int			m_nExtDataLen;

	VO_U32		m_dwStartTime;
	VO_U32		m_dwEndTime;

	bool		m_bSyncPoint;

	CSampleData *	m_pNext;
};

class CSampleChain
{
public:
	CSampleChain (void);
	virtual ~ CSampleChain (void);

public:
	bool			Init (int nDataSize, int nSampleCount);

	bool			AddTail (CSampleData * pSample);
	CSampleData *	RemoveHead (void);
	CSampleData *	RemoveTail (void);
	bool			Remove (CSampleData* pRemove);

	CSampleData *	GetHead (void) {return m_pHead;}
	CSampleData *	GetTail (void) {return m_pTail;}

	VO_U32			GetDuration (void);

	int				GetSampleCount (void);

	bool			Release (void);

private:
	voCMutex		m_csSample;
	CSampleData *	m_pHead;
	CSampleData *	m_pTail;


	int				m_nSampleCount;

};

#endif // _CSAMPLE_DATA_H
