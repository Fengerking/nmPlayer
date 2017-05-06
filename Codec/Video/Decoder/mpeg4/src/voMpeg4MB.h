/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/

#ifndef _VO_MPEG4_MB_H_
#define _VO_MPEG4_MB_H_
#include "voMpeg4DecGlobal.h"
#include "voMpeg4Dec.h"
#include "voMpegReadbits.h"
// #if ENABLE_MULTI_THREAD


// extern pthread_mutex_t * VOCONST &(pDec->parserMutex);
// extern pthread_mutex_t * VOCONST &(pDec->readyMutex);
// extern pthread_mutex_t * VOCONST &(pDec->finishMutex);
// extern pthread_cond_t * VOCONST &(pDec->parserCond);
// extern pthread_cond_t * VOCONST &(pDec->readyCond);
// extern pthread_cond_t * VOCONST &(pDec->finishCond);
// extern volatile VO_S32 pDec->nWaiter;
// extern volatile VO_S32 pDec->nParser;
// extern volatile VO_S32 pDec->bEndVop;
// extern volatile VO_S32 pDec->nFinish;
// extern volatile VO_U32 pDec->nParserErr;
// extern volatile VO_S32 pDec->nParserMBPos;
// extern VO_S32 *volatile pDec->pParserMBQuant;
// extern volatile VO_S32 pDec->vfprev;
// extern volatile VO_S32 pDec->vbprev;

extern VO_U32 MBrMPEG4VOP(MBRow_t *pMBr);
extern VO_U32 MBrH263IVOP(MBRow_t *pMBr);
extern VO_U32 MBrH263PVOP(MBRow_t *pMBr);

extern VO_U32 InitialMBThread(VO_MPEG4_DEC * VOCONST pDec);
extern VO_U32 ReleaseMBThread(VO_MPEG4_DEC* pDec);
extern VO_U32 ResetMBThread(VO_MPEG4_DEC* pDec);
// extern VO_U32 SetMBThreadBuff(VOCONST VO_MPEG4_DEC * VOCONST pDec);
extern VO_U32 SetMBThreadVOPFunc(VO_MPEG4_DEC* pDec, VO_U32 (*fpVOP)(struct _MBRow_t *));

extern VO_U32 MPEG4DecMBRow(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL *vDecLocal, VO_MPEG4_VOP_CONTEXT *vopCtxt);
extern void WaitForParser(VO_MPEG4_DEC* pDec);
extern void WaitForSubThdFinish(VO_MPEG4_DEC* pDec);

// #endif


#define MVX(a, q)		(((a)<<16)>>(16+q))
#define MVY(a, q)		((a)>>(16+q))
#define MAKEMV(x,y)	(((y)<<16)|((x)&0xFFFF))

#define MV_SUB(v) (v&1)+((v>>15)&2)
#define MV_SUB_QPEL(v) ((v&3) | ((v>>14)&0xc))
#define MV_SUB_FIELD(v) (v&1)+((v>>16)&2)
#define MV_SUB_QPEL_FIELD(v) ((v&3) | ((v>>15)&0xc))

/*idct*/
#define IDCTSCAN_ZIGZAG		0
#define IDCTSCAN_ALT_HORI	1
#define IDCTSCAN_ALT_VERT	2

//typedef FuncReconMB *azFuncReconMB;

extern void ReconIntraMB(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos);
extern void ReconCopyMB(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos);
extern void ReconCopyMB_B(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos);
extern void ReconInterMB(VO_MPEG4_DEC* pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos);
extern void ReconInterMB_B(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos);
extern void ReconInterMBInterlace(VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos);



extern VO_U32 MPEG4DecIntraMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos, VO_S32 dp_posl);
extern VO_U32 MPEG4DecInterMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL *vDecLocal , VO_S32 nMBPos);
extern void MPEG4DecGmcMB( VO_MPEG4_DEC *pDec, VO_MPEG4_DEC_LOCAL* vDecLocal, VO_S32 nMBPos, VO_S32* mv );
extern void MPEG4DecDirectMB( VO_MPEG4_DEC *pDec, VO_S32 nMBPos, VO_S32 dmv, VO_MPEG4_DEC_LOCAL* vDecLocal);

extern void H263DecIntraMB( VO_MPEG4_DEC *pDec, VO_S32 nMBPos, VO_MPEG4_DEC_LOCAL *vDecLocal);
extern void H263DecInterMB( VO_MPEG4_DEC *pDec, VO_S32 nMBPos, VO_MPEG4_DEC_LOCAL *vDecLocal);


extern VO_S32 CheckResyncMarker(VO_MPEG4_DEC *pDec);


extern VO_S32 GetChromaMV4(const VO_S32* pMV, const VO_S32 bQpel);
extern VO_S32 GetChromaMV(VO_S32 v, const VO_S32 qpel);

extern VO_S32 GetCBPY( VO_MPEG4_DEC* pDec ); //max 6 nBits
extern void GetDPDCDiff(VO_MPEG4_DEC *pDec, VO_U32 nMBPos);

extern VOCONST VO_U8 scanTab[3][64];
extern VOCONST VO_U8 MCBPCtabIntra[];
extern VOCONST VO_U8 MCBPCtabInter[];

static INLINE VO_S32 GetMCBPC_I( VO_MPEG4_DEC* pDec ) //max 9bits
{
	VO_S32 code = SHOW_BITS(pDec,9);

	if (code == 1) {
		FLUSH_BITS(pDec,9); // stuffing
		return -1;
	}

	else if (code < 8) {
		code = 8;
		pDec->nMBError |= ERR_MCBPC;
	}

	code >>= 3;
	if (code >= 32) {
		FLUSH_BITS(pDec,1);
		return 3;
	}
	FLUSH_BITS_LARGE(pDec,MCBPCtabIntra[(code<<1)+1]);
	return MCBPCtabIntra[code<<1];
}

static INLINE VO_S32 GetMCBPC_P( VO_MPEG4_DEC* pDec ) //max 9bits
{
	VO_S32 code = SHOW_BITS(pDec,9);

	if(code == 1){
		FLUSH_BITS(pDec, 9);
		return -1; //stuffing 
	}else if (code <= 0){
		code = 1;
		pDec->nMBError |= ERR_MCBPC;
	}else if (code >= 256){
		FLUSH_BITS(pDec,1);
		return 0;
	}
	FLUSH_BITS_LARGE(pDec,MCBPCtabInter[(code<<1)+1]);
	return MCBPCtabInter[code<<1];
}

static INLINE void clear32(void* p)
{
	VO_S32 *i = (VO_S32*)p;
	i[0] = 0; i[1] = 0;	i[2] = 0; i[3] = 0;
	i[4] = 0; i[5] = 0;	i[6] = 0; i[7] = 0;
}

#define DIV2ROUND(x) (((x)&1)|((x)>>1))

#endif//_VO_MPEG4_MB_H_
