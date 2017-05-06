/** 
 * \file drabase.h
 * \brief VisualOn DRA ID & sturcture, for SetParameter/GetParameter
 * \version 0.1
 * \date 10/31/2006 created
 * \author VisualOn
 */


#ifndef __VO_DRA_BASE_H_
#define __VO_DRA_BASE_H_

#include "sdkbase.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#pragma pack(push, 4)

/**
 * DRA Decoder specific parameter id 
 * \see VOCOMMONPARAMETERID
 */
typedef enum
{
	VOID_DRA_OBJECTTYPE 		= VOID_DRA_BASE | 0x0001,  /*!< Audio Object Type ID of MPEG4 audio. */
}
VODRAPARAMETERID;


#pragma pack(pop)
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* __VO_DRA_BASE_H_ */


