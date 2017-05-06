 /******************************************************************************************
 *                                                                                         *
 *  VisualOn, Inc. Confidential and Proprietary, 2012                                      *
 *                                                                                         *
 *******************************************************************************************/

/** \file     h265dec_parser.c
    \brief    parser
    \author   Renjie Yu
	\change
*/

#include "h265dec_parser.h"
#include "h265dec_vlc.h"
#include "h265dec_dequant.h"//YU_TBD
#include "h265dec_slice.h"

////////////////////////////////////////////////
//extern VOINLINE VO_VOID set_qp(H265_DEC_SLICE *p_slice,VO_S32 qp);


/** Initialize CtbAddrRsToTs CtbAddrTsToRs, TileId and MinTbAddrZs(Extend) in pps
 * \param p_slice [IN/OUT]
 * \param p_sps [IN/OUT]
 * \param p_pps [IN/OUT]
 * \returns error code
 */
static VO_S32 IniCtbAddrTS( H265_DEC_SLICE * p_slice, 
    H265_DEC_SPS *p_sps, 
	H265_DEC_PPS *p_pps )
{
	VO_U32 i, j, k, x, y, err = 0;
	VO_U32 log2_ctb_size    = p_sps->log2_luma_coding_block_size;
	VO_U32 PicWidthInCtbsY  = ( ( p_sps->pic_width_in_luma_samples + ( 1 << log2_ctb_size ) - 1 ) >> log2_ctb_size );
	VO_U32 PicHeightInCtbsY = ( ( p_sps->pic_height_in_luma_samples + ( 1 << log2_ctb_size ) - 1 ) >> log2_ctb_size );
	VO_U32 PicSizeInCtbsY   = PicWidthInCtbsY * PicHeightInCtbsY;
    VO_U32 PicWidthInMinTb  = ( PicWidthInCtbsY << log2_ctb_size ) >> 2;//p_sps->log2_min_transform_block_size;
	VO_U32 PicHeightInMinTb = ( PicHeightInCtbsY << log2_ctb_size)  >> 2;//p_sps->log2_min_transform_block_size;
	VO_U32 log2_diff_ctb_min_tb_size = p_sps->log2_luma_coding_block_size - 2;//p_sps->log2_min_transform_block_size;

	VO_U32 num_tile_columns_minus1 = p_pps->num_tile_columns_minus1;
	VO_U32 num_tile_rows_minus1    = p_pps->num_tile_rows_minus1;
	VO_U32 *p_col_width  = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( num_tile_columns_minus1 + 1 ) * sizeof( VO_U32 ) , CACHE_LINE);
	VO_U32 *p_row_height = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( num_tile_rows_minus1 + 1 ) * sizeof( VO_U32 ) , CACHE_LINE);
	VO_U32 *p_col_bd     = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( num_tile_columns_minus1 + 1 + 1 ) * sizeof( VO_U32 ) , CACHE_LINE);
	VO_U32 *p_row_bd     = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( num_tile_rows_minus1 + 1 + 1) * sizeof( VO_U32 ) , CACHE_LINE);




    /*
    CtbAddrRsToTs
    */
	if ( p_pps->CtbAddrRsToTs ) {
		AlignFree( p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_pps->CtbAddrRsToTs );
	}
	p_pps->CtbAddrRsToTs = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( PicSizeInCtbsY ) * sizeof( *p_pps->CtbAddrRsToTs ) , CACHE_LINE);
	
    /*
    CtbAddrTsToRs
    */
    if ( p_pps->CtbAddrTsToRs ) {
		AlignFree( p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_pps->CtbAddrTsToRs );
	}
	p_pps->CtbAddrTsToRs = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( PicSizeInCtbsY ) * sizeof( *p_pps->CtbAddrTsToRs ) , CACHE_LINE);
	
    /*
    CtbAddrTsToRs
    */
    if ( p_pps->TileId ) {
		AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_pps->TileId );
	}
    p_pps->TileId        = ( VO_U32 * )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, ( PicSizeInCtbsY ) * sizeof( *p_pps->TileId ) ,CACHE_LINE);
    
    /*
    MinTbAddrZs
    */
    p_pps->MinTbAddrZsStride = ( PicWidthInMinTb + 2 );
	if ( p_pps->ExtendMinTbAddrZs ) {
		AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_pps->ExtendMinTbAddrZs);
	}
    p_pps->ExtendMinTbAddrZs = ( VO_S32 * ) AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_pps->MinTbAddrZsStride * (PicHeightInMinTb + 2) * sizeof( *p_pps->ExtendMinTbAddrZs ) , CACHE_LINE);
    p_pps->MinTbAddrZs = p_pps->ExtendMinTbAddrZs + p_pps->MinTbAddrZsStride + 1;
    
	if ( !p_col_width || !p_row_height || \
		 !p_col_bd || !p_row_bd || \
		 !p_pps->CtbAddrRsToTs || !p_pps->TileId ||
         !p_pps->ExtendMinTbAddrZs ) {
		 err = 1;
		 goto IniCtbAddrTSEnd;
	}

    if ( p_pps->uniform_spacing_flag ) {
	    for ( i = 0; i  <=  num_tile_columns_minus1; i++ )
		    p_col_width[ i ] = ( ( ( i + 1 ) * PicWidthInCtbsY ) / ( num_tile_columns_minus1 + 1 ) ) - \
						    ( ( i * PicWidthInCtbsY ) / ( num_tile_columns_minus1 + 1 ) );

		for( i = 0; i  <=  num_tile_rows_minus1; i++ )
		    p_row_height[ i ] = ( ( i + 1 ) * PicHeightInCtbsY ) / ( num_tile_rows_minus1 + 1 ) - \
						( i * PicHeightInCtbsY ) / ( num_tile_rows_minus1 + 1 );

	} else {
	    p_col_width[ num_tile_columns_minus1 ] = PicWidthInCtbsY;
	    for( i = 0; i < num_tile_columns_minus1; i++ ) {
		    p_col_width[ i ] = p_pps->column_width[ i ];
		    p_col_width[ num_tile_columns_minus1 ] -=  p_col_width[ i ];
	    }

		p_row_height[ num_tile_rows_minus1 ] = PicHeightInCtbsY;
	    for( j = 0; j < num_tile_rows_minus1; j++ ) {
		    p_row_height[ j ] = p_pps->row_height[ j ];
		    p_row_height[ num_tile_rows_minus1 ]  -=  p_row_height[ j ];
	    }
    }

    for ( p_col_bd[ 0 ] = 0, i = 0; i  <=  p_pps->num_tile_columns_minus1; i++ )
		p_col_bd[ i + 1 ] = p_col_bd[ i ] + p_col_width[ i ];

    for ( p_row_bd[ 0 ] = 0, j = 0; j  <=  p_pps->num_tile_rows_minus1; j++ )
	    p_row_bd[ j + 1 ] = p_row_bd[ j ] + p_row_height[ j ]; 	

    for ( k = 0; k < PicSizeInCtbsY ; k++ ) {
		VO_U32 i, j, tile_x, tile_y;
	    VO_U32 tb_x = k % PicWidthInCtbsY;
	    VO_U32 tb_y = k / PicWidthInCtbsY;

	    for ( i = 0; i  <=  num_tile_columns_minus1; i++ ) {
		    if( tb_x  >=  p_col_bd[ i ] )
			    tile_x = i;
		}
	    for ( j = 0; j  <=  num_tile_rows_minus1; j++ ) {
		    if ( tb_y  >=  p_row_bd[ j ] )
			    tile_y = j;
		}

	    p_pps->CtbAddrRsToTs[ k ] = 0;
	    for ( i = 0; i < tile_x; i++ ) {
		    p_pps->CtbAddrRsToTs[ k ]  +=  p_row_height[ tile_y ] * p_col_width[ i ] ;
		}
	    for ( j = 0; j < tile_y; j++ ) {
		    p_pps->CtbAddrRsToTs[ k ]  +=  PicWidthInCtbsY * p_row_height[ j ];
		}
	    p_pps->CtbAddrRsToTs[ k ]  +=  ( tb_y - p_row_bd[ tile_y ] ) * p_col_width[ tile_x ] + tb_x - p_col_bd[ tile_x ];
	}

	for ( i = 0; i < PicSizeInCtbsY ; i++ )//	(6 8)
	    p_pps->CtbAddrTsToRs[ p_pps->CtbAddrRsToTs[ i ] ] = i;


//     for ( j = 0, k = 0; j  <=  num_tile_rows_minus1; j++ )
// 	    for ( i = 0; i  <=  num_tile_columns_minus1; i++, k++ )
// 		    for ( y = p_row_bd[ j ]; y < p_row_bd[ j + 1 ]; y++ )
// 			    for ( x = p_col_bd[ i ]; x < p_col_bd[ i + 1 ]; x++ )
// 				    p_pps->TileId[ p_pps->CtbAddrRsToTs[ y * PicWidthInCtbsY + x ] ] = k;
	

