#include "h264decoder.h"
//#include "configfile.h"
#include "voH264New.h"
#include "nalucommon.h"
#include "mbuffer.h"
#include "output.h"
#include "image.h"
#include "vlc.h"
#include "voLog.h"

#define H264_IS_ANNEXB_SPS(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define H264_IS_ANNEXB2_SPS(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
#define DUMPDATA 0

static VO_U8* GetNextNalu_sps(VO_U8* currPos,int size)
{
	VO_U8* p = currPos;  
	VO_U8* endPos = currPos+size-4;
	for (; p < endPos; p++)
	{
		if (H264_IS_ANNEXB_SPS(p))
			return p+3;
		else if(H264_IS_ANNEXB2_SPS(p))
			return p+4;
	}
	if(p>=endPos)
		p = NULL;

	return p;
}
#undef H264_IS_ANNEXB_SPS
#undef H264_IS_ANNEXB2_SPS

#define H264_IS_ANNEXB(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)==1))
#define H264_IS_ANNEXB2(p) ( !(*((p)+0)) && !(*((p)+1)) && !(*((p)+2))&& (*((p)+3)==1))
static const VO_U8* GetNextNalu(const VO_U8* currPos,int size)
{
	const VO_U8* p = currPos+4;  
	const VO_U8* endPos = currPos+size-4;
	for (; p < endPos; p++)
	{
		if (H264_IS_ANNEXB(p))
			return p;
		else if(H264_IS_ANNEXB2(p))
			return p;
	}
	if(p>=endPos)
		p = NULL;

	return p;
}
#undef H264_IS_ANNEXB
#undef H264_IS_ANNEXB2
static VO_U32 vo_gcd(int m, int n)
{
  if (m == 0)
    return n;
  if (n == 0)
    return m;
  if (m < n)
  {
    int tmp = m;
    m = n;
    n = tmp;
  }
  while (n != 0)
  {
    int tmp = m % n;
    m = n;
    n = tmp;
  }

  return m;
}

VO_U32	VO_API  voH264DecGetOutputData (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo);

VO_U32  VO_API  voH264DecInit (VO_HANDLE * phDec,VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData)
{
  VO_U32 returnCode;
  VOLOGI("init dec\r\n");
  returnCode = OpenDecoder((DecoderParams **)phDec, pUserData);
  VOLOGI("init dec done\r\n");
  return returnCode;	
}

