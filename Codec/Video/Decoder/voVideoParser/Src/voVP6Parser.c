#include <stdlib.h>
#include "string.h"
#include "voVP6Parser.h"

#define SIMPLE_PROFILE			0
#define PROFILE_1				1
#define PROFILE_2				2
#define ADVANCED_PROFILE		3
#define BASE_FRAME              0
#define NORMAL_FRAME            1
#define BILINEAR_ONLY_PM	    0
#define BICUBIC_ONLY_PM		    1
#define AUTO_SELECT_PM		    2

int VP6_DecodeBool ( VP6_BOOL_CODER *br, VO_S32 probability ) 
{
    VO_U32 bit=0;
	VO_U32 split;
	VO_U32 bigsplit;
    VO_U32 count = br->count;
    VO_U32 range = br->range;
    VO_U32 value = br->value;

	split = 1 +  (((range-1) * probability) >> 8);	
    bigsplit = (split<<24);
    
    range = split;
	if(value >= bigsplit)
	{
		range = br->range-split;
		value = value-bigsplit;
		bit = 1;
	}

	if(range>=0x80)
    {
        br->value = value;
        br->range = range;
        return bit;
    }
    else
	{
		do
		{
       	    range +=range;
            value +=value;
            
        	if (!--count) 
        	{
    	        count = 8;
				value |= br->buffer[br->pos];
				br->pos++;        	   
	    	}
        } 
        while(range < 0x80 );
    }
    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
} 

VO_S32 VP6_DecodeBool128 (VP6_BOOL_CODER *br ) 
{
    VO_U32 bit;
	VO_U32 split;
	VO_U32 bigsplit;
    VO_U32 count = br->count;
    VO_U32 range = br->range;
    VO_U32 value = br->value;

    split = ( range + 1) >> 1;
    bigsplit = (split<<24);
    
	if(value >= bigsplit)
	{
		range = (range-split)<<1;
		value = (value-bigsplit)<<1;
		bit = 1;
	}
	else
	{	
		range = split<<1;
		value = value<<1;
		bit = 0;
	}

    if(!--count)
    {
        count=8;
		value |= br->buffer[br->pos];
		br->pos++; 
    }
    br->count = count;
    br->value = value;
    br->range = range;
    return bit;        
}

VO_U32 VP6_bitread ( VP6_BOOL_CODER *br, VO_S32 bits )
{
	VO_U32 z = 0;
	VO_S32 bit;
	for ( bit=bits-1; bit>=0; bit-- )
	{
		z |= (VP6_DecodeBool128(br)<<bit);
	}
	return z;
}
void VP6_StartDecode ( VP6_BOOL_CODER *br, VO_U8 *source )
{
	br->lowvalue = 0;
	br->range    = 255;
	br->count    = 8;
	br->buffer   = source;
	br->pos      = 0;
	br->value    = (br->buffer[0]<<24)+(br->buffer[1]<<16)+(br->buffer[2]<<8)+(br->buffer[3]);
	br->pos     += 4;
}

VO_S32 ReadHeaderBits ( VP6_FRAME_HEADER *Header, VO_U32 BitsRequired )
{
    VO_U32 pos       = Header->pos;
    VO_U32 available = Header->bits_available;
    VO_U32 value     = Header->value;
    VO_U8 *Buffer    = &Header->buffer[pos];
    VO_U32 RetVal    = 0;

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

VO_S32 voVP6Init(VO_HANDLE *pParHandle)
{
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER*)malloc(sizeof(VO_VIDEO_PARSER));
	if(!pParser)
		return VO_ERR_VIDEOPARSER_MEMORY; 
	memset(pParser,0xFF,sizeof(VO_VIDEO_PARSER));
	*pParHandle = pParser;
	return VO_ERR_NONE;
}