// 	for ( j = 0; j < PicHeightInMinTb; j++ ) {
// 		for( i = 0; i < PicWidthInMinTb; i++ ) {
// 			VO_S32 k;
// 			VO_S32 tb_x = i >> log2_diff_ctb_min_tb_size;
// 			VO_S32 tb_y = j >> log2_diff_ctb_min_tb_size;
// 			VO_S32 val = p_pps->CtbAddrRsToTs[( tb_x + tb_y * PicWidthInCtbs ) ]<< ( log2_diff_ctb_min_tb_size * 2 );
// 	
// 			for ( k = 0; k < log2_diff_ctb_min_tb_size; k++ ) {
// 				VO_S32 m = 1 << k;
// 				val += ( m & i ? m * m : 0 ) + ( m & j ? 2 * m * m : 0 );
// 			}
// 
// 			p_pps->MinTbAddrZs[ j * PicWidthInMinTb + i ] = val;
// 		}	
// 	}
   
    // set bound value to -1;
    for ( i = 0; i < p_pps->MinTbAddrZsStride; i++ ) {  //j=-1 for  p_pps->MinTbAddrZs
        p_pps->ExtendMinTbAddrZs[ i ] = -1;
    }	

    for ( j = 0; j < PicHeightInMinTb; j++ ) {
        p_pps->MinTbAddrZs[ j * p_pps->MinTbAddrZsStride - 1 ] = -1;
        for ( i = 0 ; i < PicWidthInMinTb; i++ ) {
            VO_S32 k;
            VO_S32 tb_x = i >> log2_diff_ctb_min_tb_size;
            VO_S32 tb_y = j >> log2_diff_ctb_min_tb_size;
            VO_S32 val = p_pps->CtbAddrRsToTs[ tb_x + tb_y * PicWidthInCtbsY ] << ( log2_diff_ctb_min_tb_size * 2 );

            for ( k = 0; k < log2_diff_ctb_min_tb_size; k++ ) {
                VO_S32 m = 1 << k;
                val += ( m & i ? m * m : 0 ) + ( m & j ? 2 * m * m : 0 );
            }
            p_pps->MinTbAddrZs[ j * p_pps->MinTbAddrZsStride  + i ] = val;
        }

        for ( i =  p_sps->pic_width_in_luma_samples >> 2; i <= PicWidthInMinTb; i++ ) {
            p_pps->MinTbAddrZs[ j * p_pps->MinTbAddrZsStride  + i ] = -1;
        }
    }

    for ( j = p_sps->pic_height_in_luma_samples >> 2; j <= PicHeightInMinTb; j++ ) {
        p_pps->MinTbAddrZs[ j * p_pps->MinTbAddrZsStride  - 1 ] = -1;
        for ( i = 0; i <= PicWidthInMinTb; i++ ) {  //j=PicHeightInMinTb
            p_pps->MinTbAddrZs[ j * p_pps->MinTbAddrZsStride  + i ] = -1;
        }
    }

   // p_slice->currEntryNum = 0;
    for ( j = 0, k = 0; j  <=  num_tile_rows_minus1; j++ ) {
        for ( i = 0; i  <=  num_tile_columns_minus1; i++, k++ ) {
            //VO_U32 rsMinTbIdx = (((p_row_bd[ j ]* PicWidthInCtbsY) << (log2_diff_ctb_min_tb_size + log2_diff_ctb_min_tb_size)) + (p_col_bd[ i ]<<log2_diff_ctb_min_tb_size));
            VO_U32 startX = p_col_bd[ i ] << log2_ctb_size;
            VO_U32 startY = p_row_bd[ j ] << log2_ctb_size;
            VO_U32 rsMinTbIdx = ( startY >> p_sps->log2_min_transform_block_size ) * p_pps->MinTbAddrZsStride + ( startX >> p_sps->log2_min_transform_block_size );
            //p_slice->entries[k].tile_start_zs = p_pps->MinTbAddrZs[rsMinTbIdx];
            p_pps->tile_start_zs[k] = p_pps->MinTbAddrZs[ rsMinTbIdx ];
            for ( y = p_row_bd[ j ]; y < p_row_bd[ j + 1 ]; y++ )
                for ( x = p_col_bd[ i ]; x < p_col_bd[ i + 1 ]; x++ )
                    p_pps->TileId[ p_pps->CtbAddrRsToTs[ y * PicWidthInCtbsY + x ] ] = k;
        }
    }
IniCtbAddrTSEnd:
	if ( p_col_width )
		AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_col_width );
	if ( p_row_height )
		AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_row_height) ;
	if ( p_col_bd )
		AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_col_bd );
	if ( p_row_bd )
		AlignFree(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id, p_row_bd );

	if ( err ) 
		return VO_ERR_OUTOF_MEMORY; 

	return VO_ERR_NONE;
}


static VO_U32 GetGCD( VO_U32 m, 
	VO_U32 n )
{
    if ( m == 0 )
        return n;
    if ( n == 0 )
        return m;

    if ( m < n ) {
        VO_U32  tmp = m;
        m = n;
        n = tmp;
    }

    while ( n != 0 ) {
        VO_U32 tmp = m % n;
        m = n;
        n = tmp;
    }

    return m;
}

#define EXTENDED_SAR     255
const static VO_U32 AspectInfo[ 16 ][ 2 ] = {
{ 1, 1 }, { 12, 11 }, { 10, 11 }, { 16, 11 }, { 40, 33 }, { 24, 11 }, { 20, 11 }, { 32, 11 },
{ 80, 33 }, { 18, 11 }, { 15, 11 }, { 64 , 33 }, { 160, 99 }, { 4, 3 }, { 3, 2 }, { 2, 1 }
};


/** GetAspectRatio
 * \param aspect_ratio_idc [IN]
 * \param pic_width_in_luma_samples [IN]
 * \param pic_height_in_luma_samples [IN]
 * \param sar_width [IN]
 * \param sar_width [IN]
 * \returns VO defined aspect ratio idx
 */
static VO_U32 GetAspectRatio ( const VO_U32 aspect_ratio_idc,
	const VO_U32 pic_width_in_luma_samples,   
	const VO_U32 pic_height_in_luma_samples,  
	const VO_U32 sar_width,
	const VO_U32 sar_height )
{

    VO_U32 width = 0, height = 0;
	VO_U32 ratio = 0;

    if ( aspect_ratio_idc == 0 || aspect_ratio_idc == 1 )
        return 0;

    if ( aspect_ratio_idc > 1 && aspect_ratio_idc < 17 ) {
        width = pic_width_in_luma_samples * AspectInfo[ aspect_ratio_idc - 1][ 0 ];
        height = pic_height_in_luma_samples * AspectInfo[ aspect_ratio_idc - 1][ 1 ];
    } else if ( aspect_ratio_idc == EXTENDED_SAR ) {
        width = pic_width_in_luma_samples * sar_width;
        height = pic_height_in_luma_samples * sar_height;
    }    

    if ( height > 0 )
        ratio = width * 10 / height;

	
    if ( ratio <= 10 )
        return 1;
    else if( ratio <= 42/3 )
        return 2;
    else if( ratio <= 168 / 9 )
        return 3;
    else if( ratio <= 21 )
        return 4;
    else {  
        VO_U32 m = GetGCD( width, height );

        if ( m ) {
            width = width / m;
            height = height / m;
        }
    }

	if ( width > 0x7fff || height > 0x7fff ) {
        width = ( width + 512 ) >> 10;
        height = ( height + 512 ) >> 10;
    }

    return ( width << 16 ) | height;
}

/** ParseVUI
 * \param p_sps [IN]
 * \param p_bs [IN]
 * \returns error code
 */
static VO_S32 ParseVUI( H265_DEC_SPS *p_sps,
	BIT_STREAM *p_bs ) 
{
	VO_U32 code;

	if ( READ_FLAG1( p_bs ) ) {	//aspect_ratio_info_present_flag
		VO_U32 sar_width = 0;
		VO_U32 sar_height = 0;
		VO_U32 aspect_ratio_idc = ReadUV( p_bs, 8 );

		if ( aspect_ratio_idc == EXTENDED_SAR ) {//aspect_ratio_idc	
			sar_width = ReadUV( p_bs, 16 );//sar_width
			sar_height= ReadUV( p_bs, 16 );//sar_height
		}	
		p_sps->aspect_ratio = GetAspectRatio( aspect_ratio_idc, 
			                                  p_sps->pic_width_in_luma_samples,
											  p_sps->pic_height_in_luma_samples,
											  sar_width, 
											  sar_height);
	}	

	if ( READ_FLAG1( p_bs ) ) {	//overscan_info_present_flag
		code = READ_FLAG1( p_bs ); //overscan_appropriate_flag
	}

	if ( READ_FLAG1( p_bs ) ) {	//video_signal_type_present_flag
		code = ReadUV( p_bs, 3 ); //video_format
		code = READ_FLAG1( p_bs );//video_full_range_flag

		if ( READ_FLAG1( p_bs ) ) {	//colour_description_present_flag
			code = ReadUV( p_bs, 8 );//colour_primaries
			code = ReadUV( p_bs, 8 );//transfer_characteristics
			code = ReadUV( p_bs, 8 );//matrix_coeffs
		}	
	}	

	if ( READ_FLAG1( p_bs ) ) {	//chroma_loc_info_present_flag
		code = ReadUEV( p_bs );//chroma_sample_loc_type_top_field
		code = ReadUEV( p_bs );//chroma_sample_loc_type_bottom_field
	}	
	code = READ_FLAG1( p_bs );//neutral_chroma_indication_flag
	code = READ_FLAG1( p_bs );//field_seq_flag
	code = READ_FLAG1( p_bs );//frame_field_info_present_flag

	if ( READ_FLAG1( p_bs ) ) {	//default_display_window_flag
		code = ReadUEV( p_bs ); //def_disp_win_left_offset
		code = ReadUEV( p_bs ); //def_disp_win_right_offset
		code = ReadUEV( p_bs ); //def_disp_win_top_offset
		code = ReadUEV( p_bs ); //def_disp_win_bottom_offset
	}	

	if ( READ_FLAG1( p_bs ) ) {	//vui_timing_info_present_flag
		code = ReadUV( p_bs, 32 );//vui_num_units_in_tick
		code = ReadUV( p_bs, 32 );//vui_time_scale
		
		if ( READ_FLAG1( p_bs ) )	//vui_poc_proportional_to_timing_flag
			code = ReadUEV( p_bs ); //vui_num_ticks_poc_diff_one_minus1

		if ( READ_FLAG1( p_bs ) ) {//vui_hrd_parameters_present_flag
			//VOASSERT(0);
			//hrd_parameters( 1, sps_max_sub_layers_minus1 )	;
		}
	}	

   /*
	if ( READ_FLAG1( p_bs ) ) {	
		code = READ_FLAG1( p_bs ); //tiles_fixed_structure_flag
		code = READ_FLAG1( p_bs ); //motion_vectors_over_pic_boundaries_flag
		code = READ_FLAG1( p_bs ); //restricted_ref_pic_lists_flag
		code = ReadUEV( p_bs ); //min_spatial_segmentation_idc
		code = ReadUEV( p_bs ); //max_bytes_per_pic_denom
		code = ReadUEV( p_bs ); //max_bits_per_min_cu_denom
		code = ReadUEV( p_bs ); //log2_max_mv_length_horizontal
		code = ReadUEV( p_bs ); //log2_max_mv_length_vertical
	}	*/
    return VO_ERR_NONE; 
}


