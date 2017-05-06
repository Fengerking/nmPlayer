
#include "voDrmCallback.h"
#include "voOSFunc.h"
#include "voLog.h"
#include "base64.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif


CvoDrmCallback::CvoDrmCallback( VO_PTR pDrmCB)
:CvoBaseDrmCallback((VO_SOURCEDRM_CALLBACK*)pDrmCB)
{
	m_bMediaStop = VO_FALSE;
	m_nResponseSize = 0;
	m_pResponseBuf = NULL;
}

CvoDrmCallback::~CvoDrmCallback()
{
	m_bMediaStop = VO_TRUE;
	
	if(m_pResponseBuf)delete m_pResponseBuf;
	m_pResponseBuf = NULL;

}

VO_U32 CvoDrmCallback::DRMInfo_PlayReady(VO_PBYTE *ppDrmHeader, VO_U32 &nDrmHeader, int isNeedConvert /*= 1*/)
{
	VOLOGR("in DRMInfo_PlayReady");
	VO_U32 size = nDrmHeader;
 //	FILE *fff =NULL;
 //	fff = fopen("d:/drm_header","w");
 //	fwrite((char*)*pDrmHeader,1,nDrmHeader,fff);
	VOLOGR("DRMHEADER:%s",*ppDrmHeader);
	if(isNeedConvert)
	{
		unsigned char *decoder = Base64Decode((char*)*ppDrmHeader, size);
		delete []*ppDrmHeader; *ppDrmHeader = NULL;
		*ppDrmHeader = decoder;
	}
	nDrmHeader = (VO_U32)size;
	//fclose(fff);
//	fff = fopen("d:/drm_decoder","w");
//  	fwrite(decoder,1,nDrmHeader,fff);
 //	fclose(fff);

		VO_U32 nRc = 0;
#ifdef _TEST
	FILE *fff =NULL;
	fff = fopen("d:/license_old","r");
	int nchallenge = 6936; int url = 58; int  header = 66;
	char *pchallenge = new char[nchallenge+1];char purl[58];char pheader[66];
	fread(pchallenge,1,nchallenge,fff);
	pchallenge[nchallenge] = 0x00;
	fread(purl,1,url,fff);
	purl[url-1] = 0x00;
	fread(pheader,1,header,fff);
	pheader[header-1] = 0x00;
	VO_PCHAR ppcMimetype;VO_PBYTE ppResponse;VO_U32 pnResponse; VO_BOOL pbCancel;VO_PTR pUseData;
	 	Get_License_Response((VO_PBYTE )pchallenge,nchallenge,purl,url,pheader,header,&ppcMimetype ,&ppResponse,&pnResponse,&pbCancel,this);
	delete []pchallenge;	
#else
	nRc = CvoBaseDrmCallback::DRMInfo_PlayReady(*ppDrmHeader,nDrmHeader,(VO_GET_LICENSE_RESPONSE)Get_License_Response,this);

	if(m_pResponseBuf) delete []m_pResponseBuf;
	m_pResponseBuf = NULL;
#endif	
	VOLOGR("out DRMInfo_PlayReady.Result:0x%08x",nRc);
	return nRc;

}

