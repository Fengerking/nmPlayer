/****************************************************************************
*
*   Module Title :     readtokens_arith.c
*
*   Description  :     read arithmetic coded tokens
*
*   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
*
****************************************************************************/


/****************************************************************************
*  Header Files
****************************************************************************/
#include "pbdll.h"
#include "readtokens_arith.h"

extern TOKENEXTRABITS VP6_TokenExtraBits2[MAX_ENTROPY_TOKENS];

/***************************************************************************
*
****************************************************************************/
#define MergedScanOrderPtrPlus64 (MergedScanOrderPtr + 64)
#define MergedScanOrderPtrIncPlus64 (++MergedScanOrderPtr + 64)


/****************************************************************************
 * 
 *
 ****************************************************************************/
#ifdef STABILITY2
#define APPLYSIGN_DC(dest, valueToSign) \
{ \
    split    = (range + 1) >> 1; \
	if ( value >= split<<24 ) \
	{ \
		value = value - (split<<24); \
        value += value; \
		range = range - split; \
        range += range; \
        if( !--count ) \
        { \
            count  = 8; \
			if(brBuffer > brBufferEnd )\
			value |=*brBuffer;\
			else\
            value |= *brBuffer++; \
        } \
        dest = -valueToSign; \
    } \
    else \
    { \
        range = split; \
        range += range; \
        value += value; \
        if( !--count ) \
        { \
            count  = 8; \
			if(brBuffer > brBufferEnd)\
			value |=*brBuffer;\
			else\
            value |= *brBuffer++; \
        } \
        dest = valueToSign; \
    } \
}

#define APPLYSIGN_AC(dest, dequantPtrvalue, valueToSign) \
{ \
    split    = (range + 1) >> 1; \
	if ( value >= split<<24 ) \
	{ \
		value = value - (split<<24); \
        value += value; \
		range = range - split; \
        range += range; \
        if( !--count ) \
        { \
            count  = 8; \
			if(brBuffer > brBufferEnd)\
			value |=*brBuffer;\
			else\
            value |= *brBuffer++; \
        } \
        dest = (-valueToSign)*dequantPtrvalue; \
    } \
    else \
    { \
        range = split; \
        range += range; \
        value += value; \
        if( !--count ) \
        { \
            count  = 8; \
			if(brBuffer > brBufferEnd)\
			value |=*brBuffer;\
			else\
            value |= *brBuffer++; \
        } \
        dest = (valueToSign)*dequantPtrvalue; \
    } \
}

/****************************************************************************
 * 
 *
 ****************************************************************************/
#define NDECODEBOOL_AND_BRANCH_IF_ONE(branch) \
{ \
	if ( value >= split<<24 ) \
	{ \
		value -= (split<<24); \
		range = range - split; \
		if ( range < 0x80 ) {\
         while(range < 0x80 ) \
         { \
	        range += range; \
	        value += value; \
	        if ( !--count ) \
	        { \
			if(brBuffer > brBufferEnd)\
				value |=*brBuffer;\
			else\
		        value |= *brBuffer++; \
		        count  = 8; \
	        } \
         } \
		}\
        goto branch; \
	} \
	range = split; \
	if (range < 0x80)\
	{\
		while(range < 0x80 ) \
		{ \
			range += range; \
			value += value; \
			if ( !--count ) \
			{ \
			if(brBuffer > brBufferEnd)\
				value |=*brBuffer;\
			else\
				value |= *brBuffer++; \
				count  = 8; \
			} \
		} \
	}\
}
/****************************************************************************
 * 
 *
 ****************************************************************************/
#define NDECODEBOOL_AND_BRANCH_IF_ZERO(branch) \
{ \
	if ( value < split<<24 ) \
	{ \
    	range = split; \
		if ( range < 0x80 ) {\
         while(range < 0x80 ) \
         { \
	        range += range; \
	        value += value; \
	        if ( !--count ) \
	        { \
			if(brBuffer > brBufferEnd)\
				value |=*brBuffer;\
			else\
		        value |= *brBuffer++; \
		        count  = 8; \
	        } \
         } \
		}\
        goto branch; \
	} \
	value -= (split<<24); \
	range = range - split; \
	if (range < 0x80)\
	{\
		while(range < 0x80 ) \
		{ \
			range += range; \
			value += value; \
			if ( !--count ) \
			{ \
			if(brBuffer > brBufferEnd)\
				value |=*brBuffer;\
			else\
				value |= *brBuffer++; \
				count  = 8; \
			} \
		} \
	}\
}