static VO_S32 GetNumRpsCurrTempList( H265_DEC_RPS *p_rps )
{
	VO_U32 num_rps_curr = 0;
	VO_U32 i, num_rps = p_rps->num_negative_pics + p_rps->num_positive_pics + p_rps->num_long_term_pics;

	for ( i = 0; i < num_rps; i++ ) {
		if ( p_rps->used[ i ] ) {
			num_rps_curr++;
		}
	}
	return num_rps_curr;
}


VO_VOID xParsePredWeightTable( H265_DEC_SLICE *p_slice ,
	BIT_STREAM *p_bs )
{
	VO_U32            chroma_format_idc     = 1; //YU_TBD: only support yuv420, get from sps
	H265_DEC_PPS*     p_pps        = p_slice->p_pps;
	VO_S32            num_ref_list = ( p_slice->slice_type == B_SLICE ) ? (2) : (1);
	VO_U32            luma_log2_weight_denom, chroma_log2_weight_denom;
	VO_S32            ref_list;

		// decode delta_luma_log2_weight_denom :
	luma_log2_weight_denom = READ_UEV( p_bs , "luma_log2_weight_denom" );     // ue(v): luma_log2_weight_denom
		
    if ( chroma_format_idc != 0 ) {
		chroma_log2_weight_denom = luma_log2_weight_denom + READ_SEV( p_bs , "delta_chroma_log2_weight_denom" );     // se(v): delta_chroma_log2_weight_denom
	}

	for ( ref_list = 0 ; ref_list < num_ref_list ; ref_list++ ) {
		VO_U32 iRefIdx;
        VO_U32 num_ref_idx = p_slice->num_ref_idx[ ref_list ];
		
		for ( iRefIdx = 0 ; iRefIdx < num_ref_idx ; iRefIdx++ ) {
			p_slice->m_weightPredTable[ ref_list ][ iRefIdx ][ 0 ].log2_weight_denom = luma_log2_weight_denom;
			p_slice->m_weightPredTable[ ref_list ][ iRefIdx ][ 1 ].log2_weight_denom = chroma_log2_weight_denom;
			p_slice->m_weightPredTable[ ref_list ][ iRefIdx ][ 2 ].log2_weight_denom = chroma_log2_weight_denom;

			p_slice->m_weightPredTable[ ref_list ][ iRefIdx ][ 0 ].weight_flag = READ_FLAG( p_bs, "luma_weight_lX_flag" );
		}

		if ( chroma_format_idc != 0 ) {
			VO_U32  uiCode;
	
			for ( iRefIdx = 0 ; iRefIdx < num_ref_idx ; iRefIdx++ ) {
				uiCode = READ_FLAG(  p_bs , "chroma_weight_lX_flag" );      // u(1): chroma_weight_l0_flag

				p_slice->m_weightPredTable[ ref_list ][ iRefIdx ][ 1 ].weight_flag = uiCode;
				p_slice->m_weightPredTable[ ref_list ][ iRefIdx ][ 2 ].weight_flag = uiCode;
			}
		}

		for ( iRefIdx = 0 ; iRefIdx < num_ref_idx ; iRefIdx++ ) { 
			if ( p_slice->m_weightPredTable[ref_list][iRefIdx][0].weight_flag ) {
				VO_S32 iDeltaWeight;

				iDeltaWeight = READ_SEV(p_bs , "delta_luma_weight_lX" );  // se(v): delta_luma_weight_l0[i]
				p_slice->m_weightPredTable[ref_list][iRefIdx][0].weight = \
                    ( iDeltaWeight + ( 1 << luma_log2_weight_denom ) );
				p_slice->m_weightPredTable[ref_list][iRefIdx][0].offset = READ_SEV( p_bs , "luma_offset_lX" );       // se(v): luma_offset_l0[i]
			} else {
				p_slice->m_weightPredTable[ref_list][iRefIdx][0].weight = \
                    ( 1 << luma_log2_weight_denom );
				p_slice->m_weightPredTable[ref_list][iRefIdx][0].offset = 0;
			}

			if ( chroma_format_idc != 0 ) {
				if ( p_slice->m_weightPredTable[ref_list][iRefIdx][1].weight_flag ) {
					VO_S32 j;

					for ( j = 1 ; j < 3 ; j++ ) {
						VO_S32 iDeltaWeight;
						VO_S32 iDeltaChroma, pred ;

						iDeltaWeight = READ_SEV( p_bs, "delta_chroma_weight_lX" );  // se(v): chroma_weight_l0[i][j]
						p_slice->m_weightPredTable[ref_list][iRefIdx][j].weight = \
                            ( iDeltaWeight + ( 1 << chroma_log2_weight_denom ) );

						iDeltaChroma = READ_SEV( p_bs , "delta_chroma_offset_lX" );  // se(v): delta_chroma_offset_l0[i][j]
						pred = ( 128 - ( ( 128*p_slice->m_weightPredTable[ref_list][iRefIdx][j].weight ) >> ( p_slice->m_weightPredTable[ref_list][iRefIdx][j].log2_weight_denom) ) );
						p_slice->m_weightPredTable[ref_list][iRefIdx][j].offset = Clip3(-128, 127, (iDeltaChroma + pred) );
					}
				} else {
					VO_S32 j;

					for ( j = 1 ; j < 3 ; j++ ) { 
						p_slice->m_weightPredTable[ref_list][iRefIdx][j].weight = \
                            ( 1 << chroma_log2_weight_denom );
						p_slice->m_weightPredTable[ref_list][iRefIdx][j].offset = 0;
					}
				}
			}
		}

        //YU_TBD: memory set to clean up
		for ( iRefIdx = num_ref_idx ; iRefIdx < MAX_NUM_REF_PICS ; iRefIdx++ ) {
			p_slice->m_weightPredTable[ref_list][iRefIdx][0].weight_flag = 0;
			p_slice->m_weightPredTable[ref_list][iRefIdx][1].weight_flag = 0;
			p_slice->m_weightPredTable[ref_list][iRefIdx][2].weight_flag = 0;
		}
	}
}


/** Sorts the deltaPOC and Used by current values in the RPS based on the deltaPOC values.
 *  deltaPOC values are sorted with -ve values before the +ve values.  -ve values are in decreasing order.
 *  +ve values are in increasing order.
 * \returns VO_VOID
 */
static VO_VOID SortDeltaPOC( H265_DEC_RPS *p_rps )
{
	VO_U32 j;
	VO_S32 k;
	VO_U32 numNegPics;

	// sort in increasing order (smallest first)
	for( j = 1; j < p_rps->m_numberOfPictures; j++ ) {
		VO_S32 deltaPOC = p_rps->delta_poc[ j ];
		VO_U32 used = p_rps->used[j];

		for ( k = j - 1; k >= 0; k-- ) {
			VO_S32 temp = p_rps->delta_poc[ k ];

			if ( deltaPOC < temp ) {
				p_rps->delta_poc[ k + 1 ] =  temp;
				p_rps->used[ k + 1 ] = p_rps->used[ k ];
				p_rps->delta_poc[ k ] = deltaPOC;
				p_rps->used[ k ] = used;
			}
		}
	}
	// flip the negative values to largest first
	numNegPics = p_rps->num_negative_pics; 
	for ( j = 0, k = numNegPics - 1; j < numNegPics >> 1; j++, k-- ) {
		VO_S32 deltaPOC = p_rps->delta_poc[ j ]; 
		VO_U32  used = p_rps->used[ j ]; 
		
		p_rps->delta_poc[ j ] = p_rps->delta_poc[ k ];
		p_rps->used[ j ] = p_rps->used[ k ];
		p_rps->delta_poc[ k ] = deltaPOC;
		p_rps->used[ k ] = used;
	}
}

static VO_S32 ParseShortTermRefPicSet( H265_DEC_SPS *p_sps, 
	const H265_DEC_RPS *const rps_array, 
	H265_DEC_RPS *p_rps, 
	BIT_STREAM *p_bs,
	VO_S32 idx )
{
	VO_U32 code = 0;
	//BIT_STREAM *p_bs = &p_dec->bit_stream;

	VO_LOG(LOG_FILE,  LOG_MODULE_ALL | LOG_INFO_ALL,   "\n rps idx: %d", idx);

	if ( idx != 0 ) {
	    code = READ_FLAG( p_bs, "inter_ref_pic_set_prediction_flag" );
	}

	if ( code ) { //inter_ref_pic_set_prediction_flag
		const H265_DEC_RPS *p_rps_ref;
		VO_U32  j;//bit,
		VO_S32 delta_rps;
		VO_S32 k = 0, k0 = 0, k1 = 0;

		if ( idx == p_sps->num_short_term_ref_pic_sets ) {
			code = READ_UEV(p_bs, "delta_idx_minus1" ); // delta index of the Reference Picture Set used for prediction minus 1
		} else {
			code = 0;
		}

		p_rps_ref = rps_array +  idx - 1 - code;
		code = READ_UV( p_bs, 1, "delta_rps_sign" ); 
		delta_rps = 1 + READ_UEV(p_bs, "abs_delta_rps_minus1");
		if ( code ) 
			delta_rps = -delta_rps;
		///////////////////////////////////////////////////////////////////YU_TBD
		for ( j = 0 ; j <= p_rps_ref->m_numberOfPictures; j++ ) {
			code = READ_FLAG( p_bs, "used_by_curr_pic_flag" ); //first bit is "1" if Idc is 1 
			if ( !code ) {
				code = READ_FLAG( p_bs, "use_delta_flag" ); //second bit is "1" if Idc is 2, "0" otherwise.
				code = code << 1; //second bit is "1" if refIdc is 2, "0" if refIdc = 0.
			}
			if ( code == 1 || code == 2 ) {
				VO_S32 deltaPOC = delta_rps + ((j < p_rps_ref->m_numberOfPictures)? p_rps_ref->delta_poc[j] : 0);
				p_rps->delta_poc[k] = deltaPOC;
				p_rps->used[k] = (code == 1) ? 1 : 0;

				if ( deltaPOC < 0 ) {
					k0++;
				} else {
					k1++;
				}
				k++;
			}  
		}
		p_rps->m_numberOfPictures = k;
		p_rps->num_negative_pics = k0;
		p_rps->num_positive_pics = k1;
		SortDeltaPOC( p_rps );
		///////////////////////////////////////////////////////////////////////////////////
	} else {
		VO_S32 prev = 0, poc;
		VO_U32 j;
		p_rps->num_negative_pics = READ_UEV( p_bs, "num_negative_pics" ); 
		p_rps->num_positive_pics = READ_UEV( p_bs, "num_positive_pics" ); 

		for ( j = 0 ; j < p_rps->num_negative_pics; j++ ) {
			poc = 1 + READ_UEV( p_bs, "delta_poc_s0_minus1" );
			poc = prev - poc ;
			p_rps->delta_poc[ j ] = poc;
			p_rps->used[ j ] = READ_FLAG( p_bs, "used_by_curr_pic_s0_flag" );
			prev = poc;
		}
		prev = 0;
		for ( j = p_rps->num_negative_pics; j < p_rps->num_negative_pics + p_rps->num_positive_pics; j++ ) {
			poc =  1 + READ_UEV( p_bs, "delta_poc_s1_minus1" );
			poc += prev;
			p_rps->delta_poc[ j ] = poc;
			p_rps->used[j] = READ_FLAG( p_bs, "used_by_curr_pic_s1_flag");
			prev = poc;
		}

		p_rps->m_numberOfPictures = p_rps->num_negative_pics + p_rps->num_positive_pics;
	}

	return VO_ERR_NONE;
}


