#ifndef __VOADAPTIVESTREAMINGFILEPARSER_UNKNOW_H__

#define __VOADAPTIVESTREAMINGFILEPARSER_UNKNOW_H__

#include "voAdaptiveStreamingFileParser.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "voLiveSource.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class voAdaptiveStreamingFileParser_unknow
	:public voAdaptiveStreamingFileParser
{
public:
	voAdaptiveStreamingFileParser_unknow( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp = 0 );
	

	virtual ~voAdaptiveStreamingFileParser_unknow();


	virtual VO_U32 LoadParser();
	virtual VO_U32 FreeParser();

	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );

//	virtual VO_VOID SetFormatChange() ;
	
	virtual VO_SOURCE2_MEDIATYPE GetMeidaType();

	virtual VO_U32 CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo){ return VO_RET_SOURCE2_NOIMPLEMENT;}
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return VO_RET_SOURCE2_NOIMPLEMENT;}

protected:
	virtual void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	virtual VO_VOID SendSample( VO_SOURCE2_TRACK_TYPE nOutputType , VO_PTR pBuffer );
	int m_mediatype;
};

#ifdef _VONAMESPACE
}
#endif

#endif
