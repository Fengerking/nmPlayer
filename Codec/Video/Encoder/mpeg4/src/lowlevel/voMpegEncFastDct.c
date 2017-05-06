/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/
#include "voMpegEncFastDct.h"
#define VODESCALE(x, n)  ( ((x) + (   1 << (   (n) - 1)  ) ) >> (n)  ) 
// Use C code to simulate ARMv6 SIMD instruction. Video quality will be reduced a little with ARMv6 version.
#define	FDCT_ARMv6_C_SIMULATION	

#ifdef FDCT_ARMv6_C_SIMULATION
#define CONST_4176	  ((int)4176)
#define CONST_4433	  ((int)4433)
#define CONST_4926	  ((int)4926)
#define CONST_6436	  ((int)6436)
#define CONST_7373	  ((int)7373)
#define CONST_9633	  ((int)9633)
#define CONST_10703	  ((int)10703)
#define CONST_20995	  ((int)20995)
#endif				  
					  
#define BUTTERFILY_OPERATE_STAGE_0(o,p,m,n) \
    sum[o] = ptr_block[m] + ptr_block[n];\
    sum[p] = ptr_block[m] - ptr_block[n];

#define BUTTERFILY_OPERATE_STAGE_1(o,p,m,n) \
    sum[o]= sum[m] + sum[n];\
    sum[p] = sum[m] - sum[n];

#define BUTTERFILY_OPERATE_STAGE_2_EVEN() \
    z[0] = sum[10] + sum[11];\
    z[1] = sum[10] - sum[11];\
    z[2] = sum[12] * CONST_4433     + sum[13] * CONST_10703;\
    z[3] = sum[12] * (-CONST_10703) + sum[13] * CONST_4433;

#define BUTTERFILY_OPERATE_STAGE_2_ODD()\
    z[5] = (sum[4] + sum[6]) *(-CONST_6436) + (sum[5] + sum[7]) * CONST_9633;\
    z[6] = (sum[4] + sum[6]) *  CONST_9633  + (sum[5] + sum[7]) * CONST_6436;\
    z[4] = sum[4] * (-CONST_4926)  + sum[7] * (-CONST_7373) +z[5];\
    z[1] = sum[4] * (-CONST_7373)  + sum[7] *   CONST_4926+ z[6];\
    z[3] = sum[5] * (-CONST_4176)  + sum[6] * (-CONST_20995)+ z[6];\
    z[2] = sum[5] * (-CONST_20995) + sum[6] *   CONST_4176+ z[5];
			
VO_VOID FastDct_C(VO_S16 *const block) 
{
    VO_S16 sum[16];
    VO_S32 z[7];
    VO_S16 *ptr_block;
    VO_S32 i;           
    // 1: processs rows, first. 
    ptr_block = block;
    for (i = 0; i < 8; i++) {
        BUTTERFILY_OPERATE_STAGE_0(0, 7, 0 ,7);
        BUTTERFILY_OPERATE_STAGE_0(1 ,6, 1 ,6);
        BUTTERFILY_OPERATE_STAGE_0(2 ,5, 2 ,5);
        BUTTERFILY_OPERATE_STAGE_0(3 ,4, 3 ,4);

        BUTTERFILY_OPERATE_STAGE_1(10,13,0,3);
        BUTTERFILY_OPERATE_STAGE_1(11,12,1,2);

        BUTTERFILY_OPERATE_STAGE_2_EVEN(); //0 2 4 6
        ptr_block[0] = (VO_S16 )z[0]<<2;
        ptr_block[4] = (VO_S16 )z[1]<<2;
        ptr_block[2] =	(VO_S16 )VODESCALE(z[2],11);
        ptr_block[6] =	(VO_S16 )VODESCALE(z[3],11);

        BUTTERFILY_OPERATE_STAGE_2_ODD();
        ptr_block[7] = (VO_S16 )VODESCALE(z[4], 11);
        ptr_block[5] = (VO_S16 )VODESCALE(z[3], 11);
        ptr_block[3] = (VO_S16 )VODESCALE(z[2], 11);
        ptr_block[1] = (VO_S16 )VODESCALE(z[1], 11);		
        //next row
        ptr_block += 8;
    }
    // 2. process columns,second
    ptr_block = block;
    for (i = 0; i < 8; i++) {
        BUTTERFILY_OPERATE_STAGE_0(0, 7, 0 ,56);
        BUTTERFILY_OPERATE_STAGE_0(1 ,6, 8 ,48);
        BUTTERFILY_OPERATE_STAGE_0(2 ,5, 16 ,40);
        BUTTERFILY_OPERATE_STAGE_0(3 ,4, 24 ,32);

        BUTTERFILY_OPERATE_STAGE_1(10,13,0,3);
        BUTTERFILY_OPERATE_STAGE_1(11,12,1,2);

        BUTTERFILY_OPERATE_STAGE_2_EVEN();
        ptr_block[0]  = (VO_S16 )VODESCALE(z[0], 5);
        ptr_block[32] = (VO_S16 )VODESCALE(z[1], 5);
        ptr_block[16] = (VO_S16 )VODESCALE(z[2], 18);
        ptr_block[48] = (VO_S16 )VODESCALE(z[3], 18);

        BUTTERFILY_OPERATE_STAGE_2_ODD();
        ptr_block[56] = (VO_S16 )VODESCALE(z[4], 18);
        ptr_block[40] = (VO_S16 )VODESCALE(z[3], 18);
        ptr_block[24] = (VO_S16 )VODESCALE(z[2], 18);
        ptr_block[8]  = (VO_S16 )VODESCALE(z[1],  18);
        //next column 
        ptr_block++;				
    }
}