#include "AES128_CommonAPI.h"
#include "CSourceIOUtility.h"
#include "voOSFunc.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

#ifdef _WINDOWS
AES128_CommonAPI::AES128_CommonAPI(const TCHAR *pWorkPath)
#else
AES128_CommonAPI::AES128_CommonAPI(const char *pWorkPath)
#endif
: m_eType(VO_OSMP_DRM_SOURCE_UNKNOWN)
        , m_bExit(VO_FALSE)
{
#ifdef _WINDOWS
	VOLOGINIT(const_cast<TCHAR*>(pWorkPath));

	VOLOGI("Work Path %ls", pWorkPath);
#else
	VOLOGINIT(const_cast<char*>(pWorkPath));

	VOLOGI("Work Path %s", pWorkPath);
#endif

	memset( &m_apiIO, 0, sizeof(VO_SOURCE2_IO_API) );
	m_DLLLoader.SetWorkPath( (VO_TCHAR*)(pWorkPath) );
}

AES128_CommonAPI::~AES128_CommonAPI(void)
{
	VOLOGUNINIT();
}

int AES128_CommonAPI::beginSegment(int identifer, void* pAdditionalInfo)
{
	if (VO_OSMP_DRM_SOURCE_HLS != m_eType )
		return VO_OSMP_DRM_ERR_NONE;

	voCAutoLock lock(&m_lock);

	for (int i = 0; i < 3; i++)
	{
		if (m_VO_AES128[i].Open(this, identifer, pAdditionalInfo) == 0)
			return VO_OSMP_DRM_ERR_NONE;
	}

	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
}

int AES128_CommonAPI::decryptSegment(int identifer, long long llPosition, unsigned char* pData, int* piSize, bool bLastBlock, void* pAdditionalInfo)
{
	if (VO_OSMP_DRM_SOURCE_HLS != m_eType )
		return VO_OSMP_DRM_ERR_NONE;

	for (int i = 0; i < 3; i++)
	{
		if (m_VO_AES128[i].Identifer() == identifer)
			return m_VO_AES128[i].Decrypt(llPosition, pData, *piSize, bLastBlock, pData, piSize, pAdditionalInfo);
	}

	return VO_OSMP_DRM_ERR_STATUS;
}

int AES128_CommonAPI::decryptSegment(int identifer, long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo)
{
	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
}

int AES128_CommonAPI::endSegment(int identifer)
{
	if (VO_OSMP_DRM_SOURCE_HLS != m_eType )
		return VO_OSMP_DRM_ERR_NONE;

	voCAutoLock lock(&m_lock);

	for (int i = 0; i < 3; i++)
	{
		if (m_VO_AES128[i].Identifer() == identifer)
			return m_VO_AES128[i].Close();
	}

	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED;
}


int AES128_CommonAPI::getKey(char* pKeyURL, unsigned char* pKey, int* piKey)
{
	if (NULL == pKeyURL || NULL == pKey || NULL == piKey) {
		VOLOGE("empty pointor");
		return VO_OSMP_DRM_ERR_PARAMETER;
	}

	if (m_apiIO.Init == NULL)
	{
		if (loadIO());
	}

	void* h = NULL;
	voAutoIOInit init_obj(&m_apiIO, pKeyURL, &h);
	voAutoIOOpen open_obj(&m_apiIO, h, VO_TRUE);

	unsigned long ulRet = ReadFixedSize(&m_apiIO, h, pKey, reinterpret_cast<VO_U32*>(piKey), &m_bExit);
	if (0 != ulRet) {
		VOLOGE("!IO ReadFixedSize 0x%x", ulRet);
	}

	return (VO_TRUE == m_bExit) ? VO_OSMP_DRM_ERR_STATUS: ulRet;
}

int AES128_CommonAPI::loadIO()
{
	void (*pvoGetSourceIOAPI)(VO_SOURCE2_IO_API*);

#ifdef _IOS
	voGetSourceIOAPI(&m_apiIO);
#else
	vostrcpy(m_DLLLoader.m_szDllFile, _T("voSourceIO"));
	vostrcpy(m_DLLLoader.m_szAPIName, _T("voGetSourceIOAPI"));

	if (m_DLLLoader.LoadLib(NULL) == 0)
	{
		VOLOGE("Load IO");
		return VO_OSMP_DRM_ERR_STATUS;
	}

	if (m_DLLLoader.m_pAPIEntry)
	{
		pvoGetSourceIOAPI = (void(*)(VO_SOURCE2_IO_API*))m_DLLLoader.m_pAPIEntry;
		pvoGetSourceIOAPI(&m_apiIO);
	}
	else
	{
		VOLOGE("Get IO");
		return VO_OSMP_DRM_ERR_STATUS;
	}
#endif //_IOS

	return VO_OSMP_DRM_ERR_NONE;
}
