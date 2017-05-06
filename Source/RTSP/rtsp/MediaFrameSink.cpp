#include <stdio.h>
#include "network.h"

#include "utility.h"
#include "MediaFrameSink.h"
#include "MediaStream.h"
#include "RTSPMediaStreamSyncEngine.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CMemBlock::CMemBlock()
: m_memBlock(NULL)
, m_readPointer(NULL)
, m_writePointer(NULL)
, m_totalMemSize(0)
, m_memSufficient(false)
{
}
	
CMemBlock::~CMemBlock()
{
	Deallocate();
}

bool CMemBlock::Allocate(unsigned long size)
{
	if(m_memBlock != NULL)
	{
		Deallocate();
	}

	m_memBlock = new unsigned char[size];TRACE_NEW("CMemBlock::Allocate m_memBlock",m_memBlock);
	if(m_memBlock == NULL)
		return false;

	m_readPointer = m_memBlock;
	m_writePointer = m_memBlock;
	m_totalMemSize = size;
	m_memSufficient = true;

	return true;
}

void CMemBlock::Deallocate()
{
	SAFE_DELETE_ARRAY(m_memBlock);
	m_readPointer = NULL;
	m_writePointer = NULL;
	m_totalMemSize = 0;
	m_memSufficient = false;
}

unsigned long CMemBlock::Read(void * buffer, unsigned long size)
{
	if(AvailDataSize() < size)
	{
		size = AvailDataSize();
	}

	memcpy(buffer, m_readPointer, size);
	m_readPointer += size;
	return size;
}

unsigned long CMemBlock::Write(const void * buffer, unsigned long size)
{
	if(AvailMemSize() < size)
	{
		size = AvailMemSize();
	}

	memcpy(m_writePointer, buffer, size);
	m_writePointer += size;
	return size;
}

unsigned long CMemBlock::Peek(void * buffer, unsigned long size)
{
	if(AvailDataSize() < size)
	{
		size = AvailDataSize();
	}

	memcpy(buffer, m_readPointer, size);
	return size;
}

unsigned long CMemBlock::Skip(unsigned long size)
{
	if(AvailDataSize() < size)
	{
		size = AvailDataSize();
	}

	m_readPointer += size;
	return size;
}

void CMemBlock::Clear()
{
	m_readPointer = m_memBlock;
	m_writePointer = m_memBlock; 
	m_memSufficient = true;
}

unsigned long CMemBlock::AvailMemSize() 
{
	unsigned long availMemSize = m_memBlock + m_totalMemSize - m_writePointer; 
	return availMemSize; 
}

unsigned long CMemBlock::AvailDataSize() 
{ 
	unsigned long availDataSize = m_writePointer - m_readPointer; 
	return availDataSize; 
}

void CMemBlock::SetReadPointer(unsigned char * readPointer)
{
	if(readPointer > m_writePointer)
		readPointer = m_writePointer;

	m_readPointer = readPointer; 
} 



CMediaFrameSink::CMediaFrameSink(CMediaStream* stream):
m_stream(stream),
m_totalValidMediaFrameCount(0),
m_timePerVideoFrame(0)
{
	//InitializeCriticalSection(m_criticalSec);
	m_criticalSec=VOCPFactory::CreateOneMutex();TRACE_NEW("m_criticalSec1",m_criticalSec);
	m_mediaFrameIndex = 0;
	m_totalMediaFrameCount = 0;
	m_totalFrameSize = 0;
	m_previousBufferedTime = 0;
	m_totalTimeInterval = 0;
	m_lastOutPutTime = 0;

	memset(&m_lastMediaFrame, 0, sizeof(CMediaFrame));
}

