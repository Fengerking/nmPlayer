
#include "CMP4Writer.h"
#include "CMp4WriterStream.h"
#include "CAnalyseAAC.h"
#include "CAnalyseQCELP.h"
#include "CAnalyseAMR.h"
#include "CAnalyseH263.h"
#include "CAnalyseH264.h"
#include "CAnalyseH265.h"
#include "CAnalyseMPEG4.h"

#define AVCHEADER_H264STREAM 0    //H264 sequence header, 0x000001...;
#define AVCHEADER_PARAMETERSETS 1 // parameters sets: len(2 bytes) + sequence/picture parameter;
#define AVCHEADER_CONFIGRECORD 2  //AVCDecoderConfigurationRecord (14496-15);

#define LOG_TAG "CMP4Writer"
#include "voLog.h"

//using namespace ISOMW;

#define VOFILEWRITETIMEDIVISOR 1000

CMP4Writer::CMP4Writer()
: mbDetectSH(VO_TRUE)
, mbIsFirstVideo(VO_TRUE)
, mbIsFirstAudio(VO_TRUE)
, mnTimeOffset(0)
, mpVAnalyse(NULL)
, mpAAnalyse(NULL)
, mpStream(NULL)
, mpMoveWriter(NULL)
, mpVTrackWriter(NULL)
, mpATrackWriter(NULL)
, mESID(0)
, mbClosed(VO_FALSE)
, mbMoveWirterOpen(VO_FALSE)
, mllALastTime(0)
, mllVLastTime(0)
, mpVExtData (NULL)
, mnVExtDataLen(0)
, mpAExtData (NULL)
, mnAExtDataLen(0)
, mbIndexFirst(VO_FALSE)
, mnMoovSize(0)
, mbIntraDetectIF(VO_TRUE)
, mnVideoChunkSize(0)
, mnAudioChunkSize(0)
, mnVideoChunkDuration(0)
, mnAudioChunkDuration(0)
, mnVideoSampleCount(0)
, mnAudioSampleCount(0)
{
	memset(&mFileSource , 0 , sizeof(VO_FILE_SOURCE ));
	memset(&mOpenParam , 0 , sizeof(VO_SINK_OPENPARAM ));
	memset(&mVideoFormat , 0 , sizeof(VO_VIDEO_FORMAT ));
	memset(&mAudioFormat , 0 , sizeof(VO_AUDIO_FORMAT  ));
#ifdef DUMP_VIDEO
	mhFile = fopen("/sdcard/Video.dat" , "wb");
#endif
}

CMP4Writer::~CMP4Writer()
{
#ifdef DUMP_VIDEO
	if(mhFile != NULL)
	{
		fclose(mhFile);
		mhFile = NULL;
	}
#endif

	if(mpAExtData != NULL)
	{
		delete [] mpAExtData;
		mpAExtData = NULL;
	}

	if(mpVExtData != NULL)
	{
		delete [] mpVExtData;
		mpVExtData = NULL;
	}

	if(mFileSource.nFlag == VO_FILE_TYPE_NAME && mFileSource.pSource != NULL)
	{
		VO_TCHAR * pStr = (VO_TCHAR *)mFileSource.pSource;
		delete [] pStr;
		mFileSource.pSource = NULL;
	}

	Close();
}

VO_U32 CMP4Writer::Open(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam)
{
	if(pParam == NULL || pFileSource == NULL)
		return VO_ERR_FAILED;

	//if (pParam->nFlag != 1)
	//{
	//	return VO_ERR_INVALID_ARG;
	//}

	if(mFileSource.pSource == NULL)
		CopyOpenParam(pFileSource , pParam);

	
	mpStream = new CMp4WriterStream((VO_FILE_OPERATOR *)pParam->pSinkOP);
	//mpStream = new CMp4WriterStream(NULL);
	if(mpStream == NULL)
	{
		VOLOGE("create CMp4WriterStream return VO_ERR_OUTOF_MEMORY");
		return VO_ERR_OUTOF_MEMORY;
	}

	if(!mpStream->Open(pFileSource))
	{
		//VOLOGI("Open file return false")
		return VO_ERR_SINK_OPENFAIL;
	}

	mVideoCodec = pParam->nVideoCoding;
	mAudioCodec = pParam->nAudioCoding;
	mbIsFirstVideo = VO_TRUE;
	mbIsFirstAudio = VO_TRUE;
	mnAudioSampleCount = 0;
	mnVideoSampleCount = 0;

	mbDetectSH = VO_TRUE;

	mbClosed = VO_FALSE;

	return VO_ERR_NONE;
}

