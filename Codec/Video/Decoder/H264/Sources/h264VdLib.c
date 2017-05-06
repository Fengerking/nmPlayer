
#include		"global.h"
#include		"vlc.h"
#include		"mbuffer.h"
#include		"nalucommon.h"

#if !BUILD_WITHOUT_C_LIB
#include		<stdlib.h>
#include		<string.h>
#include		<assert.h>
#endif//BUILD_WITHOUT_C_LIB
#include		"voH264.h"

#if ARM_FILE_IO
#include		"dma.h"
#endif
//#include "H264_D_API.h"
#include "h264dec.h"  //new API for HTC
// added by gtxia 2007-10-23
#if defined(_WIN32) || defined(_WIN32_WCE) || defined(WIN32)
#   include "vochklcslib.h"
#endif
void* ARM11_MX31_PhyMalloc(void* handle,int Size,int *PhyAddress);
void ARM11_MX31_PhyFree(void* handle,void* virtAddress);
static avdBoolean FindNaluTypeSliceOrIDR(ImageParameters *img);

#define INIT_READ_SIZE 5120
int imgType[3] = {P_FRAME,B_FRAME,I_FRAME}; 
int		GetCurrentImgType(ImageParameters *img)
{
	return imgType[img->type];
}

static int  ProbeH264FrameType(unsigned char * buffer)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len,value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;
	int frameType=0;
	if(naluType==5)
		return I_FRAME;
	if(naluType==1)//need continuous check
		buffer++;
	else//the nalu type is params info
		return NULL_FRAME;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit

		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}

			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return NULL_FRAME;
	}
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf>=5)
		inf-=5;
	if(inf<0||inf>2)
		return NULL_FRAME;
	else
		return imgType[inf];   
}

VoiH264VdReturnCode CALL_API H264VdLibStartDecode(ImageParameters *img,H264VdLibParam *par)
{
	VoiH264VdReturnCode	returnCode;
	StartRunTimeClock(DT_TOTALTIME);
#ifdef THREAD_DEBLOCK
//	CreateDeblockThread();
#endif
	if (!par)
		return VOIH264VD_NULL_INPUT_PARAMETERS_POINTER;

	returnCode = initDec(img);CHECK_ERR_RET_INT
	if (!H264VDLIB_SUUCCEEDED(returnCode))
		return returnCode;
	
	

	//img->FrameCount = 0;

	//if(par->fileFormat!=VOIH264FM_14496_15&&par->fileFormat!=VOH264_AVC)
	//	par->fileFormat = VOIH264FM_ANNEXB;
	// for "img" variable;
	//img->vdLibPar = par;
	//img->lbPicReady = img->lb2ndFldReady = VOI_TRUE;
	//img->inDataPackSize = 48; // default, will be updated after height and width known;
	// use num_ref_frames
	return VOIH264VD_SUCCEEDED;
} 

VoiH264VdReturnCode CALL_API H264VdLibEndDecode(ImageParameters *img)
{
	VoiH264VdReturnCode	returnCode;
	flush_dpb(img);

	returnCode = deInitDec(img);
	if (!H264VDLIB_SUUCCEEDED(returnCode))
		return returnCode;

	return VOIH264VD_SUCCEEDED;
}

VoiH264VdReturnCode CALL_API H264Flushing(ImageParameters *img)
{
	if(img)
	{	
		
		TPOCInfo* pocInfo = img->pocInfo;
		TIOInfo	*ioInfo = img->ioInfo;
		flush_dpb(img);CHECK_ERR_RET_INT
		
		ioInfo->IsSeekingNextIFrame = 1;
		pocInfo->currPoc = 0;
		pocInfo->PrevPicOrderCntMsb = 0;
		pocInfo->PrevPicOrderCntLsb = 0;	
		//img->lbPicReady = 1;
		img->current_slice_nr=0;
		img->dec_picture = NULL;
	
#ifndef VOI_H264D_BLOCK_FMO
		img->prevMbToSliceGroupMapIdx = 0;
		img->MbToSliceGroupMap = NULL;
		img->MapUnitToSliceGroupMap = NULL;
#endif //VOI_H264D_BLOCK_FMO
		return VOIH264VD_SUCCEEDED;
	}
	
	return -1;
}

static __inline int CleanTheBitStream(H264VdLibParam *inPa)
{
	avdUInt8*	head = inPa->inBuf;
	avdUInt8*       end  = head + inPa->inBufDataLen;
	if(inPa->inBufDataLen>600*1024)
	{
		AvdLog2(LL_INFO,"error:inPa->inBufDataLen>600*1024\n");
		return -1;
	}
	do {

		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx

		if(head[2]!=3)
		{
			head+=3;
		}
		else
		{
			memmove(head+2,head+3,end-(head+3));
			inPa->inBufDataLen--;
			end--;
			head+=2;
		}

		   
	}
	while(head+3<end);
	return 0;
}
int CALL_API H264DecodeHeadDataAnnexB(ImageParameters *img,H264VdLibParam *inPa)
{
	avdUInt8*	head = inPa->inBuf;
	avdUInt8*       end  = head + inPa->inBufDataLen;
	CleanTheBitStream(inPa);//check the 00 00 03

	do {
		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx
		switch(head[2]){
		case 0:
			 //find 000000xx
			if (!head[3]){ // 00000000xx; just skip 1 avdUInt8;
				head++;
				continue; 
			}
			if (head[3] == 1)
			{
				inPa->inBuf = head+4;
				if((inPa->inBuf[0]&0x07)==0x07||(inPa->inBuf[0]&0x08)==0x08)
				{
					AvdLog2(LL_INFO,"Set HeadData2,id=%d\n",inPa->inBuf[0]);
					ProcessOneNALU(img,4);
				}
			}

			head += 4; //skip four bytes and continue search;
			break;
		case 1:
			// find 000001 header;
			inPa->inBuf = head+3;
			if((inPa->inBuf[0]&0x07)==0x07||(inPa->inBuf[0]&0x08)==0x08)
			{
				AvdLog2(LL_INFO,"Set HeadData2,id=%d\n",inPa->inBuf[0]);
				ProcessOneNALU(img,3);
			}

			head +=3; //skip three bytes and continue search;
			break;
	
		default: // other than 0, 1, 3;
			head += 3; // skip three bytes;
			break;
		} // switch;
		if (img->error)
		{
			return img->error;
		}
	} while (head+3 < end);
	return 0;

}
static int  voH264DecProcessNALU(ImageParameters *img,H264VdLibParam *inPa)
{
	int ret=0;
	if(CleanTheBitStream(inPa)==-1)//check the 00 00 03//RIM_
	{
		AVD_ERROR_CHECK(img,"CleanTheBitStream(inPa)==-1,return",100);
		return VOIH264VD_H264_DATA_ERROR;
	}

	ret=ProcessOneNALU(img,4);
	CHECK_ERR_RET_INT;
	return ret;
}




