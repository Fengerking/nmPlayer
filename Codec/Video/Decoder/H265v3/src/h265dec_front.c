 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_front.c
    \brief    front decoder functions
    \author   Renjie Yu
	\change
*/
#include "h265_decoder.h"
#include "h265dec_mem.h"
#include "h265dec_list.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "voLog.h"


#if ENABLE_LICENSE_CHECK
#include "voCheck.h"
#elif ENABLE_SIMPLE_LC
VO_S32 g_h265dec_limited_num = 30*60*60;
#endif

VO_HANDLE g_h265dec_inst = NULL;


#define CHECK_API_ERR(result) (result & (VO_ERR_BASE))
VO_U32 VO_API H265DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * p_out_vbuf, VO_VIDEO_OUTPUTINFO * p_out_vbuf_info);
// #if USE_CODEC_NEW_API
// VO_U32 VO_API H265DecGetOutputData_new(VO_HANDLE hDec, VO_VIDEO_BUFFER * p_out_vbuf, VO_VIDEO_OUTPUTINFO * p_out_vbuf_info);
// #endif
extern VO_VOID xWriteOutput(H265_DEC * const p_dec, H265_DPB_LIST* pcListPic, VO_S32 reorder_num );

#if !USE_CODEC_NEW_API

/**
* Initialize a H265 decoder instance use default settings.
* \param phCodec [out] Return the H265 Decoder handle.
* \param pQueryMem [in] Input memory allocated outside.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_OUT_OF_MEMORY Out of memory.
*/
VO_U32 VO_API H265DecInit( VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * p_user_op)
{
	H265_DEC_FRONT* p_h265dec_front = NULL;
	VO_U32 ret						= VO_ERR_NONE;
	VO_S32 codec_id;
#if ENABLE_LICENSE_CHECK	
	VO_PTR p_lic_handle				= NULL;
	VO_LIB_OPERATOR *p_lib_op		= NULL;
#endif

  if(p_user_op!=NULL){
    VOLOGINIT((VO_CHAR *)p_user_op->reserved1);
  }
  

  VOLOGI(" ===> ");

	if ( vType != VO_VIDEO_CodingH265 ) {
    VOLOGE(" <=== !!!VO_ERR_NOT_IMPLEMENT");
		return VO_ERR_NOT_IMPLEMENT;
	} else {
		codec_id = VO_INDEX_DEC_H265;
	}


#if ENABLE_LICENSE_CHECK
	if ( p_user_op ) {
		if ( p_user_op->memflag & 0XF0 ) {//valid libOperator flag
			p_lib_op = p_user_op->libOperator;
		}
	}

  VOLOGI("calls voCheckLibInit ");
	ret = voCheckLibInit( &p_lic_handle, codec_id, p_user_op ? p_user_op->memflag : 0, g_h265dec_inst, p_lib_op );
  VOLOGI("returns voCheckLibInit ");
  
    if ( ret != VO_ERR_NONE ) {
        voCheckLibUninit( p_lic_handle );
        p_lic_handle = NULL;
        VOLOGE("H265DecInit <=== !!!%d", ret);
        return ret;
    }
#endif

	if ( p_user_op ) {
		if ( ( !( p_user_op->memflag&0xF ) ) || ( p_user_op->memData == NULL ) ) {
			p_user_op = NULL;
		}
	}
	p_h265dec_front = ( H265_DEC_FRONT* )AlignMalloc( p_user_op, codec_id, sizeof( H265_DEC_FRONT ), CACHE_LINE );
  if( !p_h265dec_front ){
    VOLOGE(" <=== !!!VO_ERR_OUTOF_MEMORY");
		return VO_ERR_OUTOF_MEMORY;
  }

	SetMem( p_user_op, codec_id, ( VO_U8 * )p_h265dec_front, 0, sizeof( H265_DEC_FRONT ) );

	//if (p_h265dec_front->input_mode == STRAM_MODE) 
	{//
		//Malloc internal raw data buffer
		p_h265dec_front->p_input_buf = (VO_U8*)AlignMalloc(p_user_op, codec_id, RAW_DATA_LEN, CACHE_LINE);
    if(!p_h265dec_front->p_input_buf){
      VOLOGE(" <=== !!!VO_ERR_OUTOF_MEMORY");
      return VO_ERR_OUTOF_MEMORY;
    }

		SetMem(p_user_op, codec_id, p_h265dec_front->p_input_buf, 0, RAW_DATA_LEN);
		p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
		p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
		p_h265dec_front->left_buf_len = 0;
		p_h265dec_front->used_buf_len = 0;
		p_h265dec_front->bFindFirstSlice = 0;
		p_h265dec_front->more_dis_pics = 0;
		p_h265dec_front->bNeedFlush = 0;	
	}

	// Create video decoder engine
	ret = CreateH265Decoder( &p_h265dec_front->p_dec, p_user_op, codec_id );
	if( ret != VO_ERR_NONE ){
		AlignFree( p_user_op, codec_id, p_h265dec_front );
    VOLOGE(" <=== !!!%d", ret);
		return ret;
	}

	p_h265dec_front->p_user_op = p_user_op;
	p_h265dec_front->codec_id = codec_id;

#if ENABLE_LICENSE_CHECK	
	p_h265dec_front->p_lic_handle = p_lic_handle; 
#endif

#if H265DEC_DEBUG
  //((H265_DEC *)p_h265dec_front->p_dec)->start_time = clock();
	IniDebug();
#endif

	*phDec = p_h265dec_front;
  VOLOGI(" <=== ...VO_ERR_NONE");
	return VO_ERR_NONE;
}


/**
* Close the H265 decoder instance, release any resource it used.
* \param hCodec [in] H265 decoder instance handle, returned by H265DecInit().
* \return The function should always return VORC_OK.
*/
VO_U32 VO_API H265DecUinit(VO_HANDLE hCodec)
{
  
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hCodec;
	H265_DEC *p_dec = (H265_DEC *)p_h265dec_front->p_dec;
    //long start_time = ((H265_DEC *)p_h265dec_front->p_dec)->start_time;
  VOLOGI(" ===> ");

  if(p_h265dec_front == NULL){
    VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
  }

#if ENABLE_LICENSE_CHECK
	voCheckLibUninit (p_h265dec_front->p_lic_handle);
#endif	

#if USE_3D_WAVE_THREAD
	if (p_dec->nThdNum > 1)
	{
		ReleaseH265Threads_wave3D(p_dec);
		p_dec->nThdNum = 0;
	}

#endif
	//delete decoder engine
	DeleteH265Decoder(p_h265dec_front->p_dec);

	//if (p_h265dec_front->input_mode == STRAM_MODE) 
	{
		//Free raw data buffer
		AlignFree(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_h265dec_front->p_input_buf);
	}

	//Free front handle
	AlignFree(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_h265dec_front);

#if H265DEC_DEBUG
{
  //long decode_time  = clock() - start_time;
  //VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STAT_FINFO,"Decode Time(MilliSec)        : %d\n", decode_time ); 

  UiniDebug();
}
#endif
  VOLOGI(" <=== ...VO_ERR_NONE");
  VOLOGUNINIT();
	return VO_ERR_NONE;
}


