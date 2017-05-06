//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************


#if defined( MOTIONCOMP_COMPLETE)

#   define MotionCompMixed011 MotionCompMixed011Complete
#   define MotionCompMixed010 MotionCompMixed010Complete
#   define MotionCompMixed110 MotionCompMixed110Complete
#   define MotionCompMixed111 MotionCompMixed111Complete
#   define MotionCompMixed001 MotionCompMixed001Complete
#   define MotionCompMixed101 MotionCompMixed101Complete
#   define MotionCompMixed100 MotionCompMixed100Complete
#   define MotionCompMixed000 MotionCompMixed000Complete
#   define MotionCompMixedAlignBlock MotionCompMixedAlignBlockComplete
#   define MotionCompMixedHQHI MotionCompMixedHQHIComplete
#   define MotionCompWAddError10 MotionCompWAddError10Complete
#   define MotionCompWAddError01 MotionCompWAddError01Complete
#   define MotionCompWAddError00 MotionCompWAddError00Complete

#   define _FUNCTION_PROFILE_DECL_START(fp,id)
#   define _FUNCTION_PROFILE_STOP(fp)
#else
#   undef _FUNCTION_PROFILE_DECL_START
#   undef _FUNCTION_PROFILE_STOP
#   define _FUNCTION_PROFILE_DECL_START(fp,id) FUNCTION_PROFILE_DECL_START(fp,id);
#   define _FUNCTION_PROFILE_STOP(fp) FUNCTION_PROFILE_STOP(fp);

#endif



#if defined(WMV_OPT_MOTIONCOMP_ARM)
#   define ALIGNBLOCK_OUTPUT_TWIST
#   define _CE_ASM_OPT_
#endif


#ifdef _MIPS_ASM_MOTIONCOMP_OPT_
#   define _CE_ASM_OPT_
#   define ALIGNBLOCK_OUTPUT_TWIST
#   if defined( MOTIONCOMP_COMPLETE)&&defined(_CE_ASM_OPT_)&&defined(_MIPS64)&&defined(_MIPS64_OPTS_)
#       define ALIGNBLOCK_OUTPUT_TWIST_64BIT
#   endif
#endif

#if defined(SH3_ASM_MOTION_COMP) || defined(SH3_DSP_ASM_MOTION_COMP)
#   define _CE_ASM_OPT_
// the following is to mark tempoarary debugging adjustments only
#   define _SH3_ASM_AVOID_BUG_
#   ifdef   SH3_DSP
#       define pDspPhysicalYmem 0xA5017000
#   endif
#endif


#if defined(_SH4_ASM_MOTIONCOMP_OPT_) 
#   define ALIGNBLOCK_OUTPUT_TWIST
#   define _CE_ASM_OPT_
#endif


#ifdef MOTIONCOMP_COMPLETE
    DEBUG_ONLY(int  mc_complete_cnt;)
    DEBUG_ONLY(int mc_00cnt;)
    DEBUG_ONLY(int mc_complete_00cnt; )
    DEBUG_ONLY(int mc_10cnt;)
    DEBUG_ONLY(int mc_complete_10cnt; )
    DEBUG_ONLY(int mc_01cnt;)
    DEBUG_ONLY(int mc_complete_01cnt; )
    DEBUG_ONLY(int mc_000cnt;)
    DEBUG_ONLY(int mc_complete_000cnt; )
    DEBUG_ONLY(int mc_001cnt;)
    DEBUG_ONLY(int mc_complete_001cnt; )
    DEBUG_ONLY(int mc_complete_011cnt; )
    DEBUG_ONLY(int mc_complete_010cnt; )
    DEBUG_ONLY(int mc_complete_110cnt; )
    DEBUG_ONLY(int mc_complete_MCMHQHIcnt;)
    DEBUG_ONLY(int mc_MCMHQHIcnt;)
    DEBUG_ONLY(int mc_complete_MCMABcnt;)
    DEBUG_ONLY(int mc_MCMABcnt;)
    
#endif

//This shortcut makes the complete version of the motioncomp routines more compact, thus saving cache 
//flushing activities, at a cost of more complex code. Feel free to experiment on different platforms 
//to see how it affects the performance.

#if defined( MOTIONCOMP_COMPLETE) && !defined(_SH3_) // somehow SH3 causes linking errors. Other platforms are  free 
#define MOTIONCOMP_COMPLETE_SHORTCUT               //to switch  it on or off.
#endif

#ifdef MOTIONCOMP_COMPLETE_SHORTCUT


Void_WMV MCWAE_xx(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, 
                  I32_WMV iWidthPrev, I32_WMV *pErrorBuf, I32_WMV select)
{

    U32_WMV err_overflow;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    //I32_WMV iWidthPrev2 = iWidthPrev;
 
    I32_WMV * pErrorBuf2 = pErrorBuf;

    I32_WMV t_even_1 =0,ta1 =0,t_odd_1=0,t_even_2=0,t_odd_2=0,ta2=0,u0=0,u1=0;

    U32_WMV err_odd , err_even;
    U32_WMV rndCtrl ;
    const U8_WMV* pLine  =  ppxlcRefMB;
    I32_WMV iz, iy;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iWidthPrevRef;

#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
     rndCtrl = (iWidthPrev>>16)&1;
    if(select == 0x00) rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
     rndCtrl = (iWidthPrev>>16)&1;
    if(select == 0x00) rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif




    for( iz = 0; iz < 2 ; iz ++ )
    {
    

        if(select != 0x10)
        {
            t_even_2 = pLine[0] | pLine[2] << 16;
            t_odd_2 = pLine[1] | pLine[3] << 16;
            if(select ==0x00) ta2 = pLine[2] | pLine[4] << 16;

            pLine   +=  iWidthPrevRef;
        }

        for ( iy  =  0; iy < 8; iy ++ ) 
        {


            U32_WMV y0;
            U32_WMV * ppxlcpredU32;
                        
            if(select ==0x10 )
            {
                t_even_1 = pLine[0] | pLine[2] << 16;
                t_even_2 = pLine[1] | pLine[3] << 16;
                t_odd_1 = t_even_2;
                t_odd_2 = pLine[2] | pLine[4] << 16;

            }
            else
            {
                t_even_1 = t_even_2;
                t_odd_1 = t_odd_2;
                t_even_2 = pLine[0] | pLine[2] << 16;
                t_odd_2 = pLine[1] | pLine[3] << 16;
            }

            if(select !=0x00)
            {
                u0 = (t_even_1 + t_even_2 + rndCtrl);
                u1 = ((t_odd_1 + t_odd_2 + rndCtrl));
                u0 = u0>>1;
                u1   >>= 1;
            }
            else
            {

                ta1 = ta2;
                ta2 = pLine[2] | pLine[4] << 16;
                u0 = (t_even_1 + t_even_2 + t_odd_1 + t_odd_2 + rndCtrl);
                u1 = (t_odd_1 + t_odd_2 + ta1 + ta2 + rndCtrl);
                 u0 = u0>>2;
                u1   >>= 2;

            }

           
             ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;


            u0  &= mask;
            u1  &= mask;


            err_overflow = 0;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
                if(err_overflow & 0xff00ff00)
                {
                    test_err_cliping2(&u0,&u1,1);

                }

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);
        
            *ppxlcpredU32 = y0;
                        
                    
            ppxlcPredMB   +=  iWidthPrev;

            pLine   +=  iWidthPrevRef;

        }

       pErrorBuf=pErrorBuf2+1;
       
       pLine  =  ppxlcRefMB+4;
       ppxlcPredMB=ppxlcPredMB2+4;

    }


}