int  voH264DecProcessAnnexB(ImageParameters *img,H264VdLibParam *inPa)
{
	avdUInt8*		  head = inPa->inBuf;
	avdUInt8*       end  = head + inPa->inBufDataLen;
	avdUInt8*		  currNALU  = NULL;
	avdUInt8*		  nextNALU  =  NULL;
	int					  processedOneNALU = 0;
	int prefixSize;
	do {
		if (head[0]) {// faster if most of bytes are not zero;	
			head++;
			continue;
		}
		// find 00xx
		if (head[1]){
			head += 2; // skip two bytes;
			continue;
		}
		// find 0000xx
		switch(head[2]){
		case 0:
			//find 000000xx
			if (!head[3]){ // 00000000xx; just skip 1 avdUInt8;
				head++;
				continue; 
			}
			if (head[3] == 1)
			{
				prefixSize  = 4;
				if(currNALU==NULL)
				{
					head +=prefixSize;
					currNALU=head;
				}
				else if(nextNALU==NULL)
				{
					head +=prefixSize;
					nextNALU=head;
				}
				
				if(nextNALU&&currNALU)
				{
					inPa->inBuf = currNALU;
					inPa->inBufDataLen	= nextNALU - currNALU - prefixSize;
					ProcessOneNALU(img,prefixSize);
					head = currNALU = nextNALU;
					nextNALU = NULL;
				}

			}
			else
				head+=4;

			break;
		case 1:
			{
				prefixSize  = 3;
				if(currNALU==NULL)
				{
					head +=prefixSize;
					currNALU=head;
				}
				else if(nextNALU==NULL)
				{
					head +=prefixSize;
					nextNALU=head;
				}
				
				if(nextNALU&&currNALU)
				{
					inPa->inBuf = currNALU;
					inPa->inBufDataLen	= nextNALU - currNALU - prefixSize;
					ProcessOneNALU(img,prefixSize);
					head = currNALU = nextNALU;
					nextNALU = NULL;
				}
			}
		
			break;
		case 3:
			memmove(head+2,head+3,end-(head+3));//clean bitstream
			end--;
			//if (head[2]>1)//00 00 03 01,rare case
			{
				head+=2;
			}
			break;
		default: // other than 0, 1, 3;
			head += 3; // skip three bytes;
			break;
		} // switch;
	} while (head+3 < end);

	if(currNALU)//last nalu
	{
		inPa->inBuf = currNALU;
		inPa->inBufDataLen = end - currNALU;

		ProcessOneNALU(img,prefixSize);
	}
	return img->error;
}


