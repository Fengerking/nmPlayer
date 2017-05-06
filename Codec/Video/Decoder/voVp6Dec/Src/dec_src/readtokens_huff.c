/****************************************************************************
*
*   Module Title :     readtokens_huff.c
*
*   Description  :     readtokens optimized for the C3 processor
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "readtokens_huff.h"

/***************************************************************************
*
****************************************************************************/



/****************************************************************************
 * 
 *  ROUTINE       :     NextWord (MACRO)
 *
 *  INPUTS        :     None.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     None.
 *
 *  FUNCTION      :     Reads 32 bits from the input buffer for processing and
 *                      reverts data to little endian.
 *
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
#	define BitsAreBigEndian 1
#	if BitsAreBigEndian

#ifdef STABILITY2
#		define NextWord \
		{\
			br->remainder = (br->position[0] << 24) + (br->position[1] << 16) + (br->position[2] << 8) + br->position[3];\
			if(br->position > br->position_end)\
			{\
			br->errorflag = 1;\
			}\
			else\
				br->position += 4;\
		}
#else
#		define NextWord \
		{\
			br->remainder = (br->position[0] << 24) + (br->position[1] << 16) + (br->position[2] << 8) + br->position[3];\
			br->position += 4;\
		}
#endif


#	else
#		define NextWord \
		{\
			br->remainder = (br->position[3] << 24) + (br->position[2] << 16) + (br->position[1] << 8) + br->position[0];\
			if(br->position > br->position_end)\
			{\
				br->errorflag = 1;\
					br->position = br->position_end;\
			}\
			else\
				br->position += 4;\
		}
#	endif

/****************************************************************************
 * 
 *  ROUTINE       :     bitread
 *
 *  INPUTS        :     BITREADER *br : Wrapper for the encoded data buffer.
 *                      int bits      : Number of bits to read.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     Value of the number of bits requested (as UINT32)
 *
 *  FUNCTION      :     Extracts requested number of bits from the encoded data buffer.
 *
 *  SPECIAL NOTES :     Uses the NextWord macro. 
 *
 ****************************************************************************/
FORCEINLINE
UINT32 bitread ( BITREADER *br, int bits )
{
	UINT32 z = 0;

	br->remainder &= loMaskTbl_VP60[br->bitsinremainder];
	
	if( (bits -= br->bitsinremainder) > 0) 
	{
		z |= br->remainder << bits;
		NextWord
			bits -= 32;
	}
	return z | br->remainder >> (br->bitsinremainder = -bits);
}

/****************************************************************************
 * 
 *  ROUTINE       :     bitreadonly
 *
 *  INPUTS        :     BITREADER *br : Wrapper for the encoded data buffer.
 *                      int bits      : Number of bits to read.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     Value of the number of bits requested (as UINT32)
 *
 *  FUNCTION      :     Extracts requested number of bits from the encoded data buffer.
 *
 *  SPECIAL NOTES :     This reader variant will only read a further byte from the
 *                      encoded data buffer. 
 *
 ****************************************************************************/
FORCEINLINE
UINT32 bitreadonly ( BITREADER *br, UINT32 bits )
{
    UINT32 x = br->bitsinremainder;
    UINT32 z = (1<<x)-1;

    z &= br->remainder;
    if ( x >= bits )
    {        
        return z>>(x-bits);
    }    
    z <<= 8;
    z  |= br->position[0];
	return (z>>(8+x-bits));
}

/****************************************************************************
 * 
 *  ROUTINE       :     bitShift
 *
 *  INPUTS        :     BITREADER *br : Wrapper for the encoded data buffer.
 *                      int bits      : Number of bits to discard (shift off).
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Discards requested number of bits from the encoded data buffer.
 *
 *  SPECIAL NOTES :     Uses the NextWord macro.
 *
 ****************************************************************************/
FORCEINLINE
void bitShift ( BITREADER *br, int bits )
{			
	br->bitsinremainder -= bits;
    if ( br->bitsinremainder < 0 ) 
	{
		NextWord
			br->bitsinremainder += 32;
	}	
}

/****************************************************************************
 * 
 *  ROUTINE       :     bitread1
 *
 *  INPUTS        :     BITREADER *br : Wrapper for the encoded data buffer.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     A single bit (as UINT32)
 *
 *  FUNCTION      :     Extracts a single bit  from the encoded data buffer.
 *
 *  SPECIAL NOTES :     Uses the NextWord macro. 
 *
 ****************************************************************************/
FORCEINLINE
UINT32 bitread1 ( BITREADER *br ) 
{
	if( br->bitsinremainder)
		return (br->remainder >> --br->bitsinremainder) & 1;
	NextWord
		return br->remainder  >> (br->bitsinremainder = 31);
}

