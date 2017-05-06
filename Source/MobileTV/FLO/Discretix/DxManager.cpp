#if ENABLE_DX
#include <dlfcn.h>
#include <stdarg.h>
#include "DxManager.h"
#include "voLog.h"
#include "mylog.h"

#define LIB_CONMGR "libconnmgmt.so"
#define LIB_DXHOST "libDxHost.so"

#define DEFAULT_LIB_PATH "/data/data/com.qualcomm.mediaflo.floui/lib"
#define SYSTEM_LIB_PATH "/system/lib"

CDxManager TheDxManager;

void CDxManager::SetLibPath(const char* path)
{
    VOLOGI("libpath: %s", path);
	OpenLib(path);
	VOLOGI("End");
}

void DXResetDxManager()
{
	VOLOGI();
	TheDxManager.Close();
	if(TheDxManager.Open()!=EDX_SUCCESS)
	{
		VOLOGE("TheDxManager.Open()!=EDX_SUCCESS,reset fails");
	}
}
void CDxManager::SetDxDRM(int enable) 
{ 
    VOLOGI("Enable/Disable DRM: %d", enable);
    m_nEnable = enable; 
}

#if 0
void CDxManager::SetLogLevel(int level) 
{ 
    VOLOGI("Log Level: %d", level);
    m_nLogLevel = level; 
}
#endif

bool CDxManager::OpenLib(const char* libpath)
{
    char path[256];
    if (m_hConMgr == NULL)
    {
        sprintf(path, "%s/%s", libpath, LIB_CONMGR);
        m_hConMgr = dlopen(path, RTLD_NOW);
        VOLOGI("load library %s... %p", path, m_hConMgr);
        if (m_hConMgr)
        {
            CM_GetDxSrtpKeySalt = (T_CM_GetDxSrtpKeySalt) dlsym(m_hConMgr, "CM_GetDxSrtpKeySalt");
            CM_DestroyDxSrtpKey = (T_CM_DestroyDxSrtpKey) dlsym(m_hConMgr, "CM_DestroyDxSrtpKey");
        }
    }
#if USE_DX_SO
    if (m_hDxHost == NULL)
    {
        sprintf(path, "%s/%s", libpath, LIB_DXHOST);
        m_hDxHost = dlopen(path, RTLD_NOW);
        VOLOGI("load library %s... %p", path, m_hDxHost);
        if (m_hDxHost)
        {
            DxHost_DestroySrtpContext = (T_DxHost_DestroySrtpContext) dlsym(m_hDxHost, "DxHost_DestroySrtpContext");
            DxSrtpDecrypt_Init = (T_DxSrtpDecrypt_Init) dlsym(m_hDxHost, "DxSrtpDecrypt_Init");
            DxSrtpDecrypt_ProcessPacket = (T_DxSrtpDecrypt_ProcessPacket) dlsym(m_hDxHost, "DxSrtpDecrypt_ProcessPacket");
        }
		VOLOGI("Load DXLIB End");
    }
    return (m_hConMgr && m_hDxHost);
#else
    return m_hConMgr;
#endif //USE_DX_SO
}

void CDxManager::CloseLib()
{
	VOLOGI("DX:Enter");
#if USE_DX_SO
    if (m_hDxHost)
    {
        VOLOGI("Unload library" LIB_DXHOST);
        dlclose(m_hDxHost); 
        m_hDxHost = NULL;
    }
#endif //USE_DX_SO
    if (m_hConMgr)
    {
        VOLOGI("Unload library" LIB_CONMGR);
        dlclose(m_hConMgr); 
        m_hConMgr = NULL;
    }
	VOLOGI("DX:End");
}


CDxManager::CDxManager(void)
{
	VOLOGI("DX:Enter");
#if USE_DX_SO 
    m_hDxHost = NULL;
#endif //USE_DX_SO

    m_hConMgr = NULL;
	m_nEnable = 0;
	mbInit = false;

	//m_nLogLevel = 0;
	VOLOGI("DX:End");
}