VO_U32	VO_API  voH264DecSetParam(VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
  VOCODECDATABUFFER* params;
  VOLOGI("setparam%x\r\n",uParamID);
  if(hDec)
  {
  DecoderParams *pDecoder = (DecoderParams *)hDec;
  H264DEC_G *pDecGlobal = pDecoder->pDecGlobal;
  
  VO_U32 returnCode=0;
  if(pDecGlobal->initDone==0)
		return VORC_COM_WRONG_STATUS; 
  //printf("uParamID:%x\n",uParamID);

  switch(uParamID)
  {
    case VO_ID_H264_INPUTPARAM:
	  //SetInputParameters(pData,hDec);
	  break;
	case VO_PID_COMMON_HEADDATA:
	case VO_ID_H264_SEQUENCE_PARAMS:
	case VO_ID_H264_PICTURE_PARAMS:
	{
	   
	  NALU_t *nalu = pDecoder->pDecGlobal->nalu;
	  params = (VOCODECDATABUFFER*) pData;
      nalu->buf = params->buffer;
      nalu->len = params->length;
#if DUMPDATA
	  {
	      FILE *fp = fopen("/sdcard/H264/dumpdata.264", "ab+");
          fwrite(params->buffer, params->length, 1,fp);
	      fclose(fp);
      }
#endif
	  switch (pDecoder->FileFormat)
	  {
	    case VOH264_AVC:
	      VOH264ERROR(VO_H264_ERR_AVC_NULL);
	      break;
	    case VOH264_14496_15:
		  
	      returnCode = voH264DecProcessNALU(hDec);
	      break;
	    default:
	      returnCode = voH264DecProcessAnnexB(hDec);
	      break;
	  }

	  break;
	}
	case VO_PID_VIDEO_THUMBNAIL_MODE:
	{
		VO_S32 nThumbnailMode = *(VO_S32 *)pData;
		pDecGlobal->bThumbnailMode = nThumbnailMode;
		break;
	}
	case VO_PID_VIDEO_OUTPUTMODE:
	{
		VO_S32 nDirectOutputMode = *(VO_S32 *)pData;
		pDecGlobal->bDirectOutputMode = nDirectOutputMode;
		break;
	}
    case VO_ID_H264_FLUSH_PICS:
	{
#if COMBINE_FRAME
		if (pDecoder->bFindFirstSlice)
		{
			pDecoder->frameLen[pDecoder->new_input] = pDecoder->leftBytes;
// printf("last combine %d\n", pDecoder->leftBytes);
			pDecoder->new_input++;
			pDecoder->leftBytes = 0;
			pDecoder->bFindFirstSlice = 0;
		}
		else if (pDecoder->new_input)
		{
            
		}
		else
		{
			//reset input buffer
			pDecoder->curPos = pDecoder->inBuf;
			pDecoder->lastPos = pDecoder->inBuf;
			pDecoder->leftBytes = 0;
			pDecoder->usedBytes = 0;
		}

		if (pDecoder->bHasNext)
		{
			pDecoder->bNeedFlush = 1;	//need call GetOutputData before flush, flush will delay
			break;
		}

		while(pDecoder->new_input)
		{
			NALU_t *nalu = pDecGlobal->nalu;
			nalu->buf = pDecoder->curPos;
			nalu->len = pDecoder->frameLen[0];
			nalu->Time = pDecoder->Time[0];
			nalu->UserData = pDecoder->UserData[0];

			//LOGI("first:%d\r\n",pDecGlobal->first_sps);
			if(!(pDecGlobal->first_sps&1))
			{
				VO_S32 iRet = 0;
				VO_U8* inBuffer= nalu->buf;
				VO_U32 BufferLen = nalu->len; 
				VO_U8 * nextFrame,*currFrame;
				VO_S32 leftSize=BufferLen,inSize;

				if (BufferLen <=4)
				{
					return VO_ERR_INPUT_BUFFER_SMALL;
				}
				nextFrame = currFrame = inBuffer;
				currFrame = GetNextNalu_sps(currFrame,leftSize);
				if (currFrame!=NULL)
				{
					leftSize-=(currFrame-inBuffer);
				}
				while (leftSize>=3&&currFrame!=NULL)
				{
					nextFrame = GetNextNalu_sps(currFrame,leftSize);
					if(nextFrame)
					{
						inSize = nextFrame-currFrame;
					}
					else
					{
						inSize = leftSize;
					}
					if(inSize >=2)
					{
						nalu->buf = currFrame;
						nalu->len = inSize;

						nalu->forbidden_bit     = (*(nalu->buf) >> 7) & 1;
						nalu->nal_reference_idc = (NalRefIdc) ((*(nalu->buf) >> 5) & 3);
						nalu->nal_unit_type     = (NaluType) ((*(nalu->buf)) & 0x1f);
						if(nalu->nal_unit_type==NALU_TYPE_SPS)
						{
							iRet = process_first_sps(pDecGlobal);
							pDecGlobal->first_sps |= !iRet;
							break;
						}
					}
					leftSize-=inSize;
					currFrame=nextFrame;
				}
				if(!pDecGlobal->first_sps)
					return VO_H264_ERR_SPSIsNULL;
				nalu->buf = inBuffer;
				nalu->len = BufferLen;
			}

			if (pDecGlobal->first_sps == 1)
			{
				if(pDecGlobal->nThdNum > 1)
				{
					VO_U32 maxThdNum = /*pDecGlobal->interlace?MAX_THREAD_NUM : */MAX_THREAD_NUM2;
					if(pDecGlobal->nThdNum>maxThdNum)
						pDecGlobal->nThdNum = maxThdNum;

						InitH264Threads2(pDecGlobal);
					pDecGlobal->thdinit = 1;
				}
				pDecGlobal->first_sps |= 2;
			}

			//if (pDecGlobal->licenseCheck->lastVideo)
			//{
			//	pDecGlobal->licenseCheck->resetVideo(pDecGlobal->licenseCheck->hCheck,NULL);
			//	pDecGlobal->licenseCheck->lastVideo=NULL;
			//}

#if USE_FRAME_THREAD
			if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
			{
				if (pDecoder->FileFormat == VOH264_AVC)
				{
					VOH264ERROR(VO_H264_ERR_AVC_NULL);
				}
				if (pDecGlobal->first_sps == 3)
				{
					returnCode = DecodeFrameMain(pDecGlobal,NULL, NULL, 3);
				}
			}
			else
#endif
			{
				//printf("fileformat:%d len:%d\r\n",pDecoder->p_Inp->FileFormat,pDecGlobal->nalu->len);
				switch (pDecoder->FileFormat)
				{
				case VOH264_AVC:
					VOH264ERROR(VO_H264_ERR_AVC_NULL);
					break;
				case VOH264_14496_15:
					returnCode = voH264DecProcessNALU(hDec);
					break;
				default:
					returnCode = voH264DecProcessAnnexB(hDec);
					break;
				}
			}
			{
				VO_U32 i;
				pDecoder->curPos += pDecoder->frameLen[0];
				pDecoder->leftBytes -= pDecoder->frameLen[0];
				pDecoder->new_input--;
				for (i = 0; i < pDecoder->new_input; ++i)
				{
					pDecoder->frameLen[i] = pDecoder->frameLen[i+1];
					pDecoder->Time[i] = pDecoder->Time[i+1];
					pDecoder->UserData[i] = pDecoder->UserData[i+1];
				}
			}
		}
#endif
		if (pDecGlobal->first_sps != 3)
		{
			return VO_ERR_WRONG_STATUS;
		}
#if USE_FRAME_THREAD
		if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		{
			DecodeFrameMain(pDecGlobal, NULL, NULL, 2);
		}
		else
#endif
		{
			flush_dpb(pDecGlobal,pDecGlobal->p_Dpb);
			update_ref_list(pDecGlobal->p_Dpb);
		    update_ltref_list(pDecGlobal->p_Dpb);
		}
		break;
	case VO_PID_COMMON_FLUSH:
		{
			//Add for clean buffer
			VO_VIDEO_BUFFER outData;
			VO_VIDEO_OUTPUTINFO outFormat;
			VO_S32 iRet;

			if (pDecGlobal->first_sps != 3)
			{
				return VO_ERR_WRONG_STATUS;
			}
#if USE_FRAME_THREAD
			if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
			{
				DecodeFrameMain(pDecGlobal, NULL, NULL, 2);
			}
			else
#endif
			{
				if (pDecGlobal->p_Dpb->last_picture)
				{
					pDecGlobal->p_Dpb->last_picture->is_output=1;
					pDecGlobal->p_Dpb->last_picture=NULL;
				}
				flush_dpb(pDecGlobal,pDecGlobal->p_Dpb);
				update_ref_list(pDecGlobal->p_Dpb);
		        update_ltref_list(pDecGlobal->p_Dpb);
				//LOGI("flush premove:%d\r\n",pDecGlobal->p_Dpb->remove_size);
			}
#if COMBINE_FRAME
			pDecoder->new_input = 0;
#endif

            //LOGI("pDecoder->bFindFirstSlice:%d,pDecoder->new_input:%d,pDecoder->bHasNext:%d\r\n",pDecoder->bFindFirstSlice,pDecoder->new_input,pDecoder->bHasNext);
			iRet = voH264DecGetOutputData(hDec,&outData,&outFormat);
			while(!iRet && (outData.Buffer[0] || outFormat.Flag)) //in the end of file, we must get all next output
			{
// 				if (outData.Buffer[0])
// 				{
// 
// 				}
// 				else
// 				{
// 					//next output not ready, don't break in the end of file
// // 					Sleep(0);
// 				}
                //LOGI("while pDecoder->bFindFirstSlice:%d,pDecoder->new_input:%d,pDecoder->bHasNext:%d\r\n",pDecoder->bFindFirstSlice,pDecoder->new_input,pDecoder->bHasNext);
				iRet = voH264DecGetOutputData(hDec,&outData,&outFormat);
			}
			returnCode = iRet;
			//LOGI("flush poutput:%d returnCode:%x\r\n",pDecGlobal->output_size,returnCode);
#if COMBINE_FRAME
			//reset input buffer
			pDecoder->curPos = pDecoder->inBuf;
			pDecoder->lastPos = pDecoder->inBuf;
			pDecoder->leftBytes = 0;
			pDecoder->usedBytes = 0;
			pDecoder->bFindFirstSlice = 0;
			pDecoder->new_input = 0;
			pDecoder->bHasNext = 0;
#endif
		}
      break;
	}
	case VO_ID_H264_ENABLE_SEI:
	{
	  pDecGlobal->enbaleSEI= *((VO_U32*)pData);  
      break;
	}
	case VO_PID_COMMON_CPUNUM:
	case VO_ID_H264_MULTICORE_NUM:
		{
			VO_S32 nCPU = *((VO_S32*)pData);
			VO_S32 maxThdNum;
#if USE_FRAME_THREAD
			maxThdNum = MAX_THREAD_NUM2;
#else
			maxThdNum = MAX_THREAD_NUM;
#endif
			if (nCPU <= 0)
			{
				return VO_ERR_INVALID_ARG;
			}
			if (nCPU > maxThdNum)
			{
				nCPU = maxThdNum;
				*((VO_S32*)pData) = nCPU; //put back the value really set
			}
			if(pDecGlobal->first_sps==3)
			{
				return VO_ERR_WRONG_STATUS;
			}
			pDecGlobal->nThdNum = nCPU;
			return returnCode;

			if (nCPU != pDecGlobal->nThdNum)
			{
				if (pDecGlobal->nThdNum > 1)
				{
#if USE_FRAME_THREAD
					ReleaseH264Threads2(pDecGlobal);
#else
					ReleaseH264Threads(pDecGlobal);
#endif
				}
				pDecGlobal->nThdNum = nCPU;

				if (nCPU > 1)
				{
#if USE_FRAME_THREAD
					returnCode = InitH264Threads2(pDecGlobal);
#else
					returnCode = InitH264Threads(pDecGlobal);
#endif
				}
			}
		}
	  break;
	case VO_ID_H264_STREAMFORMAT:
	  pDecoder->FileFormat = *((VO_S32*)pData);
	  if (pDecoder->FileFormat==VO_H264_ANNEXB)
	  {
		pDecoder->FileFormat = VO_H264_ANNEXB_COMPLETE;	
	  }
	  break;
	case VO_ID_H264_DEBLOCKFLAG:
    case VO_ID_H264_DISABLEDEBLOCK:
	  pDecGlobal->disableDeblock = *((VO_S32*)pData);
	  break;
	case VO_PID_COMMON_FRAME_BUF_EX:
	  pDecGlobal->extra_dpb = *((VO_S32*)pData);
	  //pDecGlobal->extra_dpb =16;
	  //returnCode = VO_ERR_WRONG_PARAM_ID;
	  break;

	default:
	  returnCode = VO_ERR_WRONG_PARAM_ID;
  }

  return returnCode;
  }
  else
    return 0;
}

