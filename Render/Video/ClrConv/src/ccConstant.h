/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/

#ifndef _CC_CONSTANT_H__
#define _CC_CONSTANT_H__

#if defined(VOSYMBIAN)
extern unsigned char	*ccClip255;
extern unsigned char	*ccClip63;
extern unsigned char	*ccClip31;
#else
extern const unsigned char	*ccClip255;
extern const unsigned char	*ccClip63;
extern const unsigned char	*ccClip31;
#endif

#define GDITHER_P0	  0
#define GDITHER_P1	 -3
#define GDITHER_P2	 -2
#define GDITHER_P3	 -1

#define RBDITHER_P0	  2
#define RBDITHER_P1	 -1
#define RBDITHER_P2   1
#define RBDITHER_P3	 -3

#define ConstV1 1673527
#define ConstV2 852492
#define ConstU1 2114977
#define ConstU2 411041
#define ConstY	1220542

#define ARMV7ConstV1 6537
#define ARMV7ConstV2 3330
#define ARMV7ConstU1 8261
#define ARMV7ConstU2 1605
#define ARMV7ConstY	 4767

#define ConstV1_16BIT (0x198937>>8) //1673527
#define ConstV2_16BIT (0xD020C>>8)  //852492
#define ConstU1_16BIT (0x2045A1>>8) //2114977
#define ConstU2_16BIT (0x645A1>>8)  //411041
#define ConstY_16BIT	(0x129FBE>>8) //1220542

#define RBDITHERNEW_P0	  0
#define RBDITHERNEW_P1	  0
#define RBDITHERNEW_P2    0
#define RBDITHERNEW_P3	  0

#define RDITHERNEW_P0	  0
#define RDITHERNEW_P1	  0
#define RDITHERNEW_P2     0
#define RDITHERNEW_P3	  0

#define GDITHERNEW_P0	  0
#define GDITHERNEW_P1	  0
#define GDITHERNEW_P2	  0
#define GDITHERNEW_P3	  0

#define BDITHERNEW_P0	  0
#define BDITHERNEW_P1	  0
#define BDITHERNEW_P2     0
#define BDITHERNEW_P3	  0

#define IDYUYV422	0
#define IDYVYU422	1
#define IDUYVY422	2
#define IDVYUY422	3

#define SAT(Value) (Value) < 0 ? 0: ((Value) > 255 ? 255: (Value));


#endif //_CC_CONSTANT_H__