Void_WMV MCM_HQHI_001_000(U8_WMV* ppxlcPredMB,  const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev,
                  I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock, I32_WMV select)
{

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    I32_WMV iWidthPrevRef;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV * pErrorBuf = ppxliErrorBuf;
    U8_WMV* ppxlcPred = ppxlcPredMB;
    U32_WMV err_odd , err_even;
    U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock );
    I32_WMV iz, iy;
    
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

    for( iz = 0;iz<2;iz++)
    {
//      U8_WMV* ppxlcPred = ppxlcPredMB + ix;
        
        I32_WMV t_even_1 =0,t_even_2 =0,t_even_3 =0,t_even_4 =0;
        I32_WMV t_odd_1 =0,t_odd_2 =0,t_odd_3 =0,t_odd_4 =0;
        I32_WMV at_even_1 =0,at_even_2 =0,at_even_3=0,at_even_4 =0;
        I32_WMV at_odd_1 =0 ,at_odd_2 =0 ,at_odd_3 =0 ,at_odd_4 =0 ;
        I32_WMV u0 =0,u1 =0,u2 =0,u3 =0;           


        const U8_WMV* pLine = ppxlcRefMB;
        
        if(select != 0x000)
        {
                pLine = (ppxlcRefMB - iWidthPrevRef);

                at_even_2 = pLine[0] | pLine[2] << 16;
                at_odd_2  = pLine[1] | pLine[3] << 16;

                pLine   +=  iWidthPrevRef; 

                at_even_3 = pLine[0] | pLine[2] << 16;
                at_odd_3  = pLine[1] | pLine[3] << 16;

                pLine   +=  iWidthPrevRef; 

                at_even_4 = pLine[0] | pLine[2] << 16;
                at_odd_4  = pLine[1] | pLine[3] << 16;

                pLine   +=  iWidthPrevRef; 

        }

#ifndef ALIGNBLOCK_OUTPUT_TWIST

//      U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock + ix);

        if(select != 0x001)
        {

            t_even_2 = pBlockU32[0] & mask;
            t_odd_2 = (pBlockU32[0]>>8) & mask;
            t_even_3 = pBlockU32[2] & mask;
            t_odd_3 = (pBlockU32[2]>>8) & mask;
            t_even_4 = pBlockU32[4] & mask;
            t_odd_4 = (pBlockU32[4]>>8) & mask;
        }
#endif

        for ( iy  =  0; iy < 8; iy ++ ) 
        {

            
            U32_WMV y0;
            U32_WMV * ppxlcPredU32 = (U32_WMV *)(ppxlcPred);



            overflow = 0;               

            if(select != 0x000)
            {
                at_even_1 = at_even_2;
                at_odd_1  = at_odd_2 ;
                at_even_2 = at_even_3;
                at_odd_2  = at_odd_3 ;
                at_even_3 = at_even_4;
                at_odd_3  = at_odd_4 ;
                at_even_4 = pLine[0] | pLine[2] << 16;
                at_odd_4  = pLine[1] | pLine[3] << 16;



                u2 = ((at_even_2 + at_even_3)*9-(at_even_1 + at_even_4) + 0x00080008);
                overflow |= u2; 
                u2 = u2>>4;
                u3 = ((at_odd_2  + at_odd_3 )*9-(at_odd_1  + at_odd_4 ) + 0x00080008);
                overflow |= u3;
                u3   >>= 4;

                if(select == 0x001)
                {
                    u0=u2;
                    u1=u3;
                    if(overflow & 0xf000f000)
                    {

                        test_err_cliping2(&u0,&u1,0);
 
                    }
                    goto __001_0;

                }

            }

#ifndef ALIGNBLOCK_OUTPUT_TWIST

            t_even_1 = t_even_2;
            t_odd_1 = t_odd_2;
            t_even_2 = t_even_3;
            t_odd_2 = t_odd_3;
            t_even_3 = t_even_4;
            t_odd_3 = t_odd_4;
            t_even_4 = pBlockU32[6] & mask;
            t_odd_4 = (pBlockU32[6]>>8) & mask;

#else

#   ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT

            t_even_1 = pBlockU32[0];
            t_odd_1 =  pBlockU32[0+22];
            t_even_2 = pBlockU32[2];
            t_odd_2 =  pBlockU32[2+22];
            t_even_3 = pBlockU32[4];
            t_odd_3 =  pBlockU32[4+22];
            t_even_4 = pBlockU32[6];
            t_odd_4 =  pBlockU32[6+22];


#   else
            t_even_1 = pBlockU32[0];
            t_odd_1 =  pBlockU32[0+11];
            t_even_2 = pBlockU32[1];
            t_odd_2 =  pBlockU32[1+11];
            t_even_3 = pBlockU32[2];
            t_odd_3 =  pBlockU32[2+11];
            t_even_4 = pBlockU32[3];
            t_odd_4 =  pBlockU32[3+11];
#   endif

#endif

            u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
            overflow |= u0; 
            u0 = u0>>4;
            u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
            overflow |= u1;
            u1   >>= 4;


            if(select ==0x000)
            {
                if(overflow & 0xf000f000)
                    test_err_cliping2(&u0,&u1,0);
            }
            else
            {

                if(overflow & 0xf000f000)
                {
                
                    test_err_cliping4(&u0,&u1,&u2,&u3,0);
 
                }


                u0 = ((u0 & mask) + u2 + 0x00010001)>>1;

                u1 = ((u1 & mask) + u3 + 0x00010001)>>1;

            }

__001_0:

            u0  &= mask;
            u1  &= mask;
        

             err_overflow = 0;


            if(ppxliErrorBuf != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
        


                if(err_overflow & 0xff00ff00)
                {
                    test_err_cliping2(&u0,&u1,1);

                }   


                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);

            *ppxlcPredU32 = y0;

    

            ppxlcPred   +=  iWidthPrev;



#ifndef ALIGNBLOCK_OUTPUT_TWIST
            pBlockU32   +=  2;
#else

#   ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
            pBlockU32   +=  2;
#   else
            pBlockU32   +=  1;
#   endif
#endif


            pLine   +=  iWidthPrevRef; 

        }    

 
#ifdef ALIGNBLOCK_OUTPUT_TWIST

#   ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
        pBlockU32   -=  15;
#   else
        pBlockU32   +=  (3+11);
#   endif

#else

        pBlockU32 -=15;

#endif

        pErrorBuf = ppxliErrorBuf + 1;
        ppxlcPred = ppxlcPredMB + 4;
        ppxlcRefMB= ppxlcRefMB+4;
    }

}



Void_WMV MCM010_110_AB(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf, I32_WMV select)
{


    U32_WMV mask = 0x00ff00ff;
    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    I32_WMV iWidthPrevRef;
    U32_WMV err_odd , err_even;
    I32_WMV iy, iz;
    I32_WMV num_loop;

    U8_WMV*   ppxlcPredMB2;
    const U8_WMV*  ppxlcRefMB2;
    I32_WMV * pErrorBuf2;


#ifdef DYNAMIC_EDGEPAD

    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;

#else

    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    
    if(select==0xAB)
    {
       ppxlcRefMB  =  ppxlcRefMB - iWidthPrevRef;
       num_loop=11;

    }
    else
       num_loop=8;

    ppxlcPredMB2=ppxlcPredMB;
    ppxlcRefMB2=ppxlcRefMB;
    pErrorBuf2=pErrorBuf;
    
    for(iz=0; iz<2; iz++)
    {

     for (iy  =  0; iy < num_loop; iy ++ ) 
     {


        U32_WMV y0;
        I32_WMV t0,t1,t2,t3,t4,u0,u1;
        U32_WMV * ppxlcpredU32;
                    


        t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
        t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
        t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
        t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
        t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);




                
        overflow = 0;


        u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
        overflow |= u0; 
        u0 = u0>>4;
 
        u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
        overflow |= u1;
        u1   >>= 4;
  

        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;
    
        if(overflow & 0xf000f000)
        {
            test_err_cliping2(&u0,&u1,0);

        }

        if(select == 0x110)
        {
            u0 = (u0 + t2 + 0x00010001)>>1;
            u1 = (u1 + t3 + 0x00010001)>>1;
        }

        u0  &= mask;
        u1  &= mask;

        if(select!=0xAB)
        {
            err_overflow = 0;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
        
            
                if(err_overflow & 0xff00ff00)
                {
                    test_err_cliping2(&u0,&u1,1);

                }

                pErrorBuf  += 4;

            }
        

            y0 = (u0) | ((u1) << 8);

            *ppxlcpredU32 = y0;

            ppxlcPredMB    +=  iWidthPrev;
        }
        else
        {
            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;
#ifdef ALIGNBLOCK_OUTPUT_TWIST

#ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
        *ppxlcpredU32=(u0 & mask);
//      *(ppxlcpredU32+1)=(u2 & mask);
        *(ppxlcpredU32+22)=(u1 & mask);    
//        *(ppxlcpredU32+22+1)=(u3 & mask);
        ppxlcPredMB   +=  8;

#else
        *ppxlcpredU32=(u0 & mask);
        *(ppxlcpredU32+11)=(u1 & mask);
//        *(ppxlcpredU32+22)=(u2 & mask);
//        *(ppxlcpredU32+33)=(u3 & mask);
        ppxlcPredMB   +=  4;
#endif

#else


         *ppxlcpredU32 = (u0 & mask) | ((u1 & mask) << 8);
//         *(ppxlcpredU32 + 1) = (u2 & mask) | ((u3 & mask) << 8);

         ppxlcPredMB   +=  8;
#endif

        }
        ppxlcRefMB   +=  iWidthPrevRef;
     }

     pErrorBuf=pErrorBuf2+1;
     ppxlcRefMB=ppxlcRefMB2+4;

     ppxlcPredMB=ppxlcPredMB2+4;

#if defined( ALIGNBLOCK_OUTPUT_TWIST) && !defined (ALIGNBLOCK_OUTPUT_TWIST_64BIT)
    if(select==0xAB) ppxlcPredMB=ppxlcPredMB2+88;
#endif

   }

}

Void_WMV MotionCompMixed010(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf )
{

        MCM010_110_AB(ppxlcPredMB, ppxlcRefMB, iWidthPrev, pErrorBuf, 0x010);

}

Void_WMV MotionCompMixed110(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
        MCM010_110_AB(ppxlcPredMB, ppxlcRefMB, iWidthPrev, pErrorBuf, 0x110);
    
}

