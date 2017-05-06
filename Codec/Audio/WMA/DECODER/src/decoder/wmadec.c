/**********************************************************************
*
* VisualOn voplayer
*
* CopyRight (C) VisualOn SoftWare Co., Ltd. All rights are reserved.
*
***********************************************************************/

/*********************************************************************
*
* Filename:	
*		wmadec.c
* 
* Abstact:	
*
*		wma decoder AP define file.
*
* Author:
*
*		Witten Wen 2007
*
* Revision History:
*
******************************************************/
#include <stdio.h>
#include "voWMADecID.h"
#include "voWMA.h"
#ifndef RVDS
#include "voCheck.h"
#endif
#include "macros.h"
#include "wmaerror.h"
#include "wmaprodecS_api.h"
#include "msaudiodec.h"
#include "wmamalloc.h"

#ifdef DEBUG_LINUX
#include <utils/Log.h>
#define LOG_TAG "wmamalloc"
#endif

#define RETURN_ERROR_MAPS(result)			\
	switch(result){							\
	case WMA_E_OUTOFMEMORY:					\
		return VO_ERR_OUTOF_MEMORY;			\
	case WMA_E_INVALIDARG:					\
		return VO_ERR_INVALID_ARG;			\
	case WMA_E_NOTSUPPORTED:				\
		return VO_ERR_WMA_NOTSUPPORT;		\
	case WMA_E_WRONGSTATE:					\
		return VO_ERR_WRONG_STATUS;			\
	case WMA_E_BUFFEROVERFLOW:				\
		return VO_ERR_OUTOF_MEMORY;			\
	default:								\
		return VO_ERR_FAILED;				\
	}

#define BOX				0
#define DLLTEST			0
#define TESTPERFORMANCE	0
#define TESTINPUT		0
#define OUTINPUTRAWDATA	0
#define TESTCOEFDATA	0
#define ENABLECHECK		1
#define SECONDS180LIMIT	0
#define NOTUSETEST		0

#if (BOX || TESTPERFORMANCE)
#include <windows.h>
#endif
//WMAProRawDecState decState;
//WMAPlayerInfo PlayerInfo;
//WMAFormat WMAForm;
//WMA_I32   Isheader = 0; 
//WMA_U32 cbDst = 0;
//WMA_U16 cSampleReady = 0;
#if OUTINPUTRAWDATA
FILE *faudio;
int first = 1;
//static char *vop_file = "D:\\Numen\\trunk\\Codec\\Audio\\WMA10\\DECODER\\hrdout\\output.hrd";
static char *vop_file = "D:\\music\\testfile\\output.hrd";

__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
		fclose(faudio);
}

int open_vop_output(void)
{
	if ((faudio = fopen(vop_file,  "wb")) == NULL)
		return -1;
	else
		return 0;
}

int
write_vop_output (unsigned char * output_data, int output_size) 
{
	if (fwrite(output_data, output_size, 1, faudio) != 1) 
	{
		shutdown_output();
		return -1;
	}
	return 0;
}
void
close_vop_output ( void ) {
	if (faudio == NULL)
		return;
	shutdown_output();
}
#endif
#if DLLTEST | TESTINPUT | TESTCOEFDATA
FILE *faudio;
unsigned long int wav_size;
static int rate = 44100;//11025;//16000;//44100;
int first = 1;
typedef struct VOPCMFormat{
	int channelcount;
	int samplecount;
	int samplerate;
}VOPCMFormat;
struct VOPCMFormat pcmformat;
#ifdef _WIN32_
static char *wav_file = ".\\output.wav";
#else
//static char *wav_file = "\\Storage Card\\wma\\output.wav";
//static char *wav_file = "\\wma\\output.wav";
//static char *wav_file = "\\SDMMC\\wma\\output.wav";
static char *wav_file = "\\ResidentFlash\\aatest\\output.wav";
//static char *wav_file = "E:\\wma10_rvds22\\wma10-rvds22\\output.wav";
#endif
__inline void shutdown_output ( void ) 
{
	printf("Shutting Down Sound System\n");
	if (faudio != NULL)
		fclose(faudio);
}

