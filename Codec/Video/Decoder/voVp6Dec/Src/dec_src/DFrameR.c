/****************************************************************************
*
*   Module Title :     DFrameR.C
*
*   Description  :     Functions to read from the input bitstream.
*
****************************************************************************/


/****************************************************************************
*  Header Frames
****************************************************************************/
#include "pbdll.h"

extern void overridePredictFilteredFuncs(UINT32 isVP60);

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_bitread
 *
 *  INPUTS        :     BOOL_CODER *br : Pointer to a Bool Decoder instance.
 *                      int bits       : Number of bits to be read from input stream.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     UINT32: The requested bits.
 *
 *  FUNCTION      :     Decodes the requested number of bits from the encoded data buffer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
UINT32 VP6_bitread ( BOOL_CODER *br, INT32 bits )
{
	UINT32 z = 0;
	INT32 bit;
	for ( bit=bits-1; bit>=0; bit-- )
	{
		z |= (VP6_DecodeBool128(br)<<bit);
	}
	return z;
}

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_bitread1
 *
 *  INPUTS        :     BOOL_CODER *br : Pointer to a Bool Decoder instance.
 *
 *  OUTPUTS       :     None
 *
 *  RETURNS       :     UINT32: The next decoded bit (0 or 1).
 *
 *  FUNCTION      :     Decodes the next bit from the encoded data buffer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/ 
UINT32 VP6_bitread1 ( BOOL_CODER *br ) 
{
	return (VP6_DecodeBool128(br));
}