VO_U32 CMP4Writer::Close()
{
	if(mbClosed == VO_TRUE)
		return VO_ERR_NONE;

	if(mpATrackWriter != NULL)
	{
		mpATrackWriter->Close();
		mpATrackWriter = NULL;
	}

	if(mpVTrackWriter != NULL)
	{
		mpVTrackWriter->Close();
		mpVTrackWriter = NULL;
	}

	if(mpMoveWriter != NULL)
	{
		mpMoveWriter->Close();
		if(mnMoovSize == 0)
		{
			mnMoovSize = mpMoveWriter->GetMoovSize();
			VO_U32 nTempSize = mnMoovSize / 10;
			if(nTempSize < 1024)
				nTempSize = 1024;

			mnMoovSize+= nTempSize;
		}
		
		delete mpMoveWriter;
		mpMoveWriter = NULL;
	}
	

	if(mpStream != NULL)
	{
		mpStream->Close();
		delete mpStream;
		mpStream = NULL;
	}

	if(mpAAnalyse != NULL)
	{
		delete mpAAnalyse;
		mpAAnalyse = NULL;
	}

	if(mpVAnalyse != NULL)
	{
		delete mpVAnalyse;
		mpVAnalyse = NULL;
	}

	mbClosed = VO_TRUE;
	mbMoveWirterOpen = VO_FALSE;

	mllVLastTime = 0;
	mllALastTime = 0;

	return VO_ERR_NONE;
}


VO_U32 CMP4Writer::AddSample( VO_SINK_SAMPLE * pSample)
{
	if(pSample == NULL)
	{
		return VO_ERR_FAILED;
	}

	//VOLOGI("track %d   samplesize %08d  sampletimestamp %08lld" , pSample->nAV , pSample->Size & 0x7FFFFFFF , pSample->Time);

	if(pSample->Size == 0)
		return VO_ERR_NONE;

	voCAutoLock lockit(&mmxWriter);
	if(mbMoveWirterOpen == VO_FALSE)
	{
		if(mpMoveWriter == NULL)
		{
			mpMoveWriter = new MovieWriter;
			mpMoveWriter->SetMoovSize(mnMoovSize);
		}

		if(!mpMoveWriter->Open(FileFormatMP4, mpStream, VOFILEWRITETIMEDIVISOR))
		{
			delete mpStream;
			mpStream = NULL;

			return VO_ERR_SINK_OPENFAIL;
		}

#ifdef _WINDOWS
		VO_TCHAR strDir[MAX_PATH];
		vostrcpy(strDir , (VO_TCHAR *)mFileSource.pSource);
		VO_TCHAR * strTemp = vostrrchr(strDir , '\\');
		int nIndex = 0;
		if(strTemp != NULL)
			nIndex = strTemp - strDir;
		strDir[nIndex] = '\0';
		mpMoveWriter->SetTempDirectory(strDir);
#endif //_WINDOWS

		mbMoveWirterOpen = VO_TRUE;
	}

	bool bIsSync = false;
	if(pSample->Size & 0x80000000)
	{
		mbIntraDetectIF = VO_FALSE;
		bIsSync = true;
	}
	
	pSample->Size = pSample->Size & 0x7FFFFFFF;

	if(mbIsFirstAudio && mbIsFirstVideo)
	{
		if(pSample->nAV == 0 || (pSample->nAV == 1 && pSample->Size > 50) )
			mnTimeOffset = pSample->Time;
	}

	pSample->Time -= mnTimeOffset;
	if(pSample->Time < 0)
	{
		pSample->Time = 0;
	}

	
	if(pSample->nAV == 0) //audio sample
	{
		if(mbIsFirstAudio)
		{
			if(HandleFASample(pSample->Buffer , pSample->Size) == VO_FALSE)
			{
				return VO_ERR_NONE;
			}

			mbIsFirstAudio = VO_FALSE;

			if(pSample->Size < 3)
				return VO_ERR_NONE;
		}

		


		if(mpATrackWriter == NULL)
			return VO_ERR_FAILED;

		//VO_PBYTE pB = pSample->Buffer;
		//VOLOGI("&&&@@@ Audio Timestap ; %d , data len : %d  Data: %x %x %x %x %x %x %x %x" ,(VO_U32)pSample->Time , pSample->Size , pB[0] , pB[1], pB[2] , pB[3], pB[4] , pB[5], pB[6] , pB[7] );
		if(mllALastTime > pSample->Time)
		{
			pSample->Time = mllALastTime + 5;
		}

		mllALastTime = pSample->Time;

		mpAAnalyse->TrimSample( &pSample->Buffer , pSample->Size);

		//pB = pSample->Buffer;
		//VOLOGI("&&&222 Audio Timestap ; %d , data len : %d  Data: %x %x %x %x %x %x %x %x" , (VO_U32)pSample->Time , pSample->Size , pB[0] , pB[1], pB[2] , pB[3], pB[4] , pB[5], pB[6] , pB[7] );
		BufferSample BufSample;
		BufSample.buffer = pSample->Buffer;
		BufSample.size = pSample->Size;
		BufSample.sync = true;
		BufSample.time = (uint32)pSample->Time;
		

		if(mnAudioSampleCount == 0)
		{
			BufSample.time = 0;
		}

		mnAudioSampleCount ++;

		mpATrackWriter->AddSample(&BufSample);
		return VO_ERR_NONE;
	}
	else if(pSample->nAV == 1) //Video sample
	{
#ifdef DUMP_VIDEO
		if(mhFile != NULL)
		{
			fwrite(&(pSample->Size) , 4 , 1 , mhFile);
			fwrite(pSample->Buffer , pSample->Size , 1 , mhFile);
		}
#endif
		//VO_PBYTE p = pSample->Buffer;
		//VOLOGI("&&&@@@ Video timestap : %lld , data size : %d   %x %x %x %x %x %x %x %x" , pSample->Time , pSample->Size , p[0] , p[1]  , p[2] , p[3] , p[4] , p[5] , p[6] , p[7]);
		if(mbDetectSH)
		{
			if(HandleFVSample(pSample->Buffer , pSample->Size) == VO_FALSE)
			{
				//VOLOGI("HandleFVSample return Failed");
				return VO_ERR_NONE;
			}

			mbIsFirstVideo = VO_FALSE;

			mbDetectSH = VO_FALSE;
			VO_U32 nHeadLen = 0;
			mpVAnalyse->GetSequenceHeader(nHeadLen);
			if(nHeadLen >= pSample->Size)
			{
				//VOLOGI("GetSequence Len = %d , pSamplesize = %d " , (int)nHeadLen , (int)pSample->Size);
				return VO_ERR_NONE;
			}
		}

		if(mpVTrackWriter == NULL || mpVAnalyse == NULL)
			return VO_ERR_FAILED;

		if(mbIntraDetectIF == VO_TRUE)
		{
			bIsSync = false;
			if(mpVAnalyse->IsKeyFrame(pSample->Buffer , pSample->Size))
			{
				bIsSync = true;
			}
		}
	

		if(mllVLastTime > pSample->Time)
		{
			pSample->Time = mllALastTime + 20;
		}

		mllVLastTime = pSample->Time;

	
		VO_PBYTE  pDataBuf = pSample->Buffer;
		VO_U32    nDataLen = pSample->Size;
	
		mpVAnalyse->TrimSample(&pDataBuf , nDataLen);

		BufferSample BufSample;
		BufSample.buffer = pDataBuf;
		BufSample.size = nDataLen;
		BufSample.sync = bIsSync;
		BufSample.time = (uint32)pSample->Time ;

		if(mnVideoSampleCount ==0 )
		{
			BufSample.time = 0;	
		}

		mnVideoSampleCount ++;

		//uint8 *p = BufSample.buffer;
		//VOLOGI("&&&@@@ Video timestap : %08d , data size : %08d   %x %x %x %x %x %x %x %x" , BufSample.time ,BufSample.size  , p[0] , p[1]  , p[2] , p[3] , p[4] , p[5] , p[6] , p[7]);
		mpVTrackWriter->AddSample(&BufSample);
		return VO_ERR_NONE;
	}

	return VO_ERR_FAILED;
}


