
#include "voSource2_IO.h"
#include "voSource2.h"
#include "vo_http_session.h"
#include "CSourceIOLocalFile.h"
#include "vo_http_utils.h"
//#include "DRMLocal.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

extern vo_http_sessions_info g_http_session_info;

VO_U32 Init( VO_HANDLE * pHandle , VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback )
{
	VOLOGINIT(g_http_session_info.getworkpath());

	VOLOGI("pSource==%s,uFlag==%d",pSource,uFlag);
	CSourceIOBase * pBaseIO = NULL;
	if( (0 == voiostrnicmp( (char *)pSource , "http://" , strlen("http://")))
		||( 0 == voiostrnicmp( (char *)pSource , "https://" , strlen("https://"))))
	{
		pBaseIO = new vo_http_session();
	}
	else
	{
		pBaseIO = new CSourceIOLocalFile();
	}

	if (!pBaseIO)
	{
		return VO_SOURCE2_IO_FAIL;
	}
	

	voSourceIOHnd *pIOHnd = new voSourceIOHnd;
	if(pIOHnd && pBaseIO )
	{
		pIOHnd->pIOHandle = pBaseIO;
		pIOHnd->pDrmEng = NULL;
		*pHandle = pIOHnd;
		return pBaseIO->Init(pSource ,uFlag ,pAsyncCallback);
	}
	else
	{
		delete pBaseIO;
		pBaseIO = NULL;
		return VO_SOURCE2_IO_NULLPOINTOR;
	}

}

VO_U32 UnInit( VO_HANDLE hHandle )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	VO_U32 ret = VO_SOURCE2_IO_OK;
	if (pIOBase)
	{
		ret = pIOBase->UnInit();
	}

	if(pIOBase)
	{
		delete pIOBase;
		pIOBase = NULL;
	}
	if(pIOHnd)
	{
		delete pIOHnd;
		pIOHnd = NULL;
	}

	VOLOGUNINIT();

	return ret;
}


VO_U32 Open( VO_HANDLE hHandle ,VO_BOOL bIsAsyncOpen )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	VO_U32 ret = pIOBase->Open( bIsAsyncOpen );
	if (ret != VO_SOURCE2_IO_OK)
	{
		return ret;
	}
	
	return VO_SOURCE2_IO_OK;
}


VO_U32 Close( VO_HANDLE hHandle)
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}

	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;
	if (pIOBase)
	{
		return pIOBase->Close();
	}

	return VO_SOURCE2_IO_OK;
}


VO_U32 Read( VO_HANDLE hHandle , VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pReadedSize )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	VO_U32 ret = pIOBase->Read( pBuf , uSize , pReadedSize );
	if (ret != VO_SOURCE2_IO_OK)
	{
		return ret;
	}

	return VO_SOURCE2_IO_OK;
	
}

VO_U32 Write( VO_HANDLE hHandle , VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pWrittenSize )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	return pIOBase->Write( pBuf, uSize, pWrittenSize);
}

VO_U32 SetPos( VO_HANDLE hHandle , VO_S64 llPos , VO_SOURCE2_IO_POS RelativePos , VO_S64 *llActualPos)
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;
	return pIOBase->SetPos( llPos , RelativePos , llActualPos);
}

VO_U32 Flush( VO_HANDLE hHandle )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	return pIOBase->Flush();
}

VO_U32 GetSize( VO_HANDLE hHandle , VO_U64 * pSize )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	VO_U32 ret = pIOBase->GetSize( pSize );
	if (ret != VO_SOURCE2_IO_OK)
	{
		return ret;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 GetLastError( VO_HANDLE hHandle )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	return pIOBase->GetLastError();
}

VO_U32 GetParam( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam )
{
	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	return pIOBase->GetParam( uParamID , pParam );
}

VO_U32 SetParam( VO_HANDLE hHandle , VO_U32 uParamID , VO_PTR pParam )
{
	if( VO_SOURCE2_IO_PARAMID_HTTPHEADINFO == uParamID )
	{
		g_http_session_info.UpdateExternalHeaderInfo((VO_SOURCE2_HTTPHEADER*)pParam);
		return VO_SOURCE2_IO_OK;
	}
	else if( VO_SOURCE2_IO_PARAMID_HTTPPROXYINFO == uParamID )
	{
		g_http_session_info.UpdateProxyInfo((VO_SOURCE2_HTTPPROXY*)pParam);
		return VO_SOURCE2_IO_OK;
	}
	else if( VO_SOURCE2_IO_PARAMID_DESTROY == uParamID )
	{
		//android OS has bug on recursively dlclose .so module , so work around for it.
#ifdef _LINUX_ANDROID
		VOLOGI("SourceIO lib will be freed");
		if( g_http_session_info.m_module_instancecnt <= 0  )
		{
			g_http_session_info.ResetPersistSSlsock();
			g_http_session_info.m_sslload.FreeSSL();
		}
#endif
		return VO_SOURCE2_IO_OK;
	}
	else if( VO_PID_SOURCE2_WORKPATH == uParamID )
	{
		g_http_session_info.setworkpath((VO_PTCHAR)pParam);
		return VO_SOURCE2_IO_OK;
	}	
	else if( VO_PID_COMMON_LOGFUNC == uParamID )
	{
		return VO_SOURCE2_IO_OK;
	}
	else if (VO_PID_SOURCE2_SSLAPI == uParamID )
	{
		COpenSSL::m_pSSLAPI = (vosslapi *)pParam;
		return VO_SOURCE2_IO_OK;
	}

	if (!hHandle )
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	voSourceIOHnd * pIOHnd = (voSourceIOHnd *)hHandle;
	CSourceIOBase * pIOBase = (CSourceIOBase *)pIOHnd->pIOHandle;

	return pIOBase->SetParam( uParamID , pParam );
}

#ifdef _VONAMESPACE
}
#endif ///< _VONAMESPACE

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef __cplusplus
extern "C"{
#endif///<__cplusplus

VO_VOID VO_API yyGetDataIOFunc( VO_SOURCE2_IO_API * ptr_api )
{
	ptr_api->Init = Init;
	ptr_api->UnInit = UnInit;
	ptr_api->Open = Open;
	ptr_api->Close = Close;
	ptr_api->Read = Read;
	ptr_api->Write = Write;
	ptr_api->SetPos = SetPos;
	ptr_api->Flush = Flush;
	ptr_api->GetSize = GetSize;
	ptr_api->GetLastError = GetLastError;
	ptr_api->GetParam = GetParam;
	ptr_api->SetParam = SetParam;
}
#ifdef __cplusplus
}
#endif///<__cplusplus
