
#include "vo_drm_mem_stream.h"
#include "voOSFunc.h"
#include "voLog.h"

#include "vo_aes_engine.h"

#include "voIrdetoDRM.h"

#ifndef LOG_TAG
#define LOG_TAG "vo_drm_mem_stream"
#endif


//DRMHANDLE g_engine;

vo_drm_mem_stream::vo_drm_mem_stream( VO_CHAR * ptr_key , VO_CHAR * ptr_iv , ProtectionType drm_type , DRM_Callback * ptr_drm_engine , void * drm_handle )
:m_used_size(0)
,m_drm_type( drm_type )
,m_ptr_drm_engine( ptr_drm_engine )
,m_drm_handle( drm_handle )
,m_drm_status( 1 )
,m_fp(0)
,m_decrypt_fp(0)
,m_errorcode(0)
{
    VOLOGI( "+drm_setkey %p" , m_ptr_drm_engine );
	m_drm_status = m_ptr_drm_engine->drm_setkey( m_drm_handle , m_drm_type , (char *)ptr_key , (unsigned char *)ptr_iv );
    VOLOGI( "-drm_setkey" );

	if( m_drm_status != SUCCESS )
		m_errorcode = m_drm_status;

// 	if( m_ptr_drm_engine->drm_setkey == aes_setkey )
// 	{
// 		aes_setkey( g_engine , AES128 , (char *)ptr_key , (unsigned char *)ptr_iv , NULL );
// 	}
// 	else
// 	{
// 		aes_setkey( g_engine , AES128 , (char *)ptr_key , (unsigned char *)ptr_iv , m_key );
// 	}

//  	char * ptr_pos = strrchr( url , '/' );
//  	if( ptr_pos )
//  		ptr_pos++;
//  
//  	char path[1024];
//  	memset( path , 0 , 1024 );
//  
//  	strcpy( path , "/sdcard/" );
//  
//  	if( ptr_pos )
//  		strcat( path , ptr_pos );
//  	else
//  		strcat( path , "temp.ts" );
//  
//  	ptr_pos = strstr( path , "?" );
//  
//  	if( ptr_pos )
//  		*ptr_pos = '\0';
//  
//  	m_fp = fopen( path , "wb+" );
//  
//  	ptr_pos = strrchr( path , '.' );
//  	*ptr_pos = '\0';
//  
//  	strcat( path , "_decrypted.ts" );
//  
//  	m_decrypt_fp = fopen( path , "wb+" );
}

vo_drm_mem_stream::~vo_drm_mem_stream()
{
	if( m_fp )
		fclose( m_fp );

	if( m_decrypt_fp )
		fclose( m_decrypt_fp );

	m_fp = 0;
	m_decrypt_fp = 0;
}

VO_BOOL vo_drm_mem_stream::open()
{
	if( m_drm_status != 1 )
		return VO_FALSE;

	return vo_mem_stream::open();
}

VO_S64 vo_drm_mem_stream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U64 leftsize = buffer_size;
	VO_PBYTE ptr_pos = ptr_buffer;
	int ret = 0;

	if( m_used_size && leftsize + m_used_size >= 16 )
	{
		VO_S32 add_size = 16 - m_used_size;

		if( add_size )
			memcpy( m_cache_buffer + m_used_size , ptr_pos , add_size );

		int out;

		if( m_fp )
		{
			fwrite( m_cache_buffer , 1 , 16 , m_fp );
			fflush( m_fp );
		}

		//VO_U32 start = voOS_GetSysTime();
		ret = m_ptr_drm_engine->drm_decrypt( m_drm_handle , (unsigned char *)m_cache_buffer , 16 , (unsigned char *)m_cache_buffer , &out , false );

		if( ret != SUCCESS )
		{
			m_errorcode =ret;
			VOLOGE( "Decrypt Fail! %d" , m_errorcode );
			return -1;
		}
		//aes_decrypt( g_engine , (unsigned char *)m_cache_buffer , 16 , (unsigned char *)m_cache_buffer , &out , false );
		//VOLOGI( "Decrypt 16 bytes cost %u" , voOS_GetSysTime() - start );

		if( m_decrypt_fp )
		{
			fwrite( m_cache_buffer , 1 , 16 , m_decrypt_fp );
			fflush( m_decrypt_fp );
		}

		vo_mem_stream::append( (VO_PBYTE)m_cache_buffer , 16 );

		m_used_size = 0;

		ptr_pos += add_size;
		leftsize -= add_size;
	}

	if( leftsize > 16 )
	{
		VO_S32 left_in_the_end = leftsize % 16;

		if( !left_in_the_end )
			left_in_the_end = 16;

		int out;

		if( m_fp )
		{
			fwrite( ptr_pos , 1 , (VO_U32)(leftsize - left_in_the_end) , m_fp );
			fflush( m_fp );
		}

		//VO_U32 start = voOS_GetSysTime();
		ret = m_ptr_drm_engine->drm_decrypt( m_drm_handle , ptr_pos , (VO_S32)(leftsize - left_in_the_end)  , ptr_pos , &out , false );

		if( ret != SUCCESS )
		{
			m_errorcode =ret;
			VOLOGE( "Decrypt Fail! %d" , m_errorcode );
			return -1;
		}
		//aes_decrypt( g_engine , ptr_pos , leftsize - left_in_the_end  , ptr_pos , &out , false );
		//VOLOGI( "Decrypt %lld bytes cost %u" , leftsize - left_in_the_end , voOS_GetSysTime() - start );

		if( m_decrypt_fp )
		{
			fwrite( ptr_pos , 1 , (VO_U32)(leftsize - left_in_the_end) , m_decrypt_fp );
			fflush( m_decrypt_fp );
		}

		vo_mem_stream::append( ptr_pos , leftsize - left_in_the_end );

		ptr_pos = ptr_pos + ( leftsize - left_in_the_end );
		leftsize = left_in_the_end;
	}

	memcpy( m_cache_buffer + m_used_size , ptr_pos ,(VO_U32) leftsize );
	m_used_size += (VO_S32)leftsize;

	return buffer_size;
}

