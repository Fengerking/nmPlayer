/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003 - 2009		*
*																		*
************************************************************************/
/*******************************************************************************
File:		ImgEngine.cpp
Written by:   Leon Huang

*******************************************************************************/
#include "ImgEngine.h"
#include "voLog.h"

int  UpdataBuffer(VO_VIDEO_BUFFER *pOutData, int width, int height)
{
	int i;
	int yuv_width[3];  
	int	yuv_height[3];
	int	yuv_size[3];
	switch(pOutData->ColorType)
	{
	case VO_COLOR_RGB888_PLANAR:
	case VO_COLOR_YUV_PLANAR444:		/*!< YUV planar mode:444  vertical sample is 1, horizontal is 1  */
		yuv_width[0] = yuv_width[1] = yuv_width[2] = (width + 15) & ~15;
		yuv_height[0] = yuv_height[1] = yuv_height[2] = (height + 15) & ~15;
		break;
	case VO_COLOR_YUV_PLANAR422_12:	/*!< YUV planar mode:422, vertical sample is 1, horizontal is 2  */
		yuv_width[0] = (width + 15) & ~15;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
		yuv_height[0] = yuv_height[1] = yuv_height[2] = (height + 15) & ~15;
		break;
	case VO_COLOR_YUV_PLANAR422_21:	/*!< YUV planar mode:422  vertical sample is 2, horizontal is 1  */
		yuv_width[0] = yuv_width[1] = yuv_width[2] = (width + 15) & ~15;
		yuv_height[0] = (height + 15) & ~15;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/2;
		break;
	case VO_COLOR_YUV_PLANAR420:		/*!< YUV planar mode:420  vertical sample is 2, horizontal is 2  */
		yuv_width[0] = (width + 31) & ~31;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/2;
		yuv_height[0] = (height + 31) & ~31;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/2;
		break;
	case VO_COLOR_YUV_PLANAR411V:
		yuv_width[0] = (width + 7) & ~7;
		yuv_width[1] = yuv_width[2] = yuv_width[0];
		yuv_height[0] = (height + 31) & ~31;
		yuv_height[1] = yuv_height[2] = yuv_height[0]/4;
		break;
	case VO_COLOR_YUV_PLANAR411:
		yuv_width[0] = (width + 31) & ~31;
		yuv_width[1] = yuv_width[2] = yuv_width[0]/4;
		yuv_height[0] = (height + 7) & ~7;
		yuv_height[1] = yuv_height[2] = yuv_height[0];
		break;
	}

	for(i = 0; i < 3; i++)
	{
		pOutData->Stride[i] = yuv_width[i];
		yuv_size[i] = yuv_width[i] * yuv_height[i];

		pOutData->Buffer[i] = new VO_BYTE[yuv_size[i]];//];(VO_BYTE *)malloc(yuv_size[i]);
		memset(pOutData->Buffer[i], 0, yuv_size[i]);
		if(pOutData->Buffer[i] == NULL)
			return -1;
	}
	return 0;
}

ImgEngine::ImgEngine (VO_PTR hInst,VO_MEM_OPERATOR * pMemOP)
	: CBaseNode (hInst, pMemOP)
	,m_phCodec(NULL)
{
	//LoadDll ();
	//m_ImgBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
	m_sourceImgBuffer.pBuffer = NULL;
	m_curImgBuffer.pBuffer = NULL;
	m_tmpImgBuffer.pBuffer = NULL;
}

