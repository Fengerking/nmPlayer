/****************************************************************************
*
*   Module Title :     PB_Globals.c
*
*   Description  :     Video CODEC Demo: playback dll global declarations
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "on2_mem.h"

/****************************************************************************
*  Imports
****************************************************************************/  

/****************************************************************************
*  Exports
****************************************************************************/  
UINT8 LimitVal_VP6[VAL_RANGE * 3];


/****************************************************************************
*  Module Static Variables
****************************************************************************/  
static const struct 
{
	INT32 row;
	INT32 col;
} NearMacroBlocks[12] = 
{
	{ -1,  0 },
    {  0, -1 },
	{ -1, -1 },
	{ -1,  1 },
	{ -2,  0 },
    {  0, -2 },
	{ -1, -2 },
	{ -2, -1 },
	{ -2,  1 },
	{ -1,  2 },
	{ -2, -2 },
	{ -2,  2 }
};

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_InitMBI
 *
 *  INPUTS        :     PB_INSTANCE * pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Initialize MBI structure.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
VP6_InitMBI(PB_INSTANCE *pbi)
{

    pbi->mbi.blockDxInfo[0].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[1].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[2].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[3].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[4].ZeroRunProbsBasePtr = 
    pbi->mbi.blockDxInfo[5].ZeroRunProbsBasePtr = (UINT8 *)pbi->ZeroRunProbs;

    pbi->mbi.blockDxInfo[0].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[1].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[2].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[3].AcProbsBasePtr = pbi->AcProbs + ACProbOffset(0,0,0,0);
    pbi->mbi.blockDxInfo[4].AcProbsBasePtr = 
    pbi->mbi.blockDxInfo[5].AcProbsBasePtr = pbi->AcProbs + ACProbOffset(1,0,0,0);

    pbi->mbi.blockDxInfo[0].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[1].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[2].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[3].DcProbsBasePtr = pbi->DcProbs + DCProbOffset(0,0);
    pbi->mbi.blockDxInfo[4].DcProbsBasePtr = 
    pbi->mbi.blockDxInfo[5].DcProbsBasePtr = pbi->DcProbs + DCProbOffset(1,0);

    pbi->mbi.blockDxInfo[0].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[1].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[2].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[3].DcNodeContextsBasePtr = pbi->DcNodeContexts + DcNodeOffset(0,0,0);
    pbi->mbi.blockDxInfo[4].DcNodeContextsBasePtr = 
    pbi->mbi.blockDxInfo[5].DcNodeContextsBasePtr = pbi->DcNodeContexts + DcNodeOffset(1,0,0);

//dequantPtr can be moved out of blockDxInfo and into mbi
    pbi->mbi.blockDxInfo[0].dequantPtr = &pbi->quantizer->dequant_coeffs[0];
    pbi->mbi.blockDxInfo[1].dequantPtr = &pbi->quantizer->dequant_coeffs[0];
    pbi->mbi.blockDxInfo[2].dequantPtr = &pbi->quantizer->dequant_coeffs[0];
    pbi->mbi.blockDxInfo[3].dequantPtr = &pbi->quantizer->dequant_coeffs[0];
    pbi->mbi.blockDxInfo[4].dequantPtr = &pbi->quantizer->dequant_coeffs[0];
    pbi->mbi.blockDxInfo[5].dequantPtr = &pbi->quantizer->dequant_coeffs[0];


    pbi->mbi.blockDxInfo[0].LastDc = 
    pbi->mbi.blockDxInfo[1].LastDc = 
    pbi->mbi.blockDxInfo[2].LastDc = 
    pbi->mbi.blockDxInfo[3].LastDc = pbi->fc.LastDcY;
    pbi->mbi.blockDxInfo[4].LastDc = pbi->fc.LastDcU;
    pbi->mbi.blockDxInfo[5].LastDc = pbi->fc.LastDcV;

    pbi->mbi.blockDxInfo[0].Left = &pbi->fc.LeftY[0];
    pbi->mbi.blockDxInfo[1].Left = &pbi->fc.LeftY[0];
    pbi->mbi.blockDxInfo[2].Left = &pbi->fc.LeftY[1];
    pbi->mbi.blockDxInfo[3].Left = &pbi->fc.LeftY[1];
    pbi->mbi.blockDxInfo[4].Left = &pbi->fc.LeftU;
    pbi->mbi.blockDxInfo[5].Left = &pbi->fc.LeftV;

	pbi->mbi.blockDxInfo[0].MvShift =
	pbi->mbi.blockDxInfo[1].MvShift =
	pbi->mbi.blockDxInfo[2].MvShift =
	pbi->mbi.blockDxInfo[3].MvShift = Y_MVSHIFT;
	pbi->mbi.blockDxInfo[4].MvShift =
	pbi->mbi.blockDxInfo[5].MvShift = UV_MVSHIFT;

	pbi->mbi.blockDxInfo[0].MvModMask =
	pbi->mbi.blockDxInfo[1].MvModMask =
	pbi->mbi.blockDxInfo[2].MvModMask =
	pbi->mbi.blockDxInfo[3].MvModMask = Y_MVMODMASK;
	pbi->mbi.blockDxInfo[4].MvModMask =
	pbi->mbi.blockDxInfo[5].MvModMask = UV_MVMODMASK;

	pbi->mbi.blockDxInfo[0].CurrentReconStride =
	pbi->mbi.blockDxInfo[1].CurrentReconStride =
	pbi->mbi.blockDxInfo[2].CurrentReconStride =
	pbi->mbi.blockDxInfo[3].CurrentReconStride = pbi->Configuration.YStride;
	pbi->mbi.blockDxInfo[4].CurrentReconStride =
	pbi->mbi.blockDxInfo[5].CurrentReconStride = pbi->Configuration.UVStride;

	pbi->mbi.blockDxInfo[0].FrameReconStride =
	pbi->mbi.blockDxInfo[1].FrameReconStride =
	pbi->mbi.blockDxInfo[2].FrameReconStride =
	pbi->mbi.blockDxInfo[3].FrameReconStride = pbi->Configuration.YStride;
	pbi->mbi.blockDxInfo[4].FrameReconStride =
	pbi->mbi.blockDxInfo[5].FrameReconStride = pbi->Configuration.UVStride;

    // Default clear data area down to 0s
    on2_memset(pbi->mbi.coeffsBasePtr, 0, 6*64*sizeof(Q_LIST_ENTRY));

    //______ compressor only ______
	pbi->mbi.blockDxInfo[0].FrameSourceStride =
	pbi->mbi.blockDxInfo[1].FrameSourceStride =
	pbi->mbi.blockDxInfo[2].FrameSourceStride =
	pbi->mbi.blockDxInfo[3].FrameSourceStride = pbi->Configuration.VideoFrameWidth;
	pbi->mbi.blockDxInfo[4].FrameSourceStride =
	pbi->mbi.blockDxInfo[5].FrameSourceStride = pbi->Configuration.VideoFrameWidth/2;

	pbi->mbi.blockDxInfo[0].CurrentSourceStride =
	pbi->mbi.blockDxInfo[1].CurrentSourceStride =
	pbi->mbi.blockDxInfo[2].CurrentSourceStride =
	pbi->mbi.blockDxInfo[3].CurrentSourceStride = pbi->Configuration.VideoFrameWidth;
	pbi->mbi.blockDxInfo[4].CurrentSourceStride =
	pbi->mbi.blockDxInfo[5].CurrentSourceStride = pbi->Configuration.VideoFrameWidth/2;

    //______ compressor only ______
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_AllocateFragmentInfo
 *
 *  INPUTS        :     PB_INSTANCE * pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     BOOL: TRUE if successful, FALSE on error.
 *
 *  FUNCTION      :     Initializes the Playback instance passed in.
 *
 *  SPECIAL NOTES :     Uses duck_memalign to ensure data structures are aligned
 *                      on 32-byte boundaries to improve cache performance. 
 *
 ****************************************************************************/
BOOL VP6_AllocateFragmentInfo ( PB_INSTANCE *pbi )
{
	// Clear any existing info
	VP6_DeleteFragmentInfo(pbi);
    pbi->mbi.coeffsBasePtr = (Q_LIST_ENTRY *) duck_memalign(32, sizeof(Q_LIST_ENTRY)*64*6);
    if(!pbi->mbi.coeffsBasePtr) 
    {
    	VP6_DeleteFragmentInfo(pbi); 
    	return FALSE;
    }
    pbi->mbi.blockDxInfo[0].coeffsPtr = pbi->mbi.coeffsBasePtr;
    pbi->mbi.blockDxInfo[1].coeffsPtr = pbi->mbi.blockDxInfo[0].coeffsPtr + 64;
    pbi->mbi.blockDxInfo[2].coeffsPtr = pbi->mbi.blockDxInfo[1].coeffsPtr + 64;
    pbi->mbi.blockDxInfo[3].coeffsPtr = pbi->mbi.blockDxInfo[2].coeffsPtr + 64;
    pbi->mbi.blockDxInfo[4].coeffsPtr = pbi->mbi.blockDxInfo[3].coeffsPtr + 64;
    pbi->mbi.blockDxInfo[5].coeffsPtr = pbi->mbi.blockDxInfo[4].coeffsPtr + 64;

    // context allocations
    pbi->fc.AboveY = (BLOCK_CONTEXT *) duck_memalign(32, (8+pbi->HFragments) * sizeof(BLOCK_CONTEXT));
    if(!pbi->fc.AboveY) { VP6_DeleteFragmentInfo(pbi); return FALSE;}

    pbi->fc.AboveU = (BLOCK_CONTEXT *) duck_memalign(32, (8+pbi->HFragments / 2) * sizeof(BLOCK_CONTEXT));
    if(!pbi->fc.AboveU) { VP6_DeleteFragmentInfo(pbi); return FALSE;}

    pbi->fc.AboveV = (BLOCK_CONTEXT *) duck_memalign(32, (8+pbi->HFragments / 2) * sizeof(BLOCK_CONTEXT));
    if(!pbi->fc.AboveV) { VP6_DeleteFragmentInfo(pbi); return FALSE;}

    pbi->predictionMode = (char *) duck_memalign(32, pbi->MacroBlocks * sizeof(char));
    if(!pbi->predictionMode) { VP6_DeleteFragmentInfo(pbi); return FALSE;}

    pbi->MBMotionVector = (MOTION_VECTOR *) duck_memalign(32, pbi->MacroBlocks * sizeof(MOTION_VECTOR ));
    if(!pbi->MBMotionVector) { VP6_DeleteFragmentInfo(pbi); return FALSE;}
	
    return TRUE;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DeleteFrameInfo
 *
 *  INPUTS        :     PB_INSTANCE * pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     De-allocate memory associated with frame level data
 *                      structures.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_DeleteFrameInfo ( PB_INSTANCE *pbi )
{
	int i = 0;
	for(i=0; i< MAXFRAMES; i++)
	{
		On2YV12_DeAllocFrameBuffer(&pbi->FrameYV12Config[i]);
	}

	On2YV12_DeAllocFrameBuffer(&pbi->goldenFrameYV12Config);

	pbi->GoldenFrame            = 0;


}
/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DeleteTmpBuffers
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     De-allocate buffers used during decoing.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void
VP6_DeleteTmpBuffers ( PB_INSTANCE *pbi )
{ 
	//if ( pbi->mbi.blockDxInfo[0].idctOutputPtr )
	//	duck_free(pbi->mbi.blockDxInfo[0].idctOutputPtr);

	if ( pbi->LoopFilteredBlock )
		duck_free(pbi->LoopFilteredBlock);
	if ( pbi->TmpDataBuffer )
		duck_free(pbi->TmpDataBuffer);
    
	//pbi->mbi.blockDxInfo[0].idctOutputPtr     = 0;
	pbi->LoopFilteredBlock      = 0;
	pbi->TmpDataBuffer          = 0;
}


/****************************************************************************
 * 
 *  ROUTINE       :     VP6_AllocateTmpBuffers
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     BOOL: Always 0.
 * 
 *  FUNCTION      :     Allocates buffers required during decoding.
 *
 *  SPECIAL NOTES :     Uses ROUNDUP32 to ensure that buffers are aligned
 *                      on 32-byte boundaries to improve cache performance.
 *
 ****************************************************************************/
INT32
VP6_AllocateTmpBuffers ( PB_INSTANCE *pbi )
{

	// clear any existing info
	VP6_DeleteTmpBuffers ( pbi ); 

    //pbi->mbi.blockDxInfo[0].idctOutputPtr = (INT16 *)duck_memalign(32, 6*64*sizeof(INT16));
    //pbi->mbi.blockDxInfo[1].idctOutputPtr = pbi->mbi.blockDxInfo[0].idctOutputPtr + 64;
    //pbi->mbi.blockDxInfo[2].idctOutputPtr = pbi->mbi.blockDxInfo[1].idctOutputPtr + 64;
    //pbi->mbi.blockDxInfo[3].idctOutputPtr = pbi->mbi.blockDxInfo[2].idctOutputPtr + 64;
    //pbi->mbi.blockDxInfo[4].idctOutputPtr = pbi->mbi.blockDxInfo[3].idctOutputPtr + 64;
    //pbi->mbi.blockDxInfo[5].idctOutputPtr = pbi->mbi.blockDxInfo[4].idctOutputPtr + 64;

    pbi->TmpDataBuffer        = (INT16 *)duck_memalign(32, 64 * sizeof(INT16));
    if ( !pbi->TmpDataBuffer )        { VP6_DeleteTmpBuffers(pbi); return FALSE;};

//	pbi->LoopFilteredBlock        = (UINT8 *)duck_memalign(32, 12 * 16 * 6  * sizeof(UINT8) );
	pbi->LoopFilteredBlock        = (UINT8 *)duck_memalign(32, 12 * 16 * 6  * sizeof(short) );
    if ( !pbi->LoopFilteredBlock )        { VP6_DeleteTmpBuffers(pbi); return FALSE;};


    return 0;
}
/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DeleteFragmentInfo
 *
 *  INPUTS        :     PB_INSTANCE * pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     De-allocates memory associated with decoder data structures.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_DeleteFragmentInfo ( PB_INSTANCE *pbi )
{
    if(pbi->mbi.coeffsBasePtr)
        duck_free(pbi->mbi.coeffsBasePtr);		
    pbi->mbi.coeffsBasePtr = 0;

    if(	pbi->fc.AboveY)
		duck_free(pbi->fc.AboveY);
    pbi->fc.AboveY      = 0;

	if(	pbi->fc.AboveU)
		duck_free(pbi->fc.AboveU);
    pbi->fc.AboveU      = 0;

	if(	pbi->fc.AboveV)
		duck_free(pbi->fc.AboveV);
    pbi->fc.AboveV      = 0;

	if(	pbi->MBMotionVector)
		duck_free(pbi->MBMotionVector);
    pbi->MBMotionVector      = 0;

	if(	pbi->predictionMode)
		duck_free(pbi->predictionMode);
    pbi->predictionMode      = 0;		
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_AllocateFrameInfo
 *
 *  INPUTS        :     PB_INSTANCE * pbi      : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None
 *
 *  RETURNS       :     BOOL: TRUE if successful, FALSE on error.
 *
 *  FUNCTION      :     Initializes the Playback instance passed in
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
BOOL VP6_AllocateFrameInfo ( PB_INSTANCE *pbi )
{
	UINT32 i = 0;
	// clear any existing info
	VP6_DeleteFrameInfo(pbi);


  if ( On2YV12_AllocFrameBuffer(&pbi->goldenFrameYV12Config, 
    pbi->Configuration.VideoFrameWidth , 
    pbi->Configuration.VideoFrameHeight, 
    UMV_BORDER) < 0)
  {
    return FALSE;
  }
    pbi->GoldenFrame = (YUV_BUFFER_ENTRY *)pbi->goldenFrameYV12Config.BufferAlloc;

	for(i=0; i< MAXFRAMES; i++)
	{

    if (On2YV12_AllocFrameBuffer(&pbi->FrameYV12Config[i], 
      pbi->Configuration.VideoFrameWidth , 
      pbi->Configuration.VideoFrameHeight, 
      UMV_BORDER) < 0)
    {
      return FALSE;
    }
		pbi->frame_buf_fifo.img_seq[i]= &pbi->FrameYV12Config[i];

	}

	pbi->frame_buf_fifo.w_idx = MAXFRAMES;
	pbi->frame_buf_fifo.r_idx = 0;
	pbi->frame_buf_num = MAXFRAMES;
	pbi->frameNumber = 0;
    return TRUE;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DeleteHuffman
 *
 *  INPUTS        :     PB_INSTANCE *pbi.
 *  
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     None.
 *
 *  FUNCTION      :     Kills huffman space.
 *
 *  SPECIAL NOTES :     None.
 *
 ****************************************************************************/
void VP6_DeleteHuffman (PB_INSTANCE *pbi)
{
	if (pbi->huff)
		duck_free (pbi->huff);
	pbi->huff = 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_AllocateHuffman
 *
 *  INPUTS        :     PB_INSTANCE *pbi.
 *  
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     INT32 0 if sucessfull, -1 if unsucessfull
 *
 *  FUNCTION      :     Sets aside space for the Huffman tables for decoding.
 *
 *  SPECIAL NOTES :     None.
 *
 ****************************************************************************/
INT32 VP6_AllocateHuffman (PB_INSTANCE *pbi)
{
	VP6_DeleteHuffman (pbi);

	pbi->huff = duck_memalign (8, sizeof (HUFF_INSTANCE));
	if (!pbi->huff)
		return -1;

	return 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_InitFrameDetails
 *
 *  INPUTS        :     PB_INSTANCE * pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     BOOL: 0 on success, negative on failure.
 *
 *  FUNCTION      :     Initialises various details about the frame.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INT32 
VP6_InitFrameDetails ( PB_INSTANCE *pbi )
{
	UINT32 i;


    /* Set the frame size etc. */                                                        
    pbi->YPlaneSize = pbi->Configuration.VideoFrameWidth * pbi->Configuration.VideoFrameHeight; 
    pbi->UVPlaneSize = pbi->YPlaneSize / 4;  

    pbi->HFragments = pbi->Configuration.VideoFrameWidth / pbi->Configuration.HFragPixels;
    pbi->VFragments = pbi->Configuration.VideoFrameHeight / pbi->Configuration.VFragPixels;
    pbi->UnitFragments = ((pbi->VFragments * pbi->HFragments)*3)/2;
	pbi->YPlaneFragments = pbi->HFragments * pbi->VFragments;
	pbi->UVPlaneFragments = pbi->YPlaneFragments / 4;

    pbi->Configuration.YStride = (pbi->Configuration.VideoFrameWidth + STRIDE_EXTRA);
    pbi->Configuration.UVStride = pbi->Configuration.YStride / 2;

    pbi->YDataOffset = 0;
    pbi->UDataOffset = pbi->YPlaneSize;
    pbi->VDataOffset = pbi->YPlaneSize + pbi->UVPlaneSize;

    pbi->ReconYDataOffset = 0;
    pbi->ReconUDataOffset = pbi->Configuration.YStride * (pbi->Configuration.VideoFrameHeight + STRIDE_EXTRA);
    pbi->ReconVDataOffset = pbi->Configuration.YStride * (pbi->Configuration.VideoFrameHeight + STRIDE_EXTRA) * 5 / 4;

	// Image dimensions in Macro-Blocks
	pbi->MBRows  = (2*BORDER_MBS)+(pbi->Configuration.VideoFrameHeight/16)  + ( pbi->Configuration.VideoFrameHeight%16 ? 1 : 0 );
	pbi->MBCols  = (2*BORDER_MBS)+(pbi->Configuration.VideoFrameWidth/16)  + ( pbi->Configuration.VideoFrameWidth%16 ? 1 : 0 );
	pbi->MacroBlocks = pbi->MBRows * pbi->MBCols;

	for( i=0; i<12; i++ )
		pbi->mvNearOffset[i] = MBOffset(NearMacroBlocks[i].row, NearMacroBlocks[i].col);


	if ( !VP6_AllocateFragmentInfo(pbi) )
        return -1;

	if ( !VP6_AllocateFrameInfo(pbi) )
    {
        VP6_DeleteFragmentInfo(pbi);
        return -1;
    }

    VP6_InitMBI(pbi);


    return 0;
}



unsigned int ON2_GetProcessorFreq()
{
    return 1;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_VPInitLibrary
 *
 *  INPUTS        :     None.
 *  
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Fully initializes the playback library.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6C_VPInitLibrary(void)
{
    int i;

	for ( i=0; i<VAL_RANGE*3; i++ ) 
    {
		int x = i - VAL_RANGE;
		LimitVal_VP6[i] = Clamp255 ( x );
	}
}

