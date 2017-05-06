/************************************************************************
*									                                    *
*		VisualOn, Inc. Confidential and Proprietary, 2003 -2010	        *
*									                                    *
************************************************************************/
/***********************************************************************
File:		voAPE_Predictor.c

Contains:	APE Predictor component

Written by:	Huaping Liu

Change History (most recent first):
2009-11-11		LHP			Create file

***********************************************************************/
#include "voAPE_Global.h"
#include "voAPE_Predictor.h"
#include "voAPE_D_API.h"
#include "malloc.h"

void voAPE_PreInit(voCNNFState *CNNP, int nComplevel, int nVersion)
{
	m_nCNNFilter *CNNF1 = CNNP->CNNFst1;
	m_nCNNFilter *CNNF2 = CNNP->CNNFst2;
	m_nCNNFilter *CNNF3 = CNNP->CNNFst3;
	CNNP->m_nVersion = nVersion;
	if(nComplevel == COMPRESSION_LEVEL_FAST)
	{
		CNNP->m_pNNFilter1Flag  = 0;                  //no use
		CNNP->m_pNNFilter2Flag = 0;                 
		CNNP->m_pNNFilter3Flag = 0;
	}
	else if(nComplevel == COMPRESSION_LEVEL_NORMAL)
	{
		CNNP->m_pNNFilter1Flag  = 1;                  //use
		CNNF1 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));
		CNNF1->m_nOrder = 16;
		CNNF1->m_nShift = 11;
		CNNF1->m_nVersion = nVersion;
		CNNF1->m_paryM = (short *)malloc(CNNF1->m_nOrder * sizeof(short));
		CNNF1->m_rbInput.m_pData = (short *)malloc((64 + 16) * sizeof(short));                  //m_rbInput
		CNNF1->m_rbInput.m_pCurrent = (short *)&(CNNF1->m_rbInput.m_pData[16]);
		CNNF1->m_rbDeltaM.m_pData = (short *)malloc((64 + 16) * sizeof(short));                 //m_rbDeltaM
		CNNF1->m_rbDeltaM.m_pCurrent = (short *)&(CNNF1->m_rbDeltaM.m_pData[16]);

		CNNP->CNNFst1 = CNNF1;

		CNNP->m_pNNFilter2Flag = 0;                 
		CNNP->m_pNNFilter3Flag = 0;
	}
	else if(nComplevel == COMPRESSION_LEVEL_HIGH)
	{
		CNNP->m_pNNFilter1Flag  = 1;                  //use
		CNNF1 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));
		CNNF1->m_nOrder = 64;
		CNNF1->m_nShift = 11;
		CNNF1->m_nVersion = nVersion;
		CNNF1->m_paryM = (short *)malloc(CNNF1->m_nOrder * sizeof(short));
		CNNF1->m_rbInput.m_pData = (short *)malloc((64 + 64) * sizeof(short));                     //m_rbInput
		CNNF1->m_rbInput.m_pCurrent = (short *)&(CNNF1->m_rbInput.m_pData[64]);
		CNNF1->m_rbDeltaM.m_pData = (short *)malloc((64 + 64) * sizeof(short));                    //m_rbDeltaM
		CNNF1->m_rbDeltaM.m_pCurrent = (short *)&(CNNF1->m_rbDeltaM.m_pData[64]);

		CNNP->CNNFst1 = CNNF1;

		CNNP->m_pNNFilter2Flag  = 0;                
		CNNP->m_pNNFilter3Flag  = 0;                 

	}
	else if(nComplevel == COMPRESSION_LEVEL_EXTRA_HIGH)
	{
		CNNP->m_pNNFilter1Flag  = 1;                  //use
		CNNF1 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));
		CNNF1->m_nOrder = 256;
		CNNF1->m_nShift = 13;
		CNNF1->m_nVersion = nVersion;
		CNNF1->m_paryM = (short *)malloc(CNNF1->m_nOrder * sizeof(short));
		CNNF1->m_rbInput.m_pData = (short *)malloc((64 + 256) * sizeof(short));                     //m_rbInput
		CNNF1->m_rbInput.m_pCurrent = (short *)&(CNNF1->m_rbInput.m_pData[256]);
		CNNF1->m_rbDeltaM.m_pData = (short *)malloc((64 + 256) * sizeof(short));                    //m_rbDeltaM
		CNNF1->m_rbDeltaM.m_pCurrent = (short *)&(CNNF1->m_rbDeltaM.m_pData[256]);

		CNNP->CNNFst1 = CNNF1;

		CNNP->m_pNNFilter2Flag = 1; 
		CNNF2 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));
		CNNF2->m_nOrder = 32;
		CNNF2->m_nShift = 10;
		CNNF2->m_nVersion = nVersion;
		CNNF2->m_paryM = (short *)malloc(CNNF2->m_nOrder * sizeof(short));
		CNNF2->m_rbInput.m_pData = (short *)malloc((64 + 32) * sizeof(short));                     //m_rbInput
		CNNF2->m_rbInput.m_pCurrent = (short *)&(CNNF2->m_rbInput.m_pData[32]);
		CNNF2->m_rbDeltaM.m_pData = (short *)malloc((64 + 32) * sizeof(short));                    //m_rbDeltaM
		CNNF2->m_rbDeltaM.m_pCurrent = (short *)&(CNNF2->m_rbDeltaM.m_pData[32]);

		CNNP->CNNFst2 = CNNF2;

		CNNP->m_pNNFilter3Flag = 0;
	}
	else if(nComplevel == COMPRESSION_LEVEL_INSANE)
	{
		CNNP->m_pNNFilter1Flag  = 1;                  //use
		CNNF1 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));
		CNNF1->m_nOrder = 1024 + 256;
		CNNF1->m_nShift = 15;
		CNNF1->m_nVersion = nVersion;
		CNNF1->m_paryM = (short *)malloc(CNNF1->m_nOrder * sizeof(short));
		CNNF1->m_rbInput.m_pData = (short *)malloc((64 + 1024 + 256) * sizeof(short));                  //m_rbInput
		CNNF1->m_rbInput.m_pCurrent = (short *)&(CNNF1->m_rbInput.m_pData[1024 + 256]);
		CNNF1->m_rbDeltaM.m_pData = (short *)malloc((64 + 1024 + 256) * sizeof(short));                 //m_rbDeltaM
		CNNF1->m_rbDeltaM.m_pCurrent = (short *)&(CNNF1->m_rbDeltaM.m_pData[1024 + 256]);

		CNNP->CNNFst1 = CNNF1;

		CNNP->m_pNNFilter2Flag = 1;  
		CNNF2 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));	
		CNNF2->m_nOrder = 256;
		CNNF2->m_nShift = 13;
		CNNF2->m_nVersion = nVersion;
		CNNF2->m_paryM = (short *)malloc(CNNF2->m_nOrder * sizeof(short));
		CNNF2->m_rbInput.m_pData = (short *)malloc((64 + 256) * sizeof(short));                     //m_rbInput
		CNNF2->m_rbInput.m_pCurrent = (short *)&(CNNF2->m_rbInput.m_pData[256]);
		CNNF2->m_rbDeltaM.m_pData = (short *)malloc((64 + 256) * sizeof(short));                    //m_rbDeltaM
		CNNF2->m_rbDeltaM.m_pCurrent = (short *)&(CNNF2->m_rbDeltaM.m_pData[256]);

		CNNP->CNNFst2 = CNNF2;

		CNNP->m_pNNFilter3Flag = 1;
		CNNF3 = (m_nCNNFilter *)malloc(sizeof(m_nCNNFilter));
		CNNF3->m_nOrder = 16;
		CNNF3->m_nShift = 11;
		CNNF3->m_nVersion = nVersion;
		CNNF3->m_paryM = (short *)malloc(CNNF3->m_nOrder * sizeof(short));
		CNNF3->m_rbInput.m_pData = (short *)malloc((64 + 16) * sizeof(short));                     //m_rbInput
		CNNF3->m_rbInput.m_pCurrent = (short *)&(CNNF3->m_rbInput.m_pData[16]);
		CNNF3->m_rbDeltaM.m_pData = (short *)malloc((64 + 16) * sizeof(short));                    //m_rbDeltaM
		CNNF3->m_rbDeltaM.m_pCurrent = (short *)&(CNNF3->m_rbDeltaM.m_pData[16]);

		CNNP->CNNFst3 = CNNF3;
	}

}

