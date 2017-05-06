//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_function.h"

#ifdef _EMB_WMV2_

#if !defined(SH3_ASM_MOTION_COMP) && !defined(SH3_DSP_ASM_MOTION_COMP)

U32_WMV test_cliping(U32_WMV u)
{
    U32_WMV v;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(test_cliping);


    u=(u+((u&0x0800)<<1));

    

    if((v=(u&0x0f000000)))
    {
        u&=0xffff;

        if(!(v&0x08000000))
            u|=0x00ff0000;
    }

    if((v=(u&0x00000f00)))
    {
        u&=0xffff0000;
        if(!(v&0x00000800))
            u|=0x000000ff;
    }


    return u;

}

U32_WMV test_err_cliping(U32_WMV u)
{
    U32_WMV v;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(test_err_cliping);

    if(u&0x8000)
    {
        u=u+0x10000;
    }

    if((v=(u&0xff000000)))
    {
        u&=0xffff;

        if(!(v&0x80000000))
            u|=0x00ff0000;
    }

    if((v=(u&0x0000ff00)))
    {
        u&=0xffff0000;
        if(!(v&0x00008000))
            u|=0x000000ff;
    }


    return u;

}


Void_WMV test_err_cliping4(I32_WMV * pt0, I32_WMV *pt1, I32_WMV *pt2, I32_WMV *pt3, I32_WMV IsErrCliping)
{

    I32_WMV t0=*pt0;
    I32_WMV t1=*pt1;
    I32_WMV t2=*pt2;
    I32_WMV t3=*pt3;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(test_err_cliping4);

    if(IsErrCliping)
    {
        if(t0&0xff00ff00)
        {
            t0=test_err_cliping(t0);
        }
        if(t1&0xff00ff00)
        {
            t1=test_err_cliping(t1);
        }
        if(t2&0xff00ff00)
        {
            t2=test_err_cliping(t2);
        }
        if(t3&0xff00ff00)
        {
            t3=test_err_cliping(t3);
        }
    }
    else
    {
         if(t0&0x0f000f00)
            t0=test_cliping(t0);
         if(t1&0x0f000f00)
            t1=test_cliping(t1);
         if(t2&0x0f000f00)
            t2=test_cliping(t2);
         if(t3&0x0f000f00)
            t3=test_cliping(t3);
    }

    *pt0=t0;
    *pt1=t1;
    *pt2=t2;
    *pt3=t3;

}

Void_WMV test_err_cliping2(I32_WMV *pt0, I32_WMV *pt1, I32_WMV IsErrCliping)
{

    I32_WMV t0=*pt0;
    I32_WMV t1=*pt1;
    DEBUG_PROFILE_FRAME_FUNCTION_COUNT(test_err_cliping2);

    if(IsErrCliping)
    {
        if(t0&0xff00ff00)
        {
            t0=test_err_cliping(t0);
        }
        if(t1&0xff00ff00)
        {
            t1=test_err_cliping(t1);
        }

    }
    else
    {
         if(t0&0x0f000f00)
            t0=test_cliping(t0);
         if(t1&0x0f000f00)
            t1=test_cliping(t1);
    }
    *pt0=t0;
    *pt1=t1;
}

#else // !defined(SH3_ASM_MOTION_COMP) 

extern U32_WMV test_cliping(U32_WMV u);
extern U32_WMV test_err_cliping(U32_WMV u);
extern Void_WMV test_err_cliping4(I32_WMV * pt0, I32_WMV *pt1, I32_WMV *pt2, I32_WMV *pt3, I32_WMV IsErrCliping);
extern Void_WMV test_err_cliping2(I32_WMV *pt0, I32_WMV *pt1, I32_WMV IsErrCliping);

#endif // !define(SH3_ASM_MOTION_COMP) 


#define MOTIONCOMP_COMPLETE
#include "motioncomp_emb_comm.h"
#undef MOTIONCOMP_COMPLETE


#include "motioncomp_emb_comm.h"


#endif