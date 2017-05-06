	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXBaseConfig.h

	Contains:	voCOMXBaseConfig header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXBaseConfig_H__
#define __voCOMXBaseConfig_H__

#include <OMX_Types.h>
#include "voCOMXBaseObject.h"

#include "voOMXFile.h"
#include "voOMXMemory.h"

class COMXCfgSect
{
public:
	COMXCfgSect (void);
	virtual ~COMXCfgSect (void);

public:
	OMX_STRING		m_pName;
	OMX_PTR			m_pData;
	COMXCfgSect *	m_pNext;
};

class COMXCfgItem
{
public:
	COMXCfgItem (void);
	virtual ~COMXCfgItem (void);

public:
	COMXCfgSect *	m_pSection;
	OMX_STRING		m_pName;
	OMX_S32			m_nValue;
	OMX_STRING		m_pValue;

	COMXCfgItem *	m_pNext;
};

class voCOMXBaseConfig : public voCOMXBaseObject
{
public:
	voCOMXBaseConfig(void);
	virtual ~voCOMXBaseConfig(void);

	virtual	OMX_BOOL	Open (OMX_STRING pFile);
	virtual OMX_BOOL	Write (OMX_STRING pFile);

	virtual OMX_BOOL	AddSection (OMX_STRING pSection);
	virtual OMX_BOOL	RemoveSection (OMX_STRING pSection);

	virtual OMX_BOOL	AddItem (OMX_STRING pSection, OMX_STRING pName, OMX_S32 nValue);
	virtual OMX_BOOL	AddItem (OMX_STRING pSection, OMX_STRING pName, OMX_STRING pValue);

	virtual OMX_BOOL	RemoveItem (OMX_STRING pSection, OMX_STRING pName);

	virtual OMX_BOOL	UpdateItem (OMX_STRING pSection, OMX_STRING pName, OMX_S32 nValue);
	virtual OMX_BOOL	UpdateItem (OMX_STRING pSection, OMX_STRING pName, OMX_STRING pValue);

	virtual OMX_S32		GetItemValue (OMX_STRING pSection, OMX_STRING pName, OMX_S32 nDefault);
	virtual OMX_STRING	GetItemText (OMX_STRING pSection, OMX_STRING pName);

	virtual COMXCfgSect *	FindSect (OMX_STRING pSection);

	COMXCfgSect * GetFirstSect (void) {return m_pFirstSect;}
	COMXCfgItem * GetFirstItem (void) {return m_pFirstItem;}

protected:
	COMXCfgItem *		FindItem (OMX_STRING pSection, OMX_STRING pName);
	COMXCfgItem *		CreateItem (OMX_STRING pSection, OMX_STRING pName);

	virtual OMX_S8*	GetNextLine (OMX_S8 * pBuffer, OMX_S32 nBufSize, OMX_S8 * pNextLine, OMX_S32 & nLineSize);
	virtual	void	Release (void);

protected:
	OMX_STRING		m_pFileName;
	OMX_BOOL		m_bUpdated;

	COMXCfgSect *		m_pFirstSect;
	OMX_S32			m_nSectNum;

	COMXCfgItem *		m_pFirstItem;
	OMX_S32			m_nItemNum;
};

#endif // __voCOMXBaseConfig_H__