#undef NextWord


/****************************************************************************
* 
*  ROUTINE       :     VP6_ExtractTokenN
*
*  INPUTS        :     BITREADER *br : Pointer to bitreader to grab the bits from.
*                      HUFF_NODE *hn : Pointer to root of huffman tree to use for decoding.
*                      UINT16* hlt   : Pointer to Huffman table node.
*
*  OUTPUTS       :     None.
*
*  RETURNS       :     The number of bits decoded.
*
*  FUNCTION      :     Unpacks and expands a DCT token.
*
*  SPECIAL NOTES :     PROBLEM !!!!!!!!!!!   right now handles only left 
*                      justified bits in bitreader. The C version keeps every
*                      thing in place so I can't use it!!
*
****************************************************************************/
#ifndef __NEW_HUFF__
static FORCEINLINE
int VP6_ExtractTokenN ( BITREADER *br, HUFF_NODE *hn, UINT16* hlt )
{
    tokenorptr torp;
    HUFF_TABLE_NODE *htptr = (HUFF_TABLE_NODE *)hlt;
    UINT32 x = bitreadonly(br, HUFF_LUT_LEVELS);
    
    bitShift(br, (htptr[x].length));
    if(htptr[x].flag)
    {        
        return htptr[x].value;
    }
        
    torp.value = htptr[x].value;
    do
    {
        if( bitread1(br) )
        {
            torp = hn[torp.value].rightunion.right;
        }
        else
        {
            torp = hn[torp.value].leftunion.left;
        }
    }
    while ( !(torp.selector));
    
    return torp.value;
    
}
#else
static FORCEINLINE
int VP6_ExtractTokenN( BITREADER *br, HUFF_NODE *huffNode, UINT16* hlt)
{
    unsigned short torp;
    unsigned short *hn = (unsigned short *)huffNode;
    HUFF_TABLE_NODE *htptr = (HUFF_TABLE_NODE *)hlt;

    UINT32 x = bitreadonly(br, HUFF_LUT_LEVELS);
    
    bitShift(br, (htptr[x].length));
    if(htptr[x].flag)
    {        
        //return htptr[x].value;
        return htptr[x].value;
    }

    // Loop searches down through tree based upon bits read from the bitstream 
    // until it hits a leaf at which point we have decoded a token
    torp = htptr[x].value << 1;
//    torp = 0;
    do
    {
        torp += bitread1(br);
        torp = hn[torp];
    }
//	while ( !(torp.selector));
	while ( !(torp & 1));

//	return torp.value; 
	return torp >> 1; 
}
#endif

