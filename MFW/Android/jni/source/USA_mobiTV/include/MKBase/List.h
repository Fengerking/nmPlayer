#ifndef __MKBase_List_h__
#define __MKBase_List_h__

#include <MKBase/Platform.h>
#include <MKBase/Assert.h>
#include <MKBase/Type.h>

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/

/*
 * Define MK_ListNode structure.
 */
typedef struct _MK_ListNode MK_ListNode;

struct _MK_ListNode
{
    MK_ListNode* Prev;
    MK_ListNode* Next;
};

/******************************************************************************/

/*
 * Defines for default initialization. May be used instead of init function when
 * appropriate.
 */
#define MK_LISTNODE_INITVAL {NULL, NULL}
#define MK_LISTNODE_INIT MK_LISTNODE_INITVAL

/*
 * Initialize the basic ListNode structure.
 */
MK_INLINE void MK_ListNode_Init(MK_ListNode* aNod)
{
    MK_ASSERT(NULL != aNod);
    {
        MK_ListNode lVal = MK_LISTNODE_INIT;
        *aNod = lVal;
    }
}

/******************************************************************************/

/*
 * These are helper functions used internally by List and should not be used
 * directly in most cases.
 */

MK_INLINE MK_ListNode* MK_ListNode_Prev(const MK_ListNode* aAt)
{
    MK_ASSERT(NULL != aAt);
    return aAt->Prev;
}

MK_INLINE MK_ListNode* MK_ListNode_Next(const MK_ListNode* aAt)
{
    MK_ASSERT(NULL != aAt);
    return aAt->Next;
}

MK_INLINE void MK_ListNode_PushBefore(MK_ListNode* aAt, MK_ListNode* aWith)
{
    MK_ASSERT(NULL != aAt && NULL != aWith);
    aWith->Prev = aAt->Prev;
    aWith->Next = aAt;
    aAt->Prev->Next = aWith;
    aAt->Prev = aWith;
}

MK_INLINE void MK_ListNode_PushAfter(MK_ListNode* aAt, MK_ListNode* aWith)
{
    MK_ASSERT(NULL != aAt && NULL != aWith);
    aWith->Prev = aAt;
    aWith->Next = aAt->Next;
    aAt->Next->Prev = aWith;
    aAt->Next = aWith;
}

MK_INLINE MK_ListNode* MK_ListNode_Pop(MK_ListNode* aAt)
{
    MK_ASSERT(NULL != aAt);
    aAt->Prev->Next = aAt->Next;
    aAt->Next->Prev = aAt->Prev;
    aAt->Prev = aAt->Next = NULL;
    return aAt;
}

/******************************************************************************/

/*
 * Define MK_List structure.
 */
typedef struct _MK_List MK_List;

struct _MK_List
{
    MK_ListNode Node;
};

/******************************************************************************/

/*
 * Initialize the basic List structure.
 */
MK_INLINE void MK_List_Init(MK_List* aLst)
{
    MK_ASSERT(NULL != aLst);
    aLst->Node.Prev = aLst->Node.Next = &aLst->Node;
}

/******************************************************************************/

/*
 * Return a pointer to the first node of aLst, or NULL if there is none.
 */
MK_INLINE MK_ListNode* MK_List_First(const MK_List* aLst)
{
    MK_ASSERT(NULL != aLst);
    return (&aLst->Node != aLst->Node.Next) ? aLst->Node.Next : NULL;
}

/*
 * Return a pointer to the last node of aLst, or NULL if there is none.
 */
MK_INLINE MK_ListNode* MK_List_Last(const MK_List* aLst)
{
    MK_ASSERT(NULL != aLst);
    return (&aLst->Node != aLst->Node.Prev) ? aLst->Node.Prev : NULL;
}

/*
 * Return a pointer to the node preceding aAt in aLst, or NULL if we are already
 * at the start of the list.
 */
MK_INLINE MK_ListNode* MK_List_Prev(const MK_List* aLst, const MK_ListNode* aAt)
{
    MK_ASSERT(NULL != aLst);
    {
        MK_ListNode* lPrev = MK_ListNode_Prev(aAt);
        return (&aLst->Node != lPrev) ? lPrev : NULL;
    }
}

/*
 * Return a pointer to the node succeeding aAt in aLst, or NULL if we are
 * already at the end of the list.
 */
MK_INLINE MK_ListNode* MK_List_Next(const MK_List* aLst, const MK_ListNode* aAt)
{
    MK_ASSERT(NULL != aLst);
    {
        MK_ListNode* lNext = MK_ListNode_Next(aAt);
        return (&aLst->Node != lNext) ? lNext : NULL;
    }
}

/*
 * Return a pointer to the node at index aIdx in aLst, or NULL if there is no
 * such index.
 */
