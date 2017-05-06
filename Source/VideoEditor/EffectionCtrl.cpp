/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		EffectionCtrl.cpp

Contains:	EffectionCtrl class file

Written by:	Leon Huang

Change History (most recent first):
2011-01-06		Leon			Create file
*******************************************************************************/

#include "EffectionCtrl.h"
#include "CBitmapFile.h"

CEffectionCtrl::CEffectionCtrl(void)
:m_nCurEffect(VOEDT_TRANSITION_NONE)
,m_pCurMaskData(NULL)
{	
	//m_ppMaskData = new VO_PBYTE[VOEDT_TRANSITION_NONE];
}

CEffectionCtrl::~CEffectionCtrl(void)
{
	if(m_pCurMaskData) delete []m_pCurMaskData;
}
void CEffectionCtrl::SetLibOp(VO_LIB_OPERATOR *libop)
{
	m_SampleTransformer.SetLibOp(libop);
	m_SampleTransformer.Init();
}
void CEffectionCtrl::Uninit()
{
	VO_U32 effect;
	for (effect = (VO_U32)0; effect < VOEDT_TRANSITION_NONE; effect = VO_U32(effect+1))
	{
		if(*(m_ppMaskData +effect)) free( *(m_ppMaskData +effect));
	}
}
VO_VOID CEffectionCtrl::InitMashData()
{
	/*VO_TCHAR maskFile[255];
	VO_U32 effect;
	for (effect = (VO_U32)0; effect < VOEDT_TRANSITION_NONE; effect = VO_U32(effect+1))
	{
		VO_TCHAR ms[]= L"d:/data_cmp/EffectFiles/";
		VO_U32 i = sizeof(ms);
		memcpy(maskFile,ms,i);
		memcpy(maskFile+i/2-1, MASKDATA[effect],255 -i );
		m_ppMaskData[effect] = LoadMashData(maskFile);
	}*/

}
VO_S32 CEffectionCtrl::SetSize(void *pUser,int nWidth,int nHeight)
{
	CEffectionCtrl * ctrl = (CEffectionCtrl*)pUser;
	if(ctrl->m_nWidth != nWidth && ctrl->m_nHeight != nHeight)
	{
		ctrl->m_nWidth = nWidth;
		ctrl->m_nHeight = nHeight;
		ctrl->m_nCurEffect = VOEDT_TRANSITION_NONE;
	}

	return VO_ERR_NONE;
}
VO_PBYTE CEffectionCtrl::LoadMashData(VO_CHAR *maskFile)
{	
	
	CBitmapFile bmpFile;
	if (bmpFile.CreateBitmap (maskFile) < 0)
		return NULL;
	
	VO_PBYTE	pBmpData = NULL;
	pBmpData = bmpFile.GetBitmapBuffer (); 
	if (pBmpData == NULL)
		return NULL;
	VO_S32 souceH = bmpFile.GetHeight();
	VO_S32 souceW = bmpFile.GetWidth();
	VOLOGI("bmpSize (%d x %d)",souceW,souceH);
	VO_PBYTE	pBmpData2 = NULL;
	if(m_nHeight != souceH ||m_nWidth != souceW)
	{
		VO_VIDEO_BUFFER vb1;
		vb1.Buffer[0]= pBmpData;
		vb1.Buffer[1]= pBmpData;
		vb1.Buffer[2]= pBmpData;
		vb1.ColorType=VO_COLOR_RGB565_PACKED;
		vb1.Stride[0]=bmpFile.GetWidth() *2 ;
		vb1.Stride[1]=bmpFile.GetWidth() *2 ;
		vb1.Stride[2]=bmpFile.GetWidth() *2 ;
		VO_VIDEO_BUFFER vb2;
		vb2.ColorType=VO_COLOR_RGB565_PACKED;
		vb2.Buffer[0]= new unsigned char[m_nWidth * m_nHeight * 2];
		vb2.Stride[0] = m_nWidth *2;
		VOEDT_CLIP_POSITION_FORMAT vf1;
		vf1.nHeight = souceH;
		vf1.nWidth = souceW;
		VOEDT_CLIP_POSITION_FORMAT vf2;
		vf2.nHeight = m_nHeight;
		vf2.nWidth = m_nWidth;
		m_SampleTransformer.DoTransformer(&vb1,&vb2,vf1,vf2,0);
		pBmpData2 = vb2.Buffer[0];
		pBmpData = pBmpData2;
	}
	VO_U32 nWidth,nHeight;
	nWidth = m_nHeight;
	nHeight = m_nWidth;

	VO_PBYTE maskBuf = (VO_BYTE*)malloc(nWidth * nHeight);
	VO_U16 * pPixel = (VO_U16 *)pBmpData;
	VO_PBYTE pMask = maskBuf;

	for (VO_U32 nH = 0; nH < nHeight; nH++)
	{
		for (VO_U32 nW = 0; nW < nWidth; nW++)
		{
			*pMask++ = (((*pPixel++) >> 8) & 0XFe);
		}
	}
	if(pBmpData2)delete pBmpData2;
	return maskBuf;
}
VO_S32 CEffectionCtrl::EffectionAudio(void *pUser,VOEDT_TRANSITION_BUFFER* pBuf1,VOEDT_TRANSITION_BUFFER* pBuf2)
{	
	VOLOGI("+ EffectionAudio");
	if( !pBuf2)return VO_ERR_FAILED;
	CEffectionCtrl * ctrl = (CEffectionCtrl*)pUser;
	
	VO_S32 nSize = 0;
	pBuf1->nSize < pBuf2->nSize ? nSize = pBuf1->nSize : nSize = pBuf2->nSize;
	
	float fChangeValue = 0.0;
	if(pBuf2->nType == VOEDT_TRANSITION_AUDIO_MIXING)
		fChangeValue = 0.5;
	else
		fChangeValue = pBuf2->fTransitionValue;

	for(VO_S32 i = 0;i< nSize ;i+=2)
	{
		 VO_S16 buf1 = pBuf1->effectBuffer[i+1] <<8 |pBuf1->effectBuffer[i];
		 VO_S16 buf2 = pBuf2->effectBuffer[i+1] <<8 |pBuf2->effectBuffer[i];
		VO_S32 buf ;
		if(pBuf2->nType == VOEDT_TRANSITION_AUDIO_FADE_IN_FADE_OUT)
			buf = (buf1 * (1.0- fChangeValue) + buf2 * fChangeValue);
		if(pBuf2->nType == VOEDT_TRANSITION_AUDIO_MIXING)
			buf = (buf1 + buf2);
		//if(buf>0xffff)buf= 0xffff;
		if(buf<-0x8000)buf = -0x8000;
		if(buf>0x7fFF)buf= 0x7fFF;
		pBuf1->effectBuffer[i] = buf ;
		pBuf1->effectBuffer[i+1] = buf>>8;

	}
	VOLOGI("- EffectionAudio");
	return VO_ERR_NONE;
}
VO_S32 CEffectionCtrl::EffectionVideo(VO_VOID *pUser,VOEDT_TRANSITION_BUFFER* pBuf1,VOEDT_TRANSITION_BUFFER* pBuf2)
{	
	VOLOGI("++ EffectionVideo");
	if(!(pBuf1 && pBuf2))return VO_ERR_FAILED;
	CEffectionCtrl * ctrl = (CEffectionCtrl*)pUser;
	
	//void *pData1 = pBuf1->effectBuffer;
	//void *pData2 = pBuf2->effectBuffer;
	VO_U32 effect = pBuf2->nType;
	float value = pBuf2->fTransitionValue;
	if(effect == VOEDT_TRANSITION_NONE) return 1;

	if(ctrl->m_nCurEffect != effect)
	{
		if(ctrl->m_pCurMaskData) delete []ctrl->m_pCurMaskData;
		VO_CHAR maskFile[255];
		memcpy(maskFile ,MASKDATA[effect-VOEDT_TRANSITION_VIDEO_BREADCIRCLE],sizeof(maskFile));
		ctrl->m_pCurMaskData = ctrl->LoadMashData(maskFile);
		ctrl->m_nCurEffect = effect;
	}
	VO_PBYTE pMask = ctrl->m_pCurMaskData;
	if(pMask ==NULL) return 1;
	unsigned char *pV1 = (unsigned char*)pBuf1->effectBuffer;
	unsigned char *pV2 = (unsigned char*)pBuf2->effectBuffer;

	VO_U32 nWidth  = ctrl->m_nWidth;
	VO_U32 nHeight = ctrl->m_nHeight;

	ctrl->videoTransition(pMask, pV1, pV2, nWidth,nHeight,value);
	VOLOGI("-- EffectionVideo");
	return 0;
}
VO_S32 CEffectionCtrl::videoTransition(VO_PBYTE pMask,unsigned char *pImage1,unsigned char *pImage2,VO_U32 nWidth,VO_U32 nHeight,float value)
{
	VO_U32 x = 0;
	VO_U32 xxx =0;
	VO_U32 z =  nWidth  >>1;
	VO_U32 volumn = 0;
	VO_BOOL bmask = VO_FALSE;

	VO_PBYTE pp10 = pImage1 ;
	VO_PBYTE pp20 = pImage2 ;
	
	VO_PBYTE ppMask = 	pMask ;

// 	VO_PBYTE pp11,pp21,pp12,pp22;
// 	pp11 = pImage1->Buffer[1] ;
// 	pp21 = pImage2->Buffer[1] ;
// 	pp12 = pImage1->Buffer[2] ;
// 	pp22 = pImage2->Buffer[2] ;

	for (VO_U32 nH = 0; nH < nHeight; ++ nH)
	{
		volumn = nH %2;
		
		for (VO_U32 nW = 0; nW < nWidth; nW ++)
		{

			if ( *ppMask < value)
			{
				*pp10  = *pp20 ;
				*(pp10+1) = *(pp20+1);
				*(pp10+2) = *(pp20+2);
				*(pp10+3) = *(pp20+3);

				bmask = VO_TRUE;
			}
			else
				bmask = VO_FALSE;

// 			if(volumn )
// 			{
// 				if(bmask) *(pp11)= *(pp21);
// 				pp11 += 1;
// 				pp21 += 1;
// 			}
// 			else
// 			{
// 				if(bmask) *(pp12 )= *(pp22) ;
// 
// 				pp12 += 1;
// 				pp22 += 1;
// 			}
			
			pp10 += 4;
			pp20 += 4;
			
			ppMask += 1;
		}

	}
	return VO_ERR_NONE;
}
//VO_S32 CEffectionCtrl::videoTransition(VO_PBYTE pMask,VO_VIDEO_BUFFER *pImage1,VO_VIDEO_BUFFER *pImage2,VO_U32 nWidth,VO_U32 nHeight,float value)
//{
//	VO_U32 x = 0;
//	VO_U32 xxx =0;
//	VO_U32 z = 0;
//	for (VO_U32 nH = 0; nH < nHeight; ++ nH)
//	{
//		z = (nH >>1) * (nWidth >>1); 
//		VO_U32 colum = nH %2 ;
//		for (VO_U32 nW = 0; nW < nWidth; nW+=2)
//		{
//			xxx = nH * nWidth + nW;
//			if (*(pMask + xxx) >= value)
//				;
//			else
//			{
//				*(pImage1->Buffer[0] + xxx)  = *(pImage2->Buffer[0] + xxx) ;
//				*(pImage1->Buffer[0] + xxx+1)= *(pImage2->Buffer[0] + xxx+1);
//
//				x = z + (nW >>1 );
//				colum ? *(pImage1->Buffer[1] + x )= *(pImage2->Buffer[1] + x)
//					:*(pImage1->Buffer[2] + x )= *(pImage2->Buffer[2] + x) ;
//
//			}
//		}
//	}
//	return VO_ERR_NONE;
//}
