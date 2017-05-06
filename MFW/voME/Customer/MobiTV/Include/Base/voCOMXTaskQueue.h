	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2009				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXTaskQueue.h

	Contains:	voCOMXTaskQueue header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-04		JBF			Create file

*******************************************************************************/
#ifndef __voCOMXTaskQueue_H__
#define __voCOMXTaskQueue_H__

#include <string.h>
#include <OMX_Types.h>

#include "voCOMXThreadMutex.h"
#include "voCOMXBaseObject.h"

#define VOMAX_QUEUE_TASKS		12

struct voMAX_TASK_ITEM
{
  OMX_PTR 			m_pTask;
  voMAX_TASK_ITEM *	m_pNext;
};

// wrapper for whatever critical section we have
class voCOMXTaskQueue : public voCOMXBaseObject
{
public:
    voCOMXTaskQueue(void);
    virtual ~voCOMXTaskQueue(void);

	virtual void	Add (OMX_PTR pTask);

	virtual OMX_PTR	Remove (void);
	virtual OMX_PTR	First (void);

	virtual OMX_U32	Count (void);


protected:
	voMAX_TASK_ITEM *	m_pFirst;
	voMAX_TASK_ITEM *	m_pLast;
	OMX_U32				m_uItems;
	voCOMXThreadMutex	m_tm;
};


#endif //__voCOMXTaskQueue_H__
