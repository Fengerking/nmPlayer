
#ifndef __VO_OGGREADER_H_

#define __VO_OGGREADER_H_


#include "CBaseStreamFileReader.h"
#include "oggparser.h"
#include "oggtrack.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class oggreader : public CBaseStreamFileReader
{
public:
	oggreader(VO_FILE_OPERATOR* pFileOp, VO_MEM_OPERATOR* pMemOp, VO_LIB_OPERATOR* pLibOP, VO_SOURCEDRM_CALLBACK* pDrmCB);
	virtual ~oggreader();

	virtual VO_U32 Load(VO_U32 nSourceOpenFlags, VO_FILE_SOURCE* pFileSource);
	VO_U32 Close();

	VO_BOOL notify_new_page( VO_U32 stream_id , VO_U64 granule_pos );
	VO_U8 on_receive_packet( const ogg_packet* ptr_ogg_packet );

	virtual VO_U32 MoveTo(VO_S64 llTimeStamp);

	virtual VO_U32 GetParameter(VO_U32 uID, VO_PTR pParam);

protected:
	virtual VO_U32 InitTracks(VO_U32 nSourceOpenFlags);
	virtual VO_VOID	Flush();

private:
	ogg_header_parser m_header_parser;
	ogg_data_parser m_data_parser;

	oggtrack * m_ptr_audio_track;
	oggtrack * m_ptr_video_track;
};

#ifdef _VONAMESPACE
}
#endif


#endif
