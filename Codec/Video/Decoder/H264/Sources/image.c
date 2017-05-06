

#if !BUILD_WITHOUT_C_LIB
#include <setjmp.h>

#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#endif

#ifdef _WIN32_WINNT
#include <sys/timeb.h>
#endif //_WIN32_WINNT

#include "global.h"

//#include "H264_C_Type.h"
#include "avd_neighbor.h"
#include "errorconcealment.h"
#include "image.h"
#include "mbuffer.h"
#include "fmo.h"
#include "nalu.h"
#include "parsetcommon.h"
#include "parset.h"
#include "header.h"
#include "rtp.h"
#include "sei.h"
#include "output.h"
#include "biaridecod.h"
//#include "mb_access.h"
#include "memalloc.h"

#include "context_ini.h"
#include "cabac.h"

#include "vlc.h"
#include "h264VdDump.h"

#if CALC_THUMBNAIL
#ifdef LINUX
#include <stdio.h>
#include <sys/time.h>
#endif
#endif
#if DUMP_VERSION
Bitstream *debugBitStream;
int				_dumpSkipped = 0;
int				_dumpStartFrameNo = 19;
int				_dumpEndFrameNo = 17;
#define DISABLE_DUMP_YUV 0
#endif //DUMP_VERSION
#ifndef VOI_H264D_BLOCK_EC
#include "erc_api.h"
extern objectBuffer_t *erc_object_list;
extern ercVariables_t *erc_errorVar;
extern frame erc_recfr;
extern int erc_mvperMB;
extern struct img_par *erc_img;
#endif // VOI_H264D_BLOCK_EC
//int FrameCount=0;
#if ERROR_CHECK
//jmp_buf decode_slice_jmp;
//jmp_buf read_slice_jmp;
//int  read_slice_error = 0;
#endif

void StoreErrorFrame(ImageParameters *img, int error);



void preprocess_one_pic(ImageParameters *img)
{
    Slice *currSlice = img->currentSlice;
    img->current_slice_nr = 0;
    //currSlice->next_header = -1; // initialized to an impossible value for debugging -- correct value is taken from slice header

}
void setup_slice_methods(ImageParameters *img)
{
    Slice *currSlice = img->currentSlice;
    TFuncInfo	*funcInfo = img->funcInfo;

#if FEATURE_CABAC
    if(img->active_pps->entropy_coding_mode_flag)
    {
        funcInfo->read_CBP_and_coeffs_from_NAL = read_CBP_and_coeffs_from_NAL_CABAC;
        switch (img->type)
        {
#if FEATURE_BFRAME
        case B_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_B;
            funcInfo->read_motion_info_from_NAL = read_motion_info_from_NAL_pb_slice_CABAC;
            funcInfo->read_one_macroblock       = read_one_macroblock_pb_slice_CABAC;
            ////img->decode_one_macroblock      = decode_one_macroblock;
            break;
#endif//FEATURE_BFRAME
        case P_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_P;
            funcInfo->read_motion_info_from_NAL = read_motion_info_from_NAL_pb_slice_CABAC;
            funcInfo->read_one_macroblock       = read_one_macroblock_pb_slice_CABAC;
            ////img->decode_one_macroblock      = decode_one_macroblock;
            break;

        case I_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_I;
            funcInfo->read_motion_info_from_NAL = NULL;
            funcInfo->read_one_macroblock       = read_one_macroblock_i_slice_CABAC;
            //img->decode_one_macroblock      = decode_one_macroblock;
            break;
#if FEATURE_EXPROFILE
        case SP_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_P;
            funcInfo->read_motion_info_from_NAL = read_motion_info_from_NAL_p_slice;
            funcInfo->read_one_macroblock       = read_one_macroblock_pb_slice;
            //img->decode_one_macroblock      = decode_one_macroblock_sp_slice;
            break;
        case SI_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_SI;
            funcInfo->read_motion_info_from_NAL = NULL;
            funcInfo->read_one_macroblock       = read_one_macroblock_i_slice;
            //img->decode_one_macroblock      = decode_one_macroblock;
            break;
#endif//FEATURE_EXPROFILE
        default:
            AVD_ERROR_CHECK2(img, "Unsupported slice type\n", ERROR_InvalidBitstream);
            break;
        }
    }
    else
#endif//FEATURE_CABAC
    {
        funcInfo->read_CBP_and_coeffs_from_NAL = read_CBP_and_coeffs_from_NAL_CAVLC;
        switch (img->type)
        {
#if FEATURE_BFRAME
        case B_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_B;
            funcInfo->read_motion_info_from_NAL = read_motion_info_from_NAL_pb_slice_CAVLC;
            funcInfo->read_one_macroblock       = read_one_macroblock_pb_slice_CAVLC;
            //img->decode_one_macroblock      = decode_one_macroblock;
            break;
#endif//FEATURE_BFRAME
        case P_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_P;
            funcInfo->read_motion_info_from_NAL = read_motion_info_from_NAL_pb_slice_CAVLC;
            funcInfo->read_one_macroblock       = read_one_macroblock_pb_slice_CAVLC;
            //img->decode_one_macroblock      = decode_one_macroblock;
            break;

        case I_SLICE:
            funcInfo->interpret_mb_mode         = interpret_mb_mode_I;
            funcInfo->read_motion_info_from_NAL = NULL;
            funcInfo->read_one_macroblock       = read_one_macroblock_i_slice_CAVLC;
            //img->decode_one_macroblock      = decode_one_macroblock;
            break;
        default:
            AVD_ERROR_CHECK2(img, "Unsupported slice type\n", ERROR_InvalidBitstream);
            break;
        }
    }

}
void process_one_slice(ImageParameters *img, NALU_t *nalu)
{
    avdNativeInt current_header, errorID;

#if ERROR_RET_CHECK
#define ERROR_RET_PROCESS_SLICE	if(img->error) goto EXIT_ERROR_RET;
#else//ERROR_RET_CHECK
#define ERROR_RET_PROCESS_SLICE
#endif//ERROR_RET_CHECK

    int start;
    TIME_BEGIN(start)
    trace_overflow(img);

    read_new_slice(img, nalu);

    trace_overflow(img);
    ERROR_RET_PROCESS_SLICE;
    TIME_END(start, readSliceSum)	

#if DISABLE_DECODE
    return;
#endif//
#if DISABLE_DB
    img->vdLibPar->disableDeblock = 1;
#endif
    trace_overflow(img);

    decode_slice(img, current_header);

    TIME_END(start, total)
    trace_overflow(img);

    ERROR_RET_PROCESS_SLICE;
    if (img->current_mb_nr > img->sizeInfo->PicSizeInMbs - 1)
    {
        postprocess_one_pic(img);
    }


#if ERROR_RET_CHECK
EXIT_ERROR_RET:
    if(img->error < 0 )
    {
#ifdef USE_JOBPOOL
	   if(img->vdLibPar->multiCoreNum > 1)
	       restart_cores(img);
#endif

        StoreErrorFrame(img, img->error);
        //InitNewPicProcessor(img);
        img->dec_picture = NULL;
        img->current_mb_nr = -4712;
        img->current_slice_nr = 0;
        return;
    }
#endif//ERROR_RET_CHECK		
}

void postprocess_one_pic(ImageParameters *img)
{
    uninit_frame(img);

#if !BUILD_WITHOUT_C_LIB
#if (DUMP_VERSION & DUMP_BITS)
    if (_dmpBitsFp)
        fclose(_dmpBitsFp);
#endif //(DUMP_VERSION & DUMP_BITS)
#if (DUMP_VERSION & DUMP_DEBLOCK)
    if (_dmpDblkFp[0])
        fclose(_dmpDblkFp[0]);
    _dmpDblkFp[0] = NULL;
    if (img->vdLibPar->multiCoreNum >= 2 && _dmpDblkFp[1])
        fclose(_dmpDblkFp[1]);
    _dmpDblkFp[1] = NULL;
#endif //(DUMP_VERSION & DUMP_DEBLOCK)
#endif //!BUILD_WITHOUT_C_LIB
}






