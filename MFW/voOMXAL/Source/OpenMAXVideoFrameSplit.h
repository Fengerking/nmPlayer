#ifndef __OPEN_MAX_VIDEO_FRAME_SPLIT_H__
#define __OPEN_MAX_VIDEO_FRAME_SPLIT_H__
#include "voType.h"

class CVideoFrameSplit
{
public:
    CVideoFrameSplit();
    ~CVideoFrameSplit();

    VO_U32 InputFrameInfo(VO_BYTE*  pData, VO_U32 ulDataLength, VO_U64 ullTimeStamp);
    VO_U32 GetFrameInfo(VO_BYTE**  pDataOutput, VO_U32* pulDataLength, VO_U64* pullTimeStamp);
    VO_U32 ResetContext();
    

private:
    VO_VOID RemovePadding(VO_BYTE*  pData, VO_U32 ulDataLength, VO_U32*  pDataOutputLength);
private:
    VO_BYTE*    m_pWorkBuffer;
    VO_BYTE*    m_pOutputBuffer;
	VO_U32      m_ulOutputBufferLength;    
    VO_U32      m_ulWorkBufferMaxLength;
    VO_U32      m_ulCurrentLength;
    VO_U32      m_ulFrameCount;
    
    VO_U64      m_aTimeStampArray[128];
    VO_U32      m_ulCurrentTimeStampFrontIndex;
	VO_U32      m_ulCurrentTimeStampTailIndex;
};


#endif
