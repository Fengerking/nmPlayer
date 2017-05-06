
/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		CBaseTextBlend.cpp

Contains:	CBaseTextBlend class file

Written by:   Leon Huang
Change History (most recent first):
2011-01-13		Leon		 Create file
*******************************************************************************/
#include "CBaseTextBlend.h"

CBaseTextBlend::CBaseTextBlend(void)
:m_hView(NULL)
,m_pTextBuffer(NULL)
{
//	targetVFormat.nLeft = 100;
//	targetVFormat.nTop = 400;
}

CBaseTextBlend::~CBaseTextBlend(void)
{
	if(m_pTextBuffer ) delete []m_pTextBuffer;
	m_pTextBuffer = NULL;
}
VO_S32 CBaseTextBlend::SetFont(VOEDT_TEXT_FORMAT *textFormat)
{
	if(m_sTextFormat.nSize<=0 ) return VO_ERR_FAILED;
	memcpy(&m_sTextFormat, textFormat, sizeof(VOEDT_TEXT_FORMAT));
	return VO_ERR_NONE;
}

VO_S32 CBaseTextBlend::Init (VO_PTR hView)
{
	m_hView = hView;
	return VO_ERR_NONE;
}
VO_S32 CBaseTextBlend::BlendText(VO_PBYTE pInBuffer, VO_PBYTE pOutBuffer, VO_VIDEO_FORMAT vf, VOEDT_CLIP_POSITION_FORMAT positionFormat)
{
	if(!pInBuffer || !pOutBuffer ) return VO_ERR_FAILED;
	unsigned char* pImage1 = ((VOMP_BUFFERTYPE*)pOutBuffer)->pBuffer;
	unsigned char* pImage2 = ((VOMP_BUFFERTYPE*)pInBuffer)->pBuffer;
	if(!pImage1 || !pImage2 ) return VO_ERR_FAILED;

	VO_S32 left = 0;
	VO_S32 top = 0;
	if(positionFormat.nType == VOEDIT_PID_PERCENTVALUE)
	{
		left =  positionFormat.nLeft * vf.Width /100.;
		top = positionFormat.nTop * vf.Height / 100.;
	}
	else
	{
		left = positionFormat.nLeft;
		top = positionFormat.nTop;
	}


	VO_S32 h = positionFormat.nHeight;
	VO_S32 w = positionFormat.nWidth;

	VO_S32 Y = (left + top * vf.Width )  * 4 ;//RGB32
	VO_BOOL bmask = VO_FALSE;

	unsigned char* pp10 = pImage1  + Y;
	unsigned char* pp20 = pImage2 ;

	for (VO_U32 nH = 0 ; nH < h; ++ nH)
	{
		if(nH+ top > vf.Height) break;
		for (VO_U32 nW = 0; nW < w; nW ++)
		{
			if(nW +left > vf.Width) break;

			bmask= VO_FALSE;
			if(pp20[3] != 0)
			{
				//memcpy(pp10, pp20, 4);
				pp10[0] = pp20[0];
				pp10[1] = pp20[1];
				pp10[2] = pp20[2];
				pp10[3] = pp20[3];
			}
			pp20 += 4;
			pp10 += 4;
			continue;
		}
		pp10 += (vf.Width - w)  *4;
	}
	return VO_ERR_NONE;

}
VO_S32 CBaseTextBlend::BlendText(VO_PBYTE buffer,VO_VIDEO_FORMAT vf,VO_U32 nLeft,VO_U32 nTop)
{
	VO_VIDEO_BUFFER *pImage1 = (VO_VIDEO_BUFFER*)buffer;
	VO_VIDEO_BUFFER *pImage2 = (VO_VIDEO_BUFFER*)&m_pTextBuffer;
	VO_PBYTE ppp = m_pTextBuffer;
	if(!ppp ) return VO_ERR_FAILED;

	VO_S32 left = nLeft;
	VO_S32 top = nTop;

	VO_S32 Y = left + top * vf.Width ;
	VO_S32 UV = left /2+ top/2 * vf.Width/2 ;

	VO_S32 nHeight = m_nHeight;
	VO_S32 nWidth = m_nWidth;

	VO_U32 x = 0;
	VO_U32 xxx =0;
	VO_U32 z =  nWidth  >>1;
	VO_U32 volumn = 0;
	VO_BOOL bmask = VO_FALSE;

	VO_PBYTE pp10 = pImage1->Buffer[0]  + Y;

	VO_PBYTE pp11,pp21,pp12,pp22;
	pp11 = pImage1->Buffer[1] + UV ;
	pp12 = pImage1->Buffer[2] + UV ;


	for (VO_U32 nH = 0 ; nH < nHeight; ++ nH)
	{
		int i = 0;
		volumn = nH %2;		
		if(volumn)
			pp11 = pImage1->Buffer[1] + left /2+ (top + nH)/2 * vf.Width/2;
		else	
			pp12 = pImage1->Buffer[2] + left /2+ (top + nH)/2 * vf.Width/2 ;

		pp10 = pImage1->Buffer[0]  + Y + nH * vf.Width;

		for (VO_U32 nW = 0; nW < nWidth; nW ++)
		{
			bmask= VO_FALSE;

			if(*(ppp++) == 1)
				bmask = VO_TRUE;
			if(bmask)
			{
				pp10[i]  =235 ;
				if(volumn)
					pp11[i>>1]= 128;
				else
					pp12[i>>1]= 0;
			}
			i++;				
		}
	}
	return VO_ERR_NONE;
}
VO_S32 CBaseTextBlend::CreateTextBuffer(VO_CHAR* text, VOEDT_IMG_BUFFER *ppData)
{
	if(ppData)
	{
		if(m_pTextBuffer) delete []m_pTextBuffer;
		m_pTextBuffer = NULL;
		m_nWidth = ppData->nWidth;
		m_nHeight = ppData->nHeight;
		m_pTextBuffer = new VO_BYTE[m_nWidth * m_nHeight * 3];
		memcpy(m_pTextBuffer,ppData->pBuffer,m_nWidth * m_nHeight * 3);
	}
	return VO_ERR_NONE;
}

