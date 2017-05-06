#pragma once
#include "interface_download_callback.h"
#include "interface_buffer_callback.h"
#include "voSource.h"
#include "voStreaming.h"
#include "vo_download_manager.h"
#include "vo_buffer_manager.h"
#include "CDllLoad.h"
#include "vo_buffer_stream.h"
#include "voCMutex.h"

enum SEEKSTATUS
{
	SEEKSTATUS_SEEKSTART = 0,
	SEEKSTATUS_ONETRACKGOT,
	SEEKSTATUS_TWOTRACKGOT,
	SEEKSTATUS_SEEKEND,
};

class vo_PD_manager :
	public interface_download_callback,
	public interface_buffer_callback
{
public:
	vo_PD_manager(void);
	virtual ~vo_PD_manager(void);

	//download call back
	virtual CALLBACK_RET received_data( VO_S64 physical_pos , VO_PBYTE ptr_buffer , VO_S32 size );
	virtual CALLBACK_RET download_notify( DOWNLOAD_CALLBACK_NOTIFY_ID id , VO_PTR ptr_data );
	//

	//buffer call back
	virtual BUFFER_CALLBACK_RET buffer_notify( BUFFER_CALLBACK_NOTIFY_ID notify_id , VO_PTR ptr_data );
	//

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
	VO_BOOL perpare_parser();

	VO_VOID set_firstseektrack( VO_S32 track ){ m_firstseektrack = track; }
	VO_S32 get_firstseektrack(){ return m_firstseektrack; }

	vo_buffer_stream * get_stream(){ return m_buffer_stream; }

protected:
	VO_BOOL delay_open();
	VO_VOID arrange_headdata( headerdata_info * ptr_info );

	BUFFER_CALLBACK_RET buffer_notify_bufferlack( VO_BUFFER_NEED * ptr_need );
	VO_VOID process_STREAMING_QUICKRESPONSE_COMMAND( VO_QUICKRESPONSE_COMMAND command );

	VO_VOID notify( long EventCode, long * EventParam );

private:
	VO_SOURCE_OPENPARAM m_source_param;
	VOPDInitParam m_PD_param;
	VO_CHAR m_url[1024];

	vo_download_manager m_downloader;
	vo_buffer_manager m_buffer;

	VO_SOURCE_READAPI m_parser_api;
	CDllLoad m_dllloader;

	vo_buffer_stream * m_buffer_stream;

	VO_FILE_OPERATOR m_opFile;
	VO_PTR m_fileparser;

	VO_BOOL m_is_init;
	VO_BOOL m_is_to_exit;
	VO_BOOL m_is_to_break_buffering;

	VO_S32 m_bitrate;
	VO_U32 m_duration;

	voCMutex m_testlock;

	VO_S32 m_firstseektrack;

	VO_BOOL m_is_pause;
	VO_BOOL m_is_start_buffering;
	VO_S32  m_framegot[2];
//for htc test
	VO_S32	m_waitbuffertime;
	VO_S32	m_buffertimewithoutbitrate;
//

	StreamingNotifyEventFunc m_notifier;

	SEEKSTATUS m_seek_status;
};