/**
* Set parameter of the decoder instance.
* \param hCodec [in] H265 decoder instance handle, returned by H265DecInit().
* \param nID [in] Parameter ID, Supports: VOID_COM_HEAD_DATA,VOID_MPEG4_GETFIRSTFRAME
* \param lValue [in] Parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API H265DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR lValue)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hCodec;
	H265_DEC *p_dec;
	//VO_VIDEO_BUFFER out_vbuf;
	//VO_VIDEO_OUTPUTINFO out_vbuf_info;
	//VO_CODECBUFFER in_putbuf;

	VO_S32 ret = VO_ERR_NONE;
  VOLOGI(" ===> ");

	if(p_h265dec_front == NULL)
		return VO_ERR_INVALID_ARG;

	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

  VOLOGI(" nID = 0x%x", nID);

	switch(nID) {
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER *p_codec_buf = (VO_CODECBUFFER *)lValue;
			VO_U8* p_copy;

			if (p_codec_buf->Length == 0)
			{
        VOLOGE(" <=== !!!VO_ERR_INPUT_BUFFER_SMALL");
				return VO_ERR_INPUT_BUFFER_SMALL;
			}

			p_copy = (VO_U8*)AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8) * p_codec_buf->Length, CACHE_LINE);
			if (!p_copy)
			{
        VOLOGE(" <=== !!!VO_ERR_OUTOF_MEMORY");
				return VO_ERR_OUTOF_MEMORY;
			}

			CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_copy, p_codec_buf->Buffer, p_codec_buf->Length);
			ret = DecodeRawVideo((H265_DEC*)p_h265dec_front->p_dec, p_copy, p_codec_buf->Length);
			if (0)
			{
				//put header data just after input buffer
				VO_U8 *inBuf = p_h265dec_front->p_input_buf;
				const VO_U8 *curPos = p_h265dec_front->p_cur_buf_pos;
				const VO_U8 *lastPos = p_h265dec_front->lastPos;
				VO_U8 *endPos = p_h265dec_front->p_cur_buf_pos + p_h265dec_front->left_buf_len;
				assert(endPos - inBuf <= RAW_DATA_LEN);

				if (p_codec_buf->Length > RAW_DATA_LEN/2)
				{
					return VO_ERR_INVALID_ARG;
				}

				if (p_h265dec_front->left_buf_len + p_codec_buf->Length >= RAW_DATA_LEN)
				{
					if (p_h265dec_front->new_input == 0 && p_h265dec_front->left_buf_len >= RAW_DATA_LEN/2)
					{
						curPos = p_h265dec_front->p_cur_buf_pos = inBuf;
						lastPos = p_h265dec_front->lastPos = inBuf;
						endPos = inBuf;
						p_h265dec_front->left_buf_len = 0;
						p_h265dec_front->bFindFirstSlice = 0;
					}
					else
					{
						return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
					}
				}

				if (endPos + p_codec_buf->Length >= inBuf + RAW_DATA_LEN)
				{
					//copy left data to the head of memory pool
					memmove(inBuf, curPos, p_h265dec_front->left_buf_len);
					p_h265dec_front->lastPos = lastPos -= (curPos - inBuf); 
					p_h265dec_front->p_cur_buf_pos = inBuf;
					curPos = inBuf;
					endPos = inBuf + p_h265dec_front->left_buf_len;
				}

				if (p_h265dec_front->new_input >= MAX_FRAME_INPUT)
				{
					return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
				}
				CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, endPos, p_codec_buf->Buffer, p_codec_buf->Length);
			}
			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id,p_copy);
		}
		break;
	case VO_PID_COMMON_FLUSH:
		{
			//Add for clean buffer
			VO_VIDEO_BUFFER outData;
			VO_VIDEO_OUTPUTINFO outFormat;

#if USE_3D_WAVE_THREAD
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain_wave3D(p_dec, NULL, NULL, 2);
			}
			else
#endif
			{
				xWriteOutput(p_dec, &p_dec->dpb_list, 0);
			}
			p_h265dec_front->new_input = 0;

			//LOGI("pDecoder->bFindFirstSlice:%d,pDecoder->new_input:%d,pDecoder->bHasNext:%d\r\n",pDecoder->bFindFirstSlice,pDecoder->new_input,pDecoder->bHasNext);
			ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
			while(!ret && (outFormat.Format.Type != VO_VIDEO_FRAME_NULL || outFormat.Flag)) //in the end of file, we must get all next output
			{
				ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
			}
			//LOGI("flush poutput:%d returnCode:%x\r\n",pDecGlobal->output_size,returnCode);
			//reset input buffer
			p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
			p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
			p_h265dec_front->left_buf_len = 0;
			p_h265dec_front->used_buf_len = 0;
			p_h265dec_front->bFindFirstSlice = 0;
			p_h265dec_front->new_input = 0;
			p_h265dec_front->more_dis_pics = 0;
		}
		break;
	case VO_PID_DEC_H265_FLUSH_PICS:
		{
			if (p_h265dec_front->bFindFirstSlice)
			{
				p_h265dec_front->frameLen[p_h265dec_front->new_input] = p_h265dec_front->left_buf_len;
				// printf("last combine %d\n", pDecoder->leftBytes);
				p_h265dec_front->new_input++;
				p_h265dec_front->left_buf_len = 0;
				p_h265dec_front->bFindFirstSlice = 0;
			}
			else if (p_h265dec_front->new_input == 0)
			{
				//reset input buffer
				p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
				p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
				p_h265dec_front->left_buf_len = 0;
				p_h265dec_front->used_buf_len = 0;
			}

			if (p_h265dec_front->more_dis_pics)
			{
				p_h265dec_front->bNeedFlush = 1;	//need call GetOutputData before flush, flush will delay
				break;
			}

			if(p_h265dec_front->new_input > 0)
			{
#if USE_3D_WAVE_THREAD
				p_dec->in_data = p_h265dec_front->p_cur_buf_pos;
				p_dec->data_sz = p_h265dec_front->frameLen[0];
				p_dec->timestamp = p_h265dec_front->Time[0];

				if (p_dec->nThdNum > 1)
				{
					ret = DecodeFrameMain_wave3D(p_dec,NULL, NULL, 3);
				}
				else
#else
				p_dec->slice.timestamp = p_h265dec_front->Time[0];
#endif
				{
					ret = DecodeRawVideo(p_dec, p_h265dec_front->p_cur_buf_pos, p_h265dec_front->frameLen[0]);
				}

				{
					VO_U32 i;
					p_h265dec_front->p_cur_buf_pos += p_h265dec_front->frameLen[0];
					p_h265dec_front->left_buf_len -= p_h265dec_front->frameLen[0];
					p_h265dec_front->new_input--;
					for (i = 0; i < p_h265dec_front->new_input; ++i)
					{
						p_h265dec_front->frameLen[i] = p_h265dec_front->frameLen[i+1];
#if ENABLE_DROP_FRAME
						p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
#endif
						p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
					}
#if ENABLE_DROP_FRAME
					p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
#endif
					p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
				}
			}

			if (p_h265dec_front->new_input > 0)
			{
				p_h265dec_front->bNeedFlush = 1;	//need call GetOutputData before flush, flush will delay
				break;
			}
#if USE_3D_WAVE_THREAD
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain_wave3D(p_dec, NULL, NULL, 2);
			}
			else
#endif
			{
				xWriteOutput(p_dec, &p_dec->dpb_list, 0);
			}
		}
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		p_dec->user_op_all.p_pic_buf_op = (VO_MEM_VIDEO_OPERATOR *)lValue;
		break;
	case VO_PID_COMMON_HeadInfo:
		p_dec->frame_num = 0;
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		p_dec->out_mode = *((VO_S32 *)lValue);
		break;
	case VO_PID_COMMON_CPUNUM:
		{
			VO_S32 nCPU = *((VO_S32*)lValue);
			VO_S32 maxThdNum;
#if USE_3D_WAVE_THREAD
			maxThdNum = MAX_THREAD_NUM2;
#else
			maxThdNum = 1;
#endif
			if (nCPU <= 0)
			{
				return VO_ERR_INVALID_ARG;
			}
			if (nCPU > maxThdNum)
			{
				nCPU = maxThdNum;
				*((VO_S32*)lValue) = nCPU; //put back the value really set
			}

			if (nCPU != p_dec->nThdNum)
			{
#if USE_3D_WAVE_THREAD
				if (p_dec->nThdNum > 1)
				{
					ReleaseH265Threads_wave3D(p_dec);
				}
				p_dec->nThdNum = nCPU;
				if (p_dec->nThdNum > 1)
				{
					if (1 + nCPU + p_dec->dpb_list.node_num > MAX_DPB_SIZE)
                      return VO_ERR_WRONG_PARAM_ID;
					p_dec->dpb_list.node_num += nCPU + 1;
					InitH265Threads_wave3D(p_dec);
				}
#endif
			}
		}
		break;
	case VO_PID_COMMON_FRAME_BUF_EX:
		if (*((VO_U32*)lValue) > (MAX_DPB_SIZE  - p_dec->dpb_list.node_num))
			return VO_ERR_WRONG_PARAM_ID;
		p_dec->dpb_list.node_num += *((VO_U32*)lValue);
		break;
	case VO_PID_DEC_H265_FASTMODE:
		p_dec->fast_mode = *((VO_S32*)lValue);
#if USE_3D_WAVE_THREAD
		if (p_dec->nThdNum > 1)
		{
		}
		else
#endif
		{
			p_dec->slice.fast_mode = *((VO_S32*)lValue);	//single-core
		}
		break;
	case VO_PID_VIDEO_THUMBNAIL_MODE:
		{
			p_dec->thumbnail_mode = *((VO_S32*)lValue);
			break;
		}
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

/**
* Get parameter of the decoder instance.
* \param hCodec [in] H265 decoder instance handle, returned by H265DecInit().
* \param nID [in] Parameter ID, supports VOID_COM_VIDEO_FORMAT, VOID_COM_VIDEO_WIDTH, VOID_COM_VIDEO_HEIGHT
* \param plValue [out] Return the parameter value. The value may be a pointer to a struct according to the parameter ID.
* \retval VORC_OK Succeeded.
* \retval VORC_COM_WRONG_PARAM_ID No such parameter ID supported.
*/
VO_U32 VO_API H265DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{

	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hCodec;
	H265_DEC *p_dec;
	//VO_U32 ret = VO_ERR_NONE;

	if(p_h265dec_front == NULL)
		return VO_ERR_INVALID_ARG;

	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

	switch(nID) {
	case VO_PID_VIDEO_FRAMETYPE: //TBD
		{
#if 0
			VO_VIDEO_OUTPUTINFO vOutPutInfo;
			VO_CODECBUFFER* p_codec_buf = (VO_CODECBUFFER *)plValue;

			ret = voMPEGDecFrame(p_h265dec_front->p_dec, p_codec_buf, NULL, &vOutPutInfo, DEC_FRAMETYPE);
			if(CHECK_API_ERR(ret))
				return ret;

			p_codec_buf->Time = (VO_S64)vOutPutInfo.Format.Type;
#endif
		}
		break;
	case VO_PID_VIDEO_ASPECTRATIO:
		*((VO_S32 *)plValue) = ( VO_S32 )p_dec->sps[ 0 ].aspect_ratio;//YU_TBD, temp solution, aspect ration should get from display pic related sps
		break;
	case VO_PID_DEC_H265_FASTMODE:
		*((VO_U32*)plValue) = p_dec->fast_mode;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;

}

VO_U32 VO_API H265DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * p_codec_buf)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hDec;

	if (!p_h265dec_front) {
		return VO_ERR_INVALID_ARG;
	}
LOGI("SetInput length %d, timestamp %lld", p_codec_buf->Length, p_codec_buf->Time);

	if (p_codec_buf->Length > RAW_DATA_LEN/2)
	{
		return VO_ERR_INVALID_ARG;
	}

	if (p_codec_buf->Length == 0)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	{
		VO_U8 *inBuf = p_h265dec_front->p_input_buf;
		const VO_U8 *curPos = p_h265dec_front->p_cur_buf_pos;
		const VO_U8 *lastPos = p_h265dec_front->lastPos;
		VO_U8 *endPos = p_h265dec_front->p_cur_buf_pos + p_h265dec_front->left_buf_len;
		assert(endPos - inBuf <= RAW_DATA_LEN);

		if (p_h265dec_front->left_buf_len + p_codec_buf->Length >= RAW_DATA_LEN)
		{
			if (p_h265dec_front->new_input == 0 && p_h265dec_front->left_buf_len >= RAW_DATA_LEN/2)
			{
				curPos = p_h265dec_front->p_cur_buf_pos = inBuf;
				lastPos = p_h265dec_front->lastPos = inBuf;
				endPos = inBuf;
				p_h265dec_front->left_buf_len = 0;
				p_h265dec_front->bFindFirstSlice = 0;
			}
			else
			{
				return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
			}
		}

		if (endPos + p_codec_buf->Length >= inBuf + RAW_DATA_LEN)
		{
			//copy left data to the head of memory pool
			memmove(inBuf, curPos, p_h265dec_front->left_buf_len);
			p_h265dec_front->lastPos = lastPos -= (curPos - inBuf); 
			p_h265dec_front->p_cur_buf_pos = inBuf;
			curPos = inBuf;
			endPos = inBuf + p_h265dec_front->left_buf_len;
		}

		if (p_h265dec_front->new_input >= MAX_FRAME_INPUT)
		{
			return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
		}
		CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, endPos, p_codec_buf->Buffer, p_codec_buf->Length);

		{
			//Combine whole frame
			const VO_U8 *last = lastPos;		//last frame end
			const VO_U8 *currFrame = endPos;
			const VO_U8 *nextFrame = endPos;
			VO_U32 leftSize = p_codec_buf->Length;
			VO_U32 inSize;

			//skip until first nalu appear
			while (currFrame < endPos + leftSize - 2)
			{
				if (currFrame[2] > 0x02)//check [2]
				{
					currFrame += 3;
					continue;
				}
				else if (*currFrame==0 && *(currFrame+1)==0 && *(currFrame+2))//check [0] [1] [2]
				{
					break;
				}
				++currFrame;
			}
			if (currFrame < endPos + leftSize - 2)
			{
				leftSize -= currFrame - endPos;
			}
			else
			{
				leftSize = 0;
			}

			while (leftSize>0&&currFrame!=NULL&&p_h265dec_front->new_input < MAX_FRAME_INPUT)
			{
				{
					//1. find next sc
					const VO_U8 *p = currFrame+3;
					const VO_U8* endPos = currFrame+leftSize-2;

					while (p < endPos)
					{
						if (p[2] > 0x02)//check [2]
						{
							p += 3;
							continue;
						}
						else if (*p==0 && *(p+1)==0 && *(p+2))//check [0] [1] [2]
						{
							break;
						}
						++p;
					}
					if (p<endPos)
					{
						inSize = p-currFrame;
						nextFrame = p;
					}
					else
					{
						inSize = leftSize;
						nextFrame = NULL;
					}
				}

				if(inSize >= 6)// at least 3 byte sc + nalu header(2 bytes) + 1 byte in slice header
				{
					const VO_U8 *tmp_ptr = currFrame;
					VO_U32 type;
					VO_U32 value;

					//skip sc
					while(!*tmp_ptr++);

					//2.next nalu type
					type = ((*tmp_ptr)>>1) & 0x3f;
					if (type > NAL_UNIT_CODED_SLICE_CRA) //include reserved types
					{
						//current nalu is not a slice
						if (p_h265dec_front->bFindFirstSlice)
						{
							//find next pic start
							p_h265dec_front->bFindFirstSlice = 0;	//reset
							p_h265dec_front->frameLen[p_h265dec_front->new_input] = currFrame - last;
							last = currFrame;
							p_h265dec_front->new_input++;
						}
						leftSize-=inSize;
						currFrame=nextFrame;
						continue;	//not a slice, continue finding next nalu					
					}

					//3.is first slice or not
					value = tmp_ptr[2]&0x80; //first_slice_in_pic_flag
					if (value)
					{
						if (p_h265dec_front->bFindFirstSlice)
						{
							//find next pic start
							p_h265dec_front->frameLen[p_h265dec_front->new_input] = currFrame - last;
							// printf("combine %d\n", currFrame - last);
							last = currFrame;
							p_h265dec_front->new_input++;

						}
#if ENABLE_DROP_FRAME
						// printf("type %d\n", type);
						if ((((H265_DEC *)(p_h265dec_front->p_dec))->fast_mode & VO_FM_DROP_UNREF) && type == NAL_UNIT_CODED_SLICE_TRAIL_N)
						{
							p_h265dec_front->bCanDrop[p_h265dec_front->new_input] = 1;
						}
						else
						{
							p_h265dec_front->bCanDrop[p_h265dec_front->new_input] = 0;
						}
#endif
						p_h265dec_front->Time[p_h265dec_front->new_input] = p_codec_buf->Time;
						p_h265dec_front->bFindFirstSlice = 1;	//next is first slice of a pic
					}

				}
				leftSize-=inSize;
				currFrame=nextFrame;
			}
			p_h265dec_front->lastPos = last;
			p_h265dec_front->left_buf_len += p_codec_buf->Length - leftSize;
			p_h265dec_front->used_buf_len = p_codec_buf->Length - leftSize;

		}
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * p_out_vbuf, VO_VIDEO_OUTPUTINFO * p_out_vbuf_info)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hDec;
	VO_S32 ret = VO_ERR_NONE;
	H265_DEC *p_dec;

	if (!p_h265dec_front || !p_out_vbuf || !p_out_vbuf_info) {
		return VO_ERR_INVALID_ARG;
	}
	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

