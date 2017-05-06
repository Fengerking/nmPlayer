
/*!
 *************************************************************************************
 * \file annexb.c
 *
 * \brief
 *    Annex B Byte Stream format
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *      - Stephan Wenger                  <stewe@cs.tu-berlin.de>
 *************************************************************************************
 */
#include "defines.h"

#if 0//!BUILD_WITHOUT_C_LIB
#include <stdlib.h>
#include <assert.h>
#include <memory.h>
#endif
#include "memalloc.h"
//this is not used any longer

/** 
IL TBD:
void Get_Neighbor_ABCnRefIdx(ImageParameters *img,avdNativeInt x4x4Offset, avdNativeInt y4x4Offset, 
void SetMotionVectorPredictor (ImageParameters *img,
avdNativeInt GetPSliceSkippedMV(ImageParameters *img)
botMb = (img->current_mb_nr&1); //shoule be correct to mbsProcessor
imgblock_y = !curr_mb_field ? img->block_y : botMb ? ((img->block_y-4)>>1) :
(img->block_y>>1);

isTopMB = !(img->current_mb_nr&1);

if (currSlice->mb_aff_frame_flag)
p_Vid->current_mb_nr = currSlice->start_mb_nr << 1;
chroma_vector_adjustment

insert_picture_in_dpb
image.c:
init_mbaff_lists
set_ref_pic_num?? what is the AVD_MOTION_LIST_SIZE ?
MbAffPostProc(img);


spec:
(field_pic_flag is equal to 1), num_ref_idx_l0_active_minus1 shall be in the range of 0 to 31, inclusive.
(MbaffFrameFlag is equal to 1 and mb_field_decoding_flag is equal to 1), the value of
ref_idx_l0[ mbPartIdx ] shall be in the range of 0 to 2 * num_ref_idx_l0_active_minus1 + 1, inclusive
A non-paired reference field is not used for inter prediction for decoding a frame, regardless of the value of
MbaffFrameFlag.
Intra Prediction:
any case:
If MbaffFrameFlag is equal to 1 and the macroblock mbAddrN is a field macroblock,
p[ x, y ] = cSL[ xM + xW, yM + 2 * yW ] (8-45)
8x8:
If MbaffFrameFlag is equal to 1, the current macroblock is a frame coded macroblock, the
macroblock mbAddrN is a field coded macroblock, and luma8x8BlkIdx is equal to 2, n is set
equal to 3.


IPCM:
If MbaffFrameFlag is equal to 1 and the current macroblock is a field macroblock, dy is set equal to 2.

GetBlock:
Otherwise (MbaffFrameFlag is equal to 1 and mb_field_decoding_flag is equal to 1), refPicHeightEffectiveL is set
equal to PicHeightInSamplesL / 2.

WP:
If MbaffFrameFlag is equal to 1 and the current macroblock is a field macroblock
refIdxL0WP = refIdxL0 >> 1 (8-288)
refIdxL1WP = refIdxL1 >> 1 (8-289)

Picture reconstruct before deblocking:
If MbaffFrameFlag is equal to 1 and the current macroblock is a field macroblock
S'L[ xP + xO + j, yP + 2 * ( yO + i ) ] = uij with i, j = 0..nE - 1 (8


ref code:
get_mb_block_pos_mbaff
get_mb_block_pos_mbaff
mb_x and mb_y
CheckAvailabilityOfNeighbors
*/