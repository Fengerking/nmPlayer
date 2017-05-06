	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		voCOMXAudioDec.h

	Contains:	voCOMXAudioDec header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2009-04-01	JBF			Create file

*******************************************************************************/

#ifndef __voCOMXAudioDec_H__
#define __voCOMXAudioDec_H__

#include <voOMX_Index.h>

#include <voCOMXCompFilter.h>

#include "dts_package_version.h"
#include "dtshd_frame_player_api.h"
#include "dtshd_parser_api.h"

/* Extension to OMX_AUDIO_CHANNELTYPE */
typedef enum
{
	// VisualOn specific area
	OMX_VO_AUDIO_ChannelStartUnused = 0x7F000000,

	 OMX_AUDIO_ChannelLSS    = OMX_VO_AUDIO_ChannelStartUnused + 0x1,/**< Left Surround on Side */
    OMX_AUDIO_ChannelRSS,    /**< Right Surround on Side  */
    OMX_AUDIO_ChannelLC,     /**< Between Left and Centre in front  */
    OMX_AUDIO_ChannelRC,     /**< Between Right and Centre in front  */
    OMX_AUDIO_ChannelLH,     /**< Left Height in front */
    OMX_AUDIO_ChannelCH,     /**< Centre Height in Front  */
    OMX_AUDIO_ChannelRH,     /**< Right Height in front  */
    OMX_AUDIO_ChannelLFE2,   /**< Low Frequency Effects 2 */
    OMX_AUDIO_ChannelLW,     /**< Left on side in front */
    OMX_AUDIO_ChannelRW,     /**< Right on side in front  */
    OMX_AUDIO_ChannelOH,     /**< Over the listeners Head */
    OMX_AUDIO_ChannelLHS,    /**< Left Height on Side */
    OMX_AUDIO_ChannelRHS,    /**< Right Height on Side  */
    OMX_AUDIO_ChannelCHR,    /**< Centre Height in Rear  */
    OMX_AUDIO_ChannelLHR,    /**< Left Height in Rear */
    OMX_AUDIO_ChannelRHR,    /**< Right Height in Rear  */
    OMX_AUDIO_ChannelCLF,	 /*	< Low Center in Front */
    OMX_AUDIO_ChannelLLF,	 /*	< Low Left in Front */
    OMX_AUDIO_ChannelRLF,	 /*	< Low Right in Front */
    OMX_AUDIO_ChannelLT,     /**< Left Total */
    OMX_AUDIO_ChannelRT      /**< Right Total */
} OMX_AUDIO_DTS_CHANNELTYPE;


typedef enum OMX_AUDIO_DTS_SPKROUTTYPE
{
    OMX_AUDIO_DTSSPKROUT_MASK_C 		= 0x00001,
    OMX_AUDIO_DTSSPKROUT_MASK_LR		= 0x00002,
    OMX_AUDIO_DTSSPKROUT_MASK_LsRs 	    = 0x00004,
    OMX_AUDIO_DTSSPKROUT_MASK_LFE1      = 0x00008,
    OMX_AUDIO_DTSSPKROUT_MASK_Cs 		= 0x00010,
    OMX_AUDIO_DTSSPKROUT_MASK_LhRh 	    = 0x00020,
    OMX_AUDIO_DTSSPKROUT_MASK_LsrRsr	= 0x00040,
    OMX_AUDIO_DTSSPKROUT_MASK_Ch 		= 0x00080,
    OMX_AUDIO_DTSSPKROUT_MASK_Oh 		= 0x00100,
    OMX_AUDIO_DTSSPKROUT_MASK_LcRc	    = 0x00200,
    OMX_AUDIO_DTSSPKROUT_MASK_LwRw	    = 0x00400,
    OMX_AUDIO_DTSSPKROUT_MASK_LssRss	= 0x00800,
    OMX_AUDIO_DTSSPKROUT_MASK_LFE_2	    = 0x01000,
    OMX_AUDIO_DTSSPKROUT_MASK_LhsRhs	= 0x02000,
    OMX_AUDIO_DTSSPKROUT_MASK_Chr		= 0x04000,
    OMX_AUDIO_DTSSPKROUT_MASK_LhrRhr	= 0x08000,
    OMX_AUDIO_DTSSPKROUT_MASK_Clf		= 0x10000,
    OMX_AUDIO_DTSSPKROUT_MASK_LlfRlf	= 0x20000,
    OMX_AUDIO_DTSSPKROUT_MASK_LtRt	    = 0x40000
} OMX_AUDIO_DTS_SPKROUTTYPE;