/*!
 ************************************************************************
 * \brief
 *    Interpolation of 1/4 subpixel
 ************************************************************************
 */



/*!
 ************************************************************************
 * \brief
 *    A little helper for the debugging of POC code
 * \return
 *    none
 ************************************************************************
 */
void AvdLogPOC(ImageParameters *img, NALU_t *nalu)
{
    TPOCInfo *pocInfo = img->pocInfo;
    TSpecitialInfo	*specialInfo = img->specialInfo;
#if (DUMP_VERSION & DUMP_SEQUENCE)
    AvdLog(DUMP_SEQUENCE, 0, "\n\n  ...POC locals...");
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:nal_reference_idc", nalu->nal_reference_idc);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:currPoc", pocInfo->currPoc);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:framepoc", pocInfo->currPoc);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:toppoc", pocInfo->currPoc);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:bottompoc", pocInfo->currPoc);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:frame_num", specialInfo->frame_num);
#if FEATURE_INTERLACE
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:field_pic_flag", img->field_pic_flag);

    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:bottom_field_flag", specialInfo->bottom_field_flag);
#endif //VOI_H264D_NON_BASELINE
    AvdLog(DUMP_SEQUENCE, 0, "\n  ...POC in SLice Header...");
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:pic_order_present_flag", img->active_pps->pic_order_present_flag);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d,%d,%d:pocInfo->delta_pic_order_cnt[0]&[1]&[2]", pocInfo->delta_pic_order_cnt[0],
           pocInfo->delta_pic_order_cnt[1], pocInfo->delta_pic_order_cnt[2]);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:idr_flag", specialInfo->idr_flag);
    AvdLog(DUMP_SEQUENCE, 0, "\n%6d:MaxFrameNum", specialInfo->MaxFrameNum);
#endif //(DUMP_VERSION & DUMP_SEQUENCE)
}

void AvdLogSliceHeader(ImageParameters *img, avdNativeInt logId, Slice *currSlice, NALU_t *nalu, Bitstream *currStream)
{
#if ((DUMP_VERSION & DUMP_SEQUENCE) | (DUMP_VERSION & DUMP_SLICE))
    TSizeInfo	*sizeInfo = img->sizeInfo;
    pic_parameter_set_rbsp_t *active_pps = img->active_pps;
    seq_parameter_set_rbsp_t *active_sps = img->active_sps;
    TPOCInfo *pocInfo = img->pocInfo;
    TSpecitialInfo	*specialInfo = img->specialInfo;
    //TMBBitStream	*mbBits = GetMBBits(currMB);
    TCABACInfo	*cabacInfo = img->cabacInfo;
    AvdLog(logId, 0, "\n\n***** %s %s(%s = %d %s, slc# = %d) ***********",
           (currSlice->structure == FRAME ? "Frame" : currSlice->structure == TOP_FIELD ? "TOP_FIELD" : "BOTTOM_FIELD"),
           (nalu->nal_unit_type == NALU_TYPE_IDR) ? "NALU_TYPE_IDR" : "NALU_TYPE_SLICE",
           "PicDisplaceNum", (avdNativeInt)img->ioInfo->sliceNum,
           ((currSlice->picture_type == I_SLICE) ? "I" : (currSlice->picture_type == P_SLICE) ? "P" :
            (currSlice->picture_type == B_SLICE) ? "B" : (currSlice->picture_type == SI_SLICE) ? "SI" : "SP"),
           img->current_slice_nr);

    AvdLog(logId, 0, "\n%6d:currSlice->start_mb_nr", currSlice->start_mb_nr);
    AvdLog(logId, 0, "\n%6d:currSlice->pic_parameter_set_id", currSlice->pic_parameter_set_id);
    AvdLog(logId, 0, "\n%6d:img->frame_num", specialInfo->frame_num);
#if FEATURE_INTERLACE
    AvdLog(logId, 0, "\n%6d:img->field_pic_flag", img->field_pic_flag);
    AvdLog(logId, 0, "\n%6d:img->MbaffFrameFlag", GetMbAffFrameFlag(img));
#endif//#if FEATURE_INTERLACE
    AvdLog(logId, 0, "\n%6d:img->idr_pic_id", specialInfo->idr_pic_id);
    if (active_sps->pic_order_cnt_type == 0)
    {
        AvdLog(logId, 0, "\n%6d:img->pic_order_cnt_lsb", pocInfo->PicOrderCntLsb);
        AvdLog(logId, 0, "\n%6d:pocInfo->delta_pic_order_cnt_bottom", pocInfo->delta_pic_order_cnt_bottom);
    }

    if (active_sps->pic_order_cnt_type == 1 && !active_sps->delta_pic_order_always_zero_flag)
    {
        AvdLog(logId, 0, "\n%6d:pocInfo->delta_pic_order_cnt[0]", pocInfo->delta_pic_order_cnt[0]);
        if(active_pps->pic_order_present_flag == 1
#if FEATURE_INTERLACE
                && !img->field_pic_flag
#endif//#if FEATURE_INTERLACE
          )
            AvdLog(logId, 0, "\n%6d:pocInfo->delta_pic_order_cnt[1]", pocInfo->delta_pic_order_cnt[1]);
    }
    else if (active_sps->pic_order_cnt_type == 1)
    {
        AvdLog(logId, 0, "\n%6d:pocInfo->delta_pic_order_cnt[0]", pocInfo->delta_pic_order_cnt[0]);
        AvdLog(logId, 0, "\n%6d:pocInfo->delta_pic_order_cnt[1]", pocInfo->delta_pic_order_cnt[1]);
    }


    if (active_pps->redundant_pic_cnt_present_flag)
        AvdLog(logId, 0, "\n%6d:img->redundant_pic_cnt", specialInfo->redundant_pic_cnt);

    if(img->type == B_SLICE)
        AvdLog(logId, 0, "\n%6d:img->direct_type", img->currentSlice->direct_type - B_DIRECT_TEMPORAL);

    AvdLog(logId, 0, "\n%6d:img->qp", img->qp);
    AvdLog(logId, 0, "\n%6d:img->num_ref_idx_l0_active", img->num_ref_idx_l0_active);
#if FEATURE_BFRAME
    AvdLog(logId, 0, "\n%6d:img->num_ref_idx_l1_active", img->num_ref_idx_l1_active);
    AvdLog(logId, 0, "\n%6d:img->model_number", cabacInfo->model_number);

    if(img->type == SP_SLICE || img->type == SI_SLICE)
    {
        //AvdLog(logId, "\n%6d:img->qpsp", 0);
        //AvdLog(logId, "\n%6d:img->sp_switch", cabacInfo->sp_switch);
    }

    if (active_pps->num_slice_groups_minus1 > 0
#ifndef VOI_H264D_BLOCK_FMO
            && active_pps->slice_group_map_type >= 3 &&
            active_pps->slice_group_map_type <= 5
#endif
       )
        //AvdLog(logId, "\n%6d:img->slice_group_change_cycle", img->slice_group_change_cycle);
#endif //VOI_H264D_NON_BASELINE

        if (active_pps->deblocking_filter_control_present_flag)
        {
            AvdLog(logId, 0, "\n%6d:currSlice->LFDisableIdc", currSlice->LFDisableIdc);
            if (currSlice->LFDisableIdc != 1)
            {
                AvdLog(logId, 0, "\n%6d:currSlice->LFAlphaC0Offset", currSlice->LFAlphaC0Offset);
                AvdLog(logId, 0, "\n%6d:currSlice->LFBetaOffset", currSlice->LFBetaOffset);
            }
            else
                AvdLog(logId, 0, "\n%6d:currSlice->LFAlphaC0Offset", currSlice->LFAlphaC0Offset);
        }


    AvdLog(logId, 0, "\n%6d:img->PicHeightInMbs", sizeInfo->PicHeightInMbs);
    AvdLog(logId, 0, "\n%6d:img->PicSizeInMbs", sizeInfo->PicSizeInMbs);
    AvdLog(logId, 0, "\n%6d:img->FrameSizeInMbs", sizeInfo->FrameSizeInMbs);
#endif //((DUMP_VERSION & DUMP_SEQUENCE) | (DUMP_VERSION & DUMP_SLICE))
}

