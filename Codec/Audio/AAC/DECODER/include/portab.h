#ifndef _PORTAB_H_
#define _PORTAB_H_

#define USE_DIVIDE_FUNC 1

#define RENAME(func)	func

//#ifdef  NDEBUG    //disable for PPC2003 debug build

#define AllocateBuffers			RENAME(AAD_Func1)
#define FreeBuffers				RENAME(AAD_Func2)
//#define ClearBuffer				RENAME(AAD_Func3)

#define SetRawBlockParams		RENAME(AAD_Func4)
#define PrepareRawBlock			RENAME(AAD_Func5)
#define FlushCodec				RENAME(AAD_Func6)

#define UnpackADTSHeader		RENAME(AAD_Func7)
#define GetADTSChannelMapping	RENAME(AAD_Func8)
#define UnpackADIFHeader		RENAME(AAD_Func9)
#define DecodeNextElement		RENAME(AAD_Func10)
#define DecodeNoiselessData		RENAME(AAD_Func11)
#define Dequantize				RENAME(AAD_Func12)
#define StereoProcess			RENAME(AAD_Func13)
#define DeinterleaveShortBlocks	RENAME(AAD_Func14)
#define PNS						RENAME(AAD_Func15)
#define TNSFilter				RENAME(AAD_Func16)
#define IMDCT					RENAME(AAD_Func17)

#define InitSBR					RENAME(AAD_Func18)
#define voSBRExtData     		RENAME(AAD_Func19)
#define DecodeSBRData			RENAME(AAD_Func20)
#define FreeSBR					RENAME(AAD_Func21)
#define FlushCodecSBR			RENAME(AAD_Func22)

#define CLZ						RENAME(AAD_Func30)

#define Radix4_FFT				RENAME(AAD_Func37)
#define voSBR_Envelope  		RENAME(AAD_Func41)
#define voSBR_Noise 			RENAME(AAD_Func42)
#define UncoupleSBREnvelope		RENAME(AAD_Func43)
#define UncoupleSBRNoise		RENAME(AAD_Func44)
#define QMFAnalysis				RENAME(AAD_Func47)
#define QMFSynthesis			RENAME(AAD_Func48)


#define get_sr_index						RENAME(AAD_Func50)
#define voSBR_Single_Channel_Element		RENAME(AAD_Func52)
#define voSBR_Channel_Pair_Element			RENAME(AAD_Func53)


#define sampRateTab				RENAME(AAD_srtdata)
#define predSFBMax				RENAME(AAD_srtdata2)
#define channelMapTab			RENAME(AAD_srtdata3)
#define elementNumChans			RENAME(AAD_srtdata4)
#define sfBandTabShortOffset	RENAME(AAD_srtdata7)
#define sfBandTabShort			RENAME(AAD_srtdata8)
#define sfBandTabLongOffset		RENAME(AAD_srtdata9)
#define sfBandTabLong			RENAME(AAD_srtdata11)
#define tnsMaxBandsShortOffset	RENAME(AAD_srtdata12)
#define tnsMaxBandsShort		RENAME(AAD_srtdata13)

#define tnsMaxOrderShort		RENAME(AAD_srtdata14)
#define tnsMaxBandsLongOffset	RENAME(AAD_srtdata15)
#define tnsMaxBandsLong			RENAME(AAD_srtdata16)
#define tnsMaxOrderLong			RENAME(AAD_srtdata17)

#define program_config_element	RENAME(AAD_srtdata18)
#define cos4sin4tab				RENAME(AAD_srtdata20)
#define tns_data				RENAME(AAD_srtdata21)
#define raw_data_block			RENAME(AAD_srtdata22)
#define mi_decode				RENAME(AAD_srtdata23)
#define tns_decode				RENAME(AAD_srtdata25)
#define pns_data				RENAME(AAD_srtdata26)
#define pns_decode				RENAME(AAD_srtdata27)
#define filter_bank				RENAME(AAD_srtdata28)
#define dequant					RENAME(AAD_srtdata29)
#define voSqrtFix				RENAME(AAD_asrtdata26)
#define DownMixto2Chs			RENAME(AAD_asrtdata27)
#define decode_jmp				RENAME(AAD_asrtdata28)
#define error					RENAME(AAD_asrtdata29)
#define dequant_rescale			RENAME(AAD_srtdata30)			
#define k0Tab					RENAME(AAD_srtdata33)
#define k2Tab					RENAME(AAD_srtdata34)
#define huffTabSBRInfo			RENAME(AAD_srtdata36)
#define huffTabSBR				RENAME(AAD_srtdata37)
#define log2Tab					RENAME(AAD_srtdata38)
#define cTabA					RENAME(AAD_srtdata39)

