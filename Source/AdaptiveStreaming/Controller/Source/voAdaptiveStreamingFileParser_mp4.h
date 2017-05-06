#ifndef __VOADAPTIVESTREAMINGFILEPARSER_MP4__
#define __VOADAPTIVESTREAMINGFILEPARSER_MP4__

#include "voAdaptiveStreamingFileParser.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "voLiveSource.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voAdaptiveStreamingFileParser_mp4
	:public voAdaptiveStreamingFileParser
{
public:
	voAdaptiveStreamingFileParser_mp4( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp = 0 );
	virtual ~voAdaptiveStreamingFileParser_mp4();

	virtual VO_U32 LoadParser();
	virtual VO_U32 FreeParser();
	
	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );

//	virtual VO_VOID SetFormatChange();

	virtual VO_SOURCE2_MEDIATYPE GetMeidaType();

//	virtual VO_U32 CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , VO_PARSER_STREAMINFO * pStreamInfo);
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return m_Api.SetParam(m_hModule, nID,pParam);}

protected:
	//static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	virtual void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData) ;

	virtual VO_VOID CreateTrackInfo( VO_LIVESRC_TRACK_INFOEX * ptr_info, VO_U32 uStreamID );

	virtual VO_VOID SendSample( VO_SOURCE2_TRACK_TYPE nOutputType , VO_SOURCE_SAMPLE * ptr_sample, VO_U32 uStreamID );
	virtual VO_VOID ConvertAudio( _SAMPLE * ptr_sample );
	virtual VO_VOID ConvertVideo( _SAMPLE * ptr_sample );

protected:
	VO_PARSER_API m_Api;


	int m_mediatype;

	VO_PBYTE m_ptr_convertaudiobuffer;
	VO_U32 m_convertaudiosize;

	VO_ADAPTIVESTREAMPARSER_BUFFER * m_ptrCurBuffer;

	VO_U32 m_VideoTimescale;
	VO_U32 m_AudioTimescale;
	VO_U32 m_SubtitleTimescale;
};

#ifdef _VONAMESPACE
}
#endif

#endif