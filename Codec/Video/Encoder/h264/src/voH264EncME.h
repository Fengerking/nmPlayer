/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2010						*
*																		*
************************************************************************/
#ifndef _VOH264ENC_ME_H_
#define _VOH264ENC_ME_H_

#define COST_MAX (1<<28)
#define COST_MAX64 (1ULL<<60)

void MotionEstimation( H264ENC *pEncGlobal, VO_S16 (*mvc)[2], VO_S32 i_mvc ,H264ENC_L *pEncLocal);


#define MIN_SWAP1(x,y)\
if((y)<(x))\
    (x)=(y);

#define MIN_SWAP2(x,y,a,b)\
if((y)<(x))\
{\
    (x)=(y);\
    (a)=(b);\
}

#define MIN_SWAP3(x,y,a,b,c,d)\
if((y)<(x))\
{\
    (x)=(y);\
    (a)=(b);\
    (c)=(d);\
}

VO_S32 InitMVCost( H264ENC *pEncGlobal, VO_S32 qp );

void MBEstimation( H264ENC *pEncGlobal, H264ENC_L *pEncLocal );



#endif//_VOH264ENC_ME_H_
