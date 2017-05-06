/****************************************************************************
*
*   Module Title :     recon.c
*
*   Description  :     Frame reconstruction functions.
*
****************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"

/****************************************************************************
*  Macros
****************************************************************************/              

#define Mod8(a) ((a) & 7)


extern void VP6_FilteringHoriz_12 ( UINT32 FLimit, UINT8 *Src, INT32 Pitch );
extern void VP6_FilteringVert_12 ( UINT32 FLimit, UINT8 *Src, INT32 Pitch );
#if (defined(VOARMV4) || defined(VOWMMX) || defined(VOARMV6) || defined(VOARMV7))
extern FORCEINLINE void Copy16x12(const UINT8 *src, UINT8 *dest, UINT32 srcstride);
#endif

const UINT32 VP6_LoopFilterLimitValues[Q_TABLE_SIZE] = 
{ 
    14, 14, 13, 13, 12, 12, 10, 10, 
	10, 10,  8,  8,  8,  8,  8,  8,
	 8,  8,  8,  8,  8,  8,  8,  8,
	 8,  8,  8,  8,  8,  8,  8,  8,  
	 8,  8,  8,  8,  7,  7,  7,  7,	
	 7,  7,  6,  6,  6,  6,  6,  6,	
	 5,  5,  5,  5,  4,  4,  4,  4,  
     4,  4,  4,  3,  3,  3,  3,  2 
};

/****************************************************************************
 * 
 *  ROUTINE       : SatUnsigned8
 *
 *  INPUTS        : INT16 *DataBlock      : Pointer to 8x8 input block.
 *                  UINT32 ResultLineStep : Stride of output block.
 *                  UINT32 DataLineStep   : Stride of input block.
 *
 *  OUTPUTS       : UINT8 *ResultPtr      : Pointer to 8x8 output block.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Saturates the input data to 8 bits unsigned and stores
 *                  in the output buffer.
 *
 *  SPECIAL NOTES : None.
 *
 ****************************************************************************/
void SatUnsigned8 ( UINT8 *ResultPtr, INT16 *DataBlock, UINT32 ResultLineStep, UINT32 DataLineStep )
{
    INT32 i;
       
     // Partly expanded loop
    for ( i=0; i<BLOCK_HEIGHT_WIDTH; i++ )
    {
        ResultPtr[0] = (char) LIMIT(DataBlock[0]);
        ResultPtr[1] = (char) LIMIT(DataBlock[1]);
        ResultPtr[2] = (char) LIMIT(DataBlock[2]);
        ResultPtr[3] = (char) LIMIT(DataBlock[3]);
        ResultPtr[4] = (char) LIMIT(DataBlock[4]);
        ResultPtr[5] = (char) LIMIT(DataBlock[5]);
        ResultPtr[6] = (char) LIMIT(DataBlock[6]);
        ResultPtr[7] = (char) LIMIT(DataBlock[7]);

        DataBlock += DataLineStep;
        ResultPtr += ResultLineStep;
    }
}


/****************************************************************************
 * 
 *  ROUTINE       : ReconBlock_C
 *  
 *  INPUTS        : INT16 *SrcBlock    : Pointer to 8x8 prediction error.
 *					INT16 *ReconRefPtr : Pointer to 8x8 block prediction.
 *                  UINT32 LineStep    : Stride of output block.
 *
 *  OUTPUTS       : UINT8 *DestBlock   : Pointer to 8x8 reconstructed block.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Reconstrut a block by adding the prediction error
 *                  block to the source block and clipping values.
 *
 *  SPECIAL NOTES : None.
 *
 ****************************************************************************/
void ReconBlock_C ( INT16 *SrcBlock, INT16 *ReconRefPtr, UINT8 *DestBlock, UINT32 LineStep )
{
    UINT32 i;
    INT16 *SrcBlockPtr = SrcBlock;

    // For each block row
    for ( i=0; i<BLOCK_HEIGHT_WIDTH; i++ )
    {
        SrcBlock[0] = (INT16)(SrcBlock[0] + ReconRefPtr[0]);
        SrcBlock[1] = (INT16)(SrcBlock[1] + ReconRefPtr[1]);
        SrcBlock[2] = (INT16)(SrcBlock[2] + ReconRefPtr[2]);
        SrcBlock[3] = (INT16)(SrcBlock[3] + ReconRefPtr[3]);
        SrcBlock[4] = (INT16)(SrcBlock[4] + ReconRefPtr[4]);
        SrcBlock[5] = (INT16)(SrcBlock[5] + ReconRefPtr[5]);
        SrcBlock[6] = (INT16)(SrcBlock[6] + ReconRefPtr[6]);
        SrcBlock[7] = (INT16)(SrcBlock[7] + ReconRefPtr[7]);
        
        // Next row...
        SrcBlock    += BLOCK_HEIGHT_WIDTH;
        ReconRefPtr += BLOCK_HEIGHT_WIDTH;
    }

    // Saturate the output to unsigned 8 bit values in recon buffer
    SatUnsigned8( DestBlock, SrcBlockPtr, LineStep, BLOCK_HEIGHT_WIDTH );
}
/****************************************************************************
 * 
 *  ROUTINE       : UnpackBlock_C
 *  
 *  INPUTS        : UINT8 *ReconPtr           : Pointer to reference block.
 *                  UINT32 ReconPixelsPerLine : Stride of reference block.
 *					
 *  OUTPUTS       : UINT16 *ReconRefPtr       : Pointer to output block.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Converts block of 8x8 unsigned 8-bit to block of 
 *                  signed 16-bit.
 *
 *  SPECIAL NOTES : None.
 *
 ****************************************************************************/
