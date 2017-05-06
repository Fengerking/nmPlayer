#pragma once
#include "DRM.h"
#include "DRMToolUtility.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

	class DRM_Amazon_PlayReady :
		public CDRM
	{
	public:
		DRM_Amazon_PlayReady(void);
		~DRM_Amazon_PlayReady(void);

		virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
		virtual VO_U32	Uninit();

		virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam);

		virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);
		virtual VO_U32	GetParameter(VO_U32 uID, VO_PTR pParam) { return VO_ERR_NONE; }

		virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

		static unsigned char* fAllocate(VO_U32 uSize) { return new unsigned char[uSize]; }

	private:
		VO_U32 TriggerInitDataCallbackEvent();

		VO_BOOL		m_bIsDRM;

		VO_BYTE*	m_pDRMHeader;
		VO_U32		m_uDRMHeader;
		VO_BYTE*	m_pCustomDRMHeader;
		VO_U32		m_uCustomDRMHeader;

		struct TrackInfo {
			VO_U32				uTrackID;
			TrackEncryption		s_TrackEncryption;

			TrackInfo			*pNext;
		};

		TrackInfo*	m_pTrackInfos;

		voCMutex	m_lock;
	};


#ifdef _VONAMESPACE
}
#endif