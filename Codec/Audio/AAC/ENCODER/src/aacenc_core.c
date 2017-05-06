/*
   aac coder functions
 */
#include "typedef.h"
#include "aacenc_core.h"
#include "bitenc.h"

#include "psy_configuration.h"
#include "psy_main.h"
#include "qc_main.h"
#include "psy_main.h"
#include "channel_map.h"
#include "aac_rom.h"

//#define GetBitsAvail(hB) ((hB)->cntBits)

/*-----------------------------------------------------------------------------

functionname: AacInitDefaultConfig
description:  gives reasonable default configuration
returns:      ---

------------------------------------------------------------------------------*/
void AacInitDefaultConfig(AACENC_CONFIG *config)
{
  /* default configurations */
  config->adtsUsed        = 1;
  config->nChannelsIn     = 2;
  config->nChannelsOut    = 2;
  config->bitRate         = 128000;                      
  config->bandWidth       = 0;                           
}

/*---------------------------------------------------------------------------

functionname: AacEncOpen
description:  allocate and initialize a new encoder instance
returns:      0 if success

---------------------------------------------------------------------------*/

Word16  AacEncOpen(  AAC_ENCODER*      hAacEnc,        /* pointer to an encoder handle, initialized on return */
                     const  AACENC_CONFIG     config           /* pre-initialized config struct */
                     )
{
  //Word32 i;
  Word32 error = 0;
  Word16 profile = 1;

  ELEMENT_INFO *elInfo = NULL;
   
  if (hAacEnc==0) {
    error=1;                                  
  }

//  if (!error) {
//    /* sanity checks on config structure */
//    error = (&config == 0            || hAacEnc == 0             ||
//             config.nChannelsIn< 1  || config.nChannelsIn > MAX_CHANNELS  ||
//             config.nChannelsOut < 1 || config.nChannelsOut > MAX_CHANNELS ||
//             (config.bitRate!=0 && (config.bitRate/config.nChannelsOut < 8000)     ||
//                                    (config.bitRate/config.nChannelsOut > 160000)));
//  }
//  /* check sample rate */
//  if (!error) {
//    error = 2;
//	for(i = 0; i < NUM_SAMPLE_RATES; i++)
//	{
//		if(config.sampleRate == sampRateTab[i])
//		{
//			error = 0;
//			break;
//		}
//	}
//  }
//  /* check if bit rate is not too high for sample rate */
//  if (!error) {
//    if (config.bitRate > config.sampleRate*6*config.nChannelsOut) {
//      error=3;                                          
//    }
//  }

    
  if (!error) {
    hAacEnc->config = config;
  }

  if (!error) {
    error = InitElementInfo (config.nChannelsOut,
                             &hAacEnc->elInfo);
  }

  if (!error) {
    elInfo = &hAacEnc->elInfo;
  }

  if (!error) {
    Word16 tnsMask=0;                                                          

    error = psyMainInit(&hAacEnc->psyKernel,
                        config.sampleRate,
                        config.bitRate,
                        elInfo->nChannelsInEl,
                        tnsMask,
                        hAacEnc->config.bandWidth);
  }


  if(!error) {
	  hAacEnc->qcOut.qcElement.adtsUsed = config.adtsUsed;
  }

  if (!error) {
    struct QC_INIT qcInit;

    /*qcInit.channelMapping = &hAacEnc->channelMapping;*/
    qcInit.elInfo = &hAacEnc->elInfo;

    qcInit.maxBits = (Word16) (6144*elInfo->nChannelsInEl);
    qcInit.bitRes = qcInit.maxBits;                                      
    qcInit.averageBits = (Word16) ((config.bitRate * FRAME_LEN_LONG) / config.sampleRate);

    qcInit.padding.paddingRest = config.sampleRate;                          

    qcInit.meanPe = (Word16) ((10 * FRAME_LEN_LONG * hAacEnc->config.bandWidth) /
                                              (config.sampleRate>>1));

    qcInit.maxBitFac = (Word16) ((100 * (6144-744)* elInfo->nChannelsInEl)/
                                                 (qcInit.averageBits?qcInit.averageBits:1));

    qcInit.bitrate = config.bitRate;                                     

    error = QCInit(&hAacEnc->qcKernel, &qcInit);
  }

  /* init bitstream encoder */
  if (!error) {
    hAacEnc->bseInit.nChannels   = elInfo->nChannelsInEl;                
    hAacEnc->bseInit.bitrate     = config.bitRate;                       
    hAacEnc->bseInit.sampleRate  = config.sampleRate;                    
    hAacEnc->bseInit.profile     = profile;                              
  }

  return error;
}


