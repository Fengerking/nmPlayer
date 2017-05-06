
#include "voAdaptiveStreamingFileParser.h"
#include "voParser.h"
#include "CDllLoad.h"
#include "voAdaptiveStreamingFileParser_ID3.h"
	
#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voAdaptiveStreamingFileParser_ts
	:public voAdaptiveStreamingFileParser
{
public:
	voAdaptiveStreamingFileParser_ts( VO_SOURCE2_SAMPLECALLBACK * pCallback , VO_SOURCE2_LIB_FUNC * pLibOp = 0 );
	virtual ~voAdaptiveStreamingFileParser_ts();

	virtual VO_U32 LoadParser();
	virtual VO_U32 FreeParser();

	virtual VO_U32 Process( VO_ADAPTIVESTREAMPARSER_BUFFER * pBuffer );

	virtual VO_VOID SetFormatChange();

	virtual VO_SOURCE2_MEDIATYPE GetMeidaType();
	virtual VO_U32 SetParam(VO_U32 nID, VO_VOID * pParam) { return m_Api.SetParam(m_hModule, nID,pParam);}

	virtual VO_VOID	OnChunkEnd();

protected:
	//static void VO_API ParserProc(VO_PARSER_OUTPUT_BUFFER* pData);
	virtual void ParserProc(VO_PARSER_OUTPUT_BUFFER* pData) ;

	virtual VO_U32 SendMediaData( VO_SOURCE2_TRACK_TYPE type , VO_PTR pBuffer,VO_U32 uStreamID );
				
	virtual VO_U32 SendMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_U64 timestamp, VO_U32 uStreamID, VO_U32 uFlag, VO_U32 uMarkOP);
	virtual VO_U32 CreateMediaInfo( VO_SOURCE2_TRACK_TYPE type , VO_PARSER_STREAMINFO * pStreamInfo,VO_U32 uStreamID );


protected:
	VO_ADAPTIVESTREAMPARSER_BUFFER * m_pCurBuffer;

	VO_BOOL m_isFormatChange;

	VO_PARSER_API m_Api;

	

	VO_BOOL m_bTsRollback_audio;
	VO_BOOL m_bTsRollback_video;
	VO_BOOL m_bTsRollback_subtitle;

	int m_mediatype;

	VO_U32 m_ChunkCount;
	VO_BOOL m_isSampleCallbacked;

	//for ID3 data parse
protected:
	voAdaptiveStreamingFileParser_ID3 *m_pID3Mgr;

};

#ifdef _VONAMESPACE
}
#endif