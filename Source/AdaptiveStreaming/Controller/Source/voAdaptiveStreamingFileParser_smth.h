#ifndef __VOADAPTIVESTREAMINGFILEPARSER_SMTH__
#define __VOADAPTIVESTREAMINGFILEPARSER_SMTH__

#include "voAdaptiveStreamingFileParser_mp4.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "voLiveSource.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voAdaptiveStreamingFileParser_smth
	:public voAdaptiveStreamingFileParser_mp4
{
public:
	voAdaptiveStreamingFileParser_smth( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp = 0 );
	virtual ~voAdaptiveStreamingFileParser_smth();

	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );
	virtual VO_U32 CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo);
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return m_Api.SetParam(m_hModule, nID,pParam);}

protected:
	//static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	virtual void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData) ;

};

#ifdef _VONAMESPACE
}
#endif

#endif