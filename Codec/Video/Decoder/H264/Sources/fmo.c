
#if !BUILD_WITHOUT_C_LIB
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#endif

#include "global.h"
#include "elements.h"
#include "defines.h"
#include "header.h"
#include "fmo.h"
//#include "H264_C_Type.h"
#ifndef VOI_H264D_BLOCK_FMO
//#define PRINT_FMO_MAPS
static void FmoGenerateType0MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );
static void FmoGenerateType1MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );
static void FmoGenerateType2MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );
static void FmoGenerateType3MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );
static void FmoGenerateType4MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );
static void FmoGenerateType5MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );
static void FmoGenerateType6MapUnitMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps, avdNativeUInt PicSizeInMapUnits );


/*!
 ************************************************************************
 * \brief
 *    Generates MapUnitToSliceGroupMap
 *    Has to be called every time a new Picture Parameter Set is used
 *
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 *
 ************************************************************************
 */
#define ONE_SLICE_GROUP_MAP_TYPE 7 // away from standard 0-6;
avdNativeInt FmoGenerateMapUnitToSliceGroupMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, 
									   seq_parameter_set_rbsp_t* sps)
{
  avdNativeUInt NumSliceGroupMapUnits;
  //Even if img->prevSliceGroupMapType == pps->slice_group_map_type,the map still changes possibly
  /* 
  if (img->MapUnitToSliceGroupMap &&
	  clipInfo->clipArrHeight == sizeInfo->height && // make sure height(field/frame) is not changed;
	  (img->prevSliceGroupMapType == pps->slice_group_map_type 
	  || (!pps->num_slice_groups_minus1 &&
	  img->prevSliceGroupMapType == ONE_SLICE_GROUP_MAP_TYPE))){
	  img->sliceGroupMapUnchanged = AVC_TRUE;
	  return 0; // same active PPS;
  }
*/
  img->sliceGroupMapUnchanged = AVC_FALSE;
  img->prevSliceGroupMapType = pps->slice_group_map_type;
  NumSliceGroupMapUnits = img->active_sps->mb_adaptive_frame_field_flag ? 
	  (sizeInfo->PicSizeInMbs<<1) : sizeInfo->PicSizeInMbs;
  if (pps->slice_group_map_type == 6)
  { 
    if ((pps->num_slice_group_map_units_minus1+1) != NumSliceGroupMapUnits)
    {
      AVD_ERROR_CHECK(img,"wrong pps->num_slice_group_map_units_minus1 for used SPS and FMO type 6", ERROR_InvalidNumSliceGroupMapUnit);
    }
  }

  // allocate memory for MapUnitToSliceGroupMap
  if (img->MapUnitToSliceGroupMap){
	SafevoH264AlignedFree(img,img->MapUnitToSliceGroupMap);
	img->MapUnitToSliceGroupMap = NULL;
  }

  if (!img->MapUnitToSliceGroupMap &&
	  (img->MapUnitToSliceGroupMap = voH264AlignedMalloc (img,75, (NumSliceGroupMapUnits) * sizeof (avdUInt8))) == NULL)
  {
	CHECK_ERR_RET_NULL
    printf ("cannot allocated %d bytes for MapUnitToSliceGroupMap, exit\n", (pps->num_slice_group_map_units_minus1+1) * sizeof (int));
    exit (-1);
  }

  if (pps->num_slice_groups_minus1 == 0)    // only one slice group
  {
	img->prevSliceGroupMapType = ONE_SLICE_GROUP_MAP_TYPE; 
    memset (img->MapUnitToSliceGroupMap, 0, NumSliceGroupMapUnits * sizeof (avdUInt8));
    return 0;
  }

  switch (pps->slice_group_map_type)
  {
  case 0:
    FmoGenerateType0MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  case 1:
    FmoGenerateType1MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  case 2:
    FmoGenerateType2MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  case 3:
    FmoGenerateType3MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  case 4:
    FmoGenerateType4MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  case 5:
    FmoGenerateType5MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  case 6:
    FmoGenerateType6MapUnitMap (img,pps, sps, NumSliceGroupMapUnits);
    break;
  default:
    printf ("Illegal slice_group_map_type %d , exit \n", pps->slice_group_map_type);
    exit (-1);
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Generates MbToSliceGroupMap from MapUnitToSliceGroupMap
 *
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 *
 ************************************************************************
 */
static avdNativeInt FmoGenerateMbToSliceGroupMap (ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps)
{
  avdNativeUInt i;
  avdNativeUInt NumSliceGroupMapUnits = img->active_sps->mb_adaptive_frame_field_flag ? 
	  (sizeInfo->PicSizeInMbs<<1) : sizeInfo->PicSizeInMbs;
  // allocate memory for MbToSliceGroupMap
  if (img->MbToSliceGroupMap && !img->sliceGroupMapUnchanged){ // make sure height(field/frame) is not changed;
	  SafevoH264AlignedFree(img,img->MbToSliceGroupMap);
	  img->MbToSliceGroupMap = NULL;
  }

  if (!img->MbToSliceGroupMap && 
	  (img->MbToSliceGroupMap = voH264AlignedMalloc (img,76, NumSliceGroupMapUnits * sizeof (avdUInt8))) == NULL)
  {
	  CHECK_ERR_RET_NULL
    printf ("cannot allocated %d bytes for MbToSliceGroupMap, exit\n", (sizeInfo->PicSizeInMbs) * sizeof (avdUInt8));
    exit (-1);
  }


  if ((sps->frame_mbs_only_flag)|| img->field_pic_flag)
  {
	  if (img->prevMbToSliceGroupMapIdx == 1 &&
		  img->sliceGroupMapUnchanged)
		  return 0;

	  img->prevMbToSliceGroupMapIdx = 1;
	  memcpy (img->MbToSliceGroupMap, img->MapUnitToSliceGroupMap, 
		  sizeInfo->PicSizeInMbs * sizeof(avdUInt8));
  }
#if FEATURE_INTERLACE
  else
    if (sps->mb_adaptive_frame_field_flag  &&  (!img->field_pic_flag))
    {
	  if (img->prevMbToSliceGroupMapIdx == 2 &&
		  img->sliceGroupMapUnchanged)
		  return 0;

	  img->prevMbToSliceGroupMapIdx = 2;
      for (i=0; i<sizeInfo->PicSizeInMbs; i++)
      {
        img->MbToSliceGroupMap[i] = img->MapUnitToSliceGroupMap[i>>1];
      }
    }
    else
    {
	  if (img->prevMbToSliceGroupMapIdx == 3 &&
		  img->sliceGroupMapUnchanged)
		  return 0;

	  img->prevMbToSliceGroupMapIdx = 3;
      for (i=0; i<sizeInfo->PicSizeInMbs; i++)
      {
        img->MbToSliceGroupMap[i] = img->MapUnitToSliceGroupMap
			[(i/(sizeInfo->PicWidthInMbs<<1))*sizeInfo->PicWidthInMbs+(i%sizeInfo->PicWidthInMbs)];
      }
    }
#endif //FEATURE_INTERLACE
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    FMO initialization: Generates MapUnitToSliceGroupMap and MbToSliceGroupMap.
 *
 * \param pps
 *    Picture Parameter set to be used for map generation
 * \param sps
 *    Sequence Parameter set to be used for map generation
 ************************************************************************
 */
avdNativeInt FmoInit(ImageParameters *img,pic_parameter_set_rbsp_t* pps, seq_parameter_set_rbsp_t* sps)
{
#ifdef PRINT_FMO_MAPS
  avdNativeUInt i,j;
#endif

	FmoGenerateMapUnitToSliceGroupMap(img,pps, sps);
  FmoGenerateMbToSliceGroupMap(img,pps, sps);

  img->NumberOfSliceGroups = pps->num_slice_groups_minus1+1;

#ifdef PRINT_FMO_MAPS
  printf("\n");
  printf("FMO Map (Units):\n");

  for (j=0; j<sizeInfo->PicHeightInMapUnits; j++)
  {
    for (i=0; i<sizeInfo->PicWidthInMbs; i++)
    {
      printf("%c",48+img->MapUnitToSliceGroupMap[i+j*sizeInfo->PicWidthInMbs]);
    }
    printf("\n");
  }
  printf("\n");
  printf("FMO Map (Mb):\n");

  for (j=0; j<sizeInfo->PicHeightInMbs; j++)
  {
    for (i=0; i<sizeInfo->PicWidthInMbs; i++)
    {
      printf("%c",48+img->MbToSliceGroupMap[i+j*sizeInfo->PicWidthInMbs]);
    }
    printf("\n");
  }
  printf("\n");

#endif

  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    Free memory allocated by FMO functions
 ************************************************************************
 */
avdNativeInt FmoFinit(ImageParameters *img)
{
  if (img->MbToSliceGroupMap)
  {
    SafevoH264AlignedFree(img,img->MbToSliceGroupMap);
    img->MbToSliceGroupMap = NULL;
  }
  if (img->MapUnitToSliceGroupMap)
  {
    SafevoH264AlignedFree(img,img->MapUnitToSliceGroupMap);
    img->MapUnitToSliceGroupMap = NULL; 
  }
  return 0;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetNumberOfSliceGroup() 
 *
 * \par Input:
 *    None
 ************************************************************************
 */
avdNativeInt FmoGetNumberOfSliceGroup(ImageParameters *img)
{
  return img->NumberOfSliceGroups;
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetLastMBOfPicture() 
 *    returns the macroblock number of the last MB in a picture.  This
 *    mb happens to be the last macroblock of the picture if there is only
 *    one slice group
 *
 * \par Input:
 *    None
 ************************************************************************
 */
avdNativeInt FmoGetLastMBOfPicture(ImageParameters *img)
{
  return FmoGetLastMBInSliceGroup (img,FmoGetNumberOfSliceGroup(img)-1);
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetLastMBInSliceGroup: Returns MB number of last MB in SG
 *
 * \par Input:
 *    SliceGroupID (0 to 7)
 ************************************************************************
 */

avdNativeInt FmoGetLastMBInSliceGroup (ImageParameters *img,avdNativeInt SliceGroup)
{
  avdNativeUInt i;
  
  for (i=sizeInfo->PicSizeInMbs-1; ; i--){
    if (FmoGetSliceGroupId (img,i) == SliceGroup)
      return i;
	if (i == 0)
		return -1;
  }
  return -1;

}


/*!
 ************************************************************************
 * \brief
 *    Returns SliceGroupID for a given MB
 *
 * \param mb
 *    Macroblock number (in scan order)
 ************************************************************************
 */
avdNativeInt FmoGetSliceGroupId (ImageParameters *img,avdNativeInt mb)
{
  assert (mb < (avdNativeInt)sizeInfo->PicSizeInMbs);
  assert (img->MbToSliceGroupMap != NULL);
  return img->MbToSliceGroupMap[mb];
}


/*!
 ************************************************************************
 * \brief
 *    FmoGetNextMBBr: Returns the MB-Nr (in scan order) of the next
 *    MB in the (scattered) Slice, -1 if the slice is finished
 *
 * \param CurrentMbNr
 *    number of the current macroblock
 ************************************************************************
 */
avdNativeInt FmoGetNextMBNr (ImageParameters *img,avdNativeInt CurrentMbNr)
{
  avdNativeInt SliceGroup = FmoGetSliceGroupId (img,CurrentMbNr);
  while (++CurrentMbNr<(avdNativeInt)sizeInfo->PicSizeInMbs && 
	  img->MbToSliceGroupMap [CurrentMbNr] != SliceGroup);

  if (CurrentMbNr >= (avdNativeInt)sizeInfo->PicSizeInMbs)
    return -1;    // No further MB in this slice (could be end of picture)
  else
    return CurrentMbNr;
}


/*!
 ************************************************************************
 * \brief
 *    Generate interleaved slice group map type MapUnit map (type 0)
 *
 ************************************************************************
 */
static void FmoGenerateType0MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  avdNativeUInt iGroup;
  avdNativeUInt i, j;
  i = 0;
  do
  {
    for( iGroup = 0; 
         (iGroup <= pps->num_slice_groups_minus1) && (i < PicSizeInMapUnits); 
         i += pps->run_length_minus1[iGroup++] + 1 )
    {
      for( j = 0; j <= pps->run_length_minus1[ iGroup ] && i + j < PicSizeInMapUnits; j++ )
        img->MapUnitToSliceGroupMap[i+j] = iGroup;
    }
  }
  while( i < PicSizeInMapUnits );
}


/*!
 ************************************************************************
 * \brief
 *    Generate dispersed slice group map type MapUnit map (type 1)
 *
 ************************************************************************
 */
static void FmoGenerateType1MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  avdNativeUInt i;
  for( i = 0; i < PicSizeInMapUnits; i++ )
  {
    img->MapUnitToSliceGroupMap[i] = ((i%sizeInfo->PicWidthInMbs)+(((i/sizeInfo->PicWidthInMbs)*(pps->num_slice_groups_minus1+1))/2))
		                            %(pps->num_slice_groups_minus1+1);
  }
}

/*!
 ************************************************************************
 * \brief
 *    Generate foreground with left-over slice group map type MapUnit map (type 2)
 *
 ************************************************************************
 */
static void FmoGenerateType2MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  avdNativeInt iGroup;
  avdNativeUInt i, x, y;
  avdNativeUInt yTopLeft, xTopLeft, yBottomRight, xBottomRight;

  for( i = 0; i < PicSizeInMapUnits; i++ )
    img->MapUnitToSliceGroupMap[ i ] = (avdUInt8)pps->num_slice_groups_minus1;

  for( iGroup = pps->num_slice_groups_minus1 - 1 ; iGroup >= 0; iGroup-- ) 
  {
    yTopLeft = pps->top_left[ iGroup ] / sizeInfo->PicWidthInMbs;
    xTopLeft = pps->top_left[ iGroup ] % sizeInfo->PicWidthInMbs;
    yBottomRight = pps->bottom_right[ iGroup ] / sizeInfo->PicWidthInMbs;
    xBottomRight = pps->bottom_right[ iGroup ] % sizeInfo->PicWidthInMbs;
    for( y = yTopLeft; y <= yBottomRight; y++ )
      for( x = xTopLeft; x <= xBottomRight; x++ )
        img->MapUnitToSliceGroupMap[ y * sizeInfo->PicWidthInMbs + x ] = (avdUInt8)iGroup;
 }
}


/*!
 ************************************************************************
 * \brief
 *    Generate box-out slice group map type MapUnit map (type 3)
 *
 ************************************************************************
 */
static void FmoGenerateType3MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  avdNativeUInt i, k;
  avdNativeInt leftBound, topBound, rightBound, bottomBound;
  avdNativeInt x, y, xDir, yDir;
  avdNativeInt mapUnitVacant;

  avdNativeUInt mapUnitsInSliceGroup0 = min((pps->slice_group_change_rate_minus1 + 1) * img->slice_group_change_cycle, PicSizeInMapUnits);

  for( i = 0; i < PicSizeInMapUnits; i++ )
    img->MapUnitToSliceGroupMap[ i ] = 2;

  x = ( sizeInfo->PicWidthInMbs - pps->slice_group_change_direction_flag ) / 2;
  y = ( sizeInfo->PicHeightInMapUnits - pps->slice_group_change_direction_flag ) / 2;

  leftBound   = x;
  topBound    = y;
  rightBound  = x;
  bottomBound = y;

  xDir =  pps->slice_group_change_direction_flag - 1;
  yDir =  pps->slice_group_change_direction_flag;

  for( k = 0; k < PicSizeInMapUnits; k += mapUnitVacant ) 
  {
    mapUnitVacant = ( img->MapUnitToSliceGroupMap[ y * sizeInfo->PicWidthInMbs + x ]  ==  2 );
    if( mapUnitVacant )
       img->MapUnitToSliceGroupMap[ y * sizeInfo->PicWidthInMbs + x ] = ( k >= mapUnitsInSliceGroup0 );

    if( xDir  ==  -1  &&  x  ==  leftBound ) 
    {
      leftBound = max( leftBound - 1, 0 );
      x = leftBound;
      xDir = 0;
      yDir = 2 * pps->slice_group_change_direction_flag - 1;
    } 
    else 
      if( xDir  ==  1  &&  x  ==  rightBound ) 
      {
        rightBound = min( rightBound + 1, (int)sizeInfo->PicWidthInMbs - 1 );
        x = rightBound;
        xDir = 0;
        yDir = 1 - 2 * pps->slice_group_change_direction_flag;
      } 
      else 
        if( yDir  ==  -1  &&  y  ==  topBound ) 
        {
          topBound = max( topBound - 1, 0 );
          y = topBound;
          xDir = 1 - 2 * pps->slice_group_change_direction_flag;
          yDir = 0;
         } 
        else 
          if( yDir  ==  1  &&  y  ==  bottomBound ) 
          {
            bottomBound = min( bottomBound + 1, (int)sizeInfo->PicHeightInMapUnits - 1 );
            y = bottomBound;
            xDir = 2 * pps->slice_group_change_direction_flag - 1;
            yDir = 0;
          } 
          else
          {
            x = x + xDir;
            y = y + yDir;
          }
  }

}

/*!
 ************************************************************************
 * \brief
 *    Generate raster scan slice group map type MapUnit map (type 4)
 *
 ************************************************************************
 */
static void FmoGenerateType4MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  avdNativeUInt mapUnitsInSliceGroup0 = min((pps->slice_group_change_rate_minus1 + 1) * img->slice_group_change_cycle, PicSizeInMapUnits);
  avdNativeUInt sizeOfUpperLeftGroup = pps->slice_group_change_direction_flag ? ( PicSizeInMapUnits - mapUnitsInSliceGroup0 ) : mapUnitsInSliceGroup0;
  avdNativeUInt i;
  for( i = 0; i < PicSizeInMapUnits; i++ )
    if( i < sizeOfUpperLeftGroup )
        img->MapUnitToSliceGroupMap[ i ] = pps->slice_group_change_direction_flag;
    else
        img->MapUnitToSliceGroupMap[ i ] = 1 - pps->slice_group_change_direction_flag;
}

/*!
 ************************************************************************
 * \brief
 *    Generate wipe slice group map type MapUnit map (type 5)
 *
 ************************************************************************
 */
static void FmoGenerateType5MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  
  avdNativeUInt mapUnitsInSliceGroup0 = min((pps->slice_group_change_rate_minus1 + 1) * img->slice_group_change_cycle, PicSizeInMapUnits);
  avdNativeUInt sizeOfUpperLeftGroup = pps->slice_group_change_direction_flag ? ( PicSizeInMapUnits - mapUnitsInSliceGroup0 ) : mapUnitsInSliceGroup0;

  avdNativeUInt i,j, k = 0;

  for( j = 0; j < sizeInfo->PicWidthInMbs; j++ )
    for( i = 0; i < sizeInfo->PicHeightInMapUnits; i++ )
        if( k++ < sizeOfUpperLeftGroup )
            img->MapUnitToSliceGroupMap[ i * sizeInfo->PicWidthInMbs + j ] = 1 - pps->slice_group_change_direction_flag;
        else
            img->MapUnitToSliceGroupMap[ i * sizeInfo->PicWidthInMbs + j ] = pps->slice_group_change_direction_flag;

}

/*!
 ************************************************************************
 * \brief
 *    Generate explicit slice group map type MapUnit map (type 6)
 *
 ************************************************************************
 */
static void FmoGenerateType6MapUnitMap (ImageParameters *img,
										pic_parameter_set_rbsp_t* pps, 
										seq_parameter_set_rbsp_t* sps, 
										avdNativeUInt PicSizeInMapUnits )
{
  avdNativeUInt i;
  for (i=0; i<PicSizeInMapUnits; i++)
  {
    img->MapUnitToSliceGroupMap[i] = (avdUInt8)pps->slice_group_id[i];
  }
}
#endif
