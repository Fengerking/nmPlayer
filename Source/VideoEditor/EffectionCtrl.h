/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		EffectionCtrl.h

Contains:	EffectionCtrl header file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#ifndef __EffectionCtrl_H__
#define __EffectionCtrl_H__

#include "videoEditorType.h"
#include "SampleTransformer.h"

#define MASK_MAX 10
class CEffectionCtrl
{
public:
	CEffectionCtrl(void);
	~CEffectionCtrl(void);
	
	static VO_S32 EffectionVideo(void *pUser,VOEDT_TRANSITION_BUFFER* pBuf1,VOEDT_TRANSITION_BUFFER* pBuf2);
	static VO_S32 EffectionAudio(void *pUser,VOEDT_TRANSITION_BUFFER* pBuf1,VOEDT_TRANSITION_BUFFER* pBuf2);
	static VO_S32 SetSize(void *pUser,int nWidth,int nHeight);

	void SetLibOp(VO_LIB_OPERATOR *libop);
private:
	
	VO_S32 videoTransition(VO_PBYTE pMask, unsigned char *pImage1,unsigned char *pImage2,VO_U32 nWidth,VO_U32 nHeight,float value);

	VO_PBYTE m_ppMaskData[MASK_MAX];

	CSampleTransformer m_SampleTransformer;
	VO_PBYTE LoadMashData(VO_CHAR *maskFile);

	VO_U32 m_nCurEffect;
	VO_PBYTE m_pCurMaskData;
	VO_S32 m_nWidth;
	VO_S32 m_nHeight;

	void InitMashData();
	void Uninit();


};
#endif