VO_U32	VO_API  voH264DecSetInputData (VO_HANDLE hDec, VO_CODECBUFFER * pInput)
{
  DecoderParams *pDecoder = hDec;
  NALU_t *nalu;
  VOLOGI("data set, hDec:%p, buf:%p,len:%d\n", pDecoder, pInput->Buffer, pInput->Length);
  if(pDecoder==NULL || pDecoder->pDecGlobal == NULL)
  {
// printf("null point!\n");
		return VO_ERR_INVALID_ARG;
  }
  nalu = pDecoder->pDecGlobal->nalu;
  if(nalu==NULL)
  {
// printf("null point!\n");
	  return VO_ERR_INVALID_ARG;
  }
#if USE_FRAME_THREAD
  if (pInput->Length > MAX_INPUT_SIZE || pInput->Length == 0)
  {
	  return VO_ERR_INVALID_ARG;
  }
#endif
// LOGI("SetInputData!!!!\n");
#if DUMPDATA
  {
	  FILE *fp = fopen("/sdcard/H264/dumpdata.264", "ab+");
      fwrite(pInput->Buffer, pInput->Length, 1,fp );
	  fclose(fp);
  }
#endif
#if COMBINE_FRAME
  {
	  VO_U8 *inBuf = pDecoder->inBuf;
	  const VO_U8 *curPos = pDecoder->curPos;
	  const VO_U8 *lastPos = pDecoder->lastPos;
	  VO_U8 *endPos = pDecoder->curPos + pDecoder->leftBytes;

	  assert(endPos - inBuf <= 2*MAX_INPUT_SIZE);
	if (pDecoder->leftBytes + pInput->Length >= 2*MAX_INPUT_SIZE)
	{
		if (pDecoder->new_input == 0 && pDecoder->leftBytes >= MAX_INPUT_SIZE)
		{
			curPos = pDecoder->curPos = inBuf;
			lastPos = pDecoder->lastPos = inBuf;
			endPos = inBuf;
			pDecoder->leftBytes = 0;
			pDecoder->bFindFirstSlice = 0;
		}
		else
		{
			return VO_ERR_INPUT_BUFFER_SMALL;
		}
	}
	if (endPos + pInput->Length >= inBuf + 2*MAX_INPUT_SIZE)
	{
		//copy left data to the head of memory pool
		memmove(inBuf, curPos, pDecoder->leftBytes);
		pDecoder->lastPos = lastPos -= (curPos - inBuf); 
		pDecoder->curPos = inBuf;
		curPos = inBuf;
		endPos = inBuf + pDecoder->leftBytes;
	}
	if (pDecoder->new_input >= MAX_FRAME_INPUT)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}
	memcpy(endPos, pInput->Buffer, pInput->Length);
	pDecoder->leftBytes += pInput->Length;

	{
		const VO_U8 *last = lastPos;		//last frame end
		const VO_U8 *currFrame = endPos;
		const VO_U8 *nextFrame = endPos;
		VO_U32 leftSize = pInput->Length;
		VO_U32 inSize;

		while (leftSize>=3&&currFrame!=NULL)
		{
			nextFrame = GetNextNalu(currFrame,leftSize);
			if(nextFrame)
			{
				inSize = nextFrame-currFrame;
			}
			else
			{
				inSize = leftSize;
			}
			if(inSize >=2)
			{
				const VO_U8 *tmp_ptr = currFrame;
				VO_U32 value;
				VO_U32 nal_type;

				while (!*(tmp_ptr++));
				value = nal_type = (*tmp_ptr++)&0x1F;
				// printf("find nalu, type %d\r\n", value);
				if (value != 1 && value != 5)
				{
					if (pDecoder->bFindFirstSlice)
					{
						//find next pic start
						pDecoder->bFindFirstSlice = 0;	//next is not a slice
						pDecoder->frameLen[pDecoder->new_input] = currFrame - last;
// 						pDecoder->Time[pDecoder->new_input] = pInput->Time;
// 						pDecoder->UserData[pDecoder->new_input] = pInput->UserData;
// printf("combine %d\n", currFrame - last);
						last = currFrame;
						pDecoder->new_input++;
					}
					leftSize-=inSize;
					currFrame=nextFrame;
					continue;	//not a slice, continue finding next nalu
				}

				if (tmp_ptr+3 < currFrame+inSize)
				{
					//slice or IDR
					value = (((VO_U32)(*tmp_ptr))<<24) | ((VO_U32)(*(tmp_ptr+1))<<16) | ((VO_U32)(*(tmp_ptr+2))<<8) | ((VO_U32)*(tmp_ptr+3));
				}
				else
				{
					VO_U32 i;
					value = 0;
					for (i = 0; i < 4; ++i)
					{
						value <<= 8;
						value |= tmp_ptr < currFrame+inSize ? *(tmp_ptr++) : 0;
					}
				}
				// printf("value %08X\r\n", value);
				if(value >=(1<<27))
				{
					value >>= 32-9;
					value = VO_UE_VAL[value];
				}
				else
				{
					value >>= 2*vo_big_len(value)-31;
					value--;
				}
				if (value == 0)	
				{
					if(pDecoder->pDecGlobal->bThumbnailMode ==1 && nal_type == 5 && ((leftSize - inSize) < 3))
					{
						VO_U8 padData[4] = {0x00,0x00,0x00,0x01};
						if (currFrame + inSize + 4 < inBuf + 2*MAX_INPUT_SIZE)
						{
							memcpy((void *)(currFrame + inSize) , padData , 4);
							leftSize = leftSize + 4;
							nextFrame = currFrame + inSize;
						}
					}

					if (pDecoder->bFindFirstSlice)
					{
						//find next pic start
						pDecoder->frameLen[pDecoder->new_input] = currFrame - last;
// printf("combine %d\n", currFrame - last);
						last = currFrame;
						pDecoder->new_input++;

//						leftSize-=inSize;
//						currFrame=nextFrame;
//						continue;
					}
					pDecoder->Time[pDecoder->new_input] = pInput->Time;
					pDecoder->UserData[pDecoder->new_input] = pInput->UserData;
					pDecoder->bFindFirstSlice = 1;	//next is first slice of a pic
				}
				else if(pDecoder->pDecGlobal->bThumbnailMode ==1 && nal_type == 5 && ((leftSize - inSize) < 3))
				{
					VO_U8 padData[4] = {0x00,0x00,0x00,0x01};
					if (currFrame + inSize + 4 < inBuf + 2*MAX_INPUT_SIZE)
					{
						memcpy((void *)(currFrame + inSize) , padData , 4);
						leftSize = leftSize + 4;
						nextFrame = currFrame + inSize;
					}
				}
			}
			leftSize-=inSize;
			currFrame=nextFrame;
		}
		pDecoder->lastPos = last;
		pDecoder->usedBytes = pInput->Length;
	}
  }