ImgEngine::~ImgEngine ()
{
	Uninit();
	RelaeseBuffer();
}
VO_U32 ImgEngine::Init ()
{
	VO_U32 nRC = VO_ERR_NOT_IMPLEMENT;

	//	Uninit ();

		if (LoadLib (NULL) == 0)
		{
			VOLOGE ("LoadLib was failed!");
			return VO_ERR_FAILED;
		}
		nRC = m_ImgDecApi.Init(&m_phCodec,m_nFormat, NULL);
		CHECK_FAIL(nRC);

	return nRC;
}
VO_VOID ImgEngine::RelaeseBuffer()
{
	if(m_nFormat != VO_IMAGE_CodingPNG)
	{
		if(m_sourceImgBuffer.pBuffer)
		{
			VO_VIDEO_BUFFER *pOutData = (VO_VIDEO_BUFFER*)m_sourceImgBuffer.pBuffer;
			if(pOutData->Buffer[0]) delete []pOutData->Buffer[0];
			if(pOutData->Buffer[1]) delete []pOutData->Buffer[1];
			if(pOutData->Buffer[2]) delete []pOutData->Buffer[2];
			pOutData->Buffer[0] = NULL;
			pOutData->Buffer[1] = NULL;
			pOutData->Buffer[2] = NULL;
		}
	}
		if(m_curImgBuffer.pBuffer)			delete []m_curImgBuffer.pBuffer;
		m_curImgBuffer.pBuffer = NULL;

}
VO_U32 ImgEngine::Uninit (void)
{
	VOLOGF ();
	
	//RelaeseBuffer();

	if (m_phCodec != NULL)
	{
		m_ImgDecApi.Uninit (m_phCodec);
		m_phCodec = NULL;
	}
	FreeLib ();
	return 0;
}
int ImgEngine::SetDataSource (void * pSource, int nFlag)
{
	Uninit();
	RelaeseBuffer();

	if(!pSource) 
		return VO_ERR_FAILED;
	char ext[10];
	memset (m_cTitle, 0, sizeof (m_cTitle)); 
#ifdef _WIN32
 	MultiByteToWideChar (CP_ACP, 0, (VO_CHAR*)pSource, -1, m_cTitle, sizeof (m_cTitle));
 #else
	memcpy(m_cTitle, (VO_CHAR*)pSource, sizeof(m_cTitle));
#endif
	sscanf((VO_CHAR*)pSource,"%*[^.].%s",ext);

	VO_U32 nRc = VO_ERR_NONE;
	if(strcmp(ext,VOEDT_IMG_EXTENTION_PNG) >=0)
	{
		m_nFormat = VO_IMAGE_CodingPNG;
	}
	else	if(strcmp(ext,VOEDT_IMG_EXTENTION_JPG) >=0)
	{
		//DO JPG
		m_nFormat = VO_IMAGE_CodingJPEG;
	}
	else 	if(strcmp(ext,VOEDT_IMG_EXTENTION_BMP) >=0)
	{
		//DO BMP
		m_nFormat = VO_IMAGE_CodingBMP;
	}
	else
		return VO_ERR_FAILED;
	

	nRc = Init();
	CHECK_FAIL(nRc);
	CHECK_FAIL(Process());
	CHECK_FAIL(Uninit());
	return nRc;
}
VO_U32 ImgEngine::Process()
{
	VO_U32 nRc = VO_ERR_NONE;
	if (m_phCodec == NULL)
		return VOMP_ERR_Pointer;

	switch(m_nFormat)
	{
	case VO_IMAGE_CodingJPEG:
		nRc = GetJPEGSample((VO_VIDEO_BUFFER*)&ImgBuffer);
		//ImgBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
		break;
	case VO_IMAGE_CodingPNG:
		nRc = GetPNGSample((VO_VIDEO_BUFFER*)&ImgBuffer);
		//ImgBuffer.ColorType = VO_COLOR_YUV_PLANAR420;
		break;
	default:
		nRc = VO_ERR_FAILED;
		break;
	}
	m_sourceImgBuffer.pBuffer =(unsigned char*) &ImgBuffer;
	m_sourceImgBuffer.llTime = 0;
	return nRc;
}

