
//value indicate for DATA SOURCE TEAM 
#ifndef __VO_DSDRM_H__
#define __VO_DSDRM_H__

#include "voDSType.h"

#define VO_DATASOURCE_PID_DRM_FP_CB			(VO_DATASOURCE_PID_DRM_BASE| 0x0001) /*indicate the callback for fileparser, VO_DATASOURCE_FILEPARSER_DRMCB* */

typedef VO_U32 (*DATASOURCE_DataProcess_FR_CB)(VO_PTR pUserData, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData, VO_U32 *pnDesSize, VO_DRM2_DATATYPE eDataType, VO_DRM2_DATAINFO_TYPE eDataInfoType,VO_PTR pAdditionalInfo);
typedef struct
{
	VO_PTR pUserData;
	DATASOURCE_DataProcess_FR_CB FRCB;
}VO_DATASOURCE_FILEPARSER_DRMCB;


#endif