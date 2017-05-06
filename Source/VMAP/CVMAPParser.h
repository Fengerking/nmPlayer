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
#ifndef _CVMAPParser_H_
#define _CVMAPParser_H_

#include "voSource2.h"
#include "voXMLLoad.h"
#include "voVASTParser.h"
#include "CVASTParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

class CVASTParser;
////<class define
class CVMAPParser:public CXMLLoad
{
	typedef VO_U32 (CVMAPParser::*pHandleData)(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
public:
	CVMAPParser();
	virtual ~CVMAPParser();
public:
	virtual VO_U32					Open(VO_TCHAR *pWorkPath);
	virtual	VO_U32					Close();
	VO_U32							Reset();
	virtual	VO_U32					Process(VO_PTR pBuffer, VO_U32 uSize);
	virtual VO_U32					GetData(VOAdInfo** ppData);
	virtual VO_U32					SetParam(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32					GetParam(VO_U32 uID, VO_PTR pParam);
	
protected:
	VO_U32							LoadTag(VO_PTR pNode, VO_PTR pData ,pHandleData pHandle);
	
	VO_U32							LoadAdbreakAttri(VO_VOID *pAdbreakNode, pVOAdBreak pAdBreak);
	VO_U32							LoadAdSourceAttri(VO_VOID *pAdSourceNode, pVOAdSource pAdSource);

	VO_U32							HandleVmapRoot(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleAdBreak(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleAdSource(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	VO_U32							HandleAdData(VO_PTR pNode, VO_PTR pData, VO_PCHAR strTage, VO_U32 uSize);
	
	VO_U32							LoadAdData(VO_VOID *pAdDataNode, pVOAdData pAdData);
	VO_U32							OpenVastParser();
	VO_U32							CloseVastParser();

	VO_U32							ReleaseVMAPData();
private:
	VO_PCHAR			m_pContentURI;
	VOAdInfo			m_VMAPData;
#if 0
	CDllLoad*			m_hDll;
	VO_VAST_PARSER_API	m_hVastAPI;
	VO_PTR				m_hVast;
	pVOVastAdData		m_pVastData;
#endif
	CVASTParser*		m_pVastParser;
//	VOAdInfo			m_AdInfo;
};

#ifdef _VONAMESPACE
}
#endif

#endif //_CVMAPParser_H_