CMediaFrameSink::~CMediaFrameSink()
{
	_List_memBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	m_listMemBlock.clear();
	VOCPFactory::DeleteOneMutex(m_criticalSec);TRACE_DELETE(m_criticalSec);
	m_criticalSec = NULL;
	//DeleteCriticalSection(m_criticalSec);
}
int	CMediaFrameSink::AppendNullPack(unsigned char * frameData, int frameSize,long curTime,int flag,int seq)
{
	return 0;
}
void CMediaFrameSink::SinkMediaFrame(unsigned char * frameData, int frameSize, unsigned long startTime, unsigned long endTime,  bool isIntraFrame,int flag)
{
	CAutoCriticalSec autoLock(m_criticalSec);
	static int firstTimeStamp = 0;
	CRTSPMediaStreamSyncEngine * pRTSPMediaStreamSyncEngine = CRTSPMediaStreamSyncEngine::CreateRTSPMediaStreamSyncEngine();
	//guarantee the starttime is calculated if the response of play is parsed
	if(pRTSPMediaStreamSyncEngine->GetPlayResponse()&&(flag&SINK_NEED_RECALCULATE_TS))
	{
		startTime = m_stream->CalculateNPT(startTime,endTime);
		if(startTime == INVALID_TIMESTAMP)
			startTime= m_lastMediaFrame.startTime;
		flag&=~ SINK_NEED_RECALCULATE_TS;
	}
	if(CUserOptions::UserOptions.m_nFirstIFrame)
	{
		if(CUserOptions::UserOptions.m_nSinkFlushed==1&&m_stream->IsVideoStream()&& (!m_stream->IsVideoH263()))
		{
			if(!isIntraFrame)
			{
				SLOG2(LL_RTP_ERR,"videoRTP.txt","WaitforIframe: curTS=%lu,size=%d\n",startTime,frameSize);
				return;
			}
			else
				CUserOptions::UserOptions.m_nSinkFlushed=0;
		}
	}

	if (CUserOptions::UserOptions.m_isLiveStream && CUserOptions::UserOptions.m_minimumLatency && (m_totalMediaFrameCount>18))
	{
		if (m_stream->IsVideoStream())
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"videoBuffer.txt", "****************do not buffer frame since totalMediaFrameCount more than 18****************\n");	
		}
		else
		{
			CLog::Log.MakeLog(LL_SOCKET_ERR,"audioBuffer.txt", "****************do not buffer frame since totalMediaFrameCount more than 18****************\n");
		}
		
		return;
	}
	
	//calculate the time per video frame
	if(m_stream->IsVideoStream()&&m_timePerVideoFrame==0&&m_lastMediaFrame.index>50)
	{
		float average=m_lastMediaFrame.startTime*1.0/m_lastMediaFrame.index;
		float curInterval=startTime-m_lastMediaFrame.startTime;
		
		if(abs(average-curInterval)<50)
			m_timePerVideoFrame=curInterval;
		else
			m_timePerVideoFrame=average;
		CUserOptions::UserOptions.outInfo.clip_AvgFrameRate =int((1000+m_timePerVideoFrame/2)/m_timePerVideoFrame+0.5);
		sprintf(CLog::formatString,"average time per frame=%f,curInterval=%f,fps=%d\n",average,curInterval,CUserOptions::UserOptions.outInfo.clip_AvgFrameRate);
		CLog::Log.MakeLog(LL_RTP_ERR,"config.txt",CLog::formatString);
	}

	CUserOptions::UserOptions.outInfo.clip_DownloadedSize+=frameSize;
	do
	{
		CMemBlock * memBlock = NULL;
		_List_memBlock_T::iterator iter;
		for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
		{
			if((*iter)->IsMemSufficient())
			{
				memBlock = *iter;
				break;
			}
		}

		if(memBlock == NULL)
		{
			memBlock = new CMemBlock();TRACE_NEW("CMediaFrameSink::SinkMediaFrame memBlock",memBlock);
			if(memBlock == NULL)
				break;

			if(!memBlock->Allocate(MEM_BLOCK_SIZE))
				break;

			m_listMemBlock.push_back(memBlock);
		}

		if(memBlock->AvailMemSize() < (unsigned long)(sizeof(CMediaFrame) + frameSize))
		{
			memBlock->MemDeficient();
			continue;
		}
		
		m_lastMediaFrame.frameSize = frameSize;
		m_lastMediaFrame.index = m_mediaFrameIndex++;
		int tmpTime = m_lastMediaFrame.startTime;
		m_lastMediaFrame.startTime = startTime;
		m_lastMediaFrame.seqNum = (endTime)&0x0000ffff;//here,the endTime stores the seqNum of the frame
		m_lastMediaFrame.isIntraFrame = isIntraFrame;
		m_lastMediaFrame.flag		  = flag;
		m_lastMediaFrame.localTime = timeGetTime();
		m_totalFrameSize+=frameSize;
		++m_totalMediaFrameCount;
		++m_totalValidMediaFrameCount;

		memBlock->Write(&m_lastMediaFrame, sizeof(CMediaFrame));
		if((flag&SINK_NEED_RECALCULATE_TS))//if the time needs recalculation,do not set the time,just keep it
			m_lastMediaFrame.startTime = tmpTime;
		memBlock->Write(frameData, frameSize);

		static int cnt = 0;

		if(!cnt)
		{
			if(!firstTimeStamp)
			{
				firstTimeStamp = m_lastMediaFrame.localTime;
			}
			else
			{
				m_totalTimeInterval = m_lastMediaFrame.localTime - firstTimeStamp;
				if(m_totalTimeInterval > (CUserOptions::UserOptions.m_nTimeMarker*1000)) 
				{
					NotifyEvent(VO_EVENT_TIMEMARKER, 0);
					cnt++;
				}
			}
		}

		if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
		{
			char* format = CLog::formatString;

			sprintf(format,"Sink:%ld(%d):TS=%lu,flag=%d,lastout=%ld,seq=%lu,key=%d,size=%d\n",m_lastMediaFrame.index,m_totalMediaFrameCount,startTime,flag,m_lastOutPutTime,m_lastMediaFrame.seqNum,
					isIntraFrame,frameSize);
			if(m_stream->IsVideoStream())
			{
				CLog::Log.MakeLog(LL_RTP_ERR,"videoFrame.txt",format);
			}
			else if (m_stream->IsAudioStream())
			{
				CLog::Log.MakeLog(LL_RTP_ERR,"audioFrame.txt",format);
			}
		}
		return;

	}while(1);
}