MK_INLINE MK_ListNode* MK_List_Index(const MK_List* aLst, MK_U32 aIdx)
{
    const MK_ListNode* lNext;
    MK_U32 i;
    MK_ASSERT(NULL != aLst);
    for (lNext = &aLst->Node, i = 0; &aLst->Node != (lNext = MK_ListNode_Next(lNext)) && i < aIdx; ++i);
    return (&aLst->Node != lNext) ? MK_Cast(MK_ListNode*, lNext) : NULL;
}

/*
 * Push the node aWith to the first position of aLst.
 */
MK_INLINE void MK_List_PushFirst(MK_List* aLst, MK_ListNode* aWith)
{
    MK_ASSERT(NULL != aLst);
    MK_ListNode_PushAfter(&aLst->Node, aWith);
}

/*
 * Push the node aWith to the last position of aLst.
 */
MK_INLINE void MK_List_PushLast(MK_List* aLst, MK_ListNode* aWith)
{
    MK_ASSERT(NULL != aLst);
    MK_ListNode_PushBefore(&aLst->Node, aWith);
}

/*
 * Push the node aWith before the node aAt in aLst.
 */
MK_INLINE void MK_List_PushBefore(MK_List* aLst, MK_ListNode* aAt, MK_ListNode* aWith)
{
    MK_UNUSED(aLst);
    MK_ListNode_PushBefore(aAt, aWith);
}

/*
 * Push the node aWith after the node aAt in aLst.
 */
MK_INLINE void MK_List_PushAfter(MK_List* aLst, MK_ListNode* aAt, MK_ListNode* aWith)
{
    MK_UNUSED(aLst);
    MK_ListNode_PushAfter(aAt, aWith);
}

/*
 * Pop the first node off aLst and return a pointer to it or NULL if aLst is
 * empty.
 */
MK_INLINE MK_ListNode* MK_List_PopFirst(MK_List* aLst)
{
    MK_ASSERT(NULL != aLst);
    return (&aLst->Node != aLst->Node.Next) ? MK_ListNode_Pop(aLst->Node.Next) : NULL;
}

/*
 * Pop the last node off aLst and return a pointer to it or NULL if aLst is
 * empty.
 */
MK_INLINE MK_ListNode* MK_List_PopLast(MK_List* aLst)
{
    MK_ASSERT(NULL != aLst);
    return (&aLst->Node != aLst->Node.Prev) ? MK_ListNode_Pop(aLst->Node.Prev) : NULL;
}

/*
 * Pop the node aAt off aLst and return a pointer to it.
 */
MK_INLINE MK_ListNode* MK_List_Pop(MK_List* aLst, MK_ListNode* aAt)
{
    MK_UNUSED(aLst);
    return MK_ListNode_Pop(aAt);
}

/*
 * Pop the node at index aIdx off aLst and return a pointer to it or NULL if
 * there is no such index.
 */
MK_INLINE MK_ListNode* MK_List_PopIndex(MK_List* aLst, MK_U32 aIdx)
{
    MK_ListNode* lAt;
    if (NULL != (lAt = MK_List_Index(aLst, aIdx)))
    {
        lAt = MK_List_Pop(aLst, lAt);
    }
    return lAt;
}

/*
 * Returns TRUE if aLst is empty, otherwise FALSE.
 */
MK_INLINE MK_Bool MK_List_IsEmpty(const MK_List* aLst)
{
    MK_ASSERT(NULL != aLst);
    return (&aLst->Node == aLst->Node.Prev);
}

/*
 * Returns the number of nodes currently in aLst.
 */
MK_INLINE MK_U32 MK_List_Count(const MK_List* aLst)
{
    const MK_ListNode* lNext;
    MK_U32 i;
    MK_ASSERT(NULL != aLst);
    for (lNext = &aLst->Node, i = 0; &aLst->Node != (lNext = MK_ListNode_Next(lNext)); ++i);
    return i;
}

/******************************************************************************/

/*
 * Macro to help define the basics for new List implementations. The macro
 * defines wraper functions for all MK_List functions and the MK_ListNode init
 * function.
 *
 * aFName = The name of the new list type. All functions will have this value
 *          included in their names.
 * aDType = The desired data type (native or struct type).
 * aDName = The desired name of the data field in the ListNode structure.
 * aDVal  = The default value of the data field when doing an init.
 */
