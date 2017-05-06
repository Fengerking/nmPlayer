	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXCfgComp.h

	Contains:	voCOMXCfgComp header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-03-10		JBF			Create file

*******************************************************************************/

#ifndef __voCOMXCfgComp_H__
#define __voCOMXCfgComp_H__

#include "voCOMXBaseConfig.h"

class voCOMXCfgComp : public voCOMXBaseConfig
{
public:
	voCOMXCfgComp(void);
	virtual ~voCOMXCfgComp(void);

	virtual	OMX_BOOL	Open (OMX_STRING pFile);
	virtual OMX_STRING	GetCodecCompNameByCoding (OMX_U32 nDomain, OMX_U32 nCodec, OMX_U32 nCoding,  OMX_U32 nIndex, OMX_U32 nParam1 = 0, OMX_U32 nParam2 = 0, OMX_U32 nParam3 = 0);
	virtual OMX_STRING	GetCodecCompNameByMIME (OMX_U32 nDomain, OMX_U32 nCodec, OMX_STRING pMIME, OMX_U32 nFourCC, OMX_U32 nIndex, OMX_U32 nParam1 = 0, OMX_U32 nParam2 = 0, OMX_U32 nParam3 = 0);

protected:
	virtual OMX_STRING	GetCompName (OMX_STRING pSectName, OMX_U32 nFourCC , OMX_U32 nIndex);
	virtual OMX_BOOL	GetSectName (OMX_STRING pSectName, OMX_U32 nDomain, OMX_U32 nCodec, OMX_U32 nCoding, OMX_U32 nParam1 = 0, OMX_U32 nParam2 = 0, OMX_U32 nParam3 = 0);

};

#endif // __voCOMXCfgComp_H__