Void_WMV MotionCompMixed001(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{

     MCM_HQHI_001_000(ppxlcPredMB, ppxlcRefMB, iWidthPrev,
               pErrorBuf, NULL_WMV, 0x001);

}

Void_WMV MotionCompMixedAlignBlock(U8_WMV*   pBlock, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev)
{
   MCM010_110_AB(pBlock, ppxlcRefMB, iWidthPrev, NULL_WMV, 0xAB);
    return;
}

Void_WMV MotionCompMixedHQHI(U8_WMV* ppxlcPredMB,  const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock)
{
    MCM_HQHI_001_000(ppxlcPredMB, ppxlcRefMB, iWidthPrev,
               ppxliErrorBuf, alignedBlock, 0xffff);

}

Void_WMV MotionCompMixed000(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{

#ifdef ALIGNBLOCK_OUTPUT_TWIST
    U8_WMV tempBlock[11 * 16  +  15];
#else   
    U8_WMV tempBlock[11 * 8  +  15];
#endif
    U8_WMV *alignedBlock  =  (U8_WMV*) (((int)tempBlock  +  15)  &  ~15);

    iWidthPrev  &= ~0x10000;

    MotionCompMixedAlignBlock(alignedBlock, ppxlcRefMB, iWidthPrev);

    MCM_HQHI_001_000(ppxlcPredMB, ppxlcRefMB, iWidthPrev,
               pErrorBuf, alignedBlock, 0x0);

 
}

Void_WMV MotionCompWAddError10(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{

        MCWAE_xx(ppxlcPredMB, ppxlcRefMB, iWidthPrev, pErrorBuf, 0x10);
    return;

}

Void_WMV MotionCompWAddError01(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{
        MCWAE_xx(ppxlcPredMB, ppxlcRefMB, iWidthPrev, pErrorBuf, 0x01);
    return;

}

Void_WMV MotionCompWAddError00(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{
    
    MCWAE_xx(ppxlcPredMB, ppxlcRefMB, iWidthPrev, pErrorBuf, 0x00);
    return;
}

#endif //#ifdef MOTIONCOMP_COMPLETE_SHORTCUT

//To compare these functions to the desktop version(see decodePMB in vopdec.cpp), notice the index appended to a function is 3-bit binary
// index = iMixedPelMV << 2 | bInterpolateY << 1 | bInterpolateX

#if defined(MOTIONCOMP_COMPLETE) || !defined(_CE_ASM_OPT_)

Void_WMV MotionCompMixed011(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
    I32_WMV iWidthPrevRef;
    I32_WMV iy;

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE);
#   endif

    

#ifdef DYNAMIC_EDGEPAD

    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;

#else

    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif


    if(pErrorBuf == NULL)
    {
        U32_WMV ref_offset2;
        
        U32_WMV ref_offset=(((U32_WMV)ppxlcRefMB)&0x3)<<3;
        U32_WMV ppxlcRef=(U32_WMV)ppxlcRefMB;
        ppxlcRef=ppxlcRef-(((U32_WMV)ppxlcRef)&0x3);
        if(ref_offset==0)
        {
            for (iy  =  0; iy < 4; iy ++ ) 
            {

//            memcpy(ppxlcPredMB,ppxlcRefMB,8);
                U32_WMV data0=*(U32_WMV *)ppxlcRef;
                U32_WMV data1=*(U32_WMV *)(ppxlcRef+4);
            

                *(U32_WMV *)ppxlcPredMB=(data0);
                *(U32_WMV *)(ppxlcPredMB+4)=(data1);

                ppxlcRef  +=  iWidthPrevRef;
                ppxlcPredMB  +=  iWidthPrev;

                data0=*(U32_WMV *)ppxlcRef;
                data1=*(U32_WMV *)(ppxlcRef+4);
            

                *(U32_WMV *)ppxlcPredMB=(data0);
                *(U32_WMV *)(ppxlcPredMB+4)=(data1);

                ppxlcRef  +=  iWidthPrevRef;
                ppxlcPredMB  +=  iWidthPrev;
            }
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
            return;
        }

        ref_offset2=32-ref_offset;
        

        ppxlcRef=ppxlcRef-(((U32_WMV)ppxlcRef)&0x3);
        
        for (iy  =  0; iy < 8; iy ++ ) 
        {

//            memcpy(ppxlcPredMB,ppxlcRefMB,8);
            U32_WMV data0=*(U32_WMV *)ppxlcRef;
            U32_WMV data1=*(U32_WMV *)(ppxlcRef+4);
            U32_WMV data2=*(U32_WMV *)(ppxlcRef+8);

            *(U32_WMV *)ppxlcPredMB=(data0>>ref_offset)|(data1<<ref_offset2);
            *(U32_WMV *)(ppxlcPredMB+4)=(data1>>ref_offset)|(data2<<ref_offset2);

            ppxlcRef  +=  iWidthPrevRef;
            ppxlcPredMB  +=  iWidthPrev;
        }
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
        return;


    }
    else
    {
        U32_WMV t0,t1,t2,t3,err_even,err_odd , y0,y1;

#ifndef MOTIONCOMP_COMPLETE
        U32_WMV err_overflow = 0;
        U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
        const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
        I32_WMV * pErrorBuf2 = pErrorBuf;
#else
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_001cnt++; )
#endif

        for ( iy  =  0; iy < 8; iy ++ ) 
        {
            U32_WMV * ppxlcpredU32;
#ifdef MOTIONCOMP_COMPLETE
            U32_WMV err_overflow = 0;
#endif

            t0 = ppxlcRefMB[0] | (ppxlcRefMB[2] << 16);         
            t1 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
            t2 = ppxlcRefMB[4] | (ppxlcRefMB[4 + 2] << 16);
            t3 = ppxlcRefMB[5] | (ppxlcRefMB[5 + 2] << 16);

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            t0 = t0 + err_even-((err_even & 0x8000) << 1);
            err_overflow  |= t0;
            t1 = t1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow  |= t1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            t2 = t2 + err_even-((err_even & 0x8000) << 1);
            err_overflow  |= t2;
            t3 = t3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow  |= t3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4((I32_WMV *)&t0,(I32_WMV *)&t1,(I32_WMV *)&t2,(I32_WMV *)&t3,1);

            }
#endif

            pErrorBuf += 4;

            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

            y0 = (t0) | ((t1) << 8);
            y1 = (t2) | ((t3) << 8);

            *ppxlcpredU32 = y0;
            *(ppxlcpredU32 + 1) = y1;

           

            ppxlcRefMB  +=  iWidthPrevRef;
            ppxlcPredMB  +=  iWidthPrev;
        }

#ifndef MOTIONCOMP_COMPLETE
         if(err_overflow & 0xff00ff00)
         {

#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif

             MotionCompMixed011Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
         }            
#endif

    }

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif

}

#endif

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))

Void_WMV MotionCompMixed010(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf )
{
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    
        U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
        const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
        I32_WMV iWidthPrev2 = iWidthPrev;
        I32_WMV * pErrorBuf2 = pErrorBuf;


#endif

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iWidthPrevRef;
    I32_WMV iy;
    U32_WMV err_odd , err_even;
    U32_WMV * ppxlcpredU32;

#ifdef MOTIONCOMP_COMPLETE
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_010cnt++; )
#endif


#ifdef DYNAMIC_EDGEPAD

    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;

#else

    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    

    
     for (iy  =  0; iy < 8; iy ++ ) 
    {

        U32_WMV y0,y1;
        I32_WMV t0,t1,t2,t3,t4,t5,t6,t7,t8,u0,u1,u2,u3;
        

        t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
        t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
        t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
        t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
        t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);
        t5 = ppxlcRefMB[4] | (ppxlcRefMB[4 + 2] << 16);
        t6 = ppxlcRefMB[5] | (ppxlcRefMB[5 + 2] << 16);
        t7 = ppxlcRefMB[6] | (ppxlcRefMB[6 + 2] << 16);
        t8 = ppxlcRefMB[7] | (ppxlcRefMB[7 + 2] << 16);
#ifdef MOTIONCOMP_COMPLETE
        overflow = 0;
#endif
        u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
        overflow  |= u0; 
        u0 = u0>>4;
        u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
        overflow  |= u1;
        u1   >>= 4;
        

        u2 = ((t5 + t6)*9-(t4 + t7) + 0x00080008);
        overflow  |= u2; 
        u2   >>= 4;
        u3 = ((t6 + t7)*9-(t5 + t8) + 0x00080008);
        overflow  |= u3;
        u3   >>= 4;
        

        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

#ifdef MOTIONCOMP_COMPLETE
        if(overflow & 0xf000f000)
        {
            test_err_cliping4(&u0,&u1,&u2,&u3,0);
        }
#endif

        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE
        err_overflow = 0;
#endif
        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow  |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow  |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow  |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow  |= u3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
            
#endif
            pErrorBuf += 4;




        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;



        ppxlcPredMB   +=  iWidthPrev;
        ppxlcRefMB   +=  iWidthPrevRef;
    } 
#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
        MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev2, pErrorBuf2);
    }            
#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif

}
#else //#if defined( PLATFORM_32REGS0||defined(MOTIONCOMP_COMPLETE)

#if !defined(_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)

Void_WMV MotionCompMixed010(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf )
{

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
    I32_WMV * pErrorBuf2 = pErrorBuf;

    U32_WMV mask = 0x00ff00ff;
    U32_WMV err_odd , err_even;
    I32_WMV iz, iy;
    I32_WMV iWidthPrevRef;
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE);
#   endif

#ifdef DYNAMIC_EDGEPAD

    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;

#else

    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    


    
    for( iz=0;iz<2;iz++)
    {

         for ( iy  =  0; iy < 8; iy ++ ) 
        {
         

            U32_WMV y0;
            register I32_WMV t0,t1,t2,t3,t4,u0,u1;
            

            t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
            t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
            t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
            t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
            t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);

            u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
            overflow  |= u0; 
            u0 = u0>>4;
            u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
            overflow  |= u1;
            u1   >>= 4;

            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow  |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow  |= u1;

                pErrorBuf += 4;

            }

            y0 = (u0) | ((u1) << 8);


            * (U32_WMV *) ppxlcPredMB= y0;




            ppxlcPredMB   +=  iWidthPrev;
            ppxlcRefMB   +=  iWidthPrevRef;
        } 

        pErrorBuf=pErrorBuf2+1;
        ppxlcRefMB=ppxlcRefMB2+4;
        ppxlcPredMB=ppxlcPredMB2+4;

    }

    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {

#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif

        MotionCompMixed010Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
    }            
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#endif //#if !defined(_CE_ASM_OPT_)

#endif //#if defined( PLATFORM_32REGS0||defined(MOTIONCOMP_COMPLETE)

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))
Void_WMV MotionCompMixed110(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
    I32_WMV iWidthPrev2 = iWidthPrev;
    I32_WMV * pErrorBuf2 = pErrorBuf;


#endif

    U32_WMV mask = 0x00ff00ff;
    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    I32_WMV iWidthPrevRef;
    U32_WMV err_odd , err_even;
    I32_WMV iy;

#ifdef MOTIONCOMP_COMPLETE
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY( mc_complete_110cnt ++; )
#endif


#ifdef DYNAMIC_EDGEPAD

    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;

#else

    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    


    

     for (iy  =  0; iy < 8; iy ++ ) 
     {


        U32_WMV y0,y1;
        I32_WMV t0,t1,t2,t3,t4,t5,t6,t7,t8,u0,u1,u2,u3;
        U32_WMV * ppxlcpredU32;
                    


        t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
        t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
        t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
        t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
        t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);
        t5 = ppxlcRefMB[4] | (ppxlcRefMB[4 + 2] << 16);
        t6 = ppxlcRefMB[5] | (ppxlcRefMB[5 + 2] << 16);
        t7 = ppxlcRefMB[6] | (ppxlcRefMB[6 + 2] << 16);
        t8 = ppxlcRefMB[7] | (ppxlcRefMB[7 + 2] << 16);



#ifdef MOTIONCOMP_COMPLETE                  
        overflow = 0;
#endif

        u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
        overflow |= u0; 
        u0 = u0>>4;
 
        u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
        overflow |= u1;
        u1   >>= 4;
  

        u2 = ((t5 + t6)*9-(t4 + t7) + 0x00080008);
        overflow |= u2; 
        u2   >>= 4;
  
        u3 = ((t6 + t7)*9-(t5 + t8) + 0x00080008);
        overflow |= u3;
        u3   >>= 4;
  

        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

#ifdef MOTIONCOMP_COMPLETE      
        if(overflow & 0xf000f000)
        {
            test_err_cliping4(&u0,&u1,&u2,&u3,0);

        }
#endif
        u0 = (u0 + t2 + 0x00010001)>>1;
        u1 = (u1 + t3 + 0x00010001)>>1;
        u2 = (u2 + t6 + 0x00010001)>>1;
        u3 = (u3 + t7 + 0x00010001)>>1;

        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE      
        err_overflow = 0;
#endif
        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE      
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif
            pErrorBuf  += 4;

        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;

        ppxlcPredMB    +=  iWidthPrev;
        ppxlcRefMB   +=  iWidthPrevRef;
     }
#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
        MotionCompMixed110Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev2, pErrorBuf2);
    }            
#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#elif !defined (_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)
//#else

