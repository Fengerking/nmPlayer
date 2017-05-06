/****************************************************************************
*
*   Module Title :     dec_gfuncs.h
*
*   Description  :     Global function ptrs used by decode library.
*
****************************************************************************/
#ifndef __DEC_GFUNCS_H
#define __DEC_GFUNCS_H

extern void VP6_SimpleDeblockFrame ( PB_INSTANCE *pbi, YV12_BUFFER_CONFIG *srcYV12, YV12_BUFFER_CONFIG *dstYV12 );
extern void VP6_DeringFrame(PB_INSTANCE *pbi, YV12_BUFFER_CONFIG *srcYV12, YV12_BUFFER_CONFIG *dstYV12);
extern void VP6_DeblockFrame(PB_INSTANCE *pbi, YV12_BUFFER_CONFIG *srcYV12, YV12_BUFFER_CONFIG *dstYV12);

extern void  (*VP6_PlaneAddNoise)( VP6_POSTPROC_INSTANCE *ppi, UINT8 *Start, UINT32 Width, UINT32 Height, INT32 Pitch, int q);


extern void  (*VP6_FilterHoriz_Simple)(VP6_POSTPROC_INSTANCE *, UINT8 * PixelPtr, INT32 LineLength, INT32*);
extern void  (*VP6_FilterVert_Simple)(VP6_POSTPROC_INSTANCE *, UINT8 * PixelPtr, INT32 LineLength, INT32*);
extern void  (*VP6_DeringBlockWeak)(const VP6_POSTPROC_INSTANCE *, const UINT8 *, UINT8 *, const INT32, UINT32, UINT32 *);
extern void  (*VP6_DeringBlockStrong)(const VP6_POSTPROC_INSTANCE *, const UINT8 *, UINT8 *, const INT32, UINT32, UINT32 *);

extern void  (*VP6_DeblockNonFilteredBandNewFilter)(VP6_POSTPROC_INSTANCE *, UINT8 *, UINT8 *, UINT32, UINT32, UINT32, UINT32 *);
extern INT32*(*VP6_SetupDeblockValueArray)(VP6_POSTPROC_INSTANCE *, INT32 FLimit);

extern void  (*VP6_ClampLevels)(INT32 BlackClamp,	INT32 WhiteClamp, YV12_BUFFER_CONFIG *Src, YV12_BUFFER_CONFIG *Dst);

extern int (*reconFunc[MAX_MODES])( PB_INSTANCE *pbi );

extern void (*idct_dx[65])( INT16 *InputData, INT16 *QuantMatrix, INT16 *OutputData );

/****************************************************************************
* generic version prototypes used by decoder
****************************************************************************/

extern void     VP6_PlaneAddNoise_C( VP6_POSTPROC_INSTANCE *ppi, UINT8 *Start, UINT32 Width, UINT32 Height, INT32 Pitch, int q);

extern void VP6_DeringBlockWeak_C(const VP6_POSTPROC_INSTANCE *ppi, const UINT8 *SrcPtr, UINT8 *DstPtr, const INT32 Pitch, UINT32 FragQIndex, UINT32 *QuantScale);
extern void VP6_DeringBlockStrong_C(const VP6_POSTPROC_INSTANCE *ppi, const UINT8 *SrcPtr, UINT8 *DstPtr, const INT32 Pitch, UINT32 FragQIndex, UINT32 *QuantScale);


//(*reconFunc[MAX_MODES])
extern int     ReconIntra_MB(PB_INSTANCE *pbi);
extern int     ReconInter_MB(PB_INSTANCE *pbi);
extern int     VP6_PredictFiltered_MB(PB_INSTANCE *pbi); 
extern int     VP6_PredictFiltered_MB_60(PB_INSTANCE *pbi); 

#endif
