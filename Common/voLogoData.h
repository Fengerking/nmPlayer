	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voLogoData.h

	Contains:	voLogoData header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-12_17		JBF			Create file

*******************************************************************************/
#ifndef __voLogoData_H__
#define __voLogoData_H__

#define	VOLOGO_DATA_WIDTH	32
#define	VOLOGO_DATA_HEIGHT	32

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

extern const unsigned char	* voLogoBufferY;
extern const unsigned char	* voLogoBufferU;
extern const unsigned char	* voLogoBufferV;
extern const unsigned char	* voLogoBufferUV;
extern const unsigned char	* voLogoBufferVU;
extern const unsigned char	* voLogoBuffer565;

#ifdef _VONAMESPACE
}
#endif

#endif //__voLogoData_H__
