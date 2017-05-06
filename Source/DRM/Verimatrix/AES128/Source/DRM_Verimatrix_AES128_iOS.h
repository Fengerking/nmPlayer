#ifndef __CDRM_VERIMATRIX_AES128_IOS_H__
#define __CDRM_VERIMATRIX_AES128_IOS_H__
#include "DRMStreaming.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDRM_Verimatrix_AES128
	: public CDRMStreaming
{
public:
	CDRM_Verimatrix_AES128(void);
	~CDRM_Verimatrix_AES128(void);


	virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32	Uninit();

	virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NONE; }

	virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32	GetParameter(VO_U32 uID, VO_PTR pParam);

	virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

	static VO_U32	getUniqueID(VO_PTR pParam);

private:
	VO_U32 doInit();

	enum STATE_VERIMATRIX
	{
		VERIMATRIX_UNINIT,
		VERIMATRIX_INITED
	};

	struct Chunk_DRM_INFO
	{
		VO_U32	uIdentifer;
		VO_CHAR	szKey[2048];
		VO_BYTE	IV[16];
	};

	static STATE_VERIMATRIX	m_eVermatrix;
	static VO_CHAR			m_szVCASBootAddress[64];

	void*					m_pCB;
	
	VO_BOOL					m_bVRDebug;

	Chunk_DRM_INFO			m_DCI[3];

	VO_CHAR					m_szID[64];

	voCMutex				m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_VERIMATRIX_AES128_IOS_H__