int
open_wav_output ( void ) {

	char wav_hdr[] = { 0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00,
				0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
				0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
				0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
				0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
				0x00, 0x00, 0x00, 0x00 };

	if (wav_file[0] == '\0')
		return -1;
	//if ((faudio = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC | O_BINARY))) < 0)
	if ((faudio = fopen(wav_file,  "wb")) == NULL)
	{
		return -1;
	} else {
		unsigned long int bytes_per_sec, bytes_per_sample;

		//chaanel count
		wav_hdr[22] = (char)((pcmformat.channelcount) & 0xFF);
		//sample rate
		wav_hdr[24] = (char)((pcmformat.samplerate) & 0xFF);
		wav_hdr[25] = (char)((pcmformat.samplerate >> 8) & 0xFF);
		wav_hdr[26] = (char)((pcmformat.samplerate >> 16) & 0xFF);
		wav_hdr[27] = (char)((pcmformat.samplerate >> 24) & 0xFF);
		//data remited rate 
		bytes_per_sample = (pcmformat.samplecount * pcmformat.channelcount) >> 3;
		bytes_per_sec = pcmformat.samplerate * bytes_per_sample;
		wav_hdr[28] = (char)((bytes_per_sec) & 0xFF);
		wav_hdr[29] = (char)((bytes_per_sec >> 8) & 0xFF);
		wav_hdr[30] = (char)((bytes_per_sec >> 16) & 0xFF);
		wav_hdr[31] = (char)((bytes_per_sec >> 24) & 0xFF);
		//byte count per sample
		wav_hdr[32] = (char)((bytes_per_sample) & 0xFF);
		//sample bits
		wav_hdr[34] = (char)((pcmformat.samplecount) & 0xFF);
	}
	wav_size = fwrite(&wav_hdr, 44, 1, faudio);
	if ( wav_size != 1) 
	{
		shutdown_output();
		return -1;
	}

	wav_size = 0;
//	send_output = write_wav_output;
//	close_output = close_wav_output;
	return 0;
}

int
write_wav_output (unsigned char * output_data, int output_size) 
{
	if (fwrite(output_data, output_size, 1, faudio) != 1) 
	{
		shutdown_output();
		return -1;
	}

	wav_size += output_size;
	return 0;
}

void
close_wav_output ( void ) {
	char wav_count[4];
	if (faudio == NULL)
		return;

	wav_count[0] = (wav_size) & 0xFF;
	wav_count[1] = (wav_size >> 8) & 0xFF;
	wav_count[2] = (wav_size >> 16) & 0xFF;
	wav_count[3] = (wav_size >> 24) & 0xFF;
	fseek(faudio, 40, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);

	wav_size += 36;
	wav_count[0] = (wav_size) & 0xFF;
	wav_count[1] = (wav_size >> 8) & 0xFF;
	wav_count[2] = (wav_size >> 16) & 0xFF;
	wav_count[3] = (wav_size >> 24) & 0xFF;
	fseek(faudio, 4, SEEK_SET);
	fwrite(&wav_count, 4, 1, faudio);

	shutdown_output();
}
#endif

#if SECONDS180LIMIT
#define LIMITTIME	3 * 60	//
	int outlength = 0;//ms
	unsigned int LimitLength = 0x1E47F40;//LIMITTIME * 2 * 2 * 44100	 
 //	(3*60*1000)//180000, 3 minutes
#endif

VO_PTR	g_hvommPlayInst = NULL;
VO_U32 VO_API voWMADecInit(VO_HANDLE * phCodec, VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)//HVOCODEC *phCodec)
{
	VO_U32 nRet = 0;
#if ENABLECHECK
	#if defined (_MAC_OS)
	#else
		VO_PTR pTemp;
	#endif
#endif

  WMARESULT hr = 0;   
	VO_MEM_OPERATOR *vopMemOP;
	CAudioObjectDecoder *paudec;

#if SECONDS180LIMIT
	 outlength = 0;
#endif

#if OUTINPUTRAWDATA
	if (open_vop_output() == -1){
		return VO_ERR_OUTPUT_BUFFER_SMALL;
	}
#endif
	if(vType != VO_AUDIO_CodingWMA)
		return VO_ERR_INVALID_ARG;

	if(pUserData ==NULL || (pUserData->memflag & 0x0F) != VO_IMF_USERMEMOPERATOR || pUserData->memData == NULL )
	{
    return VO_ERR_INVALID_ARG;
		/*hr = cmnMemFillPointer(VO_AUDIO_CodingWMA);
		vopMemOP = &g_memOP;*/
	}
	else
	{
		vopMemOP = (VO_MEM_OPERATOR *)pUserData->memData;
	}

#if ENABLECHECK
	//	printf("pUserData->memflag: 0x%x, \n", pUserData->memflag);
	//	printf("pUserData->libOperator: 0x%x, \n", (VO_U32)pUserData->libOperator);

#if defined (_MAC_OS)
#else
	if((pUserData->memflag&0xf0) && pUserData->libOperator)
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_WMA, pUserData->memflag|1, g_hvommPlayInst, pUserData->libOperator);
	else
		nRet = voCheckLibInit (&pTemp, VO_INDEX_DEC_WMA, pUserData->memflag|1, g_hvommPlayInst, NULL);
	if (nRet != VO_ERR_NONE)
	{
		*phCodec = NULL;
		if(pTemp)
		{
			voCheckLibUninit(pTemp);
			pTemp = NULL;
		}
		return nRet;
	}	