VO_U32 CMP4Writer::SetParam(VO_U32 uID, VO_PTR pParam)
{
	switch (uID)
	{
	case VO_PID_COMMON_LOGFUNC:
		{
		//	VO_LOG_PRINT_CB * pVologCB = (VO_LOG_PRINT_CB *)pParam;
		//	vologInit (pVologCB->pUserData, pVologCB->fCallBack);
			break;
		}
	case VO_PID_AUDIO_FORMAT:
		{
			memcpy(&mAudioFormat , pParam , sizeof(VO_AUDIO_FORMAT));
			return VO_ERR_NONE;
		}
	case VO_PID_VIDEO_FORMAT:
		{
			memcpy(&mVideoFormat , pParam , sizeof(VO_VIDEO_FORMAT));
			return VO_ERR_NONE;
		}
	case VO_PID_SINK_EXTDATA:
		{
			VO_SINK_EXTENSION *pExtData = (VO_SINK_EXTENSION *)pParam;
			if(pExtData == NULL)
				return VO_ERR_FAILED;

			if(pExtData->nAV == 0)
			{
				//VOLOGI("@@@ Audio Ext data len = %d " , pExtData->Size);
				if(pExtData->Size > 0)
				{
					mnAExtDataLen = pExtData->Size;
					mpAExtData = new VO_BYTE[mnAExtDataLen];
					memcpy(mpAExtData , pExtData->Buffer , mnAExtDataLen);
				}
			}
			else
			{
				//VOLOGI("@@@ Audio Ext data len = %d " , pExtData->Size);
				if(pExtData->Size > 0)
				{
					mnVExtDataLen = pExtData->Size;
					mpVExtData = new VO_BYTE[mnVExtDataLen];
					memcpy(mpVExtData , pExtData->Buffer , mnVExtDataLen);
				}
			}
			return VO_ERR_NONE;
		}
	case VO_PID_SINK_INDEXONLY:
		{
			mbIndexFirst = *((VO_BOOL *)pParam);
			bool bValue = mbIndexFirst == VO_TRUE ? true : false;

			if(!bValue)
			{
				ResetWriter();
			}

			if(mpVTrackWriter != NULL)
			{
				mpVTrackWriter->SetIndexIn(bValue);
			}

			if(mpATrackWriter != NULL)
			{
				mpATrackWriter->SetIndexIn(bValue); 
			}

			return VO_ERR_NONE;
		}
	case VO_PID_SINK_FRAME_INFO:
		{
			VO_SINK_FramesInfo *pFI = (VO_SINK_FramesInfo *)pParam;
			VO_U32 nSize = 116;
			if(pFI->nVFrames > 0)
			{
				nSize += 566;
				if(pFI->nVSyncFrames == 0)
				{
					nSize += pFI->nVFrames * 10;
				}
				else
				{
					nSize += pFI->nVFrames * 8;
					nSize += pFI->nVSyncFrames * 4;
				}
			}

			if(pFI->nAFrames > 0)
			{
				nSize += 434;
				nSize += pFI->nAFrames * 8;
			}

			nSize += pFI->nVFrames * 4;
			nSize += pFI->nAFrames * 4;

			mnMoovSize = nSize;

			if(mpMoveWriter != NULL)
			{
				mpMoveWriter->SetMoovSize(mnMoovSize);
			}

			return VO_ERR_NONE;
		}
	case VO_PID_SINK_INTRA_DETECT_IFRAME:
		mbIntraDetectIF = *(VO_BOOL *)pParam;
		return VO_ERR_NONE;
	case VO_PID_SINK_VIDEO_CHUNK_SIZE:
		mnVideoChunkSize = *(VO_U32 *)pParam;
		return VO_ERR_NONE;
	case VO_PID_SINK_AUDIO_CHUNK_SIZE:
		mnAudioChunkSize = *(VO_U32 *)pParam;
		return VO_ERR_NONE;
	case VO_PID_SINK_VIDEO_CHUNK_DURATION:
		mnVideoChunkDuration = *(VO_U32 *)pParam;
		if(mpVTrackWriter != NULL)
		{
			mpVTrackWriter->SetChunkDuration(mnVideoChunkDuration);
		}
		return VO_ERR_NONE;
	case VO_PID_SINK_AUDIO_CHUNK_DURATION:
		mnAudioChunkDuration = *(VO_U32 *)pParam;
		if(mpATrackWriter != NULL)
		{
			mpATrackWriter->SetChunkDuration(mnAudioChunkDuration);
		}
		return VO_ERR_NONE;
	case VO_PID_SINK_FLUSH:
		Flush();
		return VO_ERR_NONE;
	}

	
	return VO_ERR_WRONG_PARAM_ID;
}


