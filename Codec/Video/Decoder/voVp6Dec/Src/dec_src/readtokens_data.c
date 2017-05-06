
#include "pbdll.h"
#include "readtokens_arith.h"
/****************************************************************************
*  Module constants
****************************************************************************/   

const TOKENEXTRABITS VP6_TokenExtraBits2[MAX_ENTROPY_TOKENS] =
{
    {  0,-1, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //ZERO_TOKEN
    {  1, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //ONE_TOKEN
    {  2, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //TWO_TOKEN
    {  3, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //THREE_TOKEN
    {  4, 0, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //FOUR_TOKEN
    {  5, 0, { 159,0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //DCT_VAL_CATEGORY1
    {  7, 1, { 145,165,0,  0,  0,  0,  0,  0,  0,  0,  0   } },   //DCT_VAL_CATEGORY2
    { 11, 2, { 140,148,173,0,  0,  0,  0,  0,  0,  0,  0   } },   //DCT_VAL_CATEGORY3
    { 19, 3, { 135,140,155,176,0,  0,  0,  0,  0,  0,  0   } },   //DCT_VAL_CATEGORY4
    { 35, 4, { 130,134,141,157,180,0,  0,  0,  0,  0,  0   } },   //DCT_VAL_CATEGORY5
    { 67,10, { 129,130,133,140,153,177,196,230,243,254,254 } },   //DCT_VAL_CATEGORY6
    {  0,-1, { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0   } },   // EOB TOKEN
};



const UINT32 VP6_HuffTokenMinVal[MAX_ENTROPY_TOKENS] = { 0,1, 2, 3, 4, 5, 7, 11, 19, 35, 67, 0};

// For Bitread functions
const UINT32 loMaskTbl_VP60[] = 
{   
    0x00000000,
    0x00000001, 0x00000003, 0x00000007, 0x0000000F,
    0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
	0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
	0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
	0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
	0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
	0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
	0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
};
