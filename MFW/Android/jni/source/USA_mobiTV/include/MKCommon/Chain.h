/*
 * Defines the node struct used in chain of components.
 */

#ifndef __MKBase_Chain_h__
#define __MKBase_Chain_h__

#include <MKBase/Platform.h>
#include <MKBase/Type.h>
#include <MKBase/Time.h>
#include <MKCommon/Sample.h>
#include <MKCommon/State.h>

#ifdef __cplusplus
extern "C"
{
#endif

/************************************************************************/

typedef struct _MK_Node MK_Node;

/************************************************************************/

/*
 * Callback function for sending data to node, aNode.
 * aData and aSize specifies the pointer to the data 
 * and the size of the data respectively.
 */
typedef MK_Result (*MK_Node_DataFunc)(MK_Node* aNode, void* aData, MK_U32 aSize);

/*
 * Callback function for deallocating node (and its internal structures), 
 * aNode, and the node downstream from it.
 */
typedef void (*MK_Node_FreeFunc) (MK_Node* aNode);

/*
 * Callback function for resetting node, aNode, and the
 * node downstream from it when seeking.
 */
typedef MK_Result (*MK_Node_SeekFunc) (MK_Node* aNode, MK_U16 aSeqBase, MK_U32 aTimeBase, MK_Time aStartTime, MK_Time aEndTime);

/*
 * Callback function for checking if aNode or one of
 * the node downstream from it considers that data flow
 * has ended.
 */
typedef MK_Bool (*MK_Node_IsEndedFunc) (MK_Node* aNode);

/************************************************************************/

typedef struct _MK_Node
{
    MK_Node_DataFunc DataFunc;
    MK_Node_FreeFunc FreeFunc;
    MK_Node_SeekFunc SeekFunc;
    MK_Node_IsEndedFunc IsEndedFunc;

    MK_Node*        mUp;
    MK_Node*        mDown;
    void*           mObject;
} _MK_Node;

/************************************************************************/

#ifdef __cplusplus
}
#endif


#endif
