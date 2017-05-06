#ifndef __CDRM_VISUALON_AES128_H__
#define __CDRM_VISUALON_AES128_H__
#include "VO_AES128_SingleInstance.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDRM_VisualOn_AES128
	: public CDRM
{
public:
	CDRM_VisualOn_AES128(void);
	virtual ~CDRM_VisualOn_AES128(void);

	virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32	Uninit();

	virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam);

	virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32	GetParameter(VO_U32 uID, VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);


private:
	CVO_AES128_SingleInstance	m_VO_AES128[3];
	VO_U32	m_uOutputSeqence;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_VISUALON_AES128_H__