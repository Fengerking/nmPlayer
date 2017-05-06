 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/
/** \file     h265dec_sao.h
    \brief    Sample Adaptive Offset(SAO) processing module
    \author   Lina Lv
		\change
*/
#ifndef __H265DEC_SAO_H__
#define __H265DEC_SAO_H__

#include "h265dec_config.h"
#include "h265dec_porting.h"
#include "h265dec_ID.h"





// ====================================================================================================================
// Constants
// ====================================================================================================================
#define SAO_DELETE_QT									     1
#define MAXX_PIXEL_RANGE                   256
#define MAX_SLICE_NUM                      600

#define MAX_LCU_SIZE                       64



// ====================================================================================================================
// struct and enum definition
// ====================================================================================================================
/// Non-deblocking filter processing block border tag
enum NDBFBlockBorderTag
{
  SGU_L = 0,
  SGU_R,
  SGU_T,
  SGU_B,
  SGU_TL,
  SGU_TR,
  SGU_BL,
  SGU_BR,
  NUM_SGU_BORDER
};

typedef enum
{
	SAO_NONE = -1,
	SAO_EO_0 = 0, 
	SAO_EO_1,
	SAO_EO_2, 
	SAO_EO_3,
	SAO_BO,
	MAX_NUM_SAO_TYPE
}SAOType;

typedef enum
{
	SAO_EO_LEN    = 4, 
	SAO_BO_LEN    = 4,
	SAO_MAX_BO_CLASSES = 32
} SAOTypeLen;



typedef struct _SaoLcuParam
{
	//VO_U8      sao_merge_up_flag;
	//VO_U8      sao_merge_left_flag;
	VO_S8      sao_type_idx[4];//luma.chroma
	VO_S8      sao_offset_abs[3][4];
	VO_S8      sao_band_position[4];                  ///< indicates EO class or BO band position
} SaoLcuParam; 

typedef struct _SaoOffsetTable
{
  VO_S8       m_iOffsetEo[8];    //5 is ok, just for align   // data must be m_iOffsetEo + m_iOffsetBo
  VO_U8       m_iOffsetBo[MAXX_PIXEL_RANGE];

} SaoOffsetTable; 

/// Non-deblocking filter processing block information
typedef struct NDBFBlockInfo
{

  VO_BOOL  isBorderAvailable[NUM_SGU_BORDER];  //!< the border availabilities
}NDBFBlockInfo;

typedef struct 
{ 
  //SaoLcuParam* saoLcuParam;       
  //SaoOffsetTable *pSaoOffsetTable;   // one more than actual size, ,because mergeLeftFlag
                                     // L00Y, L01Y,   L06Y.....
                                     // L00U, L01U,   L06U.....
                                     // L00V, L01V,   L06V.....
                                     // L00Y, L01Y,   L06Y.....
                                     // L10U, L11U,   L16U.....
                                     // L10V, L11V,   L16V.....
  SaoOffsetTable *pSaoOffsetTableLCU;  //must init to -1 index, because mergeLeftFlag
  //VO_U8   *m_lumaTableBo;

  VO_U8*  m_pTmpU;            //
  VO_U32  m_pTmpUTopIndex[3];    //can not share between Y, U and V

  VO_U8*  m_pTmpL[2];            //
  VO_U32  m_pTmpLLeftIndex[6];   //Y up, Y down; U up, U down; V up, V down;
  VO_U32  m_CurLeftIndex;        //value of  m_pTmpLLeftIndex


  VO_U8   *m_iUpBuff1All[3];
  VO_U8   *m_iUpBuff1; 


  //variables for each LCU,  set following fields in C before calling processSaoBlock
  VO_U8* pLCURec;
  VO_U32 uiLCULPelX;
  VO_U32 uiLCUTPelY;
  VO_U32 uiLCUStride;  //lumaStride or chromaStride 
  VO_U32 uiStartXByLCU;  //[uiStartXByLCU
  VO_U32 uiEndXByLCU;     //   uiEndXByLCU)
  VO_U32 uiLCURowStartY;  //[uiLCURowStartY
  VO_U32 uiLCURowEndY;     //   uiLCURowEndY)
  VO_BOOL  isNeedCopyL;

  //original 
  VO_U32          numCuInHeight;
  VO_U32          numCuInWidth;
  VO_U32          m_uiMaxCUWidth, m_iPicWidth;  
  VO_U32          m_uiMaxCUHeight, m_iPicHeight;

  //for multi slice and tile
  //VO_U32          *m_sliceIDMapLCU; //slice ID of each LCU
  //VO_BOOL  m_bUseNIF;       //!< true for performing non-cross slice boundary ALF   
  //NDBFBlockInfo *m_vNDFBlock;//default value, picture boundary, tile boundary, slice boundary
  //VO_U32        m_uiNumSlicesInPic;
 
}TComSampleAdaptiveOffset;




  //public APIs

  struct H265_DEC;
  struct H265_DEC_SLICE;


  VO_VOID setNDBBorderAvailabilityPerPPS(struct H265_DEC_SLICE *p_slice, VO_VOID *phPPS, TComSampleAdaptiveOffset *pSAO, VO_U32 *puiTileIdxMap);


  VO_VOID SAOProcessOneLCU(VO_VOID* phSlice, TComSampleAdaptiveOffset* pSAO, VO_S32 iAddrx);


  //internal functions 


  VO_VOID processSao(VO_VOID *ph_dec,TComSampleAdaptiveOffset* pSAO, VO_S32 iYCbCr, VO_S32 iAddr);

  VO_VOID processSaoBlock(TComSampleAdaptiveOffset *pSAO, VO_S32 iSaoType, VO_S32 iIsChroma, VO_U8 *pTmpU, VO_U8 *pTmpL );
 

#endif
