#ifndef __CDRMLOCAL_H__
#define __CDRMLOCAL_H__

#include "voYYDef_SourceIO.h"
#include "voSource.h"
#include "voDRM2.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CDRMLocal
{
public:
	CDRMLocal();
	virtual ~CDRMLocal(void);

	virtual VO_U32	init(VO_SOURCEDRM_CALLBACK2* pDrmCB);
	virtual VO_U32	uninit() { return 0; }

	virtual VO_U32	getDRMInfo(VO_CHAR* szURI, VO_U64* pullPos, VO_U32* puSize);
	virtual VO_U32	setDRMInfo(VO_U64 ullPos, VO_PBYTE pData, VO_U32 uSize, VO_PTR pInfo);

	virtual VO_U32	getOriginalFileSize(VO_U64* pullFileSize);

	virtual VO_U32	getActualLocation(VO_U64 ullWantedPos, VO_U32 uWantedSize, VO_U64* pullActualPos, VO_U32* pullActualSize);
	virtual VO_U32	decryptData(VO_U64 ullActualOffset,	VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE *ppDesData = NULL, VO_U32 *pnDesSize = NULL, VO_PTR pAdditionalInfo = NULL);

private:
	VO_SOURCEDRM_CALLBACK2 m_DRMCB;
};

#ifdef _VONAMESPACE
}
#endif

#endif //__CDRMLOCAL_H__