VO_S32 ParseSliceParamID(H265_DEC* p_dec, H265_DEC_SLICE *p_slice, BIT_STREAM *p_bs)
{
	H265_DEC_PPS *p_pps = p_slice->p_pps;
	H265_DEC_SPS *p_sps = p_slice->p_sps;
	VO_U32 code;
	VO_S32 ret;

	VO_LOG(LOG_FILE,  LOG_MODULE_ALL | LOG_INFO_ALL, "\n Slice header!");

	p_slice->first_slice_segment_in_pic_flag = READ_FLAG( p_bs, "first_slice_segment_in_pic_flag" );

	if ( p_slice->nal_unit_type >= NAL_UNIT_CODED_SLICE_BLA && \
		p_slice->nal_unit_type <=  NAL_UNIT_RESERVED_23 ) {
			READ_FLAG( p_bs, "no_output_of_prior_pics_flag" );
	}

	////////////////////////////////////////////////////YU_TBD
	//@Harry: active pps & sps here
	code = READ_UEV( p_bs, "slice_pic_parameter_set_id" );
#if 1 //YU_TBD: move temp
	if ( p_slice->slice_pic_parameter_set_id != code ) {
		//@Harry: TBD, to support multi-pps we need active pps here
		p_slice->slice_pic_parameter_set_id = code; //TBD
		p_pps = p_slice->p_pps = &p_dec->pps[ code ];//TBD, should use id as index

		if ( p_slice->slice_seq_parameter_set_id != p_pps->pps_seq_parameter_set_id ) {
			//@Harry: TBD, to support multi-sps we need active sps here
			p_sps = p_slice->p_sps = &p_dec->sps[ p_pps->pps_seq_parameter_set_id ]; 
            ret = ConfigDecoder( p_dec, p_slice,  p_sps );
			if ( ret ) { //YU_TBD, how to handle multi sps with different resolution
				return ret;
			}
			p_slice->slice_seq_parameter_set_id = p_pps->pps_seq_parameter_set_id;
		}
		//ActivateParameterSets( p_dec, p_slice );//YU_TBD: Clean up it
    if (p_slice->p_sps->scaling_list_enabled_flag)
    {
      p_slice->pSliceScalingListEntity = &(p_slice->p_sps->spsScalingListEntity);
      if (p_slice->p_pps->pps_scaling_list_data_present_flag)
      {
        p_slice->pSliceScalingListEntity = &(p_slice->p_pps->ppsScalingListEntity);
      }
    
      if (!p_slice->p_pps->pps_scaling_list_data_present_flag && ! p_slice->p_sps->sps_scaling_list_data_present_flag)
      {
        alloc_ScalingList(p_slice,p_slice->pSliceScalingListEntity);
        setDefaultScalingList(p_slice->pSliceScalingListEntity);
      }
      setMMulLevelScale(p_slice, p_slice->pSliceScalingListEntity);    
    }    
	}
#endif
	///////////////////////////////////////////////////////////

	return VO_ERR_NONE;
}