#define cTabS					RENAME(AAD_srtdata43)
#define noiseTab				RENAME(AAD_srtdata44)

#define gainBits				RENAME(aad_stetdata1)
#define ics_info				RENAME(aad_stetdata2)
#define section_data			RENAME(aad_stetdata3)
#define ltp_data				RENAME(aad_stetdata4)
#define ltp_decode				RENAME(aad_stetdata5)
#define ltp_update				RENAME(aad_stetdata6)

#define scale_factor_data		RENAME(AAD_srtdata120)
#define pulse_data				RENAME(AAD_srtdata121)
#define gain_control_data		RENAME(AAD_srtdata122)
#define spectral_data			RENAME(AAD_srtdata123)
#define individual_channel_stream	      RENAME(AAD_srtdata125)
#define data_stream_element				  RENAME(AAD_srtdata126)
#define channel_pair_element			  RENAME(AAD_srtdata127)
#define fill_element					  RENAME(AAD_srtdata128)
#define coupling_channel_element		  RENAME(AAD_srtdata129)
#define single_channel_element			  RENAME(AAD_srtdata130)

/* additional external symbols to name-mangle for static linking */
#define DecodeHuffmanScalar               RENAME(AAD_srtdata54)

#define huffTabSpecInfo                   RENAME(AAD_srtdata61)
#define huffTabSpec                       RENAME(AAD_srtdata62)
#define huffTabScaleFactInfo			  RENAME(AAD_srtdata63)
#define huffTabScaleFact                  RENAME(AAD_srtdata64)
#define cos4sin4tabOffset                 RENAME(AAD_srtdata66)
#define cos1sin1tab                       RENAME(AAD_srtdata67)
#define sinWindow                         RENAME(AAD_srtdata68)
#define sinWindowOffset                   RENAME(AAD_srtdata69)

#define kbdWindow                         RENAME(AAD_srtdata70)
#define kbdWindowOffset                   RENAME(AAD_srtdata71)
#define BitRevTab                         RENAME(AAD_srtdata72)
#define bitrevtabOffset                   RENAME(AAD_srtdata73)
#define uniqueIDTab                       RENAME(AAD_srtdata74)
#define twidTabEven                       RENAME(AAD_srtdata75)
#define twidTabOdd                        RENAME(AAD_srtdata76)
//#define decoder							  RENAME(AAD_srtdata77)


#define UnpackPairsEsc					  RENAME(AAD_srthdata501)
#define UnpackPairsNoEsc				  RENAME(AAD_srthdata502)

#define UnpackQuads						  RENAME(AAD_srthdata503)
#define UnpackZeros						  RENAME(AAD_srthdata504)

#define nfftTab							  RENAME(AAD_srtidata501)
#define nfftlog2Tab						  RENAME(AAD_srtidata502)
#define BitReverse						  RENAME(AAD_srtidata503)
//#define WinLongStart					  RENAME(AAD_srtidata507)
#define WinShort						  RENAME(AAD_srtidata508)
#define WinLongStop						  RENAME(AAD_srtidata509)
#define R4Core							  RENAME(AAD_srtidata510)
#define WinLong							  RENAME(AAD_srtidata511)
#define R4FirstPass						  RENAME(AAD_srtidata512)
#define R8FirstPass						  RENAME(AAD_srtidata513)
#define PostMultiply					  RENAME(AAD_srtidata514)
#define PostMultiplyRescale				  RENAME(AAD_srtidata515)
#define PreMultiply						  RENAME(AAD_srtidata516)
#define PreMultiplyRescale				  RENAME(AAD_srtidata517)
#define PreMDCTMultiply					  RENAME(AAD_srtidata518)
#define PostMDCTMultiply				  RENAME(AAD_srtidata519)
#define MDCT							  RENAME(AAD_srtidata520)
#define nmdctTab						  RENAME(AAD_srtidata81)
#define postSkip						  RENAME(AAD_srtidata82)

#ifndef _IOS    /*add iOS ARMv7 support 2011/7/27 Huaping*/
#define CVKernel1						  sbrasm1
#define CVKernel2						  sbrasm2
#define QMFAnalysisConv					  sbrasm3
#define QMFSynthesisConv				  sbrasm4
#endif

//#endif

#endif//_PORTAB_H_