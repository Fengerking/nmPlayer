
#include "voAdaptiveStreamingFileParser.h"
#include "voParser.h"
#include "CDllLoad.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voAdaptiveStreamingFileParser_pa
	:public voAdaptiveStreamingFileParser
{
public:
	voAdaptiveStreamingFileParser_pa(VO_SOURCE2_SAMPLECALLBACK * pCallback, FileFormat ff, VO_SOURCE2_LIB_FUNC * pLibOp = 0);
	virtual ~voAdaptiveStreamingFileParser_pa();

public:
	virtual VO_U32 LoadParser();
	virtual VO_U32 FreeParser();

	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );
	
	virtual VO_SOURCE2_MEDIATYPE GetMeidaType();

	VO_VOID SetTimeStampOffset( VO_U64 offset ){ m_lasttimestamp = m_offset = offset; }
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return m_Api.SetParam(m_hModule, nID,pParam);}

protected:
	static VO_S32 VO_API OnSample(VO_PTR pUserData, VO_U16 nOutputType, VO_PTR pData);

protected:
	FileFormat							m_ff;
	VO_SOURCE2_API						m_Api;
	VO_ADAPTIVESTREAMPARSER_BUFFER *	m_pCurBuf;
	VO_U64								m_lasttimestamp;
	_TRACK_INFO *						m_pTrackInfo;
};

#ifdef _VONAMESPACE
}
#endif