static int ThreadNum=0;
/**
* Initialize a H264 decoder instance use default settings.
* \param phCodec [out] Return the H264 Decoder handle.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/
//VO_CODEC_INIT_USERDATA * pUserData2=NULL;
#if defined(IMX31_ARM11)
#define FORCE_FREESCALE_IPU 1
#if FORCE_FREESCALE_IPU
#define FORCE_FREESCALE_IPU_ROW 8
#else//FORCE_FREESCALE_IPU
#define FORCE_FREESCALE_IPU_ROW 0
#endif//FORCE_FREESCALE_IPU
#endif//(IMX31_ARM11)
VOH264DECRETURNCODE VOCODECAPI voH264DecInitNew(HVOCODEC *phCodec,VO_CODEC_INIT_USERDATA * pUserData2)
{
	GlobalH264Data* gData ;//= voH264AlignedMalloc(img,1001,sizeof(GlobalH264Data));
	H264VdLibParam* params;
	ImageParameters *img;

#ifdef _WIN32_WCE1
	int ret = CheckLicenseFile(NULL);
	if(ret<0)
		return VORC_COM_LICENSE_LIMIT;
#endif//_WIN32_WCEs
	//printf("img size=%d",sizeof(ImageParameters));
	
	gData = *phCodec;
	gData->heapType = PREALLOCATE_BUF;
		
	
	*phCodec = gData;
	params = gData->params;
	gData->img = calloc(sizeof(ImageParameters),1);
	img=gData->img;
	img->vdLibPar = params;
	if(pUserData2&&pUserData2->memData)
	{
		VO_MEM_OPERATOR* oper=(VO_MEM_OPERATOR*)pUserData2->memData;
		if(params->customMem2==NULL)
			params->customMem2 = (VOH264MEMOP*)calloc(sizeof(VOH264MEMOP),1);
		params->customMem2->Alloc4 = (int  (VOAPI * ) (int uID, VOMEM_INFO * pMemInfo))oper->Alloc;
		params->customMem2->Free4 = (int (VOAPI *) (int uID, void* pBuff))oper->Free;
		params->customMem2->Set		= (int (VOAPI * ) (int uID, void* pBuff, unsigned char uValue, unsigned int uSize))oper->Set;
		params->customMem2->Copy	= (int (VOAPI * ) (int uID, void* pDest, void* pSource, unsigned int uSize))oper->Copy;
		params->customMem2->Check = (int (VOAPI * ) (int uID, void* pBuffer, unsigned int uSize))oper->Check;
	}
	//pUserData2=NULL;//for possible multi instances, reset it

	initDec(img);
	CHECK_ERR_RET_INT
	//params->fileFormat=VOIH264FM_14496_15;
	//img->threadID=++ThreadNum;


	return VORC_OK;
}

static int GetNALUSize(unsigned char* lenBuf,int size)
{
	int length=0;
	switch(size)
	{
	case 4:
		length = (lenBuf[0]<<24)|(lenBuf[1]<<16)|(lenBuf[2]<<8)|lenBuf[3];
		break;
	case 3:
		length = (lenBuf[0]<<16)|(lenBuf[1]<<8)|(lenBuf[2]);//<<8)|lenBuf[3])
		break;
	case 2:
		length = (lenBuf[0]<<8)|(lenBuf[1]);//<<8)|(lenBuf[2]);
	    break;
	case 1:
		length = lenBuf[0];
	    break;
	default:
	    break;
	}
	return length;
}
static VOH264DECRETURNCODE VOCODECAPI voH264DecProcessAVC(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECVIDEOBUFFER *pOutData, VOCODECVIDEOFORMAT *pOutFormat)
{
	GlobalH264Data* gData = (GlobalH264Data*)hCodec;
	H264VdLibParam* params;
	ImageParameters *img;
	VOCODECDATABUFFER inData;

//	int returnCode;
	unsigned char* tmpBuf;
	int imgType[3] = {P_FRAME,B_FRAME,I_FRAME}; 
	int m_lengthSizeOfNALU;
	int size1,TotalSize=pInData->length;

	if(hCodec==NULL)
		return VORC_COM_WRONG_STATUS;
	img=gData->img;
	m_lengthSizeOfNALU = img->ioInfo->lengthSizeOfNALU;
	img->vdLibPar = params = gData->params;
	
	
	tmpBuf = pInData->buffer;
	while(TotalSize>8)//there is another NALU!
	{
		
			size1 = GetNALUSize(tmpBuf,m_lengthSizeOfNALU);
			if(size1>TotalSize)
				return VORC_COM_WRONG_STATUS;

			tmpBuf+= m_lengthSizeOfNALU;
			inData.buffer = tmpBuf;//the first m_lengthSizeOfNALU byte is size
			inData.length = size1;
			TotalSize-=size1+m_lengthSizeOfNALU;
			tmpBuf+=size1;
			pOutData->data_buf[0] = NULL;
			
			//init the params
			params->inBuf = inData.buffer;
			params->inBufDataLen = inData.length;
			//decode
			voH264DecProcessNALU(img,params);
			if (img->error<0)
			{
				return img->error;
			}
			pOutFormat->width  = params->sizeInfo.frameWidth;
			pOutFormat->height = params->sizeInfo.frameHeight;
			pOutFormat->frame_type = imgType[img->type];
	
	}

	return VORC_OK;

}


/**
* Decode one frame data.
* \param hCodec [in] H264 decoder instance handle, returned by voH264DecInit().
* \param pInData [in/out] Input buffer pointer and length. 
*  When the function return, pInData->length indicates the actual size consumed by decoder.
* \param pOutData [in/out] Output buffer pointer and length. The buffer alloc and release by decoder
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_STATUS The decoder is not ready for decode. Usually this indicates more parameters needed. See voH264SetParameter().
*/
VOH264DECRETURNCODE VOCODECAPI voH264DecProcess(HVOCODEC hCodec, VOCODECDATABUFFER *pInData, VOCODECVIDEOBUFFER *pOutData, VOCODECVIDEOFORMAT *pOutFormat)
{
	GlobalH264Data* gData = (GlobalH264Data*)hCodec;
	H264VdLibParam* params;
	ImageParameters *img;
	
	int length=pInData->length;

	if(hCodec==NULL||length==0)
		return VORC_COM_WRONG_STATUS;
	img= gData->img;
	img->vdLibPar = params = gData->params;
	params->inBuf = pInData->buffer;
	params->inBufDataLen = length;
	switch (params->fileFormat)
	{
	case VOH264_AVC:
		voH264DecProcessAVC(hCodec,pInData,pOutData,pOutFormat);
		break;
	case VOH264_14496_15:
		voH264DecProcessNALU(img,params);
		break;
	default:
		voH264DecProcessAnnexB(img,params);
		break;
	}
	return img->error;
}

/**
* Close the H264 decoder instance, release any resource it used.
* \param hCodec [in] H264 decoder instance handle, returned by voH264DecInit().
* \return The function should always return VORC_OK.
*/
VOH264DECRETURNCODE VOCODECAPI voH264DecUninit(HVOCODEC hCodec)
{
	GlobalH264Data* gData = (GlobalH264Data*)hCodec;
	H264VdLibParam* params;
	ImageParameters *img;
	if(hCodec)
	{
		img= gData->img;
		if(img)
		{
			//img->vdLibPar->endOfSequence=VOIH264VD_TRUE;
			flush_dpb(img);
			deInitDec(img);	
		}
#if !USE_GLOBALE_MEM
		if(gData->heapType==0)
		{
			free(gData->params);gData->params = NULL;
			free(hCodec);
		}
#endif//USE_GLOBALE_MEM
		free(img);
	}
	return VORC_OK;
}
static char testAVCCfg[] = {0x01,0x42 ,0xc0 ,0x33 ,0xff ,0xe1 ,0x00 ,0x17 ,0x67 ,0x42 ,0xc0 ,0x33 ,0xab ,0x40 ,0x28 ,0x02
,0xdd ,0x08 ,0x00 ,0x00 ,0x03 ,0x03 ,0x20 ,0x00 ,0x00 ,0xbb ,0x54 ,0x78 ,0xc1 ,0x95 ,0x00 ,0x01
,0x00 ,0x05 ,0x68 ,0xce ,0x3c ,0x80 ,0x00};
static int ParseAVCConfig(HVOCODEC hCodec,char* lValue)
{
	//refer 14496-15 5.2.4.1.1
	int i;
	GlobalH264Data* gData = (GlobalH264Data*)hCodec;
	VOCODECDATABUFFER* params = (VOCODECDATABUFFER*) lValue;
	
	char* pData = (char*)params->buffer;
	int numOfPictureParameterSets;
	int configurationVersion	= pData[0];
	int AVCProfileIndication	= pData[1];
	int profile_compatibility	= pData[2];
	int AVCLevelIndication		= pData[3];
	int numOfSequenceParameterSets = pData[5]&0x1f;
	unsigned char * buffer = (unsigned char*)pData+6;
	VOCODECDATABUFFER inData;
	int returnCode;
	ImageParameters *img;
	H264VdLibParam *inPa;
	img= gData->img;
	img->ioInfo->lengthSizeOfNALU			= (pData[4]&0x03)+1;
	inPa = img->vdLibPar;
	//inPa->fileFormat=VOIH264FM_14496_15;
	for (i=0; i< numOfSequenceParameterSets; i++) {
		int sequenceParameterSetLength = (buffer[0]<<8)|buffer[1];
		buffer+=2;
		inData.buffer = buffer;
		inData.length = sequenceParameterSetLength;
		returnCode = voH264DecSetParameter(hCodec, VO_ID_H264_AVCCONFIG, (long)(&inData));
		if(VORC_FAILED(returnCode))
			return returnCode;
		buffer+=sequenceParameterSetLength;
	}
	numOfPictureParameterSets=*buffer++;
	for (i=0; i< numOfPictureParameterSets; i++)
	{
		int pictureParameterSetLength = (buffer[0]<<8)|buffer[1];
		buffer+=2;
		inData.buffer = buffer;
		inData.length = pictureParameterSetLength;
		returnCode = voH264DecSetParameter(hCodec, VO_ID_H264_AVCCONFIG, (long)(&inData));
		if(VORC_FAILED(returnCode))
			return returnCode;

		buffer+=pictureParameterSetLength;
	}
	return 0;
}
#if FEATURE_BFRAME
#if defined(NEON) && defined(_LINUX_ANDROID)
#include <arm_neon.h>
static char src[]={10,100,10,100,10,100,10,100};
static char mpr[]={20,50,20,50,20,50,20,50};
static int  alpha_l0=-2;
static int  alpha_fw=-2;
static int  alpha_bw= 2;
static int  adjust = 140;
static int  shift2=2;
static int  wpoffset=-16;

