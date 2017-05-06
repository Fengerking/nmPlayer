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


#ifndef __voLcsInfo_H__
#define __voLcsInfo_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "voLcsInfo.h"
#include "voString.h"

/**
 * GUID structure...
 */
typedef struct _VO_LICENSE_FILE_INFO {
	VO_TCHAR *	pFile;		// [In] file name
	char *		pProp;		// [out] prop text
	int 		nLevel;		// [out] trust level
	int			nItems;		// module items
	int *		pID;		// module ID
	int			nYear;		// Year
	int			nMonth;		// Month
	int			nDay;		// Day
} VO_LICENSE_FILE_INFO;

/**
 * return license file info
 * \param pLcsInfo [in] license info.
 * \retval VO_ERR_NONE Succeeded.
 */
VO_U32 VO_API voGetLcsFileInfo (VO_LICENSE_FILE_INFO * pLcsInfo);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voLcsInfo_H__
