#include <stdlib.h>
#include <string.h>
#include "voVideoParser.h"
#include "voVP6Parser.h"
#include "voVP8Parser.h"
#include "voWMVParser.h"
#include "voMPEG4Parser.h"
#include "voMPEG2Parser.h"
#include "voH263Parser.h"
#include "voH264Parser.h"

VO_S32 VO_API yyGetVPSRFunc (VO_VIDEO_PARSERAPI * pParserHandle, VO_VIDEO_CODINGTYPE vType)
{
	if(!pParserHandle)
		return VO_ERR_INVALID_ARG;
	if(vType == VO_VIDEO_CodingVP6)
	{
		pParserHandle->Init = voVP6Init;
		pParserHandle->Process = voVP6Parser;
		pParserHandle->GetParam = voVP6GetInfo;
		pParserHandle->Uninit = voVP6Uninit;
	}
	else if(vType == VO_VIDEO_CodingVP8)
	{
		pParserHandle->Init = voVP8Init;
		pParserHandle->Process = voVP8Parser;
		pParserHandle->GetParam = voVP8GetInfo;
		pParserHandle->Uninit = voVP8Uninit;
	}
	else if(vType == VO_VIDEO_CodingWMV || vType == VO_VIDEO_CodingVC1)
	{
		pParserHandle->Init = voWMVVC1Init;
		pParserHandle->Process = voWMVVC1Parser;
		pParserHandle->GetParam = voWMVVC1GetInfo;
		pParserHandle->Uninit = voWMVVC1Uninit;
	}
	else if (vType == VO_VIDEO_CodingMPEG4)
	{
		pParserHandle->Init = voMPEG4Init;
		pParserHandle->Process = voMPEG4Parser;
		pParserHandle->GetParam = voMPEG4GetInfo;
		pParserHandle->Uninit = voMPEG4Uninit;
	}
	else if (vType == VO_VIDEO_CodingMPEG2)
	{
		pParserHandle->Init = voMPEG2Init;
		pParserHandle->Process = voMPEG2Parser;
		pParserHandle->GetParam = voMPEG2GetInfo;
		pParserHandle->Uninit = voMPEG2Uninit;
	}
	else if (vType == VO_VIDEO_CodingH263)
	{
		pParserHandle->Init = voH263Init;
		pParserHandle->Process = voH263Parser;
		pParserHandle->GetParam = voH263GetInfo;
		pParserHandle->Uninit = voH263Uninit;
	}
	else if(vType == VO_VIDEO_CodingH264)
	{
		pParserHandle->Init = voH264ParserInit;
		pParserHandle->SetParam = voH264ParserSetParam;
		pParserHandle->Process = voH264ParserProcess;
		pParserHandle->GetParam = voH264ParserGetInfo;
		pParserHandle->Uninit = voH264ParserUninit;
	}
	else
	{
		return VO_ERR_VIDEOPARSER_CODECID;
	}

	return VO_ERR_NONE;
}