void testCombine_NEON()
{
	
	char dest[8];
	int  t1;
	int16x8_t _alpha_l0  = vdupq_n_s16 (alpha_l0);
	int16x8_t _adjust	 = vdupq_n_s16 (adjust);
	int16x8_t _wpoffset  = vdupq_n_s16 (wpoffset);
	//uint16x8_t _shift2 = vdupq_n_u16 (shift2);
	int16x8_t  temp;
	uint8x8_t result;
	uint8x8_t	src1 =  vld1_u8 (src);
	int16x8_t	_src =  (int16x8_t)vmovl_u8(src1);
	temp = vmulq_s16 (_src,_alpha_l0);
	temp = vqaddq_s16(temp, _adjust);
	temp = vshrq_n_s16 (temp, shift2);
	temp = vqaddq_s16(temp, _wpoffset);
	result = vqmovun_s16(temp);
	vst1_u8 (dest, result);
	t1 = COMBINE_4_BYTES_TO32(dest[0],dest[1],dest[2],dest[3]);
	printf("testCombine_ASM:%X\n",t1);

}


void testCombine_C()
{
	
	
#define _iClip255(x) iClip3(0,255,(x))
	int  result = COMBINE_4_BYTES_TO32(_iClip255(((alpha_l0 * src[0] + adjust)>> shift2) + wpoffset),_iClip255(((alpha_l0 * src[1] + adjust)>> shift2) + wpoffset),_iClip255(((alpha_l0 * src[2] + adjust)>> shift2) + wpoffset),_iClip255(((alpha_l0 * src[3] + adjust)>> shift2) + wpoffset));
	printf("testCombine_C:%X\n",result);
}
#define NEON_WP8_PREPARE2(adjust,alpha_fw,alpha_bw,wpoffset)\
	_alpha_fw  = vdupq_n_s16 (alpha_fw);\
	_alpha_bw  = vdupq_n_s16 (alpha_bw);\
	_adjust	   = vdupq_n_s16 (adjust);\
	_wpoffset  = vdupq_n_s16 (wpoffset);

#define NEON_WP8_2(src,mpr)\
	src1 =  vld1_u8 (src);\
	_src =  (int16x8_t)vmovl_u8(src1);\
	mpr1 =  vld1_u8 (mpr);\
	_mpr =  (int16x8_t)vmovl_u8(mpr1);\
	temp = vmulq_s16 (_src,_alpha_fw);\
	temp2 = vmulq_s16 (_mpr,_alpha_bw);\
	temp = vqaddq_s16(temp, temp2);\
	temp = vqaddq_s16(temp, _adjust);\
	temp = vshrq_n_s16 (temp, shift2);\
	temp = vqaddq_s16(temp, _wpoffset);\
	result = vqmovun_s16(temp);\
	vst1_u8 (src, result);