//#if ENABLE_LICENSE_CHECK
	//voCheckLibResetVideo(p_h265dec_front->p_lic_handle, p_h265dec_front->p_out_vbuf);
#if ENABLE_SIMPLE_LC
	if(--g_h265dec_limited_num<0)
		return VO_ERR_LICENSE_ERROR;
#endif


	p_out_vbuf_info->InputUsed = p_h265dec_front->used_buf_len;
	// 	p_h265dec_front->used_buf_len = 0;

	if (p_h265dec_front->more_dis_pics || p_dec->bOutputFull)
	{
#if USE_3D_WAVE_THREAD
		if (p_dec->nThdNum > 1)
		{
			ret = DecodeFrameMain_wave3D(p_dec, p_out_vbuf, p_out_vbuf_info, 0);
		}
		else
#endif
		{
			GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
		}
		p_h265dec_front->more_dis_pics = (p_out_vbuf_info->Format.Type != VO_VIDEO_FRAME_NULL) && p_out_vbuf_info->Flag; //Harry: use type or buffer[0]?

		p_out_vbuf_info->Flag = p_out_vbuf_info->Flag || p_h265dec_front->new_input > 0;
		// LOGI("GetOutput!!!! buf %p return %d flag %x timestamp %lld\n", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
		return VO_ERR_NONE;
	}

	p_out_vbuf_info->Flag = 0;
	if (p_h265dec_front->new_input > 0)
	{
startNewInput:
#if USE_3D_WAVE_THREAD
#if ENABLE_DROP_FRAME
		if (p_dec->fast_mode & VO_FM_DROP_UNREF) //need to drop frame
		{
			VO_U32 i;
			if (p_h265dec_front->new_input > 0 && p_h265dec_front->bCanDrop[0])
			{
				//drop this frame
				p_h265dec_front->p_cur_buf_pos += p_h265dec_front->frameLen[0];
				p_h265dec_front->left_buf_len -= p_h265dec_front->frameLen[0];
				p_h265dec_front->new_input--;
				for (i = 0; i < p_h265dec_front->new_input; ++i)
				{
					p_h265dec_front->frameLen[i] = p_h265dec_front->frameLen[i+1];
					p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
					p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
				}
				p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
				p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
				return VO_ERR_DROPPEDFRAME;
			}
		}
		if (p_h265dec_front->new_input > 0)
		{
#endif
			p_dec->in_data = p_h265dec_front->p_cur_buf_pos;
			p_dec->data_sz = p_h265dec_front->frameLen[0];
			p_dec->timestamp = p_h265dec_front->Time[0];
			// printf("input size: %d\n", p_dec->data_sz);
#if ENABLE_DROP_FRAME
		}
		// 		else
		// 		{
		// 			return VO_ERR_INPUT_BUFFER_SMALL;
		// 		}
#endif
#else
		p_dec->slice.timestamp = p_h265dec_front->Time[0];
#endif
	}
	// 	else
	// 	{
	// 		return VO_ERR_INPUT_BUFFER_SMALL;
	// 	}

#if USE_3D_WAVE_THREAD
	if (p_dec->nThdNum > 1)
	{
		// LOGI("DecodeFrameMain start");
		ret = DecodeFrameMain_wave3D(p_dec,p_out_vbuf, p_out_vbuf_info, p_h265dec_front->new_input > 0);
		// LOGI("DecodeFrameMain end");
	}
	else
#endif
	{
		if (p_h265dec_front->new_input > 0)
		{
			ret = DecodeRawVideo(p_dec, p_h265dec_front->p_cur_buf_pos, p_h265dec_front->frameLen[0]);
			// 			if (CHECK_API_ERR(ret)) {
			// 				return ret;
			// 			}
		}

		GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
	}
// endNewInput:
	if (p_h265dec_front->new_input > 0)
	{
		VO_U32 i;
		p_h265dec_front->p_cur_buf_pos += p_h265dec_front->frameLen[0];
		p_h265dec_front->left_buf_len -= p_h265dec_front->frameLen[0];
		p_h265dec_front->new_input--;
		for (i = 0; i < p_h265dec_front->new_input; ++i)
		{
			p_h265dec_front->frameLen[i] = p_h265dec_front->frameLen[i+1];
#if ENABLE_DROP_FRAME
			p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
#endif
			p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
		}
#if ENABLE_DROP_FRAME
		p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
#endif
		p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
	}
	if ((p_dec->nThdNum <= 1) && (p_dec->thumbnail_mode == 1))
	{
		// output the frame data
		p_out_vbuf_info->Flag  = 1;
		p_out_vbuf_info->Format.Width   = p_dec->sps->pic_width_in_luma_samples-p_dec->sps->conf_win_right_offset -p_dec->sps->conf_win_left_offset;//->p_out_pic->m_iPicWidth;
		p_out_vbuf_info->Format.Height  = p_dec->sps->pic_height_in_luma_samples-p_dec->sps->conf_win_bottom_offset-p_dec->sps->conf_win_top_offset;//p_out_pic->m_iPicHeight;
		p_out_vbuf_info->Format.Type    = (VO_VIDEO_FRAMETYPE)p_dec->slice.cur_pic->pic_type;
		p_out_vbuf->Buffer[0]			= p_dec->slice.cur_pic->pic_buf[ 0 ];
		p_out_vbuf->Buffer[1]			= p_dec->slice.cur_pic->pic_buf[ 1 ];
		p_out_vbuf->Buffer[2]			= p_dec->slice.cur_pic->pic_buf[ 2 ];
		p_out_vbuf->Stride[0]			= p_dec->slice.cur_pic->pic_stride[ 0 ];
		p_out_vbuf->Stride[1]			= p_dec->slice.cur_pic->pic_stride[ 1 ];
		p_out_vbuf->Stride[2]			= p_dec->slice.cur_pic->pic_stride[ 2 ];
		p_out_vbuf->Time				= p_dec->slice.cur_pic->Time;
		p_out_vbuf->ColorType           = VO_COLOR_YUV_PLANAR420;

		p_dec->thumbnail_mode = 0;
		return VO_ERR_NONE;
	} 
	if (p_h265dec_front->new_input > 0 && p_dec->bOutputFull == 0 && (p_out_vbuf_info->Format.Type == VO_VIDEO_FRAME_NULL || ret != 0))
	{  
		//no output, but there is new input
		//LOGI("startNewInput\r\n");
		goto startNewInput;
	}
	p_h265dec_front->more_dis_pics = (p_out_vbuf_info->Format.Type != VO_VIDEO_FRAME_NULL) && p_out_vbuf_info->Flag;
	p_out_vbuf_info->Flag = (p_h265dec_front->new_input<<16) | (p_out_vbuf_info->Flag<<16>>16);

	if (!p_h265dec_front->more_dis_pics && p_h265dec_front->bNeedFlush)
	{
		if (p_h265dec_front->new_input == 0)
		{
#if USE_3D_WAVE_THREAD
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain_wave3D(p_dec, NULL, NULL, 2);
			}
			else
#endif
			{
				xWriteOutput(p_dec, &p_dec->dpb_list, 0);
			}
		}
		p_out_vbuf_info->Flag = 1;
		p_h265dec_front->bNeedFlush = 0;
	}

	p_out_vbuf_info->Flag = !!(p_out_vbuf_info->Flag);

	// LOGI("GetOutput!!!! buf %p return %d flag %x timestamp %lld\n", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
	if (p_out_vbuf_info->Format.Type == VO_VIDEO_FRAME_NULL && ret==0 && p_out_vbuf_info->Flag == 0)
		return VO_ERR_INPUT_BUFFER_SMALL; //Harry: VO_ERR_INPUT_BUFFER_SMALL or VO_ERR_NONE ?


//#if ENABLE_LICENSE_CHECK
	//p_h265dec_front->p_out_vbuf = p_out_vbuf;/* backup for next voCheckLibResetVideo*/
	//if(p_out_vbuf_info->Format.Type != VO_VIDEO_FRAME_NULL)
	//	voCheckLibCheckVideo (p_h265dec_front->p_lic_handle, p_out_vbuf, &p_out_vbuf_info->Format);
//#endif


	//fprintf(outdebug, "\n out time = %d, used len = %d, type = %d, moredata = %d", (VO_S32)p_out_vbuf->Time, (VO_S32)p_out_vbuf_info->InputUsed, (VO_S32)p_out_vbuf_info->Format.Type, (VO_S32)p_out_vbuf_info->Flag);


	return ret;
}
#else


extern void * PopTaskQ(TaskQ *pQ);

static VO_VOID xUnrefAllPic(H265_DPB_LIST * const p_dpb_list )
{
	H265_LIST_NODE * node = p_dpb_list->list_node_pool;
	VO_U32 n = p_dpb_list->node_num;

	for (; n != 0; --n)
	{
		TComPic* const pic = (node++)->data;
		if (pic != NULL)
		{
			pic->m_bRefenced = VO_FALSE;
		}
	}
}