VO_U32 ImgEngine::GetPNGSample(VO_VIDEO_BUFFER *pImgBuffer)
{
	VO_U32 nRc = VO_ERR_NONE;
	if (m_phCodec == NULL)
		return VOMP_ERR_Pointer;

	VO_CODECBUFFER	pInData;
	VO_VIDEO_BUFFER	outdata;
	VO_VIDEO_FORMAT	outformat;
	VO_VIDEO_OUTPUTINFO outInfo;


	nRc = m_ImgDecApi.SetParam(m_phCodec, VO_PID_PNG_INPUTFILE, (VO_PTR)m_cTitle);	
	nRc |=m_ImgDecApi.Process(m_phCodec,&pInData, pImgBuffer,&outInfo);

	m_srcFormat.Height = outInfo.Format.Height;
	m_srcFormat.Width = outInfo.Format.Width;
	m_srcFormat.Type = pImgBuffer->ColorType;

	EX_INIT_BUFFER3(m_srcFormat, &m_curImgBuffer);
	unsigned char* pBuffer1= m_curImgBuffer.pBuffer;
	unsigned char* pBuffer2 = pImgBuffer->Buffer[0];
	for(int i = 0;i< m_srcFormat.Height;i++)
	{
		memcpy(pBuffer1, pBuffer2, m_srcFormat.Width * 4);
		pBuffer1 += m_srcFormat.Width * 4 ;
		pBuffer2 += pImgBuffer->Stride[0];
	}
	//memcpy(m_curImgBuffer.pBuffer, pImgBuffer->Buffer[0], m_curImgBuffer.nSize);
	return nRc;
}
VO_U32 ImgEngine::GetJPEGSample(VO_VIDEO_BUFFER *pImgBuffer)
{
	VO_U32 nRc = VO_ERR_NONE;
	if (m_phCodec == NULL)
		return VOMP_ERR_Pointer;

	nRc = m_ImgDecApi.SetParam(m_phCodec, VO_JPEG_FILE_PATH, (VO_PTR)m_cTitle);	
	int buffer_limite = 30000000;
	int zoomout = 1;
	m_ImgDecApi.SetParam(m_phCodec, VO_JPEG_OUTPUT_ZOOMOUT, &zoomout);
	m_ImgDecApi.SetParam(m_phCodec, VO_JPEG_BUFFER_LIMITED, &buffer_limite);
// 	VO_IV_COLORTYPE ColorType = VO_COLOR_YUV_PLANAR420;
// 	nRc = m_ImgDecApi.SetParam(m_phCodec, VO_JPEG_VIDEO_TYPE, &ColorType);
//	int modenormal = 0;  // 0 : out put 420 ;  1: out = in
//	m_ImgDecApi.SetParam(m_phCodec,VO_PID_DEC_JPEG_SET_NORMAL_CHROMA, &modenormal);
	m_ImgDecApi.GetParam(m_phCodec, VO_PID_JPEG_WIDTH, &m_srcFormat.Width);
	m_ImgDecApi.GetParam(m_phCodec, VO_PID_JPEG_HEIGHT, &m_srcFormat.Height);
	m_ImgDecApi.GetParam(m_phCodec, VO_JPEG_VIDEO_TYPE, &pImgBuffer->ColorType);

	JPEG_IMAGE_FIELD outField;
	outField.start_X = 0; 
	outField.start_Y = 0;	
	outField.end_X = m_srcFormat.Width + outField.start_X;
	outField.end_Y = m_srcFormat.Height + outField.start_Y;

	VO_VIDEO_FORMAT outvf;

	outvf.Width = outField.end_X - outField.start_X;
	outvf.Height = outField.end_Y - outField.start_Y;
	m_ImgDecApi.SetParam(m_phCodec, VO_JPEG_OUTPUT_FIELD, (VO_PTR)(&outField));

	outvf.Width = ((outvf.Width + zoomout - 1) / zoomout + 1) & ~1;
	outvf.Height = ((outvf.Height + zoomout - 1) / zoomout + 1) & ~1;
//	VO_VIDEO_BUFFER ImgBufferp;
//	pImgBuffer->ColorType = VO_COLOR_YUV_PLANAR420;
	if(UpdataBuffer(pImgBuffer, outvf.Width, outvf.Height) < 0)
		nRc = VO_ERR_FAILED;
	else
	{
 		do {
 			nRc = m_ImgDecApi.GetParam(m_phCodec, VO_JPEG_OUTPUT_BUFFER, pImgBuffer); 
		} while(nRc == VO_ERR_JPEGDEC_DECODE_UNFINISHED);
	}
	//delete []ImgBufferp.Buffer;
	return nRc;
}
int ImgEngine::GetParam (int nID, void * pValue)
{
	VO_U32 nRc = VOEDT_ERR_NONE;
	switch(nID)
	{
	case VOEDT_PID_VIDEO_SAMPLE:
		{
			VOLOGI("GetParam");
			VOMP_BUFFERTYPE **vvb = (VOMP_BUFFERTYPE**)pValue;
			if(!m_curImgBuffer.pBuffer)	{

				if(m_nFormat != VO_IMAGE_CodingPNG)	{
					CSampleTransformer transfer;
					transfer.SetLibOp(m_pLibOP);
					transfer.Init();

					EX_INIT_BUFFER3(m_outFormat, &m_tmpImgBuffer, 2);
					VO_VIDEO_FORMAT in,out;
					in.Height =  m_srcFormat.Height;
					in.Width = m_srcFormat.Width;
					out.Height = m_outFormat.Height;
					out.Width = m_outFormat.Width;
					transfer.DoTransformer2((VO_VIDEO_BUFFER*)m_sourceImgBuffer.pBuffer, (VO_VIDEO_BUFFER*)m_tmpImgBuffer.pBuffer, in , out, 1);
					m_curImgBuffer.llTime = m_tmpImgBuffer.llTime;
					m_curImgBuffer.nFlag = m_tmpImgBuffer.nFlag;
					m_curImgBuffer.nReserve = m_tmpImgBuffer.nReserve;
					m_curImgBuffer.nSize = m_tmpImgBuffer.nSize;
					m_curImgBuffer.pBuffer = ((VO_VIDEO_BUFFER*) m_tmpImgBuffer.pBuffer)->Buffer[0];
					m_curImgBuffer.pData = m_tmpImgBuffer.pData;
				}
			}
			m_curImgBuffer.llTime = 0;
			*vvb = &m_curImgBuffer;
			nRc = VOEDT_ERR_NONE;
	
			break;
		}	
	case VOMP_PID_VIDEO_FORMAT:
		{
			VOMP_VIDEO_FORMAT *vf = (VOMP_VIDEO_FORMAT*)pValue;
			memcpy(vf,&m_srcFormat,sizeof(VOMP_VIDEO_FORMAT));
			nRc = VOEDT_ERR_NONE;
			break;
		}
	default:
		nRc = VOEDT_ERR_PID_NOTFOUND;
		break;
	}
	return nRc;
}