void testCombine_NEON2()
{

	
	int  t1;
	int16x8_t _alpha_fw;
	int16x8_t _alpha_bw;
	int16x8_t _adjust;
	int16x8_t _wpoffset;

	int16x8_t   temp,temp2;
	uint8x8_t   result;
	uint8x8_t	src1,mpr1;
	int16x8_t	_src,_mpr;
	NEON_WP8_PREPARE2(adjust,alpha_fw,alpha_bw,wpoffset);
	NEON_WP8_2(src,mpr);
	t1 = COMBINE_4_BYTES_TO32(src[0],src[1],src[2],src[3]);
	printf("testCombine_ASM2:%X\n",t1);

}
void testCombine_C2()
{


#define _iClip255(x) iClip3(0,255,(x))
	int  result = 			 COMBINE_4_BYTES_TO32(_iClip255(((alpha_fw * (src[0]) + alpha_bw * (mpr[0]) + adjust)>> shift2) + wpoffset),_iClip255(((alpha_fw * (src[1]) + alpha_bw * (mpr[1]) + adjust)>> shift2) + wpoffset),_iClip255(((alpha_fw * (src[2]) + alpha_bw * (mpr[2]) + adjust)>> shift2) + wpoffset),_iClip255(((alpha_fw * (src[3]) + alpha_bw * (mpr[3]) + adjust)>> shift2) + wpoffset));

	printf("testCombine_C2:%X\n",result);
}
#endif
#endif//
int InternalSetParam(ImageParameters *img,H264VdLibParam *inPa,long nID,void* lValue)
{

	int ret = 0;
	
	switch(nID)
	{
		case VO_PID_COMMON_FLUSH:
		case VO_ID_H264_FLUSH:
			//if (img->initDone)
			{
				int flushAll=0;
				
				if(img)
				{
					if (img->active_pps)
					{
						flushAll = *((signed long*)lValue);

						H264Flushing(img);
						if(flushAll==1)
							CleanOutDataQueue(img);
						AvdLog2(LL_INFO,"SetParam:H264_FLUSH,val=%d\n",flushAll);
					}
				
				}
				else
				{
					printf("voH264:not inited yet! flush fail");
				}
			}
			
			break;
		/*
		case VO_ID_H264_TIMESTAMP:
		{
			
			inPa->timeStamp	= *((TTimeStamp*)lValue);
			//img->timePerFrame	= ts->timePerFrame;
			break;
		}*/
			case VO_PID_VIDEO_OUTPUTMODE:
				//support output mode. 0, display sequence, 1. Send out immediately.
				inPa->outputOUTPUTMODE = *((signed long*)lValue);
				AvdLog2(LL_INFO,"SetParam:OUTPUTMODE ,val=%d\n",inPa->outputOUTPUTMODE );
				break;
			case VO_ID_H264_STREAMFORMAT:

				inPa->fileFormat = *((signed long*)lValue);
				AvdLog2(LL_INFO,"SetParam:format=%d\n",inPa->fileFormat);
				if (inPa->fileFormat==VO_H264_ANNEXB)
				{
					inPa->fileFormat = VO_H264_ANNEXB_COMPLETE;	
				}
#if 0//FEATURE_BFRAME
				
#ifdef NEON
				testCombine_C();
				testCombine_NEON();
				testCombine_C2();
				testCombine_NEON2();
#endif//
#endif//
				break;

			case VO_ID_H264_DISABLEDEBLOCK:

				inPa->disableDeblock = *((signed long*)lValue);
				AvdLog2(LL_INFO,"SetParam:disable deblock,%d\n",inPa->disableDeblock);
				break;	


			case VO_ID_H264_DEBLOCKFLAG:
				{
					inPa->deblockFlag = *((signed long*)lValue);
					if(inPa->deblockFlag==EDF_DUAL_CORE)
					{
						inPa->deblockRowNum = 2;//default
					}
					AvdLog2(LL_INFO,"SetParam:deblockFlag=%d\n",inPa->deblockFlag);

					break;
				}
			case VO_ID_H264_DEBLOCKROW:
				{
					inPa->deblockRowNum = *((signed long*)lValue);
					AvdLog2(LL_INFO,"SetParam:deblockRowNum=%d\n",inPa->deblockRowNum);
					break;
				}	
			case VO_PID_VIDEO_VIDEOMEMOP:
				{
					VOMEM_VIDEO_OPERATOR* shareMem= (VOMEM_VIDEO_OPERATOR*)lValue;
					if(inPa->sharedMem==NULL)
					{
						inPa->sharedMem  = (VOH264SHAREDMEMOP*)calloc(sizeof(VOH264SHAREDMEMOP),1);
						inPa->sharedMemInfo = (VOH264SHAREDMEMOP_INFO*)calloc(sizeof(VOH264SHAREDMEMOP_INFO),1);
					}
					memcpy(inPa->sharedMem,shareMem,sizeof(VOMEM_VIDEO_OPERATOR));
					AvdLog2(LL_INFO,"SetParam:VO_PID_VIDEO_VIDEOMEMOP=%X\n",inPa->sharedMem);
					break;
				}
				break;
			case VO_ID_H264_ENABLE_FIRST_FRAME_NON_INTRA:
				{
					inPa->enableFirstFrameNonIntra = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enableFirstFrameNonIntra=%d\n",inPa->enableFirstFrameNonIntra);

					break;
				}
			case VO_ID_H264_ENABLE_JUMP2I:
				{
					inPa->enableJump2I = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enableJump2I=%d\n",inPa->enableJump2I);

					break;
				}
			case VO_ID_H264_ENABLE_PARTICAIL_DECODE:
				{
					inPa->enableParticialDecode = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enableParticialDecode=%d\n",inPa->enableParticialDecode);

					break;
				}
			case VO_ID_H264_ENABLE_OUTPUTASAP:
				{
					inPa->enableOutputASAP = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enableOutputASAP=%d\n",inPa->enableOutputASAP);

					break;
				}
			case VO_ID_H264_STREAMINGMODE:
				{
					inPa->enableReuseValidSPS = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enableReuseValidSPS=%d\n",inPa->enableReuseValidSPS);

					break;
				}
			case VO_ID_H264_ENABLE_COPYINPUT:
				{
					inPa->enableCopyInput = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enableCopyInput=%d\n",inPa->enableCopyInput);

					break;
				}
			case VO_ID_H264_MULTICORE_NUM:
			case VO_PID_COMMON_CPUNUM:
				{
#if DISABLE_MT
					AvdLog2(LL_INFO,"error:this version does not enable multi-core(%d)\n",inPa->multiCoreNum);
#else
					if(img&&img->active_sps)
					{
						AvdLog2(LL_INFO,"fail to SetParam:multiCoreNum=%d, this must be done before setting seq head\n",inPa->multiCoreNum);
						ret = -1;
					
					}
					else
					{
#if SETAFFINITY
                        if(*(int*)lValue > 256)
                        {
                            inPa->multiCoreNum = (*(int*)lValue) & 0xFF;
							inPa->for_unittest = 1;
                        }
						else
						    inPa->multiCoreNum = *(int*)lValue;
#else
						inPa->multiCoreNum = *(int*)lValue;
#endif
						if(inPa->multiCoreNum>MAX_CORE_NUM)
							inPa->multiCoreNum=MAX_CORE_NUM;
						AvdLog2(LL_INFO,"SetParam:multiCoreNum=%d\n",inPa->multiCoreNum);
					}
#endif//DISABLE_MT
					break;
				}
			case VO_ID_H264_PARSE_ROWNUM:
			
				{
					inPa->rowNumOfMBs = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:rowNumOfMBs=%d\n",inPa->rowNumOfMBs);

					break;
				}
			case VO_ID_H264_ENABLE_SEI:
				{
					inPa->enbaleSEI = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:enbaleSEI=%d\n",inPa->enbaleSEI);

					break;
				}
			case VO_ID_H264_OPT_FLAG:
				{
					inPa->optFlag |= *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:optFlag=%d\n",inPa->optFlag);

					break;
				}
			case VO_PID_COMMON_CPUVERSION:
				{
					inPa->cpuVersion = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:cpuVersion=%d\n",inPa->cpuVersion);

					break;
				}
			case VO_PID_COMMON_THREAD:
				{
					inPa->callbackThread = *(int*)lValue;
					AvdLog2(LL_INFO,"SetParam:callbackThread=%X\n",inPa->callbackThread);
					break;
				}
			default:
				ret = VO_ERR_INVALID_ARG;
				AvdLog2(LL_INFO,"SetParam:unknown ID=(%d,%X)\n",nID,nID);
				break;
	}
	return ret;
}
/**
* Set parameter of the decoder instance.
* \param hCodec [in] H264 decoder instance handle, returned by voH264DecInit().
* \param nID [in] Parameter ID, Supports VO_ID_COM_HEAD_DATA, VO_ID_H264_STREAMFORMAT, VO_ID_H264_SEQUENCE_PARAMS, VO_ID_H264_PICTURE_PARAMS,VO_ID_H264_FLUSH
* \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VOH264DECRETURNCODE VOCODECAPI voH264DecSetParameter(HVOCODEC hCodec, signed long nID, signed long lValue)
{
	H264VdLibParam *inPa;
	VOCODECDATABUFFER* params;
	int temp;
	ImageParameters *img;
	GlobalH264Data* gData = (GlobalH264Data*)hCodec;
	img= gData->img;
	if(img->initDone==0)
		return VORC_COM_WRONG_STATUS; 
	inPa = img->vdLibPar;
	img->error = 0;
	//params->outputOUTPUTMODE = 1;
	switch(nID) {
	
	case VO_ID_H264_AVCCONFIG:
		//ParseAVCConfig(hCodec,(char*)lValue);
		params =  (VOCODECDATABUFFER*) lValue;
		inPa->inBuf = params->buffer;
		inPa->inBufDataLen = params->length;
#if ENABLE_TRACE
		AvdInData(inPa->inBuf,inPa->inBufDataLen);	
#endif
		voH264DecProcessNALU(img,inPa);
		break;
	case VO_PID_COMMON_HEADDATA:
	case VO_ID_H264_SEQUENCE_PARAMS:
	case VO_ID_H264_PICTURE_PARAMS:
		{
			img->error = 0;
			switch (inPa->fileFormat)
			{
			case VOH264_AVC:
				ParseAVCConfig(hCodec,(char*)lValue);	
				break;
			case VO_H264_14496_15:
				{
					params =  (VOCODECDATABUFFER*) lValue;
					inPa->inBuf = params->buffer;
					inPa->inBufDataLen = params->length;
					voH264DecProcessNALU(img,inPa);
				}
			case VO_H264_ANNEXB:
				{
					temp = inPa->fileFormat;
					inPa->fileFormat=VOIH264FM_14496_15;
					params =  (VOCODECDATABUFFER*) lValue;
					inPa->inBuf = params->buffer;
					inPa->inBufDataLen = params->length;


					H264DecodeHeadDataAnnexB(img,inPa);
					inPa->fileFormat = temp;
				}
			case VO_H264_ANNEXB_COMPLETE:
				{
					params =  (VOCODECDATABUFFER*) lValue;
					inPa->inBuf = params->buffer;
					inPa->inBufDataLen = params->length;
					AvdLog2(LL_INFO,"Set SPS/PPS2 begine,%d\n",params->length);
					DUMPDataToLOG(inPa->inBuf,inPa->inBufDataLen);
					voH264DecProcessAnnexB(img,inPa);
					AvdLog2(LL_INFO,"Set SPS/PPS2 end\n");
				}
			}
		}
		break;
	
	default:
		return InternalSetParam(img,inPa,nID,(void*)lValue);
		break;
	}
	return img->error;
}


/**
* Get parameter of the decoder instance.
* \param hCodec [in] H264 decoder instance handle, returned by voH264DecInit().
* \param nID [in] Parameter ID, supports VO_ID_COM_VIDEO_FORMAT, VO_ID_COM_VIDEO_WIDTH, VO_ID_COM_VIDEO_HEIGHT
* \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VOH264DECRETURNCODE VOCODECAPI voH264DecGetParameter(HVOCODEC hCodec, signed long nID, signed long *plValue)
{
	H264VdLibParam *inPa;
	int temp;
	ImageParameters *img;
	GlobalH264Data* gData = (GlobalH264Data*)hCodec;
	img= gData->img;
	if(img->initDone==0)
		return VORC_COM_WRONG_STATUS; 
	inPa = img->vdLibPar;
	switch(nID) {
	case VO_PID_VIDEO_FORMAT:
		{
			VOCODECVIDEOFORMAT *fmt = (VOCODECVIDEOFORMAT*)plValue;// = inPa->fileFormat;
			fmt->width  = inPa->sizeInfo.frameWidth;
			fmt->height = inPa->sizeInfo.frameHeight;
			fmt->frame_type = 5;//invalid
			break;
		}
	
	case VO_ID_H264_DISABLEDEBLOCK:
		*plValue = inPa->disableDeblock>0;
		break;
	/*
	case VO_ID_H264_TIMESTAMP:
		{
			VOTIMESTAMPINFO* ts = (VOTIMESTAMPINFO*)plValue;
			ts->timeStamp		=	inPa->timeStamp;
			//ts->timePerFrame	=	img->timePerFrame;
			break;
		}*/
	case VO_PID_VIDEO_FRAMETYPE:
		{
			VO_CODECBUFFER* buf = (VO_CODECBUFFER*)plValue;
			unsigned char* buffer=buf->Buffer;
			if(inPa->fileFormat==VOIH264FM_ANNEXB)
			{
				if (buffer[2]==0&&buffer[3]==1)
				{
					buffer+=4;
				}
				else
				{
					buffer+=3;
				}
			}
			buf->Time =ProbeH264FrameType(buffer);
			break;
		}
	case VO_PID_COMMON_HeadInfo:
		{
#ifndef ARM
			VO_HEAD_INFO* headInfo=(VO_HEAD_INFO*)plValue;
			VOCODECDATABUFFER params;
			params.buffer = headInfo->Buffer;
			params.length = headInfo->Length;
			voH264DecSetParameter(hCodec,(signed long)VO_PID_COMMON_HEADDATA,(signed long)&params);
			if (img->error)
			{
				if (VO_H264_ERR_NotSupportProfile==img->error)
				{
					sprintf(headInfo->Description,"Error:the profile is not supported\n");
				}
				else
				{
					int size1=0;
					sprintf(headInfo->Description,"Parsing error:%d,format=%d\n",
						img->error,img->vdLibPar->fileFormat);
					size1=strlen(headInfo->Description);
					memcpy(headInfo->Description+size1,headInfo->Buffer,headInfo->Length);
					headInfo->Size=size1+headInfo->Length;
					headInfo->Description[headInfo->Size]='\0';
				}
			}
			else
			{
				char tmpBuf[1024];
				int i;
				seq_parameter_set_rbsp_t *sps=img->active_sps;
				pic_parameter_set_rbsp_t *pps = img->active_pps;
				headInfo->Description[0]='\0';
				sprintf(tmpBuf, "\n********************* NALU_TYPE_SPS(::InterpretSPS) *********************************\r\n");
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nprofile_idc                              = %d\r\n", sps->profile_idc);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nconstrained_set0_flag                    = %d\r\n", sps->constrained_set0_flag);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nconstrained_set1_flag                    = %d\r\n", sps->constrained_set1_flag);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nconstrained_set2_flag                    = %d\r\n", sps->constrained_set2_flag);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nseq_parameter_set_id                     = %d\r\n", sps->seq_parameter_set_id);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nlog2_max_frame_num_minus4                = %d\r\n", sps->log2_max_frame_num_minus4);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_order_cnt_type                       = %d\r\n", sps->pic_order_cnt_type);
				if (sps->pic_order_cnt_type == 0)
				{
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nlog2_max_PicOrderCntLsb_minus4        = %d\r\n", sps->log2_max_pic_order_cnt_lsb_minus4);
				}
				else if (sps->pic_order_cnt_type == 1)
				{
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\ndelta_pic_order_always_zero_flag         = %d\r\n", sps->delta_pic_order_always_zero_flag);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\noffset_for_non_ref_pic                   = %d\r\n", sps->offset_for_non_ref_pic);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\noffset_for_top_to_bottom_field           = %d\r\n", sps->offset_for_top_to_bottom_field);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nnum_ref_frames_in_pic_order_cnt_cycle    = %d\r\n", sps->num_ref_frames_in_pic_order_cnt_cycle);
					for(i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
						strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\noffset_for_ref_frame(%d)                  = %d\r\n", i, sps->offset_for_ref_frame[i]);
				}
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nnum_ref_frames                           = %d\r\n", sps->num_ref_frames);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\ngaps_in_frame_num_value_allowed_flag     = %d\r\n", sps->gaps_in_frame_num_value_allowed_flag);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_width_in_mbs_minus1                  = %d(width=%d)\r\n", sps->pic_width_in_mbs_minus1,(sps->pic_width_in_mbs_minus1+1)*16);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_height_in_map_units_minus1           = %d(height=%d)\r\n", sps->pic_height_in_map_units_minus1,(sps->pic_height_in_map_units_minus1+1)*16);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nframe_mbs_only_flag                      = %d", sps->frame_mbs_only_flag);
				if (!sps->frame_mbs_only_flag)
				{
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nmb_adaptive_frame_field_flag             = %d\r\n", sps->mb_adaptive_frame_field_flag);
				}
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\ndirect_8x8_inference_flag                = %d\r\n", sps->direct_8x8_inference_flag);
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nframe_cropping_flag                      = %d\r\n", sps->frame_cropping_flag);
				if (sps->frame_cropping_flag)
				{
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nframe_cropping_rect_left_offset          = %d\r\n", sps->frame_cropping_rect_left_offset);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nframe_cropping_rect_right_offset         = %d\r\n", sps->frame_cropping_rect_right_offset);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nframe_cropping_rect_top_offset           = %d\r\n", sps->frame_cropping_rect_top_offset);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nframe_cropping_rect_bottom_offset        = %d\r\n", sps->frame_cropping_rect_bottom_offset);
				}
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nvui_parameters_present_flag              = %d\r\n", sps->vui_parameters_present_flag);	
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\n\n  ...end SPS...\r\n");
		
			// dump to log file;
			strcat(headInfo->Description,tmpBuf);