#else
#define APPLYSIGN_DC(dest, valueToSign) \
{ \
    split    = (range + 1) >> 1; \
	if ( value >= split<<24 ) \
	{ \
		value = value - (split<<24); \
        value += value; \
		range = range - split; \
        range += range; \
        if( !--count ) \
        { \
            count  = 8; \
            value |= *brBuffer++; \
        } \
        dest = -valueToSign; \
    } \
    else \
    { \
        range = split; \
        range += range; \
        value += value; \
        if( !--count ) \
        { \
            count  = 8; \
            value |= *brBuffer++; \
        } \
        dest = valueToSign; \
    } \
}

#define APPLYSIGN_AC(dest, dequantPtrvalue, valueToSign) \
{ \
    split    = (range + 1) >> 1; \
	if ( value >= split<<24 ) \
	{ \
		value = value - (split<<24); \
        value += value; \
		range = range - split; \
        range += range; \
        if( !--count ) \
        { \
            count  = 8; \
            value |= *brBuffer++; \
        } \
        dest = (-valueToSign)*dequantPtrvalue; \
    } \
    else \
    { \
        range = split; \
        range += range; \
        value += value; \
        if( !--count ) \
        { \
            count  = 8; \
            value |= *brBuffer++; \
        } \
        dest = (valueToSign)*dequantPtrvalue; \
    } \
}

/****************************************************************************
 * 
 *
 ****************************************************************************/
#define NDECODEBOOL_AND_BRANCH_IF_ONE(branch) \
{ \
	if ( value >= split<<24 ) \
	{ \
		value -= (split<<24); \
		range = range - split; \
		if ( range < 0x80 ) {\
         while(range < 0x80 ) \
         { \
	        range += range; \
	        value += value; \
	        if ( !--count ) \
	        { \
		        value |= *brBuffer++; \
		        count  = 8; \
	        } \
         } \
		}\
        goto branch; \
	} \
	range = split; \
	if (range < 0x80)\
	{\
		while(range < 0x80 ) \
		{ \
			range += range; \
			value += value; \
			if ( !--count ) \
			{ \
				value |= *brBuffer++; \
				count  = 8; \
			} \
		} \
	}\
}
/****************************************************************************
 * 
 *
 ****************************************************************************/
#define NDECODEBOOL_AND_BRANCH_IF_ZERO(branch) \
{ \
	if ( value < split<<24 ) \
	{ \
    	range = split; \
		if ( range < 0x80 ) {\
         while(range < 0x80 ) \
         { \
	        range += range; \
	        value += value; \
	        if ( !--count ) \
	        { \
		        value |= *brBuffer++; \
		        count  = 8; \
	        } \
         } \
		}\
        goto branch; \
	} \
	value -= (split<<24); \
	range = range - split; \
	if (range < 0x80)\
	{\
		while(range < 0x80 ) \
		{ \
			range += range; \
			value += value; \
			if ( !--count ) \
			{ \
				value |= *brBuffer++; \
				count  = 8; \
			} \
		} \
	}\
}
#endif
	

#define MUL_RANGE_PRO \
{ \
	split = (256 +  (( probability*(range-1) ) ))>> 8; \
}
//bit = value < (split = (256 +  (((range-1) * probability) ))>> 8)<<24; 

