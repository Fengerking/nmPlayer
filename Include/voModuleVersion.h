/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

#ifndef __VO_MODULE_VERSION_H__
#define __VO_MODULE_VERSION_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voIndex.h"
    
/**
 * Structure of module version info
 */
typedef struct
{
    int     nModuleID;      /*!< [in]Indicator module ID,refer to module ID defined in voIndex.h */
    char*   pszVersion;     /*!< [out]Output the version information */
}VO_MODULE_VERSION;
    

/**
 * Get module version information
 * \param pVersion [in/out] Return the version information according the input module ID
 * \retval VO_ERR_NONE Succeeded.
 */
VO_S32 voGetModuleVersion(VO_MODULE_VERSION* pVersion);
typedef VO_S32 (* VOGETMODULEVERSION)(VO_MODULE_VERSION* pVersion);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __VO_MODULE_VERSION_H__
