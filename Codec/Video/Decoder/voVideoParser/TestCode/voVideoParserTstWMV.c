#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "voVideoParser.h"

#define IN_BUFF_LEN            (1024 * 1024 * 2)
#define IN_BUFF_LEN2           (IN_BUFF_LEN/2)
/**************************************/
int iRcvVer =0;
int	  leftLen = 0;
int GetHeadParam ( VO_BYTE *pInputBuff, int lInBuffLen, VO_BITMAPINFOHEADER *pBmpInfo )
{
#define MAKEFOURCC_WMV(ch0, ch1, ch2, ch3) \
        ((unsigned int)(unsigned char)(ch0) | ((unsigned int)(unsigned char)(ch1) << 8) |   \
        ((unsigned int)(unsigned char)(ch2) << 16) | ((unsigned int)(unsigned char)(ch3) << 24 ))

#define mmioFOURCC_WMV(ch0, ch1, ch2, ch3)  MAKEFOURCC_WMV(ch0, ch1, ch2, ch3)

#define FOURCC_WMV1_WMV     mmioFOURCC_WMV('W','M','V','1') /* Window Media Video v7 */
#define FOURCC_wmv1_WMV     mmioFOURCC_WMV('w','m','v','1')
#define FOURCC_WMV2_WMV     mmioFOURCC_WMV('W','M','V','2') /* Window Media Video v8 */
#define FOURCC_wmv2_WMV     mmioFOURCC_WMV('w','m','v','2')
#define FOURCC_WMV3_WMV     mmioFOURCC_WMV('W','M','V','3') /* Window Media Video v9 */
#define FOURCC_wmv3_WMV     mmioFOURCC_WMV('w','m','v','3')
#define FOURCC_WMVA_WMV     mmioFOURCC_WMV('W','M','V','A') /* Window Media Video v9 Advance Profile */
#define FOURCC_wmva_WMV     mmioFOURCC_WMV('w','m','v','a')
#define FOURCC_WMVP_WMV     mmioFOURCC_WMV('W','M','V','P') /* Window Media Video v9 Image */
#define FOURCC_wmvp_WMV     mmioFOURCC_WMV('w','m','v','p')
#define FOURCC_WVP2_WMV     mmioFOURCC_WMV('W','V','P','2') /* Window Media Video v9.1 Image v2 */
#define FOURCC_wvp2_WMV     mmioFOURCC_WMV('w','v','p','2')
#define FOURCC_MP4S_WMV     mmioFOURCC_WMV('M','P','4','S') /* Microsoft ISO MPEG4 Video Codec v1 */
#define FOURCC_mp4s_WMV     mmioFOURCC_WMV('m','p','4','s')
#define FOURCC_MP42_WMV     mmioFOURCC_WMV('M','P','4','2') /* Microsoft MPEG4 Video Codec v2 */
#define FOURCC_mp42_WMV     mmioFOURCC_WMV('m','p','4','2')
#define FOURCC_MP43_WMV     mmioFOURCC_WMV('M','P','4','3') /* Microsoft MPEG4 Video Codec v3 */
#define FOURCC_mp43_WMV     mmioFOURCC_WMV('m','p','4','3')
#define FOURCC_WVC1_WMV     mmioFOURCC_WMV('W','V','C','1') /* VC-1 */
#define FOURCC_wvc1_WMV     mmioFOURCC_WMV('w','v','c','1')

	int	   i;
	int    iCodecVer = 0;
    int    hdrext = 0;
	int    ext_size = 0;
	unsigned char  *pHdrExt = NULL;
	unsigned char  *pInBuff = pInputBuff;
	unsigned int	dwRcvNumFrames = 0;
	unsigned int	uiPicHorizSize = 0, uiPicVertSize = 0;

	dwRcvNumFrames  = (unsigned char)*pInBuff++;        
	dwRcvNumFrames |= ((unsigned char)(*pInBuff++) << 8);
	dwRcvNumFrames |= ((unsigned char)(*pInBuff++) << 16);
	dwRcvNumFrames |= ((unsigned char)(*pInBuff++) << 24);

    iRcvVer         = (dwRcvNumFrames>>30) & 0x1; /* RCV is V2 format or not. */
    iCodecVer       = dwRcvNumFrames>>24;
    
    hdrext=(iCodecVer>>7)&0x1;
    if (iRcvVer == 0) {
        iCodecVer&=0x7f;
    } else {
        iCodecVer&=0x3f;
    }

    if (iCodecVer == 0) {
        iCodecVer = FOURCC_WMV1_WMV;
    } else if (iCodecVer == 1) {
        iCodecVer = FOURCC_MP43_WMV;
    } else if (iCodecVer == 2) {
        iCodecVer = FOURCC_WMV2_WMV;
    } else if (iCodecVer== 3) {
        iCodecVer = FOURCC_MP42_WMV;
    } else if (iCodecVer == 4) {
        iCodecVer = FOURCC_MP4S_WMV;
    } else if (iCodecVer == 5) {
        iCodecVer = FOURCC_WMV3_WMV;
    } else if (iCodecVer == 6) {
        iCodecVer = FOURCC_WMVA_WMV;
    } else if (iCodecVer == 7) {
        iCodecVer = FOURCC_WVP2_WMV;
    } else if (iCodecVer == 8) {
        iCodecVer = FOURCC_WVC1_WMV;
    } else { 
		return -1;
	}


    if( hdrext != 0 ) {
		ext_size  = (unsigned char)*pInBuff++;
		ext_size |= (unsigned char)(*pInBuff++) << 8;
		ext_size |= (unsigned char)(*pInBuff++) << 16;
		ext_size |= (unsigned char)(*pInBuff++) << 24;

		pHdrExt = (unsigned char *)pBmpInfo + sizeof(VO_BITMAPINFOHEADER);
		for ( i = 0; i < ext_size; i ++ ) {
			*pHdrExt++ = *pInBuff++;
		}

		pBmpInfo->biSize = sizeof(VO_BITMAPINFOHEADER) + ext_size;
    }
	pBmpInfo->biCompression = iCodecVer;

	uiPicVertSize  = (unsigned char)(*pInBuff++);
	uiPicVertSize |= (unsigned char)(*pInBuff++) << 8;
	uiPicVertSize |= (unsigned char)(*pInBuff++) << 16;
	uiPicVertSize |= (unsigned char)(*pInBuff++) << 24;
	uiPicHorizSize  = (unsigned char)(*pInBuff++);
	uiPicHorizSize |= (unsigned char)(*pInBuff++) << 8;
	uiPicHorizSize |= (unsigned char)(*pInBuff++) << 16;
	uiPicHorizSize |= (unsigned char)(*pInBuff++) << 24;

	pBmpInfo->biWidth = uiPicHorizSize;
	pBmpInfo->biHeight = uiPicVertSize;

    if ( iRcvVer == 1 ) {   
		/* Jump the rcv_additional_header_size,hdr_buffer, bitrate, framerate. */ 
		pInBuff   += 16; 
    }

	return (int)( (unsigned int)pInBuff - (unsigned int)pInputBuff );
}
int GetElementaryChunk(unsigned char *pInputBuf, VO_CODECBUFFER *pInData, unsigned char *pHeaderParam)
{
#define START_CODE_LEN	4

    unsigned char *pBufStart, *pBufEnd;
	unsigned char  chunkType,firstChunkType;
	int            nIsSeqHeader = 0;

    /* Skip over any zero-stuffing bytes */
    for (pBufStart = pInputBuf;
         pBufStart - pInputBuf < leftLen && *pBufStart == 0;
         pBufStart ++);

    /* Check for a start code */
    if (pBufStart - pInputBuf     < 2		||
        pBufStart - pInputBuf + 2 > leftLen	||
        *pBufStart != 0x01)
    {
        /* No start code found */
	printf("\nThis is the last frame.");
        return -1;
    }

	firstChunkType = pBufStart[1];
    pBufEnd = pBufStart + 2;
    pBufStart += 2;          /* Point at the first byte in the encapsulated data */

    /* Scan for the start code of next chunk or end of buffer */

	if( firstChunkType == 0x0F ) { //Start code of sequence header.
		while (pBufEnd - pInputBuf < leftLen)  {
			if ( pBufEnd[0] == 0x00 && pBufEnd[1] == 0x00 && pBufEnd[2] == 0x01 ) {
				chunkType = pBufEnd[3];
				if( chunkType == 0x0D /*|| chunkType == 0x0F*/ )
					break;
			}
			pBufEnd++;
		}
	}
	else {
		while (pBufEnd - pInputBuf < leftLen)  {
			if ( pBufEnd[0] == 0x00 && pBufEnd[1] == 0x00 && pBufEnd[2] == 0x01 ) {
				chunkType = pBufEnd[3];
				if( chunkType == 0x0D )
					break;
			}
			pBufEnd ++; 
		}
	}


    if (pBufEnd-pInputBuf <= leftLen) {  //zou 330 <   ----> <=
        /* pBufEnd points at the last byte we want to include in this chunk */
        pBufEnd --;  /* scan back over zero bytes */
    }
    else{
        /* End of buffer, No start code found */
        return -1;
    }

	/* Get data and length of next chunk */
	if( firstChunkType == 0x0F ){ //Sequence header.
		pInData->Buffer = pHeaderParam;
		pInData->Length = (pBufEnd - pInputBuf + 1) + 1;
		if(pHeaderParam){
			*pHeaderParam++ = 0x25;	 //Insert ASF binding byte.
			memcpy(pHeaderParam, pInputBuf, pBufEnd - pInputBuf + 1);
		}
	}
	else {
		pInData->Buffer = pInputBuf;
		pInData->Length = pBufEnd - pInputBuf + 1;
	}
	return 0;
}
int	IsInputTypeVC1(unsigned char *pInputBuf)
{
	if( pInputBuf[0] == 0x00 && pInputBuf[1] == 0x00 && 
		pInputBuf[2] == 0x01 && pInputBuf[3] == 0x0F ) {
		return 1;
	}
	return 0;
}