/****************************************************************************
****************************************************************************/
INT32 VP6_ReadTokensPredictA_MB(PB_INSTANCE *pbi) 
{
    BLOCK_DX_INFO * bdi = pbi->mbi.blockDxInfo;
    BLOCK_DX_INFO * bdiEnd = bdi + 6;
    UINT8 * MergedScanOrder = pbi->MergedScanOrder;
    UINT8 * MergedScanOrderEnd = MergedScanOrder + BLOCK_SIZE;
    UINT8 * MergedScanOrderPtr;
	INT32 token, temp;
    register int count = pbi->mbi.br->count;
    register unsigned int range = pbi->mbi.br->range;
    register unsigned int value = pbi->mbi.br->value;
    register unsigned char * brBuffer = pbi->mbi.br->buffer;
//	register unsigned char * brBufferEnd = pbi->mbi.br->buffer_end;
	register unsigned int probability;
	register unsigned int split;

    UINT8 *BaselineProbsPtr;
	UINT8 *ContextProbsPtr;
	TOKENEXTRABITS *pVP6_TokenExtraBits2 = VP6_TokenExtraBits2;
	

	brBuffer += pbi->mbi.br->pos;


    do
    {
		UINT8 *AcProbsPtr = bdi->AcProbsBasePtr;

        MergedScanOrderPtr = MergedScanOrder;
        ContextProbsPtr = bdi->DcNodeContextsBasePtr + DcNodeOffset(0, (bdi->Left->Token + bdi->Above->Token), 0);
        BaselineProbsPtr = bdi->DcProbsBasePtr;
		probability = ContextProbsPtr[ZERO_CONTEXT_NODE];

	    // Decode the dc token -- first test to see if it is zero
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ONE(DC_NON_ZERO_);
		// Zero is implicit for DC token
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 0, *MergedScanOrderPtrIncPlus64, 0 );
		bdi->Left->Token = 0;// Update the above and left token contexts to indicate a zero
		bdi->Above->Token = 0;

        goto AC_DO_WHILE;

DC_NON_ZERO_:
	    // A non zero DC value
		bdi->Left->Token = 1;
		bdi->Above->Token = 1;

		probability = ContextProbsPtr[ONE_CONTEXT_NODE];
		MUL_RANGE_PRO;
		probability = ContextProbsPtr[LOW_VAL_CONTEXT_NODE];
		// Was the value a 1
        NDECODEBOOL_AND_BRANCH_IF_ZERO(ONE_CONTEXT_NODE_0_);

		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[HIGH_LOW_CONTEXT_NODE];
		// Value token > 1
        NDECODEBOOL_AND_BRANCH_IF_ZERO(LOW_VAL_CONTEXT_NODE_0_);
			

		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[CAT_THREEFOUR_CONTEXT_NODE];
		// High value (value category) token
        NDECODEBOOL_AND_BRANCH_IF_ZERO(HIGH_LOW_CONTEXT_NODE_0_);


		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[CAT_FIVE_CONTEXT_NODE];
		// Cat3,Cat4 or Cat5
        NDECODEBOOL_AND_BRANCH_IF_ZERO(CAT_THREEFOUR_CONTEXT_NODE_0_);

		MUL_RANGE_PRO;
		token = DCT_VAL_CATEGORY5;

        NDECODEBOOL_AND_BRANCH_IF_ZERO(DC_EXTRA_BITS_);

        token += 1;

        goto DC_EXTRA_BITS_;
                
CAT_THREEFOUR_CONTEXT_NODE_0_:
		token = DCT_VAL_CATEGORY3;
        probability = BaselineProbsPtr[CAT_THREE_CONTEXT_NODE];
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(DC_EXTRA_BITS_);

        token += 1;

        goto DC_EXTRA_BITS_;

HIGH_LOW_CONTEXT_NODE_0_:
		// Either Cat1 or Cat2
		token = DCT_VAL_CATEGORY1;

		probability = BaselineProbsPtr[CAT_ONE_CONTEXT_NODE];
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(DC_EXTRA_BITS_);

        token += 1;

DC_EXTRA_BITS_:
        {
  			unsigned int p2;
			register unsigned int split;
            register int BitsCount = pVP6_TokenExtraBits2[token].Length;
            int tValue = pVP6_TokenExtraBits2[token].MinVal;

			p2 = pVP6_TokenExtraBits2[token].Probs[BitsCount];
			temp = (range-1);
			temp = p2*temp;
			for (;;)
			{
				p2 = pVP6_TokenExtraBits2[token].Probs[BitsCount -1];
				split = (256 + temp) >> 8;
	            if ( value >= split<<24 )
	            {
		            value -= (split<<24);
		            split = range - split;        
            		tValue += ( 1 << BitsCount );
	            }
				if ( split < 0x80 )
				{
					while(split < 0x80 )
					{
						split += split;
						value += value;						
						if ( !--count ) 
						{
							count  = 8;
							value |= *brBuffer++;
						}
					}
				}		
	            range = split;
				if ( --BitsCount < 0 )
					break;
				temp = (range-1)*p2;

			} 

			// apply the sign to the value
            APPLYSIGN_DC(bdi->coeffsPtr[0], tValue);
            BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 2, *MergedScanOrderPtrIncPlus64, 0 );

            goto AC_DO_WHILE;
        }