Void_WMV MotionCompMixed110(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
    I32_WMV * pErrorBuf2 = pErrorBuf;
    U32_WMV err_odd , err_even;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iWidthPrevRef;
    I32_WMV iz, iy;

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif

#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    

    

    for(iz = 0; iz < 2 ; iz ++ )
    {

         for (iy  =  0; iy < 8; iy ++ ) 
         {


            U32_WMV y0;
            I32_WMV t0,t1,t2,t3,t4,u0,u1;
            U32_WMV * ppxlcpredU32;

            t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
            t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
            t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
            t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
            t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);



            u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
            overflow |= u0; 
            u0 = u0>>4;
 
            u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
            overflow |= u1;
            u1   >>= 4;
  

            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

            u0 = (u0 + t2 + 0x00010001)>>1;
            u1 = (u1 + t3 + 0x00010001)>>1;

            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);

            *ppxlcpredU32 = y0;
 

            ppxlcPredMB    +=  iWidthPrev;
            ppxlcRefMB   +=  iWidthPrevRef;
         }

        pErrorBuf=pErrorBuf2+1;
        ppxlcRefMB=ppxlcRefMB2+4;
        ppxlcPredMB=ppxlcPredMB2+4;

    }

  

    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixed110Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
    }            

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#endif

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||defined(MOTIONCOMP_COMPLETE)
Void_WMV MotionCompMixed111(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif


#ifndef MOTIONCOMP_COMPLETE

    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
    I32_WMV iWidthPrev2 = iWidthPrev;
    I32_WMV * pErrorBuf2 = pErrorBuf;
#endif
    U32_WMV err_odd , err_even;
    U32_WMV mask = 0x00ff00ff;
    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    I32_WMV iWidthPrevRef;
    U32_WMV * ppxlcpredU32;
    I32_WMV iy ;
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

#ifdef MOTIONCOMP_COMPLETE
        DEBUG_ONLY(mc_complete_cnt++);
#endif
    

     for (iy  =  0; iy < 8; iy ++ ) 
     {


        U32_WMV y0,y1;
        I32_WMV t0,t1,t2,t3,t4,t5,t6,t7,t8,u0,u1,u2,u3;
                    


        t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
        t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
        t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
        t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
        t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);
        t5 = ppxlcRefMB[4] | (ppxlcRefMB[4 + 2] << 16);
        t6 = ppxlcRefMB[5] | (ppxlcRefMB[5 + 2] << 16);
        t7 = ppxlcRefMB[6] | (ppxlcRefMB[6 + 2] << 16);
        t8 = ppxlcRefMB[7] | (ppxlcRefMB[7 + 2] << 16);



#ifdef MOTIONCOMP_COMPLETE                  
        overflow = 0;
#endif

        u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
        overflow |= u0; 
        u0 = u0>>4;
 
        u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
        overflow |= u1;
        u1   >>= 4;
  

        u2 = ((t5 + t6)*9-(t4 + t7) + 0x00080008);
        overflow |= u2; 
        u2   >>= 4;
  
        u3 = ((t6 + t7)*9-(t5 + t8) + 0x00080008);
        overflow |= u3;
        u3   >>= 4;
  

        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

#ifdef MOTIONCOMP_COMPLETE      
        if(overflow & 0xf000f000)
        {
            test_err_cliping4(&u0,&u1,&u2,&u3,0);
        }
#endif
        u0 = (u0 + t1 + 0x00010001)>>1;
        u1 = (u1 + t2 + 0x00010001)>>1;
        u2 = (u2 + t5 + 0x00010001)>>1;
        u3 = (u3 + t6 + 0x00010001)>>1;

        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE      
        err_overflow = 0;
#endif
 
        if(pErrorBuf != NULL)
        {
 
            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE      
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif
            pErrorBuf  += 4;

        }


        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;

        ppxlcPredMB    +=  iWidthPrev;
        ppxlcRefMB   +=  iWidthPrevRef;
     }

#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
        MotionCompMixed111Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev2, pErrorBuf2);
    }            
#endif

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}
#else

Void_WMV MotionCompMixed111(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV*  ppxlcRefMB2 = ppxlcRefMB; 
    I32_WMV * pErrorBuf2 = pErrorBuf;
    U32_WMV err_odd , err_even;

    I32_WMV iz, iy;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iWidthPrevRef;

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif


#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    

    
    for(iz = 0; iz < 2 ; iz ++ )
    {

         for (iy  =  0; iy < 8; iy ++ ) 
         {


            U32_WMV y0;
            I32_WMV t0,t1,t2,t3,t4,u0,u1;
            U32_WMV * ppxlcpredU32;
                        


            t0 = ppxlcRefMB[-1] | (ppxlcRefMB[-1 + 2] << 16);
            t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
            t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
            t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
            t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);

            u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
            overflow |= u0; 
            u0 = u0>>4;
 
            u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
            overflow |= u1;
            u1   >>= 4;
  

            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;


            u0 = (u0 + t1 + 0x00010001)>>1;
            u1 = (u1 + t2 + 0x00010001)>>1;

            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 
                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
        
                pErrorBuf  += 4;
            }


            y0 = (u0) | ((u1) << 8);


            *ppxlcpredU32 = y0;


            ppxlcPredMB    +=  iWidthPrev;
            ppxlcRefMB   +=  iWidthPrevRef;
         }

       pErrorBuf=pErrorBuf2+1;
       ppxlcRefMB=ppxlcRefMB2+4;
       ppxlcPredMB=ppxlcPredMB2+4;

    }

    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixed111Complete( ppxlcPredMB2, ppxlcRefMB2, iWidthPrev, pErrorBuf2);
    }            

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}


#endif

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))
Void_WMV MotionCompMixed001(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV * pErrorBuf2 = pErrorBuf;
#endif

    I32_WMV t_even_1,t_even_2,t_even_3,t_even_4,t_odd_1,t_odd_2,t_odd_3,t_odd_4,t_even_5,t_even_6,t_even_7,t_even_8,t_odd_5,t_odd_6,t_odd_7,t_odd_8,u0,u1,u2,u3;
    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U32_WMV err_odd , err_even;
    I32_WMV iWidthPrevRef;
    const U8_WMV* pLine;
    U32_WMV mask = 0x00ff00ff;
    U32_WMV * ppxlcpredU32;
    I32_WMV iy;


#ifdef MOTIONCOMP_COMPLETE

        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_001cnt++);


#endif

#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    

    pLine  =  ppxlcRefMB - iWidthPrevRef;

    t_even_2 = pLine[0] | pLine[2] << 16;
    t_odd_2 = pLine[1] | pLine[3] << 16;
    t_even_6 = pLine[4] | pLine[6] << 16;
    t_odd_6 = pLine[5] | pLine[7] << 16;

    pLine   +=  iWidthPrevRef;

    t_even_3 = pLine[0] | pLine[2] << 16;
    t_odd_3 = pLine[1] | pLine[3] << 16;
    t_even_7 = pLine[4] | pLine[6] << 16;
    t_odd_7 = pLine[5] | pLine[7] << 16;

    pLine   +=  iWidthPrevRef;

    t_even_4 = pLine[0] | pLine[2] << 16;
    t_odd_4 = pLine[1] | pLine[3] << 16;
    t_even_8 = pLine[4] | pLine[6] << 16;
    t_odd_8 = pLine[5] | pLine[7] << 16;

    pLine   +=  iWidthPrevRef;

      
    

    for (iy  =  0; iy < 8; iy ++ ) 
    {


        U32_WMV y0,y1;
                    
        t_even_1 = t_even_2;
        t_odd_1 = t_odd_2;
        t_even_2 = t_even_3;
        t_odd_2 = t_odd_3;
        t_even_3 = t_even_4;
        t_odd_3 = t_odd_4;
        t_even_4 = pLine[0] | pLine[2] << 16;
        t_odd_4 = pLine[1] | pLine[3] << 16;


#ifdef MOTIONCOMP_COMPLETE
        overflow = 0;
#endif

        u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
        overflow |= u0; 
        u0 = u0>>4;
        u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
        overflow |= u1;
        u1   >>= 4;
  

        t_even_5 = t_even_6;
        t_odd_5 = t_odd_6;
        t_even_6 = t_even_7;
        t_odd_6 = t_odd_7;
        t_even_7 = t_even_8;
        t_odd_7 = t_odd_8;
        t_even_8 = pLine[4] | pLine[6] << 16;
        t_odd_8 = pLine[5] | pLine[7] << 16;




        u2 = ((t_even_6 + t_even_7)*9-(t_even_5 + t_even_8) + 0x00080008);
        overflow |= u2; 
        u2 = u2>>4;
        u3 = ((t_odd_6 + t_odd_7)*9-(t_odd_5 + t_odd_8) + 0x00080008);
        overflow |= u3;
        u3   >>= 4;
   
        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

#ifdef MOTIONCOMP_COMPLETE
        if(overflow & 0xf000f000)
        {
            test_err_cliping4(&u0,&u1,&u2,&u3,0);
        }
#endif

        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE
        err_overflow = 0;
#endif

        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif

            pErrorBuf  += 4;

        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;
                    
                    
        ppxlcPredMB   +=  iWidthPrev;

        pLine   +=  iWidthPrevRef;

    }

#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixed001Complete( ppxlcPredMB2, ppxlcRefMB, iWidthPrev, pErrorBuf2);
    }            
#endif

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}
#elif !defined(_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)

Void_WMV MotionCompMixed001(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV * pErrorBuf2 = pErrorBuf;

    I32_WMV t_even_1,t_even_2,t_even_3,t_even_4,t_odd_1,t_odd_2,t_odd_3,t_odd_4,u0,u1;
    U32_WMV mask = 0x00ff00ff;
    U32_WMV err_odd , err_even;
    I32_WMV iWidthPrevRef;
    I32_WMV iz, iy;

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE);
#   endif

    DEBUG_ONLY(mc_001cnt++);

#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
    
    

    for( iz = 0; iz < 2 ; iz ++ )
    {

        const U8_WMV* pLine  =  ppxlcRefMB - iWidthPrevRef;

        t_even_2 = pLine[0] | pLine[2] << 16;
        t_odd_2 = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef;

        t_even_3 = pLine[0] | pLine[2] << 16;
        t_odd_3 = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef;

        t_even_4 = pLine[0] | pLine[2] << 16;
        t_odd_4 = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef;

          
        


        for ( iy  =  0; iy < 8; iy ++ ) 
        {


            U32_WMV y0;
            U32_WMV * ppxlcpredU32;
                        
            t_even_1 = t_even_2;
            t_odd_1 = t_odd_2;
            t_even_2 = t_even_3;
            t_odd_2 = t_odd_3;
            t_even_3 = t_even_4;
            t_odd_3 = t_odd_4;
            t_even_4 = pLine[0] | pLine[2] << 16;
            t_odd_4 = pLine[1] | pLine[3] << 16;


            u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
            overflow |= u0; 
            u0 = u0>>4;
            u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
            overflow |= u1;
            u1   >>= 4;
   
            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);

            *ppxlcpredU32 = y0;
                        
                    
            ppxlcPredMB   +=  iWidthPrev;

            pLine   +=  iWidthPrevRef;

        }

       pErrorBuf=pErrorBuf2+1;
       ppxlcRefMB=ppxlcRefMB+4;
       ppxlcPredMB=ppxlcPredMB2+4;

    }


    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixed001Complete( ppxlcPredMB2, ppxlcRefMB-8, iWidthPrev, pErrorBuf2);
    }            

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#endif



