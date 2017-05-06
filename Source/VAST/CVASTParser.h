/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
#ifndef _CVASTParser_H_
#define _CVASTParser_H_

#include "voSource2.h"
#include "voXMLLoad.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

VO_U32 DecodeCharacterEntities(VO_PCHAR pstrInput);

////<class define
class CVASTParser:public CXMLLoad
{

	typedef VO_U32 (CVASTParser::*pHandleData)(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);

public:
	CVASTParser();
	virtual ~CVASTParser();
public:
	virtual VO_U32					Open(VO_TCHAR *pWorkPath);
	virtual	VO_U32					Close();
	virtual	VO_U32					Process(VO_PTR pBuffer, VO_U32 uSize);
	virtual VO_U32					GetData(VOAdInfo* pData);
	virtual VO_U32					SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32					GetParam(VO_U32 uID, VO_PTR pParam);
public:
	///<if ues this interface,the buffer be deleted by outside
	virtual	VO_U32					ProcessByTag(VO_VOID *pAdDataNode);
	VOAdData*						GetVastData(){return &m_VastData;}
	VO_U32							ReleaseVASTData(pVOAdData pAdData);
protected:
	VO_U32							LoadTag(VO_PTR pNode, VO_PTR pData ,pHandleData pHandle);

	VO_U32							LoadAdAttri(VO_PTR pNode, pVOAdData pData);
	VO_U32							LoadCreativeAttri(VO_PTR pNode, pVOAdCreative pData);
	VO_U32							LoadLinearAttri(VO_PTR pNode, pVOVastLinear pData){return VO_RET_SOURCE2_OK;}
	VO_U32							LoadNonLinearAttri(VO_PTR pNode, pVOVastNonLinear pData){return VO_RET_SOURCE2_OK;}
	VO_U32							LoadCompanioAttri(VO_PTR pNode, pVOVastCompanionAds pData){return VO_RET_SOURCE2_OK;}
	VO_U32							LoadIconAttri(VO_PTR pNode, pVOAdIcon pData){return VO_RET_SOURCE2_OK;}
	VO_U32							LoadMediaFileAttri(VO_VOID *pNode, pVOAdMediaFile pData);
	VO_U32							LoadTrackingAttri(VO_VOID *pNode, pVOVastTrackingEvent pData);

	VO_U32							HandleAdData(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleInLine(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleCreatives(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleCreative(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleLinear(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleNonLinear(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize){return VO_RET_SOURCE2_OK;}
	VO_U32							HandleCompanion(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize){return VO_RET_SOURCE2_OK;}
	VO_U32							HandleMediaFiles(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleVideoClicks(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleAdParameter(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize){return VO_RET_SOURCE2_OK;}
	VO_U32							HandleIcons(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleTrackingEvents(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);

	VO_U32							ReleaseLiner(pVOVastLinear pData);
	VO_U32							ReleaseNonLiner(pVOVastNonLinear pData){return VO_RET_SOURCE2_OK;}
	VO_U32							ReleaseCompanion(pVOVastCompanionAds pData){return VO_RET_SOURCE2_OK;}
private:
	VOAdData						m_VastData;
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CVASTParser_H_