/*!
 ************************************************************************
 * \brief
 *    Reads new slice from bit_stream
 ************************************************************************
 */

#define TEST_INPUT 0

#if TEST_INPUT//(DUMP_VERSION & DUMP_SEQUENCE) && (TEST_INPUT)
static void AVDDumpInput(char *buf, int size)
{
#if 0
    int i = nalu->len;
    unsigned char *buf = (unsigned char *) nalu->buf;
    int type = ((nalu->buf[-1]) & 0x1f);
    if(type != 1 && type != 5) //&&type!=7&&type!=8&&type!=6)
        return;
    AvdLog(DUMP_SEQUENCE, 0, "\n*****input data***begin FrameCount=%d\n", FrameCount);
    //fwrite(nalu->buf,nalu->len,1,fp);
    while(i > 0)
    {
        if(i >= 16)
        {
            AvdLog(DUMP_SEQUENCE, 0, "%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x",
                   buf[0], buf[1], buf[2], buf[3],
                   buf[4], buf[5], buf[6], buf[7],
                   buf[8], buf[9], buf[10], buf[11],
                   buf[12], buf[13], buf[14], buf[15]);
            AvdLog(DUMP_SEQUENCE, 0, "\n");
            i -= 16;
            buf += 16;
        }
        else
        {
            while (i)
            {
                AvdLog(DUMP_SEQUENCE, 0, "%2x ", *buf);
                i--;
                buf++;
            }
            break;
        }
    }
    AvdLog(DUMP_SEQUENCE, 0, "\n*****input data***end FrameCount=%d\n", FrameCount);
#else

    AvdInData(buf, size);
#endif
}
#endif
#ifdef X86_TEST
#define TEST_ERR 0
#endif//ARM
#if TEST_ERR