CDxManager::~CDxManager(void)
{
	VOLOGI("DX:Enter");
    CloseLib();
    m_plainDumper.Close();
    m_encryptDumper.Close();
    m_dxDumper.Close();
	VOLOGI("DX:End");
}
void  CDxManager::DumpContext(char* callFunc)
{
#if ENABLE_LOG
	VOLOGI("%s:m_srtpContext : %p",callFunc,m_srtpContext);
	m_encryptDumper.Dump(m_srtpContext , 4);
#endif //ENABLE_LOG
}
EDxStatus CDxManager::Open()
{
	VOLOGI("DX:Enter");
#if ENABLE_LOG
    m_dxDumper.Open("dx.csv", LOG_DX_SUMMARY);
    m_dxDumper.Dump("%s,%s,%s,%s,%s,%s,%s" TXTLN, 
        "Buffer", "InSize", "OutSize", "Status", "StartTime(ms)", "EndTime(ms)", "Interval(ms)");
    m_encryptDumper.Open("srtp.dat", LOG_DX_DATA);
    m_plainDumper.Open("rtp.dat", LOG_DX_DATA);
#endif
	if (!m_nEnable)
		return EDX_SUCCESS;

	VOLOGI("DX:Enter2");
	voCAutoLock lock(&mLock);

	mbInit = true;
#if USE_DX_SO
    if (m_hDxHost == NULL || m_hConMgr == NULL)
#else
    if (m_hConMgr == NULL)
#endif //USE_DX_SO
    {
        VOLOGW("Library not opened");
        bool b = OpenLib(DEFAULT_LIB_PATH);
        if (!b)
            OpenLib(SYSTEM_LIB_PATH);
    }

	EDxStatus status = CM_GetDxSrtpKeySalt(&m_sessionKey, m_sessionSalt);
	VOLOGI("DxGetDxSrtpKeySalt: %d (%s)", status, m_sessionSalt);
	if (EDX_SUCCESS != status)
		return status;

#if ENABLE_LOG
	DxByte *p = m_sessionKey.m_Data;
	VOLOGI("Dx m_sessionKey Length : %d" , m_sessionKey.m_Length);
	VOLOGI("Dx KeyData: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x" , p[0] , p[1] , p[2] , p[3] , p[4] , p[5] , p[6] , p[7] , p[8] , p[9] , p[10] , p[11] , p[12] , p[13] , p[14] , p[15] );
	p += 16;
	VOLOGI("Dx KeyData: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x" , p[0] , p[1] , p[2] , p[3] , p[4] , p[5] , p[6] , p[7] , p[8] , p[9] , p[10] , p[11] , p[12] , p[13] , p[14] , p[15] );
	p += 16;
	VOLOGI("Dx KeyData: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x" , p[0] , p[1] , p[2] , p[3] , p[4] , p[5] , p[6] , p[7] , p[8] , p[9] , p[10] , p[11] , p[12] , p[13] , p[14] , p[15] );
	p += 16;
	VOLOGI("Dx KeyData:0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x" , p[0] , p[1] , p[2] , p[3] , p[4] , p[5] , p[6] , p[7] , p[8] , p[9] , p[10] , p[11] , p[12] , p[13] , p[14] , p[15] );
	p += 16;
	VOLOGI("Dx KeyData: 0x%02x 0x%02x " , p[0] , p[1] );

	p =m_sessionSalt;
	VOLOGI("Dx m_sessionSalt Data: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x" , m_sessionKey.m_Length , p[0] , p[1] , p[2] , p[3] , p[4] , p[5] , p[6] , p[7] , p[8] , p[9] , p[10] , p[11] , p[12] , p[13] );
	m_encryptDumper.Dump(m_sessionKey.m_Data , m_sessionKey.m_Length);
	m_encryptDumper.Dump(m_sessionSalt , 14);
#endif //ENABLE_LOG
	status = DxSrtpDecrypt_Init(&m_srtpContext, &m_sessionKey, m_sessionSalt);

#if ENABLE_LOG
	VOLOGI("DxSrtpDecrypt_Init: %d  m_srtpContext : %p", status, m_srtpContext);
	m_encryptDumper.Dump(m_srtpContext , 4);
#endif //ENABLE_LOG
	VOLOGI("DX:End");
	return status;
}

void CDxManager::Close()
{
	if(!mbInit)
		return ;
	VOLOGI("DX:Enter");
	voCAutoLock lock(&mLock);

	VOLOGI("CDxManager::Close()");
    m_plainDumper.Close();
    m_encryptDumper.Close();
    m_dxDumper.Close();
	if (!m_nEnable)
		return;
	VOLOGI("DX:Enter2");
	if(m_dxDumper.GetMask() > 0)
		VOLOGI("DxHost_DestroySrtpContext In");
	DxHost_DestroySrtpContext(&m_srtpContext);
	if(m_dxDumper.GetMask() > 0)
	{
		VOLOGI("DxHost_DestroySrtpContext Out")
		VOLOGI("Dx_DestroyDxSrtpKey In");
	}

	CM_DestroyDxSrtpKey(&m_sessionKey);
	if(m_dxDumper.GetMask() > 0)
		VOLOGI("Dx_DestroyDxSrtpKey Out");
	
	mbInit = false;
	VOLOGI("DX:End");
}

DxUint32 CDxManager::Decrypt(void* buffer, DxUint32 bufSize)
{
	//VOLOGI("DX:Enter");
	voCAutoLock lock(&mLock);

	EDxStatus status = EDX_SUCCESS;
    DxUint32 outSize = bufSize;
	if (m_nEnable)
	{
#if ENABLE_LOG
            m_encryptDumper.Dump(buffer, bufSize);
        unsigned int tick1 = GetAppTick();
#endif //ENABLE_LOG

		int nTimes = 3;
		while(nTimes)
		{
			status = DxSrtpDecrypt_ProcessPacket(m_srtpContext,
				buffer, bufSize,
				buffer, &outSize);
			if(status == 0)
				break;


			if( m_dxDumper.GetMask() > 0)
				VOLOGI("CDxManager::Decrypt(%p, %d) => %d ... %d", buffer, bufSize, outSize, status);

			nTimes --;
		}


#if ENABLE_LOG
        unsigned int tick2 = GetAppTick();
      //VOLOGI("CDxManager::Decrypt(%p, %d) => %d ... %d", buffer, bufSize, outSize, status);
        m_dxDumper.Dump("%p,%d,%d,%d,%d,%d,%d\n", buffer, bufSize, outSize, status, tick1, tick2, tick2 - tick1);
		 m_plainDumper.Dump(buffer, bufSize);
#endif //ENABLE_LOG
	}
	//VOLOGI("DX:End");
	if(outSize!=bufSize)
	{
		VOLOGE("outSize!=bufSize");
	}
	return status;
}


#endif //ENABLE_DX
