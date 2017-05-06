
#include <math.h>
#include "global.h"
#include "memalloc.h"
#include "sei.h"
#include "vlc.h"
#include "header.h"
#include "mbuffer.h"
#include "parset.h"
#include "voH264Readbits.h"

int InterpretSEIMessage(H264DEC_G *pDecGlobal,byte* msg, int size, Slice *pSlice)
{
  int payload_type = 0;
  int payload_size = 0;
  int offset = 1;
  byte tmp_byte;
  
  do
  {
    // sei_message();
    payload_type = 0;
    tmp_byte = msg[offset++];
    while (tmp_byte == 0xFF)
    {
      payload_type += 255;
      tmp_byte = msg[offset++];
    }
    payload_type += tmp_byte;   // this is the last byte

    payload_size = 0;
    tmp_byte = msg[offset++];
    while (tmp_byte == 0xFF)
    {
      payload_size += 255;
      tmp_byte = msg[offset++];
    }
    payload_size += tmp_byte;   // this is the last byte
    if(offset >= size)
		break;
    /*switch ( payload_type )     // sei_payload( type, size );
    {
    case  SEI_BUFFERING_PERIOD:
      interpret_buffering_period_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_PIC_TIMING:
       interpret_picture_timing_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_PAN_SCAN_RECT:
       interpret_pan_scan_rect_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_FILLER_PAYLOAD:
      //return interpret_filler_payload_info( msg+offset, payload_size );
      break;
    case  SEI_USER_DATA_REGISTERED_ITU_T_T35:
       interpret_user_data_registered_itu_t_t35_info( msg+offset, payload_size );
      break;
    case  SEI_USER_DATA_UNREGISTERED:
       interpret_user_data_unregistered_info( msg+offset, payload_size );
      break;
    case  SEI_RECOVERY_POINT:
       interpret_recovery_point_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_DEC_REF_PIC_MARKING_REPETITION:
       interpret_dec_ref_pic_marking_repetition_info( pDecGlobal,msg+offset, payload_size, pSlice );
      break;
    case  SEI_SPARE_PIC:
       interpret_spare_pic( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_SCENE_INFO:
       interpret_scene_information( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_SUB_SEQ_INFO:
       interpret_subsequence_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_SUB_SEQ_LAYER_CHARACTERISTICS:
       interpret_subsequence_layer_characteristics_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_SUB_SEQ_CHARACTERISTICS:
       interpret_subsequence_characteristics_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_FULL_FRAME_FREEZE:
       interpret_full_frame_freeze_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_FULL_FRAME_FREEZE_RELEASE:
       interpret_full_frame_freeze_release_info( msg+offset, payload_size );
      break;
    case  SEI_FULL_FRAME_SNAPSHOT:
       interpret_full_frame_snapshot_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_START:
       interpret_progressive_refinement_start_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_PROGRESSIVE_REFINEMENT_SEGMENT_END:
       interpret_progressive_refinement_end_info( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_MOTION_CONSTRAINED_SLICE_GROUP_SET:
       interpret_motion_constrained_slice_group_set_info( pDecGlobal,msg+offset, payload_size );
    case  SEI_FILM_GRAIN_CHARACTERISTICS:
       interpret_film_grain_characteristics_info ( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_DEBLOCKING_FILTER_DISPLAY_PREFERENCE:
       interpret_deblocking_filter_display_preference_info ( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_STEREO_VIDEO_INFO:
       interpret_stereo_video_info_info ( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_TONE_MAPPING:
       interpret_tone_mapping( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_POST_FILTER_HINTS:
       interpret_post_filter_hints_info ( pDecGlobal,msg+offset, payload_size );
      break;
    case  SEI_FRAME_PACKING_ARRANGEMENT:
       interpret_frame_packing_arrangement_info( pDecGlobal,msg+offset, payload_size );
      break;
    default:
       interpret_reserved_info( msg+offset, payload_size );
      break;    
    }*/
    switch ( payload_type )     // sei_payload( type, size );
    {
      case  SEI_RECOVERY_POINT:
        interpret_recovery_point_info( pDecGlobal,msg+offset, payload_size );
        break;
	  default:
        interpret_reserved_info( msg+offset, payload_size );
        break;   
	}
	  
    offset += payload_size;

  } while( msg[offset] != 0x80 );    // more_rbsp_data()  msg[offset] != 0x80
  // ignore the trailing bits rbsp_trailing_bits();
  //assert(msg[offset] == 0x80);      // this is the trailing bits
  return 0;
  //assert( offset+1 == size );
}



int interpret_spare_pic( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int i,x,y;
  Bitstream* buf;
  int bit0, bit1, bitc, no_bit0;
  int target_frame_num = 0;
  int num_spare_pics;
  int delta_spare_frame_num, CandidateSpareFrameNum, SpareFrameNum = 0;
  int ref_area_indicator;

  int m, n, left, right, top, bottom,directx, directy;
  byte ***map;

#ifdef WRITE_MAP_IMAGE
  int  j, k, i0, j0, tmp, kk;
  char filename[20] = "map_dec.yuv";
  FILE *fp;
  imgpel** Y;
  static int old_pn=-1;
  static int first = 1;

  printf("Spare picture SEI message\n");
#endif

  //p_Dec->UsedBits = 0;

  assert( payload!=NULL);
  //assert( p_Vid!=NULL);

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);

  target_frame_num = ue_v(pDecGlobal);//"SEI: target_frame_num"

#ifdef WRITE_MAP_IMAGE
  printf( "target_frame_num is %d\n", target_frame_num );
#endif

  num_spare_pics = 1 + ue_v(pDecGlobal);//"SEI: num_spare_pics_minus1"

#ifdef WRITE_MAP_IMAGE
  printf( "num_spare_pics is %d\n", num_spare_pics );
