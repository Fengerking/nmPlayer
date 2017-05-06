#ifndef __MKBase_Sync_h__
#define __MKBase_Sync_h__

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
 * Atomically compare the value pointed to by aPtr to aCmp and perform a swap
 * operation depending on the outcome of the comparison. Returns the initial
 * value of *aPtr (before an eventual swap took place).
 */
MK_INLINE MK_PInt MK_Sync_CompareAndSwap(volatile MK_PInt* aPtr, MK_PInt aCVal, MK_PInt aSVal);

/*
 * Atomically add/subtract aVal to/from the value pointed to by aPtr and return
 * the initial value of *aPtr.
 */
MK_INLINE MK_PInt MK_Sync_FetchAndAdd(volatile MK_PInt* aPtr, MK_PInt aVal);
MK_INLINE MK_PInt MK_Sync_FetchAndSub(volatile MK_PInt* aPtr, MK_PInt aVal);

/*
 * Atomically add/subtract aVal to/from the value pointed to by aPtr and return
 * the resulting value of *aPtr.
 */
MK_INLINE MK_PInt MK_Sync_AddAndFetch(volatile MK_PInt* aPtr, MK_PInt aVal);
MK_INLINE MK_PInt MK_Sync_SubAndFetch(volatile MK_PInt* aPtr, MK_PInt aVal);

/******************************************************************************/

#define MK_SYNC_OWNID_INV _MK_SYNC_OWNID_INV

/******************************************************************************/

/*
 * Returns an id representing hte current thread.
 */
MK_INLINE MK_PInt MK_Sync_ThrId(void);

/*
 * Yield the rest of the current threads timeslice.
 */
MK_INLINE void MK_Sync_Yield(void);

/*
 * Sleep current thread for aMSec milliseconds.
 */
MK_INLINE void MK_Sync_Sleep(MK_U32 aMSec);

/******************************************************************************/

/*
 * Try to acquire spin lock with aOwnId as owner identifier. Returns MK_TRUE if
 * the lock was successfully acquired (or already owned) and MK_FALSE if the
 * lock could not be acquired in aMaxTries + 1 attempts. If the lock was already
 * owned by aOwnId the value of aOwnId is set to MK_SYNC_OWNID_INV.
 *
 * The Y-version makes yielding in the loop optional.
 *
 * NOTE: The resulting aOwnId value is what should be passed to RelLock().
 */
MK_INLINE MK_Bool MK_Sync_TryAcqLock(volatile MK_PInt* aLock, MK_PInt* aOwnId, MK_U32 aMaxTries);
MK_INLINE MK_Bool MK_Sync_TryAcqLockY(volatile MK_PInt* aLock, MK_PInt* aOwnId, MK_U32 aMaxTries, MK_Bool aYield);

/*
 * Aquire spin lock with aOwnId as owner identifier. Returns MK_SYNC_OWNID_INV
 * immediately if the lock is already held by the specified owner or spins until
 * the lock is aqcuired and returns aOwnId otherwise.
 *
 * The Y-version makes yielding in the loop optional.
 *
 * NOTE: The returned owner value is what should be passed to RelLock().
 */
MK_INLINE MK_PInt MK_Sync_AcqLock(volatile MK_PInt* aLock, MK_PInt aOwnId);
MK_INLINE MK_PInt MK_Sync_AcqLockY(volatile MK_PInt* aLock, MK_PInt aOwnId, MK_Bool aYield);

/*
 * Release lock held by aOwnId. If aOwnId is MK_SYNC_OWNID_INV this is
 * essientally a NOP. This allows for recursive locks using the return value of
 * AcqLock() as argument to RelLock().
 */
MK_INLINE void MK_Sync_RelLock(volatile MK_PInt* aLock, MK_PInt aOwnId);

/******************************************************************************/
/* IMPLEMENTATION DETAILS                                                     */
/******************************************************************************/

#if MK_WIN32
    #include <windows.h>
    #if MK_WINCE
       #include <kfuncs.h>
    #endif
