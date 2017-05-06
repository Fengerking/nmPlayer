    /************************************************************************
	*									*									*
	*		VisualOn, Inc. Confidential and Proprietary, 2004	*
	*									*									*
	************************************************************************/

#include "decoder.h"
#include "bitstream.h"
#include "ps_dec.h"
#include "sbr_dec.h"

#ifdef PS_DEC

const unsigned char NrIidTab[8] = {
    10, 20, 34, 10, 20, 34, 0, 0
};

const unsigned char NrIccTab[8] = {
    10, 20, 34, 10, 20, 34, 0, 0
}; 

const unsigned char NrIpdOpdTab[8] = {
    5, 11, 17, 5, 11, 17, 0, 0
};

const unsigned char NumEnvTab[2][4] = {
    { 0, 1, 2, 4 },
    { 1, 2, 3, 4 }
};

const char fHuffIidBook[28][2] = {
    { -31,	 1 },   {   2,   3 },	 { -30, -32 },   {   4,   5 }, 
    { -29, -33 },   {   6,   7 },    { -28, -34 },   {   8,   9 },
    { -35, -27 },   { -26,  10 },    { -36,  11 },   { -25,  12 },
    { -37,  13 },   { -38,  14 },    { -24,  15 },   {  16,  17 },
    { -23, -39 },   {  18,  19 },    { -22, -21 },   {  20,  21 }, 
    { -40, -20 },   {  22,  23 },    { -41,  24 },   {  25,  26 },   
    { -42, -45 },   { -44, -43 },    { -19,  27 },   { -18, -17 }  
};

const char tHuffIidBook[28][2] = {
    { -31,   1 },    { -32,   2 },    { -30,   3 },   { -33,    4 },
    { -29,   5 },    { -34,   6 },    { -28,   7 },   { -35,    8 },  
    { -27,   9 },    { -36,  10 },    { -26,  11 },   { -37,   12 },      
    { -25,  13 },    { -24,  14 },    { -38,  15 },   {  16,   17 },              
    { -23, -39 },    {  18,  19 },    {  20,  21 },   {  22,   23 },              
    { -22, -45 },    { -44, -43 },    {  24,  25 },   {  26,   27 },              
    { -42, -41 },    { -40, -21 },    { -20, -19 },   { -18,  -17 }   
};

const char fHuffIccBook[14][2] = {
    { -31,   1 },    { -30,   2 },    { -32,   3 },   { -29,   4 },  
    { -33,   5 },    { -28,   6 },    { -34,   7 },   { -27,   8 },  
    { -26,   9 },    { -35,  10 },    { -25,  11 },   { -36,  12 },  
    { -24,  13 },    { -37, -38 }   
};

const char tHuffIccBook[14][2] = {
    { -31,   1 },    { -30,   2 },    { -32,   3 },   { -29,   4 },           
    { -33,   5 },    { -28,   6 },    { -34,   7 },   { -27,   8 },           
    { -35,   9 },    { -26,  10 },    { -36,  11 },   { -25,  12 },         
    { -37,  13 },    { -38, -24 }    
};

const char fHuffIidFineBook[60][2] = {
    {   1, -31 },    {   2,   3 },    {  4,  -32 },   { -30,   5 },        
    { -33, -29 },    {   6,   7 },    { -34, -28 },   {   8,   9 },               
    { -35, -27 },    {  10,  11 },    { -36, -26 },   {  12,  13 },                   
    { -37, -25 },    {  14,  15 },    { -24,  16 },   {  17,  18 },                  
    {  19, -39 },    {  -23, 20 },    {  21, -38 },   { -21,  22 },          
    {  23, -40 },    { -22,  24 },    { -42, -20 },   {  25,  26 },                 
    {  27, -41 },    {  28, -43 },    { -19,  29 },   {  30,  31 },                  
    {  32, -45 },    { -17,  33 },    {  34, -44 },   { -18,  35 },          
    {  36,  37 },    {  38, -46 },    { -16,  39 },   {  40,  41 },                 
    {  42,  43 },    { -48, -14 },    {  44,  45 },   {  46,  47 },                   
    {  48,  49 },    { -47, -15 },    { -52, -10 },   { -50, -12 }, 
    { -49, -13 },    {  50,  51 },    {  52,  53 },   {  54,  55 },                   
    {  56,  57 },    {  58,  59 },    { -57, -56 },   { -59, -58 }, 
    { -53,  -9 },    { -55, -54 },    {  -6,  -5 },   {  -8,  -7 },     
    {  -2,  -1 },    {  -4,  -3 },    { -61, -60 },   { -51, -11 }   
};