static void AddRandomError2(ImageParameters *img, unsigned char *inBuf, int inBufDataLen)
{
    int iRANDOM1, iRANDOM2;
    int random = inBufDataLen & 7;
    int first  = random + 13;
    int second = random / 2; //first*3/2;
    unsigned char *temp;
    int RANDOM1 = 5;
    int RANDOM2 = 4;
    int ERROR_TEST_LOOP = 1;
    if(img->ioInfo->outNumber < 1)
        return;
    if(ERROR_TEST_LOOP)
    {
        iRANDOM1 = RANDOM1;
        iRANDOM2 = RANDOM2;
        {
            int i;
            int randome1 = inBufDataLen > iRANDOM1 ? iRANDOM1 : inBufDataLen - 1;
            int randome2 = inBufDataLen % (randome1 + 1) ? inBufDataLen % randome1 : iRANDOM2;
            int nSize = (rand () + iRANDOM1) % (randome2 + 1); //iRANDOM2;
            if (nSize >= inBufDataLen)
                nSize = inBufDataLen / 2;
            for (i = 0; i < nSize; i++)
            {
                int nValue = rand () % nSize;
                *(inBuf + nValue) = (rand () >> (iRANDOM1 / 4)) & 0xff;
            }
        }
    }


}
#endif//TEST_ERR
#if DUMP_VERSION
void DumpSlice(ImageParameters *img, NALU_t *nalu)
{
    char fName[256];
    Slice *currSlice = img->currentSlice;
    Bitstream *currStream = currSlice->partArr[0].bitstream;
    //#if(AVD_PLATFORM & AVD_PENTIUM)
    {
        TPOCInfo *pocInfo = img->pocInfo;
        TIOInfo	*ioInfo = img->ioInfo;
        TSizeInfo	*sizeInfo = img->sizeInfo;
        int frameNum = ioInfo->sliceNum;
        TSpecitialInfo	*specialInfo = img->specialInfo;
        //AvdLog2(LL_INFO,"Dump:sliceNum=%d\n",sliceNum-1);
        _dumpSkipped = (frameNum < _dumpStartFrameNo ||
                        frameNum > _dumpEndFrameNo);//_dumpEndFrameNo);

        if (!_dumpSkipped)
        {
#if !BUILD_WITHOUT_C_LIB
            // let user provide log file pointer in AVDLog;
            static int fpTotal = -1;
            fpTotal++;
#if (DUMP_VERSION & DUMP_DEBLOCK)
            if(_dmpDblkFp[0] == NULL)
            {
                if((img->vdLibPar->optFlag & OPT_DUALCORE_DUMP) == 0)
                {
                    sprintf(fName, "%s/dbk_%s_%d_%d_%d.txt", _dmpDblkDir,
                            "fr",
                            frameNum, specialInfo->frame_num, pocInfo->currPoc
                           );
                    _dmpDblkFp[1] = _dmpDblkFp[0] = fopen(fName, "w");
                }
                else
                {
                    sprintf(fName, "%s/dbk_%s_%d_%d_%d_even.txt", _dmpDblkDir,
                            "fr",
                            frameNum, specialInfo->frame_num, pocInfo->currPoc
                           );
                    _dmpDblkFp[0] = fopen(fName, "w");
                    sprintf(fName, "%s/dbk_%s_%d_%d_%d_odd.txt", _dmpDblkDir,
                            "fr",
                            frameNum, specialInfo->frame_num, pocInfo->currPoc
                           );
                    _dmpDblkFp[1] = fopen(fName, "w");
                }

            }

#endif  //(DUMP_VERSION & DUMP_DEBLOCK)

#if (DUMP_VERSION & DUMP_SLICE)
            if(_dmpSlcFp[0] == NULL)
            {
                if((img->vdLibPar->optFlag & OPT_DUALCORE_DUMP) == 0)
                {
                    sprintf(fName, "%s/slice_%s_%d_%d.txt", _dmpSlcDir,
                            "fr", frameNum, pocInfo->currPoc
                           );
                    _dmpSlcFp[1] = _dmpSlcFp[0] = fopen(fName, "w");
                }
                else
                {
                    sprintf(fName, "%s/slice_%s_%d_%d_even.txt", _dmpSlcDir,
                            "fr", frameNum, pocInfo->currPoc
                           );
                    _dmpSlcFp[0] = fopen(fName, "w");
                    sprintf(fName, "%s/slice_%s_%d_%d_odd.txt", _dmpSlcDir,
                            "fr", frameNum, pocInfo->currPoc
                           );
                    _dmpSlcFp[1] = fopen(fName, "w");
                }

            }
#endif //(DUMP_VERSION & DUMP_SLICE)
#if (DUMP_VERSION & DUMP_SLICE)
            if(_dmpSlcFpData[0] == NULL)
            {
#if !DISABLE_DUMP_YUV
                if((img->vdLibPar->optFlag & OPT_DUALCORE_DUMP) == 0)
                {
                    sprintf(fName, "%s/sliceData_%s_%d_%d.txt", _dmpSlcDir,
                            "fr",
                            frameNum, pocInfo->currPoc
                           );
                    _dmpSlcFpData[1] = _dmpSlcFpData[0] = fopen(fName, "w");
                }
                else
                {
                    sprintf(fName, "%s/sliceData_%s_%d_%d_even.txt", _dmpSlcDir,
                            "fr",
                            frameNum, pocInfo->currPoc
                           );
                    _dmpSlcFpData[0] = fopen(fName, "w");
                    sprintf(fName, "%s/sliceData_%s_%d_%d_odd.txt", _dmpSlcDir,
                            "fr",
                            frameNum, pocInfo->currPoc
                           );
                    _dmpSlcFpData[1] = fopen(fName, "w");

                }

#endif
            }
#endif //(DUMP_VERSION & DUMP_SLICE)
#endif //!BUILD_WITHOUT_C_LIB



#if (DUMP_VERSION & DUMP_SLICE)
            memset(_dumpMbMode,  0, 12 * sizeof(int));
            memset(_dumpIP4,  0, 10 * sizeof(int));
            memset(_dumpIP8,  0, 5 * sizeof(int));
            memset(_dumpIP16, 0, 5 * sizeof(int));
            memset(_dumpItransTp, 0, _dumpNumItrans * sizeof(int));
            _dumpIntra4Y0Cof = _dumpIntra4C0Cof =
                                   _dumpIntra4YNon0Cof = _dumpIntra4CNon0Cof = 0;
            AvdLogSliceHeader(img, DUMP_SLICE, currSlice, nalu, currStream);
#endif //(DUMP_VERSION & DUMP_SLICE)
#if (DUMP_VERSION & DUMP_SEQUENCE)
            AvdLogSliceHeader(img, DUMP_SEQUENCE, currSlice, nalu, currStream);
            AvdLogPOC (img, nalu);
#endif
        }
    }
    DumpRefList(img, img->mbsProcessor[0]);

}
#else
#define DumpSlice(a,c)
#endif
avdNativeInt read_new_slice(ImageParameters *img, NALU_t *nalu)
{

    avdInt32 startLen;
    avdNativeInt BitsUsedByHeader;
    Slice *currSlice = img->currentSlice;
    //NALU_t *nalu = currSlice->nalu;
    Bitstream *currStream;

    avdNativeInt slice_id_a, slice_id_b, slice_id_c, IsFlushing = 0;
    avdNativeInt redundant_pic_cnt_b, redundant_pic_cnt_c;
    avdNativeInt expected_slice_type;
    //avdUInt8 *currNalu;
    H264VdLibParam *params = img->vdLibPar;


    TSpecitialInfo	*specialInfo = img->specialInfo;
    TIOInfo	*ioInfo = img->ioInfo;
    TSizeInfo	*sizeInfo = img->sizeInfo;
    int newIDR;


#if TEST_ERR
    AddRandomError2(img, nalu->buf + 2, nalu->len - 2);
#endif
    ioInfo->sliceNum++;
    if(img->active_sps == NULL)
    {
        AVD_ERROR_CHECK(img, "active_sps==NULL 2\n", ERROR_SPSIsNULL);
    }

    if(ioInfo->outNumber == 0 && params->multiCoreNum < 2) //for multi core, always use save_all_mv
    {
        //baseline or only B_SPACIAL
        img->notSaveAllMV = img->active_sps->profile_idc == PRO_BASELINE; //((params->optFlag&OPT_ONLY_B_SPACIAL)||img->active_sps->profile_idc==PRO_BASELINE);//force
    }
    specialInfo->idr_flag = (nalu->nal_unit_type == NALU_TYPE_IDR);
    specialInfo->nal_reference_idc = nalu->nal_reference_idc;
    specialInfo->disposable_flag = (nalu->nal_reference_idc == NALU_PRIORITY_DISPOSABLE);
    currSlice->dp_mode = PAR_DP_1;
    currSlice->max_part_nr = 1;
    currSlice->ei_flag = 0;
    currStream = currSlice->partArr[0].bitstream;
#if (DUMP_VERSION & DUMP_SLICE)
    debugBitStream = currStream ;
#endif
    InitBitStream(currStream, nalu->buf, nalu->len);


    BitsUsedByHeader = FirstPartOfSliceHeader(img, currStream);
    CHECK_ERR_RET_INT
    BitsUsedByHeader = UseParameterSet (img, currSlice->pic_parameter_set_id);
    trace_overflow(img);
    CHECK_ERR_RET_INT
    BitsUsedByHeader = RestOfSliceHeader (img, currStream);
    trace_overflow(img);
    CHECK_ERR_RET_INT
#if FEATURE_INTERLACE
    if (img->currentSlice->structure != 0)
    {
        sizeInfo->height >>= 1 ;
        sizeInfo->height_cr >>= 1;
    }
#endif //VOI_H264D_NON_BASELINE

    if(ioInfo->IsSeekingNextIFrame)
    {
        int gotIframe = params->enableJump2I == 2 ? specialInfo->idr_flag : img->type == I_SLICE;
        if(gotIframe && currSlice->start_mb_nr == 0)
        {
            ioInfo->IsSeekingNextIFrame = 0;
            //goto SKIP_CurrentSlice;
        }
        else if(params->enableJump2I)
        {
            AVD_ERROR_CHECK(img, "seeking next Intra frame\n", -ERROR_FirstFrameIsNotIntra);
        }
    }


    if(ioInfo->sliceNum == 1) //||IsFlushing
    {
        TSpecitialInfo	*specialInfo = img->specialInfo;
        specialInfo->pre_frame_num = specialInfo->frame_num;

        //FrameCount==1 indicates the first frame should be I frame
        if(img->type != I_SLICE)
        {

            if(params->enableFirstFrameNonIntra)
            {
                InitRefBuffer(img);
                CHECK_ERR_RET_INT
                AVD_ERROR_CHECK(img, "the first frame should be Intra frame", ERROR_FirstFrameIsNotIntra);
            }
            else
            {
                ioInfo->sliceNum = 0;
                AVD_ERROR_CHECK(img, "the first frame should be Intra frame", ERROR_InvalidBitstream);
            }

        }

    }

#if FEATURE_MATRIXQUANT
    assign_quant_params (img);
    CHECK_ERR_RET_INT
#endif
    if(currSlice->start_mb_nr == 0) //if(img->lbPicReady)
    {

#ifdef USE_JOBPOOL
//        restart_cores(img);
#endif
        init_frame(img);
        trace_overflow(img);
        CHECK_ERR_RET_INT

#if ENABLE_OTHER_ALLOC
        if(params->customMem2 && params->customMem2->Lock)
        {
            params->customMem2->Lock(img->dec_picture->plnY);
        }
#endif//ENABLE_OTHER_ALLOC				
        img->dec_picture->timestamp	= params->timeStamp;
        //preprocess_one_frame(img);
        //init_out_buffer(img);
        preprocess_one_pic(img);
        init_lists(img);
        trace_overflow(img);
        CHECK_ERR_RET_INT
    }
    else//multiple slice
    {

        //specialInfo->pre_frame_num = specialInfo->frame_num;
        if(img->current_slice_nr == 0) //only when the previous slice is not deocoded succesuffly.
        {

            AVD_ERROR_CHECK(img, "slice is lost1", ERRPR_DecSliceLoss);

        }
        else
        {
            if(img->type != img->dec_picture->typeIPB) //only when the previous slice is lost.
            {
                AVD_ERROR_CHECK(img, "The type of previous slice type is not the one of the current slice", ERRPR_DecSliceLoss);
            }
        }
	 init_lists(img);
        //printf("error: \ncurrent header is Start of slice\n");
    }
#if 0//FEATURE_INTERLACE
    if(img->ioInfo->prevInStructure > 0 && img->dec_picture->structure == img->ioInfo->prevInStructure)
        AVD_ERROR_CHECK(img, "field lost, the curr field is same as the prev one\n", ERRPR_DecSliceLoss);
    img->ioInfo->prevInStructure = img->ioInfo->prevInStructure;
#endif//FEATURE_INTERLACE
    AvdLog2(LL_INOUT, "@!@%d,startMB=%d:num=%d,poc=%d,slice=%d,type=%d,prevMB=%d,structure=%d\n", ioInfo->sliceNum, currSlice->start_mb_nr, specialInfo->frame_num, img->pocInfo->currPoc, img->current_slice_nr, img->type, img->current_mb_nr, img->dec_picture->structure);


    if(params->sharedMem == NULL)
        sizeInfo->yPlnPitch = sizeInfo->width + (Y_PADDING << 1);

    // do this only after height and width are known;
    if (img->clipInfo->clipArrHeight != sizeInfo->height) // assume no height and width changed;
        InitClipArrs(img);
    CHECK_ERR_RET_INT
    if (!GetMbAffFrameFlag(img))
    {

        if(!params->enableParticialDecode)
        {
            if(img->current_mb_nr < 0) //first slice
            {
                if(currSlice->start_mb_nr != 0)
                {
                    AVD_ERROR_CHECK(img, "first slice is lost", ERRPR_DecSliceLoss);
                }
            }
            else
            {
                int diff = currSlice->start_mb_nr - img->current_mb_nr;
                if (diff < 0 || diff > 1)
                {
                    AVD_ERROR_CHECK(img, "non-first slice is lost", ERRPR_DecSliceLoss * ENABLE_STRICT_MB_START_CHECK);
                }
            }
        }
        img->current_mb_nr = currSlice->start_mb_nr;
    }
#if FEATURE_INTERLACE
    else
    {
        if(currSlice->start_mb_nr == 0)
            init_mbaff_lists(img);
        currSlice->start_mb_nr <<= 1;
        img->current_mb_nr = currSlice->start_mb_nr;
    }
#endif//VOI_H264D_NON_BASELINE


#if FEATURE_CABAC
    if (img->active_pps->entropy_coding_mode_flag)
        StartAriDeco(currStream);
#endif
    DumpSlice(img, nalu);

    img->current_slice_nr++;
    return 0;
}