#endif
  if((pDecGlobal->height>>4)==0||(pDecGlobal->width>>4)==0)
  	VOH264ERROR(VO_H264_ERR_SEI);
  get_mem3D(pDecGlobal,&map, num_spare_pics, pDecGlobal->height >> 4, pDecGlobal->width >> 4);

  for (i=0; i<num_spare_pics; i++)
  {
    if (i==0)
    {
      CandidateSpareFrameNum = target_frame_num - 1;
      if ( CandidateSpareFrameNum < 0 ) CandidateSpareFrameNum = MAX_FN - 1;
    }
    else
      CandidateSpareFrameNum = SpareFrameNum;

    delta_spare_frame_num = ue_v(pDecGlobal);//"SEI: delta_spare_frame_num"

    SpareFrameNum = CandidateSpareFrameNum - delta_spare_frame_num;
    if( SpareFrameNum < 0 )
      SpareFrameNum = MAX_FN + SpareFrameNum;

    ref_area_indicator = ue_v(pDecGlobal);//"SEI: ref_area_indicator"

    switch ( ref_area_indicator )
    {
    case 0:   // The whole frame can serve as spare picture
      for (y=0; y<pDecGlobal->height >> 4; y++)
        for (x=0; x<pDecGlobal->width >> 4; x++)
          map[i][y][x] = 0;
      break;
    case 1:   // The map is not compressed
      for (y=0; y<pDecGlobal->height >> 4; y++)
        for (x=0; x<pDecGlobal->width >> 4; x++)
        {
          map[i][y][x] = (byte) u_v (1, pDecGlobal);//"SEI: ref_mb_indicator"
        }
      break;
    case 2:   // The map is compressed
              //!KS: could not check this function, description is unclear (as stated in Ed. Note)
      bit0 = 0;
      bit1 = 1;
      bitc = bit0;
      no_bit0 = -1;

      x = ( (pDecGlobal->width >> 4) - 1 ) / 2;
      y = ( (pDecGlobal->height >> 4) - 1 ) / 2;
      left = right = x;
      top = bottom = y;
      directx = 0;
      directy = 1;

      for (m=0; m<pDecGlobal->height >> 4; m++)
        for (n=0; n<pDecGlobal->width >> 4; n++)
        {

          if (no_bit0<0)
          {
            no_bit0 = ue_v(pDecGlobal);//"SEI: zero_run_length"
          }
          if (no_bit0>0) 
            map[i][y][x] = (byte) bit0;
          else 
            map[i][y][x] = (byte) bit1;
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
            else if (x == (pDecGlobal->width >> 4) - 1)
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
            else if (y == (pDecGlobal->height >> 4) - 1)
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

#ifdef WRITE_MAP_IMAGE
  // begin to write map seq
  if ( old_pn != pDecGlobal->number )
  {
    old_pn = pDecGlobal->number;
    get_mem2Dpel(&Y, pDecGlobal->height, pDecGlobal->width);
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
      for (i=0; i < pDecGlobal->height >> 4; i++)
        for (j=0; j < pDecGlobal->width >> 4; j++)
        {
          tmp=map[kk][i][j]==0? pDecGlobal->max_pel_value_comp[0] : 0;
          for (i0=0; i0<16; i0++)
            for (j0=0; j0<16; j0++)
              Y[i*16+i0][j*16+j0]=tmp;
        }

      // write the map image
      for (i=0; i < pDecGlobal->height; i++)
        for (j=0; j < pDecGlobal->width; j++)
          fwrite(&(Y[i][j]), symbol_size_in_bytes, 1, p_out);

      for (k=0; k < 2; k++)
        for (i=0; i < pDecGlobal->height>>1; i++)
          for (j=0; j < pDecGlobal->width>>1; j++)
            fwrite(&(pDecGlobal->dc_pred_value_comp[1]), symbol_size_in_bytes, 1, p_out);
    }
    fclose( fp );
    free_mem2Dpel( Y );
  }
  // end of writing map image
#undef WRITE_MAP_IMAGE
#endif

  free_mem3D(pDecGlobal, map );

  voFree(pDecGlobal,buf);
  return 0;
}

int interpret_subsequence_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  Bitstream* buf;
  int sub_seq_layer_num, sub_seq_id, first_ref_pic_flag, leading_non_ref_pic_flag, last_pic_flag,
      sub_seq_frame_num_flag, sub_seq_frame_num;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  sub_seq_layer_num        = ue_v(pDecGlobal);//"SEI: sub_seq_layer_num"
  sub_seq_id               = ue_v(pDecGlobal);//"SEI: sub_seq_id"
  first_ref_pic_flag       = u_v (1, pDecGlobal);//"SEI: first_ref_pic_flag" 
  leading_non_ref_pic_flag = u_v (1, pDecGlobal);//"SEI: leading_non_ref_pic_flag"
  last_pic_flag            = u_v (1, pDecGlobal);//"SEI: last_pic_flag"
  sub_seq_frame_num_flag   = u_v (1, pDecGlobal);//"SEI: sub_seq_frame_num_flag"
  if (sub_seq_frame_num_flag)
  {
    sub_seq_frame_num        = ue_v(pDecGlobal);//"SEI: sub_seq_frame_num"
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

  voFree(pDecGlobal,buf);
  return 0;
#ifdef PRINT_SUBSEQUENCE_INFO
#undef PRINT_SUBSEQUENCE_INFO
#endif
}


int interpret_subsequence_layer_characteristics_info( H264DEC_G *pDecGlobal,byte* payload, int size)
{
  Bitstream* buf;
  long num_sub_layers, accurate_statistics_flag, average_bit_rate, average_frame_rate;
  int i;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  num_sub_layers = 1 + ue_v(pDecGlobal);//"SEI: num_sub_layers_minus1"

#ifdef PRINT_SUBSEQUENCE_LAYER_CHAR
  printf("Sub-sequence layer characteristics SEI message\n");
  printf("num_sub_layers_minus1 = %d\n", num_sub_layers - 1);
#endif

  for (i=0; i<num_sub_layers; i++)
  {
    accurate_statistics_flag = u_v (1, pDecGlobal);//"SEI: accurate_statistics_flag"
    average_bit_rate         = u_v(16,pDecGlobal);//"SEI: average_bit_rate"
    average_frame_rate       = u_v(16,pDecGlobal);//"SEI: average_frame_rate"

#ifdef PRINT_SUBSEQUENCE_LAYER_CHAR
    printf("layer %d: accurate_statistics_flag = %ld \n", i, accurate_statistics_flag);
    printf("layer %d: average_bit_rate         = %ld \n", i, average_bit_rate);
    printf("layer %d: average_frame_rate       = %ld \n", i, average_frame_rate);
#endif
  }
  voFree (pDecGlobal,buf);
  return 0;
}


int interpret_subsequence_characteristics_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  Bitstream* buf;
  int i;
  int sub_seq_layer_num, sub_seq_id, duration_flag, average_rate_flag, accurate_statistics_flag;
  unsigned long sub_seq_duration, average_bit_rate, average_frame_rate;
  int num_referenced_subseqs, ref_sub_seq_layer_num, ref_sub_seq_id, ref_sub_seq_direction;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  sub_seq_layer_num = ue_v(pDecGlobal);//"SEI: sub_seq_layer_num"
  sub_seq_id        = ue_v(pDecGlobal);//"SEI: sub_seq_id"
  duration_flag     = u_v (1, pDecGlobal);//"SEI: duration_flag"

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("Sub-sequence characteristics SEI message\n");
  printf("sub_seq_layer_num = %d\n", sub_seq_layer_num );
  printf("sub_seq_id        = %d\n", sub_seq_id);
  printf("duration_flag     = %d\n", duration_flag);
#endif

  if ( duration_flag )
  {
    sub_seq_duration = GetBits32(pDecGlobal);//u_v (32,pDecGlobal);//"SEI: duration_flag"
#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("sub_seq_duration = %ld\n", sub_seq_duration);
#endif
  }

  average_rate_flag = u_v (1, pDecGlobal);//"SEI: average_rate_flag"

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("average_rate_flag = %d\n", average_rate_flag);
#endif

  if ( average_rate_flag )
  {
    accurate_statistics_flag = u_v (1, pDecGlobal);//"SEI: accurate_statistics_flag"
    average_bit_rate         = u_v (16,pDecGlobal);//"SEI: average_bit_rate"
    average_frame_rate       = u_v (16,pDecGlobal);//"SEI: average_frame_rate"

#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("accurate_statistics_flag = %d\n", accurate_statistics_flag);
    printf("average_bit_rate         = %ld\n", average_bit_rate);
    printf("average_frame_rate       = %ld\n", average_frame_rate);
#endif
  }

  num_referenced_subseqs  = ue_v(pDecGlobal);//"SEI: num_referenced_subseqs"

#ifdef PRINT_SUBSEQUENCE_CHAR
  printf("num_referenced_subseqs = %d\n", num_referenced_subseqs);
#endif

  for (i=0; i<num_referenced_subseqs; i++)
  {
    ref_sub_seq_layer_num  = ue_v(pDecGlobal);//"SEI: ref_sub_seq_layer_num"
    ref_sub_seq_id         = ue_v(pDecGlobal);//"SEI: ref_sub_seq_id"
    ref_sub_seq_direction  = u_v (1, pDecGlobal);//"SEI: ref_sub_seq_direction"

#ifdef PRINT_SUBSEQUENCE_CHAR
    printf("ref_sub_seq_layer_num = %d\n", ref_sub_seq_layer_num);
    printf("ref_sub_seq_id        = %d\n", ref_sub_seq_id);
    printf("ref_sub_seq_direction = %d\n", ref_sub_seq_direction);
#endif
  }

  voFree(pDecGlobal, buf );
  return 0;
#ifdef PRINT_SUBSEQUENCE_CHAR
#undef PRINT_SUBSEQUENCE_CHAR
#endif
}


