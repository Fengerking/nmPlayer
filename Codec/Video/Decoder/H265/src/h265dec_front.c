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
extern VO_VOID xWriteOutput(/*H265_DEC *p_dec,*/ H265_DPB_LIST* pcListPic, VO_S32 reorder_num );

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
#if USE_NEW_INPUT
		p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
		p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
		p_h265dec_front->left_buf_len = 0;
		p_h265dec_front->used_buf_len = 0;
		p_h265dec_front->bFindFirstSlice = 0;
		p_h265dec_front->more_dis_pics = 0;
		p_h265dec_front->bNeedFlush = 0;	
#endif
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

#if USE_FRAME_THREAD
	if (p_dec->nThdNum > 1)
	{
		ReleaseH265Threads(p_dec);
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

#if !USE_NEW_INPUT
	VO_S32 used_len;
#endif

  if(p_h265dec_front == NULL){
    VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
  }

	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

  VOLOGI(" nID = 0x%x", nID);

	switch(nID) {
    case VO_PID_VIDEO_THUMBNAIL_MODE:
        {
            p_dec->thumbnail_mode = *((VO_S32*)lValue);
            break;
        }
	case VO_PID_COMMON_HEADDATA:
		{
			VO_CODECBUFFER *p_codec_buf = (VO_CODECBUFFER *)lValue;
			VO_U8* p_copy;

			if (p_codec_buf->Length == 0)
			{
        VOLOGE(" <=== !!!VO_ERR_INPUT_BUFFER_SMALL");
				return VO_ERR_INPUT_BUFFER_SMALL;
			}

			p_copy = AlignMalloc(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id, sizeof(VO_U8) * p_codec_buf->Length, CACHE_LINE);
			if (!p_copy)
			{
        VOLOGE(" <=== !!!VO_ERR_OUTOF_MEMORY");
				return VO_ERR_OUTOF_MEMORY;
			}

			CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_copy, p_codec_buf->Buffer, p_codec_buf->Length);
#if USE_NEW_INPUT
			ret = DecodeRawVideo(p_h265dec_front->p_dec, p_copy, p_codec_buf->Length);
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
#else
			p_dec->flush_dis_pics = 1;//YU_TBD, temp solution
			ret = DecodeRawVideo(p_h265dec_front->p_dec, p_copy, p_codec_buf->Length, &used_len);
#endif
			AlignFree(p_dec->user_op_all.p_user_op, p_dec->user_op_all.codec_id,p_copy);
		}
		break;
	case VO_PID_COMMON_FLUSH:
		{
			//Add for clean buffer
			VO_VIDEO_BUFFER outData;
			VO_VIDEO_OUTPUTINFO outFormat;

#if USE_FRAME_THREAD
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain(p_dec, NULL, NULL, 2);
			}
			else
#endif
			{
				xWriteOutput(/*p_dec, */&p_dec->dpb_list, 0);
			}
#if USE_NEW_INPUT
			p_h265dec_front->new_input = 0;
#endif

			//LOGI("pDecoder->bFindFirstSlice:%d,pDecoder->new_input:%d,pDecoder->bHasNext:%d\r\n",pDecoder->bFindFirstSlice,pDecoder->new_input,pDecoder->bHasNext);
			ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
			while(!ret && (outFormat.Format.Type != VO_VIDEO_FRAME_NULL || outFormat.Flag)) //in the end of file, we must get all next output
			{
				ret = H265DecGetOutputData(hCodec,&outData,&outFormat);
			}
			//LOGI("flush poutput:%d returnCode:%x\r\n",pDecGlobal->output_size,returnCode);
#if USE_NEW_INPUT
			//reset input buffer
			p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
			p_h265dec_front->lastPos = p_h265dec_front->p_input_buf;
			p_h265dec_front->left_buf_len = 0;
			p_h265dec_front->used_buf_len = 0;
			p_h265dec_front->bFindFirstSlice = 0;
			p_h265dec_front->new_input = 0;
			p_h265dec_front->more_dis_pics = 0;
#endif
		}
		break;
	case VO_PID_DEC_H265_FLUSH_PICS:
