#pragma once

#include "voYYDef_SrcCmn.h"
#include "voSource2_IO.h"
#include "voAdaptiveStreamParser.h"

#ifdef _REDRESS_DOWNLOADSIZE
#include "voCMutex.h"
#include "voDSType.h"
#endif

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif



#ifdef _REDRESS_DOWNLOADSIZE

typedef struct
{
//callback instance
	VO_PTR pUserData;
/**
 * ReadFixedSize will notify client via this function for some events.
 * \param pUserData [in]
 * \param nID [in] The status type.
 * \param nParam1 [in] status specific parameter 1.
 * \param nParam2 [in] status specific parameter 2.
 */
	VO_S32 (VO_API * NotifyEvent) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

}VO_SOURCEIO_EVENTCALLBACK;

	class DownloadDataCollector
	{
	public:
		DownloadDataCollector()
			:m_uDownloadSize(0)
		{

		}
		~DownloadDataCollector()
		{

		}

		VO_VOID Reset()
		{
			voCAutoLock lock(&m_lock);
			m_uDownloadSize = 0;
		}
		VO_VOID AddSize(VO_U32 uSize)
		{
			voCAutoLock lock(&m_lock);
			m_uDownloadSize += uSize;
		}
		VO_U64 GetSize()
		{	
			voCAutoLock lock(&m_lock);
			return m_uDownloadSize;
		}
	private:
		VO_U64 m_uDownloadSize;
		voCMutex m_lock;
	};
	VO_U32 ReadFixedSize(DownloadDataCollector *pDataColletor, VO_SOURCE2_IO_API * ptr_api , VO_HANDLE handle , VO_PBYTE ptr_buffer , VO_U32* ptr_size , VO_BOOL * ptr_cancel, VO_U64 timeout = -1, VO_SOURCEIO_EVENTCALLBACK *pCallback = NULL);
//	VO_U32 DownloadItem_III(DownloadDataCollector *pDataColletor, VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo,  VO_SOURCE2_IO_HTTPCALLBACK * pIOHttpCallback,VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_BOOL * pCancel, VO_BOOL bChangeURL);

#endif
	
	VO_U32 ReadFixedSize(VO_SOURCE2_IO_API * ptr_api , VO_HANDLE handle , VO_PBYTE ptr_buffer , VO_U32* ptr_size , VO_BOOL * ptr_cancel, VO_U64 timeout = -1);

VO_U32 GetTheAbsolutePath(VO_CHAR* pstrDes, VO_CHAR* pstrInput, VO_CHAR* pstrRefer);

VO_U32  GetTheAbsolutePath2(VO_PTCHAR pstrDes, VO_PTCHAR pstrRefer, VO_PTCHAR pstrBase);

//VO_U32 DownloadItem_II( VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo, VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_CHAR **ppBuffer, VO_U32 *pSize,VO_BOOL * pCancel,  VO_BOOL bChangeURL = VO_TRUE );
VO_U32 DownloadItem_II( VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo, VO_SOURCE2_IO_HTTPCALLBACK * pIOHttpCallback,VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_CHAR **ppBuffer, VO_U32 *pSize, VO_U32 *pUsedSize,VO_BOOL * pCancel, VO_BOOL bChangeURL = VO_TRUE   );

VO_U32 DownloadItem_III( VO_SOURCE2_IO_API *pIoApi, VO_SOURCE2_VERIFICATIONINFO * pVerificationInfo,  VO_SOURCE2_IO_HTTPCALLBACK * pIOHttpCallback,VO_ADAPTIVESTREAM_PLAYLISTDATA * pData, VO_BOOL * pCancel, VO_BOOL bChangeURL = VO_TRUE);

class voAutoIOInit
{
public:
	voAutoIOInit( VO_SOURCE2_IO_API * ptr_api , VO_CHAR * ptr_url , VO_HANDLE * h )
		:m_ret(0)
		,m_h(0)
		,m_ptr_io( ptr_api )
	{
		if( !ptr_api || !ptr_api->Init )
			m_ret = VO_SOURCE2_IO_NULLPOINTOR;

		m_ret = ptr_api->Init( &m_h , ptr_url , VO_SOURCE2_IO_FLAG_OPEN_URL , 0 );

		if( m_ret == VO_SOURCE2_IO_OK )
			*h = m_h;
	}
	~voAutoIOInit()
	{
		if( m_h && m_ptr_io && m_ptr_io->Init )
			m_ptr_io->UnInit( m_h );
	}

	VO_U32 m_ret;
	VO_HANDLE m_h;
	VO_SOURCE2_IO_API * m_ptr_io;
};

class voAutoIOOpen
{
public:
	voAutoIOOpen( VO_SOURCE2_IO_API * ptr_api , VO_HANDLE hHandle , VO_BOOL bIsAsyncOpen )
		:m_ret( 0 )
		,m_h( hHandle )
		,m_ptr_io( ptr_api )
	{
		if( !ptr_api || !ptr_api->Open || !hHandle )
			m_ret = VO_SOURCE2_IO_NULLPOINTOR;

		m_ret = ptr_api->Open( m_h , bIsAsyncOpen );
	}
	~voAutoIOOpen()
	{
		if( m_h && m_ptr_io && m_ptr_io->Open && m_h )
			m_ptr_io->Close( m_h );
	}

	VO_U32 m_ret;
	VO_HANDLE m_h;
	VO_SOURCE2_IO_API * m_ptr_io;
};

#ifdef _VONAMESPACE
}
#endif ///< _VONAMESPACE