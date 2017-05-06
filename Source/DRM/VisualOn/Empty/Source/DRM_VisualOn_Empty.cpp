#include "DRM_VisualOn_Empty.h"
#include "voLog.h"

CDRM_VisualOn_Empty::CDRM_VisualOn_Empty(void)
{
	VOLOGI("CDRM_VisualOn_Empty");
}

CDRM_VisualOn_Empty::~CDRM_VisualOn_Empty(void)
{
}

VO_U32 CDRM_VisualOn_Empty::Uninit()
{
	VOLOGI("Uninit");

	return VO_ERR_NONE;
}


VO_U32 CDRM_VisualOn_Empty::OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat)
{
	VOLOGI("OnSourceDrm");

	return VO_ERR_NONE;
}
