#ifndef __CDRM_NEXTSCAPE_PLAYREADY_H__
#define __CDRM_NEXTSCAPE_PLAYREADY_H__
#include "DRMStreaming.h"
#include "Nextscape.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CDRM_Nextscape_PlayReady :
	public CDRM
{
public:
	CDRM_Nextscape_PlayReady(void);
	~CDRM_Nextscape_PlayReady(void);

	virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
	virtual VO_U32	Uninit();

	virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam) { return VO_ERR_NOT_IMPLEMENT; }

	virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

	static int RegistCallback(ActivateCallback p_activate, DecryptCallback p_callback, void * p_custom_data)
	{
		m_fActivate		= p_activate;
		m_fDecrypt		= p_callback;
		m_pCustomData	= p_custom_data;

		return 0;
	}

private:
	enum ContentType{
		CONTENT_PIFF	= 0,
		CONTENT_SSTR	= 1,
		CONTENT_HLS		= 2
	};

	static ActivateCallback	m_fActivate;
	static DecryptCallback	m_fDecrypt;
	static void *			m_pCustomData;

	int m_iDRM;

	bool		m_bHLSInit;
	VO_BYTE*	m_pManifestData;
	VO_U32		m_uSizeManifest;
};


#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_NEXTSCAPE_PLAYREADY_H__