#if USE_NEW_INPUT
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
#if USE_FRAME_THREAD
				p_dec->in_data = p_h265dec_front->p_cur_buf_pos;
				p_dec->data_sz = p_h265dec_front->frameLen[0];
				p_dec->p_avalable_slice->timestamp = p_h265dec_front->Time[0];

				if (p_dec->nThdNum > 1)
				{
					ret = DecodeFrameMain(p_dec,NULL, NULL, 3);
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
#if USE_FRAME_THREAD
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain(p_dec, NULL, NULL, 2);
			}
			else
#endif
			{
				xWriteOutput(/*p_dec, */&p_dec->dpb_list, 0);
			}
		}
#else
		p_dec->frame_num = 0;
		p_dec->flush_dis_pics = *((VO_BOOL*)lValue);
#endif
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
#if USE_FRAME_THREAD
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
#if USE_FRAME_THREAD
				if (p_dec->nThdNum > 1)
				{
					ReleaseH265Threads(p_dec);
				}
				p_dec->nThdNum = nCPU;
				if (p_dec->nThdNum > 1)
				{
					if (nCPU + p_dec->dpb_list.node_num > MAX_DPB_SIZE)
                      return VO_ERR_WRONG_PARAM_ID;
					p_dec->dpb_list.node_num += nCPU;
					InitH265Threads(p_dec);
				}
#endif
			}
		}
		break;
	case VO_PID_COMMON_FRAME_BUF_EX:
    if (*((VO_U32*)lValue) > (MAX_DPB_SIZE  - p_dec->dpb_list.node_num)){
      VOLOGE(" <=== !!!VO_ERR_WRONG_PARAM_ID");
      return VO_ERR_WRONG_PARAM_ID;
    }	
		p_dec->dpb_list.node_num += *((VO_U32*)lValue);
		break;
	case VO_PID_DEC_H265_FASTMODE:
		p_dec->fast_mode = *((VO_S32*)lValue);
#if USE_FRAME_THREAD
		if (p_dec->nThdNum > 1)
		{
		}
		else
#endif
		{
			p_dec->slice.fast_mode = *((VO_S32*)lValue);	//single-core
		}
		break;
	default:
    VOLOGE(" <=== !!!VO_ERR_WRONG_PARAM_ID");
		return VO_ERR_WRONG_PARAM_ID;
	}
  VOLOGI(" <=== ...VO_ERR_NONE");
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
  VOLOGI(" ===> ");

  if(p_h265dec_front == NULL){
    VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
  }

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
    VOLOGE(" <=== !!!VO_ERR_WRONG_PARAM_ID");
		return VO_ERR_WRONG_PARAM_ID;
	}

  VOLOGI(" <=== ...VO_ERR_NONE");
	return VO_ERR_NONE;

}

VO_U32 VO_API H265DecSetInputData(VO_HANDLE hDec, VO_CODECBUFFER * p_codec_buf)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hDec;

  VOLOGI(" ===> ");

	if (!p_h265dec_front) {
    VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
	}

  VOLOGI("H265DecSetInputData length %d, timestamp %lld", p_codec_buf->Length, p_codec_buf->Time);


#if USE_NEW_INPUT
	if (p_codec_buf->Length > RAW_DATA_LEN/2)
	{
    VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
	}

	if (p_codec_buf->Length == 0)
	{
    VOLOGE(" <=== !!!VO_ERR_INPUT_BUFFER_SMALL");
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
        VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
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
      VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
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
#else
	//Save left buffer
	memmove(p_h265dec_front->p_input_buf, p_h265dec_front->p_cur_buf_pos, p_h265dec_front->left_buf_len);//YU_TBD, unified memory operation
	
	if ((p_h265dec_front->left_buf_len + p_codec_buf->Length) > RAW_DATA_LEN) {
		return VO_ERR_INVALID_ARG;//YU_TBD need a new error code
	}

	// Save new
	CopyMem(p_h265dec_front->p_user_op, p_h265dec_front->codec_id, p_h265dec_front->p_input_buf + p_h265dec_front->left_buf_len, p_codec_buf->Buffer, p_codec_buf->Length);//YU_TBD
	
	p_h265dec_front->p_cur_buf_pos = p_h265dec_front->p_input_buf;
	p_h265dec_front->left_buf_len += p_codec_buf->Length;
	p_h265dec_front->input_len     = p_codec_buf->Length;
#endif

  VOLOGI(" <=== ...VO_ERR_NONE");
	return VO_ERR_NONE;
}

