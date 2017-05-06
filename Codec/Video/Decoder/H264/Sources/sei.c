/*!
 ************************************************************************
 * \file  sei.c
 *
 * \brief
 *    Functions to implement SEI messages
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Dong Tian        <tian@cs.tut.fi>
 *    - Karsten Suehring <suehring@hhi.de>
 ************************************************************************
 */
#include "contributors.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "global.h"

#ifndef VOI_H264D_BLOCK_SEI
#include "memalloc.h"
#include "sei.h"
#include "vlc.h"
#include "header.h"
#include "mbuffer.h"
#include "parset.h"

// #define PRINT_BUFFERING_PERIOD_INFO    // uncomment to print buffering period SEI info
// #define PRINT_PCITURE_TIMING_INFO      // uncomment to print picture timing SEI info
// #define WRITE_MAP_IMAGE                // uncomment to write spare picture map
// #define PRINT_SUBSEQUENCE_INFO         // uncomment to print sub-sequence SEI info
// #define PRINT_SUBSEQUENCE_LAYER_CHAR   // uncomment to print sub-sequence layer characteristics SEI info
// #define PRINT_SUBSEQUENCE_CHAR         // uncomment to print sub-sequence characteristics SEI info
// #define PRINT_SCENE_INFORMATION        // uncomment to print scene information SEI info
// #define PRINT_PAN_SCAN_RECT            // uncomment to print pan-scan rectangle SEI info
// #define PRINT_RANDOM_ACCESS            // uncomment to print random access point SEI info
// #define PRINT_FILLER_PAYLOAD_INFO      // uncomment to print filler payload SEI info
// #define PRINT_DEC_REF_PIC_MARKING      // uncomment to print decoded picture buffer management repetition SEI info
// #define PRINT_RESERVED_INFO            // uncomment to print reserved SEI info
// #define PRINT_USER_DATA_UNREGISTERED_INFO          // uncomment to print unregistered user data SEI info
// #define PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO  // uncomment to print ITU-T T.35 user data SEI info
// #define PRINT_FULL_FRAME_FREEZE_INFO               // uncomment to print full-frame freeze SEI info
// #define PRINT_FULL_FRAME_FREEZE_RELEASE_INFO       // uncomment to print full-frame freeze release SEI info
// #define PRINT_FULL_FRAME_SNAPSHOT_INFO             // uncomment to print full-frame snapshot SEI info
// #define PRINT_PROGRESSIVE_REFINEMENT_END_INFO      // uncomment to print Progressive refinement segment start SEI info
// #define PRINT_PROGRESSIVE_REFINEMENT_END_INFO      // uncomment to print Progressive refinement segment end SEI info
// #define PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO    // uncomment to print Motion-constrained slice group set SEI info
#if 1//FEATURE_SEI
/*!
 ************************************************************************
 *  \brief
 *     Interpret the Random access point SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_recovery_point_info(ImageParameters *img,Bitstream* buf)
{
  int recovery_frame_cnt, exact_match_flag, broken_link_flag, changing_slice_group_idc;
  recovery_frame_cnt       = avd_ue_v(buf);
  exact_match_flag         = avd_u_1 (buf);
  broken_link_flag         = avd_u_1 (buf);
  changing_slice_group_idc = avd_u_v ( 2, buf);

#ifdef PRINT_RECOVERY_POINT
  printf("Recovery point SEI message\n");
  printf("recovery_frame_cnt       = %d\n", recovery_frame_cnt);
  printf("exact_match_flag         = %d\n", exact_match_flag);
  printf("broken_link_flag         = %d\n", broken_link_flag);
  printf("changing_slice_group_idc = %d\n", changing_slice_group_idc);
#endif
#ifdef PRINT_RECOVERY_POINT
#undef PRINT_RECOVERY_POINT
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the SEI rbsp
 *  \param msg
 *     a pointer that point to the sei message.
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void InterpretSEIMessage(ImageParameters *img,NALU_t *nalu)
{
  int payload_type = 0;
  int payload_size = 0;
  int offset = 0;
  avdUInt8 tmp_byte;
  Bitstream* buf;
  avdUInt8* tmpBuf = nalu->buf;//-((int)nalu->buf&3);
  
  if(tmpBuf[offset] == 0x80||img->active_sps==NULL)//in some clips,the case exists
	  return;
#ifdef USE_JOBPOOL  
  if(img->vdLibPar->multiCoreNum>1)
      buf = (Bitstream*)img->mbsProcessor[img->job_cnt]->mbsParser->m7;//(Bitstream *)voH264AlignedMalloc(img,87, sizeof(Bitstream));
  else
       buf = (Bitstream*)img->mbsProcessor[0]->mbsParser->m7;//(Bitstream *)voH264AlignedMalloc(img,87, sizeof(Bitstream));     
#else
  buf = (Bitstream*)img->mbsProcessor[0]->mbsParser->m7;//(Bitstream *)voH264AlignedMalloc(img,87, sizeof(Bitstream));
#endif
  if(nalu->len>4096)//It is impossible unless there is something wrong in the bitstream
	  AVD_ERROR_CHECK2(img,"InterpretSEIMessage,SEI size is greater than 4096 bytes",ERROR_TooBigSEISize);
  InitBitStream(buf, nalu->buf, nalu->len);
  do
  {
    // sei_message();
    payload_type = 0;
    tmp_byte = tmpBuf[offset];//GetBits(buf, 8);
	offset++;
    while (tmp_byte == 0xFF)
    {
      payload_type += 255;
      tmp_byte = tmpBuf[offset];//GetBits(buf, 8);
	  offset++;
    }
    payload_type += tmp_byte;   // this is the last avdUInt8

           payload_size = 0;
    tmp_byte = tmpBuf[offset];//GetBits(buf, 8);
	offset++;
    while (tmp_byte == 0xFF)
    {
      payload_size += 255;
      tmp_byte = tmpBuf[offset];//GetBits(buf, 8);
      offset++;
    }
    FlushBits(buf, offset*8);
    payload_size += tmp_byte;   // this is the last avdUInt8
    
    switch ( payload_type )     // sei_payload( type, size );
    {
    case  SEI_BUFFERING_PERIOD:
      interpret_buffering_period_info(img,buf);CHECK_ERR_RET_VOID
      break;
    case  SEI_PIC_TIMING:
      interpret_picture_timing_info(img,buf);CHECK_ERR_RET_VOID
      break;
    case  SEI_PAN_SCAN_RECT:
      interpret_pan_scan_rect_info(img,buf);
      break;
    case  SEI_FILLER_PAYLOAD:
      interpret_filler_payload_info(img,buf);
      break;
    case  SEI_USER_DATA_REGISTERED_ITU_T_T35:
      interpret_user_data_registered_itu_t_t35_info(img,buf, payload_size,nalu);
      break;
    case  SEI_USER_DATA_UNREGISTERED:
      interpret_user_data_unregistered_info(img,buf, payload_size);
      break;
    case  SEI_RECOVERY_POINT:
      interpret_recovery_point_info(img,buf);
      break;
    case  SEI_DEC_REF_PIC_MARKING_REPETITION:
      interpret_dec_ref_pic_marking_repetition_info(img,buf);
      break;
    case  SEI_SPARE_PIC:
      interpret_spare_pic(img,buf);
      break;
    case  SEI_SCENE_INFO:
      interpret_scene_information(img,buf);
      break;
    case  SEI_SUB_SEQ_INFO:
      interpret_subsequence_info(img,buf);
      break;
    case  SEI_SUB_SEQ_LAYER_CHARACTERISTICS:
      interpret_subsequence_layer_characteristics_info(img,buf);
      break;
    case  SEI_SUB_SEQ_CHARACTERISTICS:
      interpret_subsequence_characteristics_info(img,buf);
      break;
    case  SEI_FULL_FRAME_FREEZE:
      interpret_full_frame_freeze_info(img,buf);
      break;
    case  SEI_FULL_FRAME_FREEZE_RELEASE:
      interpret_full_frame_freeze_release_info(img,buf);
      break;
    case  SEI_FULL_FRAME_SNAPSHOT:
      interpret_full_frame_snapshot_info(img,buf);
      break;
    case  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START:
      interpret_progressive_refinement_end_info(img,buf);
      break;
    case  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END:
      interpret_progressive_refinement_end_info(img,buf);
      break;
    case  SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET:
      interpret_motion_constrained_slice_group_set_info(img,buf);
      break;
    default:
      interpret_reserved_info(img,buf, payload_size);
      break;
    }
    offset += payload_size;
    //offset = ((32-buf->bBitToGo)>>3) + payload_size - 9;//ShowBits(buf, (1+payload_size)*8);

  } while(offset<nalu->len&&tmpBuf[offset] != 0x80);    // more_rbsp_data()  msg[offset] != 0x80
  // ignore the trailing bits rbsp_trailing_bits();
  //SafevoH264AlignedFree(img,buf);
  if(tmpBuf[offset] != 0x80)
	  AVD_ERROR_CHECK2(img,"InterpretSEIMessage,tmpBuf[offset] != 0x80",100);//ERROR_InvalidSEI);
  //assert(tmpBuf[offset] == 0x80);      // this is the trailing bits
  //assert( offset+1 == size );
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the spare picture SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_spare_pic(ImageParameters *img,Bitstream* buf)
{
  int i,x,y;
  int bit0, bit1, bitc, no_bit0;
  int target_frame_num;
  int num_spare_pics;
  int delta_spare_frame_num, CandidateSpareFrameNum, SpareFrameNum = 0;
  int ref_area_indicator;

  int m, n, left, right, top, bottom,directx, directy;
  avdUInt8 ***map;
  TSizeInfo	*sizeInfo = img->sizeInfo;

#ifdef WRITE_MAP_IMAGE
  int symbol_size_in_bytes = img->pic_unit_bitsize_on_disk/8;
  int  j, k, i0, j0, tmp, kk;
  char filename[20] = "map_dec.yuv";
  FILE *fp;
  avdUInt8** Y;
  static int old_pn=-1;
  static int first = 1;

  printf("Spare picture SEI message\n");
#endif

  target_frame_num = avd_ue_v(buf);

#ifdef WRITE_MAP_IMAGE
  printf( "target_frame_num is %d\n", target_frame_num );
#endif

  num_spare_pics = 1 + avd_ue_v(buf);

#ifdef WRITE_MAP_IMAGE
  printf( "num_spare_pics is %d\n", num_spare_pics );
#endif
#define NEW_SPARE_PIC 1
#if !NEW_SPARE_PIC
  get_mem3D(img,&map, num_spare_pics, sizeInfo->height>>4, sizeInfo->width>>4);

  for (i=0; i<num_spare_pics; i++)
  {
    if (i==0) 
    {
      CandidateSpareFrameNum = target_frame_num - 1;
      if ( CandidateSpareFrameNum < 0 ) CandidateSpareFrameNum = MAX_FN - 1;
    }
    else
      CandidateSpareFrameNum = SpareFrameNum;

    delta_spare_frame_num = avd_ue_v(buf);

    SpareFrameNum = CandidateSpareFrameNum - delta_spare_frame_num;
    if( SpareFrameNum < 0 )
      SpareFrameNum = MAX_FN + SpareFrameNum;

    ref_area_indicator = avd_ue_v(buf);

    switch ( ref_area_indicator )
    {
    case 0:   // The whole frame can serve as spare picture
      for (y=0; y<sizeInfo->height/16; y++)
        for (x=0; x<sizeInfo->width/16; x++)
          map[i][y][x] = 0;
      break;
    case 1:   // The map is not compressed
      for (y=0; y<sizeInfo->height/16; y++)
        for (x=0; x<sizeInfo->width/16; x++)
        {
		  //ref_mb_indicator;
          map[i][y][x] = (avdUInt8)avd_u_1(buf);
        }
      break;
    case 2:   // The map is compressed
              //!KS: could not check this function, description is unclear (as stated in Ed. Note)
      bit0 = 0;
      bit1 = 1;
      bitc = bit0;
      no_bit0 = -1;

      x = ( sizeInfo->width/16 - 1 ) / 2;
      y = ( sizeInfo->height/16 - 1 ) / 2;
      left = right = x;
      top = bottom = y;
      directx = 0;
      directy = 1;

      for (m=0; m<sizeInfo->height/16; m++)
        for (n=0; n<sizeInfo->width/16; n++)
        {

          if (no_bit0<0)
          {
			//zero_run_length;
            no_bit0 = avd_ue_v(buf);
          }
          if (no_bit0>0) map[i][y][x] = bit0;
          else map[i][y][x] = bit1;
          no_bit0--;

          // go to the next mb:
          if ( directx == -1 && directy == 0 )
          {
            if (x > left) x--;
            else if (x == 0)
            {
              y = bottom + 1;
              bottom++;
              directx = 1;
              directy = 0;
            }
            else if (x == left)
            {
              x--;
              left--;
              directx = 0;
              directy = 1;
            }
          }
          else if ( directx == 1 && directy == 0 )
          {
            if (x < right) x++;
            else if (x == sizeInfo->width/16 - 1)
            {
              y = top - 1;
              top--;
              directx = -1;
              directy = 0;
            }
            else if (x == right)
            {
              x++;
              right++;
              directx = 0;
              directy = -1;
            }
          }
          else if ( directx == 0 && directy == -1 )
          {
            if ( y > top) y--;
            else if (y == 0)
            {
              x = left - 1;
              left--;
              directx = 0;
              directy = 1;
            }
            else if (y == top)
            {
              y--;
              top--;
              directx = -1;
              directy = 0;
            }
          }
          else if ( directx == 0 && directy == 1 )
          {
            if (y < bottom) y++;
            else if (y == sizeInfo->height/16 - 1)
            {
              x = right+1;
              right++;
              directx = 0;
              directy = -1;
            }
            else if (y == bottom)
            {
              y++;
              bottom++;
              directx = 1;
              directy = 0;
            }
          }


        }
      break;
    default:
      printf( "Wrong ref_area_indicator %d!\n", ref_area_indicator );
      exit(0);
      break;
    }

  } // end of num_spare_pics
	 free_mem3D(img, map, num_spare_pics );
#else
AVD_ERROR_CHECK2(img,"This is the NEW_SPARE_PIC,it may have issues,Number Huang 20081112\n",100);
for (i=0; i<num_spare_pics; i++)
{
	if (i==0) 
	{
		CandidateSpareFrameNum = target_frame_num - 1;
		if ( CandidateSpareFrameNum < 0 ) CandidateSpareFrameNum = MAX_FN - 1;
	}
	else
		CandidateSpareFrameNum = SpareFrameNum;

	delta_spare_frame_num = avd_ue_v(buf);

	SpareFrameNum = CandidateSpareFrameNum - delta_spare_frame_num;
	if( SpareFrameNum < 0 )
		SpareFrameNum = MAX_FN + SpareFrameNum;

	ref_area_indicator = avd_ue_v(buf);

	switch ( ref_area_indicator )
	{
	/*
	case 0:   // The whole frame can serve as spare picture
		for (y=0; y<sizeInfo->height/16; y++)
			for (x=0; x<sizeInfo->width/16; x++)
				map[i][y][x] = 0;
		break;*/
	case 1:   // The map is not compressed
		for (y=0; y<sizeInfo->height/16; y++)
			for (x=0; x<sizeInfo->width/16; x++)
			{
				//ref_mb_indicator;
				//map[i][y][x] = 
				int id=(avdUInt8)avd_u_1(buf);
			}
			break;
	case 2:   // The map is compressed
		//!KS: could not check this function, description is unclear (as stated in Ed. Note)
		bit0 = 0;
		bit1 = 1;
		bitc = bit0;
		no_bit0 = -1;

		x = ( sizeInfo->width/16 - 1 ) / 2;
		y = ( sizeInfo->height/16 - 1 ) / 2;
		left = right = x;
		top = bottom = y;
		directx = 0;
		directy = 1;

		for (m=0; m<sizeInfo->height/16; m++)
			for (n=0; n<sizeInfo->width/16; n++)
			{

				if (no_bit0<0)
				{
					//zero_run_length;
					no_bit0 = avd_ue_v(buf);
				}
				//if (no_bit0>0) map[i][y][x] = bit0;
				//else map[i][y][x] = bit1;
				no_bit0--;

				// go to the next mb:
				if ( directx == -1 && directy == 0 )
				{
					if (x > left) x--;
					else if (x == 0)
					{
						y = bottom + 1;
						bottom++;
						directx = 1;
						directy = 0;
					}
					else if (x == left)
					{
						x--;
						left--;
						directx = 0;
						directy = 1;
					}
				}
				else if ( directx == 1 && directy == 0 )
				{
					if (x < right) x++;
					else if (x == sizeInfo->width/16 - 1)
					{
						y = top - 1;
						top--;
						directx = -1;
						directy = 0;
					}
					else if (x == right)
					{
						x++;
						right++;
						directx = 0;
						directy = -1;
					}
				}
				else if ( directx == 0 && directy == -1 )
				{
					if ( y > top) y--;
					else if (y == 0)
					{
						x = left - 1;
						left--;
						directx = 0;
						directy = 1;
					}
					else if (y == top)
					{
						y--;
						top--;
						directx = -1;
						directy = 0;
					}
				}
				else if ( directx == 0 && directy == 1 )
				{
					if (y < bottom) y++;
					else if (y == sizeInfo->height/16 - 1)
					{
						x = right+1;
						right++;
						directx = 0;
						directy = -1;
					}
					else if (y == bottom)
					{
						y++;
						bottom++;
						directx = 1;
						directy = 0;
					}
				}


			}
			break;
	default:
		printf( "Wrong ref_area_indicator %d!\n", ref_area_indicator );
		//exit(0);
		break;
	}

} // end of num_spare_pics
#endif
#ifdef WRITE_MAP_IMAGE
  // begin to write map seq
  if ( old_pn != img->number )
  {
    old_pn = img->number;
    get_mem2D(img,&Y, sizeInfo->height, sizeInfo->width);
    if (first)
    {
      fp = fopen( filename, "wb" );
      first = 0;
    }
    else
      fp = fopen( filename, "ab" );
    assert( fp != NULL );
    for (kk=0; kk<num_spare_pics; kk++)
    {
      for (i=0; i < sizeInfo->height/16; i++)
        for (j=0; j < sizeInfo->width/16; j++)
        {
          tmp=map[kk][i][j]==0? 255 : 0;
          for (i0=0; i0<16; i0++)
            for (j0=0; j0<16; j0++)
              Y[i*16+i0][j*16+j0]=tmp;
        }

      // write the map image
      for (i=0; i < sizeInfo->height; i++)
        for (j=0; j < sizeInfo->width; j++)
          fputc(Y[i][j], fp);

      for (k=0; k < 2; k++)
        for (i=0; i < sizeInfo->height/2; i++)
          for (j=0; j < sizeInfo->width/2; j++)
            fputc(128, fp);
    }
    fclose( fp );
    free_mem2D(img, Y );
  }
  // end of writing map image