LOW_VAL_CONTEXT_NODE_0_:
		probability = ContextProbsPtr[TWO_CONTEXT_NODE];
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[THREE_CONTEXT_NODE];

		// Low value token
        NDECODEBOOL_AND_BRANCH_IF_ZERO(TWO_CONTEXT_NODE_0_);
		// Either a 3 or a 4
		token = THREE_TOKEN;
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(THREE_CONTEXT_NODE_0_);      
        token += 1;

THREE_CONTEXT_NODE_0_:

		// apply the sign to the value
        APPLYSIGN_DC(bdi->coeffsPtr[0], token);
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 2, *MergedScanOrderPtrIncPlus64, 0 );

        goto AC_DO_WHILE;

TWO_CONTEXT_NODE_0_:
		// Is it a  2
		// apply the sign to the value
        APPLYSIGN_DC(bdi->coeffsPtr[0], TWO_TOKEN);
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 2, *MergedScanOrderPtrIncPlus64, 0 );

        goto AC_DO_WHILE;

ONE_CONTEXT_NODE_0_:
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 1, *MergedScanOrderPtrIncPlus64, 0 );
		// apply the sign to the value
        APPLYSIGN_DC(bdi->coeffsPtr[0], 1);

AC_DO_WHILE:
		probability = BaselineProbsPtr[ZERO_CONTEXT_NODE];
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[EOB_CONTEXT_NODE];

		// calculate the context for the next token. 
        NDECODEBOOL_AND_BRANCH_IF_ONE(NON_ZERO_RUN_);

		MUL_RANGE_PRO;

		// Is the token a Zero or EOB
        NDECODEBOOL_AND_BRANCH_IF_ZERO(BLOCK_FINISHED_1);

		// Select the appropriate Zero run context
		BaselineProbsPtr = bdi->ZeroRunProbsBasePtr;
		
        if(MergedScanOrderPtr >= (pbi->MergedScanOrder + ZRL_BAND2))
            BaselineProbsPtr += ZERO_RUN_PROB_CASES;

		// Now decode the zero run length
		// Run lenght 1-4
		probability = BaselineProbsPtr[0];
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[1];

        NDECODEBOOL_AND_BRANCH_IF_ONE(ZERO_RUN_5_8);

		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[2];

        NDECODEBOOL_AND_BRANCH_IF_ONE(ZERO_RUN_1_4_a);

		MergedScanOrderPtr += 1;


		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(ZERO_RUN_1_4_done);

		MergedScanOrderPtr += 1;

ZERO_RUN_1_4_done:
    	BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 0, *MergedScanOrderPtrPlus64, 0 );
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto NON_ZERO_RUN_;

        goto BLOCK_FINISHED;
    
ZERO_RUN_1_4_a:
		MergedScanOrderPtr += 3;
        probability = BaselineProbsPtr[3];
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(ZERO_RUN_1_4_a_done);

		MergedScanOrderPtr += 1;

ZERO_RUN_1_4_a_done:
    	BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 0, *MergedScanOrderPtrPlus64, 0 );
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto NON_ZERO_RUN_;

        goto BLOCK_FINISHED;

ZERO_RUN_5_8:
		// Run length 5-8
		probability = BaselineProbsPtr[4];
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[5];
        NDECODEBOOL_AND_BRANCH_IF_ONE(ZERO_RUN_gt_8);
		MUL_RANGE_PRO;
        probability = BaselineProbsPtr[6];
        NDECODEBOOL_AND_BRANCH_IF_ONE(ZERO_RUN_5_8_a);
		MergedScanOrderPtr += 5;
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(ZERO_RUN_5_8_done);
		MergedScanOrderPtr += 1;