void voAPE_PreUnInit(voCNNFState *CNNP, int nComplevel)
{
	m_nCNNFilter *CNNF1 = CNNP->CNNFst1;
	m_nCNNFilter *CNNF2 = CNNP->CNNFst2;
	m_nCNNFilter *CNNF3 = CNNP->CNNFst3;
	if(nComplevel == COMPRESSION_LEVEL_FAST)
	{
		CNNP->m_pNNFilter1Flag  = 0;                  //no use
		CNNP->m_pNNFilter2Flag = 0;                 
		CNNP->m_pNNFilter3Flag = 0;
	}
	else if(nComplevel == COMPRESSION_LEVEL_NORMAL)
	{
		if (CNNP->m_pNNFilter1Flag == 1)                  //use
		{
			free(CNNF1->m_paryM);                                                        //free m_paryM
			free(CNNF1->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF1->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF1);                                                                 //free CNNF1
		}
		CNNP->m_pNNFilter2Flag = 0;                 
		CNNP->m_pNNFilter3Flag = 0;
	}
	else if(nComplevel == COMPRESSION_LEVEL_HIGH)
	{
		if(CNNP->m_pNNFilter1Flag == 1)                  //use
		{
			free(CNNF1->m_paryM);                                                        //free m_paryM
			free(CNNF1->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF1->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF1);                                                                 //free CNNF1
		}

		CNNP->m_pNNFilter2Flag  = 0;                
		CNNP->m_pNNFilter3Flag  = 0;                 

	}
	else if(nComplevel == COMPRESSION_LEVEL_EXTRA_HIGH)
	{
		if(CNNP->m_pNNFilter1Flag == 1)                  //use
		{
			free(CNNF1->m_paryM);                                                        //free m_paryM
			free(CNNF1->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF1->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF1);                                                                 //free CNNF1
		}


		if(CNNP->m_pNNFilter2Flag == 1) 
		{
			free(CNNF2->m_paryM);                                                        //free m_paryM
			free(CNNF2->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF2->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF2);                                                                 //free CNNF2
		}

		CNNP->m_pNNFilter3Flag = 0;
	}
	else if(nComplevel == COMPRESSION_LEVEL_INSANE)
	{
		if(CNNP->m_pNNFilter1Flag  == 1)                  //use
		{
			free(CNNF1->m_paryM);                                                        //free m_paryM
			free(CNNF1->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF1->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF1);                                                                 //free CNNF1
		}

		if(CNNP->m_pNNFilter2Flag == 1)
		{
			free(CNNF2->m_paryM);                                                        //free m_paryM
			free(CNNF2->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF2->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF2);                                                                 //free CNNF2
		}

		if(CNNP->m_pNNFilter3Flag == 1)
		{
			free(CNNF3->m_paryM);                                                        //free m_paryM
			free(CNNF3->m_rbInput.m_pData);                                              //free m_rbInput
			free(CNNF3->m_rbDeltaM.m_pData);                                             //free m_rbDeltaM
			free(CNNF3);                                                                 //free CNNF3
		}
	}
}