typedef enum
{
    OMX_AUDIO_DTSSPEAKERMASK_CENTRE = 0x00000001,   /**< Centre */
    OMX_AUDIO_DTSSPEAKERMASK_LEFT   = 0x00000002,   /**< Left  */
    OMX_AUDIO_DTSSPEAKERMASK_RIGHT  = 0x00000004,   /**< Right  */
    OMX_AUDIO_DTSSPEAKERMASK_LS     = 0x00000008,   /**< Left Surround */
    OMX_AUDIO_DTSSPEAKERMASK_RS     = 0x00000010,   /**< Right Surround  */
    OMX_AUDIO_DTSSPEAKERMASK_LFE1   = 0x00000020,   /**< Low Frequency Effects 1 */
    OMX_AUDIO_DTSSPEAKERMASK_Cs     = 0x00000040,   /**< Center Surround  */
    OMX_AUDIO_DTSSPEAKERMASK_Lsr    = 0x00000080,   /**< Left Surround in Rear  */
    OMX_AUDIO_DTSSPEAKERMASK_Rsr    = 0x00000100,   /**< Right Surround in Rear  */
    OMX_AUDIO_DTSSPEAKERMASK_Lss    = 0x00000200,   /**< Left Surround on Side */
    OMX_AUDIO_DTSSPEAKERMASK_Rss    = 0x00000400,   /**< Right Surround on Side  */
    OMX_AUDIO_DTSSPEAKERMASK_Lc     = 0x00000800,   /**< Between Left and Centre in front  */
    OMX_AUDIO_DTSSPEAKERMASK_Rc     = 0x00001000,   /**< Between Right and Centre in front  */
    OMX_AUDIO_DTSSPEAKERMASK_Lh     = 0x00002000,   /**< Left Height in front */
    OMX_AUDIO_DTSSPEAKERMASK_Ch     = 0x00004000,   /**< Centre Height in Front  */
    OMX_AUDIO_DTSSPEAKERMASK_Rh     = 0x00008000,   /**< Right Height in front  */
    OMX_AUDIO_DTSSPEAKERMASK_LFE2   = 0x00010000,   /**< Low Frequency Effects 2 */
    OMX_AUDIO_DTSSPEAKERMASK_Lw     = 0x00020000,   /**< Left on side in front */
    OMX_AUDIO_DTSSPEAKERMASK_Rw     = 0x00040000,   /**< Right on side in front  */
    OMX_AUDIO_DTSSPEAKERMASK_Oh     = 0x00080000,   /**< Over the listeners Head */
    OMX_AUDIO_DTSSPEAKERMASK_Lhs    = 0x00100000,   /**< Left Height on Side */
    OMX_AUDIO_DTSSPEAKERMASK_Rhs    = 0x00200000,   /**< Right Height on Side  */
    OMX_AUDIO_DTSSPEAKERMASK_Chr    = 0x00400000,   /**< Centre Height in Rear  */
    OMX_AUDIO_DTSSPEAKERMASK_Lhr    = 0x00800000,   /**< Left Height in Rear */
    OMX_AUDIO_DTSSPEAKERMASK_Rhr    = 0x01000000,   /**< Right Height in Rear  */
    OMX_AUDIO_DTSSPEAKERMASK_Clf	= 0x02000000,   /**< Low Center in Front */
    OMX_AUDIO_DTSSPEAKERMASK_Llf	= 0x04000000,   /**< Low Left in Front */
    OMX_AUDIO_DTSSPEAKERMASK_Rlf	= 0x08000000,   /**< Low Right in Front */
    OMX_AUDIO_DTSSPEAKERMASK_Lt     = 0x10000000,
    OMX_AUDIO_DTSSPEAKERMASK_Rt     = 0x20000000
} OMX_AUDIO_DTS_SPEAKERTYPE;

class voCOMXAudioDTSDec : public voCOMXCompFilter
{
public:
	voCOMXAudioDTSDec(OMX_COMPONENTTYPE * pComponent);
	virtual ~voCOMXAudioDTSDec(void);

