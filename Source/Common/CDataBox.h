	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2012			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CDataBox.h

	Contains:	CDataBox head file

	Written by:	Aiven

	Change History (most recent first):
	2013-04-2		Aiven			Create file

*******************************************************************************/

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif
#include "voYYDef_SrcCmn.h"
#include "voSource2.h"

typedef struct
{
//callback instance
	VO_PTR pUserData;

/**
 * Callback function. The source will send the data out..
 * \param pUserData [in] The user data which was set by Open().
 * \param pData [in] The pData type is VO_PBYTE
 * \param nSize [in] The nSize is VO_U32
 *                   the param type is depended on the nOutputType, for VO_SOURCE2_TT_VIDEO & VO_SOURCE2_TT_AUDIO please use VO_SOURCE2_SAMPLE
 *                                                                  for VO_SOURCE2_TT_SUBTITLE please use voSubtitleInfo
 */
	VO_U32 (VO_API * MallocData) (VO_PTR pUserData, VO_PBYTE* pData, VO_U32 nSize);

}VO_DATABOX_CALLBACK;

class CDataBox
{
public:
	CDataBox();
	~CDataBox();
	
	static VO_U32 MallocData(VO_PTR pUserData, VO_PBYTE* ppData, VO_U32 nSize);
	VO_U32 GetDataAndSize(VO_PBYTE* ppData, VO_U32** ppUsedDataSize, VO_U32* pDataSize = NULL);
public:
	VO_PBYTE	m_pData;
	VO_U32		m_uUsedDataSize;
	VO_U32		m_uDataSize;	
};

#ifdef _VONAMESPACE
}
#endif