int interpret_scene_information( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  Bitstream* buf;
  int scene_id, scene_transition_type, second_scene_id;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  scene_id              = ue_v(pDecGlobal);//"SEI: scene_id"
  scene_transition_type = ue_v(pDecGlobal);//"SEI: scene_transition_type"
  if ( scene_transition_type > 3 )
  {
    second_scene_id     = ue_v(pDecGlobal);//"SEI: scene_transition_type"
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
  voFree(pDecGlobal, buf );
  return 0;
#ifdef PRINT_SCENE_INFORMATION
#undef PRINT_SCENE_INFORMATION
#endif
}


int interpret_filler_payload_info( byte* payload, int size )
{
  int payload_cnt = 0;

  while (payload_cnt<size)
  {
    if (payload[payload_cnt] == 0xFF)
    {
       payload_cnt++;
    }
  }
  return 0;
#ifdef PRINT_FILLER_PAYLOAD_INFO
#undef PRINT_FILLER_PAYLOAD_INFO
#endif
}

int interpret_user_data_unregistered_info( byte* payload, int size )
{
  int offset = 0;
  byte payload_byte;

#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
  printf("User data unregistered SEI message\n");
  printf("uuid_iso_11578 = 0x");
#endif
//   assert (size>=16);
	if (size < 16)
	{
		VOH264ERROR(VO_H264_ERR_SEI);
	}
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
  return 0;
#ifdef PRINT_USER_DATA_UNREGISTERED_INFO
#undef PRINT_USER_DATA_UNREGISTERED_INFO
#endif
}

int interpret_user_data_registered_itu_t_t35_info( byte* payload, int size )
{
  int offset = 0;
  byte itu_t_t35_country_code, itu_t_t35_country_code_extension_byte, payload_byte;

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
    printf(" ITU_T_T35_COUNTRY_CODE_EXTENSION_BYTE %d \n", itu_t_t35_country_code_extension_byte);
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
  return 0;
#ifdef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
#undef PRINT_USER_DATA_REGISTERED_ITU_T_T35_INFO
#endif
}


int interpret_pan_scan_rect_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int pan_scan_rect_cancel_flag;
  int pan_scan_cnt_minus1, i;
  int pan_scan_rect_repetition_period;
  int pan_scan_rect_id, pan_scan_rect_left_offset, pan_scan_rect_right_offset;
  int pan_scan_rect_top_offset, pan_scan_rect_bottom_offset;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  pan_scan_rect_id = ue_v(pDecGlobal);//"SEI: pan_scan_rect_id"

  pan_scan_rect_cancel_flag = u_v (1, pDecGlobal);//"SEI: pan_scan_rect_cancel_flag"
  if (!pan_scan_rect_cancel_flag) 
  {
    pan_scan_cnt_minus1 = ue_v(pDecGlobal);//"SEI: pan_scan_cnt_minus1"
    for (i = 0; i <= pan_scan_cnt_minus1; i++) 
    {
      pan_scan_rect_left_offset   = se_v(pDecGlobal);//"SEI: pan_scan_rect_left_offset"
      pan_scan_rect_right_offset  = se_v(pDecGlobal);//"SEI: pan_scan_rect_right_offset"
      pan_scan_rect_top_offset    = se_v(pDecGlobal);//"SEI: pan_scan_rect_top_offset"
      pan_scan_rect_bottom_offset = se_v(pDecGlobal);//"SEI: pan_scan_rect_bottom_offset"
#ifdef PRINT_PAN_SCAN_RECT
      printf("Pan scan rectangle SEI message %d/%d\n", i, pan_scan_cnt_minus1);
      printf("pan_scan_rect_id            = %d\n", pan_scan_rect_id);
      printf("pan_scan_rect_left_offset   = %d\n", pan_scan_rect_left_offset);
      printf("pan_scan_rect_right_offset  = %d\n", pan_scan_rect_right_offset);
      printf("pan_scan_rect_top_offset    = %d\n", pan_scan_rect_top_offset);
      printf("pan_scan_rect_bottom_offset = %d\n", pan_scan_rect_bottom_offset);
#endif
    }
    pan_scan_rect_repetition_period = ue_v(pDecGlobal);//"SEI: pan_scan_rect_repetition_period"
  }

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_PAN_SCAN_RECT
#undef PRINT_PAN_SCAN_RECT
#endif
}

int interpret_recovery_point_info( H264DEC_G *pDecGlobal,byte* payload, int size)
{
  int recovery_frame_cnt, exact_match_flag, broken_link_flag, changing_slice_group_idc;


  Bitstream* buf;


  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  recovery_frame_cnt       = ue_v(pDecGlobal);//"SEI: recovery_frame_cnt"
  exact_match_flag         = u_v (1, pDecGlobal);//"SEI: exact_match_flag"
  broken_link_flag         = u_v (1, pDecGlobal);//"SEI: broken_link_flag"
  changing_slice_group_idc = u_v ( 2,pDecGlobal);//"SEI: changing_slice_group_idc"

  pDecGlobal->recovery_point = 1;
  pDecGlobal->recovery_frame_cnt = recovery_frame_cnt;

#ifdef PRINT_RECOVERY_POINT
  printf("Recovery point SEI message\n");
  printf("recovery_frame_cnt       = %d\n", recovery_frame_cnt);
  printf("exact_match_flag         = %d\n", exact_match_flag);
  printf("broken_link_flag         = %d\n", broken_link_flag);
  printf("changing_slice_group_idc = %d\n", changing_slice_group_idc);
#endif
  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_RECOVERY_POINT
#undef PRINT_RECOVERY_POINT
#endif
}

int interpret_dec_ref_pic_marking_repetition_info( H264DEC_G *pDecGlobal,byte* payload, int size, Slice *pSlice )
{
  int original_idr_flag, original_frame_num,ret;
  int original_field_pic_flag, original_bottom_field_flag;

  //DecRefPicMarking_t *tmp_drpm;

  //DecRefPicMarking_t *old_drpm;
  int old_idr_flag , old_no_output_of_prior_pics_flag, old_long_term_reference_flag , old_adaptive_ref_pic_buffering_flag;


  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  original_idr_flag     = u_v (1, pDecGlobal);//"SEI: original_idr_flag"
  original_frame_num    = ue_v(pDecGlobal);//"SEI: original_frame_num"

  if ( !pSlice->active_sps->frame_mbs_only_flag )
  {
    original_field_pic_flag = u_v (1, pDecGlobal);//"SEI: original_field_pic_flag"
    if ( original_field_pic_flag )
    {
      original_bottom_field_flag = u_v (1, pDecGlobal);//"SEI: original_bottom_field_flag"
    }
  }

  // we need to save everything that is probably overwritten in dec_ref_pic_marking()
  old_idr_flag = pSlice->idr_flag; //p_Vid->idr_flag;

  old_no_output_of_prior_pics_flag = pSlice->no_output_of_prior_pics_flag; //p_Vid->no_output_of_prior_pics_flag;
  old_long_term_reference_flag = pSlice->long_term_reference_flag;
  old_adaptive_ref_pic_buffering_flag = pSlice->adaptive_ref_pic_buffering_flag;

  // set new initial values
  //p_Vid->idr_flag = original_idr_flag;
  pSlice->idr_flag = original_idr_flag;

  ret = dec_ref_pic_marking(pDecGlobal, pSlice);
  if(ret)
  	return ret;



  // restore old values in p_Vid
  pSlice->idr_flag = old_idr_flag;
  pSlice->no_output_of_prior_pics_flag = old_no_output_of_prior_pics_flag;
  //pDecGlobal->no_output_of_prior_pics_flag = pSlice->no_output_of_prior_pics_flag;
  pSlice->long_term_reference_flag = old_long_term_reference_flag;
  //pDecGlobal->long_term_reference_flag = old_long_term_reference_flag;
  pSlice->adaptive_ref_pic_buffering_flag = old_adaptive_ref_pic_buffering_flag;
  //pDecGlobal->adaptive_ref_pic_buffering_flag = old_adaptive_ref_pic_buffering_flag;

  voFree (pDecGlobal,buf);
  return 0;

}

