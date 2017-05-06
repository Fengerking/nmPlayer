/*
 * NOTE: Porting candidate.
 */

#ifndef __MKBase_Memory_h__
#define __MKBase_Memory_h__

#include <MKBase/Platform.h>
#include <MKBase/Config.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/* INTERFACE                                                                  */
/******************************************************************************/

/*
 * Compare first aN bytes of the two memory blocks aPtr1 and aPtr2. Returns zero
 * if the blocks are identical, otherwise a negative value is returned to if
 * aPtr1 < aPtr2 and positive value if aPtr1 > aPtr2.
 */
MK_INLINE MK_S32 MK_Mem_Cmp(const void* aPtr1, const void* aPtr2, MK_U32 aN);

/*
 * Find first occurance of aVal in aPtr within the first aN bytes.
 */
MK_INLINE void* MK_Mem_Find(const void* aPtr, MK_U8 aVal, MK_U32 aN);

/*
 * Find last occurance of aVal in aPtr within the first aN bytes.
 */
MK_INLINE void* MK_Mem_RFind(const void* aPtr, MK_U8 aVal, MK_U32 aN);

/******************************************************************************/

/*
 * Fill aSz bytes of memory in aPtr with value aVal.
 */
MK_INLINE void MK_Mem_Fill(void* aPtr, MK_U8 aVal, MK_U32 aSz);

/*
 * Fill aSz bytes of memory in aPtr with zero.
 */
MK_INLINE void MK_Mem_Zero(void* aPtr, MK_U32 aSz);

/*
 * Copy aSz bytes from aFrom into aTo. Overlapping buffers cause undefined
 * behaivour.
 */
MK_INLINE void MK_Mem_Copy(const void* MK_RESTRICT aFrom, void* MK_RESTRICT aTo, MK_U32 aSz);

/*
 * Copy aSz bytes from aFrom into aTo. Overlapping buffers are supported.
 */
MK_INLINE void MK_Mem_Move(const void* aFrom, void* aTo, MK_U32 aSz);

/******************************************************************************/

/*
 * Allocate aSz bytes of memory and return a pointer to it. Returns NULL on
 * failure.
 */
MK_INLINE void* MK_Mem_Alloc(MK_U32 aSz);

/*
 * Free memory alloction.
 */
MK_INLINE void MK_Mem_Free(void* aPtr);

/*
 * Free memory alloction with NULL ptr protection.
 */
MK_INLINE void MK_Mem_FreeN(void* aPtr);

/******************************************************************************/

/*
 * Allocate aSz bytes of memory, fill it with aVal, and return a pointer to
 * it. Returns NULL on failure.
 */
MK_INLINE void* MK_Mem_AllocAndFill(MK_U8 aVal, MK_U32 aSz);

/*
 * Allocate aSz bytes of memory, fill it with zeroes, and return a pointer to
 * it. Returns NULL on failure.
 */
MK_INLINE void* MK_Mem_AllocAndZero(MK_U32 aSz);

/*
 * Allocate aSz bytes of memory, copy aCopySz bytes from aPtr, and return a
 * pointer to it. Returns NULL on failure.
 */
MK_INLINE void* MK_Mem_AllocAndCopy(const void* aPtr, MK_U32 aCopySz, MK_U32 aSz);

/*
 * Reallocate memory to hold aNewSz bytes and copy (at least) aCopySz from
 * the previous allocation to the new one. The returned pointer may/may not
 * match aPtr. On failure NULL is returned.
 *
 * NOTE: The original buffer is NOT freed on failure!
 */
MK_INLINE void* MK_Mem_Realloc(void* aPtr, MK_U32 aCopySz, MK_U32 aNewSz);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#include <string.h>
#include <stdlib.h>

/******************************************************************************/

MK_INLINE MK_S32 MK_Mem_Cmp(const void* aPtr1, const void* aPtr2, MK_U32 aN)
{
    MK_ASSERT((NULL != aPtr1 && NULL != aPtr2) || 0 == aN);
    return memcmp(aPtr1, aPtr2, aN);
}

MK_INLINE void* MK_Mem_Find(const void* aPtr, MK_U8 aVal, MK_U32 aN)
{
    MK_ASSERT(NULL != aPtr || 0 == aN);
    return MK_Cast(void*, memchr(aPtr, aVal, aN));
}