int ProcessOneNALU(ImageParameters *img, int prefixLen)
{


    //img->currentSlice->next_header = -1;
    NALU_t nalu;
    H264VdLibParam *params = img->vdLibPar;

    img->error = 0;
    nalu.buf = params->inBuf + 1;
    nalu.len = params->inBufDataLen - 1;
//    DUMPDataIn((char *)(nalu.buf - 1), nalu.len + 1, prefixLen);

    nalu.forbidden_bit = (nalu.buf[-1] >> 7) & 1;
    nalu.nal_reference_idc = (nalu.buf[-1] >> 5) & 3;
    nalu.nal_unit_type = (nalu.buf[-1]) & 0x1f;
    // Got a NALU
    if (nalu.forbidden_bit)
    {
        AVD_ERROR_CHECK(img, "Found NALU w/ forbidden_bit set, bit error?  Let's try", 100); //ERROR_InvalidBitstream);
    }
    AvdLog2(LL_INOUT, "nalu_size %d\n", params->inBufDataLen);
    //if(nalu.nal_unit_type==NALU_TYPE_IDR)
    //	printf("NALU_TYPE_IDR %d\n",pocInfo->PicOrderCntLsb);
    switch (nalu.nal_unit_type)
    {
    case NALU_TYPE_SLICE:
    case NALU_TYPE_IDR:
        process_one_slice(img, &nalu); // one of the slices in a picture;
        break;
    case NALU_TYPE_PPS:
        AvdLog2(LL_INFO, "%d,ProcessPPS...\n", img->ioInfo->sliceNum);

        ProcessPPS(img, &nalu);
        CHECK_ERR_BREAK
        break;
    case NALU_TYPE_SPS:
        AvdLog2(LL_INFO, "%d,ProcessSPS...\n", img->ioInfo->sliceNum);

        ProcessSPS(img, &nalu);
        CHECK_ERR_BREAK
        break;


    case NALU_TYPE_SEI:

        if(params->enbaleSEI)
        {
            InterpretSEIMessage(img, &nalu);
            CHECK_ERR_BREAK
        }
        else
            AVD_ERROR_CHECK(img, "read_new_slice: ignore SEI\n", 100);
        break;

    case NALU_TYPE_AUD:
        //AVD_ERROR_CHECK(img,"read_new_slice: Found 'Access Unit Delimiter' NAL unit,  ignored", 100);
        break;
    case NALU_TYPE_EOSEQ:
        AVD_ERROR_CHECK(img, "read_new_slice: Found 'End of Sequence' NAL unit, ignored", 100);
        break;
    case NALU_TYPE_EOSTREAM:
        AVD_ERROR_CHECK(img, "read_new_slice: Found 'End of Stream' NAL unit, len, ignored", 100);
        break;
    case NALU_TYPE_FILL:
        AVD_ERROR_CHECK(img, "read_new_slice: Found NALU_TYPE_FILL, ignored", 100);
        break;

    default:
        AVD_ERROR_CHECK(img, "Found NALU type  undefined, ignore NALU, moving on", 100);
        break;
    }

#ifdef USE_JOBPOOL
    if(img->vdLibPar->multiCoreNum > 1 && img->error < 0 && img->is_cores_running)
		restart_cores(img);
#endif

    return img->error;
}
void set_ref_pic_num(ImageParameters *img)
{

    avdNativeUInt i, j;
    StorablePicture	*dec_picture = img->dec_picture;
    if (dec_picture->typeIPB != I_SLICE)
    {
#if FEATURE_BFRAME

        for (i = 0; i < img->listXsize[LIST_0]; i++)
        {
            if(img->listX[LIST_0][i] == NULL)
                AVD_ERROR_CHECK2(img, "no fwd frame\n", ERROR_REFFrameIsNULL);
            dec_picture->ref_pic_num[LIST_0][i] = img->listX[LIST_0][i]->poc * 2
                                                  + ((img->listX[LIST_0][i]->structure == BOTTOM_FIELD) ? 1 : 0) ;

        }

        for (i = 0; i < img->listXsize[LIST_1]; i++)
        {
            if(img->listX[LIST_1][i] == NULL)
                AVD_ERROR_CHECK2(img, "no bwd frame\n", ERROR_REFFrameIsNULL);
            dec_picture->ref_pic_num[LIST_1][i] = img->listX[LIST_1][i]->poc  * 2
                                                  + ((img->listX[LIST_1][i]->structure == BOTTOM_FIELD) ? 1 : 0);

        }
#endif//FEATURE_BFRAME
#if FEATURE_INTERLACE
        if (GetMbAffFrameFlag(img))
        {
            for (j = 2; j < AVD_MOTION_LIST_SIZE; j++)
            {
                for (i = 0; i < img->listXsize[j]; i++)
                {
                    dec_picture->ref_pic_num[j][i] = img->listX[j][i]->poc * 2
                                                     + ((img->listX[j][i]->structure == BOTTOM_FIELD) ? 1 : 0);
                    //dec_picture->frm_ref_pic_num[j][i] = img->listX[j][i]->frame_poc * 2 ;
                    //dec_picture->top_ref_pic_num[j][i] = img->listX[j][i]->top_poc * 2 ;
                    //dec_picture->bottom_ref_pic_num[j][i] = img->listX[j][i]->bottom_poc * 2 + 1;
                }
            }
        }
#endif//VOI_H264D_NON_BASELINE

    }

}
static char *strframeType[] = {"P", "B", "I"};
char *GetStrFrameType(int type)
{
    return strframeType[type];
}
void StoreErrorFrame(ImageParameters *img, int error)
{
    if(1)
    {
        StorablePicture	*dec_picture = img->dec_picture;
        if (dec_picture)
        {
#ifdef USE_JOBPOOL
            int i;
#endif        
            int tmp = img->error;
            img->error = 0;//save it first
            if(error < 0)
                dec_picture->error = OUTPUT_ERROR_FLAG;

            //dec_picture->is_output = 2;
            AvdLog2(LL_INFO, "StoreErrorFrame:%s,frame_num=%d,poc=%d,slice=%d,err=%d\n", GetStrFrameType(img->type), img->specialInfo->frame_num, img->pocInfo->currPoc, img->current_slice_nr, tmp);
            uninit_frame(img);
#ifdef USE_JOBPOOL
#if USE_IMG_MIRROR			
            if(img->vdLibPar->multiCoreNum > 1)
            {
                for(i = 0; i < MAX_SLICE_NUM; i++)
				    img->curr_slice_img[i]->error = tmp;
            }
#endif			
#endif
            img->error = tmp;
        }
    }
}

