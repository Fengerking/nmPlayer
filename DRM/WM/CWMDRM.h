#pragma once

#include "CBaseDRM.h"
#include "voWMDRM.h"

struct __tagDRM_MANAGER_CONTEXT;
struct __tagDRM_MANAGER_DECRYPT_CONTEXT;
class CWMDRM
{
public:
	CWMDRM();
	virtual ~CWMDRM();

public:
	virtual VO_U32		Open();
	virtual VO_U32		Close();
	virtual VO_U32		SetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		GetParameter(VO_U32 uID, VO_PTR pParam);
	virtual VO_U32		SetDrmInfo(VO_U32 nFlag, VO_PTR pDrmInfo);
	virtual VO_U32		GetDrmFormat(VO_DRM_FORMAT* pDrmFormat);
	virtual VO_U32		Commit();
	virtual VO_U32		DecryptData(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pData, VO_U32 nSize);
	virtual VO_U32		DecryptData2(VO_DRM_DATAINFO * pDataInfo, VO_PBYTE pSrcData, VO_U32 nSrcSize, VO_PBYTE* ppDstData, VO_U32* pnDstSize);

public:
	virtual VO_S32		OutputLevelsCallback(const VO_PTR pOutputLevelsData, VO_U32 nCallbackType);

protected:
	virtual VO_U32		CheckLicense();

protected:
	__tagDRM_MANAGER_CONTEXT*			m_pDrmContext;
	__tagDRM_MANAGER_DECRYPT_CONTEXT*	m_pDecryptContext;
};