#if 1	//PPS		
			sprintf(tmpBuf, "\n\n********************* NALU_TYPE_PPS(::InterpretPPS) *********************************");
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_parameter_set_id              = %d\r\n", pps->pic_parameter_set_id);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nseq_parameter_set_id              = %d\r\n", pps->seq_parameter_set_id);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nentropy_coding_mode_flag          = %s\r\n", pps->entropy_coding_mode_flag ? "CABAC" : "UVLC");
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_order_present_flag            = %d\r\n", pps->pic_order_present_flag);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nnum_slice_groups_minus1           = %d\r\n", pps->num_slice_groups_minus1);
#ifndef VOI_H264D_BLOCK_FMO
			if (pps->num_slice_groups_minus1 > 0)
			{
				strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nslice_group_map_type              = %d\r\n", pps->slice_group_map_type);
				if (pps->slice_group_map_type == 0)
				{
					for (i=0; i<=pps->num_slice_groups_minus1; i++)
						strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nrun_length_minus1(%d\r\n)              = %d\r\n", i, pps->run_length_minus1[i]);
				}
				else if (pps->slice_group_map_type == 2)
				{
					for (i=0; i<pps->num_slice_groups_minus1; i++)
					{
						strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\ntop_left(%d\r\n)                       = %d\r\n", i, pps->top_left[i]);
						strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nbottom_right(%d\r\n)                   = %d\r\n", i, pps->bottom_right[i]);
					}
				}
				else if (pps->slice_group_map_type == 3 ||
					pps->slice_group_map_type == 4 ||
					pps->slice_group_map_type == 5)
				{
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nslice_group_change_direction_flag = %d\r\n", pps->slice_group_change_direction_flag);
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nslice_group_change_rate_minus1    = %d\r\n", pps->slice_group_change_rate_minus1);
				}
				else if (pps->slice_group_map_type == 6)
				{
					strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nnum_slice_group_map_units_minus1  = %d\r\n", pps->num_slice_group_map_units_minus1);
					for (i=0; i<=pps->num_slice_group_map_units_minus1; i++)
						strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nslice_group_id(%d\r\n)                 = %d\r\n", i, pps->slice_group_id[i]);
				}
			}