/*!
 ************************************************************************
 * \brief
 *    Initializes the parameters for a new picture
 ************************************************************************
 */
void init_frame(ImageParameters *img)
{
    SliceType typeIPB;
    avdNativeInt i;
    StorablePicture	*dec_picture = img->dec_picture;
    TPOCInfo *pocInfo = img->pocInfo;
    TSpecitialInfo	*specialInfo = img->specialInfo;
    TSizeInfo	*sizeInfo = img->sizeInfo;
    //TPosInfo*  posInfo  = GetPosInfo(info);
    if (dec_picture)
    {
#ifdef USE_JOBPOOL
        if(img->vdLibPar->multiCoreNum > 1)
	        restart_cores(img);
#endif    
        // this may only happen on slice loss
        //pocInfo->currPoc++;
        AVD_ERROR_CHECK2(img, "the slice of the previous frame(PF) is lost", WARN_PREV_FRAME_SLICE_LOST);

#if 1//TRACE_ERROR_DETAIL
        AvdLog2(LL_INFO, "lost slice:frame type:%s, frameNum:%d,poc:%d, because: slice data losses\n", GetStrFrameType(img->dec_picture->typeIPB), img->dec_picture->frame_num, img->dec_picture->poc);
#endif
        StoreErrorFrame(img, 0);
    }

    //#if ENABLE_STRICT_GAP_CHECK
    if(img->vdLibPar->optFlag & OPT_STRICT_FRAMENUM)
        if (specialInfo->frame_num != specialInfo->pre_frame_num && specialInfo->frame_num != ((specialInfo->pre_frame_num + 1)&(specialInfo->MaxFrameNum - 1)) ) //% specialInfo->MaxFrameNum)
        {
            if (img->active_sps->gaps_in_frame_num_value_allowed_flag == 0)
            {
                /* Advanced Error Concealment would be called here to combat unintentional loss of pictures. */
                AVD_ERROR_CHECK(img, "An unintentional loss of pictures occurs! Exit\n", ERRPR_DecSliceLoss);
            }
            //fill_frame_num_gap();//alll
        }

#define CAL_PREFRAMENUM_EXIT 1
#if !CAL_PREFRAMENUM_EXIT
    specialInfo->pre_frame_num = specialInfo->frame_num;
#endif

    decode_poc(img);
    CHECK_ERR_RET_VOID


    typeIPB = (img->type == I_SLICE || img->type == SI_SLICE) ? I_SLICE :
              (img->type == P_SLICE || img->type == SP_SLICE) ? P_SLICE : B_SLICE;
    dec_picture = img->dec_picture = GetStorablePic(img, specialInfo->structure, typeIPB, sizeInfo->width, sizeInfo->height,
                                     sizeInfo->width_cr, sizeInfo->height_cr, (typeIPB != I_SLICE));
    CHECK_ERR_RET_VOID;
    //assert(dec_picture!=0);
    //assert(dec_picture->motionInfo);

    if(dec_picture == NULL)
    {
        AVD_ERROR_CHECK2(img, "initPic:img->dec_picture->motionInfo==NULL\n", ERROR_NULLPOINT);
    }
    dec_picture->PicSizeInMbs = sizeInfo->PicSizeInMbs;
    dec_picture->top_poc	  = pocInfo->toppoc;
    dec_picture->bottom_poc	  = pocInfo->bottompoc;
    dec_picture->frame_poc    = pocInfo->framepoc;
#if FEATURE_INTERLACE
    dec_picture->structure = specialInfo->structure;
    switch (specialInfo->structure )
    {

    case TOP_FIELD:
    {
        dec_picture->poc = pocInfo->toppoc;

        break;
    }
    case BOTTOM_FIELD:
    {
        dec_picture->poc = pocInfo->bottompoc;

        break;
    }

    case FRAME:
    {
        dec_picture->poc = pocInfo->framepoc;
        break;
    }

    }
#else//baseline
    dec_picture->poc = pocInfo->currPoc;
#endif //VOI_H264D_NON_BASELINE

    img->current_slice_nr = 0;

    if (img->type > SI_SLICE)
    {
        //		set_ec_flag(SE_PTYPE);
        img->type = P_SLICE;  // concealed element
    }



    //posInfo->block_y = posInfo->pix_y = posInfo->pix_c_y = 0; // define vertical positions
    //posInfo->block_x = posInfo->pix_x = posInfo->pix_c_x = 0; // define horizontal positions

    dec_picture->typeIPB = (SliceType) (img->type);
    dec_picture->used_for_reference = (specialInfo->nal_reference_idc != 0);
    dec_picture->idr_flag = specialInfo->idr_flag;
    dec_picture->no_output_of_prior_pics_flag = specialInfo->no_output_of_prior_pics_flag;
    dec_picture->long_term_reference_flag = specialInfo->long_term_reference_flag;
    dec_picture->adaptive_ref_pic_buffering_flag = specialInfo->adaptive_ref_pic_buffering_flag;
    dec_picture->dec_ref_pic_marking_buffer = specialInfo->dec_ref_pic_marking_buffer;
    //specialInfo->dec_ref_pic_marking_buffer = NULL;

    dec_picture->MbaffFrameFlag = GetMbAffFrameFlag(img);
    dec_picture->pic_num = specialInfo->frame_num;
    dec_picture->frame_num = specialInfo->frame_num;
    dec_picture->coded_frame = 1;
    InitNewPicProcessor(img);
    if(img->notSaveAllMV == 0)
        initArray4AllMacroblock(img);
#if FEATURE_BFRAME
    else
    {
        if(img->type != I_SLICE)
            ResetMVRefBuf(img, dec_picture->motionInfo);

    }
#endif//FEATURE_BFRAME

}

void initArray4AllMacroblock(ImageParameters *img)
{
    if (img->type != I_SLICE && img->type != SI_SLICE)
    {
        avdInt32 i, sizeRef, size;
        avdInt32 *tmp32Pt, *tmp32Pt2;
        StorablePicture	*dec_picture = img->dec_picture;
        AVDStoredPicMotionInfo *motionInfo = dec_picture->motionInfo;
        sizeRef = (avdInt32)motionInfo->alignedWidth8x8 * (dec_picture->size_y >> 3)
                  * sizeof(avdUInt8) / sizeof(avdInt32);
        AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx0[0][0], AVD_INVALID_REF_IDX_32, sizeRef);
        size = (((avdInt32)dec_picture->size_x * (avdInt32)dec_picture->size_y) >> 3)
               * sizeof(avdInt16) / sizeof(avdInt32);
        AVD_ALIGNED32_MEMSET(&motionInfo->mv0[0][0], 0,	size);

#if FEATURE_BFRAME
        if (img->type == B_SLICE)
        {
            AVD_ALIGNED32_MEMSET(&motionInfo->ref_idx1[0][0], AVD_INVALID_REF_IDX_32, sizeRef);
            AVD_ALIGNED32_MEMSET(&motionInfo->mv1[0][0], 0,	size);
        }
#endif //VOI_H264D_NON_BASELINE

    }
}



/*!
 ************************************************************************
 * \brief
 *    exit a frame
 ************************************************************************
 */
//uninit_frame2 handle the slice loss,it does not handle deblock