VO_S32 ParseSliceHeader( /*H265_DEC* p_dec,*/ 
	H265_DEC_SLICE *p_slice, 
	BIT_STREAM *p_bs,
	const H265_DEC_RPS* const rps_array,
	const VO_S32 pre_poc ) 
{
	H265_DEC_PPS *p_pps = p_slice->p_pps;
	H265_DEC_SPS *p_sps = p_slice->p_sps;
	H265_DEC_RPS *p_rps = &p_slice->local_rps;

	VO_U32 code;
// 	VO_S32 ret;

	if ( !p_slice->first_slice_segment_in_pic_flag ) {
		if ( p_pps->dependent_slice_segments_enabled_flag ) {
			p_slice->dependent_slice_segment_flag = READ_FLAG( p_bs, "dependent_slice_segment_flag" );
		} else {
			p_slice->dependent_slice_segment_flag = VO_FALSE;
		}

		p_slice->slice_segment_address = READ_UV( p_bs, ceil_log2( p_slice->PicWidthInCtbsY * p_slice->PicHeightInCtbsY ), "slice_segment_address" );    
		p_slice->cur_slice_number++;
        p_slice->m_saoParam->m_uiNumSlicesInPic++;
	} else {
		p_slice->slice_segment_address = 0;
		p_slice->cur_slice_number = 0;
        p_slice->m_saoParam->m_sliceIDMapLCU[0] = 0;
        p_slice->m_saoParam->m_uiNumSlicesInPic = 1;
	}
	//drive SliceAddrRs 
	if ( p_slice->dependent_slice_segment_flag == 0 ) {
        VO_S32 slice_x,slice_y;
		p_slice->SliceAddrRs = p_slice->slice_segment_address;
        // Need Check
        //p_slice->SliceMinTbAddrZs = p_pps->MinTbAddrZs[ p_slice->SliceAddrRs << p_sps->log2_min_transform_block_size ];
        slice_x = (p_slice->SliceAddrRs % p_slice->PicWidthInCtbsY) << (p_sps->log2_luma_coding_block_size - 2);//p_slice->MinTbLog2SizeY
        slice_y = (p_slice->SliceAddrRs / p_slice->PicWidthInCtbsY) << (p_sps->log2_luma_coding_block_size - 2);//p_slice->MinTbLog2SizeY
		p_slice->SliceMinTbAddrZs = p_pps->MinTbAddrZs[ slice_y * (p_slice->p_pps->MinTbAddrZsStride) + slice_x ];
	}

	//if ( p_slice->dependent_slice_segment_flag ) {
	//	p_slice->m_bNextSlice = VO_FALSE;
	//	p_slice->m_bNextDependentSlice = VO_TRUE;
	//}
	//else
	//{
	//	p_slice->m_bNextSlice = VO_TRUE;
	//	p_slice->m_bNextDependentSlice = VO_FALSE;
	//
	//	p_slice->m_uiSliceCurStartCUAddr = sliceAddress ;
	//	p_slice->m_uiSliceCurEndCUAddr = numCUs*maxParts;
	//}

	if ( !p_slice->dependent_slice_segment_flag ) {
		if ( p_pps->num_extra_slice_header_bits ) {
			READ_UV( p_bs, p_pps->num_extra_slice_header_bits, "slice_reserved_flag" );
		}

		p_slice->slice_type = READ_UEV ( p_bs, "slice_type" );
		if ( p_pps->output_flag_present_flag ) {
			READ_FLAG( p_bs, "pic_output_flag" );//YU_TBD: Check usage
		}

		if ( p_sps->separate_colour_plane_flag  ==  1 ) {
			READ_UV ( p_bs, 2, "colour_plane_id" );//YU_TBD: Support
			VOASSERT(0);
		}

		if ( p_slice->nal_unit_type ==  NAL_UNIT_CODED_SLICE_IDR || \
			 p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_IDR_N_LP ) {//YU_TBD: Check spec
			p_slice->m_iPOC = 0;
			p_rps->num_negative_pics = 0;
			p_rps->num_positive_pics = 0;
			p_rps->num_long_term_pics = 0;
			p_rps->m_numberOfPictures = 0;
		} else {
			VO_S32 iPrevPOC,iMaxPOClsb,iPrevPOClsb,iPrevPOCmsb,iPOCmsb,iPOClsb;

			iPOClsb = READ_UV( p_bs, p_sps->log2_max_pic_order_cnt_lsb, "pic_order_cnt_lsb");  
			////////////////////////////////////////////////////////YU_TBD
			iPrevPOC = pre_poc;
			iMaxPOClsb = 1 << p_sps->log2_max_pic_order_cnt_lsb;
			iPrevPOClsb = iPrevPOC % iMaxPOClsb;
			iPrevPOCmsb = iPrevPOC - iPrevPOClsb;

			if ( ( iPOClsb  <  iPrevPOClsb ) && ( ( iPrevPOClsb - iPOClsb )  >=  ( iMaxPOClsb / 2 ) ) ) {
				iPOCmsb = iPrevPOCmsb + iMaxPOClsb;
			} else if( (iPOClsb  >  iPrevPOClsb )  && ( (iPOClsb - iPrevPOClsb )  >  ( iMaxPOClsb / 2 ) ) ) {
				iPOCmsb = iPrevPOCmsb - iMaxPOClsb;
			} else {
				iPOCmsb = iPrevPOCmsb;
			}
			if ( p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA
				|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLANT
				|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP ) {
					// For BLA picture types, POCmsb is set to 0.
				iPOCmsb = 0;
			}
			p_slice->m_iPOC = iPOCmsb + iPOClsb;
			//////////////////////////////////////////////////////////

			if ( !READ_FLAG1( p_bs ) ) {// short_term_ref_pic_set_sps_flag
				ParseShortTermRefPicSet( p_sps, rps_array, p_rps, p_bs, p_sps->num_short_term_ref_pic_sets);
			} else if ( p_sps->num_short_term_ref_pic_sets > 1 ) {// use reference to short-term reference picture set in PPS
				code = READ_UV( p_bs, ceil_log2( p_sps->num_short_term_ref_pic_sets ), "short_term_ref_pic_set_idx");
				//p_rps = &p_dec->rps[code]; //YU_TBD: muli-thread usage
				*p_rps = rps_array[code] ; //Harry: use local always
			} else {
                *p_rps = rps_array[ 0 ] ;
            }
			p_slice->p_rps = p_rps;

			if ( p_sps->long_term_ref_pics_present_flag ) {
				VO_U32 i;
				VO_U32 offset = p_rps->num_negative_pics + p_rps->num_positive_pics;
				VO_U32 numOfLtrp = 0;
				VO_U32 num_long_term_sps = 0;

				VO_S32 maxPicOrderCntLSB;
				VO_S32 prevLSB, prevDeltaMSB, deltaPocMSBCycleLT;

				if ( p_sps->num_long_term_ref_pic_sps > 0 ) {
					num_long_term_sps = READ_UEV( p_bs, "num_long_term_sps");
					//numOfLtrp += num_long_term_sps;
					//p_rps->num_long_term_pics = numOfLtrp;
				}
				p_rps->num_long_term_pics = READ_UEV( p_bs, "num_long_term_pics");
				//numOfLtrp += p_rps->num_long_term_pics;
				//p_rps->num_long_term_pics = numOfLtrp;
				maxPicOrderCntLSB = 1 << p_sps->log2_max_pic_order_cnt_lsb;
				//prevLSB = 0, prevDeltaMSB = 0, deltaPocMSBCycleLT = 0;

				for ( i = 0; num_long_term_sps + p_rps->num_long_term_pics ; i++ ) {
					VO_S32 poc_lsb_lt = 0;

					if ( i < num_long_term_sps ) {
						//VO_S32 usedByCurrFromSPS;
						if ( p_sps->num_long_term_ref_pic_sps > 1 ) {
							VO_U32 lt_idx_sps = READ_UV( p_bs, ceil_log2( p_sps->num_long_term_ref_pic_sps ), "lt_idx_sps[i]" );
                            p_rps->used[ i ] =  p_sps->used_by_curr_pic_lt_sps_flag[ lt_idx_sps ];
						}
					} else {
						poc_lsb_lt = READ_UV( p_bs, p_sps->log2_max_pic_order_cnt_lsb, "poc_lsb_lt[i]" );//YU_TBD, not stored
						p_rps->used[ i ] =  READ_FLAG( p_bs, "used_by_curr_pic_lt_flag[i]" );
					}

					/////////////////////////////////////////////////////////////////YU_TBD
					if ( READ_FLAG1( p_bs ) ) {//  "delta_poc_msb_present_flag"             
						VO_S32 pocLTCurr = 0;

						code = READ_UEV( p_bs, "delta_poc_msb_cycle_lt[i]" );

						if ( ( i == offset + p_rps->num_long_term_pics - 1 ) || ( i == offset + ( numOfLtrp - num_long_term_sps ) - 1 ) || ( poc_lsb_lt != prevLSB ) ) {//deltaFlag = true;
							deltaPocMSBCycleLT = code;
						} else {
							deltaPocMSBCycleLT = code + prevDeltaMSB;
						}

						pocLTCurr = p_slice->m_iPOC  - deltaPocMSBCycleLT * maxPicOrderCntLSB  - iPOClsb + poc_lsb_lt;  

						p_rps->m_POC[ i ]		   = pocLTCurr;
						p_rps->delta_poc[ i ]	   = p_slice->m_iPOC + pocLTCurr;
						p_rps->m_bCheckLTMSB[ i ]  = VO_TRUE;
					} else {
						p_rps->m_POC[ i ]          = poc_lsb_lt;
						p_rps->delta_poc[ i ]      = -p_slice->m_iPOC + poc_lsb_lt;
						p_rps->m_bCheckLTMSB[ i ]  = VO_FALSE;
					}
					prevLSB = poc_lsb_lt;
					prevDeltaMSB = deltaPocMSBCycleLT;
				}
				offset += p_rps->num_long_term_pics;
				p_rps->m_numberOfPictures = offset;

				///////////////////////////////////////////////////////
			}
			if ( p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA
				|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLANT
				|| p_slice->nal_unit_type == NAL_UNIT_CODED_SLICE_BLA_N_LP ) {
					// In the case of BLA picture types, rps data is read from slice header but ignored
					p_rps->num_negative_pics = 0;
					p_rps->num_positive_pics = 0;
					p_rps->num_long_term_pics = 0;
					p_rps->m_numberOfPictures = 0;
			}

			if ( p_sps->sps_temporal_mvp_enable_flag ) {
				p_slice->enable_temporal_mvp_flag = READ_FLAG( p_bs, "enable_temporal_mvp_flag" );
			} else {
				p_slice->enable_temporal_mvp_flag = VO_FALSE;
			}
		}

		if ( p_sps->sample_adaptive_offset_enabled_flag ) {
			p_slice->slice_sao_luma_flag = READ_FLAG( p_bs, "slice_sao_luma_flag");
			p_slice->slice_sao_chroma_flag = READ_FLAG( p_bs, "slice_sao_chroma_flag");
		}

		if ( p_slice->slice_type != I_SLICE ) {
			VO_U32 NumPocTotalCurr ;

			if ( READ_FLAG1( p_bs ) ) { //num_ref_idx_active_override_flag
				p_slice->num_ref_idx[ REF_PIC_LIST_0 ] = 1 + READ_UEV( p_bs, "num_ref_idx_l0_active_minus1" );
				if ( p_slice->slice_type == B_SLICE ) {
					p_slice->num_ref_idx[ REF_PIC_LIST_1 ] = 1 + READ_UEV( p_bs, "num_ref_idx_l1_active_minus1" );
				}// else {
				//	p_slice->num_ref_idx[ REF_PIC_LIST_1 ] = 0;
				//}
			} else {
				p_slice->num_ref_idx[ REF_PIC_LIST_0 ] = p_pps ->num_ref_idx_l0_default_active; 
				if ( p_slice->slice_type == B_SLICE ) {
					p_slice->num_ref_idx[ REF_PIC_LIST_1 ] = p_pps->num_ref_idx_l1_default_active;
				}// else {
				//	p_slice->num_ref_idx[ REF_PIC_LIST_1 ] = 0;
				//}
			}

			NumPocTotalCurr = GetNumRpsCurrTempList( p_rps );
			if ( p_pps->lists_modification_present_flag && NumPocTotalCurr > 1 ) {//YU_TBD: check GetNumRpsCurrTempList
				VO_U32 i;
				p_slice->ref_pic_list_modification_flag_l0  = READ_FLAG( p_bs, "ref_pic_list_modification_flag_l0" ); 

				if ( p_slice->ref_pic_list_modification_flag_l0 ) {
					for ( i = 0; i < p_slice->num_ref_idx[ REF_PIC_LIST_0 ]; i++ ) {
						p_slice->list_entry_l0[i] = ( VO_U8 )READ_UV( p_bs, ceil_log2( NumPocTotalCurr ), "list_entry_l0" );
					}
				}// else {//YU_TBD: Really need initilization for default values???
				//	for ( i = 0; i < p_slice->num_ref_idx[ REF_PIC_LIST_0 ]; i++ ) {
				//		p_slice->list_entry_l0[i] = 0;
				//	}
				//}
				if ( p_slice->slice_type == B_SLICE ) {
					p_slice->ref_pic_list_modification_flag_l1 = READ_FLAG( p_bs, "ref_pic_list_modification_flag_l1" );
					if ( p_slice->ref_pic_list_modification_flag_l1 ) {
						for ( i = 0; i < p_slice->num_ref_idx[ REF_PIC_LIST_1 ]; i++ ) {
							p_slice->list_entry_l1[i] = READ_UV( p_bs, ceil_log2( NumPocTotalCurr), "list_entry_l1" );
						}
					}// else {
					//	for ( i = 0; i < p_slice->num_ref_idx[ REF_PIC_LIST_1 ]; i++ ) {
					//		p_slice->list_entry_l1[i] = 0;
					//	}
					//}
				}
			} else {
				p_slice->ref_pic_list_modification_flag_l0 = 0;
				p_slice->ref_pic_list_modification_flag_l1 = 0;
			}

			if ( p_slice->slice_type == B_SLICE ) {
				p_slice->mvd_l1_zero_flag = READ_FLAG( p_bs, "mvd_l1_zero_flag" );
			}

			if ( p_pps->cabac_init_present_flag ) { 
				p_slice->cabac_init_flag = READ_FLAG( p_bs, "cabac_init_flag" );
			} else {
				p_slice->cabac_init_flag = VO_FALSE;
			}

			if ( p_slice->enable_temporal_mvp_flag ) {
				if ( p_slice->slice_type == B_SLICE ) {
					p_slice->collocated_from_l0_flag = READ_FLAG( p_bs, "collocated_from_l0_flag" );
				} else {
					p_slice->collocated_from_l0_flag = 1;
				}

				if ( ( ( p_slice->collocated_from_l0_flag == 1 && p_slice->num_ref_idx[ REF_PIC_LIST_0 ] > 1 ) ||
					( p_slice->collocated_from_l0_flag == 0 && p_slice->num_ref_idx[ REF_PIC_LIST_1 ] > 1 ) ) ) {
					p_slice->collocated_ref_idx = READ_UEV( p_bs, "collocated_ref_idx" );
				} else {
					p_slice->collocated_ref_idx = 0; //YU_TBD:Check default value
				}
			}
			if ( ( p_pps->weighted_pred_flag && p_slice->slice_type == P_SLICE ) 
				|| ( p_pps->weighted_bipred_flag && p_slice->slice_type == B_SLICE ) ) {
					xParsePredWeightTable( p_slice, p_bs );
			}
			code = MRG_MAX_NUM_CANDS - READ_UEV( p_bs, "max_num_merge_cand" );
			p_slice->max_num_merge_cand = Clip3( 0, 6, code);//range [0, 5]
		}else{
      p_slice->cabac_init_flag = VO_FALSE;
    }

		p_slice->slice_qp = 26 + p_pps->pic_init_qp_minus26 + READ_SEV( p_bs, "slice_qp_delta" );
        set_qp(p_slice,p_slice->slice_qp); 

		if ( p_pps->pps_slice_chroma_qp_offsets_present_flag ) {
			p_slice->slice_qp_delta_cb = READ_SEV( p_bs, "slice_qp_delta_cb" );
			p_slice->slice_qp_delta_cr = READ_SEV( p_bs, "slice_qp_delta_cr" );
		}

		if ( p_pps->deblocking_filter_control_present_flag ) {
      VO_U32 deblocking_filter_override_flag;
      if(p_pps->deblocking_filter_override_enabled_flag){
        deblocking_filter_override_flag = READ_FLAG1( p_bs ) ; 
      }else{
        deblocking_filter_override_flag = VO_FALSE;
      }
			if ( deblocking_filter_override_flag ) {
				p_slice->slice_deblocking_filter_disabled_flag  = READ_FLAG ( p_bs, "slice_deblocking_filter_disabled_flag" );
				if ( !p_slice->slice_deblocking_filter_disabled_flag  ) {
					p_slice->beta_offset_div2 = READ_SEV( p_bs, "beta_offset_div2" );
					p_slice->tc_offset_div2 = READ_SEV( p_bs, "tc_offset_div2" );
				}
			} else {
				p_slice->slice_deblocking_filter_disabled_flag  = p_pps->pps_deblocking_filter_disabled_flag;
				p_slice->beta_offset_div2 = p_pps->pps_beta_offset_div2;
				p_slice->tc_offset_div2 = p_pps->pps_tc_offset_div2;
			}
		} else {
			p_slice->slice_deblocking_filter_disabled_flag  = 0;
			p_slice->beta_offset_div2 = 0;
			p_slice->tc_offset_div2 = 0;
		}

		if ( p_pps->pps_loop_filter_across_slices_enabled_flag && \
			( p_slice->slice_sao_luma_flag || p_slice->slice_sao_chroma_flag || !p_slice->slice_deblocking_filter_disabled_flag ) ) {
				p_slice->slice_loop_filter_across_slices_enabled_flag = READ_FLAG( p_bs, "slice_loop_filter_across_slices_enabled_flag");
		} else {
			p_slice->slice_loop_filter_across_slices_enabled_flag = p_pps ->pps_loop_filter_across_slices_enabled_flag;
		}
	}
	if ( p_pps->tiles_enabled_flag || p_pps->entropy_coding_sync_enabled_flag ) {
		VO_U32 num_entry_point_offsets, offset_len;
		VO_U32 idx;

		//VOASSERT(0);
		num_entry_point_offsets = READ_UEV( p_bs, "num_entry_point_offsets" );
		//p_slice->num_entry_point_offsets = num_entry_point_offsets;
		if ( num_entry_point_offsets > 0 ) {
			offset_len = 1 + READ_UEV( p_bs, "offset_len_minus1" );
		}

		//p_slice->entry_point_offset =( VO_U32 * ) AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  sizeof( VO_U32 ) * num_entry_point_offsets , CACHE_LINE);
		for ( idx = 0; idx < num_entry_point_offsets; idx++ ) {
			VO_S32 entry_point_offset = READ_UV( p_bs, offset_len, "entry_point_offset" );
			p_slice->entries[idx].entry_point_offset = ( entry_point_offset + 1 ) << 3;
		}
		p_slice->num_entry_point_offsets = num_entry_point_offsets;
	} else {
		p_slice->num_entry_point_offsets = 0;
	}

	if ( p_pps->slice_segment_header_extension_present_flag ) {
		VO_U32 i, exten_len;

		exten_len = READ_UEV( p_bs, "slice_header_extension_length" );
		for ( i = 0; i < exten_len; i++ ) {
			READ_UV( p_bs, 8, "slice_header_extension_data_byte" );
		}
	}
	ReadByteAlignment( p_bs );

  p_pps->m_iNumSubstreams = p_pps->entropy_coding_sync_enabled_flag ?((p_sps->pic_height_in_luma_samples + (1<<p_sps->log2_luma_coding_block_size) - 1)>> p_sps->log2_luma_coding_block_size) : 1;

	return VO_ERR_NONE;
}