#endif //VOI_H264D_BLOCK_FMO

			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nnum_ref_idx_l0_active_minus1      = %d\r\n", pps->num_ref_idx_l0_active_minus1);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nnum_ref_idx_l1_active_minus1      = %d\r\n", pps->num_ref_idx_l1_active_minus1);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nweighted_pred_flag                = %d\r\n", pps->weighted_pred_flag);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nweighted_bipred_idc               = %d\r\n", pps->weighted_bipred_idc);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_init_qp_minus26               = %d\r\n", pps->pic_init_qp_minus26);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\npic_init_qs_minus26               = %d\r\n", pps->pic_init_qs_minus26);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nchroma_qp_index_offset            = %d\r\n", pps->chroma_qp_index_offset);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\ndeblocking_filter_control_present_flag = %d\r\n", pps->deblocking_filter_control_present_flag);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nconstrained_intra_pred_flag       = %d\r\n", pps->constrained_intra_pred_flag);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\nredundant_pic_cnt_present_flag    = %d\r\n", pps->redundant_pic_cnt_present_flag);
			strcat(headInfo->Description,tmpBuf);sprintf(tmpBuf, "\n\n  ...end PPS...\r\n");strcat(headInfo->Description,tmpBuf);
#endif//
		 }
#endif//ARM
			break;
		}
	case VO_PID_VIDEO_ASPECTRATIO:
	{
		*plValue=VO_RATIO_11;
		
		if(img->active_sps)
		{
			vui_seq_parameters_t *  vui=img->active_sps->vui_seq_parameters;
			if(vui)
			{
#define MAX_ASPECT_INFO_SIZE	16
				int id=vui->aspect_ratio_idc;
				float ratio;
				int width,height;
				int ratio_w, ratio_h; 
				width = img->sizeInfo->width;
				height = img->sizeInfo->height;
				if(id>0&&id<=MAX_ASPECT_INFO_SIZE)
				{
					const static int aspectInfo[MAX_ASPECT_INFO_SIZE][2]={
						
						{1,1},
						{12,11},
						{10,11},
						{16,11},
						{40,33},
						{24,11},
						{20,11},
						{32,11},
						{80,33},
						{18,11},
						{15,11},
						{64,33},
						{160,99},
						{4,3},
						{3,2},
						{2,1},
					};

#if 0					
					ratio = ((float)((width)*aspectInfo[id-1][0]*10)/(float)(height*aspectInfo[id-1][1]));//voH264IntDiv((img->sizeInfo->width+img->sizeInfo->height/2)*aspectInfo[id-1][0]*10,img->sizeInfo->height*aspectInfo[id-1][1]);
#else
                    ratio_w = (width)*aspectInfo[id-1][0];
                    ratio_h = height*aspectInfo[id-1][1];
#endif
					
				}
				else if (id==255)
				{
#if 1
                    ratio_w = vui->sar_width*width;
                    ratio_h = vui->sar_height*height;
#else
					ratio=((float)(vui->sar_width*width*10)/((float)vui->sar_height*height));
#endif
				}
#if 0				
				if (ratio==0)
					*plValue=0;
				else if (ratio<12)
					*plValue=VO_RATIO_11;
				else if (ratio<15)
					*plValue=VO_RATIO_43;
				else if (ratio<20)
					*plValue=VO_RATIO_169;
				else 
					*plValue=VO_RATIO_21;
#else
                if (ratio_w == ratio_h)
                {
                    *((VO_S32 *)plValue) = VO_RATIO_11;
                }
                else if (ratio_w*3 == ratio_h*4)
                {
                    *((VO_S32 *)plValue) = VO_RATIO_43;
                }
                else if (ratio_w*9 == ratio_h*16)
                {
                    *((VO_S32 *)plValue) = VO_RATIO_169;
                }
                else if (ratio_w == ratio_h*2)
                {
                    *((VO_S32 *)plValue) = VO_RATIO_21;
                }
                else
                {
                    *((VO_S32 *)plValue) = (ratio_w<<16)|ratio_h;
                }
   
#endif
			}
			
		}
		break;
	}
	case VOID_COM_VIDEO_SHAREDMEMWAITOUTPUT:
		{
			ProcessOneNALU(img,4);
			if(img->error)
				return img->error;
			break;
		}
	case VO_ID_H264_VUI:
		{
			TH264VideoUsebilityInfo* info = (TH264VideoUsebilityInfo*)plValue;
			if(img->active_sps)
			{
				vui_seq_parameters_t *  vui=img->active_sps->vui_seq_parameters;
				if(vui)
				{
					info->aspect_ratio_idc  = vui->aspect_ratio_idc;
					info->sar_width			= vui->sar_width;
					info->sar_height		= vui->sar_height;
				}
				else
				{
					memset(info, 0, sizeof(TH264VideoUsebilityInfo));
				}
			}
		}
		break;
	case VO_ID_H264_T35_USERDATA:
		{
		
			VO_CODECBUFFER* buf = (VO_CODECBUFFER*)plValue;
			//AvdLog2(LL_INFO,"T35_get_size=%d\n",img->prevOutPicture->seiPicInfo.Length);
			if(img->prevOutPicture&&img->prevOutPicture->seiPicInfo.Length>0)
			{
				VO_CODECBUFFER* buf2 = &img->prevOutPicture->seiPicInfo;
				AvdLog2(LL_INFO,"T35_out:poc=%d,size=%d\n",(int)buf2->Time,buf2->Length);
				*buf = *buf2;
				buf2->Length = 0;
			}
			else
			{
				buf->Buffer = NULL;
				buf->Length = 0;
			}
		}
		break;
	default:
		return VORC_COM_WRONG_PARAM_ID;
	}
	return VORC_OK;
}





























