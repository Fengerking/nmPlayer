#ifndef __MEDIAFRAMESINK_H__
#define __MEDIAFRAMESINK_H__
#include "network.h"
#include "filebase.h" 
#include "AutoCriticalSec.h"
#include "list_T.h"
#define MEM_BLOCK_SIZE (1024*1024)
#define MERGE_FRAME 2

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CMemBlock
{
public:
	CMemBlock();
	~CMemBlock();

public:
	bool Allocate(unsigned long size);
	void Deallocate();

	unsigned long Read(void * buffer, unsigned long size);
	unsigned long Write(const void * buffer, unsigned long size);
	unsigned long Peek(void * buffer, unsigned long size);
	unsigned long Skip(unsigned long size);

	void Clear();

public:
	unsigned long  AvailMemSize();
	unsigned long  AvailDataSize();

	void MemDeficient() { m_memSufficient = false; }
	bool IsMemSufficient() { return m_memSufficient; }

	unsigned char * ReadPointer() { return m_readPointer; }
	unsigned char * WritePointer() { return m_writePointer; }
	void	ForwardWritePointer(unsigned long size){m_writePointer+=size;}
	void SetReadPointer(unsigned char * readPointer);
	void	CalculateNPTAfterPlayResponse();

public:
	unsigned char * m_memBlock;
	unsigned char * m_readPointer;
	unsigned char * m_writePointer;
	unsigned long   m_totalMemSize;
	bool            m_memSufficient;
};

enum{
	SINK_NEED_RECALCULATE_TS = 1,
};
struct CMediaFrame
{
	long 			index;
	unsigned long 	frameSize;
	unsigned long 	startTime;
	unsigned long 	seqNum;
	bool 			isIntraFrame;
	long			flag;
	long 			localTime;
};


class CMediaStream;
/**
\brief the RTP media sample buffer manager

1,transform the  media sample from RTP parser to the final media sample format
2,manage the buffer
3,maintain the buffer info
*/
class CMediaFrameSink
{
protected:
	typedef list_T<CMemBlock *, allocator<CMemBlock *, 10> > _List_memBlock_T;
	CMediaStream* m_stream;
	unsigned long m_totalValidMediaFrameCount;
public:
	CMediaFrameSink(CMediaStream* stream);
	virtual ~CMediaFrameSink();

public:
	/**sink the rtp frame from media stream*/
	virtual void 			SinkMediaFrame(unsigned char * frameData, int frameSize, unsigned long startTime, unsigned long endTime, bool isIntraFrame,int flag=0);

	/**flush the buffer,reset the media index*/	
	virtual void 			FlushMediaFrame(unsigned long starTime);
public:
	/**Get the frame by index,the first index is 0 after flush*/	
	VOFILEREADRETURNCODE 	GetMediaFrameByIndex(VOSAMPLEINFO * pSampleInfo);
	/**Get the frame info by index,the first index is 0 after flush*/
	VOFILEREADRETURNCODE 	GetMediaFrameInfoByIndex(VOSAMPLEINFO * pSampleInfo);
	/**Get the index of the next key frame*/
	int                  	GetNextKeyFrame(int nIndex, int nDirectionFlag);
public:
	/**Get the time difference of the last frame and the first frame*/
	unsigned long 			HaveBufferedTime(bool flag=false);
	long 					BufferLocalTime();
	/**Get the frame number in buffer*/
	int 					HaveSinkedMediaFrameCount();
protected:
	IVOMutex*			m_criticalSec;
	_List_memBlock_T	m_listMemBlock;

protected:
	unsigned long           m_mediaFrameIndex;
	int                     m_totalMediaFrameCount;
	int						m_totalFrameSize;
	unsigned long			m_previousBufferedTime;
	unsigned long			m_totalTimeInterval;   //this is used to count how long since we received first frame
	float					m_timePerVideoFrame;
	struct CMediaFrame      m_lastMediaFrame;  
	long					m_lastOutPutTime;

	int		AppendNullPack(unsigned char * frameData, int frameSize,long curTime,int flag,int seq);
public:
	unsigned int			GetLastTimeInBuffer(){return m_lastMediaFrame.startTime;}
	unsigned int			GetLastIndexInBuffer(){return m_lastMediaFrame.index;}
	int						GetReceivedFrames(){return m_totalMediaFrameCount;}
	int						GetFirstFrameInBuffer(CMediaFrame* frame);  
	void					CalculateNPTAfterPlayResponse();
	int						HaveBufferdFrameSize(){return m_totalFrameSize;}
	unsigned long			HaveSinkedMediaFrameCountEx();

};

#ifdef _VONAMESPACE
}
#endif

#endif //__MEDIAFRAMESINK_H__