Word16 AacEncEncode(AAC_ENCODER *aacEnc, /*!< an encoder handle */
                    Word16 *timeSignal,         /*!< BLOCKSIZE*nChannels audio samples, interleaved */
                    const UWord8 *ancBytes,     /*!< pointer to ancillary data bytes */
                    Word16 *numAncBytes,       /*!< number of ancillary Data Bytes */
                    UWord8 *outBytes,           /*!< pointer to output buffer (must be 6144/8*MAX_CHANNELS bytes large) */
                    VO_U32 *numOutBytes         /*!< number of bytes in output buffer after processing */
                    )
{
  ELEMENT_INFO *elInfo = &aacEnc->elInfo;
  Word16 globUsedBits;
  Word16 ancDataBytes, ancDataBytesLeft;
  
  ancDataBytes = ancDataBytesLeft = *numAncBytes;                          

  aacEnc->hBitStream = CreateBitBuffer(&aacEnc->bitStream, outBytes, *numOutBytes);

  psyMain(aacEnc->config.nChannelsOut,    
          elInfo,
          timeSignal,
          &aacEnc->psyKernel.psyData[elInfo->ChannelIndex[0]],
          &aacEnc->psyKernel.tnsData[elInfo->ChannelIndex[0]],
          &aacEnc->psyKernel.psyConfLong,
          &aacEnc->psyKernel.psyConfShort,
          &aacEnc->psyOut.psyOutChannel[elInfo->ChannelIndex[0]],
          &aacEnc->psyOut.psyOutElement,
          aacEnc->psyKernel.pScratchTns);

  AdjustBitrate(&aacEnc->qcKernel,
                aacEnc->config.bitRate,
                aacEnc->config.sampleRate);
    
  QCMain(&aacEnc->qcKernel,         
         &aacEnc->qcKernel.elementBits,
         &aacEnc->qcKernel.adjThr.adjThrStateElem,
         &aacEnc->psyOut.psyOutChannel[elInfo->ChannelIndex[0]],
         &aacEnc->psyOut.psyOutElement,
         &aacEnc->qcOut.qcChannel[elInfo->ChannelIndex[0]],
         &aacEnc->qcOut.qcElement,
         elInfo->nChannelsInEl,
		 min(ancDataBytesLeft,ancDataBytes));

//#ifndef MONO_ONLY
//  UpdateStereoPreProcess(&aacEnc->psyOut.psyOutChannel[elInfo->ChannelIndex[0]],
//                         &aacEnc->qcOut.qcElement,
//                         &aacEnc->stereoPrePro,
//                         aacEnc->psyOut.psyOutElement.weightMsLrPeRatio);
//#endif

  ancDataBytesLeft = ancDataBytesLeft - ancDataBytes;

  globUsedBits = FinalizeBitConsumption(&aacEnc->qcKernel,
                         &aacEnc->qcOut);

  WriteBitstream(aacEnc->hBitStream,				 
                 *elInfo,
                 &aacEnc->qcOut,
                 &aacEnc->psyOut,
                 &globUsedBits,				 
                 ancBytes,
				 aacEnc->psyKernel.sampleRateIdx);

  updateBitres(&aacEnc->qcKernel,
               &aacEnc->qcOut);

  /* write out the bitstream */
  *numOutBytes = GetBitsAvail(aacEnc->hBitStream) >> 3;

  return 0;
}


/*---------------------------------------------------------------------------

functionname:AacEncClose
description: deallocate an encoder instance

---------------------------------------------------------------------------*/

void AacEncClose (AAC_ENCODER* hAacEnc, VO_MEM_OPERATOR *pMemOP)
{
  if (hAacEnc) {  
    QCDelete(&hAacEnc->qcKernel, pMemOP);

    QCOutDelete(&hAacEnc->qcOut, pMemOP);

    PsyDelete(&hAacEnc->psyKernel, pMemOP);

    PsyOutDelete(&hAacEnc->psyOut, pMemOP);

    DeleteBitBuffer(&hAacEnc->hBitStream);

	if(hAacEnc->intbuf)
	{
		mem_free(pMemOP, hAacEnc->intbuf);
		hAacEnc->intbuf = NULL;
	}
  }
}