void CMediaFrameSink::FlushMediaFrame(unsigned long startTime)
{
	CAutoCriticalSec autoLock(m_criticalSec);

	_List_memBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		(*iter)->Clear();
	}
	m_mediaFrameIndex = 0;
	m_totalMediaFrameCount = 0;
	m_totalValidMediaFrameCount = 0;
	m_totalFrameSize = 0;
	m_lastOutPutTime = startTime;

	memset(&m_lastMediaFrame, 0, sizeof(CMediaFrame));
	CLog::Log.MakeLog(LL_SOCKET_ERR,"rtsp.txt","Flush one buffer\n");
}

void	CMediaFrameSink::CalculateNPTAfterPlayResponse()
{
	CAutoCriticalSec autoLock(m_criticalSec);
	int discardFrameNum=0;
	if(m_stream)
	{
		int frameCount=0;
		_List_memBlock_T::iterator iter;
		CMediaFrame mediaFrame;
		for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
		{
			CMemBlock * memBlock = *iter;
			unsigned char * readPointerCopy = memBlock->ReadPointer();
			for(;;)
			{
				if(memBlock->AvailDataSize() < sizeof(CMediaFrame))
					break;

				
				memBlock->Peek(&mediaFrame, sizeof(CMediaFrame));
				if(memBlock->AvailDataSize() < mediaFrame.frameSize)
				{
#if TRACE_FLOW_BUFFER
					CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","###3There is no enough data in Buffer\n");
#endif
					break;
				}
				
				
				if(mediaFrame.flag&SINK_NEED_RECALCULATE_TS)
				{
					if(m_stream->IsValidFrameBySeqNum(mediaFrame.seqNum))//mediaFrame.seqNum>=seqNumInRTPInfo)
					{
						//recalculate the ts and 
						//It is must to use memcpy,rather than store in-place because of the data-alignment on ARM
						//CMediaFrame *newframe=(CMediaFrame *)(memBlock->ReadPointer());
						mediaFrame.index-=discardFrameNum;
						mediaFrame.startTime = m_stream->CalculateNPT(mediaFrame.startTime,mediaFrame.seqNum);

						mediaFrame.flag&=~(SINK_NEED_RECALCULATE_TS);
						memcpy(memBlock->ReadPointer(),&mediaFrame,sizeof(CMediaFrame));
						++frameCount;
						if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
						{
							char* format = CLog::formatString;

							sprintf(format,"idx=%ld(%ld-%d),TS=%lu,seq=%lu\n",mediaFrame.index,mediaFrame.index+discardFrameNum,
								discardFrameNum,mediaFrame.startTime,mediaFrame.seqNum);
							if(m_stream->IsVideoStream())
							{
								CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus2.txt",format);
							}
							else if (m_stream->IsAudioStream())
							{
								CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus2.txt",format);
							}
						}
					}
					else
					{
						if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
						{
							char* format = CLog::formatString;

							sprintf(format,"d:idx=%ld,TS=%lu,seq=%lu\n",mediaFrame.index,
								mediaFrame.startTime,mediaFrame.seqNum);
							if(m_stream->IsVideoStream())
							{
								CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus2.txt",format);
							}
							else if (m_stream->IsAudioStream())
							{
								CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus2.txt",format);
							}
						}
						m_totalValidMediaFrameCount--;
						mediaFrame.index = -1;//disable the frame
						mediaFrame.flag = mediaFrame.startTime= 0;
						memcpy(memBlock->ReadPointer(),&mediaFrame,sizeof(CMediaFrame));
						discardFrameNum++;
						
					}
					
				}
				
				memBlock->Skip(sizeof(CMediaFrame) + mediaFrame.frameSize);
			}
			memBlock->SetReadPointer(readPointerCopy);
		}
		m_mediaFrameIndex-=discardFrameNum;

		if(frameCount>0)
		{
			memcpy(&m_lastMediaFrame,&mediaFrame,sizeof(CMediaFrame));
			if (CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
			{
				char* format = CLog::formatString;

				sprintf(format,"NeedRecalculate:number=%u\nlastFrame:idx=%ld,time=%lu,frames=%d\n",
					frameCount,
					m_lastMediaFrame.index
					,m_lastMediaFrame.startTime,m_totalMediaFrameCount);
				if(m_stream->IsVideoStream())
				{
					CLog::Log.MakeLog(LL_VIDEO_TIME,"videoDataStatus.txt",format);
				}
				else if (m_stream->IsAudioStream())
				{
					CLog::Log.MakeLog(LL_AUDIO_TIME,"audioDataStatus.txt",format);
				}
			}
			
		}
	}
	
}

VOFILEREADRETURNCODE CMediaFrameSink::GetMediaFrameByIndex(VOSAMPLEINFO * pSampleInfo)
{
	CAutoCriticalSec autoLock(m_criticalSec);
	
	do
	{
		if(m_listMemBlock.empty()||m_totalMediaFrameCount<=0)
		{
			//sprintf(CLog::formatString,"###fail ix=%d,a=%d\n",pSampleInfo->uIndex,m_totalMediaFrameCount);
			//CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt",CLog::formatString);
			break;
		}
		if(pSampleInfo->uIndex > m_mediaFrameIndex)
		{
			CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","###1Frame required  is beyond the latest Frame in Buffer\n");
			break;
		}

		CMemBlock * memBlock = m_listMemBlock.front();
		if(memBlock->AvailDataSize() <= 0 && !memBlock->IsMemSufficient())
		{
			memBlock->Clear();
			m_listMemBlock.pop_front();
			m_listMemBlock.push_back(memBlock);
			continue;
		}

		if(memBlock->AvailDataSize() < sizeof(CMediaFrame))////impossible actually
		{
#if TRACE_FLOW_BUFFER
			CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","###2There is no enough memory in Buffer\n");
#endif
			break;
		}

		CMediaFrame mediaFrame;
		memBlock->Peek(&mediaFrame, sizeof(CMediaFrame));
		if(memBlock->AvailDataSize() < mediaFrame.frameSize)//impossible actually
		{
#if TRACE_FLOW_BUFFER
			CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","###3There is no enough data in Buffer\n");
#endif
			break;
		}
		if(mediaFrame.flag&SINK_NEED_RECALCULATE_TS)//the following frames are needed to be recalculated
		{
			break;
		}
		memBlock->Skip(sizeof(CMediaFrame));

		
		if(pSampleInfo->uIndex == (unsigned long)mediaFrame.index)
		{
			pSampleInfo->uSize = memBlock->Read(pSampleInfo->pBuffer, mediaFrame.frameSize);
			pSampleInfo->uTime = mediaFrame.startTime;
			pSampleInfo->uDuration = 1;//mediaFrame.endTime - mediaFrame.startTime;
			pSampleInfo->uSize |= mediaFrame.isIntraFrame ? 0x80000000 : 0x0;
			//m_getIndex = mediaFrame.index;
			--m_totalMediaFrameCount;
			m_totalValidMediaFrameCount--;

			m_totalFrameSize-=mediaFrame.frameSize;
			m_lastOutPutTime = mediaFrame.startTime;

		}
		else
		{
			memBlock->Skip(mediaFrame.frameSize);
			--m_totalMediaFrameCount;
			if(mediaFrame.index!=-1)
				m_totalValidMediaFrameCount--;
			m_totalFrameSize-=mediaFrame.frameSize;
			continue;
		}

		return VORC_FILEREAD_OK;

	}while(1);
#if TRACE_FLOW_BUFFER
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		char* format = CLog::formatString;
		sprintf(format,"@@@@there is not matched frame,FrameCount=%d,meidaInex=%d,HighestFrameIndex=%d\n",m_totalMediaFrameCount,pSampleInfo->uIndex,m_mediaFrameIndex);
		CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt",format);
	}
#endif
	return VORC_FILEREAD_NEED_RETRY;
}

 int CMediaFrameSink::GetFirstFrameInBuffer(CMediaFrame* frame)  
{
	CAutoCriticalSec autoLock(m_criticalSec);
	_List_memBlock_T::iterator iter;
	iter=m_listMemBlock.begin();
	if(*iter&&m_mediaFrameIndex)
	{
		CMemBlock * memBlock = *iter;
		if(memBlock->AvailDataSize() > sizeof(CMediaFrame))
		{
			memcpy(frame,memBlock->ReadPointer(),sizeof(CMediaFrame));
		}
	}
	return 1;
}
VOFILEREADRETURNCODE CMediaFrameSink::GetMediaFrameInfoByIndex(VOSAMPLEINFO * pSampleInfo)
{
	CAutoCriticalSec autoLock(m_criticalSec);
	//return VORC_FILEREAD_NEED_RETRY;
	if(pSampleInfo->uIndex > m_mediaFrameIndex)
	{
		CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","$$$$1Frame required  is beyond the latest Frame in Buffer\n");
		return VORC_FILEREAD_NEED_RETRY;
	}

	_List_memBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		CMemBlock * memBlock = *iter;
		unsigned char * readPointerCopy = memBlock->ReadPointer();
		for(;;)
		{
			if(memBlock->AvailDataSize() < sizeof(CMediaFrame))
			{
				//CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt","$$$$2There is no enough memory in Buffer\n");
				break;
			}

			CMediaFrame mediaFrame;
			memBlock->Peek(&mediaFrame, sizeof(CMediaFrame));
			if(pSampleInfo->uIndex == (unsigned long)mediaFrame.index)
			{
				memBlock->SetReadPointer(readPointerCopy);

				pSampleInfo->uSize = mediaFrame.frameSize;
				pSampleInfo->uTime = mediaFrame.startTime;
				pSampleInfo->uDuration = 1;//mediaFrame.endTime - mediaFrame.startTime;
				return VORC_FILEREAD_OK;
			}
			else if(pSampleInfo->uIndex < (unsigned long)mediaFrame.index)
			{
				memBlock->SetReadPointer(readPointerCopy);
				return VORC_FILEREAD_NEED_RETRY;
			}
		
			memBlock->Skip(sizeof(CMediaFrame) + mediaFrame.frameSize);
		}
		memBlock->SetReadPointer(readPointerCopy);
	}
	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		char* format = CLog::formatString;
		sprintf(format,"@@@@there is not matched frame,FrameCoumt=%d,meidaInex=%lu,HighestFrameIndex=%lu\n",m_totalMediaFrameCount,pSampleInfo->uIndex,m_mediaFrameIndex);
		CLog::Log.MakeLog(LL_BUF_STATUS,"buffer.txt",format);
	}
	return VORC_FILEREAD_NEED_RETRY;
}
#define TRACE_KEY_FRAME 0
int CMediaFrameSink::GetNextKeyFrame(int nIndex, int nDirectionFlag)
{
	CAutoCriticalSec autoLock(m_criticalSec);

	if(nDirectionFlag != 1)
		return 0;
#if TRACE_KEY_FRAME
	sprintf(CLog::formatString,"GetKey:%d,%d ",nIndex,m_mediaFrameIndex);
	CLog::Log.MakeLog(LL_RTSP_ERR,"keyframe.txt",CLog::formatString);
#endif//
	if((unsigned long)nIndex > m_mediaFrameIndex)
		return 0;

	_List_memBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		CMemBlock * memBlock = *iter;
		unsigned char * readPointerCopy = memBlock->ReadPointer();
		for(;;)
		{
			if(memBlock->AvailDataSize() < sizeof(CMediaFrame))
				break;

			CMediaFrame mediaFrame;
			memBlock->Peek(&mediaFrame, sizeof(CMediaFrame));
			if(mediaFrame.isIntraFrame)
			{
				if(mediaFrame.index >= nIndex)
				{
					memBlock->SetReadPointer(readPointerCopy);
#if TRACE_KEY_FRAME
					sprintf(CLog::formatString,"key=%d \n",mediaFrame.index);
					CLog::Log.MakeLog(LL_RTSP_ERR,"keyframe.txt",CLog::formatString);
#endif//
					return mediaFrame.index;
				}
			}
		
			memBlock->Skip(sizeof(CMediaFrame) + mediaFrame.frameSize);
		}
		memBlock->SetReadPointer(readPointerCopy);
	}