static VO_VOID xRetireRefErrorPic(H265_DEC_FRONT* const p_h265dec_front, H265_DEC* const p_dec,H265_DEC_SLICE *p_slice, 
	H265_DPB_LIST *p_dpb_list )
{
	VO_U32 list_size       = p_dpb_list->real_size;  
	H265_LIST_NODE*  p_node = p_dpb_list->real_head;
	H265_LIST_NODE* const p_base = p_dpb_list->list_node_pool;
	VO_BOOL in_loop = VO_FALSE;

	while (list_size) {
		TComPic*  rpcPic = p_node->data;    
		if (p_node == p_dpb_list->head) {// first pic
			in_loop = VO_TRUE;
		}

		if ( rpcPic->errFlag || (rpcPic->m_bNeededForOutput == VO_TRUE && rpcPic->m_iPOC <= p_dpb_list->m_iPOCLastDisplay)) { // found VO_H265_ERR_InvalidRefPic, retire it
			//printf("retire pic: %x\n",rpcPic);
			if ( p_dec->nThdNum> 1 ) {

				//wait for other thread finish decoding
				h265_wave3DThdPool * const pWave3DThdPool = p_dec->pWave3DThdPool;
				pthread_mutex_lock(&pWave3DThdPool->freeFrameMutex);
				while (pWave3DThdPool->freeFrameList.size+1 < pWave3DThdPool->freeFrameList.szPool) //+1 means all other frames are finished excluding cur frame
				{
					pthread_cond_wait(&pWave3DThdPool->freeFrameCond, &pWave3DThdPool->freeFrameMutex);
				}		
				pthread_mutex_unlock(&pWave3DThdPool->freeFrameMutex);

				//wait for display list clean
LOGI("before retire err pic, waiting dis_list clean\n");
				while (p_dpb_list->disListLeft != (VO_S32)p_dpb_list->node_num)
				{
					if (p_h265dec_front->runCMD)
					{
						p_h265dec_front->runCMD = 1;	//to make sure the api-thread not block
					}
				}
LOGI("waiting dis_list clean, end\n");
			}
			//rpcPic->errFlag = 0;                             //reset errFlag
			if (p_node == p_dpb_list->real_head)
			{

				if (p_node != p_dpb_list->tail)
				{
					if (p_node == p_dpb_list->head) {// first pic
						p_dpb_list->head = p_base + p_dpb_list->head->next;
					}
					p_dpb_list->real_head = p_base + p_dpb_list->real_head->next;
					p_node = p_dpb_list->real_head;
				}
				else //last pic
				{
					VOASSERT(p_dpb_list->real_size == 1 && p_dpb_list->size == 1);
					VOASSERT(p_node == p_dpb_list->head);
					p_dpb_list->real_head = p_dpb_list->head = p_dpb_list->tail = NULL;
					p_dpb_list->real_size = p_dpb_list->size = 0;
					break;
				}


// 				VOASSERT(p_node == p_dpb_list->head);
// 				if (p_node != p_dpb_list->tail)
// 				{
// 					p_dpb_list->real_head = p_dpb_list->head = p_base + p_dpb_list->head->next;
// 					p_node = p_dpb_list->head;
// 				}
// 				else
// 				{
// 					VOASSERT(p_dpb_list->real_size == 1 && p_dpb_list->size == 1);
// 					p_dpb_list->real_head = p_dpb_list->head = p_dpb_list->tail = NULL;
// 					p_dpb_list->real_size = p_dpb_list->size = 0;
// 					break;
// 				}
			}
			else
			{
				H265_LIST_NODE* p_previous = p_base + p_dpb_list->real_head->previous;
				H265_LIST_NODE* p_next = p_base + p_node->next;

				if (p_node == p_dpb_list->head) {// first pic
					if (p_node != p_dpb_list->tail)
					{
						p_dpb_list->head = p_base + p_dpb_list->head->next;
					}
					else //last pic
					{
						VOASSERT(p_dpb_list->size == 1);
						p_dpb_list->head = NULL;
					}
				}

				//delete p_node
				(p_base + p_node->previous)->next = p_node->next;
				p_next->previous = p_node->previous;
				if (p_node == p_dpb_list->tail)
				{
					p_dpb_list->tail = p_base + p_node->previous;
				}

				//insert p_node before p_dpb_list->real_head
				p_previous->next = p_node - p_base;
				p_node->previous = p_dpb_list->real_head->previous;
				p_node->next = p_dpb_list->real_head - p_base;
				p_dpb_list->real_head->previous = p_node - p_base;

				//if fifo is empty, need assign fifo to p_node
				if (p_dpb_list->p_fifo == p_dpb_list->real_head)
				{
					p_dpb_list->p_fifo = p_node;
				}

				p_node = p_next;

			}

// 			if (p_node == p_dpb_list->head) {// first pic
// 				p_dpb_list->head = p_base + p_dpb_list->head->next;
// 				p_node = p_dpb_list->head;
// 			} else {
// 				H265_LIST_NODE* p_previous = p_base + p_dpb_list->head->previous;
// 				H265_LIST_NODE* p_next = p_base + p_node->next;
// 
// 				//delete p_node
// 				(p_base + p_node->previous)->next = p_node->next;
// 				p_next->previous = p_node->previous;
// 				if (p_node == p_dpb_list->tail)
// 				{
// 					p_dpb_list->tail = p_base + p_node->previous;
// 				}
// 
// 				//insert p_node before p_dpb_list->head
// 				p_previous->next = p_node - p_base;
// 				p_node->previous = p_dpb_list->head->previous;
// 				p_node->next = p_dpb_list->head - p_base;
// 				p_dpb_list->head->previous = p_node - p_base;
// 
// 				p_node = p_next;
// 			}

			if (in_loop)
			{
				p_dpb_list->size--;
			}
			p_dpb_list->real_size--;
			//break;
			list_size--;
			continue; //retire as possible as we can. (For multi-core)
		}

		p_node = p_base + p_node->next;
		list_size--;
	}


}

static TComPic* xGetNewPicBuffer(H265_DEC_FRONT* const p_h265dec_front, H265_DEC* const p_dec, H265_DEC_SLICE *p_slice, 
	H265_DPB_LIST *p_dpb_list )
{
	//H265_DEC_SLICE *p_slice = &p_dec->slice;
	//TComPic* p_pic;
	//YU_TBD, must cowork dpb_list and p_fifo
	H265_LIST_NODE* const p_base = p_dpb_list->list_node_pool;

	if (p_slice->p_sps != NULL)
	{
		VO_U32 list_size; 
		VO_U32 max_ref_pic_num = p_slice->p_sps->sps_max_dec_pic_buffering_minus1[p_slice->m_uiTLayer] + 1;// +1 to have space for the picture currently being decoded
		
		xRetireRefErrorPic(p_h265dec_front, p_dec, p_slice, p_dpb_list); //for stability
		list_size = p_dpb_list->real_size; 

		/* retire one dpb from head*/
		if (list_size >= max_ref_pic_num) {//list is already sorted by POC
			//YU_TBD is it standard compliant??
			//VO_BOOL bBufferIsAvailable = VO_FALSE;
			H265_LIST_NODE*  p_node = p_dpb_list->real_head;
			VO_BOOL in_loop = VO_FALSE;

			while (list_size) {
				TComPic*  rpcPic = p_node->data;
				//VO_S32  isReference = 0;//i,
				if (p_node == p_dpb_list->head) {// first pic
					in_loop = VO_TRUE;
				}
				if ( rpcPic->m_bNeededForOutput == VO_FALSE && rpcPic->m_bRefenced == VO_FALSE) {// output and no referenced pic
LOGI("retire POC %ld\n", rpcPic->m_iPOC);
					if (p_node == p_dpb_list->real_head)
					{
						if (p_node != p_dpb_list->tail)
						{
							if (p_node == p_dpb_list->head) {// first pic
								p_dpb_list->head = p_base + p_dpb_list->head->next;
							}
							p_dpb_list->real_head = p_base + p_dpb_list->real_head->next;
							p_node = p_dpb_list->real_head;
						}
						else //last pic
						{
							VOASSERT(p_dpb_list->real_size == 1 && p_dpb_list->size == 1);
							VOASSERT(p_node == p_dpb_list->head);
							p_dpb_list->real_head = p_dpb_list->head = p_dpb_list->tail = NULL;
							p_dpb_list->real_size = p_dpb_list->size = 0;
							break;
						}
					}
					else
					{
						H265_LIST_NODE* p_previous = p_base + p_dpb_list->real_head->previous;
						H265_LIST_NODE* p_next = p_base + p_node->next;

						if (p_node == p_dpb_list->head) {// first pic
							if (p_node != p_dpb_list->tail)
							{
								p_dpb_list->head = p_base + p_dpb_list->head->next;
							}
							else //last pic
							{
								VOASSERT(p_dpb_list->size == 1);
								p_dpb_list->head = NULL;
							}
						}

						//delete p_node
						(p_base + p_node->previous)->next = p_node->next;
						p_next->previous = p_node->previous;
						if (p_node == p_dpb_list->tail)
						{
							p_dpb_list->tail = p_base + p_node->previous;
						}

						//insert p_node before p_dpb_list->real_head
						p_previous->next = p_node - p_base;
						p_node->previous = p_dpb_list->real_head->previous;
						p_node->next = p_dpb_list->real_head - p_base;
						p_dpb_list->real_head->previous = p_node - p_base;

						//if fifo is empty, need assign fifo to p_node
						if (p_dpb_list->p_fifo == p_dpb_list->real_head)
						{
							p_dpb_list->p_fifo = p_node;
						}

						p_node = p_next;
					}


// 					if (p_node == p_dpb_list->head) {// first pic
// 						p_dpb_list->head = p_base + p_dpb_list->head->next;
// 						p_node = p_dpb_list->head;
// 					} else {
// 						H265_LIST_NODE* p_previous = p_base + p_dpb_list->head->previous;
// 						H265_LIST_NODE* p_next = p_base + p_node->next;
// 
// 						//delete p_node
// 						(p_base + p_node->previous)->next = p_node->next;
// 						p_next->previous = p_node->previous;
// 						if (p_node == p_dpb_list->tail)
// 						{
// 							p_dpb_list->tail = p_base + p_node->previous;
// 						}
// 
// 						//insert p_node before p_dpb_list->head
// 						p_previous->next = p_node - p_base;
// 						p_node->previous = p_dpb_list->head->previous;
// 						p_node->next = p_dpb_list->head - p_base;
// 						p_dpb_list->head->previous = p_node - p_base;
// 
// 						p_node = p_next;
// 					}
					if (in_loop)
					{
// printf("retire POC %d, size %d\n", rpcPic->m_iPOC, p_dpb_list->size);
						p_dpb_list->size--;
					}
					p_dpb_list->real_size--;

					//break;
					list_size--;
					continue; //Harry:to retire as possible as we can. (For multi-core)
				}

				p_node = p_base + p_node->next;
				list_size--;
			}


			//PushDpbFifo(p_dbp_fifo, rpcPic);
		}

//		if (p_dpb_list->size > max_ref_pic_num)
//		{
//			if (p_dpb_list->size >= p_dpb_list->node_num)
//			{
//				return NULL;
//			}
//		}
	}

	/* pop one dpb in fifo and insert to list*/
	//if (p_dpb_list->p_fifo != p_dpb_list->head ) {
	//H265_LIST_NODE* p_fifo = p_dpb_list->p_fifo;
	//p_pic = p_dpb_list->p_fifo->data;

	// 	if (p_dpb_list->p_fifo == p_dpb_list->head) {//No empty pic
	//return NULL;
	// 	}
	/* Get an empty pic from fifo and inset to tail*/
	if (p_dpb_list->tail == NULL) {//it's empty after initilization and refresh 
		p_dpb_list->tail = p_dpb_list->head = p_dpb_list->real_head = p_dpb_list->p_fifo;

		/*update fifo*/
		p_dpb_list->p_fifo = p_base + p_dpb_list->p_fifo->next;
	}else if(p_dpb_list->p_fifo == p_dpb_list->real_head){

		return NULL;
// 		if ( p_dec->nThdNum> 1 ) 
// 		{
// 			//wait for other thread finish decoding
// 			h265_wave3DThdPool * const pWave3DThdPool = p_dec->pWave3DThdPool;
// 			pthread_mutex_lock(&pWave3DThdPool->freeFrameMutex);
// 			while (pWave3DThdPool->freeFrameList.size+1 < pWave3DThdPool->freeFrameList.szPool) //+1 means all other frames are finished excluding cur frame
// 			{
// 				pthread_cond_wait(&pWave3DThdPool->freeFrameCond, &pWave3DThdPool->freeFrameMutex);
// 			}		
// 			pthread_mutex_unlock(&pWave3DThdPool->freeFrameMutex);
// 		}
// 		xWriteOutput( p_dec, p_dpb_list, 0 );//flush dis pics
// 		ClearDpbList( p_dpb_list );	
// 		p_dpb_list->tail = p_dpb_list->head = p_dpb_list->p_fifo;
// 		p_dpb_list->p_fifo = p_base + p_dpb_list->p_fifo->next;
	} else if (p_dpb_list->tail->next != p_dpb_list->p_fifo - p_base) {//insert to tail
		H265_LIST_NODE* p_fifo = p_dpb_list->p_fifo;
		H265_LIST_NODE* p_next = p_base + p_dpb_list->tail->next;

		/*update fifo*/
		p_dpb_list->p_fifo = p_base + p_fifo->next;

		//delete p_fifo
		(p_base + p_fifo->previous)->next = p_fifo->next;
		(p_base + p_fifo->next)->previous = p_fifo->previous;

		//insert p_fifo after p_dpb_list->tail
		p_next->previous  = p_fifo - p_base;
		p_fifo->previous = p_dpb_list->tail - p_base;
		p_fifo->next = p_dpb_list->tail->next;
		p_dpb_list->tail->next = p_fifo - p_base;	

		p_dpb_list->tail = p_fifo;
	} else {

		p_dpb_list->tail = p_dpb_list->p_fifo;

//		if (p_dpb_list->tail->data->nProgress_mv == VO_MAX_INT && !p_dpb_list->tail->data->bFinishDecode && p_dpb_list->tail->data->errFlag == 0)
//		{
//			assert(0);
//		}
		/*update fifo*/
		p_dpb_list->p_fifo = p_base + p_dpb_list->p_fifo->next;
	}

	if (p_dpb_list->head == NULL)
	{
		VOASSERT(p_dpb_list->size == 0);
		p_dpb_list->head = p_dpb_list->tail;
	}

	p_dpb_list->size++;
	p_dpb_list->real_size++;

	assert(p_dpb_list->real_size<= p_dpb_list->node_num);
	// reset pciture buffer parameters
	// TODO: in future need to add reset_pic_buffer() methods.
	return p_dpb_list->tail->data;
}



