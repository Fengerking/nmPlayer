	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseConfig.h

	Contains:	CBaseConfig header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __CBaseConfig_H__
#define __CBaseConfig_H__

#include "voYYDef_Common.h"
#include "voType.h"
#include "voIndex.h"
#include "voString.h"

#include "CvoBaseObject.h"

#include "cmnFile.h"
#include "cmnMemory.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CCfgSect
{
public:
	CCfgSect (void);
	virtual ~CCfgSect (void);

public:
	VO_PCHAR		m_pName;
	VO_PTR			m_pData;
	CCfgSect *		m_pNext;
};

class CCfgItem
{
public:
	CCfgItem (void);
	virtual ~CCfgItem (void);

public:
	CCfgSect *		m_pSection;
	VO_PCHAR		m_pName;
	VO_S32			m_nValue;
	VO_PCHAR		m_pValue;

	CCfgItem *		m_pNext;
};

class CBaseConfig : public CvoBaseObject
{
public:
	CBaseConfig(void);
	virtual ~CBaseConfig(void);

	virtual	VO_BOOL	Open (VO_PTCHAR pFile, VO_FILE_OPERATOR * pFileOP = NULL);
	virtual VO_BOOL	Write (VO_PTCHAR pFile);

	virtual VO_BOOL	AddSection (VO_PCHAR pSection);
	virtual VO_BOOL	RemoveSection (VO_PCHAR pSection);

	virtual VO_BOOL	AddItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_S32 nValue);
	virtual VO_BOOL	AddItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_PCHAR pValue);

	virtual VO_BOOL	RemoveItem (VO_PCHAR pSection, VO_PCHAR pItemName);

	virtual VO_BOOL	UpdateItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_S32 nValue);
	virtual VO_BOOL	UpdateItem (VO_PCHAR pSection, VO_PCHAR pItemName, VO_PCHAR pValue);

	virtual VO_S32		GetItemValue (const char* pSection, const char* pItemName, VO_S32 nDefault);
	virtual VO_PCHAR	GetItemText (const char* pSection, const char* pItemName, const char* pDefault = NULL);

	virtual CCfgSect *	FindSect (VO_PCHAR pSection);

	CCfgSect * GetFirstSect (void) {return m_pFirstSect;}
	CCfgItem * GetFirstItem (void) {return m_pFirstItem;}

protected:
	CCfgItem *		FindItem (const char* pSection, const char* pItemName);
	CCfgItem *		CreateItem (VO_PCHAR pSection, VO_PCHAR pItemName);

	virtual VO_S8*	GetNextLine (VO_S8 * pBuffer, VO_S32 nBufSize, VO_S8 * pLineText, VO_S32 & nLineSize);
	virtual	void	Release (void);

protected:
	VO_PTCHAR		m_pFileName;
	VO_BOOL			m_bUpdated;
	VO_MEM_INFO		m_memInfo;

	CCfgSect *		m_pFirstSect;
	VO_S32			m_nSectNum;

	CCfgItem *		m_pFirstItem;
	VO_S32			m_nItemNum;

	char			m_szLineText[256];
	char			m_szDefaultValue[256];
};
    
#ifdef _VONAMESPACE
}
#endif

#endif // __CBaseConfig_H__
