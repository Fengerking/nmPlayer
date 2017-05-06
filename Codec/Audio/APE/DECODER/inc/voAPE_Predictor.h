/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010 	    *
*									                                    *
************************************************************************/
/************************************************************************
File:		voAPE_Predictor.h

Contains:	APE Decoder predictor function header

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

*************************************************************************/
#ifndef __VOAPE_PREDICTOR_H__
#define __VOAPE_PREDICTOR_H__
#include "voTypedef.h"

#define   M_COUNT             8
#define   WINDOW_BLOCKS       512
#define   HISTORY_ELEMENTS    8

typedef struct{
	uint32   k;
	uint32   nKSum;
}voUnbit_Array_State;

typedef struct{
	int *m_pData;
	int *m_pCurrent;
}voCircBuf;

typedef struct{
	short *m_pData;
	short *m_pCurrent;
}voCircBufShort;

typedef struct
{
	int multiply;
	int shift;
	int m_nLastValue;
}m_stage1;

typedef struct{
	int                  m_nOrder;
	int                  m_nShift;
	int                  m_nVersion;
	int                  m_nRunningAverage;
	short                *m_paryM;
	voCircBufShort       m_rbInput;
	voCircBufShort       m_rbDeltaM;
}m_nCNNFilter;

typedef struct
{
	int                   m_pNNFilter1Flag;
	int                   m_pNNFilter2Flag;
	int                   m_pNNFilter3Flag;
	int                   m_aryMA[M_COUNT];
	int                   m_aryMB[M_COUNT];
	int                   m_nVersion;
	int                   m_nCurrentIndex;
	int                   m_nLastValueA;
	m_nCNNFilter          *CNNFst1;
	m_nCNNFilter          *CNNFst2;
	m_nCNNFilter          *CNNFst3;
	m_stage1              m_stage1FilterA;
	m_stage1              m_stage1FilterB;
	voCircBuf             m_rbPredictionA;
	voCircBuf             m_rbPredictionB;
	voCircBuf             m_rbAdaptA;
	voCircBuf             m_rbAdaptB;
	voUnbit_Array_State   Array_St;                   /* have two array state, X, Y */
}voCNNFState;

void voAPE_PreInit(voCNNFState *CNNP, int nComplevel, int nVersion);
void voAPE_PreUnInit(voCNNFState *CNNP, int nComplevel);
void voAPEPre_Reset(voCNNFState *CNNP);                       /* running the first block of each frame */
int voDecompressValue(int nA, int nB, voCNNFState *CNNP);

#endif   //__VOAPE_PREDICTOR_H__


