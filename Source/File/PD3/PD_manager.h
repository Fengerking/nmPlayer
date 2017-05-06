
#ifndef __PD_MANAGER_H__

#define __PD_MANAGER_H__

#include "buffer_stream_manager.h"
#include "CDllLoad.h"

class PD_manager
{
public:
	PD_manager(void);
	~PD_manager(void);

	VO_BOOL open( VO_SOURCE_OPENPARAM * pParam );
	VO_U32 close();
	VO_U32 get_sourceinfo(VO_SOURCE_INFO * pSourceInfo);
	VO_U32 get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO * pTrackInfo);
	VO_U32 get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE * pSample);
	VO_U32 set_pos(VO_U32 nTrack, VO_S64 * pPos);
	VO_VOID moveto( VO_S64 pos );
	VO_U32 set_sourceparam(VO_U32 uID, VO_PTR pParam);
	VO_U32 get_sourceparam(VO_U32 uID, VO_PTR pParam);
	VO_U32 set_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
	VO_U32 get_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);

	VO_VOID set_firstseektrack( VO_S32 track ){ m_firstseektrack = track; }
	VO_S32 get_firstseektrack(){ return m_firstseektrack; }

	VO_PTR get_io(){ return &m_stream; }

protected:
	VO_VOID process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command );
	VO_BOOL delay_open();

	VO_BOOL perpare_parser();

protected:
	VO_S32 m_firstseektrack;
	buffer_stream_manager m_stream;

	VO_SOURCE_OPENPARAM m_source_param;
	VOPDInitParam m_PD_param;
	VO_CHAR m_url[1024];

	StreamingNotifyEventFunc m_notifier;

	VO_SOURCE_READAPI m_parser_api;
	CDllLoad m_dllloader;
	VO_PTR m_fileparser;

	VO_FILE_OPERATOR m_opFile;
};


#endif