#else
    #include <pthread.h>
    #include <unistd.h>
    #if MK_ANDROID
        #include <sys/atomics.h>
    #endif
#endif

/******************************************************************************/

#define _MK_SYNC_OWNID_INV -1

/******************************************************************************/

MK_INLINE MK_PInt MK_Sync_ThrId(void)
{
    #if MK_ENABLE_MTSAFE
        #if MK_WIN32
            MK_ASSERT(sizeof(MK_PInt) >= sizeof(DWORD));
            return MK_ToPInt(GetCurrentThreadId());
        #else
            pthread_t lId;
            MK_ASSERT(sizeof(MK_PInt) >= sizeof(pthread_t));
            return MK_ToPInt(lId = pthread_self());
        #endif
    #else
        return 0;
    #endif
}

MK_INLINE void MK_Sync_Yield(void)
{
    #if MK_ENABLE_MTSAFE
        #if MK_WINCE
            MK_Sync_Sleep(0);
        #elif MK_WIN32
            SwitchToThread();
        #else
            sched_yield();
        #endif
    #endif
}

MK_INLINE void MK_Sync_Sleep(MK_U32 aMSec)
{
    #if MK_WIN32
        Sleep(aMSec);
    #else
        usleep(aMSec * 1000U);
    #endif
}

/******************************************************************************/

MK_INLINE MK_PInt MK_Sync_CompareAndSwap(volatile MK_PInt* aPtr, MK_PInt aCVal, MK_PInt aSVal)
{
    #if MK_ENABLE_MTSAFE
        #if MK_WINCE && 4 == MK_PTRSIZE
                return InterlockedCompareExchange((LPLONG)aPtr, aSVal, aCVal);
        #elif MK_WIN32
            #if 8 == MK_PTRSIZE
                return InterlockedCompareExchange64(aPtr, aSVal, aCVal);
            #else
                return InterlockedCompareExchange(aPtr, aSVal, aCVal);
            #endif
        #elif MK_HAVE_BUILTIN_SYNC
            return __sync_val_compare_and_swap(aPtr, aCVal, aSVal);
        #elif __GNUC__ && MK_ARM && MK_ARM_ARCH >= 6 && !MK_ARM_THUMB && 4 == MK_PTRSIZE
            MK_PInt lVal, lFail;
            #if MK_ENABLE_MPSAFE
                #if MK_ARM_ARCH >= 7
                    __asm__ volatile("dmb\n" : : : "memory");
                #elif MK_ARM_ARCH >= 6
                    __asm__ volatile("mcr p15, 0, r0, c7, c10, 5 @ dmb\n" : : : "memory");
                #else
                    __asm__ volatile("" : : : "memory");
                #endif
            #endif
            do
            {
                __asm__ volatile(
                    " ldrex %1, [%2]\n"
                    " mov %0, #0\n"
                    " teq %1, %3\n"
                    " strexeq %0, %4, [%2]\n"
                    : "=&r" (lFail), "=&r" (lVal)
                    : "r" (aPtr), "Ir" (aCVal), "r" (aSVal)
                    : "cc"
                );
            }
            while (lFail);
            return lVal;
        #elif __GNUC__ && MK_X86
            MK_PInt lVal;
            #if 8 == MK_PTRSIZE
                __asm__ volatile(
                    " lock; cmpxchgq %1, %2\n"
                    : "=a" (lVal)
                    : "r" (aSVal), "m" (*aPtr), "0" (aCVal)
                    : "memory"
                );
            #else
                __asm__ volatile(
                    " lock; cmpxchgl %1, %2\n"
                    : "=a" (lVal)
                    : "r" (aSVal), "m" (*aPtr), "0" (aCVal)
                    : "memory"
                );
            #endif
            return lVal;
        #else
            #error MTSAVE version of MK_Sync_CompareAndSwap() unsupported for current platform/configuration
            MK_ASSERT(MK_FALSE);
            return 0;
        #endif
    #else
        MK_PInt lVal;
        if (aCVal == (lVal = *aPtr))
        {
            *aPtr = aSVal;
        }
        return lVal;
    #endif
}