#ifndef BRCM_WMPP
	return -1;
#else//BRCM_WMPP
	return 0;
#endif//BRCM_WMPP
}

long CMediaFrameSink::BufferLocalTime()
{
	return m_totalTimeInterval;
}
/*
long CMediaFrameSink::BufferLocalTime()
{
	CAutoCriticalSec autoLock(m_criticalSec);

	_List_memBlock_T::iterator iter;
	for(iter=m_listMemBlock.begin(); iter!=m_listMemBlock.end(); ++iter)
	{
		CMemBlock * memBlock = *iter;
		if(memBlock->AvailDataSize() < sizeof(CMediaFrame))
			continue;

		CMediaFrame mediaFrame;
		memBlock->Peek(&mediaFrame, sizeof(CMediaFrame));
		
		int bufferedTime=m_lastMediaFrame.localTime - mediaFrame.localTime;
		return bufferedTime;
	}
	return 1000;
}*/
unsigned long CMediaFrameSink::HaveBufferedTime(bool flag)
{
	CAutoCriticalSec autoLock(m_criticalSec);
	int bufferedTime = 0;
	unsigned long referTime = 0;

	if(HaveSinkedMediaFrameCountEx()==0)
		return 0;
	
	bufferedTime = m_lastMediaFrame.startTime - m_lastOutPutTime;
	
	if(m_stream->IsAudioStream())
	{
		referTime = ((float)HaveSinkedMediaFrameCount()/(float)m_stream->GetFrameRate())*1000;

		if(abs(bufferedTime - referTime) > 1000)
		{
			sprintf(CLog::formatString,"invalid audio buffer time %d(%lu-%ld), change to refer time %lu ((%d/%d)*1000)\n",
				bufferedTime,m_lastMediaFrame.startTime,m_lastOutPutTime,referTime,HaveSinkedMediaFrameCount(),m_stream->GetFrameRate());
		
			CLog::Log.MakeLog(LL_RTSP_ERR,"audioBufferTime.txt",CLog::formatString);
			bufferedTime = referTime;
		}
	}
	if(flag)
		bufferedTime+=m_stream->GetAntiJitterBufferTime();

	if(CUserOptions::UserOptions.m_bMakeLog>=LOG_RTP)
	{
		if(m_stream->IsAudioStream())
			SLOG2(LL_RTSP_ERR,"audioBufferTime.txt","v=%d, time=%d\n",m_stream->IsVideoStream(),bufferedTime)
		else
			SLOG2(LL_RTSP_ERR,"videoBufferTime.txt","v=%d, time=%d\n",m_stream->IsVideoStream(),bufferedTime)
	}

	return bufferedTime;
}

int CMediaFrameSink::HaveSinkedMediaFrameCount()
{
	return m_totalMediaFrameCount;
}

unsigned long CMediaFrameSink::HaveSinkedMediaFrameCountEx()
{
	return m_totalValidMediaFrameCount;
}