void UnpackBlock_C ( UINT8 *ReconPtr, INT16 *ReconRefPtr, UINT32 ReconPixelsPerLine )
{
    UINT32 i;

    // For each block row
    for ( i=0; i<BLOCK_HEIGHT_WIDTH; i++ )
    {
        ReconRefPtr[0] = (INT16)ReconPtr[0];
        ReconRefPtr[1] = (INT16)ReconPtr[1];
        ReconRefPtr[2] = (INT16)ReconPtr[2];
        ReconRefPtr[3] = (INT16)ReconPtr[3];
        ReconRefPtr[4] = (INT16)ReconPtr[4];
        ReconRefPtr[5] = (INT16)ReconPtr[5];
        ReconRefPtr[6] = (INT16)ReconPtr[6];
        ReconRefPtr[7] = (INT16)ReconPtr[7];
        
        // Start next row
        ReconPtr    += ReconPixelsPerLine;
        ReconRefPtr += BLOCK_HEIGHT_WIDTH;
    }
}
#if !(defined(VOARMV4) || defined(VOWMMX) || defined(VOARMV6) || defined(VOARMV7))
/****************************************************************************
 * 
 *  ROUTINE       : Copy16x12_C
 *
 *  INPUTS        : const unsigned char *src : Pointer to source block.
 *                  unsigned int srcstride   : Stride of the source block.
 *                  unsigned int deststride  : Stride of the destination block.
 *
 *  OUTPUTS       : unsigned char *dest      : Pointer to destination block.
 *
 *  RETURNS       : void
 *
 *  FUNCTION      : Copies a 16x12 block from source to destination.
 *
 *  SPECIAL NOTES : Only a 12x12 area is needed, but because most architectures
 *                  are word aligned it can be quicker to copy a 16x12 area
 *                  instead.  The incoming and outgoing char pointers are cast
 *                  to ints and copied over.
 *
 ****************************************************************************/
static FORCEINLINE
void Copy16x12_C(const UINT8 *src, UINT8 *dest, UINT32 srcstride)
{
	const UINT32 *s = (const UINT32 *)src;
	UINT32 *d = (UINT32 *)dest;

	// strides are for chars, divided by four now that ints are being
	//	copied over
	srcstride = srcstride >> 2;

	d[0]  = s[0];
	d[1]  = s[1];
	d[2]  = s[2];
	d[3]  = s[3];
	s += srcstride;
	
	d[4]  = s[0];
	d[5]  = s[1];
	d[6]  = s[2];
	d[7]  = s[3];
	s += srcstride;
	
	d[8]  = s[0];
	d[9]  = s[1];
	d[10]  = s[2];
	d[11]  = s[3];
	s += srcstride;
	
	d[12]  = s[0];
	d[13]  = s[1];
	d[14]  = s[2];
	d[15]  = s[3];
	s += srcstride;
	
	// 4 lines complete

	d[16]  = s[0];
	d[17]  = s[1];
	d[18]  = s[2];
	d[19]  = s[3];
	s += srcstride;
	
	d[20]  = s[0];
	d[21]  = s[1];
	d[22]  = s[2];
	d[23]  = s[3];
	s += srcstride;
	
	d[24]  = s[0];
	d[25]  = s[1];
	d[26]  = s[2];
	d[27]  = s[3];
	s += srcstride;
	
	d[28]  = s[0];
	d[29]  = s[1];
	d[30]  = s[2];
	d[31]  = s[3];
	s += srcstride;
	
	// 8 lines complete

	d[32]  = s[0];
	d[33]  = s[1];
	d[34]  = s[2];
	d[35]  = s[3];
	s += srcstride;
	
	d[36]  = s[0];
	d[37]  = s[1];
	d[38]  = s[2];
	d[39]  = s[3];
	s += srcstride;
	
	d[40]  = s[0];
	d[41]  = s[1];
	d[42]  = s[2];
	d[43]  = s[3];
	s += srcstride;
	
	d[44]  = s[0];
	d[45]  = s[1];
	d[46]  = s[2];
	d[47]  = s[3];
}
#endif