#endif

#endif
		
	paudec = (CAudioObjectDecoder *)voauMalloc(vopMemOP, sizeof (CAudioObjectDecoder));
	if(paudec == NULL)
	{
		return VO_ERR_OUTOF_MEMORY;
	}
	// Wipe clean the object just allocated
	hr  = vopMemOP->Set(VO_INDEX_DEC_WMA, paudec, 0, sizeof (CAudioObjectDecoder));

	paudec->vopMemOP = vopMemOP;

//	{
//		int a = 0x235648, b = 0x48796, N = 0x4;
//		hr = MULT_HI_DWORDN_ARM(a, b, N);
//	}
	// Set all non-zero initializations
#if !NOTUSETEST
	paudec->pWMAForm = (WMAFormat *)auMalloc(sizeof(WMAFormat));
	if(paudec->pWMAForm == NULL)
	{
		hr = TraceResult(WMA_E_OUTOFMEMORY);
        // Cant return wmaResult
        return VO_ERR_OUTOF_MEMORY;
	}
	memset(paudec->pWMAForm, 0, sizeof(WMAFormat));

    hr = prvWipeCleanDecoder(paudec);
#endif	//!NOTUSETEST
#if ENABLECHECK
//	printf("pUserData->memflag: 0x%x, \n", pUserData->memflag);
//	printf("pUserData->libOperator: 0x%x, \n", (VO_U32)pUserData->libOperator);

#if defined (_MAC_OS)
#else
	paudec->hCheck = pTemp;		
#endif
	
#endif
#if !NOTUSETEST
	if (WMA_FAILED(hr)) {
		if(paudec != NULL)
			voauFree(vopMemOP, paudec);
        RETURN_ERROR_MAPS(hr);
    }

	// WMA Voice related
    paudec->m_cbWMAV_Version = 0;  // disable WMA Voice	
#endif	//!NOTUSETEST
	*phCodec = (VO_HANDLE)paudec;
//	VOLOGE("initial succeed!\n");
	DEBUG_PRINTF(printf("initial succeed!\n"));
	return VO_ERR_NONE;
}

