#ifndef __CDRM_VERIMATRIX_AES128_OTHER_H__
#define __CDRM_VERIMATRIX_AES128_OTHER_H__
#include "DRMStreaming.h"
#include "voCMutex.h"
#include "ViewRightWebClient3_4.h"
#include "DRM.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE
{
#endif

class CDRM_Verimatrix_AES128
        : public CDRMStreaming
{
public:
        CDRM_Verimatrix_AES128(void);
        ~CDRM_Verimatrix_AES128(void);

        virtual VO_U32	Init(VO_DRM_OPENPARAM *pParam);
        virtual VO_U32	Uninit();

        virtual VO_U32	SetThirdpartyAPI(VO_PTR pParam)
        {
                return VO_ERR_NONE;
        }

        virtual VO_U32	SetParameter(VO_U32 uID, VO_PTR pParam);
        virtual VO_U32	GetParameter(VO_U32 uID, VO_PTR pParam);

        virtual VO_U32	OnSourceDrm(VO_U32 nFlag, VO_PTR pParam, VO_DRM2_SOURCE_FORMAT eSourceFormat);

private:
        //VO_U32 doInit();

        ViewRightWebClient::VRWebClientError_t CheckDeviceVCASStatus();

        enum STATE_CONTENT
        {
                CONTENT_NORMAL,
                CONTENT_LINK_CHANGED,
                CONTENT_LINK_NEW_SEVER
        };

        struct Chunk_DRM_INFO
        {
                VO_U32	uIdentifer;
                VO_CHAR	szKey[2048];
                VO_BYTE	IV[16];
        };

        Chunk_DRM_INFO			m_DCI[3];

        ViewRightWebClient*		m_pVRWClient;

        STATE_CONTENT			m_eContent;
        VO_CHAR					m_szVCASBootAddress[64];

        voCMutex				m_lock;

        VO_CHAR					m_szID[64];
        bool                            b_init;

};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRM_VERIMATRIX_AES128_OTHER_H__
