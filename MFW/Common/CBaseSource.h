	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseSource.h

	Contains:	CBaseSource header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2008-04-30		JBF			Create file

*******************************************************************************/

#ifndef __CBaseSource_H__
#define __CBaseSource_H__

#include "voFile.h"
#include "voCMutex.h"
#include "voLiveSource.h"
#include "CBaseNode.h"
/**
 * Source type
 */
typedef enum
{
	VO_SOURCE_TYPE_NONE				= 0X00000000,	/*!< NO Source */
	VO_SOURCE_TYPE_FILE				= 0X00000001,	/*!< Local File */
	VO_SOURCE_TYPE_RTSP				= 0X00000002,	/*!< RTSP Stream */
	VO_SOURCE_TYPE_HTTP				= 0X00000003,	/*!< HTTP Stream */
	VO_SOURCE_TYPE_FTP				= 0X00000004,	/*!< FTP Stream */
	VO_SOURCE_TYPE_LIVE				= 0X00000005,	/*!< LIVE Stream */
	VO_SOURCE_TYPE_MAX				= VO_MAX_ENUM_VALUE
}VO_SOURCE_TYPE;

class IVOFileBasedDRM;
class CBaseSource : public CBaseNode
{
public:
	CBaseSource (VO_PTR hInst, VO_U32 nFormat, VO_MEM_OPERATOR * pMemOP, VO_FILE_OPERATOR * pFileOP, VO_SOURCEDRM_CALLBACK * pDrmCB);
	virtual ~CBaseSource (void);

	virtual VO_U32			LoadSource (const VO_PTR pSource, VO_U32 nType, VO_S64 nOffset, VO_S64 nLength);
	virtual VO_U32			CloseSource (void);

	virtual VO_U32			Start(void);
	virtual VO_U32			Pause(void);
	virtual VO_U32			Stop(void);

	virtual VO_U32			GetSourceInfo (VO_PTR pSourceInfo);
	virtual VO_U32			GetTrackInfo (VO_U32 nTrack, VO_PTR pTrackInfo);

	virtual VO_U32			GetAudioFormat (VO_U32 nTrack, VO_AUDIO_FORMAT * pAudioFormat);
	virtual VO_U32			GetVideoFormat (VO_U32 nTrack, VO_VIDEO_FORMAT * pVideoFormat);

	virtual VO_U32			GetFrameType (VO_U32 nTrack);

	virtual VO_U32			GetTrackData (VO_U32 nTrack, VO_PTR pTrackData);

	virtual VO_BOOL			CanSeek (void);
	virtual VO_U32			SetTrackPos (VO_U32 nTrack, VO_S64 * pPos);

	virtual VO_U32			GetSourceParam (VO_U32 nID, VO_PTR pValue);
	virtual VO_U32			SetSourceParam (VO_U32 nID, VO_PTR pValue);

	virtual VO_U32			GetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);
	virtual VO_U32			SetTrackParam (VO_U32 nTrack, VO_U32 nID, VO_PTR pValue);

	virtual VO_U32			GetNearKeyframe (VO_SOURCE_NEARKEYFRAME* pNearKeyframe) {return VO_ERR_NOT_IMPLEMENT;}

	virtual VO_U32			GetDuration (void) {return m_nDuration;}
	virtual VO_U32			GetCurPos (void) {return m_nCurPos;}

	virtual VO_U32			GetFormat (void) {return m_nFormat;}
	virtual VO_U32			HasError (void) {return m_nError;}
	virtual VO_SOURCE_TYPE	GetSourceType (void) {return m_nSourceType;}

	virtual VO_BOOL			IsOutputConfigData (VO_U32 nTrack) {return VO_FALSE;}
	virtual VO_U32			SetOutputConfigData (VO_U32 nTrack, VO_BOOL bOutputConfigData) {return VO_ERR_NOT_IMPLEMENT;}
	virtual void			SetPlayClearestVideo(VO_BOOL bPlayClearestVideo) {}
	virtual void			SetWVC1Decoder(VO_U32 nDecoder) {}


	virtual VO_U32			SetCallBack (VO_PTR pCallBack, VO_PTR pUserData);
	virtual void			SetIFileBasedDrm (IVOFileBasedDRM* pIFileBasedDrm) {m_pIFileBasedDrm = pIFileBasedDrm;}
	virtual void			SetLiveSrcType(VO_LIVESRC_FORMATTYPE nType) {};

	virtual	char*		    GetMetaDataText (int nIndex); //rogine add
	
	bool                    isVDrmDataAppended();
	bool                    isADrmDataAppended();
protected:
	VO_U32					m_nFormat;

	voCMutex				m_csRead;

	VO_U32					m_nDuration;
	VO_U32					m_nCurPos;

	VO_U32					m_nError;
	VO_SOURCE_TYPE			m_nSourceType;

	VO_SOURCEDRM_CALLBACK *	m_pDrmCB;
	IVOFileBasedDRM*		m_pIFileBasedDrm;
	VO_LIVESRC_FORMATTYPE   m_LiveSrcType;
	bool                    mIsVDrmDataAppended;
	bool                    mIsADrmDataAppended;
};

#endif // __CBaseSource_H__