#if TESTPERFORMANCE
int t1 = 0, total = 0;
int datalength = 0, samplerate = 0;
#endif
//int i=0;
VO_U32 VO_API voWMADecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput)
{
	Int BytePacketLength;

	CAudioObjectDecoder* paudec = (CAudioObjectDecoder*)hCodec;

	if(paudec == NULL)
		return VO_ERR_INVALID_ARG;
#if	!NOTUSETEST
	if(!paudec->Isheader)
		return VO_ERR_WRONG_STATUS;

	BytePacketLength = paudec->pau->m_cBytePacketLength;
//	printf("BytePacketLength: 0x%x\n", BytePacketLength);
//	printf("pInput->Length: 0x%x\n", pInput->Length);
	if(pInput->Length != (pInput->Length/BytePacketLength)*BytePacketLength)
	{
		DEBUG_PRINTF(printf("pInput->Length is not BytePacketLength!\n"));
//		return VO_ERR_WMA_INSIZENOTnBLOCKALIGN;
	}

//	printf("voWMADecSetInputData is ok!\n");
	paudec->pbIn = pInput->Buffer;
#endif	//!NOTUSETEST
	paudec->cbIn = paudec->bInLength = pInput->Length;

#if OUTINPUTRAWDATA
	if(paudec->cbIn != 0)
		write_vop_output(paudec->pbIn, paudec->cbIn);
#endif
	return VO_ERR_NONE;
}
#if NOTUSETEST
static int flag = 1;
#endif
VO_U32 VO_API voWMADecGetOutData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutBuffer, VO_AUDIO_OUTPUTINFO * pOutInfo)//HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECDATABUFFER *pOutData, VOCODECAUDIOFORMAT *pOutFormat)
{
	WMA_Bool fNoMoreInput = 0;
	WMARESULT hr;
	 Int BytePacketLength;
    WMA_U8* pbIn = NULL;
    Int cbIn = 0;

	CAudioObjectDecoder* paudec = (CAudioObjectDecoder*)hCodec;

	if (paudec == NULL || paudec->pau == NULL || pOutBuffer == NULL || pOutBuffer->Buffer == NULL)
	{
		return VO_ERR_INVALID_ARG;
	}
//	i++;
//	if(i ==0x10)
//		i = 0x10;
#if	!NOTUSETEST
	BytePacketLength = paudec->pau->m_cBytePacketLength;
	pbIn = paudec->pbIn;
	cbIn = paudec->cbIn;
#if TESTPERFORMANCE
	t1 = GetTickCount();
#endif
#if TESTINPUT
	if(cbIn != 0)
		write_wav_output(pbIn, cbIn);
#endif

	do
    {
        switch (paudec->decState)
        {
        case WMAProRawDecStateInput:
            {         
                       
                fNoMoreInput = 0;  
#if 0
				if(cbIn == 0)
				{
					if(paudec->f_DataRemnent <= 0){
						return VORC_COM_INPUT_BUFFER_SMALL;
					}
					else if(paudec->f_DataRemnent > BytePacketLength){
						pbIn = paudec->pBufferRement;
						cbIn = BytePacketLength;
						paudec->pBufferRement += BytePacketLength;
						paudec->f_DataRemnent -= BytePacketLength;
					}
					else if(paudec->f_DataRemnent == BytePacketLength){
						pbIn = paudec->pBufferRement;
						cbIn = BytePacketLength;
						paudec->pBufferRement = NULL;
						paudec->f_DataRemnent = 0;
					}
					else{
						voWMAMemcpy(paudec->m_pPacketBuffer, paudec->pBufferRement, paudec->f_DataRemnent);
						paudec->m_fBlockSize = paudec->f_DataRemnent;
						paudec->f_DataRemnent = 0;
						paudec->pBufferRement = NULL;
						return VORC_COM_INPUT_BUFFER_SMALL;
					}
				}
				else if(cbIn >= BytePacketLength)
				{
					if(pInData->buffer){
						paudec->pBufferRement = pInData->buffer;
						paudec->f_DataRemnent = pInData->length;
					}
					voWMAMemcpy(paudec->m_pPacketBuffer+paudec->m_fBlockSize, paudec->pBufferRement, BytePacketLength-paudec->m_fBlockSize);
					cbIn = BytePacketLength;
					paudec->f_DataRemnent -= (BytePacketLength-paudec->m_fBlockSize);
					pbIn = paudec->m_pPacketBuffer;
					paudec->pBufferRement += (BytePacketLength-paudec->m_fBlockSize);
					paudec->m_fBlockSize = 0;
				}
				else //if(cbIn < BytePacketLength)
				{
					if(cbIn + paudec->m_fBlockSize > BytePacketLength)
					{
						if(pInData->buffer)
						{
							paudec->pBufferRement = pInData->buffer;
							paudec->f_DataRemnent = pInData->length;
						}
						voWMAMemcpy(paudec->m_pPacketBuffer+paudec->m_fBlockSize, paudec->pBufferRement, BytePacketLength-paudec->m_fBlockSize);
						cbIn = BytePacketLength;
						paudec->f_DataRemnent -= (BytePacketLength-paudec->m_fBlockSize);
						pbIn = paudec->m_pPacketBuffer;
						paudec->pBufferRement += (BytePacketLength-paudec->m_fBlockSize);
						paudec->m_fBlockSize = 0;
					}
					else if(cbIn + paudec->m_fBlockSize < BytePacketLength)
					{
						voWMAMemcpy(paudec->m_pPacketBuffer+paudec->m_fBlockSize, pInData->buffer, cbIn);
						paudec->m_fBlockSize += cbIn;
						return VORC_COM_INPUT_BUFFER_SMALL;
					}
					else {
						voWMAMemcpy(paudec->m_pPacketBuffer+paudec->m_fBlockSize, pInData->buffer, cbIn);
						pbIn = paudec->m_pPacketBuffer;
						cbIn = BytePacketLength;
						paudec->m_fBlockSize = 0;
					}
				}			

#endif
				if(cbIn == 0)
				{
					if(paudec->f_DataRemnent <= 0)
					{
#if TESTPERFORMANCE
						total += GetTickCount()-t1;
#endif
						pOutInfo->InputUsed = paudec->bInLength;
						DEBUG_PRINTF(printf("input buffer small!\n"));
						return VO_ERR_INPUT_BUFFER_SMALL;
					}
					else
					{
						cbIn = paudec->f_DataRemnent;
						pbIn = NULL;//paudec->pBufferRement;
						if(cbIn <= BytePacketLength)
						{
							pbIn = paudec->pBufferRement;
							paudec->f_DataRemnent -= BytePacketLength;
							paudec->pBufferRement = NULL;
						}
					}
				}
                
				if(cbIn > BytePacketLength)
				{
					if(pbIn)
					{
						paudec->pBufferRement = pbIn;
						paudec->f_DataRemnent = cbIn;
					}
					cbIn = BytePacketLength;
					paudec->f_DataRemnent -= BytePacketLength;
					pbIn = paudec->pBufferRement;
					paudec->pBufferRement += BytePacketLength;
				}

                hr = WMAProRawDecInput(hCodec, pbIn, cbIn, 1, fNoMoreInput,
                    0, 0, &paudec->decState, NULL);
	           	
				if(WMA_FAILED(hr)) {
#if 0
                    goto gCleanup;
#else
					DEBUG_PRINTF(printf("WMAProRawDecInput error!\n"));
					return hr;
#endif
                }
//				printf("WMAProRawDecInput succeeded!\n");
                break;
            }            

        case WMAProRawDecStateDecode:
            {

                hr = WMAProRawDecDecode(hCodec, &paudec->cSampleReady, &paudec->decState, NULL);
				cbIn = paudec->cbIn;
                if(WMA_FAILED(hr)) {
//					decState = WMAProRawDecStateInput;
#if 0
					goto gCleanup;
#else
					DEBUG_PRINTF(printf("WMAProRawDecDecode error!\n"));
					return hr;
#endif
                }
                break;
            }
            
        case WMAProRawDecStateGetPCM:
			{
				WMA_U32 cSampleProduced;
				WMA_U32 cbDstUsed;
//				printf("WMAProRawDecGetPCM() begin!\n");
				hr = WMAProRawDecGetPCM (hCodec, paudec->cSampleReady, &cSampleProduced, pOutBuffer->Buffer, paudec->cbDst, 
					&cbDstUsed, NULL, &paudec->decState, &paudec->PlayerInfo, NULL);
				
				if(WMA_FAILED(hr)) {
#if 0
					goto gCleanup;
#else
					DEBUG_PRINTF(printf("WMAProRawDecGetPCM error!\n"));
					return hr;
#endif
				}
				else
				{
//					printf("WMAProRawDecGetPCM() success!\n");
					paudec->cSampleReady -= (WMA_U16)cSampleProduced;
					pOutBuffer->Length = cbDstUsed;
#if DLLTEST
					write_wav_output(pOutBuffer->Buffer, pOutBuffer->Length);
#endif
#if TESTPERFORMANCE
					datalength += pOutBuffer->Length;
#endif
					goto gEnd;
				}

				break;
			}
        
        case WMAProRawDecStateDone:
            goto gEnd;
        }
    } while (1);
    

gEnd:
	pOutInfo->Format.Channels = paudec->m_cDstChannel;//paudec->pWMAForm->nChannels;
	pOutInfo->Format.SampleBits = paudec->m_nDstValidBitsPerSample;//paudec->pWMAForm->nValidBitsPerSample;
	pOutInfo->Format.SampleRate = paudec->m_iDstSamplingRate;//paudec->pWMAForm->nSamplesPerSec;
	pOutInfo->InputUsed = 0;
//	voCheckLibCheckAudio(paudec->hCheck, pOutBuffer, pOutInfo);
#if ENABLECHECK
	
#if defined (_MAC_OS)
#else
	if(voCheckLibCheckAudio(paudec->hCheck, pOutBuffer, &(pOutInfo->Format)) != VO_ERR_NONE)
		return VO_ERR_LICENSE_ERROR;
#endif
	
#endif
#if SECONDS180LIMIT
	outlength += pOutBuffer->Length;
	if(outlength > LimitLength)
		voWMAMemset(pOutBuffer->Buffer, 0, pOutBuffer->Length);
#endif
#if TESTPERFORMANCE
	total += GetTickCount()-t1;	
#endif
#else	//!NOTUSETEST
	if(flag==1)
	{
		pOutBuffer->Length = 0x4000; 
		voWMAMemset(pOutBuffer->Buffer, 0, pOutBuffer->Length);
		flag = 0;
		pOutInfo->Format.Channels = 2;//paudec->pWMAForm->nChannels;
		pOutInfo->Format.SampleBits = 16;//paudec->pWMAForm->nValidBitsPerSample;
		pOutInfo->Format.SampleRate = 44100;
#if ENABLECHECK
		
#if defined (_MAC_OS)
#else
		if(voCheckLibCheckAudio(paudec->hCheck, pOutBuffer, &(pOutInfo->Format)) != VO_ERR_NONE)
			return VO_ERR_LICENSE_ERROR;
#endif
		
#endif	
	}
	else
	{
		pOutInfo->InputUsed = paudec->bInLength;
		flag = 1;
		return	VO_ERR_INPUT_BUFFER_SMALL;
	}
#endif	//!NOTUSETEST
	return VO_ERR_NONE;	

#if 0
gCleanup:
//	pInData->buffer = paudec->pfirst;
#if TESTPERFORMANCE
	total += GetTickCount()-t1;
#endif
	pOutInfo->InputUsed = paudec->bInLength;
	paudec->decState = WMAProRawDecStateInput;
    return VO_ERR_INPUT_BUFFER_SMALL;
#endif

}