ZERO_RUN_5_8_done:
    	BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 0, *MergedScanOrderPtrPlus64, 0 );
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto NON_ZERO_RUN_;

        goto BLOCK_FINISHED;

ZERO_RUN_5_8_a:
		MergedScanOrderPtr += 7;
        probability = BaselineProbsPtr[7];
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(ZERO_RUN_5_8_a_done);

		MergedScanOrderPtr += 1;

ZERO_RUN_5_8_a_done:
    	BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 0, *MergedScanOrderPtrPlus64, 0 );
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto NON_ZERO_RUN_;

        goto BLOCK_FINISHED;

ZERO_RUN_gt_8:
		// Run length > 8
        {
	        register unsigned int decodeCount = 0;
			register unsigned int temp = BaselineProbsPtr[8 + decodeCount]*(range-1)>>8;
			register unsigned int split;
			unsigned int p2;

			for (;;)
            {
	            p2 = BaselineProbsPtr[9 + decodeCount];
				// perform the actual decoding
				split = (1 + temp);
	            if ( value >= split<<24 )
	            {
		            value -= (split<<24);
            		MergedScanOrderPtr += ( 1 << decodeCount );
		            split = range - split;
	            }
				if ( split < 0x80 )
				{
					while(split < 0x80 )
					{
						split += split;
						value += value;					
						if ( !--count ) 
						{
							value |= *brBuffer++;
							count  = 8;
						}
					}
				}

	            range = split; 
				if ( ++decodeCount >= 6 )
					break;
				temp = p2*(range-1)>>8;
            }
    		MergedScanOrderPtr += 9;

        } 

        if( MergedScanOrderPtr >= MergedScanOrderEnd)
            goto BLOCK_FINISHED;
        
    	BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 0, *MergedScanOrderPtrPlus64, 0 );


NON_ZERO_RUN_:
		// The token codes a non zero value
		probability = BaselineProbsPtr[ONE_CONTEXT_NODE];
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[LOW_VAL_CONTEXT_NODE];
		NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_ONE_CONTEXT_0_);
		MUL_RANGE_PRO;
        probability = BaselineProbsPtr[HIGH_LOW_CONTEXT_NODE];
 		// Value token > 1
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_LOW_VAL_CONTEXT_0_);
        								
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[CAT_THREEFOUR_CONTEXT_NODE];
    	// High value (value category) token
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_HIGH_LOW_CONTEXT_0_);
		MUL_RANGE_PRO;
		probability = BaselineProbsPtr[CAT_FIVE_CONTEXT_NODE];
		// Cat3,Cat4
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_CAT_THREEFOUR_CONTEXT_0_);

		MUL_RANGE_PRO;
		token = DCT_VAL_CATEGORY5;

		// Cat5,Cat6
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_EXTRA_BITS_);


        //It is Cat6
        token += 1;

        goto AC_EXTRA_BITS_;

AC_CAT_THREEFOUR_CONTEXT_0_:
		token = DCT_VAL_CATEGORY3;
		probability = BaselineProbsPtr[CAT_THREE_CONTEXT_NODE];
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_EXTRA_BITS_);

        //It is Cat4
        token += 1;

        goto AC_EXTRA_BITS_;

AC_HIGH_LOW_CONTEXT_0_:
		// Either Cat1 or Cat2
		token = DCT_VAL_CATEGORY1;
		probability = BaselineProbsPtr[CAT_ONE_CONTEXT_NODE];
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_EXTRA_BITS_);
        
        //It is Cat2
        token += 1;

