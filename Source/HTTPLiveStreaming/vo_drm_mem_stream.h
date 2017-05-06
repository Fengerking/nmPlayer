
#ifndef __VO_DRM_MEM_STREAM_H__

#define __VO_DRM_MEM_STREAM_H__

#include "vo_mem_stream.h"
#include "DRM_API.h"
#include "stdio.h"

//add for irdeto
#include "voDRM.h"
#include "CvoBaseDrmCallback.h"
//add for irdeto


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#define MAX_PARAMSET_COUNT    8

class vo_drm_mem_stream : public vo_mem_stream
{
public:
	vo_drm_mem_stream( VO_CHAR * ptr_key , VO_CHAR * ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle );
	~vo_drm_mem_stream();

	virtual VO_BOOL open();
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

	virtual VO_VOID write_eos();
	virtual VO_S32 get_lasterror();

private:
	VO_CHAR m_cache_buffer[32];
	VO_S32 m_used_size;

	ProtectionType m_drm_type;
	DRM_Callback * m_ptr_drm_engine;
	void * m_drm_handle;

	VO_S32 m_drm_status;

	FILE * m_fp;
	FILE * m_decrypt_fp;

	VO_S32 m_errorcode;
};

class  CvoGenaralDrmCallback:public CvoBaseDrmCallback
{
public:
    CvoGenaralDrmCallback(VO_DRM_TYPE  eDrmType, void*  pCallbackPtrs);
    ~CvoGenaralDrmCallback();
    VO_S32   SetDrmParams(VO_U32*  pParamSet, VO_U32   uParamCount);
    VO_S32   DoDecryptWithParams(VO_PBYTE pData, VO_U32 nSize, VO_U32*  pParamSet, VO_U32   uParamCount);	
	VO_S32	 SetDecryptParams(VO_U32*  pParamSetInDecrypt, VO_U32	uParamCount);
	
	VO_BOOL  GettDecryptParams(VO_U32*  pParamCount, VO_U32* pParamSet);
    
protected: 
    VO_DRM_TYPE     m_DrmType;
    void*           m_pCookieForCB;
	
	VO_U32                               m_aParamSetForDecrypt[MAX_PARAMSET_COUNT];
	VO_U32                               m_ulParamCountForDecrypt;

	VO_U32                               m_aParamSetForDrm[MAX_PARAMSET_COUNT];
	VO_U32                               m_ulParamCountForDrm;	
};


class CvoGenaralDrmMemStream : public vo_mem_stream
{
public:
	CvoGenaralDrmMemStream();
	~CvoGenaralDrmMemStream();

	virtual VO_BOOL open();
	virtual VO_S64 append( VO_PBYTE ptr_buffer , VO_U64 buffer_size );

	virtual VO_VOID write_eos();
	virtual VO_S32 get_lasterror();

    VO_S32   BindDrm(void*  pCallbackPtrs);

private:
	VO_BYTE m_cache_buffer[256];
	VO_S32 m_used_size;

	VO_S32 m_drm_status;

	FILE * m_fp;
	FILE * m_decrypt_fp;
	int    m_iState;

	CvoGenaralDrmCallback*               m_pGenaralCB;
	VO_S32 m_errorcode;
};

#ifdef _VONAMESPACE
}
#endif

#endif
