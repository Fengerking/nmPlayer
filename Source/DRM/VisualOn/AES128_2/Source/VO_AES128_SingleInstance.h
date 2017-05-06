#ifndef __CAES128SINGLEINSTANCE_H__
#define __CAES128SINGLEINSTANCE_H__
#include "DRMStreaming.h"
#include "AES_CBC.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CVO_AES128_SingleInstance
	: public CDRMStreaming
	, public CAES_CBC
{
public:
	CVO_AES128_SingleInstance(void);
	virtual ~CVO_AES128_SingleInstance(void);

	virtual VO_U32 Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32 Uninit();

	virtual VO_U32 SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32 OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

protected:
	VO_CHAR m_PreKeyURI[2048];

	virtual VO_U32 getKey(VO_CHAR* pURLKey, VO_BYTE* pKey, VO_U32* puKey);

private:
	VO_SOURCE2_IO_API* m_pAPI_IO;

	VO_U32	m_uLen;
	VO_BYTE m_Buf[752];
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CAES128SINGLEINSTANCE_H__
