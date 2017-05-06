
#ifndef __CSourceIOLocalFile_H__
#define __CSourceIOLocalFile_H__

#include "voYYDef_SourceIO.h"
#include "CSourceIOBase.h"
#include "voFile.h"
#include "voCMutex.h"
#include "DRMLocal.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CSourceIOLocalFile :public CSourceIOBase
{
public:
	CSourceIOLocalFile();
	virtual ~CSourceIOLocalFile();

	VO_U32  Init(VO_PTR pSource , VO_U32 uFlag , VO_SOURCE2_IO_ASYNC_CALLBACK * pAsyncCallback);
	VO_U32	UnInit();
	VO_U32	Open (VO_BOOL bIsAsyncOpen);
	VO_U32 	Read (VO_VOID * pBuffer , VO_U32 uSize , VO_U32 * pReadSize);
	VO_U32 	Write (VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pWrittenSize);
	VO_U32 	Flush ();
	VO_U32 	SetPos (VO_S64 llPos , VO_SOURCE2_IO_POS RelativePos, VO_S64 *llActualPos);
	VO_U32 	GetSize (VO_U64 * pSize);
	VO_U32 	Close ();
	VO_U32  GetLastError(){return VO_SOURCE2_IO_NOTIMPLEMENT;}
	VO_U32  GetParam(VO_U32 uParamID , VO_PTR pParam);
	VO_U32	SetParam(VO_U32 uParamID , VO_PTR pParam);
	VO_U32	GetSource(VO_PTR* pSource);
	VO_U32	GetCurPos(VO_U64 *llPos);
	
protected:
	VO_U32	SetFileSource(VO_CHAR * pSource);

	//initilize DRM object with DRM info
	VO_U32 DrmOpen();

	VO_U32 DrmRead( VO_VOID * pBuf , VO_U32 uSize , VO_U32 * pReadSize );

	VO_U32 BasicRead (VO_VOID * pBuffer , VO_U32 uSize , VO_U32 * pReadSize );

	VO_VOID SetLocalRange( VO_U64 offset , VO_U64 size );

protected:
	VO_TCHAR*		m_Source;
	VO_PTR			m_hHandle;
	VO_BOOL			m_bExitIO;
	voCMutex		m_lock;
	CDRMLocal*		m_pDrm;
	VO_BOOL			m_bDrm_content;

	VO_U64			m_ullActualFileSize;

	VO_S32			m_nPosWithOffset;

	VO_U64			m_ullRangeFileSize;
	VO_U64			m_ullRangeOffset;
};
#ifdef _VONAMESPACE
}
#endif ///< _VONAMESPACE

#endif // __CSourceIOLocalFile_H__