VO_VOID vo_drm_mem_stream::write_eos()
{
	int ret;

	if( m_used_size == 16 )
	{
		int out;

		if( m_fp )
		{
			fwrite( m_cache_buffer , 1 , 16 , m_fp );
			fflush( m_fp );
		}

		//VO_U32 start = voOS_GetSysTime();
		ret = m_ptr_drm_engine->drm_decrypt( m_drm_handle , (unsigned char *)m_cache_buffer , 16 , (unsigned char *)m_cache_buffer , &out , true );
		//aes_decrypt( g_engine , (unsigned char *)m_cache_buffer , 16 , (unsigned char *)m_cache_buffer , &out , true );

		if( ret != SUCCESS )
		{
			m_errorcode =ret;
			VOLOGE( "Decrypt Fail! %d" , m_errorcode );
			return;
		}

		//VOLOGI( "Decrypt last 16 bytes cost %u" , voOS_GetSysTime() - start );

		int usefulsize = 16 - m_cache_buffer[15];

		if( usefulsize > 0 && usefulsize < 16 )
		{
			if( m_decrypt_fp )
			{
				fwrite( m_cache_buffer , 1 , usefulsize , m_decrypt_fp );
				fflush( m_decrypt_fp );
			}

			vo_mem_stream::append( (VO_PBYTE)m_cache_buffer , usefulsize );
		}

		m_used_size = 0;
	}
}

VO_S32  vo_drm_mem_stream::get_lasterror()
{
	VO_S32 temp = m_errorcode;
	m_errorcode = 0;
	return temp;
}

/////////////////////////////////////////////////////////////////////////////////


CvoGenaralDrmCallback::CvoGenaralDrmCallback(VO_DRM_TYPE  eDrmType, void*  pCallbackPtrs):CvoBaseDrmCallback((VO_SOURCEDRM_CALLBACK *)pCallbackPtrs)
{
    m_DrmType = eDrmType;
    m_pCookieForCB = NULL;
	
    memset((void*)m_aParamSetForDrm, 0,  sizeof(VO_U32)*MAX_PARAMSET_COUNT);
	memset((void*)m_aParamSetForDecrypt, 0,  sizeof(VO_U32)*MAX_PARAMSET_COUNT);
    m_ulParamCountForDrm = 0;
	m_ulParamCountForDecrypt = 0;	
}
CvoGenaralDrmCallback::~CvoGenaralDrmCallback()
{
    
}


VO_S32	 CvoGenaralDrmCallback::SetDrmParams(VO_U32*  pParamSet, VO_U32   uParamCount)
{
    VO_S32   sRet = 0;	

    switch(m_DrmType)
    {
        case VO_DRMTYPE_DIVX:
		{			
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_DivX(pParamSet[0], pParamSet[1]);
			break;
		}
		case VO_DRMTYPE_WindowsMedia:
		{			
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_WM(pParamSet[0], pParamSet[1], pParamSet[2], pParamSet[3]);
			break;
		}
		case VO_DRMTYPE_PlayReady:
		{
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_PlayReady(pParamSet[0], pParamSet[1], pParamSet[2], pParamSet[3]);
			break;
		}
		case VO_DRMTYPE_Widevine:
		{
			sRet = VO_ERR_DRM_BADPARAMETER;			
			//sRet = DRMInfo_Widevine(pParamSet[0], pParamSet[1]);
			break;
		}
		case VO_DRMTYPE_Irdeto:
		{	
			VOLOGI( "SetDrmParams  url:%s	",pParamSet[0]);
			sRet = DRMInfo_Irdeto((VO_PCHAR) (pParamSet[0]), (VO_PCHAR) (pParamSet[1]));
			break;
		}
		case VO_DRMTYPE_AES128:
		{			
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_AES128(pParamSet[0]);
			break;
		}
		default:
		{
			sRet = VO_ERR_DRM_BADPARAMETER;
			break;
		}
    }

	return sRet;

}