	virtual OMX_ERRORTYPE	EmptyThisBuffer (OMX_IN  OMX_HANDLETYPE hComponent,
											 OMX_IN  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE	GetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nParamIndex,  
										OMX_INOUT OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE	SetParameter (OMX_IN  OMX_HANDLETYPE hComponent, 
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentParameterStructure);

	virtual OMX_ERRORTYPE GetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_INOUT OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE SetConfig (OMX_IN  OMX_HANDLETYPE hComponent,
										OMX_IN  OMX_INDEXTYPE nIndex,
										OMX_IN  OMX_PTR pComponentConfigStructure);


	virtual OMX_ERRORTYPE ComponentRoleEnum (OMX_IN OMX_HANDLETYPE hComponent,
												OMX_OUT OMX_U8 *cRole,
												OMX_IN OMX_U32 nIndex);

protected:
	virtual OMX_ERRORTYPE	InitPortType (void);

	virtual OMX_ERRORTYPE	SetNewState (OMX_STATETYPE newState);
	virtual OMX_ERRORTYPE	Flush (OMX_U32	nPort);

	OMX_ERRORTYPE CopyPCMSamples(OMX_U32 nSamples, dtsDecoderPCMOutput *pPCMOutput);
	OMX_ERRORTYPE ResetDTSParser();
	OMX_ERRORTYPE ResetDTSDecoder();

protected:
	virtual OMX_ERRORTYPE	FillBuffer (OMX_BUFFERHEADERTYPE * pInput, OMX_BOOL * pEmptied,
										OMX_BUFFERHEADERTYPE * pOutput, OMX_BOOL * pFilled);
protected:
	OMX_AUDIO_CODINGTYPE			m_nCoding;
	OMX_PTR							m_pFormatData;
	OMX_U32							m_nExtSize;
	OMX_S8 *						m_pExtData;

	OMX_AUDIO_PARAM_PCMMODETYPE		m_pcmType;

	OMX_U32							m_nSampleRate;
	OMX_U32							m_nChannels;
	OMX_U32							m_nBits;

	OMX_BOOL						m_bSetThreadPriority;

protected:
	VO_U32							m_nCodec;

	VO_CODECBUFFER					m_inBuffer;
	VO_CODECBUFFER					m_outBuffer;

	VO_U32							m_nSysStartTime;
	VO_U32							m_nDecFrames;

	VO_U32							m_nStepSize;
	VO_S64							m_nOutputSize;
	VO_BOOL							m_bNofityError;
	OMX_BOOL							m_bDownMix;

	VO_U32							out_pcm_mode_speaker_mask;

    dtsParser parser;                                           /* DTS parser instance */
    dtsUint32 nParserFlags;
    dtsUint32 *pnParserBitstream;
    dtsUint32 nParserFrameSize;
    OMX_BOOL bParserNewTimeStamp;
    OMX_BOOL bParserTimeStampAvailable;
    OMX_TICKS nParserTimeStamp;

    dtsFramePlayer player;                                      /* DTS decoder instance */

    dtsDecoderPCMOutput *m_pPCMOutput;
	dtsDecoderPCMOutput *m_pPCMOutput20;
	dtsInt32 *ppnPlayerPCMBuf[DTS_MAX_CHANNELS];
	dtsUint32 nPlayerFlags;
	VO_U32 nPlayerAvailableSamples;
	VO_U32 nPlayerDecodedSamples;

	OMX_U32 nExSSID;                        /**< ExSS Id from which audio presentation is selected, 0~3. Default is 0. */
	OMX_U32 nAudioPresentIndex;             /**< Audio presentation index, 0~7. Default is 0. */
	OMX_U32 nDRCPercent;                    /**< Percentage of DRC to be applied, 0~100. Default is 0. */
	OMX_BOOL bDialNorm;                     /**< Enable or disable dialog normalization. Default is OMX_FALSE. */
	OMX_AUDIO_DTS_SPKROUTTYPE nSpkrOut;     /**< Requested speaker mask. Default is DTSSPKROUT_MASK_LR. 
                                                 Actual output speaker mask depends on input bit stream, may be different from this requested one.
                                                 See OMX_GetParameter, OMX_IndexParamAudioPcm and OMX_AUDIO_PARAM_PCMMODETYPE. */
    OMX_BOOL bMixLFEIntoFront;              /**< Enable or disable mixing LFE into front channels. Default is OMX_FALSE. */
	OMX_U32 nOutputBitWidth ;               /**< Output PCM bit width, 24 or 16. Default is 16.
                                                 It determines nBitPerSample in OMX_AUDIO_PARAM_PCMMODETYPE */
    OMX_U32 nMaxSampleRate;                 /**< Maximum sample rate of the stream. Decoded sample rate is reported in OMX_AUDIO_PARAM_PCMMODETYPE */
    OMX_U32 nSamplesInFrameAtMaxSR;         /**< Frame size at maximum sample rate. */
    OMX_U32 nMaxNumChannels;                /**< Maximum number of channels in the stream. Decoded channles are reported in OMX_AUDIO_PARAM_PCMMODETYPE */
    dtsUint32 nMaxChannelMask;					/**< Maximum channel mask in the stream. See OMX_AUDIO_DTS_SPEAKERTYPE */
};

#endif //__voCOMXAudioDec_H__