int ParserWMV()
{
	VO_S32 ret =0,g_InputType=0,timestap =0;
	VO_U32 iFrameLen =0,fileSize=0,iLenAdd=0;
	VO_HANDLE hCodec;
	VO_CODECBUFFER pInData;
	VO_VIDEO_PARSERAPI voVideoParser;	
	VO_CHAR* inFileName = "E:/MyResource/Video/clips/WMV/error/vc1_007_AP_InterFrame_D0B1M0U1V1Q3P1L1E1e0R0r0_176x144.rcv";
	VO_U8* pHeaderParam;
	VO_U8* inputBuf;
	VO_U8* pTmp = NULL;
	VO_CODECBUFFER		inDataHead;
	VO_S32   frameNumber =0 ,tempLen =0;

	VO_U32 nVersion=0,nProfile =0;
	VO_U32 nFrameType = 0;
	VO_U32 nWidth=0,nHeight =0;
	VO_U32 isInterlace =0;
	VO_U32 IsRefFrame =0;
	VO_U32 isVC1 =0;

	FILE* inFile = fopen ( inFileName, "rb" );
	if (!inFile) {
		printf("\nError: Cannot open input testing file! \n");
		exit(0);
	}

	fseek(inFile, 0, SEEK_END);
	fileSize = ftell(inFile);
	fseek(inFile, 0, SEEK_SET);

	memset( &inDataHead, 0, sizeof(inDataHead) );

	pHeaderParam = (VO_U8 *)malloc (sizeof(VO_BITMAPINFOHEADER) + 1024);
	memset( pHeaderParam, 0, sizeof(VO_BITMAPINFOHEADER) + 1024 );

	inputBuf = (VO_U8 *)malloc(IN_BUFF_LEN * sizeof (VO_U8));
	if( !(leftLen = fread(inputBuf, 1, IN_BUFF_LEN, inFile)) ) 
		goto End2;

	if( IsInputTypeVC1(inputBuf) ){
		int nRC = GetElementaryChunk(inputBuf, &inDataHead, pHeaderParam);
		g_InputType = 1;				
		iFrameLen = inDataHead.Length - 1;
		if ( /*-1 == nRC ||*/ 0 >= iFrameLen ) {
			goto End;
		}
	}
	else 
	{
		/* Get the header parameter for voWMV9DecSetParameter(). */
		iFrameLen = GetHeadParam ( inputBuf, leftLen, (VO_BITMAPINFOHEADER *)pHeaderParam );
		if ( -1 == iFrameLen ) {
			goto End;
		}
		inDataHead.Buffer = (VO_PBYTE)pHeaderParam;
		inDataHead.Length = ((VO_BITMAPINFOHEADER *)pHeaderParam)->biSize; 
	}

	ret = voGetVideoParserAPI(&voVideoParser,VO_VIDEO_CodingWMV);

	voVideoParser.Init(&hCodec);

	pInData.Buffer = inDataHead.Buffer;
	pInData.Length = inDataHead.Length;
	voVideoParser.Process(hCodec,&pInData);

	pInData.Buffer = inputBuf + iFrameLen;
	fileSize -= iFrameLen;
	leftLen  -= iFrameLen;

	while( (fileSize>0) && (frameNumber < 1000) ) 
	{
		if( g_InputType )
		{
			int nRC = GetElementaryChunk(pInData.Buffer, &pInData, NULL);
			iLenAdd = 0;
			iFrameLen = pInData.Length;
		}
		else 
		{
			pTmp = pInData.Buffer;
			iFrameLen  = *pTmp++;      
			iFrameLen |= (*pTmp++) << 8;
			iFrameLen |= (*pTmp++) << 16;
			iFrameLen |= (*pTmp++) << 24;
			iLenAdd = 4;
			/* RCV is V2 format. */
			if ( 1 == iRcvVer ) { 
				iFrameLen &= 0x0fffffff;
				/* Jump the dwTimeStamp. */
				iLenAdd += 4;
				timestap  = *pTmp;                
		
				timestap |= (*(pTmp+1)) << 8;
				timestap |= (*(pTmp+2)) << 16;
				timestap |= (*(pTmp+3)) << 24;

			}
			pInData.Length  = iFrameLen;
			pInData.Buffer += iLenAdd;
		}

		ret = voVideoParser.Process(hCodec,&pInData);

		{
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_VERSION, &nVersion);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_PROFILE, &nProfile);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_FRAMETYPE, &nFrameType);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_WIDTH, &nWidth);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_HEIGHT, &nHeight);

			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_INTERLANCE, &isInterlace);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_REFFRAME, &IsRefFrame);
			ret = voVideoParser.GetParam(hCodec, VO_PID_VIDEOPARSER_ISVC1, &isVC1);

			{
				static int n =0;
				FILE* fp =fopen("E:/MyResource/Video/clips/WMV/error/videoparser.txt","a");
				//fprintf(fp,"nVersion = %d nProfile= %d  \n",nVersion, nProfile);
				fprintf(fp,"%d  nFrameType = %d nWidth= %d  nHeight=%d \n",n++,nFrameType, nWidth,nHeight);
				//fprintf(fp,"isInterlace = %d IsRefFrame= %d  isVC1=%d \n\n",isInterlace, IsRefFrame,isVC1);
				fclose(fp);
			}
	
		}

		iLenAdd += iFrameLen;
		fileSize -= iLenAdd;
		leftLen  -= iLenAdd;

		/* Update input structure. */
		if( (fileSize > leftLen) && (leftLen > 0) && (leftLen < IN_BUFF_LEN2) ) {
			memmove(inputBuf, inputBuf + (IN_BUFF_LEN - leftLen),  leftLen);
			tempLen       = fread(inputBuf + leftLen, 1, (IN_BUFF_LEN - leftLen), inFile);
			leftLen       = tempLen + leftLen;
			pInData.Buffer = inputBuf;
		} 
		else {
			pInData.Buffer += iFrameLen;
		}

	}

	voVideoParser.Uninit(hCodec);

End2:
End:
	return 0;
}