MK_INLINE void* MK_Mem_RFind(const void* aPtr, MK_U8 aVal, MK_U32 aN)
{
    MK_ASSERT(NULL != aPtr || 0 == aN);
    #if MK_HAVE_MEMRCHR
        return MK_Cast(void*, memrchr(aPtr, aVal, aN));
    #else
        if (0 != aN)
        {
            const MK_U8* lPtr = MK_Cast(const MK_U8*, aPtr);
            for (lPtr += aN - 1; MK_Cast(const void*, lPtr) >= aPtr; --lPtr)
            {
                if (*lPtr == aVal)
                {
                    return MK_Cast(void*, lPtr);
                }
            }
        }
        return MK_Cast(void*, 0);
    #endif
}

/******************************************************************************/

MK_INLINE void MK_Mem_Fill(void* aPtr, MK_U8 aVal, MK_U32 aSz)
{
    MK_ASSERT(NULL != aPtr || 0 == aSz);
    memset(aPtr, aVal, aSz);
}

MK_INLINE void MK_Mem_Zero(void* aPtr, MK_U32 aSz)
{
    MK_ASSERT(NULL != aPtr || 0 == aSz);
    MK_Mem_Fill(aPtr, MK_ToU8(0), aSz);
}

MK_INLINE void MK_Mem_Copy(const void* MK_RESTRICT aFrom, void* MK_RESTRICT aTo, MK_U32 aSz)
{
    MK_ASSERT((NULL != aFrom && NULL != aTo) || 0 == aSz);
    memcpy(aTo, aFrom, aSz);
}

MK_INLINE void MK_Mem_Move(const void* aFrom, void* aTo, MK_U32 aSz)
{
    MK_ASSERT((NULL != aFrom && NULL != aTo) || 0 == aSz);
    memmove(aTo, aFrom, aSz);
}

/******************************************************************************/

void* _MK_MChk_Alloc(MK_U32 aSz);
void _MK_MChk_Free(void* aPtr);

/******************************************************************************/

MK_INLINE void* MK_Mem_Alloc(MK_U32 aSz)
{
    #if MK_MEMCHECK
        return _MK_MChk_Alloc(aSz);
    #else
        MK_ASSERT(0 != aSz);
        return malloc(aSz);
    #endif
}

MK_INLINE void MK_Mem_Free(void* aPtr)
{
    #if MK_MEMCHECK
        return _MK_MChk_Free(aPtr);
    #else
        MK_ASSERT(NULL != aPtr);
        free(aPtr);
    #endif
}

MK_INLINE void MK_Mem_FreeN(void* aPtr)
{
    if (NULL != aPtr)
    {
        MK_Mem_Free(aPtr);
    }
}

/******************************************************************************/

MK_INLINE void* MK_Mem_AllocAndFill(MK_U8 aVal, MK_U32 aSz)
{
    void* lPtr = MK_Mem_Alloc(aSz);
    if (NULL != lPtr)
    {
        MK_Mem_Fill(lPtr, aVal, aSz);
    }
    return lPtr;
}

MK_INLINE void* MK_Mem_AllocAndZero(MK_U32 aSz)
{
    #if MK_HAVE_CALLOC
        return calloc(1, aSz);
    #else
        return MK_Mem_AllocAndFill(0, aSz);
    #endif
}

MK_INLINE void* MK_Mem_AllocAndCopy(const void* aPtr, MK_U32 aCopySz, MK_U32 aSz)
{
    MK_ASSERT(NULL != aPtr && aCopySz <= aSz);
    {
        void* lPtr = MK_Mem_Alloc(aSz);
        if (NULL != lPtr)
        {
            MK_Mem_Copy(aPtr, lPtr, aCopySz);
        }
        return lPtr;
    }
}

MK_INLINE void* MK_Mem_Realloc(void* aPtr, MK_U32 aCopySz, MK_U32 aNewSz)
{
    #if MK_HAVE_REALLOC
        MK_UNUSED(aCopySz);
        return realloc(aPtr, aNewSz);
    #else
        void* lPtr = MK_Mem_Alloc(aNewSz);
        if (NULL != aPtr && NULL != lPtr)
        {
            MK_Mem_Copy(aPtr, lPtr, aCopySz);
            MK_Mem_Free(aPtr);
        }
        return lPtr;
    #endif
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