const char tHuffIidFineBook[60][2] = {
    {   1, -31 },    { -30,   2 },    {   3, -32 },   {   4,   5 },         
    {   6,   7 },    { -33, -29 },    {   8, -34 },   { -28,   9 },      
    { -35, -27 },    {  10,  11 },    { -26,  12 },   {  13,  14 },       
    { -37, -25 },    {  15,  16 },    {  17, -36 },   {  18, -38 },     
    { -24,  19 },    {  20,  21 },    { -22,  22 },   {  23,  24 },       
    { -39, -23 },    {  25,  26 },    { -20,  27 },   {  28,  29 },               
    { -41, -21 },    {  30,  31 },    {  32, -40 },   {  33, -44 },       
    { -18,  34 },    {  35,  36 },    {  37, -43 },   { -19,  38 },         
    {  39, -42 },    {  40,  41 },    {  42,  43 },   {  44,  45 },               
    {  46, -46 },    { -16,  47 },    { -45, -17 },   {  48,  49 },              
    { -52, -51 },    { -13, -12 },    { -50, -49 },   {  50,  51 },              
    {  52,  53 },    {  54,  55 },    {  56, -48 },   { -14,  57 },           
    {  58, -47 },    { -15,  59 },    { -57,  -5 },   { -59, -58 }, 
    {  -2,  -1 },    {  -4,  -3 },    { -61, -60 },   { -56,  -6 },  
    { -55,  -7 },    { -54,  -8 },    { -53,  -9 },   { -11, -10 }  
};


static void huff_data(BitStream *bs, const unsigned char dt, const unsigned char nr_par,
                      HuffPsTab t_huff, HuffPsTab f_huff, char *par);
static INLINE char ps_huff_dec(BitStream *bs, HuffPsTab t_huff);


int voAACReadPSData(AACDecoder *decoder, BitStream *bs, sbr_info *sbr, int bitsLeft)
{
    int ret;
	int ps_ext_read = 0;
	VO_MEM_OPERATOR *voMemop;

	voMemop = decoder->voMemop;

	while (bitsLeft > 7)
	{
		int usebits = 0;
		
		int bs_extension_id = BitStreamGetBits(bs, 2);
		usebits += 2;

		if (bs_extension_id == EXTENSION_ID_PS)
		{
			if (ps_ext_read == 0)
			{
				ps_ext_read = 1;
			} 
			else 
			{
				bs_extension_id = 3;
			}
		}

		switch (bs_extension_id)
		{
		case EXTENSION_ID_PS:
			if (!sbr->ps)
			{
				sbr->ps = ps_init(voMemop, sbr->sampRateIdx);
			}

			ret = ReadPsStream(sbr->ps, bs);

			/* enable PS if and only if: a header has been decoded */
			if (sbr->ps_used == 0 && sbr->ps->bEnableHeader)
			{
				sbr->ps_used = 1;
			}

			usebits += ret;

			break;
		default:
			BitStreamGetBits(bs, 6);
			usebits += 6;
			break;
		}

		if (usebits > bitsLeft)
		{
			return -1;
		}

		bitsLeft -= usebits;
	}
	
	if (bitsLeft > 0)
	{
		BitStreamGetBits(bs, bitsLeft);
	}

	return 0;
}

