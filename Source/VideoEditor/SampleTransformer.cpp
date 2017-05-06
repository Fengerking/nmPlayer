/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		SampleTransformer.cpp

Contains:	SampleTransformer class file

Written by:	Leon Huang

Change History (most recent first):
2011-02-16		Leon			Create file
*******************************************************************************/
#include "SampleTransformer.h"

CSampleTransformer::CSampleTransformer()
:m_pCCRRR(NULL)
,m_pLibOp(NULL)
{

}

void CSampleTransformer::SetLibOp(VO_LIB_OPERATOR *pLibOp)
{
	m_pLibOp = pLibOp;
}
void CSampleTransformer::Init()
{
	m_pCCRRR = new CCCRRRFunc (VO_FALSE, NULL);
	if (m_pCCRRR != NULL)
	{
		m_pCCRRR->SetLibOperator(m_pLibOp);
		if (m_pCCRRR->LoadLib (NULL) > 0)
		{
			m_pCCRRR->Init (NULL, NULL, 0);
			m_pCCRRR->SetColorType (VO_COLOR_YUV_PLANAR420, VO_COLOR_YUV_PLANAR420);
		}
	}
}

CSampleTransformer::~CSampleTransformer(void)
{
	if(m_pCCRRR) delete m_pCCRRR;
}

VO_S32 CSampleTransformer::DoTransformer(VO_VIDEO_BUFFER *pInData,VO_VIDEO_BUFFER *pOutData,VOEDT_CLIP_POSITION_FORMAT inFormat,VOEDT_CLIP_POSITION_FORMAT outFormat,VO_S64 nStart)
{
	VOLOGI("in DoTransformer");
	VO_U32 nRc = VO_ERR_NONE;
	VO_U32 inWidth,inHeight,outWidth,outHeight;
	inWidth = inFormat.nWidth;
	inHeight = inFormat.nHeight;
	outWidth = outFormat.nWidth;
	outHeight = outFormat.nHeight;
	VOLOGI("in(%d X %d),out(%d X %d)",inWidth,inHeight,outWidth,outHeight);
// 	if(pOutData->Buffer[0] == NULL)
// 	{
// 		if( pOutData->ColorType == VO_COLOR_RGB565_PACKED)
// 		{	
// 			unsigned char *tmpbuf = new unsigned char[outWidth * outHeight * 2];
// 			pOutData->Buffer[0]= tmpbuf;
// 			pOutData->Buffer[1]= tmpbuf;
// 			pOutData->Buffer[2]= tmpbuf;
// 			pOutData->Stride[0]= outWidth *2;
// 			pOutData->Stride[1]= outWidth *2;
// 			pOutData->Stride[2]= outWidth *2;
// 
// 		}
// 		else if(pOutData->ColorType == VO_COLOR_YUV_PLANAR420)
// 		{
// 			unsigned char *tmpbuf = new unsigned char[outWidth * outHeight * 3/2];
// 			pOutData->ColorType = VO_COLOR_YUV_PLANAR420;//(VO_IV_COLORTYPE)outFormat.Type;
// 			pOutData->Buffer[0]= tmpbuf;
// 			pOutData->Buffer[1]= tmpbuf + outWidth * outHeight;
// 			pOutData->Buffer[2]= pOutData->Buffer[1]+ outWidth * outHeight /4;
// 			pOutData->Stride[0]= outWidth;
// 			pOutData->Stride[1]= outWidth/2;
// 			pOutData->Stride[2]= outWidth/2;
// 		}
// 	}
	nRc = m_pCCRRR->SetColorType (pInData->ColorType ,pOutData->ColorType  );
	nRc |= m_pCCRRR->SetCCRRSize(&inWidth ,&inHeight ,&outWidth ,&outHeight,VO_RT_DISABLE);
	CHECK_FAIL(nRc);

	nRc = m_pCCRRR->Process(pInData,pOutData,nStart,VO_TRUE);
	pOutData->Time = pInData->Time;
VOLOGI("out DoTransformer");
	return nRc;
}

VO_S32 CSampleTransformer::DoTransformer2(VO_VIDEO_BUFFER *pInData,VO_VIDEO_BUFFER *pOutData,VO_VIDEO_FORMAT inFormat,VO_VIDEO_FORMAT outFormat,VO_S64 nStart)
{
	VO_U32 nRc = VO_ERR_NONE;
	VO_U32 inWidth,inHeight,outWidth,outHeight;
	inWidth = inFormat.Width;
	inHeight = inFormat.Height;
	outWidth = outFormat.Width;
	outHeight = outFormat.Height;
	VOLOGI("CCRR : in [(%d *%d),type(%d)] ,out[(%d *%d),type(%d)] ",inWidth,inHeight,pInData->ColorType,outWidth,outHeight,pOutData->ColorType);
	nRc = m_pCCRRR->SetColorType (pInData->ColorType ,pOutData->ColorType  );
	nRc |= m_pCCRRR->SetCCRRSize(&inWidth ,&inHeight ,&outWidth ,&outHeight,VO_RT_DISABLE);
	CHECK_FAIL(nRc);

	nRc = m_pCCRRR->Process(pInData,pOutData,nStart,VO_TRUE);
	pOutData->Time = pInData->Time;

	return nRc;
}
