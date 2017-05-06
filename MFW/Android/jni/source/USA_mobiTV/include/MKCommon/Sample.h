#ifndef __MKCommon_Sample_h__
#define __MKCommon_Sample_h__

#include <MKBase/Platform.h>
#include <MKBase/Type.h>
#include <MKBase/Memory.h>
#include <MKBase/Time.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct MK_Sample
{
    MK_U8*  mData;
    MK_U32  mDataLen;
    MK_Time mTime;
    MK_Bool mSyncSample;
} MK_Sample;

MK_INLINE MK_Sample* MK_Sample_Create(MK_U8* aData, MK_U32 aSz, MK_Time aTime)
{
    MK_Sample* lSample = (MK_Sample*)MK_Mem_AllocAndZero(sizeof(MK_Sample));
    if (aData)
    {
        lSample->mData = (MK_U8*)MK_Mem_AllocAndCopy(aData, aSz, aSz);
        lSample->mDataLen = aSz;
    }
    lSample->mTime = aTime;

    return lSample;
}

MK_INLINE void MK_Sample_Destroy(MK_Sample* aSample)
{
    if (aSample)
    {
        if (aSample->mData)
        {
            MK_Mem_Free(aSample->mData);
            aSample->mDataLen = 0;
        }
        MK_Mem_Free(aSample);
    }
}

MK_INLINE MK_Bool MK_Sample_IsSyncSample(MK_Sample* aSample)
{
    return aSample->mSyncSample;
}

#ifdef __cplusplus
}
#endif


#endif
