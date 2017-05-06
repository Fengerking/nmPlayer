/****************************************************************************
*        
*   Module Title :     vfwpbdll_if.c
*
*   Description  :     Video codec playback dll interface
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "voVP6back.h"

/****************************************************************************
*  Macros
****************************************************************************/
#ifndef _MSC_VER
#else
extern void (*ClearSysState)( void );
#endif

/****************************************************************************
*  Exports
****************************************************************************/   
extern YV12_BUFFER_CONFIG*  buf_seq_ctl(FIFOTYPE *priv, YV12_BUFFER_CONFIG* img , const UINT32 flag);

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_InitialiseConfiguration
 *
 *  INPUTS        :     PB_INSTANCE * pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Sets the base size of a coding block (8x8).
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_InitialiseConfiguration ( PB_INSTANCE *pbi )
{  
    pbi->Configuration.HFragPixels = 8;
    pbi->Configuration.VFragPixels = 8;
} 

/****************************************************************************
* Imports
****************************************************************************/ 
extern INT32 VP6_DecodeFrameMbs(PB_INSTANCE *pbi);
extern void InitHeaderBuffer ( FRAME_HEADER *Header, UINT8 *Buffer );

#include <stdio.h>

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_StartDecoder
 *
 *  INPUTS        :     PB_INSTANCE **pbi  : Pointer to pointer to decoder instance.
 *                      UINT32 ImageWidth  : Width of the image.
 *                      UINT32 ImageHeight : Height of the image.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     0 if succeeds, negative otherwise.
 *
 *  FUNCTION      :     Creates and initializes the decoder.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INT32
VP6_StartDecoder( PB_INSTANCE **pbi, char *p, UINT32 ImageWidth, UINT32 ImageHeight )
{ 
	VP6_DMachineSpecificConfig();
	VP6C_VPInitLibrary();
	if(p != NULL)
	{
		return -2;
	}
	else
	{
    //__try
    {
        // set up our structure holding all formerly global information about a playback instance
        *pbi = VP6_CreatePBInstance();
        if(*pbi == (PB_INSTANCE *)0)
            return -1;

        (*pbi)->OutputWidth = ImageWidth;
        (*pbi)->OutputHeight = ImageHeight;
		

        // Validate the combination of height and width.
        (*pbi)->Configuration.VideoFrameWidth = ImageWidth;
        (*pbi)->Configuration.VideoFrameHeight = ImageHeight;

        // Set up various configuration parameters.
        VP6_InitialiseConfiguration(*pbi);

        (*pbi)->quantizer = VP6_CreateQuantizer();
      
        // Fills in fragment counts as well
        if( VP6_InitFrameDetails(*pbi) < 0)
        {
            VP6_DeletePBInstance(pbi);
            return -2;
        }
        // Set last_dct_thresh to an illegal value to make sure the
        // Q tables are initialised for the new video sequence. 
        (*pbi)->quantizer->LastFrameQIndex = 0xFFFFFFFF;
        
        return 0;
    }
	}
}