static int CalDotProduct(short *pA, short *pB, int nOrder)
{
	int  nDotProduct = 0;
	nOrder >>= 4;

	while(nOrder--)
	{
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
		nDotProduct += (*pA++) * (*pB++);
	}

	return nDotProduct;
}


static void Adapt(short *pM, short *pAdapt, int nDirection, int nOrder)
{
	nOrder >>= 4;
	if(nDirection < 0)
	{
		while(nOrder--)
		{
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;		  
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;		  
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;		  
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;
			*pM++ += *pAdapt++;		  
			*pM++ += *pAdapt++;
		}
	}
	else if(nDirection > 0)
	{
		while(nOrder--)
		{
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;		  
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;		  
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;		  
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;
			*pM++ -= *pAdapt++;		  
			*pM++ -= *pAdapt++;
		}
	}
}

static int voCNNF_Decompress(int nInput, m_nCNNFilter *CNNF_Para)
{
	//figure a dot product
	int    nDotProduct;
	int    nTempABS;
	int    nOutput;
	int    m_nVersion = CNNF_Para->m_nVersion;
	int    m_nOrder = CNNF_Para->m_nOrder;
	short  *m_paryM = CNNF_Para->m_paryM;
	voCircBufShort  *m_rbInput = &(CNNF_Para->m_rbInput);
	voCircBufShort  *m_rbDeltaM = &(CNNF_Para->m_rbDeltaM);
	int  m_nShift = CNNF_Para->m_nShift;

	nDotProduct = CalDotProduct(&m_rbInput->m_pCurrent[-m_nOrder], &m_paryM[0], m_nOrder);
	//adapt
	Adapt(&m_paryM[0], &m_rbDeltaM->m_pCurrent[-m_nOrder], nInput, m_nOrder);
	//store the output value
	nOutput = nInput + ((nDotProduct + (1 << (m_nShift - 1))) >> m_nShift);
	//update the input buffer
	m_rbInput->m_pCurrent[0] = (short)((nOutput == (short)(nOutput)) ? nOutput : (nOutput >> 31) ^ 0x7FFF);

	if(m_nVersion >= 3980)
	{
		nTempABS = (nOutput > 0) ? nOutput: -nOutput;
		if(nTempABS > (CNNF_Para->m_nRunningAverage * 3))
			m_rbDeltaM->m_pCurrent[0] = ((nOutput >> 25) & 64) - 32;
		else if(nTempABS > (CNNF_Para->m_nRunningAverage * 4) / 3)
			m_rbDeltaM->m_pCurrent[0] = ((nOutput >> 26) & 32) - 16;
		else if (nTempABS > 0)
			m_rbDeltaM->m_pCurrent[0] = ((nOutput >> 27) & 16) - 8;
		else
			m_rbDeltaM->m_pCurrent[0] = 0;

		CNNF_Para->m_nRunningAverage += (nTempABS - CNNF_Para->m_nRunningAverage) / 16;

		m_rbDeltaM->m_pCurrent[-1] >>= 1;
		m_rbDeltaM->m_pCurrent[-2] >>= 1;
		m_rbDeltaM->m_pCurrent[-8] >>= 1;
	}
	else
	{
		m_rbDeltaM->m_pCurrent[0] = (nOutput == 0) ? 0: (nOutput >> 28 & 8) - 4;
		m_rbDeltaM->m_pCurrent[-4] >>= 1;
		m_rbDeltaM->m_pCurrent[-8] >>= 1;
	}

	//increment and roll if necessary, update the buffer data
	m_rbInput->m_pCurrent++;
	if(m_rbInput->m_pCurrent == &m_rbInput->m_pData[64 + m_nOrder])
	{
		memcpy(&m_rbInput->m_pData[0], &m_rbInput->m_pCurrent[-m_nOrder], m_nOrder * sizeof(short));
		m_rbInput->m_pCurrent = &m_rbInput->m_pData[m_nOrder];
	}

	m_rbDeltaM->m_pCurrent++;
	if(m_rbDeltaM->m_pCurrent == &m_rbDeltaM->m_pData[64 + m_nOrder])
	{
		memcpy(&m_rbDeltaM->m_pData[0], &m_rbDeltaM->m_pCurrent[-m_nOrder], m_nOrder * sizeof(short));
		m_rbDeltaM->m_pCurrent = &m_rbDeltaM->m_pData[m_nOrder];
	}

	return nOutput;
}