#else
  nalu->buf = pInput->Buffer;
  nalu->len = pInput->Length;
  nalu->Time = pInput->Time;
  nalu->UserData = pInput->UserData;
  pDecoder->new_input = 1;
#endif
  /*{
	  FILE *fp = fopen("/sdcard/new_data.264", "ab");
      fwrite(pInput->Buffer,pInput->Length,1,fp);
	  fclose(fp);
  }*/
  //printf("data set\r\n");
// LOGI("SetInputData!!!!return 0\n");

  return 0;
}

VO_U32	VO_API  voH264DecGetParam (VO_HANDLE hDec, VO_S32 uParamID, VO_PTR pData)
{
  DecoderParams *pDecoder = hDec;
  H264DEC_G *pDecGlobal;
  if(pDecoder==NULL)
  	return VO_ERR_INVALID_ARG;
  pDecGlobal = pDecoder->pDecGlobal;
  VOLOGI("getParam:%x\r\n",uParamID);
  if(pDecoder)
  {
	switch(uParamID) 
	{
	  case VO_PID_VIDEO_FORMAT:
	  {
		VOCODECVIDEOFORMAT *fmt = (VOCODECVIDEOFORMAT*)pData;// = inPa->fileFormat;
		fmt->width  = pDecGlobal->width;
		fmt->height = pDecGlobal->height;
		fmt->frame_type = 5;//invalid
		break;
	  }
	  case VO_ID_H264_DISABLEDEBLOCK:
		*((VO_S32*)pData) = (VO_S32)(pDecGlobal->DFDisableIdc>0);
		break;
	  case VO_ID_H264_INTERLACE:
	  	*((VO_S32*)pData) = (VO_S32)(pDecGlobal->interlace);
		break;
      case VO_PID_VIDEO_ASPECTRATIO:
#if 1
		  {
			  const static int aspectInfo[16][2]={
				  {1,1}, {12,11}, {10,11}, {16,11}, {40,33},{24,11},{20,11},{32,11},{80,33},{18,11},{15,11},{64,33},{160,99}, {4, 3}, {3, 2}, {2, 1}
			  };
			  int width;
			  int height;
			  int ratio = 0;
			  vui_seq_parameters_t * info = NULL;
			  if(pDecGlobal->spsNum >= 0 && pDecGlobal->SeqParSet[pDecGlobal->spsNum].Valid)
			  {
				  info = &pDecGlobal->SeqParSet[pDecGlobal->spsNum].vui_seq_parameters;
			  }
			  else if (pDecGlobal->SeqParSet[0].Valid)
			  {
				  info = &pDecGlobal->SeqParSet[0].vui_seq_parameters;
			  }
			  else
			  {
				  return VO_ERR_INVALID_ARG;
			  }
			
			  width = 0;
			  height = 0;
			  if( info->aspect_ratio_idc == 0 || info->aspect_ratio_idc == 1)
				  *((VO_IV_ASPECT_RATIO*)pData) = 0;


			  if(info->aspect_ratio_idc > 1 && info->aspect_ratio_idc < 17)
			  {
				  width = pDecGlobal->width*aspectInfo[info->aspect_ratio_idc - 1][0];
				  height = pDecGlobal->height*aspectInfo[info->aspect_ratio_idc - 1][1];
			  }
			  else if( info->aspect_ratio_idc == 255 )
			  {
				  width = pDecGlobal->width * info->sar_width;
				  height = pDecGlobal->height * info->sar_height;
			  }    

			  if(height > 0)
				  ratio = width*10/height;

			  *((VO_IV_ASPECT_RATIO*)pData) = 0;
			  if( ratio == 0 )
				  *((VO_IV_ASPECT_RATIO*)pData) = 0;
			  else if( ratio <= 10 )
				  *((VO_IV_ASPECT_RATIO*)pData) = 1;
			  else if( ratio <= 42/3 )
				  *((VO_IV_ASPECT_RATIO*)pData) = 2;
			  else if( ratio <= 168 / 9 )
				  *((VO_IV_ASPECT_RATIO*)pData) = 3;
			  else if( ratio <= 21)
				  *((VO_IV_ASPECT_RATIO*)pData) = 4;
              else
              {
                int m = vo_gcd(width, height);
                if(m)
				{
                  width = width/m;
                  height = height/m;
                }
              }
			  if(width > 0x7fff || height > 0x7fff)
              {
                width = (width + 512) >> 10;
                height = (height + 512) >> 10;
              }

              *((VO_IV_ASPECT_RATIO*)pData) = (width << 16) | height;
          }
		  break;
#else
	  {
		*((VO_IV_ASPECT_RATIO*)pData)=VO_RATIO_11;
		
		if(pDecGlobal->pCurSlice->active_sps)
		{
			vui_seq_parameters_t *  vui=&pDecGlobal->pCurSlice->active_sps->vui_seq_parameters;
			if(vui)
			{
#define MAX_ASPECT_INFO_SIZE	16
				int id=vui->aspect_ratio_idc;
				float ratio = 0.0f;
				int width,height;
				int ratio_w, ratio_h;
				width = pDecGlobal->width;
				height = pDecGlobal->height;
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
					*((VO_IV_ASPECT_RATIO*)pData)=0;
				else if (ratio<12)
					*((VO_IV_ASPECT_RATIO*)pData)=VO_RATIO_11;
				else if (ratio<15)
					*((VO_IV_ASPECT_RATIO*)pData)=VO_RATIO_43;
				else if (ratio<20)
					*((VO_IV_ASPECT_RATIO*)pData)=VO_RATIO_169;
				else 
					*((VO_IV_ASPECT_RATIO*)pData)=VO_RATIO_21;
#else
                if (ratio_w == ratio_h)
                {
	                *((VO_IV_ASPECT_RATIO *)pData) = VO_RATIO_11;
                }
                else if (ratio_w*3 == ratio_h*4)
                {
	                *((VO_IV_ASPECT_RATIO *)pData) = VO_RATIO_43;
                }
                else if (ratio_w*9 == ratio_h*16)
                {
	                *((VO_IV_ASPECT_RATIO *)pData) = VO_RATIO_169;
                }
                else if (ratio_w == ratio_h*2)
                {
	                *((VO_IV_ASPECT_RATIO *)pData) = VO_RATIO_21;
                }
                else
                {
	                *((VO_IV_ASPECT_RATIO *)pData) = (ratio_w<<16)|ratio_h;
                }

#endif
			}
			
		}
		break;
	  }
#endif
	  default:
		return VORC_COM_WRONG_PARAM_ID;
	}
  }
  return VORC_OK;
}