VO_S32   CvoGenaralDrmCallback::DoDecryptWithParams(VO_PBYTE pData, VO_U32 nSize, VO_U32*  pParamSet, VO_U32   uParamCount)
{
    VO_S32   sRet = 0;
	
	VO_IrdetoDRM_DecryptINFO*	pvarIrdeto = NULL;

	VOLOGI( "DoDecryptWithParams    m_DrmType:%d, nSize:%d, ", m_DrmType, nSize);
    switch(m_DrmType)
    {
        case VO_DRMTYPE_DIVX:
		{			
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_DivX(pParamSet[0], pParamSet[1]);
			break;
		}
		case VO_DRMTYPE_WindowsMedia:
		{			
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_WM(pParamSet[0], pParamSet[1], pParamSet[2], pParamSet[3]);
			break;
		}
		case VO_DRMTYPE_PlayReady:
		{
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_PlayReady(pParamSet[0], pParamSet[1], pParamSet[2], pParamSet[3]);
			break;
		}
		case VO_DRMTYPE_Widevine:
		{
			sRet = VO_ERR_DRM_BADPARAMETER;			
			//sRet = DRMInfo_Widevine(pParamSet[0], pParamSet[1]);
			break;
		}
		case VO_DRMTYPE_Irdeto:
		{
			
			VOLOGI( "DoDecryptWithParams in	VO_DRMTYPE_Irdeto ");
			pvarIrdeto = (VO_IrdetoDRM_DecryptINFO*)(pParamSet[0]);
			VOLOGI( " segmentID:%d, state:%d ", pvarIrdeto->segmentID, pvarIrdeto->process);
			sRet = DRMData_Irdeto(pData, nSize, (VO_U32)(pParamSet[0]));
			break;
		}
		case VO_DRMTYPE_AES128:
		{			
			sRet = VO_ERR_DRM_BADPARAMETER;
			//sRet = DRMInfo_AES128(pParamSet[0]);
			break;
		}
		default:
		{
			sRet = VO_ERR_DRM_BADPARAMETER;
			break;
		}
    }

	return sRet;

}

VO_S32   CvoGenaralDrmCallback::SetDecryptParams(VO_U32*  pParamSet, VO_U32   uParamCount)
{
    VO_U32   iIndex = 0;
    if(uParamCount > 0)
    {
        for(iIndex=0; iIndex<uParamCount; iIndex++)
        {
            m_aParamSetForDecrypt[iIndex] = *(pParamSet+iIndex);
        }

		m_ulParamCountForDecrypt = uParamCount;
    }

    return 0;    
}

VO_BOOL	 CvoGenaralDrmCallback::GettDecryptParams(VO_U32*	pParamCount, VO_U32* pParamSet)
{
    if(m_DrmType != VO_DRMTYPE_Irdeto)
    {
        return VO_FALSE;
    }
	
	*pParamCount = 1;
	*pParamSet = m_aParamSetForDecrypt[0];

	return VO_TRUE;

}




/////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////
CvoGenaralDrmMemStream::CvoGenaralDrmMemStream()
{
    memset(m_cache_buffer, 0, 256);
    m_used_size = 0;

    m_fp = NULL;
    m_decrypt_fp = NULL;

    m_pGenaralCB = NULL;
    m_errorcode = 0;

	m_iState = 0;
}


CvoGenaralDrmMemStream::~CvoGenaralDrmMemStream()
{

}


VO_BOOL CvoGenaralDrmMemStream::open()
{
    return vo_mem_stream::open();
}