VO_S32 voVP6Parser(VO_HANDLE pParHandle,VO_CODECBUFFER *pInData)
{
	VO_VIDEO_PARSER *pVideoInfo = (VO_VIDEO_PARSER*)pParHandle;
	VO_U8* inBuffer = pInData->Buffer;
	VO_U32 BufferLen = pInData->Length;
	VP6_FRAME_HEADER Header;
	VO_U32 DctQMask = 0;
	VO_U32 MultiStream = 0;
	VO_U32 Buff2Offset = 0;
	VO_U32 VFragments =0;
    VO_U32 HFragments =0;
    VO_U32 OutputVFragments =0;
	VO_U32 OutputHFragments =0;
	VO_U32 ScalingMode =0;
	VO_U32 PredictionFilterMode =0;
	VO_U32 PredictionFilterVarThresh =0;
	VO_U32 PredictionFilterMvSizeThresh =0;
	VO_U32 PredictionFilterAlpha =0;
	VO_U32 RefreshGoldenFrame =0;
	VO_U32 UseLoopFilter =0;
	VP6_BOOL_CODER br;

	Header.buffer = inBuffer;
    Header.value  = (inBuffer[0]<<24)+(inBuffer[1]<<16)+(inBuffer[2]<<8)+inBuffer[3];
    Header.bits_available = 32;
	Header.pos = 4;

	pVideoInfo->nFrame_type = (VO_U8)ReadHeaderBits(&Header, 1);
	DctQMask = (VO_U32)ReadHeaderBits(&Header, 6);
    MultiStream = (VO_U32)ReadHeaderBits(&Header, 1);

	if (pVideoInfo->nFrame_type == BASE_FRAME) 
	{
	    pVideoInfo->nVersion = (VO_U32)ReadHeaderBits(&Header,   5 );
	    pVideoInfo->nProfile = (VO_U32)ReadHeaderBits(&Header,   2 );

		pVideoInfo->FirstFrameisKey = 1;

		if( (VO_U32)ReadHeaderBits(&Header, 1) != 0)
            return VO_ERR_VIDEOPARSER_NOTSUPPORT;  

		if(MultiStream)
		{
			VP6_StartDecode(&br, ((VO_U8*)(Header.buffer + 4)));
			br.buffer_end = Header.buffer+BufferLen;
			Buff2Offset = (VO_U32)ReadHeaderBits(&Header, 16);
		}
		else
		{
			VP6_StartDecode(&br, ((VO_U8*)(Header.buffer + 2)));
			br.buffer_end = Header.buffer+BufferLen;
		}

		VFragments = 2 * ((VO_U32)VP6_bitread( &br,   8 ));
		HFragments = 2 * ((VO_U32)VP6_bitread( &br,   8 ));
		OutputVFragments = 2 * ((VO_U8)VP6_bitread( &br,   8 ));
		OutputHFragments = 2 * ((VO_U8)VP6_bitread( &br,   8 ));
		ScalingMode = ((VO_U32)VP6_bitread( &br, 2 ));
		pVideoInfo->nWidth = HFragments*8;
		pVideoInfo->nHeight = VFragments*8;

		if( pVideoInfo->nProfile != SIMPLE_PROFILE )
		{
			if((VO_S32)VP6_DecodeBool128(&br))
			{
				PredictionFilterMode = AUTO_SELECT_PM;
				PredictionFilterVarThresh = ((VO_U32)VP6_bitread(&br, 5) << ((pVideoInfo->nVersion > 7) ? 0 : 5) );
				PredictionFilterMvSizeThresh = (VO_U32)VP6_bitread(&br, 3);
			}
			else
			{
				if ( (VO_S32)VP6_DecodeBool(&br, 128) )
					PredictionFilterMode = BICUBIC_ONLY_PM;
                else
					PredictionFilterMode = BILINEAR_ONLY_PM;

				if ( pVideoInfo->nVersion > 7 )
					PredictionFilterAlpha = VP6_bitread(&br, 4);
				else
					PredictionFilterAlpha = 16;	// VP61 backwards compatibility
			}
		}
		else
		{
			PredictionFilterAlpha = 14;	
		    PredictionFilterMode = BILINEAR_ONLY_PM;	
		}
	}
	else // inter frame
	{
		if(pVideoInfo->FirstFrameisKey == 0)
			return VO_ERR_VIDEOPARSER_INPUTDAT;
		if (MultiStream)
		{
			VP6_StartDecode(&br, ((VO_U8*)(Header.buffer + 3)));
			br.buffer_end = Header.buffer+BufferLen;		
		    Buff2Offset = (VO_U32)ReadHeaderBits(&Header, 16);
		}
		else
		{
			VP6_StartDecode(&br, ((VO_U8*)(Header.buffer + 1)));
			br.buffer_end = Header.buffer+BufferLen;
		}
		RefreshGoldenFrame = (VO_U32)VP6_DecodeBool128(&br);

		if (  pVideoInfo->nProfile != SIMPLE_PROFILE )
		{
			UseLoopFilter = VP6_DecodeBool128(&br);
			if (UseLoopFilter )
				UseLoopFilter = (UseLoopFilter << 1) | VP6_DecodeBool128(&br);

			if ( pVideoInfo->nVersion > 7 )
			{
				if ( VP6_DecodeBool128(&br) )
				{
					if ( (VO_S32)VP6_DecodeBool128(&br) )
					{
						PredictionFilterMode = AUTO_SELECT_PM;
						PredictionFilterVarThresh = (VO_U32)VP6_bitread( &br, 5);
						PredictionFilterMvSizeThresh = (VO_U32)VP6_bitread( &br, 3);
					}
					else
					{
						if ( (VO_S32)VP6_DecodeBool128(&br) )
							PredictionFilterMode = BICUBIC_ONLY_PM;
						else
							PredictionFilterMode = BILINEAR_ONLY_PM;
					}
					PredictionFilterAlpha = VP6_bitread( &br, 4 );
				}
			}
			else
				PredictionFilterAlpha = 16;	// VP61 backwards compatibility
		}
		else
		{
			PredictionFilterAlpha = 14;	
		    PredictionFilterMode = BILINEAR_ONLY_PM;	
		}		
	} 

	return VO_RETURN_FMHEADER;

}


VO_S32 voVP6GetInfo(VO_HANDLE pParHandle,VO_S32 nID,VO_PTR pValue)
{
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER*)pParHandle;
	switch(nID)
	{
	case VO_PID_VIDEOPARSER_VERSION:
		*((VO_U32*)pValue) = pParser->nVersion;
		break;
	case VO_PID_VIDEOPARSER_PROFILE:
		*((VO_U32*)pValue) = pParser->nProfile;
		break;
	case VO_PID_VIDEOPARSER_FRAMETYPE:
		*((VO_U32*)pValue) = pParser->nFrame_type;
		break;
	case VO_PID_VIDEOPARSER_WIDTH:
		*((VO_U32*)pValue) = pParser->nWidth;
		break;
	case VO_PID_VIDEOPARSER_HEIGHT:
		*((VO_U32*)pValue) = pParser->nHeight;
		break;
	case VO_PID_VIDEOPARSER_INTERLANCE:
		*((VO_U32*)pValue) =0;
		break;
	case VO_PID_VIDEOPARSER_REFFRAME:
		*((VO_U32*)pValue) = 1;
		break;
	case VO_PID_VIDEOPARSER_ISVC1:
		*((VO_U32*)pValue) = 0;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	if(*((VO_U32*)pValue)  == 0xFFFFFFFF)
		return VO_ERR_VIDEOPARSER_NOVALUE;

	return VO_ERR_NONE;
}

VO_S32 voVP6Uninit(VO_HANDLE pParHandle)
{
	VO_VIDEO_PARSER *pParser = (VO_VIDEO_PARSER*)pParHandle;
	if(pParser)
	{
		free(pParser);
		pParser = NULL;
	}

	return VO_ERR_NONE;
}