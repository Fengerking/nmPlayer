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

#ifndef __voVASTParser_H__
#define __voVASTParser_H__

#include "voAdFormat.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
* Parameter ID
*/
#define	VO_PID_VAST_BASE						0x45000000

/**
 * Parser function set
 */
typedef struct
{
	/**
	 * Open the parser and return handle
	 * \param ppHandle [OUT] Return the parser operator handle
	 * \retval VO_RET_SOURCE2_OK Succeeded.
	 */
	VO_U32 (VO_API * Open) (VO_PTR * ppHandle, VO_TCHAR *pWorkPath);

	/**
	 * Close the opened source.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \retval VO_RET_SOURCE2_OK Succeeded.
	 */
	VO_U32 (VO_API * Close) (VO_PTR pHandle);

	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pBuffer [IN] The buffer to be parsed
	* \param pBuffer [IN] The buffer size
	* \retval VO_RET_SOURCE2_OK Succeeded.
	*/
	VO_U32 (VO_API * Process) (VO_PTR pHandle, VO_PTR pBuffer, VO_U32 uSize);

	/**
	* Parse the buffer.
	* \param pHandle [IN] The handle which was create by open function.
	* \param pData [IN]/[OUT] The buffer will be filled by parser
	* \retval VO_RET_SOURCE2_OK Succeeded.
	*/
	VO_U32 (VO_API * GetData) (VO_PTR pHandle, VOAdInfo* pData);

	/**
	 * Set param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_RET_SOURCE2_OK Succeeded. 
	 */
	VO_U32 (VO_API * SetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);

	/**
	 * Get param for special target.
	 * \param pHandle [IN] The handle which was create by open function.
	 * \param uID [IN] The param ID.
	 * \param pParam [IN] The param value depend on the ID>
	 * \retval VO_RET_SOURCE2_OK Succeeded.
	 */
	VO_U32 (VO_API * GetParam) (VO_PTR pHandle, VO_U32 uID, VO_PTR pParam);
} VO_VAST_PARSER_API;

/**
* Get parser API interface
* \param pParser [out] return VMAP parser API handle.
* \retval VO_RET_SOURCE2_OK Succeeded.
*/
VO_S32 VO_API voGetVASTParserAPI(VO_VAST_PARSER_API * pParser);

typedef VO_S32 (* VOGETVASTPARSERAPI) (VO_VAST_PARSER_API * pParser);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __voVASTParser_H__
