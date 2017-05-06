
#include "voAdaptiveStreamingClassFactory.h"

#include "voAdaptiveStreamingFileParser_ts.h"
#include "voAdaptiveStreamingFileParser_aac.h"
#include "voAdaptiveStreamingFileParser_mp4.h"
#include "voAdaptiveStreamingFileParser_smth.h"
#include "voAdaptiveStreamingFileParser_unknow.h"
#include "voAdaptiveStreamingFileParser_Subtitle.h"

#include "voChannelItemThreadHLS.h"
#include "voChannelItemThreadDASH.h"
#include "voChannelItemThreadISS.h"

#include "CFileFormatCheck.h"
#include "voBufferIO.h"

#include "voFormatChecker.h"
#include "voToolUtility.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

VO_U32 CreateAdaptiveStreamingController( voAdaptiveStreamingController ** ppController )
{
	*ppController = new voAdaptiveStreamingController;

	return VO_RET_SOURCE2_OK;
}

VO_U32 DestroyAdaptiveStreamingController( voAdaptiveStreamingController * pController )
{
	if( pController )
		delete pController;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CreateChannelItemThread( VO_ADAPTIVESTREAMPARSER_STREAMTYPE StreamType , voChannelItemThread ** ppThread )
{
	if( StreamType == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_HLS )
		*ppThread = new voChannelItemThreadHLS;
	else if( StreamType == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS )
 		*ppThread = new voChannelItemThreadISS;
	else if( StreamType == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_DASH )
		*ppThread = new voChannelItemThreadDASH;
	else
		*ppThread = new voChannelItemThread;

	return VO_RET_SOURCE2_OK;
}

VO_U32 DestroyChannelItemThread( voChannelItemThread * pThread )
{
	if( pThread )
		delete pThread;

	return VO_RET_SOURCE2_OK;
}

VO_U32 CreateAdaptiveStreamingFileParser( voAdaptiveStreamingFileParser ** ppParser , VO_ADAPTIVESTREAMPARSER_STREAMTYPE StreamType , VO_FILE_FORMAT fFormat
											, VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp 
											, VO_TCHAR *pWorkPath, bool bForceCreate )
{
	FileFormat ff = FileFormat_Unknown;
	FileFormat oldff = FileFormat_Unknown;

	if( bForceCreate )
	{
		DestroyAdaptiveStreamingFileParser( *ppParser );
		*ppParser = 0;
	}

	if( *ppParser )
		oldff = (*ppParser)->GetFileFormat();

	if(StreamType == VO_ADAPTIVESTREAMPARSER_STREAMTYPE_ISS )
		ff = FileFormat_SMTH;
	else
	{
		switch( fFormat )
		{
		case VO_FILE_FFMOVIE_MP4:
			ff = FileFormat_MP4;
			break;
		case VO_FILE_FFAUDIO_AAC:
			ff = FileFormat_AAC;
			break;
		case VO_FILE_FFMOVIE_TS:
			ff = FileFormat_TS;
			break;
		case VO_FILE_FFAUDIO_MP3:
			ff = FileFormat_MP3;
			break;
		case VO_FILE_FFSUBTITLE_WEBVTT:
			ff = FileFormat_WEBVTT;
			break;
		case VO_FILE_FFSUBTITLE_TTML:
			ff = FileFormat_TTML;
			break;
		default:
			ff = FileFormat_Unknown;
			break;
		}
	}
	
	VODS_VOLOGI("FileFormat Checked:%d(old:%d)", ff, oldff);
	if(ff == FileFormat_Unknown)
		return VO_RET_SOURCE2_FORMATUNSUPPORT;

	if( ff == oldff )
	{
		if( *ppParser )
			(*ppParser)->SetFormatChange();
	}
	else
	{

		DestroyAdaptiveStreamingFileParser( *ppParser );

		switch( ff )
		{
		case FileFormat_TS:
			VODS_VOLOGI("New FileFormat_TS");
			*ppParser = new voAdaptiveStreamingFileParser_ts( pCallback , pLibOp ); 
			break;
		case FileFormat_AAC:
			VODS_VOLOGI("New FileFormat_AAC");
			*ppParser = new voAdaptiveStreamingFileParser_pa(pCallback, ff, pLibOp);
			break;
		case FileFormat_MP3:
			VODS_VOLOGI("New FileFormat_MP3");
			*ppParser = new voAdaptiveStreamingFileParser_pa(pCallback, ff, pLibOp);
			break;
		case FileFormat_MP4:
			VODS_VOLOGI("New FileFormat_MP4");
			*ppParser = new voAdaptiveStreamingFileParser_mp4( pCallback , pLibOp );
			break;
		case FileFormat_SMTH:
			VODS_VOLOGI("New FileFormat_SMTH");
			*ppParser = new voAdaptiveStreamingFileParser_smth( pCallback , pLibOp );
			break;
		case FileFormat_WEBVTT:
			VODS_VOLOGI("New FileFormat_WEBVTT");
			*ppParser = new voAdaptiveStreamingFileParser_Subtitle( pCallback, pLibOp );
			break;
		case FileFormat_TTML:
			VODS_VOLOGI("New FileFormat_TTML");
			*ppParser = new voAdaptiveStreamingFileParser_Subtitle( pCallback, pLibOp );
			break;
		default: 
			VODS_VOLOGI("New FileFormat_Unknown");
			*ppParser = NULL;
			break;
		}


		if( *ppParser )
		{	
			(*ppParser)->SetWorkPath( pWorkPath );
			VO_U32 ret = 0;
			ret = (*ppParser)->LoadParser() ;
			if(ret)
			{
				VOLOGE( "LoadParser Error. FileType:%d", ff );
				return ret;
			}
		}

	}

	if( *ppParser )
	{
		(*ppParser)->SetFileFormat( ff );
	}
	else
		return VO_RET_SOURCE2_FAIL;

	return VO_RET_SOURCE2_OK;
}

VO_U32 DestroyAdaptiveStreamingFileParser( voAdaptiveStreamingFileParser * pParser )
{
	if( pParser )
	{
		pParser->FreeParser();
		delete pParser;
	}

	return VO_RET_SOURCE2_OK;
}

VO_U32 CreateAdaptiveStreamingBuffer( voSourceBufferManager_AI ** ppBuffer , VO_S32 nBufferingTime , VO_S32 nMaxBufferTime , VO_S32 nStartBufferTime )
{
	*ppBuffer = new voSourceBufferManager_AI( nBufferingTime , nMaxBufferTime , nStartBufferTime );
	//*ppBuffer = new voSourceBufferManager( nBufferingTime , nMaxBufferTime , nStartBufferTime );

	return VO_RET_SOURCE2_OK;
}

VO_U32 DestroyAdaptiveStreamingBuffer( voSourceBufferManager_AI * pBuffer )
{
	if( pBuffer )
		delete pBuffer;

	return VO_RET_SOURCE2_OK;
}

#ifdef _VONAMESPACE
}
#endif