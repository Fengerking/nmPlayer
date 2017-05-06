#ifndef _REPACKER_H_
#define _REPACKER_H_

#include "voYYDef_TS.h"
#include "TsParserBaseDef.h"
#include "tsparse.h"

/**
 * This is a default implementation for repacking data
 * Your class should inherit from CBaseRepacker and implement following functions at least
 * 1. Constructor(VOSTREAMPARSELISTENER* listener) : CBaseRepacker(listener) {}
 * 2. virtual ~Destructor() {}	
 * 3. virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
 * 4. virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);
 */

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

using namespace TS;

//#define FAKE_TIMESTAMP //Jason, 1/4/2011
#define DEFAULT_DELTA 1800 //50fps, 90000/50
#define MAX_FRAME_COUNT_IN_CACHE   512

class CBaseRepacker : public PESDataHandler
{
public:
	CBaseRepacker(VOSTREAMPARSELISTENER* listener);
	virtual ~CBaseRepacker();

	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);

protected:
	VOSTREAMPARSELISTENER* m_pListener;
	//bool m_bDTS;
	//void UseDTS(bool b) { m_bDTS = b; }

#ifdef FAKE_TIMESTAMP
	unsigned long long m_tsPES1;
	unsigned long long m_tsPES2;
	unsigned long long m_tsDelta;
	int m_cFramesOfPES; //frame count between PES1 & PES2 
#endif //FAKE_TIMESTAMP
};

#ifdef _DEBUG

class CNullRepacker : public CBaseRepacker
{
public:
	CNullRepacker(VOSTREAMPARSELISTENER* listener) : CBaseRepacker(listener) {}
	virtual ~CNullRepacker() {}

	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet) {}
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData) {}
};

#endif //_DEBUG


class CPrivateDataRepacker : public CBaseRepacker
{
public:
	CPrivateDataRepacker(VOSTREAMPARSELISTENER* listener);
	virtual ~CPrivateDataRepacker();

	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);
	virtual void Reset();

private:
    int   m_iCurrentDataSize;
    VOSTREAMPARSEFRAMEINFO m_frameInfo;

};



class CPatternRepacker : public CBaseRepacker
{
public:
	CPatternRepacker(VOSTREAMPARSELISTENER* listener, uint32 pattern, uint32 mask, int patternSize,	bool beNeedSyncWord=true);
	virtual ~CPatternRepacker();

public:
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);
	virtual void Reset();

public:
	void SetPESPackMode(bool bPesMode);
protected:
	virtual bool IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId = -1 ) { return true; }

protected:
	inline void CheckPendingMatched(ESConfig* escfg)
	{
		if (m_pendingMatched)
		{
			uint8 buffer[4];
			m_pListener->OnFrameData(escfg->elementary_PID, 
				m_finder.ExportPattern(buffer, m_patternSize), 
				m_pendingMatched);
			m_countFrame += m_pendingMatched;
			m_pendingMatched = 0;
		}
	}

protected:
	PatternFinder m_finder;
	int m_patternSize;
	int m_pendingMatched;
	VOSTREAMPARSEFRAMEINFO m_frameInfo;

	int m_countFrame;
	//add by qichaoshen @2011-12-05
	uint8  m_iPesCount;
	//add by qichaoshen @2011-12-05
	bool m_NeedSyncWord;
	bool m_bPesPackMode;

};

class CMpeg4Repacker : public CPatternRepacker
{
public:
	CMpeg4Repacker(VOSTREAMPARSELISTENER* listener, uint32 pattern, uint32 mask, int patternSize);
	virtual ~CMpeg4Repacker();

protected:
	virtual bool IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId);
private:
	VO_BOOL beSequenceHead;
};


class CPatternRepackerFixedSize : public CPatternRepacker
{
public:
	CPatternRepackerFixedSize(VOSTREAMPARSELISTENER* listener, uint32 pattern, uint32 mask, int patternSize, int frameSize,bool beNeedSyncWord = true);
	virtual ~CPatternRepackerFixedSize();

	//virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);

protected:
	virtual bool IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId);

private:
	int m_frameSize;
};




class CH264Repacker : public CPatternRepacker
{
public:
	CH264Repacker(VOSTREAMPARSELISTENER* listener);
	virtual ~CH264Repacker();
	virtual void OnPESHead(ESConfig* escfg, PESPacket* packet);
	virtual void OnPESData(ESConfig* escfg, uint8* pData, uint32 cbData);
    virtual void Reset();

private:
    void InitVideoParser();
    void UInitVideoParser();
    VO_U32    GetFrame(ESConfig* escfg);
    VO_S64    GetTimeStamp(VO_U32 ulFrameLength);
    void      PurgeFrame();


    VO_VOID*   m_pVideoParser;
    VO_HANDLE  m_VideoParserHandle;
    VO_U32     m_ulIndexHeader;
    VO_U32     m_ulIndexTailer;
    VO_S64     m_aTimeStamp[MAX_FRAME_COUNT_IN_CACHE];
	VO_U32     m_aPesLength[MAX_FRAME_COUNT_IN_CACHE];
    VO_BYTE*   m_pWorkBuffer;
    VO_U32     m_ulMaxSize;
    VO_U32     m_ulCurSize;


	//

};

class CADTSAACRepacker : public CPatternRepacker
{
public:
	CADTSAACRepacker(VOSTREAMPARSELISTENER* listener,uint32 pattern, uint32 mask, int patternSize);
	virtual ~CADTSAACRepacker();

protected:
	virtual bool IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId);
	virtual void Reset();

private:
	uint8* m_pHeaderCacheBuffer;
	uint32 m_nCntCacheBuffe;
	int64 m_FrameLength;
};

class CM2TSLPCMRepacker : public CPatternRepacker
{
public:
	CM2TSLPCMRepacker(VOSTREAMPARSELISTENER* listener,uint32 pattern, uint32 mask, int patternSize,uint32 PacketSize,uint32 FrameSize);
	virtual ~CM2TSLPCMRepacker();

protected:
	virtual bool IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId);

private:
	int64 m_FrameLength;
	int64 m_PacketSize;
};

class CLATMRepacker : public CPatternRepacker
{
public:
	CLATMRepacker(VOSTREAMPARSELISTENER* listener);
	virtual ~CLATMRepacker();

protected:
	virtual bool IsNewFrame(uint8* pStart, uint8* pEnd, int countFrame,int nStreamId);

private:
	int m_frameSize;
};


#endif //_REPACKER_H_