

#include "global.h"
//#include "annexb.h"
#include "nalu.h"
#include "memalloc.h"
#if !(defined(RVDS) || defined(_WIN32_WCE))
//#include "rtp.h"
#endif
#if (MVC_EXTENSION_ENABLE)
#include "vlc.h"
#endif



void CheckZeroByteNonVCL(H264DEC_G *pDecGlobal, NALU_t *nalu)
{
#if 0
  int CheckZeroByte=0;

  //This function deals only with non-VCL NAL units
  if(nalu->nal_unit_type>=1&&nalu->nal_unit_type<=5)
    return;

  //for SPS and PPS, zero_byte shall exist
  if(nalu->nal_unit_type==NALU_TYPE_SPS || nalu->nal_unit_type==NALU_TYPE_PPS)
    CheckZeroByte=1;
  //check the possibility of the current NALU to be the start of a new access unit, according to 7.4.1.2.3
  if(nalu->nal_unit_type==NALU_TYPE_AUD  || nalu->nal_unit_type==NALU_TYPE_SPS ||
    nalu->nal_unit_type==NALU_TYPE_PPS || nalu->nal_unit_type==NALU_TYPE_SEI ||
    (nalu->nal_unit_type>=13 && nalu->nal_unit_type<=18))
  {
    if(pDecGlobal->LastAccessUnitExists)
    {
      pDecGlobal->LastAccessUnitExists=0;    //deliver the last access unit to decoder
      pDecGlobal->NALUCount=0;
    }
  }
  pDecGlobal->NALUCount++;
  //for the first NAL unit in an access unit, zero_byte shall exists
  if(pDecGlobal->NALUCount==1)
    CheckZeroByte=1;
  if(CheckZeroByte && nalu->startcodeprefix_len==3)   
  {
    printf("Warning: zero_byte shall exist\n");
    //because it is not a very serious problem, we do not exit here
  }
#endif
}

void CheckZeroByteVCL(H264DEC_G *pDecGlobal, NALU_t *nalu)
{
#if 0
  int CheckZeroByte=0;

  //This function deals only with VCL NAL units
  if(!(nalu->nal_unit_type>=NALU_TYPE_SLICE && nalu->nal_unit_type <= NALU_TYPE_IDR))
    return;

  if(pDecGlobal->LastAccessUnitExists)
  {
    pDecGlobal->NALUCount=0;
  }
  pDecGlobal->NALUCount++;
  //the first VCL NAL unit that is the first NAL unit after last VCL NAL unit indicates
  //the start of a new access unit and hence the first NAL unit of the new access unit.           (sounds like a tongue twister :-)
  if(pDecGlobal->NALUCount == 1)
    CheckZeroByte = 1;
  pDecGlobal->LastAccessUnitExists = 1;
  if(CheckZeroByte && nalu->startcodeprefix_len==3)
  {
    printf("warning: zero_byte shall exist\n");
    //because it is not a very serious problem, we do not exit here
  }
#endif
}

