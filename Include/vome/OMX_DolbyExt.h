/**
  @file src/components/audio_effects/OMX_DolbyExt.h

  OpenMAX Dolby Audio Kernel component. 

  Copyright (C) 2009 Dolby Laboratories

  $Date: 2009-02-02 09:51:00 +1000 (Fri, 2 February 2009) $
  Revision $Rev: 001 $
  Author $Author: Eytan_Rubin $

*/
#ifndef _OMX_DOLBY_EXT_H_
#define _OMX_DOLBY_EXT_H_

#include <OMX_Index.h>


/** this is the list of custom vendor index */
typedef enum OMX_INDEX_DAK_TYPE {
	OMX_IndexConfigDAK_BUNDLE = OMX_IndexVendorStartUnused /*0x7F000000*/,	/**< reference: OMX_DAK_CONFIG_BUNDLETYPE */
	OMX_IndexConfigDAK_M2S,	/**< reference: OMX_DAK_CONFIG_M2STYPE */
	OMX_IndexConfigDAK_SSE,	/**< reference: OMX_DAK_CONFIG_SSETYPE */
	OMX_IndexConfigDAK_SLC,	/**< reference: OMX_DAK_CONFIG_SLCTYPE */
	OMX_IndexConfigDAK_VOL,	/**< reference: OMX_DAK_CONFIG_VOLTYPE */
	OMX_IndexConfigDAK_NB,	/**< reference: OMX_DAK_CONFIG_NBTYPE */
	OMX_IndexConfigDAK_GEQ,	/**< reference: OMX_DAK_CONFIG_GEQTYPE */
	OMX_IndexConfigDAK_MSR, /**< reference: OMX_DAK_CONFIG_MSRTYPE */
	OMX_IndexConfigDAK_HFE,	/**< reference: OMX_DAK_CONFIG_HFETYPE */
	OMX_IndexConfigDAK_FADE,/**< reference: OMX_DAK_CONFIG_FADETYPE */
	OMX_IndexConfigDAK_SEQ,	/**< reference: OMX_DAK_CONFIG_SEQTYPE */
	OMX_IndexConfigDAK_DEFAULT

} OMX_INDEX_DAK_TYPE;


/** Dolby Audio Kernel TDAS bundle */
typedef struct OMX_DAK_CONFIG_BUNDLETYPE {
    	OMX_U32 nSize;              	/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_U32 nDAK_Version;		/**< Dolby Audio Kernel version information */
	OMX_U32 nDAK_Revision;		/**< Dolby Audio Kernel revision information */
	OMX_U8 nLfeMixLevel;		/**< level at which the LFE channel is mixed into the output audio */
	OMX_U8 nSampleRateIndex;	/**< Output sample rate */
	OMX_U8 nInChans;		/**< Channel format of input audio */
	OMX_U8 nInMatrix;		/**< L/R mixing specification for stereo audio input */
	OMX_U8 nBypass;			/**< Audio Processing bypass */
	OMX_U8 nRateMultipier;		/**< Sample-rate multiplier (output with respect to input) */
	OMX_U8 nInChanFormat;		/**< Input/Output channel format */
	OMX_U8 nMsrMaxProfile;		/**< Maximum number of virtual rendering channels in Mobile Surround */
} OMX_DAK_CONFIG_BUNDLETYPE;

/** Dolby Audio Kernel Mono-to-Stereo Creator */
typedef struct OMX_DAK_CONFIG_M2STYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Mono-to-Stereo Creator enable */
	OMX_BOOL bDetector;		/**< Stereo detector status */
} OMX_DAK_CONFIG_M2STYPE;

/** Dolby Audio Kernel Sound Space Expander */
typedef struct OMX_DAK_CONFIG_SSETYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Sound Space Expander enable */
	OMX_U8 nWidth;			/**< Width of expansion effect */
	OMX_U8 nSpkMode;		/**< Speaker Mode */
} OMX_DAK_CONFIG_SSETYPE;

/** Dolby Audio Kernel Sound Level Controller */
typedef struct OMX_DAK_CONFIG_SLCTYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Sound Level Controller enable */
	OMX_U8 nLevel;			/**< Source audio RMS level */
	OMX_U8 nDepth;			/**< Depth of effect */
} OMX_DAK_CONFIG_SLCTYPE;

/** Dolby Audio Kernel Volume */
typedef struct OMX_DAK_CONFIG_VOLTYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Volume enable */
	OMX_U8 nGainType;		/**< Linear/Logarithmic audio scaling */
	OMX_U8 nInternalGain;		/**< Audio volume scale */
	OMX_U8 nExternalGain;		/**< Audio volume scale applied by external volume control */
	OMX_S8 nBalance;		/**< L/R panning for output audio */
	OMX_BOOL bMute;			/**< Audio Mute */ 
} OMX_DAK_CONFIG_VOLTYPE;