MK_INLINE MK_PInt MK_Sync_FetchAndAdd(volatile MK_PInt* aPtr, MK_PInt aVal)
{
    #if MK_ENABLE_MTSAFE
        #if MK_WINCE && 4 == MK_PTRSIZE
                return InterlockedExchangeAdd((LPLONG)aPtr, aVal);
        #elif MK_WIN32
            #if 8 == MK_PTRSIZE
                return InterlockedExchangeAdd64(aPtr, aVal);
            #else
                return InterlockedExchangeAdd(aPtr, aVal);
            #endif
        #elif MK_HAVE_BUILTIN_SYNC
            return __sync_fetch_and_add(aPtr, aVal);
        #elif __GNUC__ && MK_X86
            MK_PInt lVal;
            #if 8 == MK_PTRSIZE
                __asm__ volatile(
                    " lock; xaddq %0, %1\n"
                    : "=r" (lVal), "=m" (*aPtr)
                    : "0" (aVal), "m" (*aPtr)
                );
            #else
                __asm__ volatile(
                    " lock; xaddl %0, %1\n"
                    : "=r" (lVal), "=m" (*aPtr)
                    : "0" (aVal), "m" (*aPtr)
                );
            #endif
            return lVal;
        #elif __GNUC__ && MK_ARM && MK_ARM_ARCH >= 6 && !MK_ARM_THUMB && 4 == MK_PTRSIZE
            MK_PInt lOVal, lNVal, lFail;
            #if MK_ENABLE_MPSAFE
                #if MK_ARM_ARCH >= 7
                    __asm__ volatile("dmb\n" : : : "memory");
                #elif MK_ARM_ARCH >= 6
                    __asm__ volatile("mcr p15, 0, r0, c7, c10, 5 @ dmb\n" : : : "memory");
                #else
                    __asm__ volatile("" : : : "memory");
                #endif
            #endif
            do
            {
                __asm__ volatile(
                    " ldrex %1, [%3]\n"
                    " add %2, %1, %4\n"
                    " strex %0, %2, [%3]\n"
                    : "=&r" (lFail), "=&r" (lOVal), "=&r" (lNVal)
                    : "r" (aPtr), "Ir" (aVal)
                    : "cc"
                );
            }
            while (lFail);
            return lOVal;
        #else
            MK_PInt lOVal, lNVal = *aPtr;
            do
            {
                lOVal = lNVal;
                lNVal += aVal;
            }
            while (lOVal != (lNVal = MK_Sync_CompareAndSwap(aPtr, lOVal, lNVal)));
            return lNVal;            
        #endif
    #else
        MK_PInt lVal = *aPtr;
        *aPtr = lVal + aVal;
        return lVal;
    #endif
}

MK_INLINE MK_PInt MK_Sync_FetchAndSub(volatile MK_PInt* aPtr, MK_PInt aVal)
{
    return MK_Sync_FetchAndAdd(aPtr, -aVal);
}

MK_INLINE MK_PInt MK_Sync_AddAndFetch(volatile MK_PInt* aPtr, MK_PInt aVal)
{
    #if MK_ENABLE_MTSAFE
        #if MK_HAVE_BUILTIN_SYNC
            return __sync_add_and_fetch(aPtr, aVal);
        #elif __GNUC__ && MK_ARM && MK_ARM_ARCH >= 6 && !MK_ARM_THUMB && 4 == MK_PTRSIZE
            MK_PInt lOVal, lNVal, lFail;
            #if MK_ENABLE_MPSAFE
                #if MK_ARM_ARCH >= 7
                    __asm__ volatile("dmb\n" : : : "memory");
                #elif MK_ARM_ARCH >= 6
                    __asm__ volatile("mcr p15, 0, r0, c7, c10, 5 @ dmb\n" : : : "memory");
                #else
                    __asm__ volatile("" : : : "memory");
                #endif
            #endif
            do
            {
                __asm__ volatile(
                    " ldrex %1, [%3]\n"
                    " add %2, %1, %4\n"
                    " strex %0, %2, [%3]\n"
                    : "=&r" (lFail), "=&r" (lOVal), "=&r" (lNVal)
                    : "r" (aPtr), "Ir" (aVal)
                    : "cc"
                );
            }
            while (lFail);
            return lNVal;
        #else
            return MK_Sync_FetchAndAdd(aPtr, aVal) + aVal;
        #endif
    #else
        return *aPtr += aVal;
    #endif
}

