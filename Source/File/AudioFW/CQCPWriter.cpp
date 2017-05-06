//#include "CMp4WriterStream.h"
//#include "CLogFile.h"

#include "CQCPWriter.h"
#include "fCC2.h"

#define DEBUG

#if defined _LINUX && defined(__VOTT_ARM__) && defined DEBUG 
#define LOG_TAG "CQCPWriter"
#include <utils/Log.h>
#  define __D(fmt, args...) ({LOGD("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#  define __E(fmt, args...) ({LOGE("->%d: %s(): " fmt, __LINE__, __FUNCTION__, ## args);})
#else
#  define __D(fmt, args...) ()
#  define __E(fmt, args...) ()
#endif


static VO_U16 QCPPackedSize[16] = {1, 4, 8, 17, 35, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static VO_U16 EVCPackedSize[16] = {0, 3, 0, 11, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

#define IsEqualGUID(g1, g2) (!MemCompare(&(g1), &(g2), sizeof(g2)))
// {5E7F6D41-B115-11D0-BA91-00805FB4B97E}
static GUID GUID_CODEC_QCELP13 = {
	0x5E7F6D41,										 //Data1: l
	0xB115,											 //Data2: w1
	0x11D0,											 //Data3: w2
	{0xBA, 0x91, 0x00, 0x80, 0x5F, 0xB4, 0xB9, 0x7E} //Data4[8]
};

// {5E7F6D42-B115-11D0-BA91-00805FB4B97E}
static GUID GUID_CODEC_QCELP13_1 = {
	0x5E7F6D42,										 //Data1: l
	0xB115,											 //Data2: w1
	0x11D0,											 //Data3: w2
	{0xBA, 0x91, 0x00, 0x80, 0x5F, 0xB4, 0xB9, 0x7E} //Data4[8]
};

// {E689D48D-9076-46B5-91EF-736A5100CEB4}
static GUID GUID_CODEC_EVRC = {
	0xE689D48D,										 //Data1: l
	0x9076,											 //Data2: w1
	0x46B5,											 //Data3: w2
	{0x91, 0xEF, 0x73, 0x6A, 0x51, 0x00, 0xCE, 0xB4} //Data4[8]
};


CQCPWriter::CQCPWriter(VO_MEM_OPERATOR* pMemOp, VO_FILE_OPERATOR* pFileOp)
  : CvoBaseMemOpr(pMemOp)
  , CvoBaseFileOpr(pFileOp)
  , m_hFile(NULL)
  , mnTimeOffset(0)
  , m_nDataSize(0)
  , mnTotalFrames(0)
  , mnFrameSize(0)
  , mbIsCBR(VO_TRUE)
  , mAudioCodec(VO_AUDIO_CodingQCELP13)
  , mbIsFirstAudio(VO_TRUE)
{
	VO_AUDIO_FORMAT initAudioFormat = {
		8000,						//SampleRate;
		1,							//Channels; 
		16							//SampleBits;
	};

	QCP_FILE_TYPE initFileType = {
		FOURCC2_FFIR,						//szRIFF;
		0,							//nFileSize;
		FOURCC2_MCLQ						//szType;
	};

	QCP_FILE_FORMAT initFileFormat = {
		FOURCC2_tmf,						//szFormat;
		sizeof(initFileFormat)-2-8,	//nFormatSize;
		1,							//cMajor;
		0,							//cMinor;
		GUID_CODEC_QCELP13,			//gCodec;
		1,							//sVersion;
		"Qcelp 13K",				//szName[80];
		0,							//sAverageBPS;
		0,							//sPocketSize;
		160,						//sBlockSize;
		initAudioFormat.SampleRate, //sSampleRate;
		initAudioFormat.SampleBits, //sSampleSize;
		0,							//dwNumRate;
		{{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}},						//cRateMapEntry[8][2];
		{0, 0 ,0,0,0}							//dwReserve[5];
	};

	QCP_FILE_VRAT initFileVart = {
		FOURCC2_tarv,						//szType;
		8,							//nChunkSize;
		0,							//nFlag;
		0							//nPackets;	
	};

	QCP_FILE_CHUNK initFileChunk = {
		FOURCC2_atad,						//szChunk;
		0							//nChunkSize 
	};


	mAudioFormat = initAudioFormat;
	mFileType    = initFileType;
	mFileFormat  = initFileFormat;
	mFileVart    = initFileVart;
	mFileChunk   = initFileChunk;
	
	mnHeadSize = sizeof(mFileType) + (sizeof(mFileFormat) - 2) + sizeof(mFileVart) + sizeof(mFileChunk);
}

CQCPWriter::~CQCPWriter()
{
	Close();
}

VO_U32 CQCPWriter::Open(VO_FILE_SOURCE *pSource, VO_SINK_OPENPARAM * pParam)
{
	Close();

	mAudioCodec    = pParam->nAudioCoding;
	mbIsFirstAudio = VO_TRUE;
	m_nDataSize    = 0;
	mnTotalFrames  = 0;
	mnFrameSize    = 0;

	if(mAudioCodec == VO_AUDIO_CodingEVRC)
		memcpy(&mFileFormat.gCodec, &GUID_CODEC_EVRC, sizeof(GUID_CODEC_EVRC));

	if((m_hFile = FileOpen(pSource)) == NULL)
		return VO_ERR_SINK_OPENFAIL;
	
	WriteHeadData();
	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::Close()
{
	

	if(m_hFile == NULL)
		return 0;

	mFileType.nFileSize   = 4 + (sizeof(mFileFormat) - 2) + sizeof(mFileVart) + sizeof(mFileChunk) + m_nDataSize; // TODO:4?
	mFileChunk.nChunkSize = m_nDataSize;

	if(mbIsCBR)	{
		mFileVart.nFlag         = 0;
		mFileVart.nPackets      = mnTotalFrames;
		mFileFormat.sAverageBPS = mnFrameSize * 50 * 8;
		mFileFormat.sPocketSize = mnFrameSize;
	} else {
		mFileVart.nFlag         = 1;
		mFileVart.nPackets      = mnTotalFrames;
		mFileFormat.sAverageBPS = mnFrameSize * 50 * 8;
		mFileFormat.sPocketSize = 35;
		mFileFormat.dwNumRate   = 5;
		for(int i = 0 ; i < 5 ; i++) {
			mFileFormat.cRateMapEntry[i][0] = QCPPackedSize[4 - i] - 1;
			mFileFormat.cRateMapEntry[i][1] = 4 - i;
		}
	}

	WriteHeadData();
	FileClose(m_hFile);
	m_hFile = NULL;
	
	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::SetParam(VO_U32 uID, VO_PTR pParam)
{
	
	if(uID == VO_PID_AUDIO_FORMAT) { // must uID
		MemCopy(&mAudioFormat, pParam, sizeof(mAudioFormat));
		mFileFormat.sSampleRate = mAudioFormat.SampleRate;
		mFileFormat.sSampleSize = mAudioFormat.SampleBits;
	}
	
	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::GetParam(VO_U32 uID, VO_PTR pParam)
{
	if(uID == VO_PID_AUDIO_FORMAT) {
		MemCopy(pParam, &mAudioFormat, sizeof(mAudioFormat));
	}

	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::WriteHeadData (void)
{
	

	FileSeek(m_hFile, 0, VO_FILE_BEGIN);

	FileWrite (m_hFile, &mFileType,     sizeof(mFileType));

	FileWrite (m_hFile, &mFileFormat.szFormat,      sizeof(mFileFormat.szFormat));
	FileWrite (m_hFile, &mFileFormat.nFormatSize,   sizeof(mFileFormat.nFormatSize));
	FileWrite (m_hFile, &mFileFormat.cMajor,        sizeof(mFileFormat.cMajor));
	FileWrite (m_hFile, &mFileFormat.cMinor,        sizeof(mFileFormat.cMinor));
	FileWrite (m_hFile, &mFileFormat.gCodec,        sizeof(mFileFormat.gCodec));
	FileWrite (m_hFile, &mFileFormat.sVersion,      sizeof(mFileFormat.sVersion));
	FileWrite (m_hFile, &mFileFormat.szName,        sizeof(mFileFormat.szName));
	FileWrite (m_hFile, &mFileFormat.sAverageBPS,   sizeof(mFileFormat.sAverageBPS));
	FileWrite (m_hFile, &mFileFormat.sPocketSize,   sizeof(mFileFormat.sPocketSize));
	FileWrite (m_hFile, &mFileFormat.sBlockSize,    sizeof(mFileFormat.sBlockSize));
	FileWrite (m_hFile, &mFileFormat.sSampleRate,   sizeof(mFileFormat.sSampleRate));
	FileWrite (m_hFile, &mFileFormat.sSampleSize,   sizeof(mFileFormat.sSampleSize));
	FileWrite (m_hFile, &mFileFormat.dwNumRate,     sizeof(mFileFormat.dwNumRate));
	FileWrite (m_hFile, &mFileFormat.cRateMapEntry, sizeof(mFileFormat.cRateMapEntry));
	FileWrite (m_hFile, &mFileFormat.dwReserve,     sizeof(mFileFormat.dwReserve));

	FileWrite (m_hFile, &mFileVart,     sizeof(mFileVart));
	FileWrite (m_hFile, &mFileChunk,    sizeof(mFileChunk));

	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::CalculateFrames(VO_PBYTE pBuf , VO_U32 nBufLen)
{
	
	if(pBuf == NULL)
		return -1;

	VO_S32 nTempLen = nBufLen;
	while(nTempLen > 0) {
		//VO_S32 curFrameSize = GetFrameSize(pBuf[0]);
		VO_U32 curFrameSize = nBufLen;  // only 1 Frame
		
		if(mnFrameSize == 0) {
			mnFrameSize = nTempLen;
		} else if(mnFrameSize != curFrameSize) {
			mbIsCBR = VO_FALSE;
		}
		
		mnTotalFrames++;
		nTempLen -= curFrameSize;
		pBuf     += curFrameSize;
	}

	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::AddSample(VO_SINK_SAMPLE * pSample)
{
	
	if(pSample == NULL) return VO_ERR_FAILED;

	pSample->Size = pSample->Size & 0x7FFFFFFF;
	
	if(mbIsFirstAudio) {
		mbIsFirstAudio = VO_FALSE;
		mnTimeOffset   = pSample->Time;
		pSample->Time  = 0;
	} else {
		pSample->Time  = ((pSample->Time - mnTimeOffset) < 0) ? 0 : (pSample->Time - mnTimeOffset);
	}

	CalculateFrames(pSample->Buffer, pSample->Size);
	FileWrite(m_hFile, pSample->Buffer, pSample->Size);
	m_nDataSize += pSample->Size;
				
	return VO_ERR_NONE;
}

VO_U32 CQCPWriter::GetFileSize(void)
{
	return mnHeadSize + m_nDataSize;
}

VO_S32 CQCPWriter::GetFrameSize(VO_U8 nValue)
{
	if(IsEqualGUID(GUID_CODEC_QCELP13,   mFileFormat.gCodec) || 
	   IsEqualGUID(GUID_CODEC_QCELP13_1, mFileFormat.gCodec)) {

		if(nValue > 5) return -1;
		return QCPPackedSize[nValue];
	} else {
		if(nValue > 4) return -1;
		return EVCPackedSize[nValue];
	}
}



