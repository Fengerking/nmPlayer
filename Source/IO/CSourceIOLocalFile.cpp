
#include "CSourceIOLocalFile.h"
#include "vo_http_utils.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "cmnFile.h"
#include "voSource2.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CSourceIOLocalFile::CSourceIOLocalFile(void)
: CSourceIOBase()
, m_hHandle(NULL)
, m_Source(NULL)
, m_bExitIO(VO_FALSE)
, m_pDrm(NULL)
, m_bDrm_content(VO_TRUE)
, m_ullActualFileSize(0xFFFFFFFFFFFFFFFFULL)
, m_nPosWithOffset(0)
, m_ullRangeFileSize(0xFFFFFFFFFFFFFFFFULL)
, m_ullRangeOffset(0xFFFFFFFFFFFFFFFFULL)
{
	m_Source = new VO_TCHAR[2048];
}

CSourceIOLocalFile::~CSourceIOLocalFile(void)
{
	if (m_Source)
	{
		delete m_Source;
		m_Source = NULL;
	}

	UnInit();
}

VO_U32 CSourceIOLocalFile::Init(VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback )
{
	m_pDrm = NULL;
//	if (uFlag & VO_SOURCE2_IO_FLAG_OPEN_DRM)
	{
		m_pDrm = new CDRMLocal();
		if (!m_pDrm)
		{
			return VO_SOURCE2_IO_FAIL;
		}
	}

	VOLOGI("local path:%s", (VO_CHAR*)pSource );

	return SetFileSource((VO_CHAR*)pSource);
}

VO_U32 CSourceIOLocalFile::UnInit()
{
	if(m_pDrm)
	{
		m_pDrm->uninit();

		delete m_pDrm;
		m_pDrm = NULL;
	}

	return VO_SOURCE2_IO_OK;
}

//initilize DRM object with DRM info
VO_U32 CSourceIOLocalFile::DrmOpen()
{
	VO_U32 ret = 0;
	if (m_pDrm)
	{
		VO_PBYTE pDrmInfo = NULL;
		VO_PTR pSrcPath;
		VO_U64 ullPos = 0;
		VO_U32 ulsize = 0;

		//1, get resource url to map
		GetSource(&pSrcPath);
		ret =  VO_SOURCE2_IO_RETRY;

		//2, get the right position and size of DRM info
		while( VO_SOURCE2_IO_RETRY == ret )
		{
			ret = m_pDrm->getDRMInfo( (VO_CHAR*)pSrcPath , &ullPos , &ulsize );
			if(ret)
			{
				//it is clear content
				m_bDrm_content = VO_FALSE;
				return VO_SOURCE2_IO_OK;
			}

			//3,seek to the specified position to read DRM info
			VO_S64 ret64 = cmnFileSeek(m_hHandle, ullPos, VO_FILE_BEGIN);
			if (ret64 < 0 || ullPos != ret64)
			{
				return VO_SOURCE2_IO_FAIL;
			}

			m_nPosWithOffset = (VO_S32)ullPos;
			//4,read DRM info data from file to temp buffer
			VO_U32 ulActualReadSize = 0;
			pDrmInfo = new VO_BYTE[ulsize];
			ret = BasicRead( pDrmInfo , ulsize , &ulActualReadSize );
			if (ret != VO_SOURCE2_IO_OK || ulActualReadSize != ulsize)
			{
				//we should make sure IO read the requested data totally in IO Read process.
				delete []pDrmInfo;
				pDrmInfo = NULL;
				return VO_SOURCE2_IO_FAIL;
			}

			//5,set DRM info to DRM object to initialize DRM object
			ret = m_pDrm->setDRMInfo( ullPos , pDrmInfo , ulsize , NULL );
			if(VO_SOURCE2_IO_OK != ret && VO_SOURCE2_IO_RETRY != ret)
			{
				delete []pDrmInfo;
				pDrmInfo = NULL;
								
				//it is clear content
				m_bDrm_content = VO_FALSE;
				return VO_SOURCE2_IO_OK;
			}

			//6,free the temp memory allocated for DRM info.
			delete []pDrmInfo;
			pDrmInfo = NULL;
		}
		if (ret != VO_SOURCE2_IO_OK)
		{
			return ret;
		}

		return VO_SOURCE2_IO_OK;
	}
	else
	{
		//for clear content, drm pointer will be null, so should set flag to none drm content .
		m_bDrm_content = VO_FALSE;
		return VO_SOURCE2_IO_OK;
	}
}