static THREAD_FUNCTION DecoderMain(void *pArg)
{
	THREAD_FUNCTION_RETURN exitCode = 0;
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT *)pArg;
	H265_DEC *p_dec = (H265_DEC *)p_h265dec_front->p_dec;
	H265_DEC_SLICE *p_slice;
	TComPic* p_pic;
	VO_S32 ret;

	p_h265dec_front->bRunning = 1;

	while (p_h265dec_front->runCMD)
	{
		//1.wait for input
		pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
waitForNewInput:
LOGI("wait for input\n");
		while (p_h265dec_front->new_input == 0 && p_h265dec_front->runCMD == 1)
		{
			pthread_cond_wait(&p_h265dec_front->inputStreamCond, &p_h265dec_front->inputStreamMutex);
		}
LOGI("wait for input end\n");
		
		if (!p_h265dec_front->runCMD)
		{
LOGI("recv uninit cmd\n");
			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
			break;
		}

		if (p_h265dec_front->runCMD == 2)
		{
			//flush cmd
LOGI("recv flush cmd\n");
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain_wave3D(p_dec, NULL, NULL, 2);
			}
			else
			{
				xWriteOutput(p_dec, &p_dec->dpb_list, -1);
			}

			
			p_h265dec_front->runCMD = 1; //tell back to the API: flush done
// 			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);

			goto waitForNewInput;
		}
// 		if (p_h265dec_front->new_input > 0)
		{
#if ENABLE_DROP_FRAME
			if (p_dec->fast_mode & VO_FM_DROP_UNREF) //need to drop frame
			{
				VO_U32 i;
				while (p_h265dec_front->new_input > 0 && p_h265dec_front->bCanDrop[0] && p_h265dec_front->nContinueDropped < MAX_DROP_FRAME_INTERVAL)
				{
					//drop this frame
					p_h265dec_front->p_cur_buf_pos += p_h265dec_front->frameLen[0];
					p_h265dec_front->left_buf_len -= p_h265dec_front->frameLen[0];
					p_h265dec_front->new_input--;
					p_h265dec_front->nContinueDropped++;
					for (i = 0; i < p_h265dec_front->new_input; ++i)
					{
						p_h265dec_front->frameLen[i] = p_h265dec_front->frameLen[i+1];
						p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
						p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
					}
					p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
					p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
// 					return VO_ERR_DROPPEDFRAME;
				}
			}
			if (p_h265dec_front->new_input > 0)
			{
				p_h265dec_front->nContinueDropped = 0;
#endif
// 				p_dec->in_data = p_h265dec_front->p_cur_buf_pos;
				p_dec->data_sz = p_h265dec_front->frameLen[0];
				p_dec->timestamp = p_h265dec_front->Time[0];

				{
					VO_U32 size = p_dec->data_sz/INPUT_SIZE_INC_STEP;
					if (size >= p_dec->frameDataSize)
					{
						if (p_dec->in_data)
						{
							AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_dec->in_data);
						}
						p_dec->in_data = (VO_U8 *)AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, (++size)*INPUT_SIZE_INC_STEP, CACHE_LINE);
						if (p_dec->in_data == NULL)
						{
							//TBD
							p_dec->frameDataSize = 0;
							pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
							exitCode = (THREAD_FUNCTION_RETURN)VO_ERR_OUTOF_MEMORY; 
							break;
						}
						p_dec->frameDataSize = size;
					}
				}
				CopyMem(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, p_dec->in_data, p_h265dec_front->p_cur_buf_pos, p_dec->data_sz);

LOGI("get a new input, size: %lu\n", p_dec->data_sz);
#if ENABLE_DROP_FRAME
			}
			else
			{
				//all input has dropped
// 				pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
				goto waitForNewInput;
			}
#endif
		}

// 		if (p_dec->nThdNum > 1)
// 		{
// 			// LOGI("DecodeFrameMain start");
// 			ret = DecodeFrameMain_wave3D(p_dec,p_out_vbuf, p_out_vbuf_info, p_h265dec_front->new_input > 0);
// 			// LOGI("DecodeFrameMain end");
// 		}
// 		else
// 		{
// 			if (p_h265dec_front->new_input > 0)
// 			{
// 				ret = DecodeRawVideo(p_dec, p_h265dec_front->p_cur_buf_pos, p_h265dec_front->frameLen[0]);
// 			}
// 
// 			GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
// 		}

// 		if (p_h265dec_front->new_input > 0)
		{
			VO_U32 i;
			p_h265dec_front->p_cur_buf_pos += p_h265dec_front->frameLen[0];
			VOASSERT(p_h265dec_front->left_buf_len >= p_h265dec_front->frameLen[0]);
			p_h265dec_front->left_buf_len -= p_h265dec_front->frameLen[0];
			p_h265dec_front->new_input--;
			for (i = 0; i < p_h265dec_front->new_input; ++i)
			{
				p_h265dec_front->frameLen[i] = p_h265dec_front->frameLen[i+1];
#if ENABLE_DROP_FRAME
				p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
#endif
				p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
			}
#if ENABLE_DROP_FRAME
			p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
#endif
			p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
		}

		pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);


LOGI("wait for free frame room\n");
		if (p_dec->nThdNum > 1)
		{
			//2.wait for free frame room
			h265_wave3DThdPool *pThdPool = p_dec->pWave3DThdPool;

			pthread_mutex_lock(&pThdPool->freeFrameMutex);
			while ((p_slice = (Parser_T *)PopTaskQ(&pThdPool->freeFrameList)) == NULL)
			{
				pthread_cond_wait(&pThdPool->freeFrameCond, &pThdPool->freeFrameMutex);
			}
			pthread_mutex_unlock(&pThdPool->freeFrameMutex);
		}
		else
		{
			p_slice = &p_dec->slice;
			p_slice->timestamp = p_dec->timestamp;
		}
		VOASSERT(p_slice != NULL);
		p_slice->errFlag = 0;
LOGI("wait for free frame room......end\n");
		//3.wait for free frame buffer
		if (p_dec->pic_buf_done)
		{
			pthread_mutex_lock(&p_h265dec_front->getPicMutex);
startGetNewBuffer:
			while (p_h265dec_front->runCMD == 1 && ((p_pic = xGetNewPicBuffer(p_h265dec_front, p_dec, p_slice, &p_dec->dpb_list)) == NULL))
			{
				pthread_cond_wait(&p_h265dec_front->getPicCond, &p_h265dec_front->getPicMutex);
			}
			if (p_h265dec_front->runCMD == 2)
			{
LOGI("flush cmd not done yet\n");
				p_dec->bOutputFull = 1;
				p_h265dec_front->runCMD = 1;
				goto startGetNewBuffer;
			}
// printf("get new pic, size %d, real size %d\n", p_dec->dpb_list.size, p_dec->dpb_list.real_size);
			pthread_mutex_unlock(&p_h265dec_front->getPicMutex);

			if (p_pic == NULL)
			{
				VOASSERT(p_h265dec_front->runCMD == 0);
LOGI("recv uninit cmd\n");
				//recv unit cmd
				if (p_dec->nThdNum > 1)
				{
					h265_wave3DThdPool *pThdPool = p_dec->pWave3DThdPool;

					pthread_mutex_lock(&pThdPool->freeFrameMutex);
					PushTaskQ(&pThdPool->freeFrameList, p_slice);
					pthread_mutex_unlock(&pThdPool->freeFrameMutex);

					break;
				}
			}

			VOASSERT(p_pic != NULL);
			p_slice->cur_pic = p_pic;

		}
		else
		{
			p_slice->cur_pic = NULL;
		}
LOGI("wait for free frame buffer......end. start decoder thd %ld\n", p_dec->nThdNum);
		
		
		//4.do decode
		if (p_dec->nThdNum > 1)
		{
			ret = DecodeFrameMain_wave3D(p_dec, (VO_VIDEO_BUFFER *)p_slice, NULL, 1);
		}
		else
		{
			ret = DecodeRawVideo(p_dec, p_dec->in_data, p_dec->data_sz);
		}
		if (ret)
		{
			pthread_mutex_lock(&p_dec->disListMutex);
			if (p_slice->cur_pic)
			{
				p_slice->cur_pic->errFlag = ret;
			}
			pthread_cond_signal(&p_dec->disListCond);
			pthread_mutex_unlock(&p_dec->disListMutex);
		}
	}

	p_h265dec_front->bRunning = 0;
	return exitCode;
}

static VO_U32 NewAPIInit(H265_DEC_FRONT* p_h265dec_front)
{

	VO_CODEC_INIT_USERDATA *p_user_op = p_h265dec_front->p_user_op;
	VO_U32 codec_id = p_h265dec_front->codec_id;

	//if (p_h265dec_front->input_mode == STRAM_MODE) 
	{//
		//Malloc internal raw data buffer
		p_h265dec_front->p_input_buf = (VO_U8*)AlignMalloc(p_user_op, codec_id, RAW_DATA_LEN, CACHE_LINE);
		if(!p_h265dec_front->p_input_buf)
			return VO_ERR_OUTOF_MEMORY;

		SetMem(p_user_op, codec_id, p_h265dec_front->p_input_buf, 0, RAW_DATA_LEN);
		p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
		p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
		p_h265dec_front->left_buf_len = 0;
		p_h265dec_front->used_buf_len = 0;
		p_h265dec_front->bFindFirstSlice = 0;
		p_h265dec_front->more_dis_pics = 0;
		p_h265dec_front->bNeedFlush = 0;	
	}

	//Create decoder main thread
	if (pthread_mutex_init(&p_h265dec_front->inputStreamMutex, NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_mutex_init(&p_h265dec_front->getPicMutex, NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&p_h265dec_front->inputStreamCond, NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&p_h265dec_front->getPicCond, NULL))
	{
		return VO_ERR_FAILED;
	}

	if (pthread_mutex_init(&((H265_DEC*)p_h265dec_front->p_dec)->disListMutex, NULL))
	{
		return VO_ERR_FAILED;
	}
	if (pthread_cond_init(&((H265_DEC*)p_h265dec_front->p_dec)->disListCond, NULL))
	{
		return VO_ERR_FAILED;
	}

	((H265_DEC*)p_h265dec_front->p_dec)->getPicCond = &p_h265dec_front->getPicCond;

	p_h265dec_front->runCMD = 1;
	p_h265dec_front->bRunning = 0;
	if (pthread_create((pthread_t *)(&(p_h265dec_front->thread_id)), NULL, DecoderMain, p_h265dec_front))
	{
		return VO_ERR_FAILED;
	}
	while (!p_h265dec_front->bRunning)
	{
		;
	}

	return VO_ERR_NONE;
}

static VO_U32 NewAPIUinit(H265_DEC_FRONT* p_h265dec_front)
{
	VO_U32 voRC = VO_ERR_NONE;
	H265_DEC *p_dec = (H265_DEC *)p_h265dec_front->p_dec;

	//release the decoder main thread
	p_h265dec_front->runCMD = 0;
	//signal
	pthread_cond_signal(&p_h265dec_front->inputStreamCond);
	pthread_cond_signal(&p_h265dec_front->getPicCond);
	//wait for thread finish
	// LOGI("wait for main thread finish\n");
	while(p_h265dec_front->bRunning)
	{
		thread_sleep(1);
	}
	pthread_join(p_h265dec_front->thread_id, NULL);
	p_h265dec_front->thread_id = (pthread_t)NULL;
	// LOGI("main thread finished\n");

	if (pthread_cond_destroy(&p_h265dec_front->inputStreamCond))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_cond_destroy(&p_h265dec_front->getPicCond))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&p_h265dec_front->inputStreamMutex))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&p_h265dec_front->getPicMutex))
	{
		voRC |= VO_ERR_FAILED;
	}

	if (pthread_cond_destroy(&p_dec->disListCond))
	{
		voRC |= VO_ERR_FAILED;
	}
	if (pthread_mutex_destroy(&p_dec->disListMutex))
	{
		voRC |= VO_ERR_FAILED;
	}

	//if (p_h265dec_front->input_mode == STRAM_MODE) 
	{
		//Free raw data buffer
		AlignFree(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_h265dec_front->p_input_buf);
	}

	return voRC;
}

