#ifndef __RTPSOURCE_H__
#define __RTPSOURCE_H__

#include "vortpsrc.h"
#include "commonheader.h"

class CSDPParse;
class CSourceEngine;

class CRTPSource
{
	friend class CSourceEngine;

public:
	CRTPSource();
	~CRTPSource();

public:
	VORC_RTPSRC Open(char * pSDPData, VO_RTPSRC_INIT_INFO * pInitInfo);
	VORC_RTPSRC Close();
	VORC_RTPSRC Start();
	VORC_RTPSRC Stop();
	VORC_RTPSRC SetParam(VO_U32 uID, VO_PTR pParam);
	VORC_RTPSRC GetParam(VO_U32 uID, VO_PTR pParam);
	VORC_RTPSRC SendData(VO_PBYTE pData, VO_U32 nSize);

protected:
	VORC_RTPSRC SendSDPData();

private:
	VO_RTPSRC_INIT_INFO   m_InitInfo;
	CSourceEngine       * m_pSourceEngine;
private:
	char      * m_pSDPData;
	CSDPParse * m_pSDPParse;
};

#endif //__RTPSOURCE_H__