VO_U32 CMP4Writer::GetParam(VO_U32 uID, VO_PTR pParam)
{
	return VO_ERR_NONE;
}

VO_BOOL	CMP4Writer::Flush()
{
	voCAutoLock lockit(&mmxWriter);
	if(mpATrackWriter != NULL)
		mpATrackWriter->Flush();

	if(mpVTrackWriter != NULL)
		mpVTrackWriter->Flush();

	return VO_TRUE;
}

VO_BOOL	CMP4Writer::HandleFVSample(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(mpVAnalyse == NULL)
	{
		if(mVideoCodec == VO_VIDEO_CodingMPEG4)
		{
			mpVAnalyse = new CAnalyseMPEG4();
		}
		else if(mVideoCodec == VO_VIDEO_CodingH264)
		{
			mpVAnalyse = new CAnalyseH264();
		}
		else if(mVideoCodec == VO_VIDEO_CodingH265)
		{
			mpVAnalyse = new CAnalyseH265();
		}
		else if(mVideoCodec == VO_VIDEO_CodingH263)
		{
			mpVAnalyse = new CAnalyseH263();
		}
	}

	if(mpVAnalyse == NULL)
		return VO_FALSE;

	mpVAnalyse->SetFormat(&mVideoFormat);

	if(mnVExtDataLen > 0)
	{
		return AddVideoTrack();
	}
	else
	{
		VO_BOOL nRC = mpVAnalyse->AnalyseData(pData , nDataLen);
		if(nRC == VO_TRUE)
		{
			return AddVideoTrack();
		}

		return nRC;
	}
}