AC_EXTRA_BITS_:
		{
			unsigned int p2;
			unsigned int temp;
			register unsigned int split;
            register int BitsCount = pVP6_TokenExtraBits2[token].Length;
            int tValue = pVP6_TokenExtraBits2[token].MinVal;

			p2 = pVP6_TokenExtraBits2[token].Probs[BitsCount];
			temp = (range-1);
			temp = p2*temp>>8;
			for (;;)
			{
				p2 = pVP6_TokenExtraBits2[token].Probs[BitsCount -1];
				split = (1 + temp);
	            if ( value >= split<<24 )
	            {
		            value -= (split<<24);
		            split = range - split;        
            		tValue += ( 1 << BitsCount );
	            }
				if ( split < 0x80 )
				{
					while(split < 0x80 )
					{
						split += split;
						value += value;						
						if ( !--count ) 
						{
							value |= *brBuffer++;
							count  = 8;
						}
					}
				}		
	            range = split;
				if ( --BitsCount < 0 )
					break;
				temp = p2*(range-1)>>8;

			} 
        	// apply the sign to the value
            //APPLYSIGN_AC(bdi->coeffsPtr[*(MergedScanOrderPtr++)], bdi->dequantPtr[*(MergedScanOrderPtr)], tValue);
			
			//1245
			APPLYSIGN_AC(bdi->coeffsPtr[*(MergedScanOrderPtr)], bdi->dequantPtr[*(MergedScanOrderPtr)], tValue);
		    MergedScanOrderPtr++;

           //APPLYSIGN_AC(bdi->coeffsPtr[*(MergedScanOrderPtr)], bdi->dequantPtr[*(MergedScanOrderPtr+1)], tValue);
		   //MergedScanOrderPtr++;
        }
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 2, *MergedScanOrderPtrPlus64, 0 );
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto AC_DO_WHILE;
        
        goto BLOCK_FINISHED;


AC_LOW_VAL_CONTEXT_0_:
		probability = BaselineProbsPtr[TWO_CONTEXT_NODE];
		MUL_RANGE_PRO;
        probability = BaselineProbsPtr[THREE_CONTEXT_NODE];

		// Low value token
        NDECODEBOOL_AND_BRANCH_IF_ZERO(AC_TWO_CONTEXT_0_);

		// Either a 3 or a 4
		token = THREE_TOKEN + 1;
		MUL_RANGE_PRO;
        NDECODEBOOL_AND_BRANCH_IF_ONE(AC_THREE_CONTEXT_1_);

        //It is a 3
        token = token - 1;

AC_THREE_CONTEXT_1_:
        // apply the sign to the value
        APPLYSIGN_AC(bdi->coeffsPtr[*(MergedScanOrderPtr)], bdi->dequantPtr[*(MergedScanOrderPtr)],token);
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 2, *MergedScanOrderPtrIncPlus64, 0 );
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto AC_DO_WHILE;
       
        goto BLOCK_FINISHED;


AC_TWO_CONTEXT_0_:
		// Is it a  2
        // apply the sign to the TWO_TOKEN
        APPLYSIGN_AC(bdi->coeffsPtr[*(MergedScanOrderPtr)], bdi->dequantPtr[*(MergedScanOrderPtr)], TWO_TOKEN);
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 2, *MergedScanOrderPtrIncPlus64, 0 );

        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto AC_DO_WHILE;
        
        goto BLOCK_FINISHED;

AC_ONE_CONTEXT_0_:
		// apply the sign to the value
        APPLYSIGN_AC(bdi->coeffsPtr[*(MergedScanOrderPtr)], bdi->dequantPtr[*(MergedScanOrderPtr)], 1);
        BaselineProbsPtr = AcProbsPtr + ACProbOffset(0, 1, *MergedScanOrderPtrIncPlus64, 0 );
    
        if( MergedScanOrderPtr < MergedScanOrderEnd)
            goto AC_DO_WHILE;
	
BLOCK_FINISHED:
        MergedScanOrderPtr--;

BLOCK_FINISHED_1:				    
	    bdi->EobPos =  pbi->EobOffsetTable[(unsigned int)(MergedScanOrderPtr - MergedScanOrder)];
         //printf("\n length = %d", bdi->EobPos); 
    }while(++bdi < bdiEnd); 


    brBuffer -= pbi->mbi.br->pos;
	//brBuffer = (brBuffer - pbi->mbi.br->buffer);
	//pbi->mbi.br->pos += (unsigned int)brBuffer;
    pbi->mbi.br->pos += (unsigned int)(brBuffer - pbi->mbi.br->buffer);
    pbi->mbi.br->count = count;
    pbi->mbi.br->value = value;
    pbi->mbi.br->range = range;
	return 0;
//END:
//	return -11;

}