#if (defined(_CE_ASM_OPT_)&&(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT)))||((!defined(_CE_ASM_OPT_))&&((!defined(MOTIONCOMP_COMPLETE))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))))

Void_WMV MotionCompMixedAlignBlock(U8_WMV*   pBlock, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev)
{

#ifndef MOTIONCOMP_COMPLETE
    
    U8_WMV*   pBlock2 = pBlock;

#endif

    U32_WMV mask = 0x00ff00ff;
    U32_WMV overflow = 0;
    I32_WMV iWidthPrevRef;
    const U8_WMV* pLine;
    U32_WMV * ppxlcpredU32;
    I32_WMV iy;
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif

#ifndef MOTIONCOMP_COMPLETE
    DEBUG_ONLY(mc_MCMABcnt++);
#else
    DEBUG_ONLY(mc_complete_cnt++);
    DEBUG_ONLY(mc_complete_MCMABcnt++);


#endif

#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif
            
    pLine  =  ppxlcRefMB - iWidthPrevRef;
    
    for ( iy  =  0; iy < 11; iy ++ ) 
    {

                    
        
        I32_WMV t0,t1,t2,t3,t4,t5,t6,t7,t8,u0,u1,u2,u3;
                

        t0 = pLine[-1] | (pLine[-1 + 2] << 16);
        t1 = pLine[0] | (pLine[0 + 2] << 16);
        t2 = pLine[1] | (pLine[1 + 2] << 16);
        t3 = pLine[2] | (pLine[2 + 2] << 16);
        t4 = pLine[3] | (pLine[3 + 2] << 16);
        t5 = pLine[4] | (pLine[4 + 2] << 16);
        t6 = pLine[5] | (pLine[5 + 2] << 16);
        t7 = pLine[6] | (pLine[6 + 2] << 16);
        t8 = pLine[7] | (pLine[7 + 2] << 16);

#ifdef MOTIONCOMP_COMPLETE
         overflow = 0;
#endif

        u0 = ((t1 + t2)*9-(t0 + t3) + 0x00080008);
        overflow |= u0; 
        u0 = u0>>4;
        u1 = ((t2 + t3)*9-(t1 + t4) + 0x00080008);
        overflow |= u1;
        u1   >>= 4;
 

        u2 = ((t5 + t6)*9-(t4 + t7) + 0x00080008);
        overflow |= u2; 
        u2   >>= 4;
        u3 = ((t6 + t7)*9-(t5 + t8) + 0x00080008);
        overflow |= u3;
        u3   >>= 4;
 
        ppxlcpredU32 = (U32_WMV *)pBlock;

#ifdef MOTIONCOMP_COMPLETE
        if(overflow & 0xf000f000)
        {

            test_err_cliping4(&u0,&u1,&u2,&u3,0);

        }
#endif

#ifdef ALIGNBLOCK_OUTPUT_TWIST

#ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
        *ppxlcpredU32=(u0 & mask);
        *(ppxlcpredU32+1)=(u2 & mask);
        *(ppxlcpredU32+22)=(u1 & mask);    
        *(ppxlcpredU32+22+1)=(u3 & mask);
        pBlock   +=  8;

#else
        *ppxlcpredU32=(u0 & mask);
        *(ppxlcpredU32+11)=(u1 & mask);
        *(ppxlcpredU32+22)=(u2 & mask);
        *(ppxlcpredU32+33)=(u3 & mask);
        pBlock   +=  4;
#endif

#else


         *ppxlcpredU32 = (u0 & mask) | ((u1 & mask) << 8);
         *(ppxlcpredU32 + 1) = (u2 & mask) | ((u3 & mask) << 8);

         pBlock   +=  8;
#endif
         pLine   +=  iWidthPrevRef;
    }
#ifndef MOTIONCOMP_COMPLETE
    if(overflow & 0xf000f000)
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixedAlignBlockComplete(pBlock2, ppxlcRefMB, iWidthPrev);
    }            
#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}


#endif // (!defined(_CE_ASM_OPT_))||defined(MOTIONCOMP_COMPLETE)


#if (defined(_CE_ASM_OPT_)&&( (defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT)))) ||((!defined(_CE_ASM_OPT_))&&( !defined(MOTIONCOMP_COMPLETE)||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))))||defined(_SH3_ASM_AVOID_BUG_)

Void_WMV MotionCompMixedHQHI(U8_WMV* ppxlcPredMB,  const U8_WMV* ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf, U8_WMV* alignedBlock)
{

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    I32_WMV iWidthPrevRef;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV * pErrorBuf = ppxliErrorBuf;
    U8_WMV* ppxlcPred = ppxlcPredMB;
    U32_WMV err_odd , err_even;
    U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock );
    I32_WMV iz, iy;
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    DEBUG_ONLY(mc_MCMHQHIcnt++);
#else
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_MCMHQHIcnt++);


#endif

    
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

    for( iz = 0;iz<2;iz++)
    {
//      U8_WMV* ppxlcPred = ppxlcPredMB + ix;
        
        I32_WMV t_even_1,t_even_2,t_even_3,t_even_4,t_odd_1,t_odd_2,t_odd_3,t_odd_4,at_even_1,at_even_2,at_even_3,at_even_4,at_odd_1 ,at_odd_2 ,at_odd_3 ,at_odd_4 ,u0,u1,u2,u3;
            


        const U8_WMV* pLine = (ppxlcRefMB - iWidthPrevRef);

        at_even_2 = pLine[0] | pLine[2] << 16;
        at_odd_2  = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef; 

        at_even_3 = pLine[0] | pLine[2] << 16;
        at_odd_3  = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef; 

        at_even_4 = pLine[0] | pLine[2] << 16;
        at_odd_4  = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef; 

#ifndef ALIGNBLOCK_OUTPUT_TWIST

//      U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock + ix);

        t_even_2 = pBlockU32[0] & mask;
        t_odd_2 = (pBlockU32[0]>>8) & mask;
        t_even_3 = pBlockU32[2] & mask;
        t_odd_3 = (pBlockU32[2]>>8) & mask;
        t_even_4 = pBlockU32[4] & mask;
        t_odd_4 = (pBlockU32[4]>>8) & mask;
#endif

        for ( iy  =  0; iy < 8; iy ++ ) 
        {

            
            U32_WMV y0;
            U32_WMV * ppxlcPredU32 = (U32_WMV *)(ppxlcPred);


#ifdef MOTIONCOMP_COMPLETE
            overflow = 0;
#endif                  

            at_even_1 = at_even_2;
            at_odd_1  = at_odd_2 ;
            at_even_2 = at_even_3;
            at_odd_2  = at_odd_3 ;
            at_even_3 = at_even_4;
            at_odd_3  = at_odd_4 ;
            at_even_4 = pLine[0] | pLine[2] << 16;
            at_odd_4  = pLine[1] | pLine[3] << 16;



            u2 = ((at_even_2 + at_even_3)*9-(at_even_1 + at_even_4) + 0x00080008);
            overflow |= u2; 
            u2 = u2>>4;
            u3 = ((at_odd_2  + at_odd_3 )*9-(at_odd_1  + at_odd_4 ) + 0x00080008);
            overflow |= u3;
            u3   >>= 4;

#ifndef ALIGNBLOCK_OUTPUT_TWIST

            t_even_1 = t_even_2;
            t_odd_1 = t_odd_2;
            t_even_2 = t_even_3;
            t_odd_2 = t_odd_3;
            t_even_3 = t_even_4;
            t_odd_3 = t_odd_4;
            t_even_4 = pBlockU32[6] & mask;
            t_odd_4 = (pBlockU32[6]>>8) & mask;

#else

#ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT

            t_even_1 = pBlockU32[0];
            t_odd_1 =  pBlockU32[0+22];
            t_even_2 = pBlockU32[2];
            t_odd_2 =  pBlockU32[2+22];
            t_even_3 = pBlockU32[4];
            t_odd_3 =  pBlockU32[4+22];
            t_even_4 = pBlockU32[6];
            t_odd_4 =  pBlockU32[6+22];


#else
            t_even_1 = pBlockU32[0];
            t_odd_1 =  pBlockU32[0+11];
            t_even_2 = pBlockU32[1];
            t_odd_2 =  pBlockU32[1+11];
            t_even_3 = pBlockU32[2];
            t_odd_3 =  pBlockU32[2+11];
            t_even_4 = pBlockU32[3];
            t_odd_4 =  pBlockU32[3+11];
#endif

#endif

            u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
            overflow |= u0; 
            u0 = u0>>4;
            u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
            overflow |= u1;
            u1   >>= 4;


#ifdef MOTIONCOMP_COMPLETE
            if(overflow & 0xf000f000)
            {

                test_err_cliping4(&u0,&u1,&u2,&u3,0);
 
            }
#endif

            u0 = ((u0 & mask) + u2 + 0x00010001)>>1;

            u1 = ((u1 & mask) + u3 + 0x00010001)>>1;


            u0  &= mask;
            u1  &= mask;
        
#ifdef MOTIONCOMP_COMPLETE
             err_overflow = 0;
#endif

            if(ppxliErrorBuf != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
        

#ifdef MOTIONCOMP_COMPLETE
                if(err_overflow & 0xff00ff00)
                {
                    test_err_cliping2(&u0,&u1,1);

                }   
#endif

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);

            *ppxlcPredU32 = y0;

    

            ppxlcPred   +=  iWidthPrev;

#ifndef ALIGNBLOCK_OUTPUT_TWIST
            pBlockU32   +=  2;
#else

#ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
            pBlockU32   +=  2;
#else
            pBlockU32   +=  1;
#endif
#endif


            pLine   +=  iWidthPrevRef; 

        }    

#ifdef ALIGNBLOCK_OUTPUT_TWIST

#ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
        pBlockU32   -=  15;
#else
        pBlockU32   +=  (3+11);
#endif

#else

        pBlockU32 -=15;

#endif

        pErrorBuf = ppxliErrorBuf + 1;
        ppxlcPred = ppxlcPredMB + 4;
        ppxlcRefMB= ppxlcRefMB+4;
    }