MK_INLINE MK_PInt MK_Sync_SubAndFetch(volatile MK_PInt* aPtr, MK_PInt aVal)
{
    return MK_Sync_AddAndFetch(aPtr, -aVal);
}

/******************************************************************************/

MK_INLINE MK_Bool MK_Sync_TryAcqLock(volatile MK_PInt* aLock, MK_PInt* aOwnId, MK_U32 aMaxTries)
{
    return MK_Sync_TryAcqLockY(aLock, aOwnId, aMaxTries, MK_TRUE);
}

MK_INLINE MK_Bool MK_Sync_TryAcqLockY(volatile MK_PInt* aLock, MK_PInt* aOwnId, MK_U32 aMaxTries, MK_Bool aYield)
{
    MK_PInt lOwnId, lLockId;
    MK_ASSERT(NULL != aOwnId && MK_SYNC_OWNID_INV != *aOwnId);

    /* Try to acquire lock & check for previous ownership */
    if (MK_SYNC_OWNID_INV == (lLockId = MK_Sync_CompareAndSwap(aLock, MK_SYNC_OWNID_INV, lOwnId = *aOwnId)))
    {
        /* Ownership acquired */
        return MK_TRUE;
    }
    else if (lOwnId == lLockId)
    {
        /* Already owned */
        *aOwnId = MK_SYNC_OWNID_INV;
        return MK_TRUE;
    }

    /* Spin for lock ownership or max spin termination */
    while (0 != aMaxTries--)
    {
        if (MK_SYNC_OWNID_INV == (lLockId = MK_Sync_CompareAndSwap(aLock, MK_SYNC_OWNID_INV, lOwnId)))
        {
            return MK_TRUE;
        }
        if (aYield) MK_Sync_Yield();
    }
    return MK_FALSE;
}

MK_INLINE MK_PInt MK_Sync_AcqLock(volatile MK_PInt* aLock, MK_PInt aOwnId)
{
    return MK_Sync_AcqLockY(aLock, aOwnId, MK_TRUE);
}

MK_INLINE MK_PInt MK_Sync_AcqLockY(volatile MK_PInt* aLock, MK_PInt aOwnId, MK_Bool aYield)
{
    MK_PInt lLockId;
    MK_ASSERT(MK_SYNC_OWNID_INV != aOwnId);

    /* Try to acquire lock & check for previous ownership */
    if (MK_SYNC_OWNID_INV == (lLockId = MK_Sync_CompareAndSwap(aLock, MK_SYNC_OWNID_INV, aOwnId)))
    {
        /* Ownership acquired */
        return aOwnId;
    }
    else if (aOwnId == lLockId)
    {
        /* Already owned */
        return MK_SYNC_OWNID_INV;
    }

    /* Spin until ownership is acquired */
    while (MK_SYNC_OWNID_INV != (lLockId = MK_Sync_CompareAndSwap(aLock, MK_SYNC_OWNID_INV, aOwnId)))
    {
        if (aYield) MK_Sync_Yield();
    }
    return aOwnId;
}

MK_INLINE void MK_Sync_RelLock(volatile MK_PInt* aLock, MK_PInt aOwnId)
{
    MK_PInt lOwnId;
    if (aOwnId != (lOwnId = MK_Sync_CompareAndSwap(aLock, aOwnId, MK_SYNC_OWNID_INV)))
    {
        MK_ASSERT(MK_SYNC_OWNID_INV == aOwnId);
    }
}

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
