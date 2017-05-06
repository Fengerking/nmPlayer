#ifndef __voAdaptiveStreamingFileParser_Subtitle_H__

#define __voAdaptiveStreamingFileParser_Subtitle_H__

#include "voAdaptiveStreamingFileParser_unknow.h"
#include "voSubtitleParser.h"
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class voAdaptiveStreamingFileParser_Subtitle
	:public voAdaptiveStreamingFileParser_unknow
{
public:
	voAdaptiveStreamingFileParser_Subtitle( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp = 0 );
	

	virtual ~voAdaptiveStreamingFileParser_Subtitle();


	virtual VO_U32 LoadParser();
	virtual VO_U32 FreeParser();

	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );

	virtual VO_VOID SetFileFormat( FileFormat ff );

	virtual VO_U32 CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo);
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return VO_RET_SOURCE2_NOIMPLEMENT;}

protected:
	virtual void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	virtual VO_VOID SendSample( VO_SOURCE2_TRACK_TYPE nOutputType , VO_PTR pBuffer );

protected:
	VO_PBYTE m_ptr_buffer;
	VO_U32 m_buffer_size;
	VO_U32 m_buffer_usedsize;

	SubtitleCodingType m_subtitleCodingType;

};

#ifdef _VONAMESPACE
}
#endif

#endif
