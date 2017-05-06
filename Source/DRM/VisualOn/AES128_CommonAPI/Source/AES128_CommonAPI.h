#ifndef __AES128_COMMONAPI_H__
#define __AES128_COMMONAPI_H__
#include "VOOSMPDRM.h"
#include "VOAES128_CBC.h"
#include "voCMutex.h"
#include "CDllLoad.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class AES128_CommonAPI
	: public VOOSMPDRM
	, public VOAES128_CBC::Delegate
{
public:

#ifdef _WINDOWS
	AES128_CommonAPI(const TCHAR *pWorkPath);
#else
	AES128_CommonAPI(const char *pWorkPath);
#endif
	~AES128_CommonAPI(void);

	/*virtual*/ int init() { m_bExit = VO_FALSE; return VO_OSMP_DRM_ERR_NONE; }
	/*virtual*/ int destroy() { m_bExit = VO_TRUE; return VO_OSMP_DRM_ERR_NONE; }

	//通常src和des是同一个buffer，若*piDes，请rerutn small并告知piDes; again
	/*virtual*/ int processURI(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcURI, char* pDesURI, int* piDesURI) {	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }//VO_OSMP_DRM_ERR_NONE
	/*virtual*/ int processPlaylist(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcPlaylist, int iSrcPlaylist, char* pDesPlaylist, int* piDesPlaylist) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }//VO_OSMP_DRM_ERR_NONE
	/*virtual*/ int processHeader(VO_OSMP_DRM_SOURCE_TYPE eType, const unsigned char* pHeader, int iHeaderSize) { m_eType = eType; return VO_OSMP_DRM_ERR_NONE; }//VO_OSMP_DRM_ERR_NONE

	/*virtual*/ int getHeaderLocation(long long* piPosition, int* piSize) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int getDataLocation(long long* piPosition, int* piSize) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int getDataFileSize(long long *piSize) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }

	/*virtual*/ int beginSegment(int identifer, void* pAdditionalInfo);
	/*virtual*/ int decryptSegment(int identifer, long long llPosition, unsigned char* pData, int* piSize, bool bLastBlock, void* pAdditionalInfo);
	/*virtual*/ int decryptSegment(int identifer, long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo);
	/*virtual*/ int endSegment(int identifer);

	/*virtual*/ int decryptSample(int iPosition, unsigned char* pData, int* piSize, void* pAdditionalInfo) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int decryptSample(int iPosition, unsigned char* pSrcData, int iSrcDdata, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }

private:
	int getKey(char* pKeyURL, unsigned char* pKey, int* piKey);

	int loadIO();

	VO_OSMP_DRM_SOURCE_TYPE	m_eType;

	VO_BOOL				m_bExit;

	VOAES128_CBC		m_VO_AES128[3];

	CDllLoad			m_DLLLoader;
	VO_SOURCE2_IO_API   m_apiIO;

	voCMutex			m_lock;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__AES128_COMMONAPI_H__