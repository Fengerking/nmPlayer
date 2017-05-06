	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		RGBTable.h

	Contains:	RGBTable header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2006-12_17		JBF			Create file

*******************************************************************************/
#ifndef __RGBTable_H__
#define __RGBTable_H__

extern const unsigned char	* gtRGB8;
extern const unsigned char	* gtRGB6;
extern const unsigned char	* gtRGB5;

extern const int * gRGBYUV601;
extern const int * gRGBYUV709;


#define G_DITHER_P0			0
#define G_DITHER_P1			-3
#define G_DITHER_P2			-4
#define G_DITHER_P3			-1

#define RB_DITHER_P0		2
#define RB_DITHER_P1		3
#define RB_DITHER_P2		1
#define RB_DITHER_P3		-3


/*
#define G_DITHER_P0			0
#define G_DITHER_P1			0
#define G_DITHER_P2			0
#define G_DITHER_P3			0

#define RB_DITHER_P0		0
#define RB_DITHER_P1		0
#define RB_DITHER_P2		0
#define RB_DITHER_P3		0
*/

#endif //__RGBTable_H__