/*************************************************************************** 
 *
 *  ROUTINE       :     Var16Point
 *
 *  INPUTS        :     UINT8 *DataPtr     : Pointer to data block.
 *                      INT32 SourceStride : Block stride.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     UINT32: Calculated 16-point variance (no scaling).
 *
 *  FUNCTION      :     Calculates variance for the 8x8 block *BUT* only samples
 *                      every second pixel in every second row of the block. In
 *                      other words for the 8x8 block only 16 sample points are used.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
UINT32 Var16Point ( UINT8 *DataPtr, INT32 SourceStride )
{
    UINT32  i;
    UINT32  XSum=0;
    UINT32  XXSum=0;
    UINT8   *DiffPtr;

    // Loop expanded out for speed.
    DiffPtr = DataPtr;

    for ( i=0; i<BLOCK_HEIGHT_WIDTH; i+=2 )
    {
        // Examine alternate pixel locations.
        XSum += DiffPtr[0];
        XXSum += DiffPtr[0] * DiffPtr[0];
        XSum += DiffPtr[2];
        XXSum += DiffPtr[2] * DiffPtr[2];
        XSum += DiffPtr[4];
        XXSum += DiffPtr[4] * DiffPtr[4];
        XSum += DiffPtr[6];
        XXSum += DiffPtr[6] * DiffPtr[6];

        // Step to next row of block.
        DiffPtr += (SourceStride << 1);
    }

    // Compute population variance as mis-match metric.
    return (( (XXSum<<4) - XSum*XSum ) ) >> 8;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_PredictFiltered
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *                      UINT8 *SrcPtr    : Pointer to block to be filtered.
 *	                    INT32 mx         :
 *	                    INT32 my         :
 *                      UINT8 **pTempBuffer : Buffer that needs to be aligned.
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Build an 8x8 motion prediction block. If the block is 
 *                      copied across a block boundary, attempt to eliminate 
 *                      the internal border by applying the loop filter internally.
 *
 *  SPECIAL NOTES :     None.
 *
 *****************************************************************************/
int VP6_PredictFiltered
(
	PB_INSTANCE *pbi,
	UINT8 *SrcPtr,
	INT32 mx,
	INT32 my,
    UINT32 bp,
	UINT8 **pTempBuffer
) 
{
	UINT8  *src_plus_recon;
	INT32  offset;
	INT32  mVx, mVy;
	INT32  ReconIndex;
	MACROBLOCK_INFO *mbi=&pbi->mbi;

	INT32  BoundaryX, BoundaryY; 

	// Calculate full pixel motion vector position 
	if(mx > 0 )
		mVx = (mx >> pbi->mbi.blockDxInfo[bp].MvShift);
	else 
		mVx = -((-mx) >> pbi->mbi.blockDxInfo[bp].MvShift);

	if(my > 0 )
		mVy = (my >> pbi->mbi.blockDxInfo[bp].MvShift);
	else
		mVy = -((-my) >> pbi->mbi.blockDxInfo[bp].MvShift);

	// calculate offset in last frame matching motion vector
	ReconIndex = mbi->blockDxInfo[bp].FrameReconStride * mVy + mVx;

	//if ((((SrcPtr + ReconIndex) < pbi->RefFrameReconLow)|| ((SrcPtr + ReconIndex) > pbi->RefFrameReconHigh))  && (bp < 4)){
	//	return -1;
	//}

	// Give our selves a border of 2 extra pixel on all sides (for loop filter and half pixel moves)
	ReconIndex -= ((2 * mbi->blockDxInfo[bp].CurrentReconStride) + 2);

	// copy the 16x12 region starting from reconpixel index into our temp buffer.

	// Check for alignment here so that an aligned 4 byte (int) copy can be made.
	//	Normally a 12x12 copy is made, but here back off for an aligned copy, and then
	//	make the 16x16 copy.  Make sure to realign the output (TempBuffer) to whatever
	//	was done on the source.
	src_plus_recon = SrcPtr + ReconIndex;

	if (((src_plus_recon < pbi->RefFrameReconLow)|| (src_plus_recon > pbi->RefFrameReconHigh))  && (bp < 4)){
		return -1;
	}

	offset = (((UINT32)(src_plus_recon)) & 0x3);
	src_plus_recon = (UINT8 *)((UINT32)src_plus_recon & 0xFFFFFFFC);

	Copy16x12 ( src_plus_recon, *pTempBuffer, mbi->blockDxInfo[bp].CurrentReconStride);

	*pTempBuffer += offset;

	// calculate block border position for x
	BoundaryX = (8 - Mod8(mVx))&7;

	// calculate block border position for y
	BoundaryY = (8 - Mod8(mVy))&7;

	// apply the loop filter at the horizontal boundary we selected
	if(BoundaryX)
		VP6_FilteringHoriz_12(
		VP6_LoopFilterLimitValues[pbi->quantizer->FrameQIndex], 
		*pTempBuffer + 2 + BoundaryX,
		16); 

	// apply the loop filter at the vertical boundary we selected
	if (BoundaryY)
		VP6_FilteringVert_12(
		VP6_LoopFilterLimitValues[pbi->quantizer->FrameQIndex], 
		*pTempBuffer + 2 * 16 + BoundaryY * 16,
		16);
	return 0;
}


