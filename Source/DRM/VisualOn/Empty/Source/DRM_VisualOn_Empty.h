#pragma once
#ifndef __CDRM_VISUALON_EMPTY_H__
#define __CDRM_VISUALON_EMPTY_H__
#include "DRM.h"

class CDRM_VisualOn_Empty :
	public CDRM
{
public:
	CDRM_VisualOn_Empty(void);
	virtual ~CDRM_VisualOn_Empty(void);

	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);
};

#endif //__CDRM_VISUALON_EMPTY_H__