VO_BOOL CMP4Writer::HandleFASample(VO_BYTE *pData , VO_U32 nDataLen)
{
	if(mpAAnalyse == NULL)
	{
		if(mAudioCodec == VO_AUDIO_CodingAMRNB)
		{
			mpAAnalyse = new CAnalyseAMR();
		}
		else if(mAudioCodec == VO_AUDIO_CodingAMRWB || mAudioCodec == VO_AUDIO_CodingAMRWBP)
		{
			mpAAnalyse = new CAnalyseAMR(false);
		}
		else if(mAudioCodec == VO_AUDIO_CodingAAC)
		{
			mpAAnalyse = new CAnalyseAAC();
		}
		else if(mAudioCodec == VO_AUDIO_CodingQCELP13)
		{
			mpAAnalyse = new CAnalyseQCELP();
		}
	}

	if(mpAAnalyse == NULL)
	{
		return VO_FALSE;
	}

	VO_BOOL nRC = mpAAnalyse->AnalyseData(pData , nDataLen);
	if(nRC == VO_TRUE)
	{
		if(mAudioFormat.SampleRate == 0)
		{
			VO_WAVEFORMATEX *pWF = mpAAnalyse->GetAudioInfo();
			mAudioFormat.Channels = pWF->nChannels;
			mAudioFormat.SampleBits = pWF->wBitsPerSample;
			mAudioFormat.SampleRate = pWF->nSamplesPerSec;
		}

		AddAudioTrack();
	}

	return nRC;
}

VO_BOOL	CMP4Writer::AddVideoTrack()
{
	if(mpVAnalyse == NULL)
		return VO_FALSE;

	mpVTrackWriter = mpMoveWriter->AddTrack();
	if(mpVTrackWriter == NULL)
	{
		return VO_FALSE;
	}

	if(mnVideoChunkDuration != 0)
	{
		mpVTrackWriter->SetChunkDuration(mnVideoChunkDuration);
	}

	if(mbIndexFirst == VO_TRUE)
		mpVTrackWriter->SetIndexIn(true);

	Track* track = mpVTrackWriter->GetTrack();

	track->SetType(Track::ttVideo);
	track->SetHandlerType(FOURCC2_vide);
	//track->SetSamplesPerChunk(20);
	track->SetSampleType(FOURCC2_vide); //defaul sample type

	track->SetWidth((uint16)mVideoFormat.Width);
	track->SetHeight((uint16)mVideoFormat.Height);

	if(mnVideoChunkSize == 0)
	{
		mnVideoChunkSize = mVideoFormat.Width * mVideoFormat.Height;
		mnVideoChunkSize = (mnVideoChunkSize / (64 * 1024) + 1) * 64 * 1024;

		if(mnVideoChunkSize > 2 * 1024 * 1024)
			mnVideoChunkSize = 2 * 1024 * 1024;
	}
	
	mpVTrackWriter->SetMaxChunkSize(mnVideoChunkSize);

	if(mVideoCodec == VO_VIDEO_CodingMPEG4)
	{
		SetFormatMPEG4();
	}
	else if(mVideoCodec == VO_VIDEO_CodingH264)
	{
		SetFormatH264();
	}
	else if(mVideoCodec == VO_VIDEO_CodingH265)
	{
		SetFormatH265();
	}
	else if(mVideoCodec == VO_VIDEO_CodingH263)
	{
		SetFormatH263();
	}
	
	return VO_TRUE;
}

VO_BOOL CMP4Writer::AddAudioTrack()
{
	if(mpAAnalyse == NULL)
		return VO_FALSE;

	mpATrackWriter = mpMoveWriter->AddTrack();
	if(mpATrackWriter == NULL)
	{
		return VO_FALSE;
	}

	if(mnAudioChunkDuration != 0)
	{
		mpATrackWriter->SetChunkDuration(mnAudioChunkDuration);
	}

	if(mbIndexFirst == VO_TRUE)
		mpATrackWriter->SetIndexIn(true);

	Track* track = mpATrackWriter->GetTrack();

	track->SetType(Track::ttAudio);
	track->SetHandlerType(FOURCC2_soun);
	//track->SetSamplesPerChunk(30);
	track->SetSampleType(FOURCC2_soun);  //default sample type

	track->SetSampleRate(mAudioFormat.SampleRate);
	track->SetChannelCount((uint16)mAudioFormat.Channels);
	track->SetSampleBits((uint16)mAudioFormat.SampleBits);

	if(mnAudioChunkSize == 0)
		mnAudioChunkSize = 16 * 1024;

	mpATrackWriter->SetMaxChunkSize(mnAudioChunkSize);

	if(mAudioCodec == VO_AUDIO_CodingAMRNB)
	{
		SetFormatAMR();
	}
	else if(mAudioCodec == VO_AUDIO_CodingAMRWB || mAudioCodec == VO_AUDIO_CodingAMRWBP)
	{
		SetFormatAMRWB();
	}
	else if(mAudioCodec == VO_AUDIO_CodingQCELP13)
	{
		SetFormatQCELF();
	}
	else if(mAudioCodec == VO_AUDIO_CodingAAC)
	{
		SetFormatAAC();
	}

	return VO_TRUE;
}