#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixedHQHIComplete(ppxlcPredMB,  ppxlcRefMB-8, iWidthPrev, ppxliErrorBuf, alignedBlock);
      
    }            
#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#endif

#if !defined( MOTIONCOMP_COMPLETE)
Void_WMV MotionCompMixed101(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * ppxliErrorBuf)
{

#   ifdef SH3_DSP_ASM_MOTION_COMP
        U8_WMV *alignedBlock  =  (U8_WMV *) ((int)pDspPhysicalYmem+256);
#   elif defined(SH3_ASM_MOTION_COMP)
        U32_WMV tempBlock[32 + 3];
        U8_WMV *alignedBlock  =  (U8_WMV *) (((int)tempBlock  +  15)  &  ~15);
#   else
#       ifdef ALIGNBLOCK_OUTPUT_TWIST
            U32_WMV tempBlock[11 * 4];
#       else    
            U32_WMV tempBlock[11 * 2];
#       endif
        U8_WMV *alignedBlock  =  (U8_WMV *)tempBlock;  // I32 alignment is good enough
#   endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE);
#   endif

    iWidthPrev  &= ~0x10000;
    MotionCompMixedAlignBlock(alignedBlock, ppxlcRefMB, iWidthPrev);
    MotionCompMixedHQHI(ppxlcPredMB,  ppxlcRefMB, iWidthPrev, ppxliErrorBuf, alignedBlock);

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

Void_WMV MotionCompMixed100(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev,I32_WMV * ppxliErrorBuf)
{
#   ifdef SH3_DSP_ASM_MOTION_COMP
        U8_WMV *alignedBlock  =  (U8_WMV*) ((int)pDspPhysicalYmem+256);
#   elif defined(SH3_ASM_MOTION_COMP)
        U32_WMV tempBlock[32 + 3];
        U8_WMV *alignedBlock  =  (U8_WMV *) (((int)tempBlock  +  15)  &  ~15);
#   else
#       ifdef ALIGNBLOCK_OUTPUT_TWIST
            U32_WMV tempBlock[11 * 4];
#       else    
            U32_WMV tempBlock[11 * 2];
#       endif
        U8_WMV *alignedBlock  =  (U8_WMV *) tempBlock;  // I32 alignment is good enough
#   endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE);
#   endif

    iWidthPrev  &= ~0x10000;
    MotionCompMixedAlignBlock(alignedBlock, ppxlcRefMB, iWidthPrev);
    MotionCompMixedHQHI(ppxlcPredMB, ppxlcRefMB + 1, iWidthPrev, ppxliErrorBuf, alignedBlock);

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#endif 

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))
Void_WMV MotionCompMixed000(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE

    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV * pErrorBuf2 = pErrorBuf;
#endif
#ifdef ALIGNBLOCK_OUTPUT_TWIST
    U8_WMV tempBlock[11 * 16  +  15];
#else   
    U8_WMV tempBlock[11 * 8  +  15];
#endif
    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U8_WMV *alignedBlock  =  (U8_WMV*) (((int)tempBlock  +  15)  &  ~15);
    U32_WMV * ppxlcPredU32;
            
    
    U32_WMV err_odd , err_even;
    U32_WMV mask = 0x00ff00ff;
    U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock);
    I32_WMV iy ;
    I32_WMV iWidthPrevRef;
    I32_WMV t_even_1,t_even_2,t_even_3,t_even_4,t_odd_1,t_odd_2,t_odd_3,t_odd_4,at_even_1,at_even_2,at_even_3,at_even_4,at_odd_1 ,at_odd_2 ,at_odd_3 ,at_odd_4 ,u0,u1,u2,u3;
#ifndef MOTIONCOMP_COMPLETE
#else
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_000cnt++);
#endif
    iWidthPrev  &= ~0x10000;

    MotionCompMixedAlignBlock(alignedBlock, ppxlcRefMB, iWidthPrev);

#ifdef MOTIONCOMP_COMPLETE


#endif
    
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

    

#ifndef ALIGNBLOCK_OUTPUT_TWIST
    t_even_2 = pBlockU32[0] & mask;
    t_odd_2 = (pBlockU32[0] >> 8) & mask;
    t_even_3 = pBlockU32[2] & mask;
    t_odd_3 = (pBlockU32[2] >> 8) & mask;
    t_even_4 = pBlockU32[4] & mask;
    t_odd_4 = (pBlockU32[4] >> 8) & mask;
    at_even_2 = pBlockU32[1] & mask;
    at_odd_2  = (pBlockU32[1] >> 8) & mask;
    at_even_3 = pBlockU32[3] & mask;
    at_odd_3  = (pBlockU32[3] >> 8) & mask;
    at_even_4 = pBlockU32[5] & mask;
    at_odd_4  = (pBlockU32[5] >> 8) & mask;
#endif

    for ( iy  =  0; iy < 8; iy ++ ) 
    {

            
        U32_WMV y0,y1;
        ppxlcPredU32 = (U32_WMV *)(ppxlcPredMB);

#ifdef ALIGNBLOCK_OUTPUT_TWIST

#   ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
        t_even_1 = pBlockU32[0];
        t_odd_1 =  pBlockU32[0+22];
        t_even_2 = pBlockU32[2];
        t_odd_2 =  pBlockU32[2+22];
        t_even_3 = pBlockU32[4];
        t_odd_3 =  pBlockU32[4+22];
        t_even_4 = pBlockU32[6];
        t_odd_4 =  pBlockU32[6+22];
        at_even_1 = pBlockU32[1+0];
        at_odd_1  = pBlockU32[1+0+22];
        at_even_2 = pBlockU32[1+2];
        at_odd_2  = pBlockU32[1+2+22];
        at_even_3 = pBlockU32[1+4];
        at_odd_3  = pBlockU32[1+4+22];
        at_even_4 = pBlockU32[1+6];
        at_odd_4  = pBlockU32[1+6+22];

#   else
        t_even_1 = pBlockU32[0];
        t_odd_1 =  pBlockU32[0+11];
        t_even_2 = pBlockU32[1];
        t_odd_2 =  pBlockU32[1+11];
        t_even_3 = pBlockU32[2];
        t_odd_3 =  pBlockU32[2+11];
        t_even_4 = pBlockU32[3];
        t_odd_4 =  pBlockU32[3+11];
        at_even_1 = pBlockU32[22+0];
        at_odd_1  = pBlockU32[22+0+11];
        at_even_2 = pBlockU32[22+1];
        at_odd_2  = pBlockU32[22+1+11];
        at_even_3 = pBlockU32[22+2];
        at_odd_3  = pBlockU32[22+2+11];
        at_even_4 = pBlockU32[22+3];
        at_odd_4  = pBlockU32[22+3+11];
#   endif

#else
        t_even_1 = t_even_2;
        t_odd_1 = t_odd_2;
        t_even_2 = t_even_3;
        t_odd_2 = t_odd_3;
        t_even_3 = t_even_4;
        t_odd_3 = t_odd_4;
        t_even_4 = pBlockU32[6] & mask;
        t_odd_4 = (pBlockU32[6] >> 8) & mask;
        at_even_1 = at_even_2;
        at_odd_1  = at_odd_2 ;
        at_even_2 = at_even_3;
        at_odd_2  = at_odd_3 ;
        at_even_3 = at_even_4;
        at_odd_3  = at_odd_4 ;
        at_even_4 = pBlockU32[7] & mask;
        at_odd_4  = (pBlockU32[7]>>8) & mask;

#endif

#ifdef MOTIONCOMP_COMPLETE
        overflow = 0;
#endif

        u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
        overflow |= u0; 
        u0 = u0>>4;
        u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
        overflow |= u1;
        u1   >>= 4;

        u2 = ((at_even_2 + at_even_3)*9-(at_even_1 + at_even_4) + 0x00080008);
        overflow |= u2; 
        u2 = u2>>4;
        u3 = ((at_odd_2  + at_odd_3 )*9-(at_odd_1  + at_odd_4 ) + 0x00080008);
        overflow |= u3;
        u3   >>= 4;

#ifdef MOTIONCOMP_COMPLETE
        if(overflow & 0xf000f000)
        {
            test_err_cliping4(&u0,&u1,&u2,&u3,0);

        }
#endif

        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE
        err_overflow = 0;
#endif

        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif


            pErrorBuf  += 4;

        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcPredU32 = y0;
        *(ppxlcPredU32 + 1) = y1;

        ppxlcPredMB   +=  iWidthPrev;

#ifdef ALIGNBLOCK_OUTPUT_TWIST

#ifdef ALIGNBLOCK_OUTPUT_TWIST_64BIT
        pBlockU32   +=  2;
#else
        pBlockU32   +=  1;
#endif

#else
        pBlockU32   +=  2;
#endif

    }

#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixed000Complete( ppxlcPredMB2, ppxlcRefMB, iWidthPrev, pErrorBuf2);
    }            
#endif
                
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#elif !defined(_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)

Void_WMV MotionCompMixed000(U8_WMV*   ppxlcPredMB, const U8_WMV*  ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV * pErrorBuf)
{

    U32_WMV err_overflow = 0;
    U32_WMV overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV * pErrorBuf2 = pErrorBuf;

#ifdef ALIGNBLOCK_OUTPUT_TWIST

    U8_WMV tempBlock[11 * 16  +  15];

#else   
    U8_WMV tempBlock[11 * 8  +  15];
#endif
    U8_WMV *alignedBlock  =  (U8_WMV*) (((int)tempBlock  +  15)  &  ~15);
    U32_WMV * ppxlcPredU32;
        U32_WMV err_odd , err_even;
    U32_WMV mask = 0x00ff00ff;

    I32_WMV t_even_1,t_even_2,t_even_3,t_even_4,t_odd_1,t_odd_2,t_odd_3,t_odd_4,u0,u1;
            
    I32_WMV iWidthPrevRef;
    I32_WMV iz, iy;
#ifdef ALIGNBLOCK_OUTPUT_TWIST
    U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock);