#define MK_NEW_LIST(aFName, aDType, aDName, aDVal) \
\
typedef struct _MK_ListNode_##aFName MK_ListNode_##aFName; \
\
struct _MK_ListNode_##aFName \
{ \
    MK_ListNode Node; \
    aDType aDName; \
}; \
\
MK_INLINE void MK_ListNode_##aFName##_Init(MK_ListNode_##aFName* aNod) \
{ \
    MK_ASSERT(NULL != aNod); \
    { \
        MK_ListNode_##aFName lVal = aDVal; \
        *aNod = lVal; \
    } \
} \
\
MK_INLINE void MK_List_##aFName##_Init(MK_List* aLst) \
{ \
    MK_List_Init(aLst); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_First(const MK_List* aLst) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_First(aLst)); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_Last(const MK_List* aLst) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_Last(aLst)); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_Prev(const MK_List* aLst, const MK_ListNode_##aFName* aAt) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_Prev(aLst, MK_Cast(const MK_ListNode*, aAt))); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_Next(const MK_List* aLst, const MK_ListNode_##aFName* aAt) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_Next(aLst, MK_Cast(const MK_ListNode*, aAt))); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_Index(const MK_List* aLst, MK_U32 aIdx) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_Index(aLst, aIdx)); \
} \
\
MK_INLINE void MK_List_##aFName##_PushFirst(MK_List* aLst, MK_ListNode_##aFName* aWith) \
{ \
    MK_List_PushFirst(aLst, MK_Cast(MK_ListNode*, aWith)); \
} \
\
MK_INLINE void MK_List_##aFName##_PushLast(MK_List* aLst, MK_ListNode_##aFName* aWith) \
{ \
    MK_List_PushLast(aLst, MK_Cast(MK_ListNode*, aWith)); \
} \
\
MK_INLINE void MK_List_##aFName##_PushBefore(MK_List* aLst, MK_ListNode_##aFName* aAt, MK_ListNode_##aFName* aWith) \
{ \
    MK_List_PushBefore(aLst, MK_Cast(MK_ListNode*, aAt), MK_Cast(MK_ListNode*, aWith)); \
} \
\
MK_INLINE void MK_List_##aFName##_PushAfter(MK_List* aLst, MK_ListNode_##aFName* aAt, MK_ListNode_##aFName* aWith) \
{ \
    MK_List_PushAfter(aLst, MK_Cast(MK_ListNode*, aAt), MK_Cast(MK_ListNode*, aWith)); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_PopFirst(MK_List* aLst) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_PopFirst(aLst)); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_PopLast(MK_List* aLst) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_PopLast(aLst)); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_Pop(MK_List* aLst, MK_ListNode_##aFName* aAt) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_Pop(aLst, MK_Cast(MK_ListNode*, aAt))); \
} \
\
MK_INLINE MK_ListNode_##aFName* MK_List_##aFName##_PopIndex(MK_List* aLst, MK_U32 aIdx) \
{ \
    return MK_Cast(MK_ListNode_##aFName*, MK_List_PopIndex(aLst, aIdx)); \
} \
\
MK_INLINE MK_Bool MK_List_##aFName##_IsEmpty(const MK_List* aLst) \
{ \
    return MK_List_IsEmpty(aLst); \
} \
\
MK_INLINE MK_U32 MK_List_##aFName##_Count(const MK_List* aLst) \
{ \
    return MK_List_Count(aLst); \
}

/******************************************************************************/

/*
 * Defines for default Ptr ListNode initialization. May be used instead of init
 * function when appropriate.
 */
#define MK_LISTNODE_PTR_INITVAL NULL
#define MK_LISTNODE_PTR_INIT {MK_LISTNODE_INITVAL, MK_LISTNODE_PTR_INITVAL}

/*
 * Define Ptr list type basics.
 */
MK_NEW_LIST(Ptr, void*, Ptr, MK_LISTNODE_PTR_INIT);

/******************************************************************************/

/*
 * Defines for default CStr ListNode initialization. May be used instead of init
 * function when appropriate.
 */
#define MK_LISTNODE_CSTR_INITVAL NULL
#define MK_LISTNODE_CSTR_INIT {MK_LISTNODE_INITVAL, MK_LISTNODE_CSTR_INITVAL}

/*
 * Define CStr list type basics.
 */
MK_NEW_LIST(CStr, MK_Char*, CStr, MK_LISTNODE_CSTR_INIT);

/******************************************************************************/

/*
 * Define MK_CStrKV structure.
 */
typedef struct _MK_CStrKV MK_CStrKV;

struct _MK_CStrKV
{
    MK_Char* Key;
    MK_Char* Val;
};

/*
 * Defines for default CStrKV ListNode initialization. May be used instead of
 * init function when appropriate.
 */
#define MK_LISTNODE_CSTRKV_INITVAL {NULL, NULL}
#define MK_LISTNODE_CSTRKV_INIT {MK_LISTNODE_INITVAL, MK_LISTNODE_CSTRKV_INITVAL}

/*
 * Define CStrKV list type basics.
 */
MK_NEW_LIST(CStrKV, MK_CStrKV, KV, MK_LISTNODE_CSTRKV_INIT);

/******************************************************************************/

/*
* Defines for default MK_U32 ListNode initialization. May be used instead of init
* function when appropriate.
*/
#define MK_LISTNODE_U32_INITVAL 0
#define MK_LISTNODE_U32_INIT {MK_LISTNODE_INITVAL, MK_LISTNODE_U32_INITVAL}

/*
* Define MK_U32 list type basics.
*/
MK_NEW_LIST(U32, MK_U32, Val, MK_LISTNODE_U32_INIT);

/******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif
