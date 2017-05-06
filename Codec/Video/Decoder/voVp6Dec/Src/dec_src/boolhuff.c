/****************************************************************************
*
*   Module Title :     boolhuff.c
*
*   Description  :     Boolean Encoder/Decoder
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "boolhuff.h"
#include "tokenentropy.h"
#include <stdio.h>

// STATS Variables for measuring section costs

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeBool
 *
 *  INPUTS        :     BOOL_CODER *br  : pointer to instance of a boolean decoder.
 *						int probability : probability that next symbol is a 0 (0-255) 
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :		Next decoded symbol (0 or 1)
 *
 *  FUNCTION      :     Decodes the next symbol (0 or 1) using the specified
 *                      boolean decoder.
 *
 *  SPECIAL NOTES :     None.
 *
 ****************************************************************************/
int VP6_DecodeBool ( BOOL_CODER *br, int probability ) 
{
    unsigned int bit=0;
	unsigned int split;
	unsigned int bigsplit;
    unsigned int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

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
#ifdef STABILITY2
				if(&br->buffer[br->pos] > br->buffer_end)
				{
					br->errorflag = 1;
				}
				else
#endif
				{
					value |= br->buffer[br->pos];
					 br->pos++;
				}
        	   
	    	}
        } 
        while(range < 0x80 );
    }
    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
} 

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_DecodeBool128
 *
 *  INPUTS        :     BOOL_CODER *br : pointer to instance of a boolean decoder.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :		int: Next decoded symbol (0 or 1)
 *
 *  FUNCTION      :     This function determines the next value stored in the 
 *						boolean coder based upon a fixed probability of 0.5 
 *                      (128 in normalized units).
 *
 *  SPECIAL NOTES :     VP6_DecodeBool128() is a special case of VP6_DecodeBool()
 *                      where the input probability is fixed at 128.
 *
 ****************************************************************************/
int VP6_DecodeBool128 (	BOOL_CODER	*br ) 
{
    unsigned int bit;
	unsigned int split;
	unsigned int bigsplit;
    unsigned int count = br->count;
    unsigned int range = br->range;
    unsigned int value = br->value;

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
#ifdef STABILITY2
		if(&br->buffer[br->pos] > br->buffer_end)
		{
			br->errorflag = 1;
		}
		else
#endif
		{
			value |= br->buffer[br->pos];
			br->pos++; 
		}
    }
    br->count = count;
    br->value = value;
    br->range = range;
    return bit;
        
}    

/****************************************************************************
 * 
 *  ROUTINE       :     VP6_StartDecode
 *
 *  INPUTS        :     BOOL_CODER *bc		  : pointer to instance of a boolean decoder.
 *						unsigned char *source : pointer to buffer of data to be decoded.
 *
 *  OUTPUTS       :     None.
 *
 *  RETURNS       :     void
 *
 *  FUNCTION      :     Performs initialization of the boolean decoder.
 *                           
 *  SPECIAL NOTES :     None. 
 *
 ****************************************************************************/
void VP6_StartDecode ( BOOL_CODER *br, unsigned char *source )
{
	br->lowvalue = 0;
	br->range    = 255;
	br->count    = 8;
	br->buffer   = source;
	br->pos      = 0;
	br->value    = (br->buffer[0]<<24)+(br->buffer[1]<<16)+(br->buffer[2]<<8)+(br->buffer[3]);
	br->pos     += 4;
}