VO_BOOL	CMP4Writer::SetFormatMPEG4()
{
	ESDescriptor* esd = new ESDescriptor();
	DecoderConfigDescriptor* dcd = new DecoderConfigDescriptor();
	SLConfigDescriptor* slcd = new SLConfigDescriptor();
	DecoderSpecificInfo* dsi = new DecoderSpecificInfo();
	dcd->AddChild(dsi);
	esd->AddChild(dcd);
	esd->AddChild(slcd);

	esd->ES_ID = ++mESID;

	//DecoderConfigDescriptor
	dcd->objectTypeIndication = 0x20;
	dcd->streamType = 4;
	dcd->upStream = 0;
	dcd->bufferSizeDB = 0; //81920;
	dcd->maxBitrate = 0; //512000; 
	dcd->avgBitrate = 0; //512000; 

	VO_U32  nDataLen = 0;
	VO_BYTE *pSeqData = NULL;

	if(mnVExtDataLen > 0)
	{
		nDataLen = mnVExtDataLen;
		pSeqData = mpVExtData;
	}
	else
		pSeqData = mpVAnalyse->GetSequenceHeader(nDataLen);

	//DecoderSpecificInfo
	dsi->SetBody(pSeqData, nDataLen);

	//ESDescriptor
	esd->UpdateSize();

	int descsize = esd->GetSize();
	uint8* descdata = new uint8[descsize];
	MemStream ms(descdata, descsize);
	WriterMSB w(&ms);

	esd->Dump(w);
	delete esd;

	return SetTrackFormat(mpVTrackWriter->GetTrack() , FOURCC2_mp4v, FOURCC2_esds, descsize, descdata);
}

VO_U32  CMP4Writer::CheckH264HeaderType(VO_U32 nLen , VO_BYTE *pData)
{
	VO_BYTE *p = pData;
	if(p[0] == 0x0 && p[1] == 0x0 && (p[2] == 0x1 || (p[2] == 0x0 && p[3] == 0x1)))
		return AVCHEADER_H264STREAM;

	VO_U32 size = BufToWord(pData);
	if(size > nLen - 2)
		return AVCHEADER_CONFIGRECORD;
	else if(size == nLen - 2)
		return AVCHEADER_PARAMETERSETS;

	return CheckH264HeaderType(nLen - size - 2 , pData - size - 2);
}

VO_BOOL CMP4Writer::SetFormatH264()
{
	/* 01 42 e0 0d ff e1 ...
	uint8 configurationVersion = 1;
	w.Write(configurationVersion);
	w.Write(AVCProfileIndication);
	w.Write(profile_compatibility);
	w.Write(AVCLevelIndication);
	w.Write(lengthSizeMinusOne);
	*/
	AVCDecoderConfigurationRecord cfg;
	cfg.AVCProfileIndication = 0x42;
	cfg.profile_compatibility = 0xe0;
	cfg.AVCLevelIndication = 0x0d;
	cfg.lengthSizeMinusOne = 0xff; // raw data, no size field

	int descsize = 0;
	uint8* descdata = NULL;

	//VO_U32  nDataLen = 0;
	//VO_BYTE *pSeqData = NULL;

	if(mnVExtDataLen <= 0 && mpVAnalyse != NULL)
	{
		VO_BYTE *pTemp = mpVAnalyse->GetSequenceHeader(mnVExtDataLen);
		mpVExtData = new VO_BYTE[mnVExtDataLen];
		memcpy(mpVExtData ,pTemp ,  mnVExtDataLen);
	}

	if(mnVExtDataLen > 0)
	{
		VO_U32 nType = CheckH264HeaderType(mnVExtDataLen , mpVExtData);

		if (nType == AVCHEADER_CONFIGRECORD)
		{
			descsize =  mnVExtDataLen;
			descdata = new uint8[descsize];
			memcpy(descdata, mpVExtData , descsize);
		}
		else
		{
			bool b = true;
			if (nType == AVCHEADER_PARAMETERSETS)
				b = cfg.InitParameterSetsFromAVCHeader(mpVExtData, mnVExtDataLen);
			else
				b = cfg.InitParameterSetsFromH264Stream(mpVExtData,mnVExtDataLen);
			if  (!b)
				return VO_FALSE;

			descsize = cfg.GetSize();
			descdata = new uint8[descsize];
			MemStream ms(descdata, descsize);
			WriterMSB w(&ms);
			b = cfg.Dump(w);
		}
	}

	return SetTrackFormat(mpVTrackWriter->GetTrack(), FOURCC2_avc1, FOURCC2_avcC, descsize, descdata);
}