static VO_VOID ParseProfileTier( BIT_STREAM *p_bs, 
	ProfileTierLevel *ptl )
{
	VO_S32 j;

    ptl->general_profile_space = READ_UV( p_bs, 2, "general_profile_space" );
	ptl->general_tier_flag =  READ_FLAG( p_bs, "general_tier_flag" );
	ptl->general_profile_idc = READ_UV( p_bs, 5, "general_profile_idc" );
	
    for(j = 0; j < 32; j++) {
		UPDATE_CACHE( p_bs );
		ptl->general_profile_compatibility_flag[ j ] = READ_FLAG( p_bs, "general_profile_compatibility_flag" ) ;
    }

    UPDATE_CACHE_LARGE(p_bs);
    READ_FLAG( p_bs, "general_progressive_source_flag" );
    READ_FLAG( p_bs, "general_interlaced_source_flag" );
    READ_FLAG( p_bs, "general_non_packed_constraint_flag" );
    READ_FLAG( p_bs, "general_frame_only_constraint_flag" );
	READ_UV( p_bs, 16, "general_reserved_zero_16bits" );
	READ_UV( p_bs, 16, "general_reserved_zero_16bits" );
    READ_UV( p_bs, 12, "general_reserved_zero_12bits" );
}

static VO_VOID ParsePTL( BIT_STREAM *p_bs, 
	TComPTL *rpcPTL, 
	VO_S32 maxNumSubLayersMinus1 )
{
    VO_S32 i;

    ParseProfileTier(p_bs, &rpcPTL->m_generalPTL);
    rpcPTL->m_generalPTL.sub_layer_level_idc = READ_UV( p_bs, 8, "sub_layer_level_idc" );

    for ( i = 0; i < maxNumSubLayersMinus1; i++ ) {
        rpcPTL->sub_layer_profile_present_flag[i] = READ_FLAG( p_bs, "sub_layer_profile_present_flag[i]" );
        rpcPTL->sub_layer_level_present_flag[i] = READ_FLAG( p_bs, "sub_layer_level_present_flag[i]" );
    }
  
    if ( maxNumSubLayersMinus1 > 0 ) {
        for ( i = maxNumSubLayersMinus1; i < 8; i++ ) {
            READ_UV( p_bs, 2, "reserved_zero_2bits" );
        }
    }

    for ( i = 0; i < maxNumSubLayersMinus1; i++ ) {
	    if ( rpcPTL->sub_layer_profile_present_flag[ i ] ) {
            ParseProfileTier(p_bs, &rpcPTL->m_subLayerPTL[i]);
        }
        if ( rpcPTL->sub_layer_level_present_flag[i] ) {
		    rpcPTL->m_subLayerPTL[i].sub_layer_level_idc = READ_UV( p_bs,8, "sub_layer_level_idc[i]" );;
        }
    }
}