VO_U32 VO_API H265DecInit( VO_HANDLE * phDec, VO_VIDEO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * p_user_op)
{
	H265_DEC_FRONT* p_h265dec_front = NULL;
	VO_U32 ret						= VO_ERR_NONE;
	VO_S32 codec_id;
#if ENABLE_LICENSE_CHECK	
	VO_PTR p_lic_handle				= NULL;
	VO_LIB_OPERATOR *p_lib_op		= NULL;
#endif

	if ( vType != VO_VIDEO_CodingH265 ) {
		return VO_ERR_NOT_IMPLEMENT;
	} else {
		codec_id = VO_INDEX_DEC_H265;
	}

    if(p_user_op!=NULL){
        VOLOGINIT((VO_CHAR *)p_user_op->reserved1);
    }
	VOLOGI("start init!\n");
#if ENABLE_LICENSE_CHECK
	if ( p_user_op ) {
		if ( p_user_op->memflag & 0XF0 ) {//valid libOperator flag
			p_lib_op = p_user_op->libOperator;
		}
	}
	ret = voCheckLibInit( &p_lic_handle, codec_id, p_user_op ? p_user_op->memflag : 0, g_h265dec_inst, p_lib_op );

	if ( ret != VO_ERR_NONE ) {
		voCheckLibUninit( p_lic_handle );
		p_lic_handle = NULL;
		return ret;
	}
#endif

	if ( p_user_op ) {
		if ( ( !( p_user_op->memflag&0xF ) ) || ( p_user_op->memData == NULL ) ) {
			p_user_op = NULL;
		}
	}
	p_h265dec_front = ( H265_DEC_FRONT* )AlignMalloc( p_user_op, codec_id, sizeof( H265_DEC_FRONT ), CACHE_LINE );
	if( !p_h265dec_front )
		return VO_ERR_OUTOF_MEMORY;
    
	SetMem( p_user_op, codec_id, ( VO_U8 * )p_h265dec_front, 0, sizeof( H265_DEC_FRONT ) );

	// Create video decoder engine
	ret = CreateH265Decoder( &p_h265dec_front->p_dec, p_user_op, codec_id );
	if( ret != VO_ERR_NONE ){
		AlignFree( p_user_op, codec_id, p_h265dec_front );
		return ret;
	}

	p_h265dec_front->p_user_op = p_user_op;
	p_h265dec_front->codec_id = codec_id;

#if ENABLE_LICENSE_CHECK	
	p_h265dec_front->p_lic_handle = p_lic_handle; 
#endif

#if H265DEC_DEBUG
	//((H265_DEC *)p_h265dec_front->p_dec)->start_time = clock();
	IniDebug();
#endif

	ret = NewAPIInit(p_h265dec_front);
	if( ret != VO_ERR_NONE )
	{
		NewAPIUinit(p_h265dec_front);
		DeleteH265Decoder(p_h265dec_front->p_dec);
		AlignFree( p_user_op, codec_id, p_h265dec_front );
		return ret;
	}

	*phDec = p_h265dec_front;
	VOLOGI("finish init!\n");
	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecUinit(VO_HANDLE hCodec)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hCodec;
	H265_DEC *p_dec;
// 	VO_U32 voRC = VO_ERR_NONE;
	//long start_time = ((H265_DEC *)p_h265dec_front->p_dec)->start_time;
    VOLOGI("unit start\n");

	if(p_h265dec_front == NULL)
		return VO_ERR_INVALID_ARG;

	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

#if ENABLE_LICENSE_CHECK
	voCheckLibUninit (p_h265dec_front->p_lic_handle);
#endif	

#if USE_3D_WAVE_THREAD
	if (p_dec->nThdNum > 1)
	{
		ReleaseH265Threads_wave3D(p_dec);
		p_dec->nThdNum = 0;
	}

#endif

	NewAPIUinit(p_h265dec_front);

	//delete decoder engine
	DeleteH265Decoder(p_h265dec_front->p_dec);

	//Free front handle
	AlignFree(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_h265dec_front);

#if H265DEC_DEBUG
	{
		//long decode_time  = clock() - start_time;
		//VO_LOG(LOG_FILE, LOG_GLOBAL | LOG_STAT_FINFO,"Decode Time(MilliSec)        : %d\n", decode_time ); 

		UiniDebug();
	}
#endif
    VOLOGI("unit end\n");
    VOLOGUNINIT();
	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecGetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR plValue)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hCodec;
	H265_DEC *p_dec;
	//VO_U32 ret = VO_ERR_NONE;

	if(p_h265dec_front == NULL)
		return VO_ERR_INVALID_ARG;

	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

	switch(nID) {
	case VO_PID_VIDEO_FRAMETYPE: //TBD
		{
#if 0
			VO_VIDEO_OUTPUTINFO vOutPutInfo;
			VO_CODECBUFFER* p_codec_buf = (VO_CODECBUFFER *)plValue;

			ret = voMPEGDecFrame(p_h265dec_front->p_dec, p_codec_buf, NULL, &vOutPutInfo, DEC_FRAMETYPE);
			if(CHECK_API_ERR(ret))
				return ret;

			p_codec_buf->Time = (VO_S64)vOutPutInfo.Format.Type;
#endif
		}
		break;
	case VO_PID_VIDEO_ASPECTRATIO:
		*((VO_S32 *)plValue) = ( VO_S32 )p_dec->sps[ 0 ].aspect_ratio;//YU_TBD, temp solution, aspect ration should get from display pic related sps
		break;
	case VO_PID_DEC_H265_FASTMODE:
		*((VO_U32*)plValue) = p_dec->fast_mode;
		break;
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}

	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecSetParameter(VO_HANDLE hCodec, VO_S32 nID, VO_PTR lValue)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hCodec;
	H265_DEC *p_dec;
	//VO_VIDEO_BUFFER out_vbuf;
	//VO_VIDEO_OUTPUTINFO out_vbuf_info;
	//VO_CODECBUFFER in_putbuf;

	VO_S32 ret = VO_ERR_NONE;


	if(p_h265dec_front == NULL)
		return VO_ERR_INVALID_ARG;

	p_dec = (H265_DEC *)p_h265dec_front->p_dec;
	if(p_dec == NULL)
		return VO_ERR_INVALID_ARG;
    VOLOGI("start setparameter\n");
	switch(nID) {
	case VO_PID_COMMON_FRAME_BUF_BACK:
		{
			VO_VIDEO_BUFFER *buf = (VO_VIDEO_BUFFER *)lValue;
			TComPic* pic = (TComPic *)(buf->CodecData);
			if (pic && pic->m_bNeededForOutput == PIC_OUT)
			{
				pic->m_bNeededForOutput = VO_FALSE;
                VOLOGI("POC %ld back\n", pic->m_iPOC);
				//signal
				pthread_cond_signal(&p_h265dec_front->getPicCond);
			}
		}
		break;
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER *p_codec_buf = (VO_CODECBUFFER *)lValue;
			VO_U8* p_copy;

			if (p_codec_buf->Length == 0)
			{
				return VO_ERR_INPUT_BUFFER_SMALL;
			}

			p_copy = (VO_U8*)AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8) * p_codec_buf->Length, CACHE_LINE);
			if (!p_copy)
			{
				return VO_ERR_OUTOF_MEMORY;
			}

			CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_copy, p_codec_buf->Buffer, p_codec_buf->Length);

