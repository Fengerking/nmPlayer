	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CModuleVersion.cpp

	Contains:	CModuleVersion class file

	Written by:	Jim

	Change History (most recent first):
	2013-06-07		Jim			Create file

*******************************************************************************/
#include "CModuleVersion.h"
#include "voModuleVersion.h"

#define LOG_TAG "CModuleVersion"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CModuleVersion::CModuleVersion ()
{
    
}

CModuleVersion::~CModuleVersion ()
{
}

int CModuleVersion::GetModuleVersion(VOOSMP_MODULE_VERSION* pVersion)
{
//    if(!m_pAPIEntry)
//        LoadLib(NULL);
    
    if(!pVersion || !m_pAPIEntry)
    {
        VOLOGE("Get module version failed, %x %x", (void*)pVersion, m_pAPIEntry);
        return VOOSMP_ERR_Pointer;
    }
    
    
    VO_MODULE_VERSION version;
    memset(&version, 0, sizeof(VO_MODULE_VERSION));
    
    if(pVersion->nModuleType == VOOSMP_MODULE_TYPE_SDK)
    {
        version.nModuleID = VO_INDEX_OSMP_MEDIAPLAYER;
    }
    else if(pVersion->nModuleType == VOOSMP_MODULE_TYPE_DRM_VENDOR_A)
    {
        version.nModuleID = VO_INDEX_DRM_VERIMATRIX_PR;
    }
    else
    {
        VOLOGE("Not support version info for this module : %d", pVersion->nModuleType);
        return VOOSMP_ERR_Args;
    }
    
    VOGETMODULEVERSION getVersionAPI = (VOGETMODULEVERSION)m_pAPIEntry;
    
    int nRet = getVersionAPI(&version);
    
    if( (nRet!=VO_ERR_NONE) || !version.pszVersion)
    {
        VOLOGE("Get module version failed,module ID is %0x", version.nModuleID);
        return VOOSMP_ERR_Unknown;
    }
    
    pVersion->pszVersion = version.pszVersion;
    
    VOLOGI("Module version is %s", pVersion->pszVersion);

    return VOOSMP_ERR_None;
}



VO_U32 CModuleVersion::LoadLib (VO_HANDLE hInst)
{
    vostrcpy(m_szDllFile, _T("voVersion"));
    vostrcpy(m_szAPIName, _T("voGetModuleVersion"));

#if defined _WIN32
	vostrcat(m_szDllFile, _T(".Dll"));
#elif defined LINUX
	vostrcat(m_szDllFile, ".so");
#elif defined _MAC_OS
	vostrcat(m_szDllFile, ".dylib");
#endif

#ifdef _IOS
    m_pAPIEntry = (VO_PTR)voGetModuleVersion;
#else
	if(CDllLoad::LoadLib(NULL) == 0)
	{
		VOLOGE( "load %s fail!", m_szDllFile);
		return 0;
	}
#endif // _IOS
    
    if(!m_pAPIEntry)
    {
        VOLOGE("voGetModuleVersion not found!!!");
        return 0;
    }

	return 1;
}
