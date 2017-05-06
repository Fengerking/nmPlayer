/****************************************************************************
*
*   Module Title :     Quantise
*
*   Description  :     Quantisation and dequanitsation of an 8x8 dct block. .
*
****************************************************************************/						


/****************************************************************************
*  Header Frames
****************************************************************************/
#include "quantize.h"
#include "gconst.h"
#include "on2_mem.h"

/****************************************************************************
*  Module Statics
****************************************************************************/ 
      
// Scale factors used to improve precision of DCT/IDCT
#define IDCT_SCALE_FACTOR       2       // Shift left bits to improve IDCT precision

// AC Quantizer Tables
static const UINT32 VP6_QThreshTable[Q_TABLE_SIZE] = 
{   94, 92, 90, 88, 86, 82, 78, 74,
    70, 66, 62, 58, 54, 53, 52, 51,
	50, 49, 48, 47, 46, 45, 44, 43,
	42,	40, 39, 37, 36, 35, 34, 33,
    32, 31, 30, 29, 28, 27, 26, 25, 
    24, 23, 22, 21, 20, 19, 18, 17,
    16, 15, 14, 13, 12, 11, 10,  9,  
    8,   7,  6,  5,  4,  3,  2,  1
};

/*
static const UINT32 VP6_UvQThreshTable[Q_TABLE_SIZE] = 
{   94, 92, 90, 88, 86, 82, 78, 74,
    70, 66, 62, 58, 54, 53, 52, 51,
	50, 49, 48, 47, 46, 45, 44, 43,
	42,	40, 39, 37, 36, 35, 34, 33,
    32, 31, 30, 29, 28, 27, 26, 25, 
    24, 23, 22, 21, 20, 19, 18, 17,
    16, 15, 14, 13, 12, 11, 10,  9,  
    8,   7,  6,  5,  4,  3,  2,  1
};
*/


//This contains the offsets used by the token decoder(s) inorder to convert 
//zigzag order into idct input order.
const UINT32 transIndexC[64] = 
{	

    0,  1,  8,  16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,

    35, 42, 49, 56, 57, 50, 43, 36, 
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_init_dequantizer
 *
 *  INPUTS        :     QUANTIZER *qi      : Pointer to quantizer instance.
 *                      UINT8 Vp3VersionNo : Decoder version number (NOT USED)
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Performs initialization of the dequantizer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
VP6_init_dequantizer ( QUANTIZER *qi)
{
    int dequant_coeff;
    int i;

    // DC
    qi->dequant_coeffs[0] = VP6_DcQuant[qi->FrameQIndex] << IDCT_SCALE_FACTOR;

    //AC
	dequant_coeff = VP6_QThreshTable[qi->FrameQIndex] << IDCT_SCALE_FACTOR;

    qi->dequant_coeffs[1] = dequant_coeff;

    if(sizeof(qi->dequant_coeffs) > sizeof(Q_LIST_ENTRY) * 2)
    {
        /* some idct implementation require all 64 dequant coeffs */
        for(i = 2; i < 64; i++)
            qi->dequant_coeffs[i] = dequant_coeff;
    }
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_UpdateQ
 *
 *  INPUTS        :     QUANTIZER *qi      : Pointer to quantizer instance.
 *                      UINT8 Vp3VersionNo : Decoder version number.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Updates the quantisation tables for a new Q.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_UpdateQ ( QUANTIZER *qi, UINT8 Vp3VersionNo )
{  
	if ( qi->FrameQIndex == qi->LastFrameQIndex )
		return;

	// Update the record of last Q index.
    qi->LastFrameQIndex = qi->FrameQIndex;

    //Init transIndex ptr
    qi->transIndex = (UINT32 *)transIndexC;


	// Re-initialise the q tables for forward and reverse transforms.    
	VP6_init_dequantizer(qi);
}


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DeleteQuantizer
 *
 *  INPUTS        :     QUANTIZER **qi : Pointer to pointer to quantizer instance.
 *
 *  OUTPUTS       :     QUANTIZER **qi : Pointer to pointer to quantizer instance,
 *                                       set to NULL on exit.
 *
 *  RETURNS       :     void.
 *
 *  FUNCTION      :     De-allocates memory associated with the quantizer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_DeleteQuantizer ( QUANTIZER **qi )
{
    if ( *qi )
    {
        // De-allocate the quantizer
        duck_free(*qi);
		*qi=0;
    }
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_CreateQuantizer
 *
 *  INPUTS        :     None.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     Pointer to allocated quantizer instance.
 *
 *  FUNCTION      :     Allocated memory for and initializes a quantizer instance.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
QUANTIZER *VP6_CreateQuantizer ( void )
{
	QUANTIZER *qi = 0;
	int quantizer_size = sizeof(QUANTIZER);

//dm642 breaks if align 8 or above	
    qi = (QUANTIZER *) on2_memalign(32, quantizer_size);
    if ( !qi )
        return 0;

	// initialize whole structure to 0
	duck_memset ( (unsigned char *)qi, 0, quantizer_size );
	
	return qi;
}