//#define TESTMEMORYLEAK
#ifdef TESTMEMORYLEAK
extern int MallocNumber;
#endif
VO_U32 VO_API voWMADecUninit(VO_HANDLE hCodec)//HVOCODEC hCodec)
{
#if DLLTEST | TESTINPUT | TESTCOEFDATA
	close_wav_output();
#endif
#if OUTINPUTRAWDATA
	close_vop_output();
#endif
#if TESTPERFORMANCE
	{
		TCHAR msg[64];
		double performance = 0.6* total *samplerate * 4 /datalength;
		wsprintf(msg, TEXT("Performance: %f Mhz/s"), performance);
		MessageBox(NULL, msg, TEXT("WMA Decode Finished"), MB_OK);
	}
#endif
#ifdef DEBUG_LINUX
	LOGW("hCodec: 0x%x", (VO_U32)hCodec);
#endif

	if(hCodec)
	{
		
#if ENABLECHECK
		
#if defined (_MAC_OS)
#else
		CAudioObjectDecoder *paudec = (CAudioObjectDecoder *)hCodec;
		if(paudec->hCheck != NULL)
			voCheckLibUninit(paudec->hCheck);
#endif
		
#endif
#if !NOTUSETEST
		if(WMAProRawDecDelete(hCodec) != WMA_OK)
			return VO_ERR_FAILED;
#else
	{
		CAudioObjectDecoder *paudec = (CAudioObjectDecoder *)hCodec;
		VO_MEM_OPERATOR *vopMemOP=paudec->vopMemOP;
		voauFree(vopMemOP, paudec);
	}
#endif
	}
#ifdef TESTMEMORYLEAK
#ifdef DEBUG_LINUX
	LOGW("mem leak number: %d", MallocNumber);
#endif
#endif
 	return VO_ERR_NONE;
}

