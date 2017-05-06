#if !defined(_IOS)
	#ifdef _PUSHPLAYPARSER

#ifndef __VO_FILE_IO_H__

#define __VO_FILE_IO_H__


#include "voYYDef_filcmn.h"
#include "voSource.h"
#include "voFile.h"
#include "CvoBaseFileOpr.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

enum IOMODE
{
    IO_NORMAL,
    IO_CHECK,
};


VO_PTR file_open(VO_FILE_SOURCE * pSource);
VO_S32 file_read(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);
VO_S32 file_write(VO_PTR pFile, VO_PTR pBuffer, VO_U32 uSize);
VO_S32 file_flush(VO_PTR pFile);
VO_S64 file_seek(VO_PTR pFile, VO_S64 nPos, VO_FILE_POS uFlag);
VO_S64 file_size(VO_PTR pFile);
VO_S64 file_save(VO_PTR pFile);
VO_S32 file_close(VO_PTR pFile);

class vo_file_parser;

class vo_file_io
{
public:
    vo_file_io();
    ~vo_file_io();

    VO_VOID init( vo_file_parser * ptr_org_op , VO_PTR org_filehandle  );

    virtual VO_S32 read( VO_PBYTE buffer , VO_U32 toread );
    virtual VO_S64 seek( VO_S64 seekpos , VO_FILE_POS pos );

    virtual VO_S64 get_filesize();
    virtual VO_S64 get_filepos(){ return m_filepos; }

    virtual VO_VOID set_to_close( VO_BOOL is_closing ){ m_is_closing = is_closing; }

    VO_VOID set_mode( IOMODE mode ){ m_mode = mode; }

	VO_VOID	setRealFileSize(VO_S64);
	VO_S64	getRealFileSize();

protected:
    VO_S64 m_filepos;
    VO_BOOL m_is_closing;
    vo_file_parser * m_ptr_org_op;
    VO_PTR m_org_filehandle;
    IOMODE m_mode;

    //for test use
    VO_U32 start_time;
    VO_S64 m_filesize;
    //
	VO_S64 m_RealFileSize;
};

#ifdef _VONAMESPACE
}
#endif


#endif

#endif
#endif