VO_BOOL CMP4Writer::SetFormatH263()
{
	int descsize = sizeof(H263DecSpecStruc);
	uint8* descdata = new uint8[descsize];
	H263DecSpecStruc* p = (H263DecSpecStruc*) descdata;
	p->vendor = 0x766F766F; //'vovo'
	p->decoder_version = 0;
	p->H263_Level = 0x0a;
	p->H263_Profile = 0; //Baseline


	return SetTrackFormat(mpVTrackWriter->GetTrack() , FOURCC2_s263, FOURCC2_d263, descsize, descdata);
}

VO_BOOL CMP4Writer::SetFormatAMR()
{
	int descsize = 9;
	uint8* descdata = new uint8[descsize];

	AMRDecSpecStruc* p = (AMRDecSpecStruc*) descdata;
	p->vendor =  0x766F766F; //'vovo'
	p->decoder_version = 0x01;
	p->mode_set = 0x8000;
	p->mode_change_period = 0;
	p->frames_per_sample = 1;

	return SetTrackFormat(mpATrackWriter->GetTrack(), FOURCC2_samr, FOURCC2_damr, descsize, descdata);
}

VO_BOOL CMP4Writer::SetFormatAMRWB()
{
	int descsize = 9;
	uint8* descdata = new uint8[descsize];

	AMRDecSpecStruc* p = (AMRDecSpecStruc*) descdata;
	p->vendor =  0x766F766F; //'vovo'
	p->decoder_version = 0x01;
	p->mode_set = 0x8000;
	p->mode_change_period = 0;
	p->frames_per_sample = 1;

	return SetTrackFormat(mpATrackWriter->GetTrack(), FOURCC2_sawb, FOURCC2_damr, descsize, descdata);
}

VO_BOOL CMP4Writer::SetFormatQCELF()
{
	ESDescriptor*            esd = new ESDescriptor();
	DecoderConfigDescriptor* dcd = new DecoderConfigDescriptor();
	SLConfigDescriptor*     slcd = new SLConfigDescriptor();
	DecoderSpecificInfo*     dsi = new DecoderSpecificInfo();
	dcd->AddChild(dsi);
	esd->AddChild(dcd);
	esd->AddChild(slcd);

	esd->ES_ID = ++mESID;

	//DecoderConfigDescriptor
	dcd->objectTypeIndication = 0xE1;
	dcd->streamType   = 5;
	dcd->upStream     = 0;
	dcd->bufferSizeDB = 0;
	dcd->maxBitrate   = 14000; 
	dcd->avgBitrate   = 14000; 
	dcd->bufferSizeDB = 4096;

	//3GPP2 File Formats for Multimedia Services.pdf, p22
	//RFC 3625
	const int dsisize = 162;
	uint8 dsidata[dsisize];
	uint8* p = dsidata;
	memset(p, 0, dsisize);
	memcpy(p, "QLCMfmt ", 8);
	p += 8;
	*(uint32*)p = dsisize - 12;
	p += 4;
	memcpy(p, "\x01\x00\x41\x6D\x7F\x5E\x15\xB1\xD0\x11\xBA\x91\x00\x80\x5F\xB4\xB9\x7E\x01\x00", 20);
	p += 20;
	strcpy((char*)p, "Qcelp 14K fixed rate");

	//DecoderSpecificInfo
	dsi->SetBody(dsidata, dsisize);

	//ESDescriptor
	esd->UpdateSize();

	int descsize = esd->GetSize();
	uint8* descdata = new uint8[descsize];
	MemStream ms(descdata, descsize);
	WriterMSB w(&ms);
	/*bool b = */esd->Dump(w);
	delete esd;

	return SetTrackFormat(mpATrackWriter->GetTrack(), FOURCC2_mp4a, FOURCC2_esds, descsize, descdata);
}