int interpret_full_frame_freeze_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int full_frame_freeze_repetition_period;
  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  full_frame_freeze_repetition_period  = ue_v(pDecGlobal);//"SEI: full_frame_freeze_repetition_period"

#ifdef PRINT_FULL_FRAME_FREEZE_INFO
  printf("full_frame_freeze_repetition_period = %d\n", full_frame_freeze_repetition_period);
#endif

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_FULL_FRAME_FREEZE_INFO
#undef PRINT_FULL_FRAME_FREEZE_INFO
#endif
}


int interpret_full_frame_freeze_release_info( byte* payload, int size )
{
#ifdef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
  printf("Full-frame freeze release SEI message\n");
  if (size)
  {
    printf("payload size of this message should be zero, but is %d bytes.\n", size);
  }
#endif
  return 0;
#ifdef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
#undef PRINT_FULL_FRAME_FREEZE_RELEASE_INFO
#endif
}

int interpret_full_frame_snapshot_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int snapshot_id;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  snapshot_id = ue_v(pDecGlobal);//"SEI: snapshot_id"

#ifdef PRINT_FULL_FRAME_SNAPSHOT_INFO
  printf("Full-frame snapshot SEI message\n");
  printf("snapshot_id = %d\n", snapshot_id);
#endif
  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_FULL_FRAME_SNAPSHOT_INFO
#undef PRINT_FULL_FRAME_SNAPSHOT_INFO
#endif
}


int interpret_progressive_refinement_start_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int progressive_refinement_id, num_refinement_steps_minus1;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  progressive_refinement_id   = ue_v(pDecGlobal);//"SEI: progressive_refinement_id"
  num_refinement_steps_minus1 = ue_v(pDecGlobal);//"SEI: num_refinement_steps_minus1"

#ifdef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
  printf("Progressive refinement segment start SEI message\n");
  printf("progressive_refinement_id   = %d\n", progressive_refinement_id);
  printf("num_refinement_steps_minus1 = %d\n", num_refinement_steps_minus1);
#endif
  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
#undef PRINT_PROGRESSIVE_REFINEMENT_START_INFO
#endif
}


int interpret_progressive_refinement_end_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int progressive_refinement_id;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  progressive_refinement_id   = ue_v(pDecGlobal);//"SEI: progressive_refinement_id"

#ifdef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
  printf("Progressive refinement segment end SEI message\n");
  printf("progressive_refinement_id   = %d\n", progressive_refinement_id);
#endif
  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
#undef PRINT_PROGRESSIVE_REFINEMENT_END_INFO
#endif
}


int interpret_motion_constrained_slice_group_set_info( H264DEC_G *pDecGlobal,byte* payload, int size)
{
  int num_slice_groups_minus1, slice_group_id, exact_match_flag, pan_scan_rect_flag, pan_scan_rect_id;
  int i;
  int sliceGroupSize;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  num_slice_groups_minus1   = ue_v(pDecGlobal);//"SEI: num_slice_groups_minus1"
  sliceGroupSize = CeilLog2( num_slice_groups_minus1 + 1 );
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
  printf("Motion-constrained slice group set SEI message\n");
  printf("num_slice_groups_minus1   = %d\n", num_slice_groups_minus1);
#endif

  for (i=0; i<=num_slice_groups_minus1;i++)
  {

    slice_group_id   = u_v (sliceGroupSize,pDecGlobal);//"SEI: slice_group_id"
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
    printf("slice_group_id            = %d\n", slice_group_id);
#endif
  }

  exact_match_flag   = u_v (1, pDecGlobal);//"SEI: exact_match_flag"
  pan_scan_rect_flag = u_v (1, pDecGlobal);//"SEI: pan_scan_rect_flag"

#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
  printf("exact_match_flag         = %d\n", exact_match_flag);
  printf("pan_scan_rect_flag       = %d\n", pan_scan_rect_flag);
#endif

  if (pan_scan_rect_flag)
  {
    pan_scan_rect_id = ue_v(pDecGlobal);//"SEI: pan_scan_rect_id"
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
    printf("pan_scan_rect_id         = %d\n", pan_scan_rect_id);
#endif
  }

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
#undef PRINT_MOTION_CONST_SLICE_GROUP_SET_INFO
#endif
}


int interpret_film_grain_characteristics_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int film_grain_characteristics_cancel_flag;
  int model_id, separate_colour_description_present_flag;
  int film_grain_bit_depth_luma_minus8, film_grain_bit_depth_chroma_minus8, film_grain_full_range_flag, film_grain_colour_primaries, film_grain_transfer_characteristics, film_grain_matrix_coefficients;
  int blending_mode_id, log2_scale_factor, comp_model_present_flag[3];
  int num_intensity_intervals_minus1, num_model_values_minus1;
  int intensity_interval_lower_bound, intensity_interval_upper_bound;
  int comp_model_value;
  int film_grain_characteristics_repetition_period;

  int c, i, j;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  film_grain_characteristics_cancel_flag = u_v (1, pDecGlobal);//"SEI: film_grain_characteristics_cancel_flag"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
  printf("film_grain_characteristics_cancel_flag = %d\n", film_grain_characteristics_cancel_flag);