VO_S32 ParsePPS( H265_DEC_SLICE * p_slice, H265_DEC_SPS* sps_array ,
	H265_DEC_PPS* pps_array , 
	BIT_STREAM *p_bs )
{
	//BIT_STREAM *p_bs = &p_dec->bit_stream;
	H265_DEC_PPS *p_pps;
	VO_U32 i;
	VO_S32 code, ret;

    VO_LOG(LOG_FILE,  LOG_MODULE_ALL | LOG_INFO_ALL, "\n PPS!");

	code = READ_UEV( p_bs, "pps_pic_parameter_set_id" );
    //YU_TBD, check code range
	p_pps = pps_array + code ;

#if USE_FRAME_THREAD
  //Harry: temp resolution for more than one PPS
  if ( p_slice->nThdNum> 1 ) {
    if ( p_pps->Valid ) {
      //wait for other thread finish decoding
      VO_U32 i;
      for ( i = 0; i < p_slice->nThdNum; ++i ) {
        h265FrameThdInfo *pThdInfo_t = p_slice->pThdInfo->pDecGlobal->pFrameThdPool->thdInfo + i;
        THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
      }		
    }
  }
#endif

	p_pps->pps_seq_parameter_set_id = READ_UEV( p_bs, "pps_seq_parameter_set_id" );
	p_pps->dependent_slice_segments_enabled_flag = READ_FLAG(p_bs, "dependent_slice_segments_enabled_flag" );
	p_pps->output_flag_present_flag = READ_FLAG( p_bs, "output_flag_present_flag" );
	p_pps->num_extra_slice_header_bits = READ_UV( p_bs, 3, "num_extra_slice_header_bits" );
	p_pps->sign_data_hiding_enabled_flag = READ_FLAG( p_bs, "sign_data_hiding_enabled_flag" );					
	p_pps->cabac_init_present_flag = READ_FLAG( p_bs,  "cabac_init_present_flag" );					
	p_pps->num_ref_idx_l0_default_active = 1 + READ_UEV( p_bs, "num_ref_idx_l0_default_active_minus1" );			 
	p_pps->num_ref_idx_l1_default_active = 1 + READ_UEV( p_bs, "num_ref_idx_l1_default_active_minus1" );			
	p_pps->pic_init_qp_minus26 = READ_SEV( p_bs, "pic_init_qp_minus26" );							
	p_pps->constrained_intra_pred_flag = READ_FLAG( p_bs, "constrained_intra_pred_flag" );					
	p_pps->transform_skip_enabled_flag = READ_FLAG( p_bs, "transform_skip_enabled_flag" );               

	p_pps->cu_qp_delta_enabled_flag = READ_FLAG( p_bs, "cu_qp_delta_enabled_flag" );  
	if ( p_pps->cu_qp_delta_enabled_flag) {
		p_pps->diff_cu_qp_delta_depth = READ_UEV( p_bs, "diff_cu_qp_delta_depth" );
	} else {
		p_pps->diff_cu_qp_delta_depth = 0;//YU_TBD, ini
	}

	p_pps->pps_chroma_qp_offset[ 0 ] = READ_SEV( p_bs, "pps_cb_qp_offset" );
	p_pps->pps_chroma_qp_offset[ 1 ] = READ_SEV( p_bs, "pps_cr_qp_offset" );
	p_pps->pps_slice_chroma_qp_offsets_present_flag = READ_FLAG( p_bs, "pps_slice_chroma_qp_offsets_present_flag" );

	p_pps->weighted_pred_flag = READ_FLAG( p_bs, "weighted_pred_flag" );          // Use of Weighting Prediction (P_SLICE)
	p_pps->weighted_bipred_flag = READ_FLAG( p_bs, "weighted_bipred_flag" );         // Use of Bi-Directional Weighting Prediction (B_SLICE)
	
	UPDATE_CACHE(p_bs);
	p_pps->transquant_bypass_enabled_flag = READ_FLAG( p_bs, "transquant_bypass_enabled_flag" );
	p_pps->tiles_enabled_flag = READ_FLAG( p_bs, "tiles_enabled_flag" ); 
	p_pps->entropy_coding_sync_enabled_flag = READ_FLAG( p_bs, "entropy_coding_sync_enabled_flag" );

	if ( p_pps->tiles_enabled_flag ) {
		p_pps->num_tile_columns_minus1 = READ_UEV( p_bs, "num_tile_columns_minus1" );	
		p_pps->num_tile_rows_minus1 = READ_UEV( p_bs, "num_tile_rows_minus1" );		
		p_pps->uniform_spacing_flag = READ_FLAG( p_bs, "uniform_spacing_flag" );	
	
		if ( !p_pps->uniform_spacing_flag ) {
	        if ( p_pps->column_width ) {
                AlignFree( p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  p_pps->column_width );
            }
            if ( p_pps->row_height ) {
                AlignFree( p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  p_pps->row_height );
            }		    
            p_pps->column_width = (VO_U32*)AlignMalloc( p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  sizeof( VO_U32 ) * p_pps->num_tile_columns_minus1 , CACHE_LINE);
			if ( !p_pps->column_width )
                return VO_ERR_OUTOF_MEMORY;

            p_pps->row_height = ( VO_U32* )AlignMalloc(p_slice->p_user_op_all->p_user_op, p_slice->p_user_op_all->codec_id,  sizeof( VO_U32 ) * p_pps->num_tile_rows_minus1, CACHE_LINE);        
			if ( !p_pps->row_height )
                return VO_ERR_OUTOF_MEMORY;

            for( i = 0; i < p_pps->num_tile_columns_minus1; i++ ) {
				p_pps->column_width[i] = 1 + READ_UEV( p_bs, "column_width_minus1" ); 
			}

			for ( i = 0; i < p_pps->num_tile_rows_minus1; i++ ){
				p_pps->row_height[ i ] =  1 + READ_UEV( p_bs, "row_height_minus1" ); 
			}
		}

		if ( p_pps->num_tile_columns_minus1 != 0 || p_pps->num_tile_rows_minus1 != 0 ) {
			p_pps->loop_filter_across_tiles_enabled_flag = READ_FLAG( p_bs, "loop_filter_across_tiles_enabled_flag" );  
		}
	}

	p_pps->pps_loop_filter_across_slices_enabled_flag = READ_FLAG( p_bs, "pps_loop_filter_across_slices_enabled_flag" );
	p_pps->deblocking_filter_control_present_flag = READ_FLAG( p_bs, "deblocking_filter_control_present_flag" ); 

	if ( p_pps->deblocking_filter_control_present_flag ) {
		p_pps->deblocking_filter_override_enabled_flag = READ_FLAG( p_bs, "deblocking_filter_override_enabled_flag" );
		p_pps->pps_deblocking_filter_disabled_flag = READ_FLAG( p_bs, "pps_deblocking_filter_disabled_flag" );
			
		if ( !p_pps->pps_deblocking_filter_disabled_flag ) {
			p_pps->pps_beta_offset_div2 = READ_SEV( p_bs, "pps_beta_offset_div2" );	 
			p_pps->pps_tc_offset_div2 = READ_SEV( p_bs, "pps_tc_offset_div2" );		 
		}
	}
	p_pps->pps_scaling_list_data_present_flag = READ_FLAG( p_bs, "pps_scaling_list_data_present_flag" );                 ///pcPPS->setScalingListPresentFlag ( (uiCode==1)?true:false );

    if ( p_pps->pps_scaling_list_data_present_flag ) {
        alloc_ScalingList(p_slice, &p_pps->ppsScalingListEntity );
        scaling_list_data(&p_pps->ppsScalingListEntity, p_bs );
    }
	
	p_pps->lists_modification_present_flag = READ_FLAG( p_bs, "lists_modification_present_flag");
	p_pps->log2_parallel_merge_level_minus2 = READ_UEV( p_bs, "log2_parallel_merge_level_minus2");
	p_pps->slice_segment_header_extension_present_flag = READ_FLAG(p_bs, "slice_segment_header_extension_present_flag");


	//YU_TBD
	if ( READ_FLAG1( p_bs ) ) {//  "pps_extension_flag"
	}

	//YU_TBD
	//rbsp_trailing_bits( );
	ret = IniCtbAddrTS( p_slice, sps_array + p_pps->pps_seq_parameter_set_id , p_pps );
    if ( ret != VO_ERR_NONE )
        return ret;

	p_pps->Valid = VO_TRUE;
	return VO_ERR_NONE;
}