// 			ret = DecodeRawVideo((H265_DEC*)p_h265dec_front->p_dec, p_copy, p_codec_buf->Length);
			if (1)
			{
				//put header data just after input buffer
				VO_U8 *inBuf = p_h265dec_front->p_input_buf;
				const VO_U8 *curPos = p_h265dec_front->p_cur_buf_pos;
				const VO_U8 *lastPos = p_h265dec_front->lastPos;
				VO_U8 *endPos = p_h265dec_front->p_cur_buf_pos + p_h265dec_front->left_buf_len;
				assert(endPos - inBuf <= RAW_DATA_LEN);

				if (p_codec_buf->Length > RAW_DATA_LEN/2)
				{
					return VO_ERR_INVALID_ARG;
				}

				if (p_h265dec_front->left_buf_len + p_codec_buf->Length >= RAW_DATA_LEN)
				{
					if (p_h265dec_front->new_input == 0 && p_h265dec_front->left_buf_len >= RAW_DATA_LEN/2)
					{
						curPos = p_h265dec_front->p_cur_buf_pos = inBuf;
						lastPos = p_h265dec_front->lastPos = inBuf;
						endPos = inBuf;
						p_h265dec_front->left_buf_len = 0;
						p_h265dec_front->bFindFirstSlice = 0;
					}
					else
					{
						return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
					}
				}

				if (endPos + p_codec_buf->Length >= inBuf + RAW_DATA_LEN)
				{
					//copy left data to the head of memory pool
					memmove(inBuf, curPos, p_h265dec_front->left_buf_len);
					p_h265dec_front->lastPos = lastPos -= (curPos - inBuf); 
					p_h265dec_front->p_cur_buf_pos = inBuf;
					curPos = inBuf;
					endPos = inBuf + p_h265dec_front->left_buf_len;
				}

				if (p_h265dec_front->new_input >= MAX_FRAME_INPUT)
				{
					return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
				}
				CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, endPos, p_codec_buf->Buffer, p_codec_buf->Length);
				p_h265dec_front->left_buf_len += p_codec_buf->Length;
			}

			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id,p_copy);
		}
		break;
	case VO_PID_COMMON_FLUSH:
		{
			//Add for clean buffer
			VO_VIDEO_BUFFER outData;
			VO_VIDEO_OUTPUTINFO outFormat;

			pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
			//reset input buffer
			p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
			p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
			p_h265dec_front->left_buf_len = 0;
			p_h265dec_front->used_buf_len = 0;
			p_h265dec_front->bFindFirstSlice = 0;
			p_h265dec_front->new_input = 0;
			p_h265dec_front->more_dis_pics = 0;
			p_h265dec_front->nContinueDropped = 0;

			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
			pthread_cond_signal(&p_h265dec_front->inputStreamCond);
			pthread_cond_signal(&p_h265dec_front->getPicCond);

			p_h265dec_front->bNeedFlush = 0;  //disable the file-end flush

seekFlushStart:
            VOLOGI("flush by seek1\n");
			//step 1: output the frame which should be normally output for display
			while (1)
			{
				ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
				if(!ret && (outFormat.Format.Type != VO_VIDEO_FRAME_NULL)) //in the end of file, we must get all next output
				{
					//for valid frame, set back the frame buffer to decoder
					TComPic* pic = (TComPic *)(outData.CodecData);
					if (pic->m_bNeededForOutput == PIC_OUT)
					{
						pic->m_bNeededForOutput = VO_FALSE;
// 						pic->m_bRefenced = VO_FALSE;
						//signal
						pthread_cond_signal(&p_h265dec_front->getPicCond);
					}

					if (!outFormat.Flag)
					{
						break;
					}
				}
				else
				{
					break;
				}
			}

			//step 2: send flush cmd, sync up for all decoding frames finished and then flush dpb
			pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
			pthread_mutex_lock(&p_h265dec_front->getPicMutex);
			//set flush cmd to decoder
			p_h265dec_front->runCMD = 2;
			pthread_mutex_unlock(&p_h265dec_front->getPicMutex);
			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
			pthread_cond_signal(&p_h265dec_front->inputStreamCond);
			pthread_cond_signal(&p_h265dec_front->getPicCond);

			//wait for decoder finish flush
			while (p_h265dec_front->runCMD == 2)
			{
				;
			}

            VOLOGI("flush by seek2\n");
			pthread_mutex_lock(&p_h265dec_front->getPicMutex);
			if (p_dec->bOutputFull) //make sure decoder recv the cmd
			{
				p_dec->bOutputFull = 0;
				pthread_mutex_unlock(&p_h265dec_front->getPicMutex);
				goto seekFlushStart;
			}
			pthread_mutex_unlock(&p_h265dec_front->getPicMutex);

			//step 3: get out all frames left
			//LOGI("pDecoder->bFindFirstSlice:%d,pDecoder->new_input:%d,pDecoder->bHasNext:%d\r\n",pDecoder->bFindFirstSlice,pDecoder->new_input,pDecoder->bHasNext);
			ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
			while(!ret && (outFormat.Format.Type != VO_VIDEO_FRAME_NULL || outFormat.Flag)) //in the end of file, we must get all next output
			{
				if (outFormat.Format.Type != VO_VIDEO_FRAME_NULL)
				{
					//for valid frame, set back the frame buffer to decoder
					TComPic* pic = (TComPic *)(outData.CodecData);
					if (pic->m_bNeededForOutput == PIC_OUT)
					{
						pic->m_bNeededForOutput = VO_FALSE;
						pic->m_bRefenced = VO_FALSE;

						//signal
						pthread_cond_signal(&p_h265dec_front->getPicCond);
					}
				}
				ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
			}

			//step 4: clear ref flag for all pic in dpb
			xUnrefAllPic(&p_dec->dpb_list);
			p_dec->frame_num = 0; //clear the output frame num
            VOLOGI("flush by seek done\n");
			//LOGI("flush poutput:%d returnCode:%x\r\n",pDecGlobal->output_size,returnCode);
		}
		break;
	case VO_PID_DEC_H265_FLUSH_PICS:
	case VO_PID_COMMON_EOF_FLUSH:
		{
			pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
			if (p_h265dec_front->bFindFirstSlice)
			{
				p_h265dec_front->frameLen[p_h265dec_front->new_input] = p_h265dec_front->left_buf_len - (p_h265dec_front->lastPos - p_h265dec_front->p_cur_buf_pos);
				p_h265dec_front->new_input++;
// 				p_h265dec_front->left_buf_len = 0;
				p_h265dec_front->bFindFirstSlice = 0;
			}
			else if (p_h265dec_front->new_input == 0)
			{
				//reset input buffer
				p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
				p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
				p_h265dec_front->left_buf_len = 0;
				p_h265dec_front->used_buf_len = 0;
			}
			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
			pthread_cond_signal(&p_h265dec_front->inputStreamCond);

			p_h265dec_front->bNeedFlush = 1;

// 			if (p_h265dec_front->more_dis_pics)
// 			{
// 				p_h265dec_front->bNeedFlush = 1;	//need call GetOutputData before flush, flush will delay
// 				break;
// 			}
// 
// 			if(p_h265dec_front->new_input > 0)
// 			{
// #if USE_3D_WAVE_THREAD
// 				p_dec->in_data = p_h265dec_front->p_cur_buf_pos;
// 				p_dec->data_sz = p_h265dec_front->frameLen[0];
// 				p_dec->timestamp = p_h265dec_front->Time[0];
// 
// 				if (p_dec->nThdNum > 1)
// 				{
// 					ret = DecodeFrameMain_wave3D(p_dec,NULL, NULL, 3);
// 				}
// 				else
// #else
// 				p_dec->slice.timestamp = p_h265dec_front->Time[0];
// #endif
// 				{
// 					ret = DecodeRawVideo(p_dec, p_h265dec_front->p_cur_buf_pos, p_h265dec_front->frameLen[0]);
// 				}
// 
// 				{
// 					VO_U32 i;
// 					p_h265dec_front->p_cur_buf_pos += p_h265dec_front->frameLen[0];
// 					p_h265dec_front->left_buf_len -= p_h265dec_front->frameLen[0];
// 					p_h265dec_front->new_input--;
// 					for (i = 0; i < p_h265dec_front->new_input; ++i)
// 					{
// 						p_h265dec_front->frameLen[i] = p_h265dec_front->frameLen[i+1];
// #if ENABLE_DROP_FRAME
// 						p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
// #endif
// 						p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
// 					}
// #if ENABLE_DROP_FRAME
// 					p_h265dec_front->bCanDrop[i] = p_h265dec_front->bCanDrop[i+1];
// #endif
// 					p_h265dec_front->Time[i] = p_h265dec_front->Time[i+1];
// 				}
// 			}
// 
// 			if (p_h265dec_front->new_input > 0)
// 			{
// 				p_h265dec_front->bNeedFlush = 1;	//need call GetOutputData before flush, flush will delay
// 				break;
// 			}
// #if USE_3D_WAVE_THREAD
// 			if (p_dec->nThdNum > 1)
// 			{
// 				DecodeFrameMain_wave3D(p_dec, NULL, NULL, 2);
// 			}
// 			else
// #endif
// 			{
// 				xWriteOutput(p_dec, &p_dec->dpb_list, 0);
// 			}
		}
		break;
	case VO_PID_VIDEO_VIDEOMEMOP:
		p_dec->user_op_all.p_pic_buf_op = (VO_MEM_VIDEO_OPERATOR *)lValue;
		break;
	case VO_PID_COMMON_HeadInfo:
		p_dec->frame_num = 0;
		break;
	case VO_PID_COMMON_MAX_BA_SIZE:
		{
			VO_VIDEO_FORMAT * pf = (VO_VIDEO_FORMAT*)lValue;
			if (p_dec->using_BA_size == 0 && p_dec->BA_max_width == 0 && p_dec->BA_max_height == 0)
			{
				if (pf->Width <= 0 || pf->Height <= 0 ||
					pf->Width > 2560 || pf->Height > 2560)
				{
					return VO_ERR_INVALID_ARG;
				}
				p_dec->using_BA_size = 1;
				p_dec->BA_max_width = (VO_U32)pf->Width;
				p_dec->BA_max_height = (VO_U32)pf->Height;
			}
			else
			{
				return VO_ERR_INVALID_ARG;
			}
		}
		break;
	case VO_PID_VIDEO_OUTPUTMODE:
		p_dec->out_mode = *((VO_S32 *)lValue);
		break;
	case VO_PID_COMMON_CPUNUM:
		{
			VO_S32 nCPU = *((VO_S32*)lValue);
			VO_S32 maxThdNum;
#if USE_3D_WAVE_THREAD
			maxThdNum = MAX_THREAD_NUM2;
#else
			maxThdNum = 1;
#endif
			if (nCPU <= 0)
			{
				return VO_ERR_INVALID_ARG;
			}
			if (nCPU > maxThdNum)
			{
				nCPU = maxThdNum;
				*((VO_S32*)lValue) = nCPU; //put back the value really set
			}

			if (nCPU != p_dec->nThdNum)
			{
#if USE_3D_WAVE_THREAD
				if (p_dec->nThdNum > 1)
				{
					ReleaseH265Threads_wave3D(p_dec);
				}
				p_dec->nThdNum = nCPU;
				if (p_dec->nThdNum > 1)
				{
					if (1 + nCPU + p_dec->dpb_list.node_num > MAX_DPB_SIZE)
						return VO_ERR_WRONG_PARAM_ID;
					p_dec->dpb_list.node_num += nCPU + 1;
					InitH265Threads_wave3D(p_dec);
				}
#endif
			}
		}
		break;
	case VO_PID_COMMON_FRAME_BUF_EX:
		if (*((VO_U32*)lValue) > (MAX_DPB_SIZE  - p_dec->dpb_list.node_num))
			return VO_ERR_WRONG_PARAM_ID;
		p_dec->dpb_list.node_num += *((VO_U32*)lValue);
		break;
	case VO_PID_DEC_H265_FASTMODE:
		if ((p_dec->fast_mode & VO_FM_AUTO_FASTMODE) && *((VO_S32*)lValue) != 0) //in auto mode, ignore the outside cmd unless recv 0
		{
			break;
		}
		p_dec->fast_mode = *((VO_S32*)lValue);
        VOLOGI("Set fast mode %lx", (VO_U32)p_dec->fast_mode);
#if USE_3D_WAVE_THREAD
		if (p_dec->nThdNum > 1)
		{
		}
		else
