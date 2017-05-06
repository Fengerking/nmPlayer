	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CSEIInfo.h

	Contains:	CSEIInfo header file

	Change History (most recent first):
	2011-11-30		JBF			Create file

*******************************************************************************/

#ifndef __CSEIINFO_H__
#define __CSEIINFO_H__

#include "CDllLoad.h"
#include "voCMutex.h"
#include "voH264SEIDataBuffer.h"
#include "voOnStreamType.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CSEIInfo : public CDllLoad
{
public:
	CSEIInfo (int nCount);
	virtual ~CSEIInfo (void);

	virtual VO_U32		Init (unsigned char* pBuffer, int nLength, int nType);
	virtual VO_U32		Uninit (void);

	virtual VO_U32		AddSEIData (void* pBuffer);
	virtual VO_U32		GetSEIData (void *pBuffer);
	virtual void*			GetSEISample (void *pSample, int nSEIType);
	virtual VO_U32		Flush (void);
	virtual VO_U32		Enable (int nEnable);

	virtual VO_U32		SetPos (int nCurPos);

	virtual VO_U32		SetParam (VO_S32 uParamID, VO_PTR pData);
	virtual VO_U32		GetParam (VO_S32 uParamID, VO_PTR pData);


	virtual VO_U32		LoadLib (VO_HANDLE hInst);

protected:
	int						m_nSEIType;
	int						m_nSETCount;

	voCMutex				m_Lock;

	/* for VO_SEI_PIC_TIMING */
	voH264SEIDataBuffer*	m_pSEIData;

	/* for VO_SEI_USER_DATA_UNREGISTERED */
	typedef struct _SEI_USERDdata_unRegst
	{
		int	m_nTime;
		VO_H264_USERDATA_Params* m_pSeiUserData;
	} SEI_USER_DATA_UNREGST;
	int	m_nSeiUserDataCount;
	SEI_USER_DATA_UNREGST* m_pSeiUserDataSet;
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CVideoEffect_H__