#endif
  if(!film_grain_characteristics_cancel_flag)
  {

    model_id                                    = u_v(2,pDecGlobal);//"SEI: model_id"
    separate_colour_description_present_flag    = u_v (1, pDecGlobal);//"SEI: separate_colour_description_present_flag"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
    printf("model_id = %d\n", model_id);
    printf("separate_colour_description_present_flag = %d\n", separate_colour_description_present_flag);
#endif
    if (separate_colour_description_present_flag)
    {
      film_grain_bit_depth_luma_minus8          = u_v(3,pDecGlobal);//"SEI: film_grain_bit_depth_luma_minus8"
      film_grain_bit_depth_chroma_minus8        = u_v(3,pDecGlobal);//"SEI: film_grain_bit_depth_chroma_minus8"
      film_grain_full_range_flag                = u_v(1,pDecGlobal);//"SEI: film_grain_full_range_flag"
      film_grain_colour_primaries               = u_v(8,pDecGlobal);//"SEI: film_grain_colour_primaries"
      film_grain_transfer_characteristics       = u_v(8,pDecGlobal);//"SEI: film_grain_transfer_characteristics"
      film_grain_matrix_coefficients            = u_v(8,pDecGlobal);//"SEI: film_grain_matrix_coefficients"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
      printf("film_grain_bit_depth_luma_minus8 = %d\n", film_grain_bit_depth_luma_minus8);
      printf("film_grain_bit_depth_chroma_minus8 = %d\n", film_grain_bit_depth_chroma_minus8);
      printf("film_grain_full_range_flag = %d\n", film_grain_full_range_flag);
      printf("film_grain_colour_primaries = %d\n", film_grain_colour_primaries);
      printf("film_grain_transfer_characteristics = %d\n", film_grain_transfer_characteristics);
      printf("film_grain_matrix_coefficients = %d\n", film_grain_matrix_coefficients);
#endif
    }
    blending_mode_id                            = u_v(2,pDecGlobal);//"SEI: blending_mode_id"
    log2_scale_factor                           = u_v(4,pDecGlobal);//"SEI: log2_scale_factor"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
    printf("blending_mode_id = %d\n", blending_mode_id);
    printf("log2_scale_factor = %d\n", log2_scale_factor);
#endif
    for (c = 0; c < 3; c ++)
    {
      comp_model_present_flag[c]                = u_v (1, pDecGlobal);//"SEI: comp_model_present_flag"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
      printf("comp_model_present_flag = %d\n", comp_model_present_flag[c]);
#endif
    }
    for (c = 0; c < 3; c ++)
      if (comp_model_present_flag[c])
      {
        num_intensity_intervals_minus1          = u_v(8,pDecGlobal);//"SEI: num_intensity_intervals_minus1"
        num_model_values_minus1                 = u_v(3,pDecGlobal);//"SEI: num_model_values_minus1"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
        printf("num_intensity_intervals_minus1 = %d\n", num_intensity_intervals_minus1);
        printf("num_model_values_minus1 = %d\n", num_model_values_minus1);
#endif
        for (i = 0; i <= num_intensity_intervals_minus1; i ++)
        {
          intensity_interval_lower_bound        = u_v(8,pDecGlobal);//"SEI: intensity_interval_lower_bound"
          intensity_interval_upper_bound        = u_v(8,pDecGlobal);//"SEI: intensity_interval_upper_bound"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
          printf("intensity_interval_lower_bound = %d\n", intensity_interval_lower_bound);
          printf("intensity_interval_upper_bound = %d\n", intensity_interval_upper_bound);
#endif
          for (j = 0; j <= num_model_values_minus1; j++)
          {
            comp_model_value                    = se_v(pDecGlobal);//"SEI: comp_model_value"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
            printf("comp_model_value = %d\n", comp_model_value);
#endif
          }
        }
      }
    film_grain_characteristics_repetition_period = ue_v(pDecGlobal);//"SEI: film_grain_characteristics_repetition_period"
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
    printf("film_grain_characteristics_repetition_period = %d\n", film_grain_characteristics_repetition_period);
#endif
  }

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
#undef PRINT_FILM_GRAIN_CHARACTERISTICS_INFO
#endif
}

int interpret_deblocking_filter_display_preference_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int deblocking_display_preference_cancel_flag;
  int display_prior_to_deblocking_preferred_flag, dec_frame_buffering_constraint_flag, deblocking_display_preference_repetition_period;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  deblocking_display_preference_cancel_flag             = u_v (1, pDecGlobal);//"SEI: deblocking_display_preference_cancel_flag"
#ifdef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
  printf("deblocking_display_preference_cancel_flag = %d\n", deblocking_display_preference_cancel_flag);
#endif
  if(!deblocking_display_preference_cancel_flag)
  {
    display_prior_to_deblocking_preferred_flag            = u_v (1, pDecGlobal);//"SEI: display_prior_to_deblocking_preferred_flag"
    dec_frame_buffering_constraint_flag                   = u_v (1, pDecGlobal);//"SEI: dec_frame_buffering_constraint_flag"
    deblocking_display_preference_repetition_period       = ue_v(pDecGlobal);//"SEI: deblocking_display_preference_repetition_period"
#ifdef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
    printf("display_prior_to_deblocking_preferred_flag = %d\n", display_prior_to_deblocking_preferred_flag);
    printf("dec_frame_buffering_constraint_flag = %d\n", dec_frame_buffering_constraint_flag);
    printf("deblocking_display_preference_repetition_period = %d\n", deblocking_display_preference_repetition_period);
#endif
  }

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
#undef PRINT_DEBLOCKING_FILTER_DISPLAY_PREFERENCE_INFO
#endif
}


int interpret_stereo_video_info_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int field_views_flags;
  int top_field_is_left_view_flag, current_frame_is_left_view_flag, next_frame_is_second_view_flag;
  int left_view_self_contained_flag;
  int right_view_self_contained_flag;

  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  field_views_flags = u_v (1, pDecGlobal);//"SEI: field_views_flags"
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
  printf("field_views_flags = %d\n", field_views_flags);
#endif
  if (field_views_flags)
  {
    top_field_is_left_view_flag         = u_v (1, pDecGlobal);//"SEI: top_field_is_left_view_flag"
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
    printf("top_field_is_left_view_flag = %d\n", top_field_is_left_view_flag);
#endif
  }
  else
  {
    current_frame_is_left_view_flag     = u_v (1, pDecGlobal);//"SEI: current_frame_is_left_view_flag"
    next_frame_is_second_view_flag      = u_v (1, pDecGlobal);//"SEI: next_frame_is_second_view_flag"
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
    printf("current_frame_is_left_view_flag = %d\n", current_frame_is_left_view_flag);
    printf("next_frame_is_second_view_flag = %d\n", next_frame_is_second_view_flag);
#endif
  }

  left_view_self_contained_flag         = u_v (1, pDecGlobal);//"SEI: left_view_self_contained_flag"
  right_view_self_contained_flag        = u_v (1, pDecGlobal);//"SEI: right_view_self_contained_flag"
#ifdef PRINT_STEREO_VIDEO_INFO_INFO
  printf("left_view_self_contained_flag = %d\n", left_view_self_contained_flag);
  printf("right_view_self_contained_flag = %d\n", right_view_self_contained_flag);
#endif

  voFree (pDecGlobal,buf);
  return 0;

#ifdef PRINT_STEREO_VIDEO_INFO_INFO
#undef PRINT_STEREO_VIDEO_INFO_INFO
#endif
}

int interpret_reserved_info( byte* payload, int size )
{
  int offset = 0;
  byte payload_byte;

#ifdef PRINT_RESERVED_INFO
  printf("Reserved SEI message\n");
#endif

  while (offset < size)
  {
    payload_byte = payload[offset];
    offset ++;
#ifdef PRINT_RESERVED_INFO
    printf("reserved_sei_message_payload_byte = %d\n", payload_byte);
#endif
  }
  return 0;
#ifdef PRINT_RESERVED_INFO
#undef PRINT_RESERVED_INFO
#endif
}



int interpret_buffering_period_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  int seq_parameter_set_id, initial_cpb_removal_delay, initial_cpb_removal_delay_offset;
  unsigned int k;

  Bitstream* buf;
  seq_parameter_set_rbsp_t *sps;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  seq_parameter_set_id   = ue_v(pDecGlobal);//"SEI: seq_parameter_set_id"

  sps = &pDecGlobal->SeqParSet[seq_parameter_set_id];
  if (sps->Valid != TRUE)
    VOH264ERROR(VO_H264_ERR_SPSIsNULL);
  activate_sps(pDecGlobal, sps);

#ifdef PRINT_BUFFERING_PERIOD_INFO
  printf("Buffering period SEI message\n");
  printf("seq_parameter_set_id   = %d\n", seq_parameter_set_id);