VO_S64 CvoGenaralDrmMemStream::append( VO_PBYTE ptr_buffer , VO_U64 buffer_size )
{
	VO_U64 leftsize = buffer_size;
	VO_PBYTE ptr_pos = ptr_buffer;
	VO_U32   ulTransactLen = 0;
	int iRet = 0;
	VO_IrdetoDRM_DecryptINFO	varIrdeto = {0};
	VO_U32   aParamset[8] = {0};
	VO_U32   ulParamCount = 0;
	VO_U32   ulParamData = 0;
	
	
	VOLOGE( "append buffer_size  :%d" ,(VO_U32)buffer_size);

	if(m_pGenaralCB->GettDecryptParams(&ulParamCount, &ulParamData) != VO_TRUE)
	{
	    return -1;
	}

	if(m_iState == 0)
	{
	   varIrdeto.process = IrdetoDRM_DecryptProcess_BEGIN;
	   varIrdeto.segmentID = *((VO_U32*)ulParamData);
	   VOLOGE( "varIrdeto.segmentID  %d" , varIrdeto.segmentID );
	   
	   aParamset[0] = (VO_U32)&varIrdeto;

       //make the data for 188*n
       if(buffer_size == 0)
       {
           return 0;
       }
	   
	   ulTransactLen = (VO_U32)((buffer_size/188)*188);
	   
	   iRet = m_pGenaralCB->DoDecryptWithParams(ptr_pos, (VO_U32)ulTransactLen, (VO_U32*)aParamset, 1);

	   m_errorcode = iRet ;

	   VOLOGE( "Decrypt Ret! %d   m_istate:%d, ulTransactLen:%d" , m_errorcode,m_iState, ulTransactLen );


	   
	   vo_mem_stream::append(ptr_buffer , ulTransactLen );
	   memcpy(m_cache_buffer, ptr_pos+ulTransactLen, (VO_U32)(buffer_size-ulTransactLen));
	   m_used_size = (VO_S32)(buffer_size-ulTransactLen);
	   
	   m_iState = 1;
	   return ulTransactLen;
	}
	else
	{
	    
		varIrdeto.process = IrdetoDRM_DecryptProcess_PROCESSING;
		varIrdeto.segmentID = *((VO_U32*)ulParamData);
	    VOLOGE( "varIrdeto.segmentID  %d" , varIrdeto.segmentID );

		aParamset[0] = (VO_U32)&varIrdeto;

        if((m_used_size > 0))
        {
            if((m_used_size+buffer_size) >= 188 )
            {
                memcpy(m_cache_buffer+m_used_size, ptr_pos, 188-m_used_size);
		        iRet = m_pGenaralCB->DoDecryptWithParams((VO_BYTE*)m_cache_buffer, 188, (VO_U32*)aParamset, 1);

			    vo_mem_stream::append(m_cache_buffer , 188 );

			    m_errorcode = iRet ;
			
			    VOLOGE( "Decrypt Ret! %d   m_istate:%d, m_used_size:%d" , m_errorcode,m_iState, m_used_size );

			    ptr_pos = ptr_buffer+(188-m_used_size);
			
			    leftsize = buffer_size-(188-m_used_size);
				m_used_size = 0;
            }
			else
			{
			    memcpy(m_cache_buffer+m_used_size, ptr_pos,(VO_U32) buffer_size);
				return buffer_size;
			}
        }

        if(leftsize == 0)
        {
            VOLOGE( "leftsize is 0" );
            return 188;
        }
		
		ulTransactLen =(VO_U32) ((leftsize/188)*188);
		iRet = m_pGenaralCB->DoDecryptWithParams(ptr_pos, (VO_U32)ulTransactLen, (VO_U32*)aParamset, 1);

		m_errorcode = iRet ;
		
		VOLOGE( "Decrypt Ret! %d   m_istate:%d, ulTransactLen:%d" , m_errorcode,m_iState, ulTransactLen );
		vo_mem_stream::append(ptr_pos , ulTransactLen );


		
		memcpy(m_cache_buffer, ptr_pos+ulTransactLen, (VO_U32)(leftsize-ulTransactLen));
		m_used_size = (VO_S32)(leftsize-ulTransactLen);
		return (ulTransactLen+188);  
	}
}

VO_VOID CvoGenaralDrmMemStream::write_eos()
{

	VO_IrdetoDRM_DecryptINFO	varIrdeto = {0};
	VO_U32   aParamset[8] = {0};
	VO_U32   ulParamCount = 0;
	VO_U32   ulParamData = 0;

	if(m_pGenaralCB->GettDecryptParams(&ulParamCount, &ulParamData) != VO_TRUE)
	{
	    return;
	}
	
	varIrdeto.process = IrdetoDRM_DecryptProcess_END;
	varIrdeto.segmentID = *((VO_U32*)ulParamData);
	aParamset[0] = (VO_U32)(&varIrdeto);		
	m_pGenaralCB->DoDecryptWithParams(NULL, (VO_U32)0, aParamset, 1);
}

VO_S32 CvoGenaralDrmMemStream::get_lasterror()
{
    return m_errorcode;
}

VO_S32   CvoGenaralDrmMemStream::BindDrm(void*  pCallbackPtrs)
{
    m_pGenaralCB = (CvoGenaralDrmCallback* )pCallbackPtrs;
	return 0;
}



