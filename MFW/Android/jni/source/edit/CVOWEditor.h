	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CVOWEdit.h

	Contains:	CVOWEdit header file

	Written by:	YW (TOM)

	Change History (most recent first):
	2010-05-05		YW		Create file

*******************************************************************************/

#ifndef __CVOWEditor_H__
#define __CVOWEditor_H__

#include "voMMEdit.h"
#include "CVOWPlayer.h"
#include "voMMEdit.h"

class CVOWEditor : public CVOWPlayer
{
protected:
	VOMM_EDITAPI		m_fVOEditorAPI;

public:
	CVOWEditor (void);
	virtual ~CVOWEditor (void);	
	
	 virtual int	Init (void);	
	  virtual int	SetSource (const char* pSource);
	  virtual int     ExportFile(const char *url);

	  virtual int     GetExportPercent();

	  virtual int			Run (void);

protected:
	virtual int			LoadModule (void);
};

#endif // __CVOWEditor_H__