int ReadPsStream(ps_info *ps, BitStream *bs)
{
    int n, bits, NumIdx;
	int cachedBits = bs->cachedBits;
	unsigned char *bytePtr = bs->bytePtr;

	ps->bAvailblePsData = 0;

	ps->bEnableHeader = BitStreamGetBit(bs);

	if (ps->bEnableHeader)
    {
		ps->bEnableIid = (unsigned char)BitStreamGetBit(bs);
        if (ps->bEnableIid)
        {
            ps->IidMode = (unsigned char)BitStreamGetBits(bs, 3);
            ps->NrIidNum = NrIidTab[ps->IidMode];
        }

        ps->bEnableIcc = (unsigned char)BitStreamGetBit(bs);
        if (ps->bEnableIcc)
        {
            ps->IccMode = (unsigned char)BitStreamGetBits(bs, 3);
            ps->NrIccNum = NrIccTab[ps->IccMode];
        }

        ps->bEnableExt = (unsigned char)BitStreamGetBit(bs);

		if(ps->IidMode > 5 || ps->IccMode > 5)
		{
			ps->bEnableHeader = 0;
			return 10;
		}
    }

    ps->bFrameClass = (unsigned char)BitStreamGetBit(bs);

    NumIdx = (unsigned char)BitStreamGetBits(bs, 2);
    ps->NumEnv = NumEnvTab[ps->bFrameClass][NumIdx];

    if (ps->bFrameClass)
    {
        for (n = 1; n < ps->NumEnv+1; n++)
        {
            ps->aEnvStartStop[n] = (unsigned char)BitStreamGetBits(bs, 5) + 1;
        }
    }

    if (ps->bEnableIid)
    {
        for (n = 0; n < ps->NumEnv; n++)
        {
            ps->IidDtFlag[n] = (unsigned char)BitStreamGetBit(bs);
            if (ps->IidMode < 3)
            {
                huff_data(bs, ps->IidDtFlag[n], ps->NrIidNum, tHuffIidBook,
                    fHuffIidBook, ps->IidIndex[n]);
            } else {
                huff_data(bs, ps->IidDtFlag[n], ps->NrIidNum, tHuffIidFineBook,
                    fHuffIidFineBook, ps->IidIndex[n]);
            }
        }
    }

    if (ps->bEnableIcc)
    {
        for (n = 0; n < ps->NumEnv; n++)
        {
            ps->IccDtFlag[n] = (unsigned char)BitStreamGetBit(bs);
            huff_data(bs, ps->IccDtFlag[n], ps->NrIccNum, tHuffIccBook,
                fHuffIccBook, ps->IccIndex[n]);
        }
    }

    if (ps->bEnableExt)
    {
        int num_bits_left;
        int cnt = BitStreamGetBits(bs, 4);

        if (cnt == 15)
        {
            cnt += BitStreamGetBits(bs, 8);
        }

        num_bits_left = 8 * cnt;
        while (num_bits_left > 7)
        {
            BitStreamGetBits(bs, 8);
            num_bits_left -= 8;
        }

        BitStreamGetBits(bs, num_bits_left);
    }

    bits = CalcBitsUsed(bs, bytePtr, cachedBits);

    ps->bAvailblePsData = 1;

    return bits;
}

static void huff_data(BitStream *bs, const unsigned char dt, const unsigned char nr_par,
                      HuffPsTab t_huff, HuffPsTab f_huff, char *par)
{
    int n;

    if (dt)
    {
        /* coded in time direction */
        for (n = 0; n < nr_par; n++)
        {
            par[n] = ps_huff_dec(bs, t_huff);
        }
    } else {
        /* coded in frequency direction */
        par[0] = ps_huff_dec(bs, f_huff);

        for (n = 1; n < nr_par; n++)
        {
            par[n] = ps_huff_dec(bs, f_huff);
        }
    }
}

static INLINE char ps_huff_dec(BitStream *bs, HuffPsTab t_huff)
{
    int bit;
    int index = 0;

    while (index >= 0)
    {
        bit = (int)BitStreamGetBit(bs);
        index = (int)t_huff[index][bit];
    }

    return index + 31;
}

#endif
