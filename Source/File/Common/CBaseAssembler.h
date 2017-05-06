#pragma once
#include "voYYDef_filcmn.h"
#include "voSink.h"
#include "fMacros.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


class CItem
{
public:
	CItem(VO_U32 nID, VO_VOID* pDataPtr, VO_U32 nDataLen)
		:m_nID(nID)
		,m_pDataPtr(pDataPtr)
		,m_nDataLen(nDataLen)
		,m_pNext(VO_NULL)
	{

	}
	virtual ~CItem(void){};

	friend class CItemList;

public:
	VO_U32		ID(){return m_nID;}
	VO_VOID*	Data(){return m_pDataPtr;}
	VO_U32		DataLen(){return m_nDataLen;}

private:
	VO_U32		m_nID;
	VO_VOID*	m_pDataPtr;
	VO_U32		m_nDataLen;
	CItem*		m_pNext;
};

class CItemList
{
public:
	CItemList(void){m_pHead=VO_NULL;};
	virtual ~CItemList(void){Release();};

public:
	VO_U16	Append(CItem* pItem)
	{
		if(!m_pHead)
		{
			m_pHead = pItem;
			return GetCount();
		}

		CItem* pLast = m_pHead;

		while(pLast)
		{
			if(!pLast->m_pNext)
			{
				pLast->m_pNext = pItem;
				return GetCount();
			}
		}
		return GetCount();
	}

	VO_U16	Append(VO_U32 nID, VO_VOID* pDataPtr, VO_U32 nDataLen)
	{
		CItem* pItem = new CItem(nID, pDataPtr, nDataLen);

		return Append(pItem);
	}

	CItem*  GetItemByID(VO_U32 nID)
	{
		CItem* pLast = m_pHead;

		while (pLast)
		{
			if(pLast->ID() == nID)
				return pLast;

			pLast = pLast->m_pNext;
		}

		return VO_NULL;
	}

	CItem*  GetItemByIndex(VO_U32 nIndex)
	{
		VO_U32 idx = 0;
		CItem* pLast = m_pHead;

		while (pLast)
		{
			if(idx == nIndex)
				return pLast;

			idx++;
			pLast = pLast->m_pNext;
		}

		return VO_NULL;
	}

	VO_U16	GetCount()
	{
		VO_U16 count = 0;
		CItem* pLast = m_pHead;

		while(pLast)
		{
			count++;

			pLast = pLast->m_pNext;
		}

		return count;
	}

	VO_VOID	Release()
	{
		CItem* pTmp		= VO_NULL;
		CItem* pLast	= m_pHead;

		while(pLast)
		{
			pTmp	= pLast;
			pLast	= pLast->m_pNext;

			delete pTmp;
		}

		m_pHead = VO_NULL;
	}

protected:
	CItem*	m_pHead;
};



class CBaseDumper;

#define MAX_AV_STREAM_COUNT	24

typedef struct _tagAssemblerInitInfo
{
	VO_U16			codec_count;
	VO_U16			codec_list[MAX_AV_STREAM_COUNT];
	CBaseDumper*	dumper;

}ASSEMBLERINITINFO;

class CBaseAssembler
{
public:
	CBaseAssembler(void);
	virtual ~CBaseAssembler(void);

public:
	virtual VO_VOID	Open(ASSEMBLERINITINFO* pInit);
	virtual VO_VOID	Close();
	virtual VO_VOID	AddExtData(VO_U16 nStreamID, VO_SINK_EXTENSION* pExtData);
	virtual VO_VOID	ReleaseExtData();

public:
	virtual VO_VOID	AddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample);
	virtual VO_VOID	Flush(){}
	virtual VO_U32	AssembleBasicTable(){return VO_ERR_NONE;};
protected:
	virtual VO_VOID	doAddSample(VO_BOOL bSync, VO_SINK_SAMPLE * pSample)=0;
	virtual VO_U32 Write(VO_BYTE* pData, VO_U32 nLen);

	VO_SINK_EXTENSION*	GetExtDataByID(VO_U16 nID);
	VO_U16				GetExtDataLenByID(VO_U16 nID);

	VO_SINK_EXTENSION*  CheckExtData(VO_U16 nCodec, VO_U16 nStreamID, VO_BYTE* pFrameStart, VO_U32 nFrameLen);
	VO_VOID				AddExtDataEx(VO_U16 nStreamID, VO_SINK_EXTENSION* pExtData);

protected:
	VO_BOOL				GetHeadData(VO_U16 nStreamID, VO_BYTE** pData, VO_U32* pDataLen);
	VO_BOOL				AddHeadData(VO_U16 nStreamID, VO_BYTE*  pData, VO_U32 pDataLen);
	VO_SINK_EXTENSION*	CloneExtData(VO_SINK_EXTENSION* pExtData);
protected:
	CBaseDumper*		m_pDumper;
	ASSEMBLERINITINFO*	m_pAssembleInitInfo;
	VO_SINK_EXTENSION*	m_pExtData[MAX_AV_STREAM_COUNT];
	VO_BOOL				m_bWaitFirstFrame[MAX_AV_STREAM_COUNT];

	CItemList			m_HeadDataList;
};

#ifdef _VONAMESPACE
}
#endif
