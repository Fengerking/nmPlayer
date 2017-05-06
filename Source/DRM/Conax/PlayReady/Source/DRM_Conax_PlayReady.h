#ifndef __DRM_CONAX_PLAYREADY_H__
#define __DRM_CONAX_PLAYREADY_H__
#include "VOOSMPDRM.h"
#include "cxdrmapi.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class DRM_Conax_PlayReady :
	public VOOSMPDRM
{
public:
	DRM_Conax_PlayReady(conaxDrmAPI* pParam);
	~DRM_Conax_PlayReady(void);

	/*virtual*/ int init();
	/*virtual*/ int destroy() { return VO_OSMP_DRM_ERR_NONE; }

	//通常src和des是同一个buffer，若*piDes，请rerutn small并告知piDes; again
	/*virtual*/ int processURI(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcURI, char* pDesURI, int* piDesURI) {	return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int processPlaylist(VO_OSMP_DRM_SOURCE_TYPE eType, const char* szSrcPlaylist, int iSrcPlaylist, char* pDesPlaylist, int* piDesPlaylist) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int processHeader(VO_OSMP_DRM_SOURCE_TYPE eType, const unsigned char* pHeader, int iHeaderSize);

	/*virtual*/ int getHeaderLocation(long long* piPosition, int* piSize) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int getDataLocation(long long* piPosition, int* piSize) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int getDataFileSize(long long *piSize) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }

	/*virtual*/ int beginSegment(int identifer, void* pAdditionalInfo);
	/*virtual*/ int decryptSegment(int identifer, long long llPosition, unsigned char* pData, int* piSize, bool bLastBlock, void* pAdditionalInfo);
	/*virtual*/ int decryptSegment(int identifer, long long llPosition, unsigned char* pSrcData, int iSrcDdata, bool bLastBlock, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }
	/*virtual*/ int endSegment(int identifer);

	/*virtual*/ int decryptSample(int iPosition, unsigned char* pData, int* piSize, void* pAdditionalInfo);
	/*virtual*/ int decryptSample(int iPosition, unsigned char* pSrcData, int iSrcDdata, unsigned char* pDesData, int* piDesData, void* pAdditionalInfo) { return VO_OSMP_DRM_ERR_NOT_IMPLEMENTED; }

private:
	VO_OSMP_DRM_SOURCE_TYPE	m_eType;

	conaxDrmAPI*	m_pAPIs;
	void*			m_hContext;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__DRM_CONAX_PLAYREADY_H__