VO_U32	VO_API  voH264DecGetOutputData (VO_HANDLE hDec, VO_VIDEO_BUFFER * pOutput, VO_VIDEO_OUTPUTINFO * pOutInfo)
{
  VO_U32 returnCode=0;
  DecoderParams *pDecoder = hDec;
  H264DEC_G *pDecGlobal;
  NALU_t *nalu;
  VOLOGI("getoutputdata\r\n");
  if (pOutInfo == NULL || pOutput == NULL)
  {
	  return VO_ERR_INVALID_ARG;
  }
  pOutput->Buffer[0] = pOutput->Buffer[1] = pOutput->Buffer[2] = NULL;
  if (pDecoder == NULL)
  {
	  return VO_ERR_INVALID_ARG;
  }
  pDecGlobal = pDecoder->pDecGlobal;
  if (pDecGlobal == NULL)
  {
	  return VO_ERR_WRONG_STATUS;
  }
  nalu = pDecGlobal->nalu;
#if COMBINE_FRAME
  pOutInfo->InputUsed = pDecoder->usedBytes;
  pDecoder->usedBytes = 0;

  if (pDecoder->bHasNext)
  {
#if USE_FRAME_THREAD
	  if (pDecGlobal->nThdNum > 1 /*&& !pDecGlobal->interlace*/)
	  {
		  returnCode = DecodeFrameMain(pDecGlobal,pOutput, pOutInfo, 0);
	  }
	  else
#endif
	  {
		  GetOutputPic(pDecGlobal, pOutput, pOutInfo);
	  }
	  pDecoder->bHasNext = (pOutput->Buffer[0] != NULL) && pOutInfo->Flag;
	
	  pOutInfo->Flag = pOutInfo->Flag || pDecoder->new_input > 0;
	  VOLOGI("bHasNext GetOutput!!!! buf %p return %d flag %x\n", pOutput->Buffer[0], returnCode, pOutInfo->Flag);
	  return VO_ERR_NONE;
  }
  if (pDecoder->new_input > 0)
  {
startNewInput:
	  nalu->buf = pDecoder->curPos;
	  nalu->len = pDecoder->frameLen[0];
	  nalu->Time = pDecoder->Time[0];
	  nalu->UserData = pDecoder->UserData[0];
  }
#endif
  pOutInfo->Flag = 0;
  //printf("new_input:%d\r\n",pDecoder->new_input);

  if (pDecoder->new_input)
  {
	  //LOGI("first:%d\r\n",pDecGlobal->first_sps);
	  if(!(pDecGlobal->first_sps&1))
	  {
		VO_S32 iRet = 0;
		VO_U8* inBuffer= nalu->buf;
		VO_U32 BufferLen = nalu->len; 
		VO_U8 * nextFrame,*currFrame;
		VO_S32 leftSize=BufferLen,inSize;

		if (BufferLen <=4)
		{
#if COMBINE_FRAME
			returnCode = (VO_ERR_INPUT_BUFFER_SMALL);
			goto endNewInput;
#else
			VOH264ERROR(VO_ERR_INPUT_BUFFER_SMALL)
#endif
		}
		nextFrame = currFrame = inBuffer;
		currFrame = GetNextNalu_sps(currFrame,leftSize);
		if (currFrame!=NULL)
		{
		  leftSize-=(currFrame-inBuffer);
		}
		while (leftSize>=3&&currFrame!=NULL)
		{
		  nextFrame = GetNextNalu_sps(currFrame,leftSize);
		  if(nextFrame)
		  {
			inSize = nextFrame-currFrame;
		  }
		  else
		  {
			inSize = leftSize;
		  }
		  if(inSize >=2)
		  {
			nalu->buf = currFrame;
			nalu->len = inSize;

			nalu->forbidden_bit     = (*(nalu->buf) >> 7) & 1;
			nalu->nal_reference_idc = (NalRefIdc) ((*(nalu->buf) >> 5) & 3);
			nalu->nal_unit_type     = (NaluType) ((*(nalu->buf)) & 0x1f);
			if(nalu->nal_unit_type==NALU_TYPE_SPS)
			{
			  iRet = process_first_sps(pDecGlobal);
			  pDecGlobal->first_sps |= !iRet;
			  break;
			}
		  }
		  leftSize-=inSize;
		  currFrame=nextFrame;
		}
		if(!pDecGlobal->first_sps)
		{
#if COMBINE_FRAME
			returnCode = (VO_H264_ERR_SPSIsNULL);
			goto endNewInput;
#else
			VOH264ERROR(VO_H264_ERR_SPSIsNULL)
#endif
		}
		nalu->buf = inBuffer;
		nalu->len = BufferLen;
	  }
  }

  if (pDecGlobal->first_sps == 1)
  {
	  if(pDecGlobal->nThdNum > 1)
	  {
		  VO_U32 maxThdNum = /*pDecGlobal->interlace?MAX_THREAD_NUM : */MAX_THREAD_NUM2;
		  if(pDecGlobal->nThdNum>maxThdNum)
			  pDecGlobal->nThdNum = maxThdNum;

		  //if(pDecGlobal->interlace)
		  //	  InitH264Threads(pDecGlobal);
		  //else
			  InitH264Threads2(pDecGlobal);
		  pDecGlobal->thdinit = 1;
	  }
	  pDecGlobal->first_sps |= 2;
  }

  //if (pDecGlobal->licenseCheck->lastVideo)
  //{
	//  pDecGlobal->licenseCheck->resetVideo(pDecGlobal->licenseCheck->hCheck,NULL);
	//  pDecGlobal->licenseCheck->lastVideo=NULL;
  //}
  
#if USE_FRAME_THREAD
  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
  {
	  if(pDecoder->new_input)
	  {
#if COMBINE_FRAME
#else
		  pOutInfo->InputUsed = pDecGlobal->nalu->len;
#endif
		  if (pDecoder->FileFormat == VOH264_AVC)
		  {
#if COMBINE_FRAME
			  returnCode = (VO_H264_ERR_AVC_NULL);
			  goto endNewInput;
#else
			  VOH264ERROR(VO_H264_ERR_AVC_NULL)
#endif
		  }
	  }
	  if (pDecGlobal->first_sps == 3)
	  {
		  returnCode = DecodeFrameMain(pDecGlobal,pOutput, pOutInfo, pDecoder->new_input > 0);
	  }
  }
  else
#endif
  {
	  if(pDecoder->new_input)
	  {
		//printf("fileformat:%d len:%d\r\n",pDecoder->p_Inp->FileFormat,pDecGlobal->nalu->len);
	    
#if COMBINE_FRAME
#else
		  pOutInfo->InputUsed = pDecGlobal->nalu->len;
#endif
		  switch (pDecoder->FileFormat)
		  {
		  case VOH264_AVC:
#if COMBINE_FRAME
			  returnCode = (VO_H264_ERR_AVC_NULL);
			  goto endNewInput;
#else
			  VOH264ERROR(VO_H264_ERR_AVC_NULL)
#endif
			  break;
		  case VOH264_14496_15:
			  returnCode = voH264DecProcessNALU(hDec);
			  break;
		  default:
			  returnCode = voH264DecProcessAnnexB(hDec);
			  break;
		  }
	  }
	  GetOutputPic(pDecGlobal, pOutput, pOutInfo);
  }
#if COMBINE_FRAME
endNewInput:
  if (pDecoder->new_input > 0)
  {
	  VO_U32 i;
	  pDecoder->curPos += pDecoder->frameLen[0];
	  pDecoder->leftBytes -= pDecoder->frameLen[0];
	  pDecoder->new_input--;
	  for (i = 0; i < pDecoder->new_input; ++i)
	  {
		  pDecoder->frameLen[i] = pDecoder->frameLen[i+1];
		  pDecoder->Time[i] = pDecoder->Time[i+1];
		  pDecoder->UserData[i] = pDecoder->UserData[i+1];
	  }
	  pDecoder->Time[i] = pDecoder->Time[i+1];
	  pDecoder->UserData[i] = pDecoder->UserData[i+1];
  }
  if (pDecoder->new_input > 0 && (pOutput->Buffer[0] == NULL || returnCode != 0))
  {  
	  //no output, but there is new input
	  goto startNewInput;
  }
  pDecoder->bHasNext = (pOutput->Buffer[0] != NULL) && pOutInfo->Flag;
  pOutInfo->Flag = (pDecoder->new_input<<16) | (pOutInfo->Flag<<16>>16);

  if (!pDecoder->bHasNext && pDecoder->bNeedFlush)
  {
	  if (pDecoder->new_input == 0)
	  {
		  if (pDecGlobal->first_sps != 3)
		  {
			  return VO_ERR_WRONG_STATUS;
		  }
#if USE_FRAME_THREAD
		  if (pDecGlobal->nThdNum > 1/*&&!pDecGlobal->interlace*/)
		  {
			  DecodeFrameMain(pDecGlobal, NULL, NULL, 2);
		  }
		  else
#endif
		  {
			  flush_dpb(pDecGlobal,pDecGlobal->p_Dpb);
		  }
	  }
	  pOutInfo->Flag = 1;
	  pDecoder->bNeedFlush = 0;
  }
#else
  pDecoder->new_input = 0;
#endif
  pOutInfo->Flag = !!(pOutInfo->Flag);

VOLOGI("GetOutput!!!! buf %p return %d flag %x\n", pOutput->Buffer[0], returnCode, pOutInfo->Flag);
  if (pOutput->Buffer[0] == NULL && returnCode==0 && pOutInfo->Flag == 0)
	  return VO_ERR_INPUT_BUFFER_SMALL;

  if(returnCode == 0 && pDecoder->pDecGlobal->bThumbnailMode == 1)
	  pDecoder->pDecGlobal->bThumbnailMode = 0;

  return returnCode;
}

VO_U32	VO_API  voH264DecUninit(VO_HANDLE hDec)
{
  VO_U32 returnCode=0;
  VOLOGI("start uninit\r\n");
  returnCode = FinitDecoder(hDec);
  
  returnCode = CloseDecoder(hDec);
  VOLOGI("finish uninit\r\n");
  VOLOGUNINIT();
  return returnCode;
}

VO_S32 VO_API voGetH264DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
  if(pDecHandle==NULL)
	return VO_ERR_INVALID_ARG;
  pDecHandle->Init			= voH264DecInit;
  pDecHandle->GetOutputData	= voH264DecGetOutputData;
  pDecHandle->SetInputData	= voH264DecSetInputData;
  pDecHandle->GetParam		= voH264DecGetParam;
  pDecHandle->SetParam		= voH264DecSetParam;
  pDecHandle->Uninit		= voH264DecUninit;
  return VO_ERR_NONE;
}

