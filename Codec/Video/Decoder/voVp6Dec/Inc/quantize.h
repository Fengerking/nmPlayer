/****************************************************************************
*
*   Module Title :     quantize.h
*
*   Description  :     Quantizer header file.
*
****************************************************************************/
#ifndef __INC_QUANTIZE_H
#define __INC_QUANTIZE_H


/****************************************************************************
*  Header Files
****************************************************************************/
#include "codec_common.h"


/****************************************************************************
*  Macros
****************************************************************************/
#define MIN16 ((1<<16)-1)

/****************************************************************************
*  Structures
****************************************************************************/
typedef struct 
{
	INT32 QuantCoeffs[2];			   // Quantizer values table

	INT32 QuantRound[2][2];			   // Quantizer rounding table

	INT32 ZeroBinSize[2];			   // Quantizer zero bin table

	INT32 ZlrZbinCorrections[64];	   // Zbin corrections based upon zero run length.

#ifndef NDS_NITRO
	INT32 ZlrZbinACCorrections[64];    // Zbin corrections for AC components
#endif

#if 0
#ifdef _X86_
    /* 
        The mmx idct uses all 64.  It only needs to use [0] and [1] because all of 
        the ac are the same.
    */
	Q_LIST_ENTRY  dequant_coeffs[64];	
#else
    /*
        
    */
	Q_LIST_ENTRY  dequant_coeffs[2];	
#endif
#else
	Q_LIST_ENTRY  dequant_coeffs[64];	
#endif

	UINT32 FrameQIndex;					   // Quality specified as a table index 
	UINT32 LastFrameQIndex;	
    UINT32 *transIndex;					   // array to reorder zig zag to idct's ordering

    ALIGN16 short round[8];
    ALIGN16 short mult[8];
    ALIGN16 short zbin[8];

} QUANTIZER;

/****************************************************************************
*  Exports
****************************************************************************/
extern const Q_LIST_ENTRY VP6_DcQuant[Q_TABLE_SIZE];
extern const Q_LIST_ENTRY VP6_UvDcQuant[ Q_TABLE_SIZE ];

extern void (*VP6_quantize) ( QUANTIZER *pbi, INT16 * DCT_block, Q_LIST_ENTRY * quantized_list, UINT8 bp, INT16 *ZeroCounts );

extern void VP6_UpdateQ ( QUANTIZER *pbi, UINT8 Vp3VersionNo );
extern void VP6_UpdateQC ( QUANTIZER *pbi, UINT8 Vp3VersionNo );

extern QUANTIZER * VP6_CreateQuantizer ( void );
extern void VP6_DeleteQuantizer ( QUANTIZER **pbi );          


extern void VP6_init_dequantizer ( QUANTIZER *qi);


extern void VP6_quantize_c
( 
    QUANTIZER * qi, 
    INT16 * DCT_block, 
    Q_LIST_ENTRY * quantized_list, 
    UINT8 bp,
    INT16 *zeroCoeffCount
);

#endif
