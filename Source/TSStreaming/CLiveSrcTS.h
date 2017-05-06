	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 ~				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CLiveSrcCTS.h

	Contains:	CLiveSrcCTS header file

	Written by:	Jeff Huang

	Change History (most recent first):
	2011-07-22		hjf			Create file

*******************************************************************************/
#ifndef __CLiveSrcCTS_H__
#define __CLiveSrcCTS_H__

#include "CLiveSrcBase.h"

class CLiveSrcTS : public CLiveSrcBase
{
public:
	CLiveSrcTS(void);
	virtual ~CLiveSrcTS(void);

	virtual	VO_U32 Stop();
	virtual	VO_U32 SetParam(VO_U32 nParamID, VO_PTR pValue);

protected:
	virtual VO_U32 doInitParser();
	virtual VO_U32 doOpenDevice();

	
protected:
	VO_BOOL doOnParsedData(VO_PARSER_OUTPUT_BUFFER * pData);
	
private:
	VO_U32	m_nDiscontinuousCount;
	VO_U32	m_nLossPackageCount;
};

#endif //__CLiveSrcCTS_H__
