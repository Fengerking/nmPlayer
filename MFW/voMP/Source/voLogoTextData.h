	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voLogoTextData.h

	Contains:	voLogoTextData header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-12_17		JBF			Create file

*******************************************************************************/
#ifndef __voLogoTextData_H__
#define __voLogoTextData_H__

#define	VOLOGOTEXT_DATA_WIDTH	136
#define	VOLOGOTEXT_DATA_HEIGHT	32

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
#ifndef voLogoTextBufferY
#define voLogoTextBufferY EurnbieMDdCUFJXhkCXvYyv /*-W-*/
#endif
#ifndef voLogoTextBufferU
#define voLogoTextBufferU BUOwIQFLZBReRwITaPfFBxJ /*-W-*/
#endif
#ifndef voLogoTextBufferV
#define voLogoTextBufferV CZYBBubarHmsgvexnjccnBT /*-W-*/
#endif

extern const unsigned char	* voLogoTextBufferY;
extern const unsigned char	* voLogoTextBufferU;
extern const unsigned char	* voLogoTextBufferV;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__voLogoTextData_H__
