/*
   channel mapping functionality
 */
#include "channel_map.h"
#include "bitenc.h"
#include "psy_const.h"
#include "qc_data.h"

static const Word16 maxChannelBits = 6144;

static Word16 initElement(ELEMENT_INFO* elInfo, ELEMENT_TYPE elType)
{
  Word16 error=0;                                    

  elInfo->elType=elType;                             

  switch(elInfo->elType) {

    case ID_SCE:
      elInfo->nChannelsInEl=1;                       

      elInfo->ChannelIndex[0]=0;                     

      elInfo->instanceTag=0;                         
      break;

    case ID_CPE:

      elInfo->nChannelsInEl=2;                        

      elInfo->ChannelIndex[0]=0;                      
      elInfo->ChannelIndex[1]=1;                      

      elInfo->instanceTag=0;                    
      break;

    default:
      error=1;                                  
  }

  return error;
}


Word16 InitElementInfo (Word16 nChannels, ELEMENT_INFO* elInfo)
{
  Word16 error;
  error = 0;                                        

  switch(nChannels) {

    case 1: 
      initElement(elInfo, ID_SCE);
      break;

    case 2:
      initElement(elInfo, ID_CPE);
      break;

    default:
      error=4;                                         
  }

  return error;
}


Word16 InitElementBits(ELEMENT_BITS *elementBits,
                       ELEMENT_INFO elInfo,
                       Word32 bitrateTot,
                       Word16 averageBitsTot,
                       Word16 staticBitsTot)
{
  Word16 error;
  error = 0;                                    

   switch(elInfo.nChannelsInEl) {
    case 1:
      elementBits->chBitrate = bitrateTot;                     
      elementBits->averageBits = averageBitsTot - staticBitsTot;
      elementBits->maxBits = maxChannelBits;                   

      elementBits->maxBitResBits = maxChannelBits - averageBitsTot;
      elementBits->maxBitResBits = elementBits->maxBitResBits - (elementBits->maxBitResBits & 7); 
      elementBits->bitResLevel = elementBits->maxBitResBits;   
      elementBits->relativeBits  = 0x4000; /* 1.0f/2 */        
      break;

    case 2:
      elementBits->chBitrate   = bitrateTot >> 1;
      elementBits->averageBits = averageBitsTot - staticBitsTot;
      elementBits->maxBits     = maxChannelBits << 1;

      elementBits->maxBitResBits = (maxChannelBits << 1) - averageBitsTot;
      elementBits->maxBitResBits = elementBits->maxBitResBits - (elementBits->maxBitResBits & 7);   
      elementBits->bitResLevel = elementBits->maxBitResBits;     
      elementBits->relativeBits = 0x4000; /* 1.0f/2 */           
      break;

    default:
      error = 1;                                                 
  }
  return error;
}