#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPMIXED_PROFILE);
#   endif

    DEBUG_ONLY(mc_000cnt++);

    iWidthPrev  &= ~0x10000;
  


    MotionCompMixedAlignBlock(alignedBlock, ppxlcRefMB, iWidthPrev);

    
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    iWidthPrev  &= 0xffff;
#else
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

    for( iz = 0; iz < 2 ; iz ++ )
    {
#ifndef ALIGNBLOCK_OUTPUT_TWIST
        U32_WMV * pBlockU32 = (U32_WMV *)(alignedBlock);

        t_even_2 = pBlockU32[0] & mask;
        t_odd_2 = (pBlockU32[0] >> 8) & mask;
        t_even_3 = pBlockU32[2] & mask;
        t_odd_3 = (pBlockU32[2] >> 8) & mask;
        t_even_4 = pBlockU32[4] & mask;
        t_odd_4 = (pBlockU32[4] >> 8) & mask;
#endif

        for ( iy  =  0; iy < 8; iy ++ ) 
        {

                
            U32_WMV y0;
            ppxlcPredU32 = (U32_WMV *)(ppxlcPredMB);
#ifdef ALIGNBLOCK_OUTPUT_TWIST
        t_even_1 = pBlockU32[0];
        t_odd_1 =  pBlockU32[0+11];
        t_even_2 = pBlockU32[1];
        t_odd_2 =  pBlockU32[1+11];
        t_even_3 = pBlockU32[2];
        t_odd_3 =  pBlockU32[2+11];
        t_even_4 = pBlockU32[3];
        t_odd_4 =  pBlockU32[3+11];
#else
            t_even_1 = t_even_2;
            t_odd_1 = t_odd_2;
            t_even_2 = t_even_3;
            t_odd_2 = t_odd_3;
            t_even_3 = t_even_4;
            t_odd_3 = t_odd_4;
            t_even_4 = pBlockU32[6] & mask;
            t_odd_4 = (pBlockU32[6] >> 8) & mask;
#endif

            u0 = ((t_even_2 + t_even_3)*9-(t_even_1 + t_even_4) + 0x00080008);
            overflow |= u0; 
            u0 = u0>>4;
            u1 = ((t_odd_2 + t_odd_3)*9-(t_odd_1 + t_odd_4) + 0x00080008);
            overflow |= u1;
            u1   >>= 4;


            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
        

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);

            *ppxlcPredU32 = y0;

            ppxlcPredMB   +=  iWidthPrev;

#ifdef ALIGNBLOCK_OUTPUT_TWIST
        pBlockU32   +=  1;
#else
        pBlockU32   +=  2;
#endif
            

        }

       pErrorBuf=pErrorBuf2+1;
       ppxlcPredMB=ppxlcPredMB2+4;

#ifdef ALIGNBLOCK_OUTPUT_TWIST

        pBlockU32   +=  (3+11);

#else
       alignedBlock+=4; 
#endif
       

    }



    if((err_overflow & 0xff00ff00) || (overflow & 0xf000f000))
    {
#ifdef DYNAMIC_EDGEPAD
    iWidthPrev  |= iWidthPrevRef<<17;
#endif
        MotionCompMixed000Complete( ppxlcPredMB2, ppxlcRefMB, iWidthPrev, pErrorBuf2);
    }            

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}



#endif

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))
Void_WMV MotionCompWAddError10(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{
#   if !defined(_SH4_)
        _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPADDERROR_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    I32_WMV iy;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV __huge*ppxlcRefMB2 = ppxlcRefMB;
    I32_WMV iWidthPrev2 = iWidthPrev;
    I32_WMV * pErrorBuf2 = pErrorBuf;
#endif

    U32_WMV mask = 0x00ff00ff;
    U32_WMV err_overflow = 0;
    U32_WMV err_odd , err_even;
    U32_WMV rndCtrl;
    I32_WMV iy;
    I32_WMV iWidthPrevRef;


#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
    rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

#ifndef MOTIONCOMP_COMPLETE
#else
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_10cnt++);
#endif

    for ( iy  =  0; iy < 8; iy ++ ) 
    {

        U32_WMV y0,y1;
        I32_WMV t1,t2,t3,t4,t5,t6,t7,u0,u1,u2,u3;
        U32_WMV * ppxlcpredU32;
        


        t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
        t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
        t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);
        t4 = ppxlcRefMB[3] | (ppxlcRefMB[3 + 2] << 16);
        t5 = ppxlcRefMB[4] | (ppxlcRefMB[4 + 2] << 16);
        t6 = ppxlcRefMB[5] | (ppxlcRefMB[5 + 2] << 16);
        t7 = ppxlcRefMB[6] | (ppxlcRefMB[6 + 2] << 16);

        u0 = (t1 + t2 + rndCtrl);
        u0 = u0>>1;

        u1 = (t2 + t3 + rndCtrl);
        u1   >>= 1;
        

        u2 = (t5 + t6 + rndCtrl);
        u2   >>= 1;
        u3 = (t6 + t7 + rndCtrl);
        u3   >>= 1;
        

        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE
        err_overflow = 0;
#endif

        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif

            pErrorBuf  += 4;

        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;



        ppxlcPredMB   +=  iWidthPrev;
        ppxlcRefMB   +=  iWidthPrevRef;
    } 
#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00))
    {
        MotionCompWAddError10Complete(ppxlcPredMB2, ppxlcRefMB2,iWidthPrev2, pErrorBuf2);
    }            
#endif

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#elif !defined(_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)