VO_S32 ParseSPS( H265_DEC* const p_dec,
    H265_DEC_SPS *sps_array,
	H265_DEC_RPS *rps_array,
	BIT_STREAM *p_bs )
{
	H265_DEC_SPS *p_sps;
	VO_U32 i, sps_max_sub_layers_minus1;
	VO_U32 code;
	TComPTL sps_ptl;

    VO_LOG(LOG_FILE,  LOG_MODULE_ALL | LOG_INFO_ALL, "\n SPS!");

	READ_UV( p_bs, 4, "sps_video_parameter_set_id" );
	sps_max_sub_layers_minus1 = READ_UV( p_bs, 3, "sps_max_sub_layers_minus1" );
	READ_FLAG( p_bs, "sps_temporal_id_nesting_flag" );

	ParsePTL( p_bs, &sps_ptl, sps_max_sub_layers_minus1 );
	code = READ_UEV( p_bs, "slice_seq_parameter_set_id" );  
	p_sps = sps_array + code ;

#if USE_FRAME_THREAD
  //Harry: temp resolution for more than one PPS
  if ( p_dec->nThdNum > 1 ) {
    if ( p_sps->Valid ) {
      //wait for other thread finish decoding
      VO_U32 i;
      for ( i = 0; i < p_dec->p_avalable_slice->nThdNum; ++i ) {
        h265FrameThdInfo *pThdInfo_t = p_dec->p_avalable_slice->pThdInfo->pDecGlobal->pFrameThdPool->thdInfo + i;
        THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
      }		
    }
  }
#endif
	code = READ_UEV( p_bs, "chroma_format_idc" );  

    VOASSERT( code == 1 );
    //YU_TBD: separate color plane
	if( code == 3 ) {
        p_sps->separate_colour_plane_flag = READ_FLAG( p_bs, "separate_colour_plane_flag");
    }
	p_sps->pic_width_in_luma_samples =  READ_UEV(p_bs, "pic_width_in_luma_samples" ); 
	p_sps->pic_height_in_luma_samples = READ_UEV(p_bs, "pic_height_in_luma_samples" ); 

	if ( READ_FLAG1( p_bs ) ) {//conformance_window_flag
        p_sps->conf_win_left_offset = READ_UEV( p_bs, "conf_win_left_offset" );
        p_sps->conf_win_right_offset = READ_UEV( p_bs, "conf_win_right_offset" );
        p_sps->conf_win_top_offset = READ_UEV( p_bs, "conf_win_top_offset" );
        p_sps->conf_win_bottom_offset = READ_UEV( p_bs, "conf_win_bottom_offset" );
	}

	p_sps->bit_depth_luma = 8 + READ_UEV( p_bs, "bit_depth_luma_minus8" );
	p_sps->bit_depth_chroma = 8 + READ_UEV(p_bs, "bit_depth_chroma_minus8" );

	p_sps->log2_max_pic_order_cnt_lsb = 4 + READ_UEV( p_bs, "log2_max_pic_order_cnt_lsb_minus4" );
	code = READ_FLAG( p_bs, "sps_sub_layer_ordering_info_present_flag" );

	for ( i = code ? 0 : sps_max_sub_layers_minus1; \
		i <= sps_max_sub_layers_minus1; i++ ) { 
		p_sps->sps_max_dec_pic_buffering_minus1[ i ] = READ_UEV( p_bs, "sps_max_dec_pic_buffering_minus1" ); 
		p_sps->sps_max_num_reorder_pics[ i ] = READ_UEV( p_bs, "sps_max_num_reorder_pics" );
		p_sps->sps_max_latency_increase_plus1[ i ] = READ_UEV( p_bs, "sps_max_latency_increase_plus1" );
    if(!code){
      for(i=0; i<sps_max_sub_layers_minus1; i++){
        p_sps->sps_max_dec_pic_buffering_minus1[ i ] = p_sps->sps_max_dec_pic_buffering_minus1[ sps_max_sub_layers_minus1 ];
        p_sps->sps_max_num_reorder_pics[ i ] = p_sps->sps_max_num_reorder_pics[ sps_max_sub_layers_minus1 ];
        p_sps->sps_max_latency_increase_plus1[ i ] = p_sps->sps_max_latency_increase_plus1[ sps_max_sub_layers_minus1 ];
      }
      break;
    }
	}

	p_sps->log2_min_luma_coding_block_size = 3 + READ_UEV( p_bs, "log2_min_coding_block_size_minus3" );
	p_sps->log2_luma_coding_block_size = p_sps->log2_min_luma_coding_block_size + READ_UEV(p_bs, "log2_diff_max_min_luma_coding_block_size" );


	///uiMaxCUDepthCorrect = p_sps->log2_diff_max_min_luma_coding_block_size;
	///p_sps->max_cu_width = ( 1 << ( log2MinCUSize + uiMaxCUDepthCorrect ) ); 
	///g_uiMaxCUWidth  = 1 << ( log2MinCUSize + uiMaxCUDepthCorrect );//YU_TBD global
	///p_sps->max_cu_height = ( 1 << (log2MinCUSize + uiMaxCUDepthCorrect) ); 
	///g_uiMaxCUHeight = 1 << ( log2MinCUSize + uiMaxCUDepthCorrect );//YU_TBD global

	p_sps->log2_min_transform_block_size = 2 + READ_UEV( p_bs, "log2_min_transform_block_size_minus2" );  
	p_sps->log2_max_transform_block_size = p_sps->log2_min_transform_block_size + READ_UEV( p_bs, "log2_diff_max_min_transform_block_size" );

	p_sps->max_transform_hierarchy_depth_inter = READ_UEV(p_bs, "max_transform_hierarchy_depth_inter" );// YU_TBD:  + 1 
	p_sps->max_transform_hierarchy_depth_intra = READ_UEV(p_bs, "max_transform_hierarchy_depth_intra" );// YU_TBD : + 1

	///g_uiAddCUDepth = 0;
	///while ((p_sps->max_cu_width >> uiMaxCUDepthCorrect ) > (VO_U32)( 1 << ( p_sps->log2_min_transform_block_size + g_uiAddCUDepth))) {
	///	g_uiAddCUDepth++;
	///}
 
	//YU_TBD
	///p_sps->max_cu_depth = uiMaxCUDepthCorrect + g_uiAddCUDepth;
	///g_uiMaxCUDepth  = uiMaxCUDepthCorrect + g_uiAddCUDepth;
	
	p_sps->scaling_list_enabled_flag = READ_FLAG( p_bs, "scaling_list_enabled_flag" );
	if ( p_sps->scaling_list_enabled_flag ) {
		p_sps->sps_scaling_list_data_present_flag = READ_FLAG( p_bs, "sps_scaling_list_data_present_flag" );           
		if ( p_sps->sps_scaling_list_data_present_flag ) {
            alloc_ScalingList_dec(p_dec, &p_sps->spsScalingListEntity );//YU_TBD
            scaling_list_data(&p_sps->spsScalingListEntity, p_bs);
		} else {

		}
	}

	p_sps->amp_enabled_flag = READ_FLAG( p_bs, "amp_enabled_flag" ); 
	p_sps->sample_adaptive_offset_enabled_flag = READ_FLAG(p_bs, "sample_adaptive_offset_enabled_flag" );   

	p_sps->pcm_enabled_flag = READ_FLAG( p_bs, "pcm_enabled_flag" );
	if ( p_sps->pcm_enabled_flag) {
		p_sps->pcm_sample_bit_depth_luma_minus1 = READ_UV( p_bs, 4,  "pcm_sample_bit_depth_luma_minus1" );
		p_sps->pcm_sample_bit_depth_chroma_minus1 = READ_UV( p_bs, 4,  "pcm_sample_bit_depth_chroma_minus1" );
		code = 3 + READ_UEV( p_bs, "log2_min_pcm_luma_coding_block_size_minus3" ); 
	  p_sps->log2_min_pcm_luma_coding_block_size = Clip3(p_sps->log2_min_luma_coding_block_size, MIN( p_sps->log2_luma_coding_block_size , 5 ), code );
		code = READ_UEV( p_bs, "log2_diff_max_min_pcm_luma_coding_block_size" );
		p_sps->log2_max_pcm_luma_coding_block_size = ( code + p_sps->log2_min_pcm_luma_coding_block_size ); //YU_TBD, Check spec clip value
		p_sps->pcm_loop_filter_disable_flag = READ_FLAG( p_bs, "pcm_loop_filter_disable_flag" );
	}

	p_sps->num_short_term_ref_pic_sets = READ_UEV( p_bs, "num_short_term_ref_pic_sets" );
	for ( i = 0; i < p_sps->num_short_term_ref_pic_sets; i++ ) {
		ParseShortTermRefPicSet( p_sps, rps_array, rps_array + i, p_bs, i );
	}
	p_sps->long_term_ref_pics_present_flag = READ_FLAG( p_bs, "long_term_ref_pics_present_flag" );       
	if ( p_sps->long_term_ref_pics_present_flag ) {
		p_sps->num_long_term_ref_pic_sps = READ_UEV(p_bs, "num_long_term_ref_pic_sps" );
		for ( i = 0; i < p_sps->num_long_term_ref_pic_sps; i++ ) {
			p_sps->lt_ref_pic_poc_lsb_sps[ i ] = READ_UV( p_bs, p_sps->log2_max_pic_order_cnt_lsb, "lt_ref_pic_poc_lsb_sps" );
			p_sps->used_by_curr_pic_lt_sps_flag[ i ] = READ_FLAG( p_bs,  "used_by_curr_pic_lt_sps_flag[i]");
		}
	}
	p_sps->sps_temporal_mvp_enable_flag = READ_FLAG(p_bs, "sps_temporal_mvp_enable_flag" );        
    p_sps->sps_strong_intra_smoothing_enable_flag = READ_FLAG( p_bs, "sps_strong_intra_smoothing_enable_flag" );

    if ( READ_FLAG1( p_bs ) ) { //"vui_parameters_present_flag"
        ParseVUI( p_sps, p_bs);//JO@TBD
    }

	if ( READ_FLAG1( p_bs ) ) { // "sps_extension_flag"
		///while (xMoreRbspData()) {//YU_TBD
		///	p_sps->sps_extension_data_flag = READ_FLAG(p_bs, "sps_extension_data_flag");
		///}
	}

	///rbsp_trailing_bits( ); YU_TBD
    ///for (i = 0; i < p_sps->max_cu_depth - g_uiAddCUDepth; i++)
	///{
	///	p_sps->m_iAMPAcc[i] = p_sps->amp_enabled_flag;
	///}

	///for (i = p_sps->max_cu_depth - g_uiAddCUDepth; i < p_sps->max_cu_depth; i++)
	///{
	///	p_sps->m_iAMPAcc[i] = 0;
	///}   
	//YU_TBD; error check
	//IniMinTbAddrZs( p_sps );
  p_sps->Valid =  VO_TRUE;
	return VO_ERR_NONE;
}

VO_S32 ParseVPS( H265_DEC_SLICE * p_slice, H265_DEC_VPS *p_vps,
	BIT_STREAM *p_bs )
{
	VO_U32 i, j;
	TComPTL  vps_ptl;
	VO_U32 vps_sub_layer_ordering_info_present_flag;
	VO_U32 vps_max_sub_layers_minus1;
	
#if USE_FRAME_THREAD
  //Harry: temp resolution for more than one PPS
  if ( p_slice->nThdNum> 1 ) {
    if ( p_vps->Valid ) {
      //wait for other thread finish decoding
      VO_U32 i;
      for ( i = 0; i < p_slice->nThdNum; ++i ) {
        h265FrameThdInfo *pThdInfo_t = p_slice->pThdInfo->pDecGlobal->pFrameThdPool->thdInfo + i;
        THD_COND_WAIT(pThdInfo_t->readyCond, pThdInfo_t->stateMutex, pThdInfo_t->state != FRAME_STATE_READY);
      }		
    }
  }
#endif

	VO_LOG(LOG_FILE,  LOG_MODULE_ALL | LOG_INFO_ALL, "\n VPS!");

	p_vps->sps_video_parameter_set_id = READ_UV( p_bs, 4, "sps_video_parameter_set_id" );
	READ_UV( p_bs, 2,  "vps_reserved_three_2bits" );
	READ_UV( p_bs, 6,  "vps_reserved_zero_6bits" );
	vps_max_sub_layers_minus1 = READ_UV( p_bs, 3, "vps_max_sub_layers_minus1");
	p_vps->vps_temporal_id_nesting_flag = READ_FLAG( p_bs, "vps_temporal_id_nesting_flag");
	READ_UV( p_bs, 16, "vps_reserved_ffff_16bits" );
	ParsePTL( p_bs, &vps_ptl, vps_max_sub_layers_minus1);

	vps_sub_layer_ordering_info_present_flag = READ_FLAG( p_bs, "vps_sub_layer_ordering_info_present_flag");
	
	for ( i = vps_sub_layer_ordering_info_present_flag ? 0 : vps_max_sub_layers_minus1; \
		i <= vps_max_sub_layers_minus1; i++ ) {
		p_vps->vps_max_dec_pic_buffering_minus1[ i ] = READ_UEV( p_bs, "vps_max_dec_pic_buffering_minus1");
		p_vps->vps_num_reorder_pics[ i ] = READ_UEV( p_bs, "vps_num_reorder_pics");
		p_vps->vps_max_latency_increase_plus1[ i ] = READ_UEV( p_bs, "vps_max_latency_increase_plus1");
	}
	p_vps->vps_max_layer_id = READ_UV( p_bs, 6, "vps_max_layer_id" );
	p_vps->vps_num_layer_sets_minus1 = READ_UEV( p_bs, "vps_num_layer_sets_minus1" );

	for ( j = 1; j < p_vps->vps_num_layer_sets_minus1; j++ ) {
		for ( i = 0; i <= p_vps->vps_max_layer_id; i++ ) {
			p_vps->layer_id_included_flag[ j ][ i ] = READ_FLAG( p_bs, "op_layer_id_included_flag[j][i]" );
		}
	}

	if ( READ_FLAG1( p_bs ) ) {// "vps_timing_info_present_flag"
		VO_U32 vps_num_hrd_parameters;

		READ_UV( p_bs, 32, "vps_num_units_in_tick");
		READ_UV( p_bs, 32, "vps_time_scale");
		if ( READ_FLAG1( p_bs) ) {// "vps_poc_proportional_to_timing_flag"
			READ_UEV( p_bs, "vps_num_ticks_poc_diff_one_minus1");
		}
		vps_num_hrd_parameters =  READ_UEV( p_bs, "vps_num_hrd_parameters" );
		///if ( p_vps->vps_num_hrd_parameters > 0 ) {
		///	//TODO:createHrdParamBuffer();
		///}
		for ( i = 0; i < vps_num_hrd_parameters; i ++ ) {
			READ_UEV( p_bs, "hrd_layer_set_idx" );
			if ( i > 0 ) {
				READ_FLAG( p_bs, "cprms_present_flag[i]" );
			}
		}
			//YU_TBD
			//TODO:parseHrdParameters(pcVPS->getHrdParameters(i), pcVPS->getCprmsPresentFlag( i ), pcVPS->getMaxTLayers() - 1);
    }

	//TODO:YU_TBD
	if ( READ_FLAG1( p_bs ) ) {//  "vps_extension_flag" 
		//while ( xMoreRbspData() )
		//{
		//	READ_FLAG( p_bs, "vps_extension_data_flag");
		//}
	}
  p_vps->Valid = VO_TRUE;
  return VO_ERR_NONE;
}