/****************************************************************************
****************************************************************************/
INT32
ReadHuffTokensPredictA_MB(PB_INSTANCE *pbi)
{
	HUFF_INSTANCE	*huff = pbi->huff;
    BITREADER *br = &pbi->br3;
    INT32   SignBit;
	UINT32  Prec;

	UINT32   token;
    UINT32  blockIndex;

    UINT32  Plane = 0;

    INT16 *CoeffData, *DeQuant;

    UINT8 *MergedScanOrderPtr;

    //BLOCK_DX_INFO *bdi = pbi->mbi.blockDxInfo;
    //BLOCK_DX_INFO *bdiEnd = bdi + 6;

    for(blockIndex = 0; blockIndex < 6; blockIndex++)
    {
        MergedScanOrderPtr = pbi->MergedScanOrder;

        CoeffData = pbi->mbi.blockDxInfo[blockIndex].coeffsPtr; //mbi->Coeffs[blockIndex];


        if(blockIndex > 3)
        {
            Plane = 1;
        }

		if ( huff->CurrentDcRunLen[Plane] > 0 )
        {
            // DC -- run of zeros in progress
			--huff->CurrentDcRunLen[Plane];
            Prec = 0;        
        }
        else
        {
#ifdef STABILITY
			if(br->position > br->position_end + 32) //1245 error
				return -11;
#endif

            // DC -- no current run of zeros
			token = VP6_ExtractTokenN(br, huff->DcHuffTree[Plane], huff->DcHuffLUT[Plane]);                                                      

            if(token == DCT_EOB_TOKEN)
                goto Finished;

            if(token == ZERO_TOKEN)
            {   
                // Read zero run-length
                {
                    // Run of zeros at DC is coded as a tree
                    UINT32 val = 1 + bitread(br, 2);

                    if ( val == 3 )
                        val += bitread(br, 2);
                    else if ( val == 4 )
                    {
                        if ( bitread1(br) )
                            val = 11 + bitread(br, 6);
                        else
                            val = 7 + bitread(br, 2); 
                    }
					huff->CurrentDcRunLen[Plane] = val - 1;
                }
                Prec = 0;
            }
            else
            {
                register INT32 value;
            
                value = VP6_HuffTokenMinVal[token];
    
                if(token <=FOUR_TOKEN)
                {
                    SignBit = bitread1(br);
                }
                else if(token <=DCT_VAL_CATEGORY5)
                {
                    value   += bitread(br, (token-4));
                    SignBit = bitread1(br);
                }
                else
                {
                    value   += bitread(br, 11);
                    SignBit = bitread1(br);
                
                }
                CoeffData[0] = (Q_LIST_ENTRY)((value ^ -SignBit) + SignBit); 
                Prec = (value>1)?2:1;
            }

        }
        //first AC
    
        MergedScanOrderPtr++;

		if ( huff->CurrentAc1RunLen[Plane] > 0 )
        {
            // First AC in scan order -- run of EOBs in progress
			--huff->CurrentAc1RunLen[Plane];
            goto Finished;
        }

        do
	    {
		    
            UINT32 Band = *(MergedScanOrderPtr + 64); //VP6_CoeffToHuffBand[EncodedCoeffs]; 
#ifdef STABILITY
			//if(br->position > br->position_end && br->bitsinremainder < 0) //1245 error
			//	return -11;
			if(br->position > br->position_end + 32) //1245 error
				return -11;
#endif

			token = VP6_ExtractTokenN(br, huff->AcHuffTree[Prec][Plane][Band], huff->AcHuffLUT[Prec][Plane][Band]);              

            if(token == ZERO_TOKEN)
            {
                {
                    //UINT32 ZrlBand;
                    //UINT32 ZrlToken;
                    #define ZrlBand Band
                    #define ZrlToken token

                    // Read zero run-length
                    ZrlBand  = (MergedScanOrderPtr >= (pbi->MergedScanOrder + ZRL_BAND2));
                    
					ZrlToken = VP6_ExtractTokenN(br, huff->ZeroHuffTree[ZrlBand], huff->ZeroHuffLUT[ZrlBand]);
              
                    if ( ZrlToken<8 )
                        MergedScanOrderPtr += ZrlToken;             // Zero run <= 8
                    else
                        MergedScanOrderPtr += 8 + bitread(br, 6);   // Zero run > 8
                }
                Prec =0;
                MergedScanOrderPtr ++;			
                continue;
            }
        
            if(token == DCT_EOB_TOKEN)
            {
                if ( MergedScanOrderPtr == (pbi->MergedScanOrder + 1) )
                {
                    // Read run of EOB at first AC position
                    UINT32 val = 1 + bitread(br, 2);
                
                    if ( val == 3 )
                        val += bitread(br, 2);
                    else if ( val == 4 )
                    {
                        if ( bitread1(br) )
                            val = 11 + bitread(br, 6);
                        else
                            val = 7 + bitread(br, 2); 
                    }
					huff->CurrentAc1RunLen[Plane] = val - 1;
                }
                goto Finished;

            }

            {
                register INT32 value;

                value = VP6_HuffTokenMinVal[token];
        
                if(token <=FOUR_TOKEN)
                {
                    SignBit = bitread1(br);
                }
                else if(token <=DCT_VAL_CATEGORY5)
                {
                    value   += bitread(br, (token-4));
                    SignBit = bitread1(br);
                }
                else
                {
                    value   += bitread(br, 11);
                    SignBit = bitread1(br);
            
                }
				DeQuant   = pbi->mbi.blockDxInfo[blockIndex].dequantPtr;
                CoeffData[*(MergedScanOrderPtr)] = ((Q_LIST_ENTRY)((value ^ -SignBit) + SignBit))* DeQuant[*(MergedScanOrderPtr)];         
                Prec = (value>1)?2:1;        
                MergedScanOrderPtr ++;			
            }

        } while (MergedScanOrderPtr < (pbi->MergedScanOrder + BLOCK_SIZE));

	    MergedScanOrderPtr--;

    Finished:
#ifdef STABILITY
		if(br->errorflag)
			return -11;
#endif
	    //EobArray[blockIndex] =  pbi->EobOffsetTable[(UINT32)(MergedScanOrderPtr - (pbi->MergedScanOrder))];
	    //pbi->mbi.blockDxInfo[blockIndex].EobPos =  (unsigned int)(MergedScanOrderPtr - pbi->MergedScanOrder);
	    pbi->mbi.blockDxInfo[blockIndex].EobPos =  pbi->EobOffsetTable[(unsigned int)(MergedScanOrderPtr - pbi->MergedScanOrder)];
		//printf("\n length = %d", pbi->mbi.blockDxInfo[blockIndex].EobPos); 
    } //for(blockIndex = 0; blockIndex < 6; blockIndex++)
    //}while(++bdi < bdiEnd);
	return 0;

}