Void_WMV MotionCompWAddError10(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{

    U32_WMV err_overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    const U8_WMV __huge*ppxlcRefMB2 = ppxlcRefMB;
    I32_WMV * pErrorBuf2 = pErrorBuf;
    I32_WMV iWidthPrev2= iWidthPrev;


    U32_WMV mask = 0x00ff00ff;
    U32_WMV rndCtrl;
    U32_WMV err_odd , err_even;
    I32_WMV iz, iy;
    I32_WMV iWidthPrevRef;
#   if !defined(_SH4_)
        _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPADDERROR_PROFILE);
#   endif

    DEBUG_ONLY(mc_10cnt++);
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
    rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

    for( iz = 0; iz < 2 ; iz ++ )
    {

        for ( iy  =  0; iy < 8; iy ++ ) 
        {

            U32_WMV y0;
            I32_WMV t1,t2,t3,u0,u1;
            U32_WMV * ppxlcpredU32 ;


            t1 = ppxlcRefMB[0] | (ppxlcRefMB[0 + 2] << 16);
            t2 = ppxlcRefMB[1] | (ppxlcRefMB[1 + 2] << 16);
            t3 = ppxlcRefMB[2] | (ppxlcRefMB[2 + 2] << 16);

            u0 = (t1 + t2 + rndCtrl);
            u0 = u0>>1;

            u1 = (t2 + t3 + rndCtrl);
            u1   >>= 1;
            

            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;

            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);


            *ppxlcpredU32 = y0;

            ppxlcPredMB   +=  iWidthPrev;
            ppxlcRefMB   +=  iWidthPrevRef;
        } 

       pErrorBuf=pErrorBuf2+1;
       ppxlcRefMB = ppxlcRefMB2 + 4;
       ppxlcPredMB=ppxlcPredMB2+4;

    }



    if((err_overflow & 0xff00ff00))
    {
        MotionCompWAddError10Complete(ppxlcPredMB2, ppxlcRefMB2,iWidthPrev2, pErrorBuf2);
    }            

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}
#endif

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))
Void_WMV MotionCompWAddError01(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{
#   if !defined(_SH4_)
        _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPADDERROR_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV iWidthPrev2 = iWidthPrev;
    I32_WMV * pErrorBuf2 = pErrorBuf;
#endif
    I32_WMV t_even_1,t_even_2,t_odd_1,t_odd_2,t_even_5,t_even_6,t_odd_5,t_odd_6,u0,u1,u2,u3;
    U32_WMV err_overflow = 0;
    U32_WMV err_odd , err_even;
    U32_WMV rndCtrl ;
    I32_WMV iWidthPrevRef;
    const U8_WMV* pLine  =  ppxlcRefMB;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iy;

#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
    rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

#ifndef MOTIONCOMP_COMPLETE
#else
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_01cnt++);
#endif

    

    t_even_2 = pLine[0] | pLine[2] << 16;
    t_odd_2 = pLine[1] | pLine[3] << 16;
    t_even_6 = pLine[4] | pLine[6] << 16;
    t_odd_6 = pLine[5] | pLine[7] << 16;

    pLine   +=  iWidthPrevRef;


    for (iy  =  0; iy < 8; iy ++ ) 
    {


        U32_WMV y0,y1;
        U32_WMV * ppxlcpredU32;
                    
        t_even_1 = t_even_2;
        t_odd_1 = t_odd_2;
        t_even_2 = pLine[0] | pLine[2] << 16;
        t_odd_2 = pLine[1] | pLine[3] << 16;

        u0 = (t_even_1 + t_even_2 + rndCtrl);
        u0 = u0>>1;

        u1 = ((t_odd_1 + t_odd_2 + rndCtrl));
        u1   >>= 1;

        t_even_5 = t_even_6;
        t_odd_5 = t_odd_6;
        t_even_6 = pLine[4] | pLine[6] << 16;
        t_odd_6 = pLine[5] | pLine[7] << 16;




        u2 = (t_even_5 + t_even_6 + rndCtrl);
        u2 = u2>>1;
        u3 = ((t_odd_5 + t_odd_6 + rndCtrl));
        u3   >>= 1;


        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;


        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE
        err_overflow = 0;
#endif

        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif

            pErrorBuf  += 4;

        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;
                    
                    
        ppxlcPredMB   +=  iWidthPrev;

        pLine   +=  iWidthPrevRef;

    }
#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00))
    {
        MotionCompWAddError01Complete(ppxlcPredMB2, ppxlcRefMB,iWidthPrev2, pErrorBuf2);
    }            
#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}
#elif !defined(_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)
Void_WMV MotionCompWAddError01(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{

    U32_WMV err_overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV iWidthPrev2 = iWidthPrev;
    I32_WMV * pErrorBuf2 = pErrorBuf;
    U32_WMV rndCtrl;
    I32_WMV t_even_1,t_even_2,t_odd_1,t_odd_2, u0,u1;

    U32_WMV err_odd , err_even;

    I32_WMV iWidthPrevRef;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iz, iy;
    const U8_WMV* pLine  =  ppxlcRefMB;


#   if !defined(_SH4_)
        _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPADDERROR_PROFILE);
#   endif

    DEBUG_ONLY(mc_01cnt++);
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
     rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
     rndCtrl = (iWidthPrev>>16)&1;
    //rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif


    for( iz = 0; iz < 2 ; iz ++ )
    {
    

        t_even_2 = pLine[0] | pLine[2] << 16;
        t_odd_2 = pLine[1] | pLine[3] << 16;

        pLine   +=  iWidthPrevRef;

        for ( iy  =  0; iy < 8; iy ++ ) 
        {


            U32_WMV y0;
            U32_WMV * ppxlcpredU32;
                        
            t_even_1 = t_even_2;
            t_odd_1 = t_odd_2;
            t_even_2 = pLine[0] | pLine[2] << 16;
            t_odd_2 = pLine[1] | pLine[3] << 16;

            u0 = (t_even_1 + t_even_2 + rndCtrl);
            u0 = u0>>1;

            u1 = ((t_odd_1 + t_odd_2 + rndCtrl));
            u1   >>= 1;

            ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;


            u0  &= mask;
            u1  &= mask;

            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;
        
                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);
 

            *ppxlcpredU32 = y0;

                        
                    
            ppxlcPredMB   +=  iWidthPrev;

            pLine   +=  iWidthPrevRef;

        }

        pErrorBuf=pErrorBuf2+1;
       
       pLine  =  ppxlcRefMB+4;
       ppxlcPredMB=ppxlcPredMB2+4;

    }

    if((err_overflow & 0xff00ff00))
    {
        MotionCompWAddError01Complete(ppxlcPredMB2, ppxlcRefMB,iWidthPrev2, pErrorBuf2);
    }            

#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}


#endif

#if (defined( PLATFORM_32REGS)&&!defined(_CE_ASM_OPT_))||(defined(MOTIONCOMP_COMPLETE)&& !defined(MOTIONCOMP_COMPLETE_SHORTCUT))
Void_WMV MotionCompWAddError00(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{
#   if !defined(_SH4_)
        _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPADDERROR_PROFILE)
#   endif
#ifndef MOTIONCOMP_COMPLETE
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV iWidthPrev2 = iWidthPrev;
    I32_WMV * pErrorBuf2 = pErrorBuf;
#endif
    I32_WMV t_even_1,ta1,t_odd_1,t_even_2,t_odd_2,ta2,t_even_5,t_even_6,t_odd_5,t_odd_6,ta5,ta6,u0,u1,u2,u3;
    U32_WMV err_odd , err_even;
    U32_WMV err_overflow = 0;
    I32_WMV iWidthPrevRef;
    U32_WMV rndCtrl;
    const U8_WMV* pLine  =  ppxlcRefMB;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iy;



#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
    rndCtrl = (iWidthPrev>>16)&1;
    rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
    rndCtrl = (iWidthPrev>>16)&1;
    rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif

#ifndef MOTIONCOMP_COMPLETE
#else
        DEBUG_ONLY(mc_complete_cnt++);
        DEBUG_ONLY(mc_complete_00cnt++);
#endif
    

    t_even_2 = pLine[0] | pLine[2] << 16;
    t_odd_2 = pLine[1] | pLine[3] << 16;
    ta2 = pLine[2] | pLine[4] << 16;
    t_even_6 = pLine[4] | pLine[6] << 16;
    t_odd_6 = pLine[5] | pLine[7] << 16;
    ta6 = pLine[6] | pLine[8] << 16;

    pLine   +=  iWidthPrevRef;

      
    


    for ( iy  =  0; iy < 8; iy ++ ) 
    {


        U32_WMV y0,y1;
        U32_WMV * ppxlcpredU32;
                    
        t_even_1 = t_even_2;
        t_odd_1 = t_odd_2;
        ta1 = ta2;
        t_even_2 = pLine[0] | pLine[2] << 16;
        t_odd_2 = pLine[1] | pLine[3] << 16;
        ta2 = pLine[2] | pLine[4] << 16;

        u0 = (t_even_1 + t_even_2 + t_odd_1 + t_odd_2 + rndCtrl);
        u0 = u0>>2;

        u1 = (t_odd_1 + t_odd_2 + ta1 + ta2 + rndCtrl);
        u1   >>= 2;

        t_even_5 = t_even_6;
        t_odd_5 = t_odd_6;
        ta5 = ta6;
        t_even_6 = pLine[4] | pLine[6] << 16;
        t_odd_6 = pLine[5] | pLine[7] << 16;
        ta6 = pLine[6] | pLine[8] << 16;



        u2 = (t_even_5 + t_even_6 + t_odd_5 + t_odd_6 + rndCtrl);
        u2 = u2>>2;
        u3 = ((t_odd_5 + t_odd_6 + ta5 + ta6 + rndCtrl));
        u3   >>= 2;


        ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;


        u0  &= mask;
        u1  &= mask;
        u2  &= mask;
        u3  &= mask;

#ifdef MOTIONCOMP_COMPLETE
        err_overflow = 0;
#endif

        if(pErrorBuf != NULL)
        {
 

            err_even = pErrorBuf[0];
            err_odd  = pErrorBuf[0 + 32];

            u0 = u0 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u0;
            u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u1;
        
            err_even = pErrorBuf[1];
            err_odd  = pErrorBuf[1 + 32];

            u2 = u2 + err_even-((err_even & 0x8000) << 1);
            err_overflow |= u2;
            u3 = u3 + err_odd -((err_odd  & 0x8000) << 1);
            err_overflow |= u3;

#ifdef MOTIONCOMP_COMPLETE
            if(err_overflow & 0xff00ff00)
            {
                test_err_cliping4(&u0,&u1,&u2,&u3,1);

            }
#endif

            pErrorBuf  += 4;

        }

        y0 = (u0) | ((u1) << 8);
        y1 = (u2) | ((u3) << 8);

        *ppxlcpredU32 = y0;
        *(ppxlcpredU32 + 1) = y1;
                    
                    
        ppxlcPredMB   +=  iWidthPrev;

        pLine   +=  iWidthPrevRef;

    }
#ifndef MOTIONCOMP_COMPLETE
    if((err_overflow & 0xff00ff00))
    {
        MotionCompWAddError00Complete(ppxlcPredMB2, ppxlcRefMB,iWidthPrev2, pErrorBuf2);
    }            
#endif
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif

}

#elif !defined(_CE_ASM_OPT_) && !defined(MOTIONCOMP_COMPLETE)

Void_WMV MotionCompWAddError00(U8_WMV __huge* ppxlcPredMB, const U8_WMV __huge*ppxlcRefMB, I32_WMV iWidthPrev, I32_WMV *pErrorBuf)
{

    U32_WMV err_overflow = 0;
    U8_WMV*   ppxlcPredMB2 = ppxlcPredMB;
    I32_WMV iWidthPrev2 = iWidthPrev;
 
    I32_WMV * pErrorBuf2 = pErrorBuf;

    I32_WMV t_even_1,ta1,t_odd_1,t_even_2,t_odd_2,ta2,u0,u1;

    U32_WMV err_odd , err_even;
    U32_WMV rndCtrl ;
    const U8_WMV* pLine  =  ppxlcRefMB;
    I32_WMV iz, iy;
    U32_WMV mask = 0x00ff00ff;
    I32_WMV iWidthPrevRef;
#   if !defined(_SH4_)
        _FUNCTION_PROFILE_DECL_START(fp,MOTIONCOMPADDERROR_PROFILE);
#   endif

    DEBUG_ONLY(mc_00cnt++);
#ifdef DYNAMIC_EDGEPAD
    iWidthPrevRef=(iWidthPrev>>17);
     rndCtrl = (iWidthPrev>>16)&1;
    rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
#else
     rndCtrl = (iWidthPrev>>16)&1;
    rndCtrl ++ ;
    rndCtrl |= rndCtrl << 16;
    iWidthPrev  &= 0xffff;
    iWidthPrevRef=iWidthPrev;
#endif




    for( iz = 0; iz < 2 ; iz ++ )
    {
    

        t_even_2 = pLine[0] | pLine[2] << 16;
        t_odd_2 = pLine[1] | pLine[3] << 16;
        ta2 = pLine[2] | pLine[4] << 16;

        pLine   +=  iWidthPrevRef;

          


        for ( iy  =  0; iy < 8; iy ++ ) 
        {


            U32_WMV y0;
            U32_WMV * ppxlcpredU32;
                        
            t_even_1 = t_even_2;
            t_odd_1 = t_odd_2;
            ta1 = ta2;
            t_even_2 = pLine[0] | pLine[2] << 16;
            t_odd_2 = pLine[1] | pLine[3] << 16;
            ta2 = pLine[2] | pLine[4] << 16;

            u0 = (t_even_1 + t_even_2 + t_odd_1 + t_odd_2 + rndCtrl);
            u0 = u0>>2;

            u1 = (t_odd_1 + t_odd_2 + ta1 + ta2 + rndCtrl);
            u1   >>= 2;




             ppxlcpredU32 = (U32_WMV *)ppxlcPredMB;


            u0  &= mask;
            u1  &= mask;


            if(pErrorBuf2 != NULL)
            {
 

                err_even = pErrorBuf[0];
                err_odd  = pErrorBuf[0 + 32];

                u0 = u0 + err_even-((err_even & 0x8000) << 1);
                err_overflow |= u0;
                u1 = u1 + err_odd -((err_odd  & 0x8000) << 1);
                err_overflow |= u1;

                pErrorBuf  += 4;

            }

            y0 = (u0) | ((u1) << 8);
        
            *ppxlcpredU32 = y0;
                        
                    
            ppxlcPredMB   +=  iWidthPrev;

            pLine   +=  iWidthPrevRef;

        }

       pErrorBuf=pErrorBuf2+1;
       
       pLine  =  ppxlcRefMB+4;
       ppxlcPredMB=ppxlcPredMB2+4;

    }


    if((err_overflow & 0xff00ff00))
    {
        MotionCompWAddError00Complete(ppxlcPredMB2, ppxlcRefMB,iWidthPrev2, pErrorBuf2);
    }            
#   if !defined(_SH4_)
    _FUNCTION_PROFILE_STOP(&fp);
#   endif
}

#endif

#if defined( MOTIONCOMP_COMPLETE)

#undef MotionCompMixed011 
#undef MotionCompMixed010 
#undef MotionCompMixed110 
#undef MotionCompMixed111 
#undef MotionCompMixed001 
#undef MotionCompMixed101 
#undef MotionCompMixed100 
#undef MotionCompMixed000 
#undef MotionCompMixedAlignBlock 
#undef MotionCompMixedHQHI 
#undef MotionCompWAddError10 
#undef MotionCompWAddError01 
#undef MotionCompWAddError00 

#undef MOTIONCOMP_COMPLETE_SHORTCUT

#endif