VO_U32 VO_API voWMADecSetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)//HVOCODEC hCodec, LONG nID, LONG lValue)
{
    WMARESULT hr = WMA_OK;
	PCMFormat PCMForm;
	VO_WAVEFORMATEX *WMAProp;
	
	WMA_U32 iVersion;	
	WMAFormat *pWMAForm;
	VO_CODECBUFFER *WMABuffer;
	VO_U8 *pExtData;
	CAudioObjectDecoder *paudec = (CAudioObjectDecoder *)hCodec;
	
#if	!NOTUSETEST
	switch(uParamID)
	{
	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *wma_format = (VO_AUDIO_FORMAT *)pData;
			paudec->pWMAForm->nChannels	= (WMA_U16)wma_format->Channels;
			paudec->pWMAForm->nValidBitsPerSample	= (WMA_U16)wma_format->SampleBits;
			paudec->pWMAForm->nSamplesPerSec	= wma_format->SampleRate;
		}
		break;

	case VO_PID_COMMON_HEADDATA:
		pWMAForm	= paudec->pWMAForm;
		WMABuffer	= (VO_CODECBUFFER *)pData;
		if(WMABuffer->Buffer == NULL || WMABuffer->Length == 0)
			return VO_ERR_INVALID_ARG;

		WMAProp = (VO_WAVEFORMATEX *)WMABuffer->Buffer;//pData;
		pWMAForm->wFormatTag = WMAProp->wFormatTag;
		if(WMAProp->nChannels != 0)
			pWMAForm->nChannels = WMAProp->nChannels;
		if(WMAProp->nSamplesPerSec != 0)
			pWMAForm->nSamplesPerSec = WMAProp->nSamplesPerSec;
#if TESTPERFORMANCE
		samplerate = pWMAForm->nSamplesPerSec;
#endif
#if DLLTEST | TESTINPUT | TESTCOEFDATA
		if(first)
		{
			first = 0;
			pcmformat.channelcount = min(2, WMAProp->nChannels);
			pcmformat.samplecount = min(16, WMAProp->wBitsPerSample);
			pcmformat.samplerate = WMAProp->nSamplesPerSec;
			if (open_wav_output() == -1){
				return VO_ERR_INPUT_BUFFER_SMALL;
			}
		}
#endif
//		{	//test voWMAMalloc
//			char *pbdst=NULL;
//			char *pbSrc = (char *)malloc(100);
//			pbdst = memset(pbSrc+1, 0x12345, 25);
//			free(pbSrc);
//		}
		pWMAForm->nAvgBytesPerSec = WMAProp->nAvgBytesPerSec;
		pWMAForm->nBlockAlign = WMAProp->nBlockAlign;

		/* First, use sequence header, if it is 0, use set audio format value, 
		if it is also 0, default 16 bits */
		if(WMAProp->wBitsPerSample != 0)
			pWMAForm->nValidBitsPerSample = WMAProp->wBitsPerSample;
		if(pWMAForm->nValidBitsPerSample == 0)
			pWMAForm->nValidBitsPerSample = 16;
//		if(WMAProp->cbSize != 12)
//		{
//			return VO_ERR_INVALID_ARG;
//		}
#if OUTINPUTRAWDATA
		if(first)
		{
			first = 0;
			if(write_vop_output((void *)&WMABuffer->Length, 4)<0)
				return VO_ERR_OUTPUT_BUFFER_SMALL;
			if(write_vop_output((void *)WMABuffer->Buffer, WMABuffer->Length)<0)
				return VO_ERR_OUTPUT_BUFFER_SMALL;
		}
#endif
		pExtData = (VO_U8 *)WMAProp + 18;
		if (WMAProp->wFormatTag == WAVE_FORMAT_MSAUDIO3 || WMAProp->wFormatTag == WAVE_FORMAT_WMAUDIO_LOSSLESS)
		{
			pWMAForm->nChannelMask = (VO_U32)(*((VO_U16 *)pExtData + 1)) + (VO_U32)((*((VO_U16 *)pExtData + 2))<<16);
			pWMAForm->wEncodeOpt = *((VO_U16 *)pExtData + 7);
			pWMAForm->wAdvancedEncodeOpt = *((VO_U16 *)pExtData + 8);
			pWMAForm->dwAdvancedEncodeOpt2 = (VO_U32)(*((VO_U16 *)pExtData + 5)) + (VO_U32)((*((VO_U16 *)pExtData + 6))<<16);
//			pWMAForm->nChannelMask = (VO_U32)((VO_U16) *(pExtData + 2) + (((VO_U16) *(pExtData + 4))<<16));
//			pWMAForm->wEncodeOpt = (VO_U16) *(pExtData + 14);
//			pWMAForm->wAdvancedEncodeOpt = (VO_U16) *(pExtData + 16);
//			pWMAForm->dwAdvancedEncodeOpt2 = (VO_U16) *(pExtData + 10);
		}
		else
		{
			if(WMAProp->wFormatTag == WAVE_FORMAT_MSAUDIO2)
				pWMAForm->wEncodeOpt = *((VO_U16 *)pExtData + 2); 
			else if(WMAProp->wFormatTag == WAVE_FORMAT_MSAUDIO1)
				pWMAForm->wEncodeOpt = *((VO_U16 *)pExtData + 1);
			pWMAForm->nChannelMask = WMAProDefaultChannelMask(pWMAForm->nChannels);
		}
		hr  = paudec->vopMemOP->Set(VO_INDEX_DEC_WMA, &paudec->PlayerInfo, 0, sizeof(WMAPlayerInfo));
		WMAProWMAFormat2PCMFormat((void *)paudec, pWMAForm, &PCMForm);
		hr = WMAProRawDecInit(paudec, pWMAForm, &PCMForm, &paudec->PlayerInfo, &paudec->decState, NULL);
//		audecReset(paudec);
		if(WMA_FAILED(hr)) {
			RETURN_ERROR_MAPS(hr);//VORC_COM_INVALID_ARG;
		}
//		printf("paudec->m_rgrgfltChDnMixMtx[0]:");
//		for(cRestSize = 0; cRestSize < 8; cRestSize++)
//		{
//			printf(" 0x%x,", paudec->m_rgrgfltChDnMixMtx[0][cRestSize]);
//		}
//		printf("\n");
//		printf("paudec->m_rgrgfltChDnMixMtx[1]:");
//		for(cRestSize = 0; cRestSize < 8; cRestSize++)
//		{
//			printf(" 0x%x,", paudec->m_rgrgfltChDnMixMtx[1][cRestSize]);
//		}
//		printf("\n");
//		printf("CAudioObjectDecoder_pau                  EQU	%d\n", offsetof(CAudioObjectDecoder, pau));
//		printf("CAudioObjectDecoder_m_fChannelFoldDown   EQU	%d\n", offsetof(CAudioObjectDecoder, m_fChannelFoldDown));
//		printf("CAudioObjectDecoder_m_fReQuantizeTo16    EQU	%d\n", offsetof(CAudioObjectDecoder, m_fReQuantizeTo16));
//		printf("CAudioObjectDecoder_m_cDstChannel        EQU	%d\n", offsetof(CAudioObjectDecoder, m_cDstChannel));	
//		printf("CAudioObjectDecoder_m_rgrgfltChDnMixMtx  EQU	%d\n", offsetof(CAudioObjectDecoder, m_rgrgfltChDnMixMtx));
		switch (pWMAForm->wFormatTag) {
		case 0x160: iVersion = 1; break;
		case 0x161: iVersion = 2; break;
		case 0x162: iVersion = 3; break;
		case 0x163: iVersion = 3; break;
		case 0x165: iVersion = 2; break;
		case 0x166: iVersion = 3; break;
		case 0x167: iVersion = 3; break;
		default: 
			return VO_ERR_NOT_IMPLEMENT;
		}
		
#define ICEILDIVN(x,N) (((x)+(N)-1)/(N))		
		paudec->cSubband = WMAProGetSamplePerFrame(pWMAForm->nSamplesPerSec, pWMAForm->nAvgBytesPerSec * 8,
			iVersion, pWMAForm->wEncodeOpt);
		paudec->cbDst = paudec->cSubband *
            max(pWMAForm->nChannels, PCMForm.nChannels) *
            max(ICEILDIVN(pWMAForm->nValidBitsPerSample, 8), (signed)PCMForm.cbPCMContainerSize);		
		
		DEBUG_PRINTF(printf("set parameter succeed!\n"));
		paudec->Isheader = 1;	
#if	SECONDS180LIMIT
		LimitLength = LIMITTIME * paudec->m_cDstChannel * paudec->m_nDstValidBitsPerSample / 8 *paudec->m_iDstSamplingRate;
#endif
		break;
	case VO_PID_COMMON_FLUSH:
		hr = audecReset(paudec);
		paudec->f_DataRemnent = 0;
		paudec->cbIn = 0;
		paudec->cSampleReady = 0;
		paudec->m_externalState = audecStateInput;
		paudec->decState = WMAProRawDecStateInput;
		break;
	case VO_PID_WMA_SUPPTHISMPLRT:
		paudec->SupptHiSmplrt = 1;
		break;
	case VO_PID_WMA_SUPPORT24BIT:
		paudec->Support24Bit = 1;
		break;
	case VO_PID_WMA_SUPPTMTCHANL:
		paudec->SupptMtChanl = 1;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
		
	}