void uninit_frame(ImageParameters *img)
{
    //  avdNativeInt ercStartMB;
    //  avdNativeInt ercSegment;
    avdNativeInt structure, slice_type, refpic;
    StorablePicture	*dec_picture = img->dec_picture;
    H264VdLibParam *params = img->vdLibPar;
    int		preFramesInQueue = img->ioInfo->outDataQueue.frameNum;
    TSpecitialInfo	*specialInfo = img->specialInfo;
    // this should not happen, just to be sure....
    if (dec_picture == NULL)
    {
        return;
    }





    structure  = dec_picture->structure;
    slice_type = dec_picture->typeIPB;
    refpic     = dec_picture->used_for_reference;

    if((params->enbaleSEI & VHSF_GET_T35_USERDDATA) && img->specialInfo->seiBuf.Length)
    {
        AvdLog2(LL_INFO, "T35_in:poc=%d,size=%d\n", img->dec_picture->poc, img->specialInfo->seiBuf.Length);
        copyT35Data(img, &img->dec_picture->seiPicInfo, img->specialInfo->seiBuf.Buffer, img->specialInfo->seiBuf.Length);
        img->dec_picture->seiPicInfo.Time = img->dec_picture->poc;
        img->specialInfo->seiBuf.Length = 0;
    }

    store_picture_in_dpb(img, dec_picture);
    CHECK_ERR_RET_VOID

    if(params->enableOutputASAP && preFramesInQueue == img->ioInfo->outDataQueue.frameNum) //||img->dpb->last_output_poc==VO_INT_MIN))
        Try_output_one_frame_from_dpb(img, dec_picture);
    if (specialInfo->last_has_mmco_5)
    {
        specialInfo->pre_frame_num = 0;
    }

    img->dec_picture = NULL;
    img->current_mb_nr = -4712;   // impossible value for debugging, StW
    img->current_slice_nr = 0;
    ExitNewPicProcessor(img);
#if (DUMP_VERSION & DUMP_SLICE)
    if (_dmpSlcFpData[0])
    {
        TSizeInfo	*sizeInfo = img->sizeInfo;
        //DumpMBBeforeDeblock(img,img->mbsProcessor[0],0,sizeInfo->PicSizeInMbs);

        fclose(_dmpSlcFpData[0]);
        _dmpSlcFpData[0] = NULL;
        if(img->vdLibPar->multiCoreNum >= 2 && _dmpSlcFpData[1])
            fclose(_dmpSlcFpData[1]);
        _dmpSlcFpData[1] = NULL;
    }
    if (_dmpSlcFp[0])
    {
        fclose(_dmpSlcFp[0]);
        if (img->vdLibPar->multiCoreNum >= 2 && _dmpSlcFp[1])
            fclose(_dmpSlcFp[1]);
        _dmpSlcFp[0] = NULL;
        _dmpSlcFp[1] = NULL;
    }
#endif
#if CAL_PREFRAMENUM_EXIT
    if(img->type != B_SLICE)
        specialInfo->pre_frame_num = specialInfo->frame_num;
#endif
}

/*!
 ************************************************************************
 * \brief
 *    write the encoding mode and motion vectors of current
 *    MB to the buffer of the error concealment module.
 ************************************************************************
 */



#if defined(NEON)&&defined(MAC)
#define SAVE_NEON_ON_3GS 1
#endif//
#if SAVE_NEON_ON_3GS
static avdUInt64 tmpNeonReg[8];
#endif//SAVE_NEON_ON_3GS


#if CALC_THUMBNAIL
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
static unsigned long GetTickCount(){
#if 1
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000 + tval.tv_usec/1000;
#else
	return clock();
#endif
}
#endif
#endif
#ifdef USE_JOBPOOL
void decode_slice(ImageParameters *img, avdNativeInt current_header)
{
    Slice *currSlice = img->currentSlice;
    if (img->dec_picture == NULL)
    {
        return;
    }
#if CALC_THUMBNAIL  
       if(img->type == I_SLICE)
       {
           img->decode_ms = 0;
           img->parse_ms = 0;
       }
#endif  

#if FEATURE_CABAC
    if (img->active_pps->entropy_coding_mode_flag)
    {
        //if(currSlice->start_mb_nr==0)
        init_contexts(img);
        cabac_new_slice(img);
    }
#endif
#if FEATURE_WEIGHTEDPRED
    //OPT:only call once
    if (currSlice->start_mb_nr == 0 && img->type == B_SLICE && img->apply_weights)
        fill_wp_params(img);
#endif
    //Init_Slice_Neighbor_Info(img);

    // decode main slice information
    if (1)//current_header == SOP)// && GetEIFlag(currSlice) == 0)
    {
        int l, k;
        TMBsProcessor *info	  ;
        //avdNativeInt end_of_slice = VOI_FALSE;
        Slice *slice = img->currentSlice;
        TPosInfo  *posInfo;
        setup_slice_methods(img);


        info = img->mbsProcessor[0];

        if (img->type == I_SLICE && slice->LFDisableIdc != 1 && !GetMbAffFrameFlag(img))
        {
            avdNativeInt k;
            avdNativeInt		coreNum 		= img->vdLibPar->multiCoreNum > 1 ? img->sizeInfo->FrameHeightInMbs : img->vdLibPar->multiCoreNum;
            avdNativeInt		mbsProcessorNum = max(2, coreNum); //at least we need 2 mbsProcessor
            for (k = 0; k < mbsProcessorNum; k++)
            {
                TMBsProcessor	*info2 = img->mbsProcessor[k];
                AVDIPDeblocker *ipDeblocker = GetIPDeblocker(info2);
                SetInPlaceIntraMBStrength(img, info2, ipDeblocker->strength);
            }

        }

        img->cod_counter = -1;


#if FEATURE_BFRAME
        if(currSlice->start_mb_nr == 0)
            set_ref_pic_num(img);
#if FEATURE_INTERLACE
        if (!GetMbAffFrameFlag(img))
        {
            int structure = GetPicStructure(img);
            for (l = 0; l < 2; l++)
                for(k = 0; k < img->listXsize[l]; k++)
                {
                    if(img->listX[l][k])
                    {
                        img->listX[l][k]->chroma_vector_adjustment = 0;
                        if(structure == TOP_FIELD && structure != img->listX[l][k]->structure)
                            img->listX[l][k]->chroma_vector_adjustment = -2;
                        else if(structure == BOTTOM_FIELD && structure != img->listX[l][k]->structure)
                            img->listX[l][k]->chroma_vector_adjustment = 2;
                    }

                }
        }
#endif//FEATURE_INTERLACE
#endif //VOI_H264D_NON_BASELINE
        InitNewSliceProcessor(img);
        if(img->vdLibPar->multiCoreNum < 2)
        {
            info = GetMainProcessor(img);
            do
            {
                processmbsSingleCore(img, info);
                CHECK_ERR_BREAK
                if(info->end_of_slice)
                {
                    break;
                }
                else
                    info = info->anotherProcessor;
            }
            while (1); // loop over macroblocks
        }
        else//SIMUL_DCORE
        {
            int i = 0;
#if       CALC_THUMBNAIL
            int start = 0;
#endif
            int end_of_slice = 0;
            int first_of_slice = 1;
#if defined(WIN32) || defined(WINCE)
            WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
            pthread_mutex_lock(&img->JobMutex);
#endif

            img->slice_start_mby = currSlice->start_mb_nr / img->sizeInfo->PicWidthInMbs;
            img->slice_start_mbx = (currSlice->start_mb_nr % img->sizeInfo->PicWidthInMbs) - 1;
            memcpy(img->SliceList[img->current_slice_nr-1], img->currentSlice, sizeof(Slice) + sizeof (NALU_t) );
            memcpy(img->curr_slice_img[img->current_slice_nr-1], img, sizeof(ImageParameters));

            for( i = 0; i < AVD_MOTION_LIST_SIZE; i++)
            {
               memcpy(img->curr_slice_img[img->current_slice_nr-1]->mirror_listX[img->current_slice_nr-1][i], img->listX[i],  (img->maxListSize) * sizeof (StorablePicture *));
	        img->curr_slice_img[img->current_slice_nr-1]->listX[i] = img->curr_slice_img[img->current_slice_nr-1]->mirror_listX[img->current_slice_nr-1][i];		   
            }
			
#if defined(WIN32) || defined(WINCE)
            ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
            pthread_mutex_unlock(&img->JobMutex);
#endif
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 start = GetTickCount();
#endif
            for(i = img->next_slice_nr; (i < img->max_jobs) && !end_of_slice && img->error >= 0; i++)
            {
                GetMBY(img->mbsProcessor[i]) = img->slice_start_mby;
                if(i == img->next_slice_nr)
                    GetMBX(img->mbsProcessor[i]) = img->slice_start_mbx;
                else
                    GetMBX(img->mbsProcessor[i]) = -1;
                end_of_slice = parsembs(img, img->mbsProcessor[i]);

                if(first_of_slice)
                {
                    AddAJobToPool(img, end_of_slice, first_of_slice );
                    first_of_slice = 0;
                }
                else
                    AddAJobToPool(img, end_of_slice, first_of_slice );

                img->slice_start_mby += ( 1 + GetMbAffFrameFlag(img));
            }
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 img->parse_ms += GetTickCount() - start;
#endif
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 start = GetTickCount();
#endif

            img->next_slice_nr = img->job_cnt;

            if(img->end_of_frame)
            {
                img->main_got_a_slice = 0;
                do
                {
#if defined(WIN32) || defined(WINCE)
                    WaitForSingleObject(img->JobMutex, INFINITE);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                    pthread_mutex_lock(&img->JobMutex);
#endif


                    info = WaitForJobFromPool(img, NULL);
                    if(info != NULL)
                    {
                        info->mcoreThreadParam = NULL;
#if defined(WIN32) || defined(WINCE)
                        ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                        pthread_mutex_unlock(&img->JobMutex);
#endif

                        processmbs(img, info);
#if 0
                        if(info->flag & 8 || info->end_of_slice)
                        {
                            clr_1bit(img, ( GetMBY(info) >> GetMbAffFrameFlag(img)));
                            if(!img->statea && !img->stateb && !img->statec )
                                img->last_job_flag[0] = 1;
                        }
#else
                        if(1)
                        {
	                        clr_1bit(img, info->curr_job_pos);
	                        if(!img->statea && !img->stateb && !img->statec && !img->stated)
		                        img->last_job_flag[0] = 1;
                        }
#endif

#if defined(WIN32) || defined(WINCE)
                        ReleaseMutex(img->JobMutex);
#endif
#if defined (_IOS) || defined(_MAC_OS) || defined(_LINUX)
                        pthread_mutex_unlock(&img->JobMutex);
#endif
                        CHECK_ERR_BREAK


                    }
                }
                while (info != NULL &&  !img->last_job_flag[0]); // loop over macroblocks
                if(info && img->error < 0)
                {
                   restart_cores(img);
                }
            }
#if        CALC_THUMBNAIL
             if(img->type == I_SLICE)
                 img->decode_ms += GetTickCount() - start;
#endif				
        }
    }

    exit_slice(img);
#if CALC_THUMBNAIL  
    if(img->type == I_SLICE)
        printf("parse %6d, decode %6d,", img->parse_ms, img->decode_ms);
#endif	
    CHECK_ERR_RET_VOID
}