int voDecompressValue3930to3950(voAPE_GData *st, int nInput)
{
	int nRetVal = 0;
	return nRetVal;
}

int voDecompressValue(int nA, int nB, voCNNFState *CNNP)
{
	int nPredictionA;
	int nPredictionB;
	int nCurrentA;
	int nRetVal;
	voCircBuf *m_rbPredictionA = &(CNNP->m_rbPredictionA);
	voCircBuf *m_rbPredictionB = &(CNNP->m_rbPredictionB);
	voCircBuf *m_rbAdaptA = &(CNNP->m_rbAdaptA);
	voCircBuf *m_rbAdaptB = &(CNNP->m_rbAdaptB);

	m_stage1 *m_Stage1FilterA = &(CNNP->m_stage1FilterA);
	m_stage1 *m_Stage1FilterB = &(CNNP->m_stage1FilterB);
	m_nCNNFilter *CNNF1 = CNNP->CNNFst1;
	m_nCNNFilter *CNNF2 = CNNP->CNNFst2;
	m_nCNNFilter *CNNF3 = CNNP->CNNFst3;

	if (CNNP->m_nCurrentIndex == WINDOW_BLOCKS)
	{
		/* m_rbAdaptA buffer roll */
		memcpy(&m_rbAdaptA->m_pData[0], &m_rbAdaptA->m_pCurrent[-HISTORY_ELEMENTS], HISTORY_ELEMENTS * sizeof(int));
		m_rbAdaptA->m_pCurrent = &m_rbAdaptA->m_pData[HISTORY_ELEMENTS];

		/* m_rbAdaptB buffer roll */
		memcpy(&m_rbAdaptB->m_pData[0], &m_rbAdaptB->m_pCurrent[-HISTORY_ELEMENTS], HISTORY_ELEMENTS * sizeof(int));
		m_rbAdaptB->m_pCurrent = &m_rbAdaptB->m_pData[HISTORY_ELEMENTS];

		/* m_rbPredictionA buffer roll */
		memcpy(&m_rbPredictionA->m_pData[0], &m_rbPredictionA->m_pCurrent[-HISTORY_ELEMENTS], HISTORY_ELEMENTS * sizeof(int));
		m_rbPredictionA->m_pCurrent = &m_rbPredictionA->m_pData[HISTORY_ELEMENTS];

		/* m_rbPredictionA buffer roll */
		memcpy(&m_rbPredictionB->m_pData[0], &m_rbPredictionB->m_pCurrent[-HISTORY_ELEMENTS], HISTORY_ELEMENTS * sizeof(int));
		m_rbPredictionB->m_pCurrent = &m_rbPredictionB->m_pData[HISTORY_ELEMENTS];

		CNNP->m_nCurrentIndex = 0;
	}
	//stage2 : NNFilter
	if(CNNP->m_pNNFilter3Flag == 1)
		nA = voCNNF_Decompress(nA, CNNF3);
	if(CNNP->m_pNNFilter2Flag == 1)
		nA = voCNNF_Decompress(nA, CNNF2);
	if(CNNP->m_pNNFilter1Flag == 1)
		nA = voCNNF_Decompress(nA, CNNF1);

	//stage1: multiple predictor(order 2 and offset 1)
	*(m_rbPredictionA->m_pCurrent) = CNNP->m_nLastValueA;
	*((m_rbPredictionA->m_pCurrent) -1) = m_rbPredictionA->m_pCurrent[0] - m_rbPredictionA->m_pCurrent[-1];

	//m_rbPredictionB[0] = m_Stage1FilterB.Compress(nB); 
	*(m_rbPredictionB->m_pCurrent) = nB - ((m_Stage1FilterB->m_nLastValue * m_Stage1FilterB->multiply) >> m_Stage1FilterB->shift);
	m_Stage1FilterB->m_nLastValue = nB;

	*((m_rbPredictionB->m_pCurrent) -1) = m_rbPredictionB->m_pCurrent[0] - m_rbPredictionB->m_pCurrent[-1];

	nPredictionA =   (m_rbPredictionA->m_pCurrent[0]  * CNNP->m_aryMA[0]) 
		+ (m_rbPredictionA->m_pCurrent[-1] * CNNP->m_aryMA[1]) 
		+ (m_rbPredictionA->m_pCurrent[-2] * CNNP->m_aryMA[2])
		+ (m_rbPredictionA->m_pCurrent[-3] * CNNP->m_aryMA[3]);

	nPredictionB =   (m_rbPredictionB->m_pCurrent[0]  * CNNP->m_aryMB[0]) 
		+ (m_rbPredictionB->m_pCurrent[-1] * CNNP->m_aryMB[1]) 
		+ (m_rbPredictionB->m_pCurrent[-2] * CNNP->m_aryMB[2])
		+ (m_rbPredictionB->m_pCurrent[-3] * CNNP->m_aryMB[3]) 
		+ (m_rbPredictionB->m_pCurrent[-4] * CNNP->m_aryMB[4]);

	nCurrentA = nA + ((nPredictionA + (nPredictionB >> 1)) >> 10);

	*(m_rbAdaptA->m_pCurrent)      = (m_rbPredictionA->m_pCurrent[0]) ? ((m_rbPredictionA->m_pCurrent[0] >> 30) & 2)-1:0;
	*((m_rbAdaptA->m_pCurrent) -1) = (m_rbPredictionA->m_pCurrent[-1]) ? ((m_rbPredictionA->m_pCurrent[-1] >> 30) & 2)-1:0;

	*(m_rbAdaptB->m_pCurrent)  = (m_rbPredictionB->m_pCurrent[0]) ? ((m_rbPredictionB->m_pCurrent[0] >> 30) & 2)-1:0;
	*((m_rbAdaptB->m_pCurrent) -1) = (m_rbPredictionB->m_pCurrent[-1]) ? ((m_rbPredictionB->m_pCurrent[-1] >> 30) & 2)-1:0;

	if(nA > 0)
	{
		CNNP->m_aryMA[0] -= m_rbAdaptA->m_pCurrent[0];
		CNNP->m_aryMA[1] -= m_rbAdaptA->m_pCurrent[-1];
		CNNP->m_aryMA[2] -= m_rbAdaptA->m_pCurrent[-2];
		CNNP->m_aryMA[3] -= m_rbAdaptA->m_pCurrent[-3];

		CNNP->m_aryMB[0] -= m_rbAdaptB->m_pCurrent[0];
		CNNP->m_aryMB[1] -= m_rbAdaptB->m_pCurrent[-1];
		CNNP->m_aryMB[2] -= m_rbAdaptB->m_pCurrent[-2];
		CNNP->m_aryMB[3] -= m_rbAdaptB->m_pCurrent[-3];
		CNNP->m_aryMB[4] -= m_rbAdaptB->m_pCurrent[-4];		
	}
	else if (nA < 0)
	{
		CNNP->m_aryMA[0] += m_rbAdaptA->m_pCurrent[0];
		CNNP->m_aryMA[1] += m_rbAdaptA->m_pCurrent[-1];
		CNNP->m_aryMA[2] += m_rbAdaptA->m_pCurrent[-2];
		CNNP->m_aryMA[3] += m_rbAdaptA->m_pCurrent[-3];

		CNNP->m_aryMB[0] += m_rbAdaptB->m_pCurrent[0];
		CNNP->m_aryMB[1] += m_rbAdaptB->m_pCurrent[-1];
		CNNP->m_aryMB[2] += m_rbAdaptB->m_pCurrent[-2];
		CNNP->m_aryMB[3] += m_rbAdaptB->m_pCurrent[-3];
		CNNP->m_aryMB[4] += m_rbAdaptB->m_pCurrent[-4];
	}

	//nRetVal = m_Stage1FilterA.Decompress(nCurrentA);
	nRetVal = nCurrentA + ((m_Stage1FilterA->m_nLastValue * m_Stage1FilterA->multiply) >> m_Stage1FilterA->shift);
	m_Stage1FilterA->m_nLastValue = nRetVal;
	CNNP->m_nLastValueA = nCurrentA;

	m_rbPredictionA->m_pCurrent++;

	m_rbPredictionB->m_pCurrent++;
	m_rbAdaptA->m_pCurrent++;
	m_rbAdaptB->m_pCurrent++;

	CNNP->m_nCurrentIndex++;

	return nRetVal;
}