/** Dolby Audio Kernel Natural Bass */
typedef struct OMX_DAK_CONFIG_NBTYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Natural Bass enable */
	OMX_U8 nCutoff;			/**< Speakers/headphones lower cutoff frequency */
	OMX_U8 nBoost;			/**< Strength of effect */
	OMX_U8 nLevel;			/**< Maximum output level capability of speakers/headphones */
} OMX_DAK_CONFIG_NBTYPE;

/** Dolby Audio Kernel Graphic EQ */
typedef struct OMX_DAK_CONFIG_GEQTYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Graphic EQ enable */
	OMX_U8 nNbands;			/**< Number of frequency bands */
	OMX_S8 nPreamp;			/**< Global attenuation to apply prior to band level adjustment */
	OMX_U8 nMaxboost;		/**< Maximum absolute boost with respect to the source audio */
	OMX_S8 nBand1;			/**< Boost/cut for 1st frequency band */
	OMX_S8 nBand2;			/**< Boost/cut for 2nd frequency band */
	OMX_S8 nBand3;			/**< Boost/cut for 3rd frequency band */
	OMX_S8 nBand4;			/**< Boost/cut for 4th frequency band */
	OMX_S8 nBand5;			/**< Boost/cut for 5th frequency band */
	OMX_S8 nBand6;			/**< Boost/cut for 6th frequency band */
	OMX_S8 nBand7;			/**< Boost/cut for 7th frequency band */
} OMX_DAK_CONFIG_GEQTYPE;

/** Dolby Audio Kernel, Mobile Surround and Surround Upmixer */
typedef struct OMX_DAK_CONFIG_MSRTYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bMsrEnable;		/**< Mobile Surround enable */
	OMX_U8 nMsrRoom;		/**< Room Size control */
	OMX_U8 nMsrBright;		/**< Brightness control */
	OMX_BOOL bMupEnable;		/**< Mobile Surround Upmixer enable */
} OMX_DAK_CONFIG_MSRTYPE;

/** Dolby Audio Kernel High Frequency Enhancer */
typedef struct OMX_DAK_CONFIG_HFETYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< High Frequency Enhancer enable */
	OMX_U8 nDepth;			/**< Strength of effect */
} OMX_DAK_CONFIG_HFETYPE;

/** Dolby Audio Kernel Fade */
typedef struct OMX_DAK_CONFIG_FADETYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Fade enable */
	OMX_U8 nTarget;			/**< Target fade level */
	OMX_U16 nTime;			/**< Fade time interval */
} OMX_DAK_CONFIG_FADETYPE;

/** Dolby Audio Kernel Speaker EQ */
typedef struct OMX_DAK_CONFIG_SEQTYPE {
	OMX_U32 nSize;			/**< size of the structure in bytes */
	OMX_VERSIONTYPE nVersion;	/**< OMX specification version information */
	OMX_BOOL bEnable;		/**< Speaker EQ enable */
	OMX_S8 nLeftGainDB;		/**< Additional gain for Left channel */
	OMX_S8 nRightGainDB;		/**< Additional gain for Right channel */
	OMX_U8 nCoef48000Size;		/**< Length of the block of coefficients for 48KHz Sampling Rate case */
	OMX_PTR pCoef48000;		/**< Pointer to the block of coefficients for the 48KHz case */
	OMX_U8 nCoef44100Size;		/**< Length of the block of coefficients for 44.1KHz Sampling Rate case */
	OMX_PTR pCoef44100;		/**< Pointer to the block of coefficients for the 44.1KHz case */
	OMX_U8 nCoef32000Size;		/**< Length of the block of coefficients for 32KHz Sampling Rate case */
	OMX_PTR pCoef32000;		/**< Pointer to the block of coefficients for the 32KHz case */
	OMX_U8 nCoef24000Size;		/**< Length of the block of coefficients for 24KHz Sampling Rate case */
	OMX_PTR pCoef24000;		/**< Pointer to the block of coefficients for the 24KHz case */

} OMX_DAK_CONFIG_SEQTYPE;


typedef enum OMX_AUDIO_CHANNELTYPE_EXTENSION {
    OMX_AUDIO_ChannelTS = OMX_AUDIO_ChannelVendorStartUnused,    /**< Top Surround */
    OMX_AUDIO_ChannelCVH    /**< Central Vertical Height */
} OMX_AUDIO_CHANNELTYPE_EXTENSION;

#endif