#else

void decode_slice(ImageParameters *img,avdNativeInt current_header)
{
  Slice *currSlice = img->currentSlice;
  if (img->dec_picture==NULL)
  {
	  return;
  }
  
#if CALC_THUMBNAIL  
       img->decode_ms = 0;
       img->parse_ms = 0;
#endif  
#if FEATURE_CABAC
	if (img->active_pps->entropy_coding_mode_flag)
	{
		//if(currSlice->start_mb_nr==0)
		init_contexts(img);
		cabac_new_slice(img);
	}
#endif
#if FEATURE_WEIGHTEDPRED
	//OPT:only call once
	if (currSlice->start_mb_nr==0&&img->type == B_SLICE&&img->apply_weights)
		fill_wp_params(img);
#endif
	//Init_Slice_Neighbor_Info(img);

  // decode main slice information
  if (1)//current_header == SOP)// && GetEIFlag(currSlice) == 0)
  {
	 int l,k;
  	 TMBsProcessor* info	  ;
	 //avdNativeInt end_of_slice = VOI_FALSE;
	 Slice *slice = img->currentSlice; 
	 TPosInfo*  posInfo;
	 setup_slice_methods(img);
	 

	 info = img->mbsProcessor[0];
	
	 if (img->type == I_SLICE && slice->LFDisableIdc != 1&&!GetMbAffFrameFlag(img))
	 {
		 avdNativeInt k;
		 avdNativeInt		coreNum 		= img->vdLibPar->multiCoreNum;
		 avdNativeInt		mbsProcessorNum = max(2,coreNum);//at least we need 2 mbsProcessor
		 for (k=0;k<mbsProcessorNum;k++)
		 {
			 TMBsProcessor*	info2 = img->mbsProcessor[k];
			 AVDIPDeblocker *ipDeblocker = GetIPDeblocker(info2);
			 SetInPlaceIntraMBStrength(img,info2,ipDeblocker->strength);
		 }
		
	 }
	 
	 img->cod_counter=-1;
	 	

#if FEATURE_BFRAME
	if(currSlice->start_mb_nr==0)
		set_ref_pic_num(img);
#if FEATURE_INTERLACE
	if (!GetMbAffFrameFlag(img)) {
		int structure = GetPicStructure(img);
		for (l = 0; l < 2; l++)
			for(k = 0; k < img->listXsize[l]; k++)
			{
				if(img->listX[l][k])
				{
					img->listX[l][k]->chroma_vector_adjustment= 0;
					if(structure == TOP_FIELD && structure != img->listX[l][k]->structure)
						img->listX[l][k]->chroma_vector_adjustment = -2;
					else if(structure == BOTTOM_FIELD && structure != img->listX[l][k]->structure)
						img->listX[l][k]->chroma_vector_adjustment = 2;
				}
			
			}
	}
#endif//FEATURE_INTERLACE
#endif //VOI_H264D_NON_BASELINE
	 InitNewSliceProcessor(img);
	 if(img->vdLibPar->multiCoreNum<2)
	 {
		 info = GetMainProcessor(img);
		 do
		 {
			 processmbsSingleCore(img,info);
			 CHECK_ERR_BREAK
			 if(info->end_of_slice)
			 {
				 break;
			 }
			 else
				 info = info->anotherProcessor;
		 }
		 while (1); // loop over macroblocks
	 }
	 else//SIMUL_DCORE
	 {
		 do
		 {
			 processmbs(img,info);
			 CHECK_ERR_BREAK
		 }
		 while (!info->end_of_slice); // loop over macroblocks
	 }
  }
 
  exit_slice(img);
  CHECK_ERR_RET_VOID
}
#endif
void exit_slice(ImageParameters *img)
{
#if (DUMP_VERSION & DUMP_SLICE)

#endif //(DUMP_VERSION & DUMP_SLICE)
    ExitNewSliceProcessor(img);
}
/*!
 ************************************************************************
 * \brief
 *    Prepare field and frame buffer after frame decoding
 ************************************************************************
 */
void frame_postprocessing(ImageParameters *img)
{
}

/*!
 ************************************************************************
 * \brief
 *    Prepare field and frame buffer after field decoding
 ************************************************************************
 */
void field_postprocessing(ImageParameters *img)
{
}




#ifndef IMX31_ARM11
void IMX31_AddOneFrame(ImageParameters *img, unsigned char *virtualBuf, unsigned long phyBuf) {}
void ARM11_MX31Deblock(ImageParameters *img) {}
void ARM11_MX31DeblockUnInit(ImageParameters *img) {}
int ARM11_MX31DeblockInit(ImageParameters *img)
{
    return -2;
}
void ARM11_MX31SetDBStrength(ImageParameters *img, int rowcount) {}
#endif//IMX31_ARM11



