void image_setedges_t(vouint8_t *src_in, const vouint32_t ExWidth, vouint32_t width, vouint32_t height, vouint32_t edge)
{
	int i;
	vouint8_t *dst,*src;
	int iedge = edge;

	src = src_in;
	dst = src - (iedge + iedge * ExWidth);

	for (i = 0; i < iedge; i++) {
		memset(dst, *src, iedge);
		memcpy(dst + iedge, src, width);
		memset(dst + ExWidth - iedge, *(src + width - 1),
			   iedge);
		dst += ExWidth;
	}
	for (i = 0; i < (int)height; i++) {
		memset(dst, *src, iedge);
		memset(dst + ExWidth - iedge, src[width - 1], iedge);
		dst += ExWidth;
		src += ExWidth;
	}
	src -= ExWidth;
	for (i = 0; i < iedge; i++) {
		memset(dst, *src, iedge);
		memcpy(dst + iedge, src, width);
		memset(dst + ExWidth - iedge, *(src + width - 1),
				   iedge);
		dst += ExWidth;
	}
} 

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeFrameToYUV
 *
 *  INPUTS        :     PB_INSTANCE *pbi       : Pointer to decoder instance.
 *                      char *VideoBufferPtr   : Pointer to compressed data buffer.
 *                      unsigned int ByteCount : Size in bytes of compressed data buffer.
 *                      UINT32 ImageWidth      : Image width.
 *                      UINT32 ImageHeight     : Image height.
 *
 *  OUTPUTS       :     None
 *
 *  RETURNS       :     int: 0 for success, negative value for error.
 *
 *  FUNCTION      :     Decodes a frame into the internal YUV reconstruction buffer.
 *                      Details of this buffer can be obtained by calling GetYUVConfig().
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INT32 CCONV VP6_DecodeFrameToYUV 
( 
    PB_INSTANCE *pbi,
	DEC_FRAME_INFO* dec_frame_info, 
	UINT8* VideoBufferPtr, 
    UINT32 ByteCount,
	const voint64_t time,
	VO_PTR hCheck
)
{
	INT32 returnvalue = 0;
	if(ByteCount<=0)
            return -1;
    //__try
    {
//remove		pbi->CurrentFrameSize = ByteCount;

		// Initialise the bit reader used to read the fixed raw part of the header
        //InitHeaderBuffer ( &pbi->Header, (unsigned char*)VideoBufferPtr ); 

		// decode the frame header
        if ( VP6_LoadFrameHeader(pbi,(UINT8*)VideoBufferPtr, ByteCount,0) < 0)
		{
			if(pbi->FrameType == BASE_FRAME)
				pbi->keyframe_error = 1;
			pbi->Configuration.ExpandedFrameWidth = 0;
			pbi->Configuration.ExpandedFrameWidth = 0;
			pbi->OutputWidth =0;
			pbi->OutputHeight =0;
            return -1;
		}

		//printf(" aaaa \n");

		if(pbi->OutputWidth == 2352 && pbi->OutputHeight == 2160 )
		{
			pbi->OutputHeight = pbi->OutputHeight;
		}

		//  Start the second boolean decoder
		if ( pbi->MultiStream || (pbi->VpProfile == SIMPLE_PROFILE) )
		{
		    pbi->mbi.br = &pbi->br2;

			if ( pbi->UseHuffman )
			{
				// Initialise BITREADER for second bitstream partition
				pbi->br3.bitsinremainder = 0;
				pbi->br3.remainder = 0;
				pbi->br3.errorflag = 0;
				//if(ByteCount <= pbi->Buff2Offset)
				//	return 0;
#ifdef STABILITY
				if(ByteCount <= pbi->Buff2Offset)
				{
					if(pbi->FrameType == BASE_FRAME)
						pbi->keyframe_error = 1;
					return -11;
				}
#endif
				pbi->br3.position = ((UINT8*)VideoBufferPtr)+pbi->Buff2Offset;
				pbi->br3.position_end = ((UINT8*)VideoBufferPtr)+ByteCount;


			}
			else
			{
#ifdef STABILITY
				if(pbi->Buff2Offset > ByteCount)
				{
					if(pbi->FrameType == BASE_FRAME)
						pbi->keyframe_error = 1;
					return -11;
				}
#endif
				VP6_StartDecode(&pbi->br2,((UINT8*)VideoBufferPtr)+pbi->Buff2Offset);
				pbi->br2.buffer_end = VideoBufferPtr+ByteCount;
			}
		}
        else
        {
        	pbi->mbi.br = &pbi->br;
        }

		pbi->mbi.br->errorflag = 0;

		if(pbi->FrameType != BASE_FRAME )
		{
			VO_VIDEO_BUFFER pOutData;
			pOutData.Buffer[0] = pbi->lastFrameYV12Config->YBuffer;
			pOutData.Buffer[1] = pbi->lastFrameYV12Config->UBuffer;
			pOutData.Buffer[2] = pbi->lastFrameYV12Config->VBuffer;
			pOutData.Stride[0] = pbi->lastFrameYV12Config->YStride;
			pOutData.Stride[1] = 
			pOutData.Stride[2] = pbi->lastFrameYV12Config->UVStride;
			pOutData.ColorType = VO_COLOR_YUV_PLANAR420;
			pOutData.Time      = dec_frame_info->time;
#ifdef		LICENSEFILE
			voCheckLibResetVideo (hCheck, &pOutData);
#else

#endif
		}

		pbi->thisFrameYV12Config = (YV12_BUFFER_CONFIG*)buf_seq_ctl(&pbi->frame_buf_fifo, NULL , FIFO_READ);
		if(pbi->thisFrameYV12Config == NULL)
		{
			if(pbi->FrameType == BASE_FRAME)
				pbi->keyframe_error = 1;
			return -11;
		}
		pbi->ThisFrameRecon = (YUV_BUFFER_ENTRY *)pbi->thisFrameYV12Config->BufferAlloc;

        // decode and reconstruct frame
        returnvalue=VP6_DecodeFrameMbs(pbi);
		if(returnvalue != 0)
		{
			if(pbi->FrameType == BASE_FRAME)
				pbi->keyframe_error = 1;
			return returnvalue;
		}

        {
            pbi->lastFrameYV12Config = pbi->thisFrameYV12Config;
			pbi->LastFrameRecon = (YUV_BUFFER_ENTRY *)pbi->lastFrameYV12Config->BufferAlloc;
        }

        // update the border 
#if defined(VOARMV6) || defined(VOARMV7)

		//image_setedges_t(pbi->lastFrameYV12Config->YBuffer, pbi->lastFrameYV12Config->YStride, (pbi->lastFrameYV12Config->YWidth+15)&~15, (pbi->lastFrameYV12Config->YHeight+15)&~15, pbi->lastFrameYV12Config->border);
		//image_setedges_t(pbi->lastFrameYV12Config->UBuffer, pbi->lastFrameYV12Config->YStride/2, ((pbi->lastFrameYV12Config->YWidth+15)&~15)/2, ((pbi->lastFrameYV12Config->YHeight+15)&~15)/2, pbi->lastFrameYV12Config->border/2);
		//image_setedges_t(pbi->lastFrameYV12Config->VBuffer, pbi->lastFrameYV12Config->YStride/2, ((pbi->lastFrameYV12Config->YWidth+15)&~15)/2, ((pbi->lastFrameYV12Config->YHeight+15)&~15)/2, pbi->lastFrameYV12Config->border/2);	

		//need to modify
		//fill_edge_y_armv6(pbi->lastFrameYV12Config->YBuffer, pbi->lastFrameYV12Config->YStride, (pbi->lastFrameYV12Config->YWidth+15)&~15, (pbi->lastFrameYV12Config->YHeight+15)&~15, pbi->lastFrameYV12Config->border);
		//fill_edge_uv_armv6(pbi->lastFrameYV12Config->UBuffer, pbi->lastFrameYV12Config->YStride/2, ((pbi->lastFrameYV12Config->YWidth+15)&~15)/2, ((pbi->lastFrameYV12Config->YHeight+15)&~15)/2, pbi->lastFrameYV12Config->border/2);
		//fill_edge_uv_armv6(pbi->lastFrameYV12Config->VBuffer, pbi->lastFrameYV12Config->YStride/2, ((pbi->lastFrameYV12Config->YWidth+15)&~15)/2, ((pbi->lastFrameYV12Config->YHeight+15)&~15)/2, pbi->lastFrameYV12Config->border/2);	

        On2YV12_ExtendFrameBorders(pbi->lastFrameYV12Config);
#else
        On2YV12_ExtendFrameBorders(pbi->lastFrameYV12Config);
#endif

		pbi->frameNumber++;

        // Update the golden frame buffer
		if( (pbi->FrameType == BASE_FRAME) || pbi->RefreshGoldenFrame )
		{
			memcpy(pbi->GoldenFrame, pbi->LastFrameRecon, pbi->lastFrameYV12Config->frameSize); 

		}


	    if(pbi->FrameType == BASE_FRAME )
			pbi->AvgFrameQIndex = pbi->quantizer->FrameQIndex;
		else
			pbi->AvgFrameQIndex = (2 + 3 * pbi->AvgFrameQIndex + pbi->quantizer->FrameQIndex) / 4 ;
    }   

		buf_seq_ctl(&pbi->frame_buf_fifo, pbi->lastFrameYV12Config , FIFO_WRITE);

		dec_frame_info->image_y = pbi->lastFrameYV12Config->YBuffer;
		dec_frame_info->image_u = pbi->lastFrameYV12Config->UBuffer;
		dec_frame_info->image_v = pbi->lastFrameYV12Config->VBuffer;
		dec_frame_info->image_width = pbi->OutputWidth;
		dec_frame_info->image_height = pbi->OutputHeight;
		dec_frame_info->image_stride_y = pbi->lastFrameYV12Config->YStride;
		dec_frame_info->image_stride_uv = pbi->lastFrameYV12Config->UVStride;
		dec_frame_info->image_type = pbi->FrameType;				/* 0-BASE_FRAME, 1-NORMAL_FRAME */
		dec_frame_info->error_frame = 0;
		dec_frame_info->time = pbi->time_cur;
		pbi->time_cur = time;

		if(pbi->mbi.br->pos > ByteCount)
			pbi->mbi.br->pos = pbi->mbi.br->pos;
    return 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_StopDecoder
 *
 *  INPUTS        :     PB_INSTANCE **pbi : Pointer to pointer to decoder instance.
 *
 *  OUTPUTS       :     PB_INSTANCE **pbi : Pointer to pointer to decoder instance,
 *                                          set to NULL on return.
 *
 *  RETURNS       :     int: TRUE on success, FALSE otherwise.
 *
 *  FUNCTION      :     Detroys the decoder instance.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INT32
VP6_StopDecoder ( PB_INSTANCE **pbi )
{
    //__try
    {
        if ( *pbi )
        {
            // Set flag to say that the decoder is no longer initialised
            VP6_DeleteQuantizer(&(*pbi)->quantizer);
            VP6_DeleteFragmentInfo(*pbi);
            VP6_DeleteFrameInfo(*pbi);
            VP6_DeletePBInstance(pbi);
            return 0;
        }
    }  

    return 0;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_ErrorTrap
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *                      int ErrorCode    : Error code to report.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Called when a fatal error is detected.
 *
 *  SPECIAL NOTES :     Currently does nothing. 
 *
 ****************************************************************************/
//static 
void VP6_ErrorTrap ( PB_INSTANCE *pbi, INT32 ErrorCode )
{
}