#undef WRITE_MAP_IMAGE
#endif

 

}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Sub-sequence information SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_subsequence_info(ImageParameters *img,Bitstream* buf)
{
  int sub_seq_layer_num, sub_seq_id, first_ref_pic_flag, leading_non_ref_pic_flag, last_pic_flag, 
      sub_seq_frame_num_flag, sub_seq_frame_num;
  avdInt32 code;

  sub_seq_layer_num        = avd_ue_v(buf);
  sub_seq_id               = avd_ue_v(buf);

  code = ShowBits(buf, 4);
  first_ref_pic_flag       = (code>>3); //avd_u_1 (buf);
  leading_non_ref_pic_flag = (code>>2) & 0x1; //avd_u_1 (buf);
  last_pic_flag            = (code>>1) & 0x1; //avd_u_1 (buf);
  sub_seq_frame_num_flag   =  code     & 0x1; //avd_u_1 (buf);
  FlushBits(buf, 4);

  if (sub_seq_frame_num_flag)
  {
    sub_seq_frame_num        = avd_ue_v(buf);
  }

#ifdef PRINT_SUBSEQUENCE_INFO
  printf("Sub-sequence information SEI message\n");
  printf("sub_seq_layer_num        = %d\n", sub_seq_layer_num );
  printf("sub_seq_id               = %d\n", sub_seq_id);
  printf("first_ref_pic_flag       = %d\n", first_ref_pic_flag);
  printf("leading_non_ref_pic_flag = %d\n", leading_non_ref_pic_flag);
  printf("last_pic_flag            = %d\n", last_pic_flag);
  printf("sub_seq_frame_num_flag   = %d\n", sub_seq_frame_num_flag);
  if (sub_seq_frame_num_flag)
  {
    printf("sub_seq_frame_num        = %d\n", sub_seq_frame_num);
  }
#endif

#ifdef PRINT_SUBSEQUENCE_INFO
#undef PRINT_SUBSEQUENCE_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Sub-sequence layer characteristics SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_subsequence_layer_characteristics_info(ImageParameters *img,Bitstream* buf)
{
  long num_sub_layers, accurate_statistics_flag, average_bit_rate, average_frame_rate;
  int i;

  num_sub_layers = 1 + avd_ue_v(buf);

#ifdef PRINT_SUBSEQUENCE_LAYER_CHAR
  printf("Sub-sequence layer characteristics SEI message\n");
  printf("num_sub_layers_minus1 = %d\n", num_sub_layers - 1);
#endif

  for (i=0; i<num_sub_layers; i++)
  {
    accurate_statistics_flag = avd_u_1(buf);
    average_bit_rate         = avd_u_v(16, buf);
    average_frame_rate       = avd_u_v(16, buf);

#ifdef PRINT_SUBSEQUENCE_LAYER_CHAR
    printf("layer %d: accurate_statistics_flag = %ld \n", i, accurate_statistics_flag);
    printf("layer %d: average_bit_rate         = %ld \n", i, average_bit_rate);
    printf("layer %d: average_frame_rate       = %ld \n", i, average_frame_rate);
#endif
  }
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Sub-sequence characteristics SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_subsequence_characteristics_info(ImageParameters *img,Bitstream* buf)
{
  int i;
  int sub_seq_layer_num, sub_seq_id, duration_flag, average_rate_flag, accurate_statistics_flag;
  unsigned long sub_seq_duration, average_bit_rate, average_frame_rate;
  int num_referenced_subseqs, ref_sub_seq_layer_num, ref_sub_seq_id, ref_sub_seq_direction;

  sub_seq_layer_num = avd_ue_v(buf);
  sub_seq_id        = avd_ue_v(buf);
  duration_flag     = avd_u_1 (buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("Sub-sequence characteristics SEI message\n");
  printf("sub_seq_layer_num = %d\n", sub_seq_layer_num );
  printf("sub_seq_id        = %d\n", sub_seq_id);
  printf("duration_flag     = %d\n", duration_flag);
#endif

  if ( duration_flag )
  {
    sub_seq_duration = avd_u_v (32, buf);
#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("sub_seq_duration = %ld\n", sub_seq_duration);
#endif
  }

  average_rate_flag = avd_u_1 (buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("average_rate_flag = %d\n", average_rate_flag);
#endif

  if ( average_rate_flag )
  {
    accurate_statistics_flag = avd_u_1 (buf);
    average_bit_rate         = avd_u_v (16, buf);
    average_frame_rate       = avd_u_v (16, buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("accurate_statistics_flag = %d\n", accurate_statistics_flag);
    printf("average_bit_rate         = %ld\n", average_bit_rate);
    printf("average_frame_rate       = %ld\n", average_frame_rate);
#endif
  }

  num_referenced_subseqs  = avd_ue_v(buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("num_referenced_subseqs = %d\n", num_referenced_subseqs);
#endif

  for (i=0; i<num_referenced_subseqs; i++)
  {
    ref_sub_seq_layer_num  = avd_ue_v(buf);
    ref_sub_seq_id         = avd_ue_v(buf);
    ref_sub_seq_direction  = avd_u_1 (buf);

#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("ref_sub_seq_layer_num = %d\n", ref_sub_seq_layer_num);
    printf("ref_sub_seq_id        = %d\n", ref_sub_seq_id);
    printf("ref_sub_seq_direction = %d\n", ref_sub_seq_direction);
#endif
  }

#ifdef PRINT_SUBSEQUENCE_CHAR
#undef PRINT_SUBSEQUENCE_CHAR
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Scene information SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_scene_information(ImageParameters *img,Bitstream* buf)
{
  int scene_id, scene_transition_type, second_scene_id;

  scene_id              = avd_ue_v(buf);
  scene_transition_type = avd_ue_v(buf);
  if ( scene_transition_type > 3 )
  {
    second_scene_id     = avd_ue_v(buf);;
  }

#ifdef PRINT_SCENE_INFORMATION
  printf("Scene information SEI message\n");
  printf("scene_transition_type = %d\n", scene_transition_type);
  printf("scene_id              = %d\n", scene_id);
  if ( scene_transition_type > 3 )
  {
    printf("second_scene_id       = %d\n", second_scene_id);
  }
#endif

#ifdef PRINT_SCENE_INFORMATION
#undef PRINT_SCENE_INFORMATION
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Filler payload SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_filler_payload_info(ImageParameters *img,Bitstream *buf)
{
  int payload_cnt = 0;
  //this code may be dead loop,
  while (payload_cnt < buf->code_len)
  {
    if (ShowBits(buf, 8) == 0xFF)
    {
		FlushBits(buf, 8);
        
    }
    payload_cnt++;
  }
  
#ifdef PRINT_FILLER_PAYLOAD_INFO
  printf("Filler payload SEI message\n");
  if (payload_cnt==size)
  {
    printf("read %d bytes of filler payload\n", payload_cnt);
  }
  else
  {
    printf("error reading filler payload: not all bytes are 0xFF (%d of %d)\n", payload_cnt, size);
  }
#endif

#ifdef PRINT_FILLER_PAYLOAD_INFO
#undef PRINT_FILLER_PAYLOAD_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the User data unregistered SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_user_data_unregistered_info(ImageParameters *img,Bitstream *buf, avdNativeInt size)
{
  int offset = 0;
  while (offset < size)
  {
    FlushBits(buf, 8);
    offset ++;
  }

#if 0
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
  printf("User data unregistered SEI message\n");
  printf("uuid_iso_11578 = 0x");
#endif
  assert (size>=16);

  for (offset = 0; offset < 16; offset++)
  {
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
    printf("%02x",payload[offset]);
#endif
  }

#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
    printf("\n");
#endif

  while (offset < size)
  {
    payload_byte = payload[offset];
    offset ++;
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
    printf("Unreg data payload_byte = %d\n", payload_byte);
#endif
  }
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
#undef PRINT_USER_DATA_UNREGISTERED_INFO
#endif
#endif //0
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the User data registered by ITU-T T.35 SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void copyT35Data(ImageParameters *img,VO_CODECBUFFER* dstBuf,avdUInt8* srcBuf, int size)
{
	avdUInt8* seiBuf = dstBuf->Buffer;
	int size2 = min(MAX_T35_SIZE,size);
	if(seiBuf==NULL)
	{
		seiBuf = dstBuf->Buffer = (avdUInt8*)voH264AlignedMalloc(img,2018,MAX_T35_SIZE);
	}
	memcpy(seiBuf,srcBuf,size2);
	dstBuf->Length = size2;
	if(size2<size)
	{
		AVD_ERROR_CHECK2(img,"the ser_data_registered_itu_t_t35_info size is overflow\n",100);
	}
}
void interpret_user_data_registered_itu_t_t35_info(ImageParameters *img,Bitstream *buf, int size,NALU_t* nalu)
{
  int offset = 0;
  //AvdLog2(LL_INFO,"T35_:%d,size=%d\n",img->vdLibPar->enbaleSEI,size);
  if(img->vdLibPar->enbaleSEI&VHSF_GET_T35_USERDDATA)
  {
	  copyT35Data(img,&img->specialInfo->seiBuf,nalu->buf,nalu->len);
  }
  while (offset < size)
  {
    FlushBits(buf, 8);
    offset ++;
  }

#if 0
  avdUInt8 itu_t_t35_country_code, itu_t_t35_country_code_extension_byte, payload_byte;

  itu_t_t35_country_code = payload[offset];
  offset++;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
  printf("User data registered by ITU-T T.35 SEI message\n");
  printf(" itu_t_t35_country_code = %d \n", itu_t_t35_country_code);
#endif
  if(itu_t_t35_country_code == 0xFF) 
  {
    itu_t_t35_country_code_extension_byte = payload[offset];
    offset++;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
    printf(" ITU_T_T35_COUNTRTY_CODE_EXTENSION_BYTE %d \n", itu_t_t35_country_code_extension_byte);
#endif
  }
  while (offset < size)
  {
    payload_byte = payload[offset];
    offset ++;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
    printf("itu_t_t35 payload_byte = %d\n", payload_byte);
#endif
  }
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
#undef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
#endif
#endif //0
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Pan scan rectangle SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_pan_scan_rect_info(ImageParameters *img,Bitstream *buf)
{
  int pan_scan_rect_id, pan_scan_rect_left_offset, pan_scan_rect_right_offset;
  int pan_scan_rect_top_offset, pan_scan_rect_bottom_offset;

  pan_scan_rect_id = avd_ue_v(buf);
  pan_scan_rect_left_offset   = avd_se_v(buf);
  pan_scan_rect_right_offset  = avd_se_v(buf);
  pan_scan_rect_top_offset    = avd_se_v(buf);
  pan_scan_rect_bottom_offset = avd_se_v(buf);

}



/*!
 ************************************************************************
 *  \brief
 *     Interpret the Decoded Picture Buffer Management Repetition SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_dec_ref_pic_marking_repetition_info(ImageParameters *img,Bitstream *buf)
{
  int original_idr_flag, original_frame_num;

  DecRefPicMarking_t *tmp_drpm;

  DecRefPicMarking_t *old_drpm;
  TSpecitialInfo	*specialInfo = img->specialInfo;
  int old_idr_flag , old_no_output_of_prior_pics_flag, old_long_term_reference_flag , old_adaptive_ref_pic_buffering_flag;

  original_idr_flag     = avd_u_1 (buf);
  original_frame_num    = avd_ue_v(buf);

#ifdef PRINT_DEC_REF_PIC_MARKING
  printf("Decoded Picture Buffer Management Repetition SEI message\n");
  printf("original_idr_flag       = %d\n", original_idr_flag);
  printf("original_frame_num      = %d\n", original_frame_num);
#endif

  // we need to save everything that is probably overwritten in dec_ref_pic_marking()
  //old_drpm = specialInfo->dec_ref_pic_marking_buffer;
  old_idr_flag = specialInfo->idr_flag;

  old_no_output_of_prior_pics_flag = specialInfo->no_output_of_prior_pics_flag;
  old_long_term_reference_flag = specialInfo->long_term_reference_flag;
  old_adaptive_ref_pic_buffering_flag = specialInfo->adaptive_ref_pic_buffering_flag;

  // set new initial values
  specialInfo->idr_flag = original_idr_flag;

  dec_ref_pic_marking(img,buf);


  //specialInfo->dec_ref_pic_marking_buffer = old_drpm;
  specialInfo->idr_flag = old_idr_flag;
  specialInfo->no_output_of_prior_pics_flag = old_no_output_of_prior_pics_flag;
  specialInfo->long_term_reference_flag = old_long_term_reference_flag;
  specialInfo->adaptive_ref_pic_buffering_flag = old_adaptive_ref_pic_buffering_flag;
  
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Full-frame freeze SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_full_frame_freeze_info(ImageParameters *img,Bitstream *buf)
{
#ifdef PRINT_FULL_FRAME_FREEZE_INFO
  printf("Full-frame freeze SEI message\n");
  if (size)
  {
    printf("payload size of this message should be zero, but is %d bytes.\n", size);
  }
#endif

#ifdef PRINT_FULL_FRAME_FREEZE_INFO
#undef PRINT_FULL_FRAME_FREEZE_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Full-frame freeze release SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_full_frame_freeze_release_info(ImageParameters *img,Bitstream *buf)
{
#ifdef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
  printf("Full-frame freeze release SEI message\n");
  if (size)
  {
    printf("payload size of this message should be zero, but is %d bytes.\n", size);
  }
#endif

#ifdef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
#undef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Full-frame snapshot SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_full_frame_snapshot_info(ImageParameters *img,Bitstream *buf)
{
  int snapshot_id;
  snapshot_id = avd_ue_v(buf);

#ifdef PRINT_FULL_FRAME_SNAPSHOT_INFO
  printf("Full-frame snapshot SEI message\n");
  printf("snapshot_id = %d\n", snapshot_id);
#endif

#ifdef PRINT_FULL_FRAME_SNAPSHOT_INFO
#undef PRINT_FULL_FRAME_SNAPSHOT_INFO
#endif
}

/*!
 ************************************************************************
 *  \brief
 *     Interpret the Progressive refinement segment start SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_progressive_refinement_start_info(ImageParameters *img,Bitstream *buf)
{
  int progressive_refinement_id, num_refinement_steps_minus1;
  progressive_refinement_id   = avd_ue_v(buf);
  num_refinement_steps_minus1 = avd_ue_v(buf);

#ifdef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
  printf("Progressive refinement segment start SEI message\n");
  printf("progressive_refinement_id   = %d\n", progressive_refinement_id);
  printf("num_refinement_steps_minus1 = %d\n", num_refinement_steps_minus1);
#endif

#ifdef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
#undef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Progressive refinement segment end SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_progressive_refinement_end_info(ImageParameters *img,Bitstream *buf)
{
  int progressive_refinement_id;
  progressive_refinement_id   = avd_ue_v(buf);

#ifdef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
  printf("Progressive refinement segment end SEI message\n");
  printf("progressive_refinement_id   = %d\n", progressive_refinement_id);
#endif

#ifdef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
#undef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Motion-constrained slice group set SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_motion_constrained_slice_group_set_info(ImageParameters *img,Bitstream *buf)
{
  int num_slice_groups_minus1, slice_group_id, exact_match_flag, pan_scan_rect_flag, pan_scan_rect_id;
  int i;

  num_slice_groups_minus1   = avd_ue_v(buf);
  
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
  printf("Motion-constrained slice group set SEI message\n");
  printf("num_slice_groups_minus1   = %d\n", num_slice_groups_minus1);
#endif

  for (i=0; i<=num_slice_groups_minus1;i++)
  {
    slice_group_id   = avd_ue_v(buf);
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
    printf("slice_group_id            = %d\n", slice_group_id);
#endif
  }
  
  exact_match_flag   = avd_u_1(buf);
  pan_scan_rect_flag = avd_u_1(buf);

#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
  printf("exact_match_flag         = %d\n", exact_match_flag);
  printf("pan_scan_rect_flag       = %d\n", pan_scan_rect_flag);
#endif
  
  if (pan_scan_rect_flag)
  {
    pan_scan_rect_id = avd_ue_v(buf);
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
    printf("pan_scan_rect_id         = %d\n", pan_scan_rect_id);
#endif
  }


#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
#undef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Reserved SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_reserved_info(ImageParameters *img,Bitstream *buf, int size)
{
  int offset = 0;

#ifdef PRINT_RESERVED_INFO
  printf("Reserved SEI message\n");
#endif
 // assert (size<16);
 //if(size>=16)
 //	AVD_ERROR_CHECK(img,"interpret_reserved_info",99);
  while (offset < size)
  {
    FlushBits(buf, 8);
    offset ++;
#ifdef PRINT_RESERVED_INFO
    printf("reserved_sei_message_payload_byte = %d\n", payload_byte);
#endif
  }
#ifdef PRINT_RESERVED_INFO
#undef PRINT_RESERVED_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Buffering period SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_buffering_period_info(ImageParameters *img,Bitstream *buf)
{
  int seq_parameter_set_id, initial_cpb_removal_delay, initial_cpb_removal_delay_offset;
  unsigned int k;

  seq_parameter_set_rbsp_t *sps;

  seq_parameter_set_id   = avd_ue_v(buf);
#if 1//ERROR_CHECK
  if(seq_parameter_set_id>10||seq_parameter_set_id<0)
	  AVD_ERROR_CHECK2(img,"interpret_buffering_period_info",ERROR_InvalidSEQParSetID);
#endif
  sps = img->SeqParSet[seq_parameter_set_id];

  // turn off for now;
  UpdateSize(img,sps);CHECK_ERR_RET_VOID

#ifdef PRINT_BUFFERING_PERIOD_INFO
  printf("Buffering period SEI message\n");
  printf("seq_parameter_set_id   = %d\n", seq_parameter_set_id);
#endif

  // Note: NalHrdBpPresentFlag and CpbDpbDelaysPresentFlag can also be set "by some means not specified in this Recommendation | International Standard"
  if (sps->vui_parameters_present_flag)
  {
    
    if (sps->vui_seq_parameters->nal_hrd_parameters_present_flag)
    {
      for (k=0; k<sps->vui_seq_parameters->nal_hrd_parameters.cpb_cnt_minus1+1; k++)
      {
        initial_cpb_removal_delay        = avd_u_v(sps->vui_seq_parameters->nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, buf);
        initial_cpb_removal_delay_offset = avd_u_v(sps->vui_seq_parameters->nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, buf);

#ifdef PRINT_BUFFERING_PERIOD_INFO
        printf("nal initial_cpb_removal_delay[%d]        = %d\n", k, initial_cpb_removal_delay);
        printf("nal initial_cpb_removal_delay_offset[%d] = %d\n", k, initial_cpb_removal_delay_offset);
#endif
      }
    }
    
    if (sps->vui_seq_parameters->vcl_hrd_parameters_present_flag)
    {
      for (k=0; k<sps->vui_seq_parameters->vcl_hrd_parameters.cpb_cnt_minus1+1; k++)
      {
        initial_cpb_removal_delay        = avd_u_v(sps->vui_seq_parameters->vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, buf);
        initial_cpb_removal_delay_offset = avd_u_v(sps->vui_seq_parameters->vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1, buf);

#ifdef PRINT_BUFFERING_PERIOD_INFO
        printf("vcl initial_cpb_removal_delay[%d]        = %d\n", k, initial_cpb_removal_delay);
        printf("vcl initial_cpb_removal_delay_offset[%d] = %d\n", k, initial_cpb_removal_delay_offset);
#endif
      }
    }
  }

#ifdef PRINT_BUFFERING_PERIOD_INFO
#undef PRINT_BUFFERING_PERIOD_INFO
#endif
}


/*!
 ************************************************************************
 *  \brief
 *     Interpret the Picture timing SEI message
 *  \param payload
 *     a pointer that point to the sei payload
 *  \param size
 *     the size of the sei message
 *  \param img
 *     the image pointer
 *    
 ************************************************************************
 */
void interpret_picture_timing_info(ImageParameters *img,Bitstream *buf)
{
  int cpb_removal_delay, dpb_output_delay, picture_structure_present_flag, picture_structure;
  int clock_time_stamp_flag;
  int ct_type, nuit_field_based_flag, counting_type, full_timestamp_flag, discontinuity_flag, cnt_dropped_flag, nframes;
  int seconds_value, minutes_value, hours_value, seconds_flag, minutes_flag, hours_flag, time_offset;
  int clockTimestamp,NumClockTs = 0;
  int i;
  int cpb_removal_len = 24;
  int dpb_output_len  = 24;
  seq_parameter_set_rbsp_t		*active_sps = img->active_sps;
  vui_seq_parameters_t*	vui;//=active_sps->vui_seq_parameters;
  avdUInt8 CpbDpbDelaysPresentFlag;

 vui=active_sps->vui_seq_parameters;
  assert (NULL!=active_sps);
  // CpbDpbDelaysPresentFlag can also be set "by some means not specified in this Recommendation | International Standard"
  CpbDpbDelaysPresentFlag =  (active_sps->vui_parameters_present_flag 
                              && (   (active_sps->vui_seq_parameters->nal_hrd_parameters_present_flag != 0)
                                   ||(active_sps->vui_seq_parameters->vcl_hrd_parameters_present_flag != 0)));

  if (CpbDpbDelaysPresentFlag )
  {
    if (active_sps->vui_parameters_present_flag)
    {
      if (active_sps->vui_seq_parameters->nal_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters->nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters->nal_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
      else if (active_sps->vui_seq_parameters->vcl_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters->vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters->vcl_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
    }
    
    if ((active_sps->vui_seq_parameters->nal_hrd_parameters_present_flag)||
      (active_sps->vui_seq_parameters->vcl_hrd_parameters_present_flag))
    {
      //img->seiTimeInfo.cpb_removal_delay= cpb_removal_delay = avd_u_v(cpb_removal_len, buf);
      dpb_output_delay  = avd_u_v(dpb_output_len, buf);
#ifdef PRINT_PCITURE_TIMING_INFO
      printf("cpb_removal_delay = %d\n",cpb_removal_delay);
      printf("dpb_output_delay  = %d\n",dpb_output_delay);
#endif
    }
  }

  if (!active_sps->vui_parameters_present_flag)
  {
    picture_structure_present_flag = 0;
  }
  else
  {
    picture_structure_present_flag  =  vui->pic_struct_present_flag;
  }
  if (picture_structure_present_flag)
  {
    picture_structure = avd_u_v(4, buf);
#ifdef PRINT_PCITURE_TIMING_INFO
    printf("picture_structure = %d\n",picture_structure);
#endif    
    switch (picture_structure)
    {
    case 0:
    case 1:
    case 2:
      NumClockTs = 1;
      break;
    case 3:
    case 4:
    case 7:
      NumClockTs = 2;
      break;
    case 5:
    case 6:
    case 8:
      NumClockTs = 3;
      break;
    default:
      //AVD_ERROR_CHECK2(img,"reserved picture_structure used (can't determine NumClockTs)", ERROR_InvalidPicStruct);
	  break;
    }
    for (i=0; i<NumClockTs; i++)
    {
      clock_time_stamp_flag = avd_u_1(buf);
#ifdef PRINT_PCITURE_TIMING_INFO
      printf("clock_time_stamp_flag = %d\n",clock_time_stamp_flag);
#endif
      if (clock_time_stamp_flag)
      {
		avdInt32			  code, j;
		code				  = ShowBits(buf, 19);
        ct_type               = (code>>17) & 0x3; //avd_u_v(2, buf);
        nuit_field_based_flag = (code>>16) & 0x1; //avd_u_1(buf);
        counting_type         = (code>>11) & 0x1f; //avd_u_v(5, buf);
        full_timestamp_flag   = (code>>10) & 0x1; //avd_u_1(buf);
        discontinuity_flag    = (code>>9)  & 0x1; //avd_u_1(buf);
        cnt_dropped_flag      = (code>>8)  & 0x1; //avd_u_1(buf);
        nframes               =  code      & 0xff; //avd_u_v(8, buf);
		FlushBits(buf, 19);

#ifdef PRINT_PCITURE_TIMING_INFO
        printf("ct_type               = %d\n",ct_type);
        printf("nuit_field_based_flag = %d\n",nuit_field_based_flag);
        printf("full_timestamp_flag   = %d\n",full_timestamp_flag);
        printf("discontinuity_flag    = %d\n",discontinuity_flag);
        printf("cnt_dropped_flag      = %d\n",cnt_dropped_flag);
        printf("nframes               = %d\n",nframes);
#endif    
        if (full_timestamp_flag)
        {
		  code				    = ShowBits(buf, 17);
          seconds_value         = (code>>11) & 0x3f; //avd_u_v(6, buf);
          minutes_value         = (code>>5 ) & 0x3f; //avd_u_v(6, buf);
          hours_value           =  code      & 0x1f; //avd_u_v(5, buf);
		  FlushBits(buf, 17);
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("seconds_value = %d\n",seconds_value);
          printf("minutes_value = %d\n",minutes_value);
          printf("hours_value   = %d\n",hours_value);
#endif    
        }
        else
        {
		  j = 1;
		  code				    = ShowBits(buf, 20);
          seconds_flag          = (code>>19); //avd_u_1(buf);
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("seconds_flag = %d\n",seconds_flag);
#endif    
          if (seconds_flag)
          {
			j = 8;
            seconds_value         = (code>>13) & 0x3f; //avd_u_v(6, buf);
            minutes_flag          = (code>>12) & 0x1; //avd_u_1(buf);
#ifdef PRINT_PCITURE_TIMING_INFO
            printf("seconds_value = %d\n",seconds_value);
            printf("minutes_flag  = %d\n",minutes_flag);
#endif    
            if(minutes_flag)
            {
			  j = 15;
              minutes_value         = (code>>6) & 0x3f; //avd_u_v(6, buf);
              hours_flag            = (code>>5) & 0x1; //avd_u_1(buf);
#ifdef PRINT_PCITURE_TIMING_INFO
              printf("minutes_value = %d\n",minutes_value);
              printf("hours_flag    = %d\n",hours_flag);
#endif    
              if(hours_flag)
              {
				j = 20;
                hours_value           = code & 0x1f; //avd_u_v(5, buf);
#ifdef PRINT_PCITURE_TIMING_INFO
                printf("hours_value   = %d\n",hours_value);
#endif    
              }
            
            }
          }
		  FlushBits(buf, code);
        }
        if(vui->nal_hrd_parameters.time_offset_length) //!KS which HRD params shall be used?
        {
          //time_offset=0;
          time_offset = avd_u_v(vui->nal_hrd_parameters.time_offset_length, buf);
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("time_offset   = %d\n",time_offset);
#endif    
        }
      }
	  {
		  int num_units_in_tick=vui->num_units_in_tick;
		  int time_scale = vui->time_scale;
		  clockTimestamp = ( ( hours_value * 60 + minutes_value ) * 60 + seconds_value ) * time_scale +
			  nframes * ( num_units_in_tick * ( 1 + nuit_field_based_flag ) ) + time_offset;
	  }
	
    }
  }

#ifdef PRINT_PCITURE_TIMING_INFO
#undef PRINT_PCITURE_TIMING_INFO
#endif
}
#endif
#endif//FEATURE_SEI