void voAPEPre_Reset(voCNNFState *CNNP)  //running the first block of each frame
{
	int             m_nOrder;
	int             m_nHistoryElements = 64;  //need check the variable
	short           *m_paryM;
	voCircBufShort  m_rbInput;
	voCircBufShort  m_rbDeltaM;

	m_nCNNFilter *CNNF1 = CNNP->CNNFst1;
	m_nCNNFilter *CNNF2 = CNNP->CNNFst2;
	m_nCNNFilter *CNNF3 = CNNP->CNNFst3;

	if(CNNP->m_pNNFilter1Flag)  //flush 1 stage predictor
	{
		m_nOrder = CNNF1->m_nOrder;
		m_paryM  = CNNF1->m_paryM;
		m_rbInput  = CNNF1->m_rbInput;
		m_rbDeltaM = CNNF1->m_rbDeltaM;
		memset(m_paryM, 0, m_nOrder * sizeof(short));
		//m_rbInput.Flush()
		memset(m_rbInput.m_pData, 0, (m_nOrder + 1) * sizeof(short));
		m_rbInput.m_pCurrent = &(m_rbInput.m_pData[m_nOrder]);

		//m_rbDeltaM.Flush()
		memset(m_rbDeltaM.m_pData, 0, (m_nOrder + 1) * sizeof(short));
		m_rbDeltaM.m_pCurrent = &(m_rbDeltaM.m_pData[m_nOrder]);

		CNNF1->m_nRunningAverage = 0;
	}
	if(CNNP->m_pNNFilter2Flag)
	{
		m_nOrder = CNNF2->m_nOrder;
		m_paryM  = CNNF2->m_paryM;
		m_rbInput  = CNNF2->m_rbInput;
		m_rbDeltaM = CNNF2->m_rbDeltaM;
		memset(m_paryM, 0, m_nOrder * sizeof(short));
		//m_rbInput.Flush()
		memset(m_rbInput.m_pData, 0, (m_nOrder + 1) * sizeof(short));
		m_rbInput.m_pCurrent = &(m_rbInput.m_pData[m_nOrder]);

		//m_rbDeltaM.Flush(0
		memset(m_rbDeltaM.m_pData, 0, (m_nOrder + 1) * sizeof(short));
		m_rbDeltaM.m_pCurrent = &(m_rbDeltaM.m_pData[m_nOrder]);

		CNNF2->m_nRunningAverage = 0;
	}
	if(CNNP->m_pNNFilter3Flag)
	{
		m_nOrder = CNNF3->m_nOrder;
		m_paryM  = CNNF3->m_paryM;
		m_rbInput  = CNNF3->m_rbInput;
		m_rbDeltaM = CNNF3->m_rbDeltaM;
		memset(m_paryM, 0, m_nOrder * sizeof(short));
		//m_rbInput.Flush()
		memset(m_rbInput.m_pData, 0, (m_nOrder + 1) * sizeof(short));
		m_rbInput.m_pCurrent = &(m_rbInput.m_pData[m_nOrder]);

		//m_rbDeltaM.Flush(0
		memset(m_rbDeltaM.m_pData, 0, (m_nOrder + 1) * sizeof(short));
		m_rbDeltaM.m_pCurrent = &(m_rbDeltaM.m_pData[m_nOrder]);

		CNNF2->m_nRunningAverage = 0;
	}
	//ZeroMemory(m_aryMA, sizeof(m_aryMA));
	memset(CNNP->m_aryMA, 0, sizeof(CNNP->m_aryMA));

	//ZeroMemory(m_aryMB, sizeof(m_aryMB));
	memset(CNNP->m_aryMB, 0, sizeof(CNNP->m_aryMB));

	//m_rbPredictionA.Flush();
	memset(CNNP->m_rbPredictionA.m_pData, 0, (HISTORY_ELEMENTS + 1) * sizeof(int));
	CNNP->m_rbPredictionA.m_pCurrent = &(CNNP->m_rbPredictionA.m_pData[HISTORY_ELEMENTS]);

	//m_rbPredictionB.Flush();	
	memset(CNNP->m_rbPredictionB.m_pData, 0, (HISTORY_ELEMENTS + 1) * sizeof(int));
	CNNP->m_rbPredictionB.m_pCurrent = &(CNNP->m_rbPredictionB.m_pData[HISTORY_ELEMENTS]);

	//m_rbAdaptA.Flush();
	memset(CNNP->m_rbAdaptA.m_pData, 0, (HISTORY_ELEMENTS + 1) * sizeof(int));
	CNNP->m_rbAdaptA.m_pCurrent = &(CNNP->m_rbAdaptA.m_pData[HISTORY_ELEMENTS]);

	//m_rbAdaptB.Flush();
	memset(CNNP->m_rbAdaptB.m_pData, 0, (HISTORY_ELEMENTS + 1) * sizeof(int));
	CNNP->m_rbAdaptB.m_pCurrent = &(CNNP->m_rbAdaptB.m_pData[HISTORY_ELEMENTS]);

	CNNP->m_aryMA[0] = 360;
	CNNP->m_aryMA[1] = 317;
	CNNP->m_aryMA[2] = -109;
	CNNP->m_aryMA[3] = 98;

	//m_Stage1FilterA.Flush();
	CNNP->m_stage1FilterA.m_nLastValue = 0;
	CNNP->m_stage1FilterA.multiply = 31;
	CNNP->m_stage1FilterA.shift = 5;


	//m_Stage1FilterB.Flush();
	CNNP->m_stage1FilterB.m_nLastValue = 0;
	CNNP->m_stage1FilterB.multiply = 31;
	CNNP->m_stage1FilterB.shift = 5;


	CNNP->m_nLastValueA = 0;

	CNNP->m_nCurrentIndex = 0;
}