#endif
		{
			p_dec->slice.fast_mode = *((VO_S32*)lValue);	//single-core
		}
		break;
	case VO_PID_VIDEO_THUMBNAIL_MODE:
		{
			p_dec->thumbnail_mode = *((VO_S32*)lValue);
			break;
		}
	default:
		return VO_ERR_WRONG_PARAM_ID;
	}
	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * p_codec_buf)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hDec;

	if (!p_h265dec_front) {
		return VO_ERR_INVALID_ARG;
	}
    VOLOGI("SetInput length %lu, timestamp %lld\n", p_codec_buf->Length, p_codec_buf->Time);

	if (p_codec_buf->Length > RAW_DATA_LEN/2)
	{
		return VO_ERR_INVALID_ARG;
	}

	if (p_codec_buf->Length == 0)
	{
		return VO_ERR_INPUT_BUFFER_SMALL;
	}

	pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
	{
		VO_U8 *inBuf = p_h265dec_front->p_input_buf;
		const VO_U8 *curPos = p_h265dec_front->p_cur_buf_pos;
		const VO_U8 *lastPos = p_h265dec_front->lastPos;
		VO_U8 *endPos = p_h265dec_front->p_cur_buf_pos + p_h265dec_front->left_buf_len;
		assert(endPos - inBuf <= RAW_DATA_LEN);

		if (p_h265dec_front->left_buf_len + p_codec_buf->Length >= RAW_DATA_LEN)
		{
			if (p_h265dec_front->new_input == 0 && p_h265dec_front->left_buf_len >= RAW_DATA_LEN/2)
			{
				curPos = p_h265dec_front->p_cur_buf_pos = inBuf;
				lastPos = p_h265dec_front->lastPos = inBuf;
				endPos = inBuf;
				p_h265dec_front->left_buf_len = 0;
				p_h265dec_front->bFindFirstSlice = 0;
			}
			else
			{
				pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
				return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
			}
		}

		if (endPos + p_codec_buf->Length >= inBuf + RAW_DATA_LEN)
		{
			//copy left data to the head of memory pool
			memmove(inBuf, curPos, p_h265dec_front->left_buf_len);
			p_h265dec_front->lastPos = lastPos -= (curPos - inBuf); 
			p_h265dec_front->p_cur_buf_pos = inBuf;
			curPos = inBuf;
			endPos = inBuf + p_h265dec_front->left_buf_len;
		}

		if (p_h265dec_front->new_input >= MAX_FRAME_INPUT - 1 )
		{
            VOLOGI("SetInput %d ret RETRY\n", p_h265dec_front->new_input);
			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
			pthread_cond_signal(&p_h265dec_front->inputStreamCond);
			p_codec_buf->Length = 0;
			return VO_ERR_RETRY;
//			return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
		}
		CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, endPos, p_codec_buf->Buffer, p_codec_buf->Length);

		{
			//Combine whole frame
			const VO_U8 *last = lastPos;		//last frame end
			const VO_U8 *currFrame = endPos;
			const VO_U8 *nextFrame = endPos;
			VO_U32 leftSize = p_codec_buf->Length;
			VO_U32 inSize;

			//skip until first nalu appear
			while (currFrame < endPos + leftSize - 2)
			{
				if (currFrame[2] > 0x02)//check [2]
				{
					currFrame += 3;
					continue;
				}
				else if (*currFrame==0 && *(currFrame+1)==0 && *(currFrame+2))//check [0] [1] [2]
				{
					break;
				}
				++currFrame;
			}
			if (currFrame < endPos + leftSize - 2)
			{
				leftSize -= currFrame - endPos;
			}
			else
			{
				leftSize = 0;
			}

			while (leftSize>0&&currFrame!=NULL&&p_h265dec_front->new_input < MAX_FRAME_INPUT)
			{
				{
					//1. find next sc
					const VO_U8 *p = currFrame+3;
					const VO_U8* endPos = currFrame+leftSize-2;

					while (p < endPos)
					{
						if (p[2] > 0x02)//check [2]
						{
							p += 3;
							continue;
						}
						else if (*p==0 && *(p+1)==0 && *(p+2))//check [0] [1] [2]
						{
							break;
						}
						++p;
					}
					if (p<endPos)
					{
						inSize = p-currFrame;
						nextFrame = p;
					}
					else
					{
						inSize = leftSize;
						nextFrame = NULL;
					}
				}

				if(inSize >= 6)// at least 3 byte sc + nalu header(2 bytes) + 1 byte in slice header
				{
					const VO_U8 *tmp_ptr = currFrame;
					VO_U32 type;
					VO_U32 value;

					//skip sc
					while(!*tmp_ptr++);

					//2.next nalu type
					type = ((*tmp_ptr)>>1) & 0x3f;
					if (type > NAL_UNIT_CODED_SLICE_CRA) //include reserved types
					{
						//current nalu is not a slice
						if (p_h265dec_front->bFindFirstSlice)
						{
							//find next pic start
							p_h265dec_front->bFindFirstSlice = 0;	//reset
							p_h265dec_front->frameLen[p_h265dec_front->new_input] = currFrame - last;
// LOGI("combine %d\n", currFrame - last);
							last = currFrame;
							p_h265dec_front->new_input++;
// 							if (p_h265dec_front->new_input >= MAX_FRAME_INPUT)
// 							{
// 								break;
// 							}
						}
						leftSize-=inSize;
						currFrame=nextFrame;
						continue;	//not a slice, continue finding next nalu					
					}

					//3.is first slice or not
					value = tmp_ptr[2]&0x80; //first_slice_in_pic_flag
					if (value)
					{
						if (p_h265dec_front->bFindFirstSlice)
						{
							//find next pic start
							p_h265dec_front->frameLen[p_h265dec_front->new_input] = currFrame - last;
// LOGI("find first slice, combine %d\n", currFrame - last);
							last = currFrame;
							p_h265dec_front->new_input++;
// 							if (p_h265dec_front->new_input >= MAX_FRAME_INPUT)
// 							{
// 								p_h265dec_front->bFindFirstSlice = 0;
// 								break;
// 							}

						}
#if ENABLE_DROP_FRAME
						// printf("type %d\n", type);
						if ((((H265_DEC *)(p_h265dec_front->p_dec))->fast_mode & VO_FM_DROP_UNREF) && type == NAL_UNIT_CODED_SLICE_TRAIL_N)
						{
							p_h265dec_front->bCanDrop[p_h265dec_front->new_input] = 1;
						}
						else
						{
							p_h265dec_front->bCanDrop[p_h265dec_front->new_input] = 0;
						}
#endif
						p_h265dec_front->Time[p_h265dec_front->new_input] = p_codec_buf->Time;
						p_h265dec_front->bFindFirstSlice = 1;	//next is first slice of a pic
// LOGI("find first slice\n");
					}

				}
				leftSize-=inSize;
				currFrame=nextFrame;
			}
			p_h265dec_front->lastPos = last;
			p_h265dec_front->left_buf_len += p_codec_buf->Length - leftSize;
			p_h265dec_front->used_buf_len = p_codec_buf->Length - leftSize;

		}
	}
	pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
	pthread_cond_signal(&p_h265dec_front->inputStreamCond);

	//put used len back
	p_codec_buf->Length = p_h265dec_front->used_buf_len;

	if (p_h265dec_front->new_input < MAX_FRAME_INPUT - 1)
	{
        VOLOGI("SetInput %d len %d ret SMALL\n", p_h265dec_front->new_input, p_codec_buf->Length);
		return VO_ERR_INPUT_BUFFER_SMALL;		//need more input
	}

    VOLOGI("SetInput %d len %d ret NONE\n", p_h265dec_front->new_input, p_codec_buf->Length);
	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * p_out_vbuf, VO_VIDEO_OUTPUTINFO * p_out_vbuf_info)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hDec;
	VO_S32 ret = VO_ERR_NONE;
	H265_DEC *p_dec;

	if (!p_h265dec_front || !p_out_vbuf || !p_out_vbuf_info) {
		return VO_ERR_INVALID_ARG;
	}
	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

	//#if ENABLE_LICENSE_CHECK
	//voCheckLibResetVideo(p_h265dec_front->p_lic_handle, p_h265dec_front->p_out_vbuf);
#if ENABLE_SIMPLE_LC
	if(--g_h265dec_limited_num<0)
		return VO_ERR_LICENSE_ERROR;
#endif

    VOLOGI("start getoutputdata\n");
	p_out_vbuf_info->InputUsed = p_h265dec_front->used_buf_len;
	// 	p_h265dec_front->used_buf_len = 0;

// 	if (p_h265dec_front->more_dis_pics || p_dec->bOutputFull)
// 	{
// #if USE_3D_WAVE_THREAD
// 		if (p_dec->nThdNum > 1)
// 		{
// 			ret = DecodeFrameMain_wave3D(p_dec, p_out_vbuf, p_out_vbuf_info, 0);
// 		}
// 		else
// #endif
// 		{
// 			GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
// 		}
// 		p_h265dec_front->more_dis_pics = (p_out_vbuf_info->Format.Type != VO_VIDEO_FRAME_NULL) && p_out_vbuf_info->Flag; //Harry: use type or buffer[0]?
// 
// 		p_out_vbuf_info->Flag = p_out_vbuf_info->Flag || p_h265dec_front->new_input > 0;
// 		// LOGI("GetOutput!!!! buf %p return %d flag %x timestamp %lld\n", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
// 		return VO_ERR_NONE;
// 	}

	if (p_h265dec_front->bNeedFlush && p_h265dec_front->new_input == 0)
	{
		//set the flush cmd to decoder
		pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
		//set flush cmd to decoder
		p_h265dec_front->runCMD = 2;
		pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
		pthread_cond_signal(&p_h265dec_front->inputStreamCond);
		pthread_cond_signal(&p_h265dec_front->getPicCond);

        VOLOGI("wait for flush\n");
		//wait for decoder finish flush
		while (p_h265dec_front->runCMD == 2)
		{
			;
		}
        VOLOGI("flushed some thing\n");

		if (!p_dec->bOutputFull)
		{
			p_h265dec_front->bNeedFlush = 0;
            VOLOGI("flush to display done\n");
		}
	}

    VOLOGI("try output\n");
	pthread_mutex_lock(&p_dec->disListMutex);
	while (1)
	{
		p_out_vbuf_info->Flag = 0;

		GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
        VOLOGI("after GetDisPic, flag %d, new input %d", p_out_vbuf_info->Flag, p_h265dec_front->new_input);
		if (!p_h265dec_front->bNeedFlush)
		{
			pthread_mutex_lock(&p_h265dec_front->inputStreamMutex);
			p_out_vbuf_info->Flag = (p_h265dec_front->new_input<<16) | (p_out_vbuf_info->Flag<<16>>16);
			p_out_vbuf_info->Flag = !!(p_out_vbuf_info->Flag);
			pthread_mutex_unlock(&p_h265dec_front->inputStreamMutex);
		}
        VOLOGI("Type:%x,flay:%d\n",p_out_vbuf_info->Format.Type,p_out_vbuf_info->Flag);
		if (p_out_vbuf_info->Format.Type == VO_VIDEO_FRAME_NULL && p_out_vbuf_info->Flag)
		{
		    VOLOGI("try ouput wait\n");
			pthread_cond_wait(&p_dec->disListCond, &p_dec->disListMutex);
		}
		else
		{
		    VOLOGI("try ouput break\n");
			break;
		}
	}
	pthread_mutex_unlock(&p_dec->disListMutex);
    VOLOGI("try get output end\n");


// 	if ((p_dec->nThdNum <= 1) && (p_dec->thumbnail_mode == 1))
// 	{
// 		// output the frame data
// 		p_out_vbuf_info->Flag  = 1;
// 		p_out_vbuf_info->Format.Width   = p_dec->sps->pic_width_in_luma_samples-p_dec->sps->conf_win_right_offset -p_dec->sps->conf_win_left_offset;//->p_out_pic->m_iPicWidth;
// 		p_out_vbuf_info->Format.Height  = p_dec->sps->pic_height_in_luma_samples-p_dec->sps->conf_win_bottom_offset-p_dec->sps->conf_win_top_offset;//p_out_pic->m_iPicHeight;
// 		p_out_vbuf_info->Format.Type    = (VO_VIDEO_FRAMETYPE)p_dec->slice.cur_pic->pic_type;
// 		p_out_vbuf->Buffer[0]			= p_dec->slice.cur_pic->pic_buf[ 0 ];
// 		p_out_vbuf->Buffer[1]			= p_dec->slice.cur_pic->pic_buf[ 1 ];
// 		p_out_vbuf->Buffer[2]			= p_dec->slice.cur_pic->pic_buf[ 2 ];
// 		p_out_vbuf->Stride[0]			= p_dec->slice.cur_pic->pic_stride[ 0 ];
// 		p_out_vbuf->Stride[1]			= p_dec->slice.cur_pic->pic_stride[ 1 ];
// 		p_out_vbuf->Stride[2]			= p_dec->slice.cur_pic->pic_stride[ 2 ];
// 		p_out_vbuf->Time				= p_dec->slice.cur_pic->Time;
// 		p_out_vbuf->ColorType           = VO_COLOR_YUV_PLANAR420;
// 
// 		p_dec->thumbnail_mode = 0;
// 		return VO_ERR_NONE;
// 	} 

	p_out_vbuf_info->Flag = p_out_vbuf_info->Flag || p_h265dec_front->new_input || p_h265dec_front->bNeedFlush;
// 	if (p_h265dec_front->bNeedFlush)
// 	{
// 		p_out_vbuf_info->Flag = 1;
// 	}

    VOLOGI("GetOutput!!!! buf %p return %ld flag %lx timestamp %lld\n", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
	if (p_out_vbuf_info->Format.Type == VO_VIDEO_FRAME_NULL && ret==0 && p_out_vbuf_info->Flag == 0)
		return VO_ERR_INPUT_BUFFER_SMALL; //Harry: VO_ERR_INPUT_BUFFER_SMALL or VO_ERR_NONE ?

	//fprintf(outdebug, "\n out time = %d, used len = %d, type = %d, moredata = %d", (VO_S32)p_out_vbuf->Time, (VO_S32)p_out_vbuf_info->InputUsed, (VO_S32)p_out_vbuf_info->Format.Type, (VO_S32)p_out_vbuf_info->Flag);


	return ret;

}
#endif

VO_S32 VO_API yyGetHEVCDecFunc (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
{
	VO_VIDEO_DECAPI *p_h265_dec = pDecHandle;

	if (!p_h265_dec) {
		return VO_ERR_INVALID_ARG;
	}

	p_h265_dec->Init				= H265DecInit;
	p_h265_dec->Uninit				= H265DecUinit;
	p_h265_dec->SetParam			= H265DecSetParameter;
	p_h265_dec->GetParam			= H265DecGetParameter;
	p_h265_dec->SetInputData		= H265DecSetInputData;
	p_h265_dec->GetOutputData	    = H265DecGetOutputData;

	return VO_ERR_NONE;

}
