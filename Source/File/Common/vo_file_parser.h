#if !defined(_IOS)
	#ifdef _PUSHPLAYPARSER

#ifndef __VO_FILE_PARSER_H__

#define __VO_FILE_PARSER_H__

#include "voYYDef_filcmn.h"
#include "voSource.h"
#include "vo_file_io.h"
#include "voCMutex.h"
#include "voCSemaphore.h"
#include "vo_thread.h"


#ifdef LIB
#include "voFileReader.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class vo_file_parser;

typedef VO_U32 ( vo_file_parser:: * ptr_close )();
typedef VO_U32 ( vo_file_parser:: * ptr_get_sample)(VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample);
typedef VO_U32 ( vo_file_parser:: * ptr_set_pos)(VO_U32 nTrack, VO_S64* pPos);

enum ParserMode
{
    NORMALFILEPARSE,
    PUSHMODEFILEPARSE,
    PDFILEPARSER,
};

struct GetSampleThreadInfo
{
    VO_S32 track_number;
    VO_SOURCE_SAMPLE sample;
    VO_PBYTE ptr_buffer;
    VO_U32 ret_getsample;
    VO_S64 dropframe_timestamp;

    THREAD_HANDLE thread_handle;

    voCSemaphore m_sem_sample;
    voCMutex sample_lock;
};

struct SampleThreadCreateInfo
{
    vo_file_parser * ptr_obj;
    GetSampleThreadInfo * ptr_thread_info;
};


class vo_file_parser : public CvoBaseFileOpr
{
    friend class vo_file_io;
public:

    vo_file_parser();
    ~vo_file_parser();

    VO_U32 open(VO_SOURCE_OPENPARAM* pParam);

    VO_U32 get_fileinfo(VO_SOURCE_INFO* pSourceInfo);
    VO_U32 get_trackinfo(VO_U32 nTrack, VO_SOURCE_TRACKINFO* pTrackInfo);

    VO_U32 set_fileparam(VO_U32 uID, VO_PTR pParam);
    VO_U32 get_fileparam(VO_U32 uID, VO_PTR pParam);

    VO_U32 set_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);
    VO_U32 get_trackparam(VO_U32 nTrack, VO_U32 uID, VO_PTR pParam);

    VO_U32 close();
    ptr_get_sample get_sample;
    ptr_set_pos set_pos;

    VO_PTR get_stream(){ return &m_io; }

protected:
    VO_U32 nor_get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample);
    VO_U32 nor_set_pos(VO_U32 nTrack, VO_S64* pPos);

    VO_U32 async_get_sample(VO_U32 nTrack, VO_SOURCE_SAMPLE* pSample);
    VO_U32 async_set_pos(VO_U32 nTrack, VO_S64* pPos);

    VO_U32 open_internal(VO_SOURCE_OPENPARAM * pParam);
    VO_VOID judge_parsemode_fillfunction();

    static unsigned int getsample_threadfunc( void * pParam );
    static unsigned int seek_threadfunc( void * pParam );

    VO_VOID stop_getsamplethread();

    VO_VOID init_asyncmode();


private:

    VO_PTR m_h_fileparser;
    ParserMode m_mode;

    VO_FILE_OPERATOR m_opFile;
    VO_FILE_OPERATOR * m_ptr_org_opFile;

    VO_PTR m_org_filehandle;

    vo_file_io m_io;

    GetSampleThreadInfo * m_ptr_sample_thread;
    VO_S32 m_trackcount;

    VO_S32 m_videotrack;
    VO_S32 m_audiotrack;

    voCMutex m_getsamplelock;

    VO_BOOL m_is_stop;
    VO_BOOL m_is_opening;

    THREAD_HANDLE m_seekthread_handle;
    voCSemaphore m_sem_seek;
    VO_S64       m_seek_pos;

    VO_BOOL m_stop_getsample;

    VO_U32 m_firstseek_track;
    voCMutex m_seekpos_lock;
    VO_BOOL m_is_seeking;

    VO_S64 m_actualfilesize;

};

#ifdef _VONAMESPACE
}
#endif

#endif

	#endif
#endif