#endif	//!NOTUSETEST
	return VO_ERR_NONE;
}

VO_U32 VO_API voWMADecGetParameter(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData)//HVOCODEC hCodec, LONG nID, LONG *plValue)
{
#if	!NOTUSETEST
	CAudioObjectDecoder* paudec = (CAudioObjectDecoder*)hCodec;	
	switch(uParamID)
	{
	case VO_PID_WMA_OUTBUFFERSIZE:
		{
			VO_S32 *plValue = (VO_S32 *)pData;
			*plValue = paudec->cbDst;
		}
		break;

	case VO_PID_WMA_FRAMELENGTH:
		{
			VO_S32 *plValue = (VO_S32 *)pData;
			*plValue	= paudec->cSubband;
		}
		break;

	case VO_PID_AUDIO_FORMAT:
		{
			VO_AUDIO_FORMAT *wma_format = (VO_AUDIO_FORMAT *)pData;
			wma_format->Channels	= paudec->m_cDstChannel;
			wma_format->SampleBits	= paudec->m_nDstValidBitsPerSample;
			wma_format->SampleRate	= paudec->m_iDstSamplingRate;
		}
		break;

	case VO_PID_AUDIO_SAMPLEREATE:
		{
			Int *pSamplerate = (Int *)pData;
			*pSamplerate  = paudec->m_iDstSamplingRate;
		}
		break;

	case VO_PID_AUDIO_CHANNELS:
		{
			Int *pChannel = (Int *)pData;
			*pChannel  = paudec->m_cDstChannel;
		}
		break;

	case VO_PID_AUDIO_BITRATE:
		{
			WMA_U32 *pBitrate = (WMA_U32 *)pData;
			*pBitrate  = paudec->pWMAForm->nAvgBytesPerSec;
		}
		break;

	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
#endif	//!NOTUSETEST
	return VO_ERR_NONE;
}

VO_S32 VO_API voGetWMADecAPI (VO_AUDIO_CODECAPI * pDecHandle)
{
	if(pDecHandle == NULL)
		return	VO_ERR_INVALID_ARG;

	pDecHandle->Init			= voWMADecInit;
	pDecHandle->SetParam		= voWMADecSetParameter;
	pDecHandle->GetParam		= voWMADecGetParameter;
	pDecHandle->SetInputData	= voWMADecSetInputData;
	pDecHandle->GetOutputData	= voWMADecGetOutData;
	pDecHandle->Uninit			= voWMADecUninit;

	return VO_ERR_NONE;
}