/****************************************************************************
 * 
 *  ROUTINE       :     InitHeaderBuffer
 *
 *  INPUTS        :     FRAME_HEADER *Header  : Pointer to FRAME_HEADER data structure.
 *                      unsigned char *Buffer : Pointer to buffer containing bitstream header.
 *
 *  OUTPUTS       :     None
 *
 *  RETURNS       :     void
 *
 *
 *  FUNCTION      :     Initialises extraction of bits from header buffer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void 
InitHeaderBuffer ( FRAME_HEADER *Header, UINT8 *Buffer )
{
    Header->buffer = Buffer;
    Header->value  = (Buffer[0]<<24)+(Buffer[1]<<16)+(Buffer[2]<<8)+Buffer[3];
    Header->bits_available = 32;
	Header->pos = 4;
}

/****************************************************************************
 * 
 *  ROUTINE       :     ReadHeaderBits
 *
 *  INPUTS        :     FRAME_HEADER *Header : Pointer to FRAME_HEADER data structure.
 *                      UINT32 BitsRequired  : Number of bits to extract.
 *
 *  OUTPUTS       :     None
 *
 *  RETURNS       :     UINT32: Bits requested
 *
 *  FUNCTION      :     Extracts requested number of bits from header buffer.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
UINT32 
ReadHeaderBits ( FRAME_HEADER *Header, UINT32 BitsRequired )
{
    UINT32 pos       = Header->pos;
    UINT32 available = Header->bits_available;
    UINT32 value     = Header->value;
    UINT8 *Buffer    = &Header->buffer[pos];
    UINT32 RetVal    = 0;

    if ( available < BitsRequired )
    {
        // Need more bits from input buffer...
        RetVal = value >> (32-available);
        BitsRequired -= available;
        RetVal <<= BitsRequired;

        value  = (Buffer[0]<<24)+(Buffer[1]<<16)+(Buffer[2]<<8)+(Buffer[3]);
        pos += 4;
        available = 32;
    }

    RetVal |= value >> (32-BitsRequired);
    
    // Update data struucture
    Header->value          = value<<BitsRequired;
    Header->bits_available = available-BitsRequired;
    Header->pos = pos;

    return RetVal;
}

/****************************************************************************
 * 
 *  ROUTINE       :     LoadFrameHeader
 *
 *  INPUTS        :     PB_INSTANCE *pbi : Pointer to decoder instance.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     BOOL: FALSE in case of error, TRUE otherwise.
 *
 *  FUNCTION      :     Loads a frame header & carries out some initialization.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
INT32
VP6_LoadFrameHeader ( PB_INSTANCE *pbi, UINT8 *Buffer, UINT32 BufferLength,const UINT32 frame_type_flag)
{
    UINT8  DctQMask;
    FRAME_HEADER *Header = &pbi->Header;

    Header->buffer = Buffer;
    Header->value  = (Buffer[0]<<24)+(Buffer[1]<<16)+(Buffer[2]<<8)+Buffer[3];
    Header->bits_available = 32;
	Header->pos = 4;

    // Is the frame and inter frame or a key frame
    pbi->FrameType = (UINT8)ReadHeaderBits(Header, 1);

	//if(!pbi->FrameType)
		//printf(" *******%d \n\n\n\n",pbi->FrameType);

	//if(frame_type_flag)  //? zou
	//	return 0;

    // Quality (Q) index
    DctQMask = (UINT8)ReadHeaderBits(Header, 6);

	// Are we using two BOOL coder data streams/partitions
    pbi->MultiStream = (UINT8)ReadHeaderBits(Header, 1);

	// If the frame was a base frame then read the frame dimensions and build a bitmap structure. 
	if ( (pbi->FrameType == BASE_FRAME) )
	{
		pbi->keyframe_error = 0;
        // Read the frame dimensions bytes (0,0 indicates vp31 or later)
	    pbi->Vp3VersionNo = (UINT8)ReadHeaderBits(Header,   5 );
	    pbi->VpProfile = (UINT8)ReadHeaderBits(Header,   2 );

		//if(pbi->Vp3VersionNo != 6 && pbi->Vp3VersionNo != 7 && pbi->Vp3VersionNo !=8)//stabilty
		//	return -1;

		//if(pbi->VpProfile != 0 && pbi->VpProfile != 3) //stabilty
		//	return -1;

		//if(pbi->Vp3VersionNo == 6 && pbi->VpProfile == 3)
		//	return -1;

		// reserved
        if( (UINT8)ReadHeaderBits(Header, 1) != 0)
        {
            //this is an old test bitstream and is not supported
			//printf("this is an old test bitstream and is not supported \n");
            return -1;
        }    	
		pbi->first_frametype = 1;

		// Start the first bool decoder (modes, mv, probs and some flags)
		// The offset depends on whether or not we are using multiple bool code streams
		if ( pbi->MultiStream )
		{
			VP6_StartDecode(&pbi->br, ((UINT8*)(Header->buffer + 4)));
			pbi->br.buffer_end = Header->buffer+BufferLength;

			// Read the buffer offset for the second bool decoder buffer if it is being used
		    pbi->Buff2Offset = (UINT32)ReadHeaderBits(Header, 16);
		}
		else
		{
			VP6_StartDecode(&pbi->br, ((UINT8*)(Header->buffer + 2)));
			pbi->br.buffer_end = Header->buffer+BufferLength;
		}


        {             
             UINT32 HFragments;             
             UINT32 VFragments;             
             UINT32 HOldScaled;
             UINT32 VOldScaled;
             UINT32 HNewScaled;
             UINT32 VNewScaled;
			 UINT32 OutputHFragments;
			 UINT32 OutputVFragments;

             VFragments = 2 * ((UINT8)VP6_bitread( &pbi->br,   8 ));             
             HFragments = 2 * ((UINT8)VP6_bitread( &pbi->br,   8 ));      

#ifdef STABILITY
			 //if(HFragments*8 > 3000 || HFragments > 3000)
			//	 return -11;
#endif

             OutputVFragments = 2 * ((UINT8)VP6_bitread( &pbi->br,   8 ));             
             OutputHFragments = 2 * ((UINT8)VP6_bitread( &pbi->br,   8 ));              

             if(pbi->Configuration.HRatio == 0)
                 pbi->Configuration.HRatio = 1;

             if(pbi->Configuration.VRatio == 0)
                 pbi->Configuration.VRatio = 1;

             HOldScaled = pbi->Configuration.HScale * pbi->HFragments * 8 / pbi->Configuration.HRatio;
             VOldScaled = pbi->Configuration.VScale * pbi->VFragments * 8 / pbi->Configuration.VRatio;

			 pbi->Configuration.ExpandedFrameWidth = OutputHFragments * 8;
			 pbi->Configuration.ExpandedFrameHeight = OutputVFragments * 8;

			 if(VFragments >= OutputVFragments)
			 {
	             pbi->Configuration.VScale = 1;
		         pbi->Configuration.VRatio = 1;
			 }
			 else if (5*VFragments >= 4*OutputVFragments)
			 {
	             pbi->Configuration.VScale = 5;
		         pbi->Configuration.VRatio = 4;
			 }
			 else if (5*VFragments >= 3*OutputVFragments)
			 {
	             pbi->Configuration.VScale = 5;
		         pbi->Configuration.VRatio = 3;
			 }
			 else
			 {
	             pbi->Configuration.VScale = 2;
		         pbi->Configuration.VRatio = 1;
			 }

			 if(HFragments >= OutputHFragments)
			 {
	             pbi->Configuration.HScale = 1;
		         pbi->Configuration.HRatio = 1;
			 }
			 else if (5*HFragments >= 4*OutputHFragments)
			 {
	             pbi->Configuration.HScale = 5;
		         pbi->Configuration.HRatio = 4;
			 }
			 else if (5*HFragments >= 3*OutputHFragments)
			 {
	             pbi->Configuration.HScale = 5;
		         pbi->Configuration.HRatio = 3;
			 }
			 else
			 {
	             pbi->Configuration.HScale = 2;
		         pbi->Configuration.HRatio = 1;
			 }

             HNewScaled = pbi->Configuration.HScale * HFragments * 8 / pbi->Configuration.HRatio;
             VNewScaled = pbi->Configuration.VScale * VFragments * 8 / pbi->Configuration.VRatio;

             pbi->Configuration.ScalingMode = ((UINT32)VP6_bitread( &pbi->br, 2 ));

             // we have a new input size
             if( VFragments != pbi->VFragments || HFragments != pbi->HFragments )
             {
                 // Validate the combination of height and width.                 
                 pbi->Configuration.VideoFrameWidth = HFragments*8;                 
                 pbi->Configuration.VideoFrameHeight = VFragments*8;


				//need to check here
				if( VP6_InitFrameDetails(pbi) < 0)
				{
					VP6_DeletePBInstance(&pbi);
					return -2;
				}
				pbi->OutputWidth = pbi->Configuration.VideoFrameWidth ;
				pbi->OutputHeight = pbi->Configuration.VideoFrameHeight;

             }

             // we have a new intermediate buffer clean the screen 
             //if( (HOldScaled != HNewScaled || VOldScaled != VNewScaled) )
             //{
             //   On2YV12_BlackFrameBuffer(pbi->scaleYV12Config);
             //}
		}        

		// Unless in SIMPLE_PROFILE read the the filter strategy for fractional pels
		if ( pbi->VpProfile != SIMPLE_PROFILE )
		{
			// Find out if selective bicubic filtering should be used for motion prediction.
			if ( (BOOL)VP6_DecodeBool128(&pbi->br) )
			{
				pbi->PredictionFilterMode = AUTO_SELECT_PM;

				// Read in the variance threshold to be used
				pbi->PredictionFilterVarThresh = ((UINT32)VP6_bitread( &pbi->br, 5) << ((pbi->Vp3VersionNo > 7) ? 0 : 5) );

				// Read the bicubic vector length limit (0 actually means ignore vector length)
				pbi->PredictionFilterMvSizeThresh = (UINT8)VP6_bitread( &pbi->br, 3);
			}
			else
			{
				if ( (BOOL)VP6_DecodeBool(&pbi->br, 128) )
                {
					pbi->PredictionFilterMode = BICUBIC_ONLY_PM;
                    //note: BICUBIC_ONLY_PM is currently not being used
                    // but if set, return an error
                    //return -77;
				}
                else
					pbi->PredictionFilterMode = BILINEAR_ONLY_PM;
			}

			if ( pbi->Vp3VersionNo > 7 )
				pbi->PredictionFilterAlpha = VP6_bitread( &pbi->br, 4);
			else
				pbi->PredictionFilterAlpha = 16;	// VP61 backwards compatibility

            //uses vp62 predicted filters
            overridePredictFilteredFuncs(0);
		}
        else
        {
            // VP60 backwards compatibility
			pbi->PredictionFilterAlpha = 14;	
		    pbi->PredictionFilterMode = BILINEAR_ONLY_PM;	

            //uses vp60 predicted filters
            overridePredictFilteredFuncs(1);
            // VP60 backwards compatibility
        }

    }
	// Non key frame sopecific stuff
	else
	{
#ifdef STABILITY
		if(pbi->keyframe_error ==1)
			return -11;
#endif
		//Check to make sure that the first frame is not a non-keyframe
		if(pbi->Vp3VersionNo == 0 || pbi->first_frametype == 0)
		{
			return -88;
		}
		//if(pbi->Vp3VersionNo != 6 && pbi->Vp3VersionNo != 7 && pbi->Vp3VersionNo !=8) //stabilty
		//	return -1;

		//if(pbi->VpProfile != 0 && pbi->VpProfile != 3) //stabilty
		//	return -1;

		//if(pbi->Vp3VersionNo == 6 && pbi->VpProfile == 3)
		//	return -1;
	
		// Start the first bool decoder (modes, mv, probs and some flags)
		// The offset depends on whether or not we are using multiple bool code streams
		if ( pbi->MultiStream)
		{
			VP6_StartDecode(&pbi->br, ((UINT8*)(Header->buffer + 3)));
			pbi->br.buffer_end = Header->buffer+BufferLength;
			
			// Read the buffer offset for the second bool decoder buffer if it is being used
		    pbi->Buff2Offset = (UINT32)ReadHeaderBits(Header, 16);
		}
		else
		{
			VP6_StartDecode(&pbi->br, ((UINT8*)(Header->buffer + 1)));
			pbi->br.buffer_end = Header->buffer+BufferLength;
		}

		// Find out if the golden frame should be refreshed this frame - use bool decoder
		pbi->RefreshGoldenFrame = (BOOL)VP6_DecodeBool128(&pbi->br);

		if ( pbi->VpProfile != SIMPLE_PROFILE )
		{
			// Determine if loop filtering is on and if so what type should be used
			pbi->UseLoopFilter = VP6_DecodeBool128(&pbi->br);
			if ( pbi->UseLoopFilter )
			{
				pbi->UseLoopFilter = (pbi->UseLoopFilter << 1) | VP6_DecodeBool128(&pbi->br);
			}

			if ( pbi->Vp3VersionNo > 7 )
			{
				// Are the prediction characteristics being updated this frame
				if ( VP6_DecodeBool128(&pbi->br) )
				{
					// Find out if selective bicubic filtering should be used for motion prediction.
					if ( (BOOL)VP6_DecodeBool128(&pbi->br) )
					{
						pbi->PredictionFilterMode = AUTO_SELECT_PM;

						// Read in the variance threshold to be used
						pbi->PredictionFilterVarThresh = (UINT32)VP6_bitread( &pbi->br, 5);

						// Read the bicubic vector length limit (0 actually means ignore vector length)
						pbi->PredictionFilterMvSizeThresh = (UINT8)VP6_bitread( &pbi->br, 3);
					}
					else
					{
						if ( (BOOL)VP6_DecodeBool128(&pbi->br) )
                        {
							pbi->PredictionFilterMode = BICUBIC_ONLY_PM;
                            //note: BICUBIC_ONLY_PM is currently not being used
                            // but if set, return an error
//                            return -77;
                        }
						else
							pbi->PredictionFilterMode = BILINEAR_ONLY_PM;
					}

					pbi->PredictionFilterAlpha = VP6_bitread( &pbi->br, 4 );
				}
			}
			else
				pbi->PredictionFilterAlpha = 16;	// VP61 backwards compatibility

            //uses vp62 predicted filters
            overridePredictFilteredFuncs(0);
		}
        else
        {
            // VP60 backwards compatibility
			pbi->PredictionFilterAlpha = 14;	
		    pbi->PredictionFilterMode = BILINEAR_ONLY_PM;	

            //uses vp60 predicted filters
            overridePredictFilteredFuncs(1);
            // VP60 backwards compatibility
        }

	}

	// All frames (Key & Inter frames)

	// Should this frame use huffman for the dct data
	// need to check if huffman buffers have been initialized, if not, do it.
	pbi->UseHuffman = (BOOL)VP6_DecodeBool128(&pbi->br);
	if (pbi->UseHuffman && !pbi->huff)
		if (VP6_AllocateHuffman (pbi))
			return -2;

	// Set this frame quality value from Q Index
	pbi->quantizer->FrameQIndex = DctQMask;

	VP6_UpdateQ( pbi->quantizer, pbi->Vp3VersionNo );  

    return 0;                    
}

