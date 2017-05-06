

#include "global.h"


int RBSPtoSODB(byte *streamBuffer, int last_byte_pos)
{
  int ctr_bit, bitoffset;

  bitoffset = 0;
  //find trailing 1
  ctr_bit = (streamBuffer[last_byte_pos-1] & (0x01<<bitoffset));   // set up control bit

  while (ctr_bit==0)
  {                 // find trailing 1 bit
    ++bitoffset;
    if(bitoffset == 8)
    {
      //if(last_byte_pos == 0)
      //  printf(" Panic: All zero data sequence in RBSP \n");
      assert(last_byte_pos != 0);
      --last_byte_pos;
      bitoffset = 0;
    }
    ctr_bit= streamBuffer[last_byte_pos - 1] & (0x01<<(bitoffset));
  }

  return(last_byte_pos);

}

#if 1


int EBSPtoRBSP(byte *streamBuffer, int end_bytepos, int begin_bytepos)
{
  int i, j, count;
  count = 0;

  if(end_bytepos < begin_bytepos)
    return end_bytepos;

  j = begin_bytepos;

  for(i = begin_bytepos; i < end_bytepos; ++i)
  { //starting from begin_bytepos to avoid header information
    //in NAL unit, 0x000000, 0x000001 or 0x000002 shall not occur at any byte-aligned position
    if(count == ZEROBYTES_SHORTSTARTCODE && streamBuffer[i] < 0x03) 
      return j-2;
    if(count == ZEROBYTES_SHORTSTARTCODE && streamBuffer[i] == 0x03)
    {
      //check the 4th byte after 0x000003, except when cabac_zero_word is used, in which case the last three bytes of this NAL unit must be 0x000003
      //if((i < end_bytepos-1) && (streamBuffer[i+1] > 0x03))
      //  return j;
      //if cabac_zero_word is used, the final byte of this NAL unit(0x03) is discarded, and the last two bytes of RBSP must be 0x0000
      if(i == end_bytepos-1)
        return j;

      ++i;
      count = 0;
    }
    streamBuffer[j] = streamBuffer[i];
    if(streamBuffer[i] == 0x00)
      ++count;
    else
      count = 0;
    ++j;
  }

  return j;
}
#else
int EBSPtoRBSP(byte *streamBuffer, int end_bytepos, int begin_bytepos)
{
  int i, j, count;
  count = 0;

  if(end_bytepos < begin_bytepos)
    return end_bytepos;
  

  for (i = begin_bytepos; i + 2 < end_bytepos ;) 
  {
    if (streamBuffer[i + 2] > 0x3) 
    {//not find 0x 00 00 03
	   i += 2;
    } 
    else if (streamBuffer[i] == 0 && streamBuffer [i + 1] == 0) 
    {
	  if (streamBuffer[i + 2] == 0x3)
	  { //escape
	    break;
	  }
	  else if (streamBuffer[i + 2] < 0x3) 
	  {
	    return i;
	  }
    }
    i++;
  }
  if (i >= end_bytepos - 1) 
  	return end_bytepos;
  
  for(j = i; j+2 < end_bytepos;)
  { 
    if (streamBuffer[j + 2] > 0x3) 
	{// not find 0x 00 00 03
	 streamBuffer[i++] = streamBuffer[j++];
	 streamBuffer[i++] = streamBuffer[j++];
	} 
	else if (streamBuffer[j] == 0 && streamBuffer [j + 1] == 0) 
	{
	  if (streamBuffer[j + 2] == 0x3)
	  { //escape
	    streamBuffer[i++] = 0;
		streamBuffer[i++] = 0;
		j += 3;
		continue;
	  }
	  else if (streamBuffer[j + 2] < 0x3) 
	  {//next start code
		return j;
	  }
	}
	
	streamBuffer[i++] = streamBuffer[j++];
  }
  while(j<end_bytepos)
    streamBuffer[i++]= streamBuffer[j++];
  return j;
}

#endif

