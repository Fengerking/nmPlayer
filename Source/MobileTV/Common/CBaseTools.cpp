#include "cmnMemory.h"
#include "CBaseTools.h"
#include "CAnalyseAAC.h"
#include "CAnalyseH264.h"
#include "CAnalyseMPEG4.h"


CBaseTools::CBaseTools(void)
{
}

CBaseTools::~CBaseTools(void)
{
}

VO_BOOL CBaseTools::IsKeyFrame(VO_U32 inCodecType, VO_BYTE* inBuf, VO_U32 inLen)
{
	VO_BOOL pass = VO_FALSE;

	if (inCodecType == VO_VIDEO_CodingH264)
	{
		CAnalyseH264 parser;
		return parser.IsKeyFrame(inBuf, inLen);
	}
	else if (inCodecType == VO_AUDIO_CodingAAC)
	{
		// ADTS check
		CAnalyseAAC parser;
		return parser.IsKeyFrame(inBuf, inLen);
	}
	else if (inCodecType == VO_VIDEO_CodingMPEG4)
	{
		CAnalyseMPEG4 parser;
		return parser.IsKeyFrame(inBuf, inLen);
	}

	return pass?VO_TRUE:VO_FALSE;
}


// need add more format
VO_BOOL CBaseTools::GetHeadData(VO_U32 inCodecType, VO_BYTE* inBuf, VO_U32 inLen, VO_BOOL& outVideo, VO_BYTE** outHeadData, VO_U32& outHeadDataLen)
{
	VO_BOOL pass = VO_FALSE;

	if (inCodecType == VO_VIDEO_CodingH264)
	{
		CAnalyseH264 parser;
		
		if(parser.AnalyseData(inBuf, inLen))
		{
			pass = VO_TRUE;
			VO_BYTE* pHead	= parser.GetSequenceHeader(outHeadDataLen);

			VO_MEM_INFO vmi = {0};
			vmi.Size = outHeadDataLen;
			cmnMemAlloc(0, &vmi);
			*outHeadData	= (VO_BYTE*)vmi.VBuffer;
			
			cmnMemCopy(0, *outHeadData, pHead, outHeadDataLen);
			outVideo		= VO_TRUE;
		}
	}

	if (inCodecType == VO_AUDIO_CodingAAC)
	{
		// ADTS check
		CAnalyseAAC parser;

		if(parser.AnalyseData(inBuf, inLen))
		{
			pass = VO_TRUE;
			VO_BYTE* pHead	= parser.GetSequenceHeader(outHeadDataLen);

			VO_MEM_INFO vmi = {0};
			vmi.Size = outHeadDataLen;
			cmnMemAlloc(0, &vmi);
			*outHeadData	= (VO_BYTE*)vmi.VBuffer;
			
			cmnMemCopy(0, outHeadData, pHead, outHeadDataLen);
			outVideo		= VO_FALSE;
		}
	}


	if (inCodecType == VO_VIDEO_CodingMPEG4)
	{
		CAnalyseMPEG4 parser;

		if(parser.AnalyseData(inBuf, inLen))
		{
			pass = VO_TRUE;
			VO_BYTE* pHead	= parser.GetSequenceHeader(outHeadDataLen);

			VO_MEM_INFO vmi = {0};
			vmi.Size = outHeadDataLen;
			cmnMemAlloc(0, &vmi);
			*outHeadData	= (VO_BYTE*)vmi.VBuffer;

			cmnMemCopy(0, outHeadData, pHead, outHeadDataLen);
			outVideo		= VO_TRUE;
		}
	}

	return pass?VO_TRUE:VO_FALSE;
}