#endif

  // Note: NalHrdBpPresentFlag and CpbDpbDelaysPresentFlag can also be set "by some means not specified in this Recommendation | International Standard"
  if (sps->vui_parameters_present_flag)
  {

    if (sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
    {
      for (k=0; k<sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1+1; k++)
      {
        initial_cpb_removal_delay        = u_v(sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1,pDecGlobal);//"SEI: initial_cpb_removal_delay"
        initial_cpb_removal_delay_offset = u_v(sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1,pDecGlobal);//"SEI: initial_cpb_removal_delay_offset"

#ifdef PRINT_BUFFERING_PERIOD_INFO
        printf("nal initial_cpb_removal_delay[%d]        = %d\n", k, initial_cpb_removal_delay);
        printf("nal initial_cpb_removal_delay_offset[%d] = %d\n", k, initial_cpb_removal_delay_offset);
#endif
      }
    }

    if (sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
    {
      for (k=0; k<sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1+1; k++)
      {
        initial_cpb_removal_delay        = u_v(sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1,pDecGlobal);//"SEI: initial_cpb_removal_delay"
        initial_cpb_removal_delay_offset = u_v(sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1,pDecGlobal);//"SEI: initial_cpb_removal_delay_offset"

#ifdef PRINT_BUFFERING_PERIOD_INFO
        printf("vcl initial_cpb_removal_delay[%d]        = %d\n", k, initial_cpb_removal_delay);
        printf("vcl initial_cpb_removal_delay_offset[%d] = %d\n", k, initial_cpb_removal_delay_offset);
#endif
      }
    }
  }

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_BUFFERING_PERIOD_INFO
#undef PRINT_BUFFERING_PERIOD_INFO
#endif
}


int interpret_picture_timing_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  seq_parameter_set_rbsp_t *active_sps = pDecGlobal->pCurSlice->active_sps;

  int cpb_removal_delay, dpb_output_delay, picture_structure_present_flag, picture_structure;
  int clock_time_stamp_flag;
  int ct_type, nuit_field_based_flag, counting_type, full_timestamp_flag, discontinuity_flag, cnt_dropped_flag, nframes;
  int seconds_value, minutes_value, hours_value, seconds_flag, minutes_flag, hours_flag, time_offset;
  int NumClockTs = 0;
  int i;

  int cpb_removal_len = 24;
  int dpb_output_len  = 24;

  Boolean CpbDpbDelaysPresentFlag;

  Bitstream* buf;

  if (NULL==active_sps)
  {
    fprintf (stderr, "Warning: no active SPS, timing SEI cannot be parsed\n");
    return 0;
  }

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;


#ifdef PRINT_PCITURE_TIMING_INFO
  printf("Picture timing SEI message\n");
#endif

  // CpbDpbDelaysPresentFlag can also be set "by some means not specified in this Recommendation | International Standard"
  CpbDpbDelaysPresentFlag =  (Boolean) (active_sps->vui_parameters_present_flag
                              && (   (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag != 0)
                                   ||(active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag != 0)));

  if (CpbDpbDelaysPresentFlag )
  {
    if (active_sps->vui_parameters_present_flag)
    {
      if (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
      else if (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
      {
        cpb_removal_len = active_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
        dpb_output_len  = active_sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1  + 1;
      }
    }

    if ((active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)||
      (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag))
    {
      cpb_removal_delay = u_v(cpb_removal_len,pDecGlobal);//"SEI: cpb_removal_delay"
      dpb_output_delay  = u_v(dpb_output_len,pDecGlobal);//"SEI: dpb_output_delay"
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
    picture_structure_present_flag  =  active_sps->vui_seq_parameters.pic_struct_present_flag;
  }

  if (picture_structure_present_flag)
  {
    picture_structure = u_v(4,pDecGlobal);//"SEI: pic_struct"
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
      //error("reserved picture_structure used (can't determine NumClockTs)", 500);
      break;
    }
    for (i=0; i<NumClockTs; i++)
    {
      clock_time_stamp_flag = u_v (1, pDecGlobal);//"SEI: clock_time_stamp_flag"
#ifdef PRINT_PCITURE_TIMING_INFO
      printf("clock_time_stamp_flag = %d\n",clock_time_stamp_flag);
#endif
      if (clock_time_stamp_flag)
      {
        ct_type               = u_v(2,pDecGlobal);//"SEI: ct_type"
        nuit_field_based_flag = u_v (1, pDecGlobal);//"SEI: nuit_field_based_flag"
        counting_type         = u_v(5,pDecGlobal);//"SEI: counting_type"
        full_timestamp_flag   = u_v (1, pDecGlobal);//"SEI: full_timestamp_flag"
        discontinuity_flag    = u_v (1, pDecGlobal);//"SEI: discontinuity_flag"
        cnt_dropped_flag      = u_v (1, pDecGlobal);//"SEI: cnt_dropped_flag"
        nframes               = u_v(8,pDecGlobal);//"SEI: nframes"

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
          seconds_value         = u_v(6,pDecGlobal);//"SEI: seconds_value"
          minutes_value         = u_v(6,pDecGlobal);//"SEI: minutes_value"
          hours_value           = u_v(5,pDecGlobal);//"SEI: hours_value"
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("seconds_value = %d\n",seconds_value);
          printf("minutes_value = %d\n",minutes_value);
          printf("hours_value   = %d\n",hours_value);
#endif
        }
        else
        {
          seconds_flag          = u_v (1, pDecGlobal);//"SEI: seconds_flag"
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("seconds_flag = %d\n",seconds_flag);
#endif
          if (seconds_flag)
          {
            seconds_value         = u_v(6,pDecGlobal);//"SEI: seconds_value"
            minutes_flag          = u_v (1, pDecGlobal);//"SEI: minutes_flag"
#ifdef PRINT_PCITURE_TIMING_INFO
            printf("seconds_value = %d\n",seconds_value);
            printf("minutes_flag  = %d\n",minutes_flag);
#endif
            if(minutes_flag)
            {
              minutes_value         = u_v(6,pDecGlobal);//"SEI: minutes_value"
              hours_flag            = u_v (1, pDecGlobal);//"SEI: hours_flag"
#ifdef PRINT_PCITURE_TIMING_INFO
              printf("minutes_value = %d\n",minutes_value);
              printf("hours_flag    = %d\n",hours_flag);
#endif
              if(hours_flag)
              {
                hours_value           = u_v(5,pDecGlobal);//"SEI: hours_value"
#ifdef PRINT_PCITURE_TIMING_INFO
                printf("hours_value   = %d\n",hours_value);
#endif
              }
            }
          }
        }
        {
          int time_offset_length;
          if (active_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag)
            time_offset_length = active_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length;
          else if (active_sps->vui_seq_parameters.nal_hrd_parameters_present_flag)
            time_offset_length = active_sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length;
          else
            time_offset_length = 24;
          if (time_offset_length)
            time_offset = i_v(time_offset_length,pDecGlobal);//"SEI: time_offset"
          else
            time_offset = 0;
#ifdef PRINT_PCITURE_TIMING_INFO
          printf("time_offset   = %d\n",time_offset);
#endif
        }
      }
    }
  }

  voFree (pDecGlobal,buf);
  return 0;
#ifdef PRINT_PCITURE_TIMING_INFO
#undef PRINT_PCITURE_TIMING_INFO
#endif
}


int interpret_frame_packing_arrangement_info( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  frame_packing_arrangement_information_struct seiFramePackingArrangement;
  Bitstream* buf;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
  printf("Frame packing arrangement SEI message\n");
#endif

  seiFramePackingArrangement.frame_packing_arrangement_id = (unsigned int)ue_v(pDecGlobal );//"SEI: frame_packing_arrangement_id"
  seiFramePackingArrangement.frame_packing_arrangement_cancel_flag = u_v (1, pDecGlobal);//"SEI: frame_packing_arrangement_cancel_flag"
#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
  printf("frame_packing_arrangement_id                 = %d\n", seiFramePackingArrangement.frame_packing_arrangement_id);
  printf("frame_packing_arrangement_cancel_flag        = %d\n", seiFramePackingArrangement.frame_packing_arrangement_cancel_flag);
#endif
  if ( seiFramePackingArrangement.frame_packing_arrangement_cancel_flag == FALSE )
  {
    seiFramePackingArrangement.frame_packing_arrangement_type = (unsigned char)u_v( 7,pDecGlobal);//"SEI: frame_packing_arrangement_type"
    seiFramePackingArrangement.quincunx_sampling_flag         = u_v (1, pDecGlobal);//"SEI: quincunx_sampling_flag"
    seiFramePackingArrangement.content_interpretation_type    = (unsigned char)u_v( 6,pDecGlobal);//"SEI: content_interpretation_type"
    seiFramePackingArrangement.spatial_flipping_flag          = u_v (1, pDecGlobal);//"SEI: spatial_flipping_flag"
    seiFramePackingArrangement.frame0_flipped_flag            = u_v (1, pDecGlobal);//"SEI: frame0_flipped_flag"
    seiFramePackingArrangement.field_views_flag               = u_v (1, pDecGlobal);//"SEI: field_views_flag"
    seiFramePackingArrangement.current_frame_is_frame0_flag   = u_v (1, pDecGlobal);//"SEI: current_frame_is_frame0_flag"
    seiFramePackingArrangement.frame0_self_contained_flag     = u_v (1, pDecGlobal);//"SEI: frame0_self_contained_flag"
    seiFramePackingArrangement.frame1_self_contained_flag     = u_v (1, pDecGlobal);//"SEI: frame1_self_contained_flag"
#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
    printf("frame_packing_arrangement_type    = %d\n", seiFramePackingArrangement.frame_packing_arrangement_type);
    printf("quincunx_sampling_flag            = %d\n", seiFramePackingArrangement.quincunx_sampling_flag);
    printf("content_interpretation_type       = %d\n", seiFramePackingArrangement.content_interpretation_type);
    printf("spatial_flipping_flag             = %d\n", seiFramePackingArrangement.spatial_flipping_flag);
    printf("frame0_flipped_flag               = %d\n", seiFramePackingArrangement.frame0_flipped_flag);
    printf("field_views_flag                  = %d\n", seiFramePackingArrangement.field_views_flag);
    printf("current_frame_is_frame0_flag      = %d\n", seiFramePackingArrangement.current_frame_is_frame0_flag);
    printf("frame0_self_contained_flag        = %d\n", seiFramePackingArrangement.frame0_self_contained_flag);
    printf("frame1_self_contained_flag        = %d\n", seiFramePackingArrangement.frame1_self_contained_flag);
#endif
    if ( seiFramePackingArrangement.quincunx_sampling_flag == FALSE && seiFramePackingArrangement.frame_packing_arrangement_type != 5 )
    {
      seiFramePackingArrangement.frame0_grid_position_x = (unsigned char)u_v( 4,pDecGlobal);//"SEI: frame0_grid_position_x"
      seiFramePackingArrangement.frame0_grid_position_y = (unsigned char)u_v( 4,pDecGlobal);//"SEI: frame0_grid_position_y"
      seiFramePackingArrangement.frame1_grid_position_x = (unsigned char)u_v( 4,pDecGlobal);//"SEI: frame1_grid_position_x"
      seiFramePackingArrangement.frame1_grid_position_y = (unsigned char)u_v( 4,pDecGlobal);//"SEI: frame1_grid_position_y"
#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
      printf("frame0_grid_position_x      = %d\n", seiFramePackingArrangement.frame0_grid_position_x);
      printf("frame0_grid_position_y      = %d\n", seiFramePackingArrangement.frame0_grid_position_y);
      printf("frame1_grid_position_x      = %d\n", seiFramePackingArrangement.frame1_grid_position_x);
      printf("frame1_grid_position_y      = %d\n", seiFramePackingArrangement.frame1_grid_position_y);
#endif
    }
    seiFramePackingArrangement.frame_packing_arrangement_reserved_byte = (unsigned char)u_v( 8,pDecGlobal);//"SEI: frame_packing_arrangement_reserved_byte"
    seiFramePackingArrangement.frame_packing_arrangement_repetition_period = (unsigned int)ue_v(pDecGlobal );//"SEI: frame_packing_arrangement_repetition_period"
#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
    printf("frame_packing_arrangement_reserved_byte          = %d\n", seiFramePackingArrangement.frame_packing_arrangement_reserved_byte);
    printf("frame_packing_arrangement_repetition_period      = %d\n", seiFramePackingArrangement.frame_packing_arrangement_repetition_period);
#endif
  }
  seiFramePackingArrangement.frame_packing_arrangement_extension_flag = u_v (1, pDecGlobal);//"SEI: frame_packing_arrangement_extension_flag"
#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
  printf("frame_packing_arrangement_extension_flag          = %d\n", seiFramePackingArrangement.frame_packing_arrangement_extension_flag);
#endif

  voFree (pDecGlobal,buf);
  return 0;

#ifdef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
#undef PRINT_FRAME_PACKING_ARRANGEMENT_INFO
#endif
}


typedef struct
{
  unsigned int  tone_map_id;
  unsigned char tone_map_cancel_flag;
  unsigned int  tone_map_repetition_period;
  unsigned char coded_data_bit_depth;
  unsigned char sei_bit_depth;
  unsigned int  model_id;
  // variables for model 0
  int  min_value;
  int  max_value;
  // variables for model 1
  int  sigmoid_midpoint;
  int  sigmoid_width;
  // variables for model 2
  int start_of_coded_interval[1<<MAX_SEI_BIT_DEPTH];
  // variables for model 3
  int num_pivots;
  int coded_pivot_value[MAX_NUM_PIVOTS];
  int sei_pivot_value[MAX_NUM_PIVOTS];
} tone_mapping_struct_tmp;

int interpret_tone_mapping( H264DEC_G *pDecGlobal,byte* payload, int size )
{
  tone_mapping_struct_tmp seiToneMappingTmp;
  Bitstream* buf;
  int i = 0, max_coded_num, max_output_num;

  memset (&seiToneMappingTmp, 0, sizeof (tone_mapping_struct_tmp));

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  seiToneMappingTmp.tone_map_id = ue_v(pDecGlobal);//"SEI: tone_map_id"
  seiToneMappingTmp.tone_map_cancel_flag = (unsigned char) u_v (1, pDecGlobal);//"SEI: tone_map_cancel_flag"

#ifdef PRINT_TONE_MAPPING
  printf("Tone-mapping SEI message\n");
  printf("tone_map_id = %d\n", seiToneMappingTmp.tone_map_id);

  if (seiToneMappingTmp.tone_map_id != 0)
    printf("WARNING! Tone_map_id != 0, print the SEI message info only. The tone mapping is actually applied only when Tone_map_id==0\n\n");
  printf("tone_map_cancel_flag = %d\n", seiToneMappingTmp.tone_map_cancel_flag);
#endif

  if (!seiToneMappingTmp.tone_map_cancel_flag) 
  {
    seiToneMappingTmp.tone_map_repetition_period  = ue_v(pDecGlobal);//"SEI: tone_map_repetition_period"
    seiToneMappingTmp.coded_data_bit_depth        = (unsigned char)u_v (8,pDecGlobal);//"SEI: coded_data_bit_depth"
    seiToneMappingTmp.sei_bit_depth               = (unsigned char)u_v (8,pDecGlobal);//"SEI: sei_bit_depth"
    seiToneMappingTmp.model_id                    = ue_v(pDecGlobal);//"SEI: model_id"

#ifdef PRINT_TONE_MAPPING
    printf("tone_map_repetition_period = %d\n", seiToneMappingTmp.tone_map_repetition_period);
    printf("coded_data_bit_depth = %d\n", seiToneMappingTmp.coded_data_bit_depth);
    printf("sei_bit_depth = %d\n", seiToneMappingTmp.sei_bit_depth);
    printf("model_id = %d\n", seiToneMappingTmp.model_id);
#endif

    max_coded_num  = 1<<seiToneMappingTmp.coded_data_bit_depth;
    max_output_num = 1<<seiToneMappingTmp.sei_bit_depth;

    if (seiToneMappingTmp.model_id == 0) 
    { // linear mapping with clipping
      seiToneMappingTmp.min_value   = GetBits32(pDecGlobal);//u_v (32,pDecGlobal);//"SEI: min_value"
      seiToneMappingTmp.max_value   = GetBits32(pDecGlobal);//u_v (32,pDecGlobal);//"SEI: min_value"
#ifdef PRINT_TONE_MAPPING
      printf("min_value = %d, max_value = %d\n", seiToneMappingTmp.min_value, seiToneMappingTmp.max_value);
#endif
    }
    else if (seiToneMappingTmp.model_id == 1) 
    { // sigmoidal mapping
      seiToneMappingTmp.sigmoid_midpoint = GetBits32(pDecGlobal);//u_v (32,pDecGlobal);//"SEI: sigmoid_midpoint"
      seiToneMappingTmp.sigmoid_width    = GetBits32(pDecGlobal);//u_v (32,pDecGlobal);//"SEI: sigmoid_width"
#ifdef PRINT_TONE_MAPPING
      printf("sigmoid_midpoint = %d, sigmoid_width = %d\n", seiToneMappingTmp.sigmoid_midpoint, seiToneMappingTmp.sigmoid_width);
#endif
    }
    else if (seiToneMappingTmp.model_id == 2) 
    { // user defined table mapping
      for (i=0; i<max_output_num; i++) 
      {
        seiToneMappingTmp.start_of_coded_interval[i] = u_v((((seiToneMappingTmp.coded_data_bit_depth+7)>>3)<<3),pDecGlobal);//"SEI: start_of_coded_interval"
#ifdef PRINT_TONE_MAPPING // too long to print
        //printf("start_of_coded_interval[%d] = %d\n", i, seiToneMappingTmp.start_of_coded_interval[i]);
#endif
      }
    }
    else if (seiToneMappingTmp.model_id == 3) 
    {  // piece-wise linear mapping
      seiToneMappingTmp.num_pivots = u_v (16,pDecGlobal);//"SEI: num_pivots"
#ifdef PRINT_TONE_MAPPING
      printf("num_pivots = %d\n", seiToneMappingTmp.num_pivots);
#endif
	  if (seiToneMappingTmp.num_pivots>=MAX_NUM_PIVOTS)
        VOH264ERROR(VO_H264_ERR_SEI);
      seiToneMappingTmp.coded_pivot_value[0] = 0;
      seiToneMappingTmp.sei_pivot_value[0] = 0;
      seiToneMappingTmp.coded_pivot_value[seiToneMappingTmp.num_pivots+1] = max_coded_num-1;
      seiToneMappingTmp.sei_pivot_value[seiToneMappingTmp.num_pivots+1] = max_output_num-1;

      for (i=1; i < seiToneMappingTmp.num_pivots+1; i++) 
      {
        seiToneMappingTmp.coded_pivot_value[i] = u_v( (((seiToneMappingTmp.coded_data_bit_depth+7)>>3)<<3),pDecGlobal);//"SEI: coded_pivot_value"
        seiToneMappingTmp.sei_pivot_value[i] = u_v( (((seiToneMappingTmp.sei_bit_depth+7)>>3)<<3),pDecGlobal);//"SEI: sei_pivot_value"
#ifdef PRINT_TONE_MAPPING
        printf("coded_pivot_value[%d] = %d, sei_pivot_value[%d] = %d\n", i, seiToneMappingTmp.coded_pivot_value[i], i, seiToneMappingTmp.sei_pivot_value[i]);
#endif
      }
    }

  } // end !tone_map_cancel_flag
  voFree (pDecGlobal,buf);
  return 0;
}


int interpret_post_filter_hints_info( H264DEC_G *pDecGlobal,byte* payload, int size)
{
  Bitstream* buf;
  unsigned int filter_hint_size_y, filter_hint_size_x, filter_hint_type, color_component, cx, cy, additional_extension_flag;
  int ***filter_hint;

  buf = voMalloc(pDecGlobal,sizeof(Bitstream));
  buf->bitstream_length = size;
  buf->streamBuffer = payload;
  buf->frame_bitoffset = 0;
  InitBitStream(pDecGlobal,payload, size);
  //p_Dec->UsedBits = 0;

  filter_hint_size_y = ue_v(pDecGlobal); // interpret post-filter hint SEI here"SEI: filter_hint_size_y"
  filter_hint_size_x = ue_v(pDecGlobal); // interpret post-filter hint SEI here"SEI: filter_hint_size_x"
  filter_hint_type   = u_v(2,pDecGlobal); // interpret post-filter hint SEI here"SEI: filter_hint_type"
  if(filter_hint_size_y==0||filter_hint_size_x==0)
  	VOH264ERROR(VO_H264_ERR_SEI);
  get_mem3Dint (pDecGlobal,&filter_hint, 3, filter_hint_size_y, filter_hint_size_x);

  for (color_component = 0; color_component < 3; color_component ++)
    for (cy = 0; cy < filter_hint_size_y; cy ++)
      for (cx = 0; cx < filter_hint_size_x; cx ++)
        filter_hint[color_component][cy][cx] = se_v(pDecGlobal); // interpret post-filter hint SEI here"SEI: filter_hint"

  additional_extension_flag = u_v (1, pDecGlobal); // interpret post-filter hint SEI here"SEI: additional_extension_flag"

#ifdef PRINT_POST_FILTER_HINT_INFO
  printf(" Post-filter hint SEI message\n");
  printf(" post_filter_hint_size_y %d \n", filter_hint_size_y);
  printf(" post_filter_hint_size_x %d \n", filter_hint_size_x);
  printf(" post_filter_hint_type %d \n",   filter_hint_type);
  for (color_component = 0; color_component < 3; color_component ++)
    for (cy = 0; cy < filter_hint_size_y; cy ++)
      for (cx = 0; cx < filter_hint_size_x; cx ++)
        printf(" post_filter_hint[%d][%d][%d] %d \n", color_component, cy, cx, filter_hint[color_component][cy][cx]);

  printf(" additional_extension_flag %d \n", additional_extension_flag);

#undef PRINT_POST_FILTER_HINT_INFO
#endif

  free_mem3Dint (pDecGlobal,filter_hint);
  voFree(pDecGlobal, buf );
  return 0;
}