VO_BOOL CMP4Writer::SetFormatAAC()
{
	ESDescriptor* esd = new ESDescriptor();
	DecoderConfigDescriptor* dcd = new DecoderConfigDescriptor();
	SLConfigDescriptor* slcd = new SLConfigDescriptor();
	DecoderSpecificInfo* dsi = new DecoderSpecificInfo();
	dcd->AddChild(dsi);
	esd->AddChild(dcd);
	esd->AddChild(slcd);

	esd->ES_ID = ++mESID;

	//DecoderConfigDescriptor
	dcd->objectTypeIndication = 0x40;
	dcd->streamType = 5;
	dcd->upStream = 0;
	dcd->bufferSizeDB = 0;
	dcd->maxBitrate = 0; 
	dcd->avgBitrate = 0; 
	dcd->bufferSizeDB = 0; //81920;

	const int sample_rates[] =
	{
		96000, 88200, 64000, 48000, 
		44100, 32000,24000, 22050, 
		16000, 12000, 11025, 8000,
		0, 0, 0, 0
	};

	int nRateIndex = 0;

	for( nRateIndex = 0; nRateIndex < 16 ; nRateIndex++ )
	{
		if(mAudioFormat.SampleRate == sample_rates[nRateIndex])
			break;
	}

	if(nRateIndex > 12)
	{
		if(mnAExtDataLen == 2)
		{
			nRateIndex = ((mpAExtData[0] & 0x7) << 1)  + (mpAExtData[1] >> 7);
			if(nRateIndex > 12)
				return VO_FALSE;
		}
		else
		{
			return VO_FALSE;
		}
	}

	//DecoderSpecificInfo
	VO_BYTE aacInfo[2];

	aacInfo[0] = 0x10;
	aacInfo[0] = aacInfo[0] + (nRateIndex >> 1);
	aacInfo[1] = (VO_BYTE)(nRateIndex << 7);
	aacInfo[1] += (VO_BYTE)(mAudioFormat.Channels << 3);

	dsi->SetBody(aacInfo, 2);

	//ESDescriptor
	esd->UpdateSize();

	int descsize = esd->GetSize();
	uint8* descdata = new uint8[descsize];
	MemStream ms(descdata, descsize);
	WriterMSB w(&ms);
	/*bool b = */esd->Dump(w);
	delete esd;

	return SetTrackFormat(mpATrackWriter->GetTrack(), FOURCC2_mp4a, FOURCC2_esds, descsize, descdata);
}

VO_BOOL CMP4Writer:: SetTrackFormat(Track* track, VO_U32 sampletype, VO_U32 desctype, VO_U32 descsize, VO_BYTE* descdata)
{
	track->SetSampleType(sampletype);
	if (descsize)
	{
		track->SetDescriptor(desctype, descdata, descsize);
		delete[] descdata;
	}
	return VO_TRUE;
}

VO_BOOL CMP4Writer::CopyOpenParam(VO_FILE_SOURCE* pFileSource , VO_SINK_OPENPARAM * pParam)
{
	memcpy(&mFileSource , pFileSource , sizeof(mFileSource));
	if(mFileSource.nFlag == VO_FILE_TYPE_NAME)
	{
		VO_S32 nLen = vostrlen((VO_TCHAR *)pFileSource->pSource);
		mFileSource.pSource = new VO_TCHAR[nLen + 1];
		vostrcpy((VO_TCHAR *)mFileSource.pSource , (VO_TCHAR *)pFileSource->pSource);
	}

	if(mFileSource.nReserve != 0)
	{
		VO_S32 nLen = vostrlen((VO_TCHAR *)pFileSource->nReserve);
		mFileSource.nReserve = (VO_U32) new VO_TCHAR[nLen + 1];
		vostrcpy((VO_TCHAR *)mFileSource.nReserve , (VO_TCHAR *)pFileSource->nReserve);
	}

	memcpy(&mOpenParam , pParam , sizeof(mOpenParam));
	return VO_TRUE;
}

VO_BOOL CMP4Writer::ResetWriter()
{
	Close();
	Open(&mFileSource , &mOpenParam);

	return VO_TRUE;
}

VO_BOOL CMP4Writer::SetFormatH265()
{
	HEVCDecoderConfigurationRecord cfg;

	cfg.profile_space = 0; //TODO
	cfg.profile_idc = 0; //TODO;
	cfg.constraint_indicator_flags = 0; //TODO;
	cfg.level_idc = 0; //TODO;
	cfg.profile_compatibility_indications = 0; //TODO;
	cfg.chromaFormat = 0; //TODO;
	cfg.bitDepthLumaMinus8 = 0; //TODO;
	cfg.bitDepthChromaMinus8 = 0; //TODO;
	cfg.avgFrameRate = 0; //TODO;
	cfg.constantFrameRate = 0; //TODO;
	cfg.numTemporalLayers = 0; //TODO;
	cfg.lengthSizeMinusOne = 0xff; // raw data, no size field

	int descsize = 0;
	uint8* descdata = NULL;

	//AnalyseH265 is not ready, we don't use it

	if(mnVExtDataLen > 0)
	{
		bool b = cfg.InitArrayFromVOHeadData(mpVExtData, mnVExtDataLen);
		if  (!b)
			return VO_FALSE;

		descsize = cfg.GetSize();
		descdata = new uint8[descsize];
		MemStream ms(descdata, descsize);
		WriterMSB w(&ms);
		b = cfg.Dump(w);
	}

	return SetTrackFormat(mpVTrackWriter->GetTrack(), FOURCC2_hvc1, FOURCC2_hvcC, descsize, descdata);
}