int ImgEngine::SetParam (int nID, void * pValue)
{

	VO_U32 nRc = VOMP_ERR_None;
	switch(nID)
	{
	case VOEDT_PID_VIDEO_FORMAT:
		{
			VOMP_VIDEO_FORMAT *ff = (VOMP_VIDEO_FORMAT*)pValue;
			VOLOGI("Target(%d,%d,%d),Source(%d,%d,%d)",ff->Width, ff->Height, ff->Type, m_outFormat.Width, m_outFormat.Height, m_outFormat.Type);
			if(!(m_outFormat.Height == ff->Height && m_outFormat.Width == ff->Width && m_outFormat.Type == ff->Type))
			{
				VOLOGI("Reset videoFormat");
				EX_Release_Buffer(&m_curImgBuffer);

				m_outFormat.Height = ff->Height;
				m_outFormat.Width = ff->Width;
				m_outFormat.Type = ff->Type;
			}
			return nRc;
		}
	}
	return m_ImgDecApi.SetParam (m_phCodec, nID, pValue);
}


VO_U32 ImgEngine::LoadLib (VO_HANDLE hInst)
{
	VO_PCHAR pDllFile = NULL;
	VO_PCHAR pApiName = NULL;
	
	switch(m_nFormat)
	{
	case VO_IMAGE_CodingJPEG:
		vostrcpy (m_szDllFile, _T("voJPEGDec"));
		vostrcpy (m_szAPIName, _T("voGetJPEGDecAPI"));
		break;
	case VO_IMAGE_CodingGIF:
		break;
	case VO_IMAGE_CodingPNG:
		vostrcpy (m_szDllFile, _T("voPNGDec"));
		vostrcpy (m_szAPIName, _T("voGetPNGDecAPI"));
		break;
	case VO_IMAGE_CodingBMP:
		break;
	default: 
		return VO_ERR_FAILED;	
	}

#if defined _WIN32
	if (pDllFile != NULL)
	{
		memset (m_szDllFile, 0, sizeof (m_szDllFile));
		MultiByteToWideChar (CP_ACP, 0, pDllFile, -1, m_szDllFile, sizeof (m_szDllFile));
	}
	vostrcat (m_szDllFile, _T(".Dll"));

	if (pApiName != NULL)
	{
		memset (m_szAPIName, 0, sizeof (m_szAPIName));
		MultiByteToWideChar (CP_ACP, 0, pApiName, -1, m_szAPIName, sizeof (m_szAPIName));
	}
#elif defined LINUX
	if (pDllFile != NULL)
		vostrcpy (m_szDllFile, pDllFile);
	vostrcat (m_szDllFile, _T(".so"));

	if (pApiName != NULL)
		vostrcpy (m_szAPIName, pApiName);
#endif

	if (CBaseNode::LoadLib (m_hInst) == 0)
	{
		VOLOGE ("CBaseNode::LoadLib File %s, API %s Failed.", m_szDllFile, m_szAPIName);
		return 0;
	}

	VOGETIMGDECAPI pAPI = (VOGETIMGDECAPI) m_pAPIEntry;
	pAPI (&m_ImgDecApi);


	return 1;
}