VO_U32 VO_API CvoDrmCallback::Get_License_Response(VO_PBYTE pChallenge, VO_U32 nChallenge, VO_PCHAR pcUrl, VO_U32 nUrl, VO_PCHAR pcHttpHeader, VO_U32 nHttpHeader, 
												  VO_PCHAR* ppcMimetype, VO_PBYTE* ppResponse, VO_U32* pnResponse, VO_BOOL* pbCancel,VO_PTR pUseData)
{
 	VOLOGR("in Get_License_Response");
	//if(*pbCancel ) return VO_ERR_NONE;
	VO_U32 nRc = VO_ERR_FAILED;
	
	CvoDrmCallback *drm = (CvoDrmCallback*)pUseData;
	if(drm->m_pResponseBuf) delete drm->m_pResponseBuf;
	drm->m_pResponseBuf = NULL;
	drm->m_nResponseSize = 0;
	//2. callback drm to get request url

	//3. request & wait for response	
	VOLOGR("Get_License_Response 1");
	char *pchallenge = new char[nChallenge+1];
	memcpy(pchallenge,pChallenge,nChallenge);
	pchallenge[nChallenge] = 0x00;
	char *pheader = new char[nHttpHeader+1];
	memcpy(pheader,pcHttpHeader,nHttpHeader);
	pheader[nHttpHeader] = 0x00;
	char *purl = new char[nUrl+1];
	memcpy(purl,pcUrl,nUrl);
	purl[nUrl] = 0x00;
	VOLOGR("Get_License_Response 12");
	if( !drm->m_http.startdownload( (VO_PBYTE )pchallenge , nChallenge,purl,nUrl,pheader,nHttpHeader,DOWNLOAD2MEM,VO_HTTP_POST ) )
	{
		VOLOGE("Get_License_Response fail");
		nRc = VO_ERR_FAILED;
	}
	else
	{
		VO_S64 read_size = 0; 

		VOLOGR("Get_License_Response 2");

		drm->m_nResponseSize = VO_U32(read_size = drm->m_http.get_content_length());

		drm->m_pResponseBuf = new VO_BYTE[ drm->m_nResponseSize + 1 ];
		memset( drm->m_pResponseBuf , 0 , drm->m_nResponseSize + 1 );

		VO_PBYTE ptr = drm->m_pResponseBuf;
		
		while( read_size != 0)
		{
			 if( drm->m_bMediaStop)break;

			voOS_Sleep(10);
			VO_S64 size = drm->m_http.read( ptr , read_size );

			if( size == -2 )
				continue;

			if( size == -1 )
			{	
				
				break;
			}

			read_size -= size;
			ptr = ptr + size;
		}
		
		*ppcMimetype = drm->m_http.get_mime_type();
		*ppResponse = drm->m_pResponseBuf;
		*pnResponse = drm->m_nResponseSize;
		
		VOLOGR("Reponse MimeType:%s, Size:%d, Content:%s",*ppcMimetype,*pnResponse,*ppResponse);
		nRc = VO_ERR_NONE;
// #ifndef _WIN32
// 		FILE *fff =NULL;
// 		fff = fopen("data/local/ff/licensex","wb");
// 		if(fff) fwrite(drm->m_pResponseBuf,1,(int)*pnResponse,fff);
// 		fclose(fff);
// #endif
		VOLOGR("+ closedownload");
		drm->m_http.closedownload();
		VOLOGR("- closedownload");

	}
	delete []pchallenge; delete []pheader;delete []purl;
	
	VOLOGR("out Get_License_Response");
	return nRc;
}


VO_U32	CvoDrmCallback::DRMData_PlayRead_PacketData(VO_PBYTE pData, VO_U32 nSize, VO_PBYTE pInitializationVector,
																								VO_U32    nIVsize,VO_U32    nBlockOffset,VO_U8		btByteOffset)
{
	VO_PLAYREADY_AESCTR_INFO aesctr_info;
	if(pInitializationVector != NULL && nIVsize > 0)
		memcpy(aesctr_info.btInitializationVector,pInitializationVector,sizeof(aesctr_info.btInitializationVector));
	aesctr_info.btByteOffset = btByteOffset;
	aesctr_info.nBlockOffset = nBlockOffset;
	return CvoBaseDrmCallback::DRMData_PlayRead_PacketData(pData,nSize,(VO_PTR)&aesctr_info);
}


VO_U32 CvoDrmCallback::DRMInfo_PlayReady_for_Discretix(VO_PBYTE pBuffer, VO_U32 nSize)
{
	VO_U32 nRc = CvoBaseDrmCallback::DRMInfo_PlayReady(pBuffer,nSize,NULL,NULL);
	return nRc;
}

VO_U32	CvoDrmCallback::DRMData_PlayRead_PacketData_for_Discretix(VO_PBYTE pData, VO_U32 nSize, VO_PBYTE pSampleEncryptionBox,
													VO_U32    uSampleEncryptionBoxSize,VO_U32    uSampleIndex,VO_U32		uTrackId)
{
	VO_PIFFFRAGMENT_SAMPLEDECYPTION_INFO piff_info;
	piff_info.pSampleEncryptionBox = pSampleEncryptionBox;
	piff_info.uSampleEncryptionBoxSize = uSampleEncryptionBoxSize;
	piff_info.uSampleIndex = uSampleIndex;
	piff_info.uTrackId = uTrackId;
	return CvoBaseDrmCallback::DRMData_PlayRead_PacketData(pData,nSize,(VO_PTR)&piff_info);
}
