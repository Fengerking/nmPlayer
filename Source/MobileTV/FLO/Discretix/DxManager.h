#ifndef _DX_MANAGER_H_
#define _DX_MANAGER_H_
#if ENABLE_DX

#include <stdio.h>
#include "connmgmt_srtp.h"
#include "DxHost.h"
#include "dbgdump.h"
#include "voCMutex.h"

typedef EDxStatus (*T_CM_GetDxSrtpKeySalt)(DxSrtpKey* srtpSessionKey, DxSrtpSalt srtpSessionSalt);
typedef void (*T_CM_DestroyDxSrtpKey)(DxSrtpKey* srtpSessionKey);
typedef DX_FUNC DX_FLO_EXPORT void (*T_DxHost_DestroySrtpContext)(DxHost_SrtpContext* contextToDestroy);
typedef DX_FUNC DX_FLO_EXPORT EDxStatus (*T_DxSrtpDecrypt_Init)(DxHost_SrtpContext* newSrtpContext, const DxSrtpKey* srtpSessionKey, const DxSrtpSalt srtpSessionSalt); 
typedef DX_FUNC DX_FLO_EXPORT EDxStatus (*T_DxSrtpDecrypt_ProcessPacket)(DxHost_SrtpContext srtpContext, const void* encPacket, DxUint32 encPacketSize, void* plainPacket, DxUint32* plainPacketSize);


class CDxManager
{
public:
	CDxManager(void);
	~CDxManager(void);

	EDxStatus Open();
	void Close();
	DxUint32 Decrypt(void* buffer, DxUint32 bufSize);

private:
	DxSrtpKey m_sessionKey;
	DxSrtpSalt m_sessionSalt;
	DxHost_SrtpContext m_srtpContext;

private:
    void* m_hConMgr;
    T_CM_GetDxSrtpKeySalt CM_GetDxSrtpKeySalt;
    T_CM_DestroyDxSrtpKey CM_DestroyDxSrtpKey;

#if USE_DX_SO
    void* m_hDxHost;
    T_DxHost_DestroySrtpContext DxHost_DestroySrtpContext;
    T_DxSrtpDecrypt_Init DxSrtpDecrypt_Init;
    T_DxSrtpDecrypt_ProcessPacket DxSrtpDecrypt_ProcessPacket;
#endif //USE_DX_SO

protected:
    bool OpenLib(const char* libpath);
    void CloseLib();

public:
	void SetLibPath(const char* path);
	//void SetDataPath(const char* path);
	void SetDxDRM(int enable);
	//void SetLogLevel(int level);
	void DumpContext(char* callFunc);
private:
	int m_nEnable;
	bool	mbInit;
	//int m_nLogLevel;
    CSimplePacketDumper m_plainDumper;
    CSimplePacketDumper m_encryptDumper;
    CTextDumper m_dxDumper;

	 voCMutex        mLock;
};
void DXResetDxManager();
extern CDxManager TheDxManager;

#endif //ENABLE_DX
#endif //_DX_MANAGER_H_