VO_U32 VO_API H265DecGetOutputData(VO_HANDLE hDec, VO_VIDEO_BUFFER * p_out_vbuf, VO_VIDEO_OUTPUTINFO * p_out_vbuf_info)
{
	H265_DEC_FRONT* p_h265dec_front = (H265_DEC_FRONT*)hDec;
	VO_S32 ret = VO_ERR_NONE;
	H265_DEC *p_dec;

  VOLOGI(" ===> ");

	if (!p_h265dec_front || !p_out_vbuf || !p_out_vbuf_info) {
    VOLOGE(" <=== !!!VO_ERR_INVALID_ARG");
		return VO_ERR_INVALID_ARG;
	}
	p_dec = (H265_DEC *)p_h265dec_front->p_dec;

#if ENABLE_LICENSE_CHECK
	voCheckLibResetVideo(p_h265dec_front->p_lic_handle, p_h265dec_front->p_out_vbuf);
#elif ENABLE_SIMPLE_LC
	if(--g_h265dec_limited_num<0)
		return VO_ERR_LICENSE_ERROR;
#endif


#if USE_NEW_INPUT

	p_out_vbuf_info->InputUsed = p_h265dec_front->used_buf_len;
	// 	p_h265dec_front->used_buf_len = 0;

	if (p_h265dec_front->more_dis_pics)
	{
#if USE_FRAME_THREAD
		if (p_dec->nThdNum > 1)
		{
			ret = DecodeFrameMain(p_dec, p_out_vbuf, p_out_vbuf_info, 0);
		}
		else
#endif
		{
			GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
		}
		p_h265dec_front->more_dis_pics = (p_out_vbuf_info->Format.Type != VO_VIDEO_FRAME_NULL) && p_out_vbuf_info->Flag; //Harry: use type or buffer[0]?

		p_out_vbuf_info->Flag = p_out_vbuf_info->Flag || p_h265dec_front->new_input > 0;
		// LOGI("GetOutput!!!! buf %p return %d flag %x timestamp %lld\n", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
		VOLOGI("buf %p return %d flag %x timestamp %lld", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
    VOLOGI(" <=== ...VO_ERR_NONE");
    return VO_ERR_NONE;
	}

	p_out_vbuf_info->Flag = 0;
	if (p_h265dec_front->new_input > 0)
	{
startNewInput:
#if USE_FRAME_THREAD
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
        VOLOGI(" <=== ...VO_ERR_DROPPEDFRAME");
				return VO_ERR_DROPPEDFRAME;
			}
		}
		if (p_h265dec_front->new_input > 0)
		{
#endif
			p_dec->in_data = p_h265dec_front->p_cur_buf_pos;
			p_dec->data_sz = p_h265dec_front->frameLen[0];
			p_dec->p_avalable_slice->timestamp = p_h265dec_front->Time[0];
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

#if USE_FRAME_THREAD
	if (p_dec->nThdNum > 1)
	{
		// LOGI("DecodeFrameMain start");
		ret = DecodeFrameMain(p_dec,p_out_vbuf, p_out_vbuf_info, p_h265dec_front->new_input > 0);
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
        p_out_vbuf_info->Format.Type    = p_dec->slice.cur_pic->pic_type;
        p_out_vbuf->Buffer[0]			= p_dec->slice.cur_pic->pic_buf[ 0 ];
        p_out_vbuf->Buffer[1]			= p_dec->slice.cur_pic->pic_buf[ 1 ];
        p_out_vbuf->Buffer[2]			= p_dec->slice.cur_pic->pic_buf[ 2 ];
        p_out_vbuf->Stride[0]			= p_dec->slice.cur_pic->pic_stride[ 0 ];
        p_out_vbuf->Stride[1]			= p_dec->slice.cur_pic->pic_stride[ 1 ];
        p_out_vbuf->Stride[2]			= p_dec->slice.cur_pic->pic_stride[ 2 ];
        p_out_vbuf->Time				= p_dec->slice.cur_pic->Time;
        p_out_vbuf->ColorType           = VO_COLOR_YUV_PLANAR420;

        p_dec->thumbnail_mode = 0;
        VOLOGI("buf %p return %d flag %x timestamp %lld", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
        VOLOGI(" <=== ...VO_ERR_NONE");
        return VO_ERR_NONE;
    } 
	if (p_h265dec_front->new_input > 0 && (p_out_vbuf_info->Format.Type == VO_VIDEO_FRAME_NULL || ret != 0))
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
#if USE_FRAME_THREAD
			if (p_dec->nThdNum > 1)
			{
				DecodeFrameMain(p_dec, NULL, NULL, 2);
			}
			else
#endif
			{
				xWriteOutput(/*p_dec, */&p_dec->dpb_list, 0);
			}
		}
		p_out_vbuf_info->Flag = 1;
		p_h265dec_front->bNeedFlush = 0;
	}

	p_out_vbuf_info->Flag = !!(p_out_vbuf_info->Flag);

	// LOGI("GetOutput!!!! buf %p return %d flag %x timestamp %lld\n", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
  if (p_out_vbuf_info->Format.Type == VO_VIDEO_FRAME_NULL && ret==0 && p_out_vbuf_info->Flag == 0){
    VOLOGI(" <=== ...VO_ERR_INPUT_BUFFER_SMALL");
		return VO_ERR_INPUT_BUFFER_SMALL; //Harry: VO_ERR_INPUT_BUFFER_SMALL or VO_ERR_NONE ?
  }

#else
	//smaller than start code and no dis pic 
	if (p_h265dec_front->more_dis_pics > 0 ) {
		goto GET_DIS_PIC;
	}

	p_out_vbuf_info->Flag = 0;// 
	//Decode One pic
	ret = DecodeRawVideo(p_dec, p_h265dec_front->p_cur_buf_pos, p_h265dec_front->left_buf_len, &used_len);

	/* No matter error, just update input buf,if VO_ERR_INPUT_BUFFER_SMALL,  used_len return 0*/
	p_h265dec_front->left_buf_len				-= used_len;
	p_h265dec_front->p_cur_buf_pos				+= used_len;

	//p_h265dec_front->used_buf_len	+= used_len;

	if (CHECK_API_ERR(ret)) {
		return ret;
	}

GET_DIS_PIC:
	GetDisPic(p_dec, p_out_vbuf, p_out_vbuf_info);
	p_h265dec_front->more_dis_pics   = p_out_vbuf_info->Flag;// more_dis_pics

	p_out_vbuf_info->InputUsed		 = p_h265dec_front->input_len ;//need to return totally used length
#endif

#if ENABLE_LICENSE_CHECK
	p_h265dec_front->p_out_vbuf = p_out_vbuf;/* backup for next voCheckLibResetVideo*/
	if(p_out_vbuf_info->Format.Type != VO_VIDEO_FRAME_NULL)
		voCheckLibCheckVideo (p_h265dec_front->p_lic_handle, p_out_vbuf, &p_out_vbuf_info->Format);
#endif


	
  VOLOGI("buf %p return %d flag %x timestamp %lld", p_out_vbuf->Buffer[0], ret, p_out_vbuf_info->Flag, p_out_vbuf->Time);
  VOLOGI(" <=== ...VO_ERR_NONE");
	return ret;
}

VO_S32 VO_API voGetH265DecAPI (VO_VIDEO_DECAPI * pDecHandle, VO_U32 uFlag)
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