VO_U32 CSourceIOLocalFile::Open(VO_BOOL bIsAsyncOpen)
{
	m_bExitIO = VO_FALSE;
	VO_FILE_SOURCE src;
	memset(&src,0,sizeof(VO_FILE_SOURCE));
	src.nFlag = VO_FILE_TYPE_NAME;
	src.pSource = m_Source;
	src.nMode = VO_FILE_READ_ONLY;

	m_hHandle = cmnFileOpen(&src);
	if (!m_hHandle)
	{
#if (defined(_WIN32) || defined(WINCE))
		return VO_SOURCE2_IO_FAIL;
#else
		VO_CHAR * pSource = (VO_CHAR *)m_Source;
		VO_CHAR * strdecode = NULL;
		strdecode = url_decode(pSource);
		if( strdecode )
		{
			VOLOGI("decoded path:%s", strdecode);
			vostrcpy( m_Source , strdecode );
			free(strdecode);
			src.pSource = m_Source;
			m_hHandle = cmnFileOpen(&src);
			if(!m_hHandle)
			{
				return VO_SOURCE2_IO_FAIL;
			}
		}
		else
		{
			VOLOGE("urldecod path fail");
			return VO_SOURCE2_IO_FAIL;
		}
#endif
	}

	if( m_ullRangeOffset != 0xFFFFFFFFFFFFFFFFULL )
	{
		//it means range request had been sent for local file
		VO_FILE_POS uFlag = VO_FILE_BEGIN;
		m_nCurPos = m_ullRangeOffset;
		VO_S64 ret = cmnFileSeek(m_hHandle,(VO_S64)m_ullRangeOffset,uFlag);

		if (ret < 0)
		{
			return VO_SOURCE2_IO_FAIL;
		}

		VOLOGI("do range seek :%llu", m_ullRangeOffset );
	}


	if( m_bDrm_content )
	{
		VO_U32 ret = 0;
		//initilize DRM object with DRM info
		ret = DrmOpen();
		if (ret != VO_SOURCE2_IO_OK)
		{
			return ret;
		}

		m_nPosWithOffset = 0;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::DrmRead( VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pReadSize )
{
	if( m_pDrm )
	{
		//1,get the wanted data start position 
		VO_U64 llCurPos = 0;
		VO_U64 llActualPos = 0;
		VO_U64 llActualOrigPos = 0;
		VO_U32 ulActualsize = 0;

		VO_U32 ret = GetCurPos(&llCurPos);
		if (ret != VO_SOURCE2_IO_OK)
		{
			return ret;
		}


		VO_U32 ulreadedcnt = 0;
			/****************************************************************************************
				since the required drm encrypted data maybe seperated by drm info, so we may need to 
				read it several times, and splice them together.

				----------------------------------------------------------------------------
				|																			|
				|																			|
				|	drm data	|	drm info	|	drm data	|	drm data	|	......	|
				|																			|
				|																			|				
				----------------------------------------------------------------------------
			*****************************************************************************************/
		do
		{
			//2,translate the position and size required to actual file position and size.
			ret =  VO_SOURCE2_IO_RETRY;
			while( VO_SOURCE2_IO_RETRY == ret )
			{
				ret = m_pDrm->getActualLocation( llCurPos + ulreadedcnt , uSize - ulreadedcnt , &llActualPos , &ulActualsize );
			}
			if ( ret != VO_SOURCE2_IO_OK  )
			{
				return ret;
			}

			//we should store the first actual pos for later usage.
			if( ulreadedcnt == 0 )
			{
				llActualOrigPos = llActualPos;
			}

			//3,seek to the specified position
			VO_S64 ret64 = cmnFileSeek(m_hHandle, llActualPos, VO_FILE_BEGIN);
			if (ret64 < 0 || llActualPos != ret64 )
			{
				return VO_SOURCE2_IO_FAIL;
			}

			m_nPosWithOffset = (VO_S32)llActualPos;
			//4,read data from the actual position , and splice the data.
			VO_U32 llActualReadedsize = 0;
			ret = BasicRead( (VO_PBYTE)pBuf + ulreadedcnt , ulActualsize , &llActualReadedsize );
			if (ret != VO_SOURCE2_IO_OK || llActualReadedsize != ulActualsize)
			{
				//we should make sure IO read the requested data totally in IO Read process.
				return VO_SOURCE2_IO_FAIL;
			}

			//calculate the total actual read size
			ulreadedcnt += ulActualsize;
		}while( ulreadedcnt < uSize );
		

		//5,feed the readed data to descryption routine
		VO_PBYTE  ppdest = NULL;
		VO_U32   destsize = 0;
		ret = m_pDrm->decryptData( llActualOrigPos , (VO_PBYTE)pBuf , ulActualsize , &ppdest , &destsize  );
		if (ret != VO_SOURCE2_IO_OK)
		{
			return ret;
		}

		pBuf = ppdest;
		*pReadSize = destsize;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::Read (VO_VOID * pBuffer , VO_U32 uSize , VO_U32 * pReadSize )
{
	VOLOGI("CSourceIOLocalFile::Read wanted:%d",uSize);
	voCAutoLock lock(&m_lock);

	VO_U32 ret = VO_SOURCE2_IO_OK;
	if( m_bDrm_content )
	{
		ret = DrmRead( pBuffer , uSize , pReadSize);
	}
	else
	{
		ret = BasicRead( pBuffer , uSize , pReadSize );
	}

	if (ret != VO_SOURCE2_IO_OK)
	{
		return ret;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::BasicRead (VO_VOID * pBuffer , VO_U32 uSize , VO_U32 * pReadSize )
{
	VO_S32 nReadSize = 0;

	VO_U64 ullSizeEnd = m_nPosWithOffset ? m_nPosWithOffset + uSize : m_nCurPos + uSize;
	VO_S64 llSizeCur = 0;
	//VO_U32 uT0 = voOS_GetSysTime();

	//if actual file size is set, then it means it works in push & play mode, the file size will be change by time.
	if ( m_ullActualFileSize != 0xFFFFFFFFFFFFFFFFULL )
	{
		if (ullSizeEnd > m_ullActualFileSize)
		{
			VOLOGE("ullSizeEnd %llu", ullSizeEnd);
			return VO_SOURCE2_IO_FAIL;
		}

		do 
		{
			llSizeCur = cmnFileSize(m_hHandle);
			if (llSizeCur < 0)
			{
				return VO_SOURCE2_IO_FAIL;
			}
			else if (ullSizeEnd > (VO_U64)llSizeCur)
			{
				m_lock.Unlock();
				voOS_Sleep(30);
				m_lock.Lock();

				//#define MAX_IO_TIMEOUT 5000
				//			if( voOS_GetSysTime() - uT0 >= MAX_IO_TIMEOUT )
				//			{
				//				VOLOGW("VO_SOURCE2_IO_RETRY");
				//				return VO_SOURCE2_IO_RETRY;
				//			}
			}
		} while (ullSizeEnd > (VO_U64)llSizeCur && !m_bExitIO);
	}

	nReadSize = cmnFileRead(m_hHandle,pBuffer,uSize);
	VOLOGI("CSourceIOLocalFile::Read:%d===%d",uSize,nReadSize);
	if (nReadSize < 0)
	{
		VOLOGW("nReadSize < 0");
		return VO_SOURCE2_IO_FAIL;
	}

	if (pReadSize)
	{
		* pReadSize = (VO_U32)nReadSize;
	}
	VOLOGI("nReadSize >=0");
	m_nCurPos += nReadSize;

	//if we failed to read any of the required data,it means EOS has been reached
	if( 0 == nReadSize )
	{
		VOLOGI("partial readed, reach EOS");
		return VO_SOURCE2_IO_EOS;
	}
	else
	{
		return VO_SOURCE2_IO_OK;
	}
}

VO_U32 CSourceIOLocalFile::Write (VO_VOID * pBuffer , VO_U32 uSize , VO_U32 * pWrittenSize )
{
	voCAutoLock lock(&m_lock);
	VO_S32 nWriteSize = 0;
	nWriteSize = cmnFileWrite(m_hHandle,pBuffer,uSize);
	if (nWriteSize < 0)
	{
		return VO_SOURCE2_IO_FAIL;
	}
	if (pWrittenSize)
	{
		*pWrittenSize = (VO_U32)nWriteSize;
	}
	
	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::Flush (void)
{
	voCAutoLock lock(&m_lock);
	VO_S32 ret = 0;
	ret = cmnFileFlush(m_hHandle);
	if (ret < 0)
	{
		return VO_SOURCE2_IO_FAIL;
	}
	return VO_SOURCE2_IO_OK;

}

VO_VOID CSourceIOLocalFile::SetLocalRange( VO_U64 offset , VO_U64 size )
{
	voCAutoLock lock(&m_lock);

	m_ullRangeOffset = offset;
	m_ullRangeFileSize = size;
	VOLOGI("m_ullRangeOffset: %llu , m_ullRangeFileSize :%llu", m_ullRangeOffset , m_ullRangeFileSize );
}

VO_U32 CSourceIOLocalFile::SetParam(VO_U32 uParamID , VO_PTR pParam)
{
	voCAutoLock lock(&m_lock);

	switch( uParamID )
	{
	case VO_SOURCE2_IO_PARAMID_DRMPOINTOR:
		{
			if(m_pDrm)
			{
				m_pDrm->init( (VO_SOURCEDRM_CALLBACK2*)pParam );
			}
			return VO_SOURCE2_IO_OK;
		}
		break;

	case VO_PID_SOURCE2_ACTUALFILESIZE:
		{
			m_ullActualFileSize = *(VO_U64*)pParam;
			VOLOGI("m_ullActualFileSize %llu", m_ullActualFileSize);
		}
		break;

	case VO_SOURCE2_IO_PARAMID_HTTPRANGE:
		{
			VO_SOURCE2_IO_HTTPRANGE * ptr = (VO_SOURCE2_IO_HTTPRANGE *)pParam;
			if( ptr )
			{
				SetLocalRange( ptr->ullOffset , ptr->ullLength );
			}
		}
		break;

	default:
		{
		}
		break;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::GetParam(VO_U32 uParamID , VO_PTR pParam)
{
	switch( uParamID )
	{
	case VO_SOURCE2_IO_PARAMID_HTTPDOWNLOADTIME:
		{
			VO_U64 * ptr_time = ( VO_U64 * )pParam;
			*ptr_time = (VO_U64)0;

			return VO_SOURCE2_IO_OK;
		}
		break;
	case VO_SOURCE2_IO_PARAMID_HTTPREDIRECTURL:
		{
			return VO_SOURCE2_IO_NOTIMPLEMENT;
		}
		break;
	}
	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::SetPos (VO_S64 llPos , VO_SOURCE2_IO_POS RelativePos, VO_S64 *llActualPos)
{
	voCAutoLock lock(&m_lock);
	VOLOGI("SetPos from %d move %lld", RelativePos, llPos);
	VO_FILE_POS uFlag = VO_FILE_POS_MAX;
	switch(RelativePos)
	{
	case VO_SOURCE2_IO_POS_BEGIN:
		{
			uFlag = VO_FILE_BEGIN;
			m_nCurPos = llPos;
		}
		break;
	case VO_SOURCE2_IO_POS_CURRENT:
		{
			uFlag = VO_FILE_CURRENT;
			m_nCurPos = m_nCurPos + llPos;
		}
		break;
	case VO_SOURCE2_IO_POS_END:
		{
			uFlag = VO_FILE_END;
			VO_U64 llSize = 0;
			if (VO_SOURCE2_IO_OK == GetSize(&llSize))
			{
				m_nCurPos = llSize - llPos;
			}
		}
		break;
	default:
		break;

	}
	
	VO_S64 ret = cmnFileSeek(m_hHandle,llPos,uFlag);
	if (llActualPos)
	{
		*llActualPos = ret;
	}
	
	if (ret < 0)
	{
		return VO_SOURCE2_IO_FAIL;
	}
	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::GetSize (VO_U64 * pSize)
{
	voCAutoLock lock(&m_lock);

	if( m_ullRangeFileSize!= 0xFFFFFFFFFFFFFFFFULL )
	{
		//for range case, we should return the length of range
		if (pSize)
		{
			*pSize = (VO_U64)m_ullRangeFileSize;
			VOLOGI("ret range filesize :%llu", m_ullRangeFileSize );
		}
		return VO_SOURCE2_IO_OK;
	}

	if (m_bDrm_content && 0xFFFFFFFFFFFFFFFFULL != m_ullActualFileSize)
	{
		VO_U64 ullFileSize = m_ullActualFileSize;
		VO_U32 uRet = m_pDrm->getOriginalFileSize(&ullFileSize);
		if (VO_SOURCE2_IO_OK != uRet)
		{
			VOLOGW("cann't get file size 0x%x", uRet);
		}
		else
		{
			VOLOGI("Origin file size is %lld", ullFileSize);
			*pSize = ullFileSize;

			return VO_SOURCE2_IO_OK;
		}
	}

	if (!m_hHandle)
	{
		return VO_SOURCE2_IO_FAIL;
	}

	VO_S64 llFileSize = cmnFileSize(m_hHandle);
	if (llFileSize < 0)
	{
		return VO_SOURCE2_IO_FAIL;
	}

	if (pSize)
	{
		* pSize = (VO_U64)llFileSize;
	}

	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::Close (void)
{
	VOLOGI("Close+");
	m_bExitIO = VO_TRUE;
	voCAutoLock lock(&m_lock);

	VO_S32 iRet = 0;
	if (m_hHandle)
	{
		iRet = cmnFileClose(m_hHandle);
		m_hHandle = NULL;
	}
	VOLOGI("Close-");
	return 0 == iRet ? VO_SOURCE2_IO_OK : VO_SOURCE2_IO_FAIL;
}

VO_U32 CSourceIOLocalFile::SetFileSource(VO_CHAR * pSource)
{
	if (!pSource)
	{
		return VO_SOURCE2_IO_NULLPOINTOR;
	}
	memset( m_Source, 0, 2048 );   
#if (defined(_WIN32) || defined(WINCE))
	MultiByteToWideChar( CP_UTF8, 0, pSource, strlen(pSource), m_Source, 2048 );   
#else
	//mbstowcs( m_Source, pSource, 2048 );
	vostrcpy( m_Source , pSource );
#endif
  
	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::GetSource(VO_PTR* pSource)
{
	*pSource = m_Source;
	return VO_SOURCE2_IO_OK;
}

VO_U32 CSourceIOLocalFile::GetCurPos(VO_U64 *llPos)
{
	voCAutoLock lock(&m_lock);
	if (llPos)
	{
		*llPos = m_nCurPos;
	}
	return  VO_SOURCE2_IO_OK;
}