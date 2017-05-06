#ifndef __voAdaptiveStreamingFileParser_ID3__
#define __voAdaptiveStreamingFileParser_ID3__

#include "voAdaptiveStreamingFileParser.h"
#include "voParser.h"
#include "CID3Parser.h"
#include "voSubtitleType.h"
#include "voSampleOP.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voAdaptiveStreamingFileParser_ID3
	:public voAdaptiveStreamingFileParser
{
public:
	voAdaptiveStreamingFileParser_ID3( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp = 0 );
	virtual ~voAdaptiveStreamingFileParser_ID3();

	virtual VO_U32 LoadParser();
	virtual VO_U32 FreeParser();
	
	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );

	virtual VO_SOURCE2_MEDIATYPE GetMeidaType();

	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return VO_RET_SOURCE2_NOIMPLEMENT;}

	virtual VO_U32 CreateTrackInfo2(VO_SOURCE2_TRACK_TYPE type , _TRACK_INFO * pStreamInfo);
protected:

	static VO_S32 VO_API OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);


	VO_U32 NotifyCustomerTag( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );

	VO_VOID ResetSampleList();

protected:

	int m_mediatype;
protected:
	CID3Parser m_ID3Parser;
	SubtitleCodingType m_subtitleCodingType;
	VO_BOOL m_bStop;

	vo_singlelink_list< _SAMPLE* > m_SampleList;
	vo_allocator< VO_BYTE > m_alloc;
};

#ifdef _VONAMESPACE
}
#endif

#endif