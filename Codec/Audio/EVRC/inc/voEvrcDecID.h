#ifndef __VOOGGDECID_H_
#define __VOOGGDECID_H_

#ifndef _WIN32_WCE
//#ifndef _DEBUG

#define RENAME(func)	_##func

//#define ACELP_Code                                    voEVRCDec00000000 
#define BitUnpack                                     voEVRCDec00000001 
#define Bitpack                                       voEVRCDec00000002 
#define ComputeACB                                    voEVRCDec00000003 
//#define ConIR_asm                                     voEVRCDec00000004 
#define ConvolveImpulseR                              voEVRCDec00000005 
#define Fix_delay_contour                             voEVRCDec00000006 
//#define FlushFrameBuffer                              voEVRCDec00000007 
#define FrameErrorHandler                             voEVRCDec00000008 
#define GAMMA11                                       voEVRCDec00000009 
#define GAMMA13                                       voEVRCDec00000010 
#define G_code                                        voEVRCDec00000011 
#define GetExc800bps                                  voEVRCDec00000012 
#define GetExc800bps_dec                              voEVRCDec00000013 
#define GetResidual                                   voEVRCDec00000014 
//#define GetResidual_asm                               voEVRCDec00000015 
#define ImpulseRzp                                    voEVRCDec00000016 
#define ImpulseRzp1                                   voEVRCDec00000017 
//#define ImpulseRzp1_asm                               voEVRCDec00000018 
#define ImpulseRzp2                                   voEVRCDec00000019 
#define InitDecoder                                   voEVRCDec00000020 
#define InitEncoder                                   voEVRCDec00000021 
//#define InitFrameBuffer                               voEVRCDec00000022 
#define Interpol                                      voEVRCDec00000023 
#define Interpol_delay                                voEVRCDec00000024 
//#define L_divide                                      voEVRCDec00000025 
//#define L_mpy_ll                                      voEVRCDec00000026 
#define Logqtbl                                       voEVRCDec00000027 
#define PickToAverage                                 voEVRCDec00000028 
#define Powqtbl                                       voEVRCDec00000029 
#define SetDecodeRate                                 voEVRCDec00000030 
#define SetEncodeRate                                 voEVRCDec00000031 
#define SynthesisFilter                               voEVRCDec00000032 
#define Table                                         voEVRCDec00000033 
#define Table1                                        voEVRCDec00000034 
//#define UpdateFrameBuffer                             voEVRCDec00000035 
#define Weight2Res                                    voEVRCDec00000036 
#define ZeroInput                                     voEVRCDec00000037 
#define __data_start                                  voEVRCDec00000038 
#define a2lsp                                         voEVRCDec00000039 
#define acb_excitation                                voEVRCDec00000040 
#define apf                                           voEVRCDec00000041 
#define autocorrelation                               voEVRCDec00000042 
#define band_energy_fcn                               voEVRCDec00000043 
#define bl_intrp                                      voEVRCDec00000044 
#define bl_intrp_asm                                  voEVRCDec00000045 
#define _bl_intrp_asm                                _voEVRCDec00000045 
#define block_denorm                                  voEVRCDec00000046 
#define block_norm                                    voEVRCDec00000047 
#define bq_w                                          voEVRCDec00000048 
#define bqiir                                         voEVRCDec00000049 
#define c_fft                                         voEVRCDec00000050 
//#define cmnMemAlloc                                   voEVRCDec00000051 
//#define cmnMemCheck                                   voEVRCDec00000052 
#define cmnMemCompare                                 voEVRCDec00000053 
//#define cmnMemCopy                                    voEVRCDec00000054 
#define cmnMemFillPointer                             voEVRCDec00000055 
//#define cmnMemFree                                    voEVRCDec00000056 
#define cmnMemMove                                    voEVRCDec00000057 
//#define cmnMemSet                                     voEVRCDec00000058 
#define code_3i54_10bits                              voEVRCDec00000059 
#define code_8i55_35bits                              voEVRCDec00000060 
//#define cor_h_vec123                                  voEVRCDec00000061 
//#define cor_h_x2_asm                                  voEVRCDec00000062 
#define cos129_table                                  voEVRCDec00000063 
#define cshiftframe                                   voEVRCDec00000064 
//#define dec3_10                                       voEVRCDec00000065 
//#define dec8_35                                       voEVRCDec00000066 
#define decode                                        voEVRCDec00000067 
#define decode_fer                                    voEVRCDec00000068 
#define decode_no_fer                                 voEVRCDec00000069 
#define decode_rate_1                                 voEVRCDec00000070 
#define durbin                                        voEVRCDec00000071 
#define e_ran_g                                       voEVRCDec00000072 
#define encode                                        voEVRCDec00000073 
#define evrc_framelen                                 voEVRCDec00000074 
#define fcb_gainq                                     voEVRCDec00000075 
#define fir                                           voEVRCDec00000076 
//#define fir_asm                                       voEVRCDec00000077 
//#define fnExp10                                       voEVRCDec00000078 
#define fnExp2                                        voEVRCDec00000079 
//#define fnLog                                         voEVRCDec00000080 
//#define fnLog10                                       voEVRCDec00000081 
#define fnLog2                                        voEVRCDec00000082 
#define fndppf                                        voEVRCDec00000083 
#define g_hEVRCDecInst                                voEVRCDec00000084 
#define g_memOP                                       voEVRCDec00000085 
#define getgain                                       voEVRCDec00000086 
#define gnvq_4                                        voEVRCDec00000087 
#define gnvq_8                                        voEVRCDec00000088 
#define hamm_table                                    voEVRCDec00000089 
#define iir                                           voEVRCDec00000090 
//#define iir_asm                                       voEVRCDec00000091 
//#define initialize_rate                               voEVRCDec00000092 
//#define interpolation_cos129                          voEVRCDec00000093 
#define ipos                                          voEVRCDec00000094 
#define lognsize22                                    voEVRCDec00000095 
#define lognsize28                                    voEVRCDec00000096 
#define lognsize8                                     voEVRCDec00000097 
#define lpcanalys                                     voEVRCDec00000098 
#define lsp2a                                         voEVRCDec00000099 
#define lspmaq                                        voEVRCDec00000100 
#define lspmaq1                                       voEVRCDec00000101 
#define lspmaq_dec                                    voEVRCDec00000102 
#define lsptab22                                      voEVRCDec00000103 
#define lsptab28                                      voEVRCDec00000104 
#define lsptab8                                       voEVRCDec00000105 
#define maxeloc                                       voEVRCDec00000106 
#define mod                                           voEVRCDec00000107 
#define modifyorig                                    voEVRCDec00000108 
#define noise_suprs                                   voEVRCDec00000109 
#define nsize22                                       voEVRCDec00000110 
#define nsize28                                       voEVRCDec00000111 
#define nsize8                                        voEVRCDec00000112 
#define nsub22                                        voEVRCDec00000113 
#define nsub28                                        voEVRCDec00000114 
#define nsub8                                         voEVRCDec00000115 
//#define pit_shrp                                      voEVRCDec00000116 
#define post_encode                                   voEVRCDec00000117 
#define ppvq                                          voEVRCDec00000118 
#define ppvq_mid                                      voEVRCDec00000119 
#define pre_encode                                    voEVRCDec00000120 
#define putacbc                                       voEVRCDec00000121 
//#define r_fft                                         voEVRCDec00000122 
#define ran0                                          voEVRCDec00000123 
#define ran_g                                         voEVRCDec00000124 
//#define rnd_delay                                     voEVRCDec00000125 
//#define select_rate                                   voEVRCDec00000126 
#define sin129_table                                  voEVRCDec00000127 
#define sin_cosT                                      voEVRCDec00000128 
//#define sqroot                                        voEVRCDec00000129 
#define update_background                             voEVRCDec00000130 
#define voEVRCCodecInv_sqrt                           voEVRCDec00000131 
#define voEVRCCodec_cor_h_x                           voEVRCDec00000132 
//#define voEVRCCodec_mem_free                          voEVRCDec00000133 
//#define voEVRCCodec_mem_malloc                        voEVRCDec00000134 
#define voEVRCDec_GetOutputData                       voEVRCDec00000135 
#define voEVRCDec_GetParam                            voEVRCDec00000136 
#define voEVRCDec_Init                                voEVRCDec00000137 
#define voEVRCDec_SetInputData                        voEVRCDec00000138 
#define voEVRCDec_SetParam                            voEVRCDec00000139 
#define voEVRCDec_Uninit                              voEVRCDec00000140 
//#define voGetEVRCDecAPI                               voEVRCDec00000141 
#define w_long                                        voEVRCDec00000142 
#define w_table                                       voEVRCDec00000143 
#define weight                                        voEVRCDec00000144 
#define wfac_ALPHA                                    voEVRCDec00000145 
#define wfac_BETA                                     voEVRCDec00000146 
#define wfac_GAMMA1                                   voEVRCDec00000147 
#define wfac_GAMMA2                                   voEVRCDec00000148 
#define wfac_Gamma_4                                  voEVRCDec00000149 

//#define ACELP_Code                                  RENAME(  voEVRCDec00000000) 
#define _BitUnpack                                     RENAME(voEVRCDec00000001  )
#define _Bitpack                                       RENAME(voEVRCDec00000002  )
#define _ComputeACB                                    RENAME(voEVRCDec00000003  )
#define _ConIR_asm                                     RENAME(voEVRCDec00000004  )
#define _ConvolveImpulseR                              RENAME(voEVRCDec00000005  )
#define _Fix_delay_contour                             RENAME(voEVRCDec00000006  )
//#define FlushFrameBuffer                            RENAME(  voEVRCDec00000007) 
#define _FrameErrorHandler                             RENAME(voEVRCDec00000008  )
#define _GAMMA11                                       RENAME(voEVRCDec00000009  )
#define _GAMMA13                                       RENAME(voEVRCDec00000010  )
#define _G_code                                        RENAME(voEVRCDec00000011  )
#define _GetExc800bps                                  RENAME(voEVRCDec00000012  )
#define _GetExc800bps_dec                              RENAME(voEVRCDec00000013  )
#define _GetResidual                                   RENAME(voEVRCDec00000014  )
#define _GetResidual_asm                               RENAME(voEVRCDec00000015  )
#define _ImpulseRzp                                    RENAME(voEVRCDec00000016  )
#define _ImpulseRzp1                                   RENAME(voEVRCDec00000017  )
#define _ImpulseRzp1_asm                               RENAME(voEVRCDec00000018  )
#define _ImpulseRzp2                                   RENAME(voEVRCDec00000019  )
#define _InitDecoder                                   RENAME(voEVRCDec00000020  )
#define _InitEncoder                                   RENAME(voEVRCDec00000021  )
//#define InitFrameBuffer                             RENAME(  voEVRCDec00000022) 
#define _Interpol                                      RENAME(voEVRCDec00000023  )
#define _Interpol_delay                                RENAME(voEVRCDec00000024  )
//#define L_divide                                    RENAME(  voEVRCDec00000025) 
//#define L_mpy_ll                                    RENAME(  voEVRCDec00000026) 
#define _Logqtbl                                       RENAME(voEVRCDec00000027  )
#define _PickToAverage                                 RENAME(voEVRCDec00000028  )
#define _Powqtbl                                       RENAME(voEVRCDec00000029  )
#define _SetDecodeRate                                 RENAME(voEVRCDec00000030  )
#define _SetEncodeRate                                 RENAME(voEVRCDec00000031  )
#define _SynthesisFilter                               RENAME(voEVRCDec00000032  )
#define _Table                                         RENAME(voEVRCDec00000033  )
#define _Table1                                        RENAME(voEVRCDec00000034  )
//#define UpdateFrameBuffer                           RENAME(  voEVRCDec00000035) 
#define _Weight2Res                                    RENAME(voEVRCDec00000036  )
#define _ZeroInput                                     RENAME(voEVRCDec00000037  )
//#define __data_start                                  RENAME(voEVRCDec00000038  )
#define _a2lsp                                         RENAME(voEVRCDec00000039  )
#define _acb_excitation                                RENAME(voEVRCDec00000040  )
#define _apf                                           RENAME(voEVRCDec00000041  )
#define _autocorrelation                               RENAME(voEVRCDec00000042  )
#define _band_energy_fcn                               RENAME(voEVRCDec00000043  )
#define _bl_intrp                                      RENAME(voEVRCDec00000044  )
//#define _bl_intrp_asm                                  RENAME(voEVRCDec00000045  )
#define _block_denorm                                  RENAME(voEVRCDec00000046  )
#define _block_norm                                    RENAME(voEVRCDec00000047  )
#define _bq_w                                          RENAME(voEVRCDec00000048  )
#define _bqiir                                         RENAME(voEVRCDec00000049  )
#define _c_fft                                         RENAME(voEVRCDec00000050  )
//#define cmnMemAlloc                                 RENAME(  voEVRCDec00000051) 
//#define cmnMemCheck                                 RENAME(  voEVRCDec00000052) 
#define _cmnMemCompare                                 RENAME(voEVRCDec00000053  )
//#define cmnMemCopy                                  RENAME(  voEVRCDec00000054) 
#define _cmnMemFillPointer                             RENAME(voEVRCDec00000055  )
//#define cmnMemFree                                  RENAME(  voEVRCDec00000056) 
#define _cmnMemMove                                    RENAME(voEVRCDec00000057  )
//#define cmnMemSet                                   RENAME(  voEVRCDec00000058) 
#define _code_3i54_10bits                              RENAME(voEVRCDec00000059  )
#define _code_8i55_35bits                              RENAME(voEVRCDec00000060  )
//#define _cor_h_vec123                                  RENAME(voEVRCDec00000061  )
//#define _cor_h_x2_asm                                  RENAME(voEVRCDec00000062  )
#define _cos129_table                                  RENAME(voEVRCDec00000063  )
#define _cshiftframe                                   RENAME(voEVRCDec00000064  )
//#define dec3_10                                     RENAME(  voEVRCDec00000065) 
//#define dec8_35                                     RENAME(  voEVRCDec00000066) 
#define _decode                                        RENAME(voEVRCDec00000067  )
#define _decode_fer                                    RENAME(voEVRCDec00000068  )
#define _decode_no_fer                                 RENAME(voEVRCDec00000069  )
#define _decode_rate_1                                 RENAME(voEVRCDec00000070  )
#define _durbin                                        RENAME(voEVRCDec00000071  )
#define _e_ran_g                                       RENAME(voEVRCDec00000072  )
#define _encode                                        RENAME(voEVRCDec00000073  )
#define _evrc_framelen                                 RENAME(voEVRCDec00000074  )
#define _fcb_gainq                                     RENAME(voEVRCDec00000075  )
#define _fir                                           RENAME(voEVRCDec00000076  )
//#define fir_asm                                     RENAME(  voEVRCDec00000077) 
//#define fnExp10                                     RENAME(  voEVRCDec00000078) 
#define _fnExp2                                        RENAME(voEVRCDec00000079  )
//#define fnLog                                       RENAME(  voEVRCDec00000080) 
//#define fnLog10                                     RENAME(  voEVRCDec00000081) 
#define _fnLog2                                        RENAME(voEVRCDec00000082  )
#define _fndppf                                        RENAME(voEVRCDec00000083  )
#define _g_hEVRCDecInst                                RENAME(voEVRCDec00000084  )
#define _g_memOP                                       RENAME(voEVRCDec00000085  )
#define _getgain                                       RENAME(voEVRCDec00000086  )
#define _gnvq_4                                        RENAME(voEVRCDec00000087  )
#define _gnvq_8                                        RENAME(voEVRCDec00000088  )
#define _hamm_table                                    RENAME(voEVRCDec00000089  )
#define _iir                                           RENAME(voEVRCDec00000090  )
#define _iir_asm                                       RENAME(voEVRCDec00000091  )
//#define initialize_rate                             RENAME(  voEVRCDec00000092) 
//#define interpolation_cos129                        RENAME(  voEVRCDec00000093) 
#define _ipos                                          RENAME(voEVRCDec00000094  )
#define _lognsize22                                    RENAME(voEVRCDec00000095  )
#define _lognsize28                                    RENAME(voEVRCDec00000096  )
#define _lognsize8                                     RENAME(voEVRCDec00000097  )
#define _lpcanalys                                     RENAME(voEVRCDec00000098  )
#define _lsp2a                                         RENAME(voEVRCDec00000099  )
#define _lspmaq                                        RENAME(voEVRCDec00000100  )
#define _lspmaq1                                       RENAME(voEVRCDec00000101  )
#define _lspmaq_dec                                    RENAME(voEVRCDec00000102  )
#define _lsptab22                                      RENAME(voEVRCDec00000103  )
#define _lsptab28                                      RENAME(voEVRCDec00000104  )
#define _lsptab8                                       RENAME(voEVRCDec00000105  )
#define _maxeloc                                       RENAME(voEVRCDec00000106  )
#define _mod                                           RENAME(voEVRCDec00000107  )
#define _modifyorig                                    RENAME(voEVRCDec00000108  )
#define _noise_suprs                                   RENAME(voEVRCDec00000109  )
#define _nsize22                                       RENAME(voEVRCDec00000110  )
#define _nsize28                                       RENAME(voEVRCDec00000111  )
#define _nsize8                                        RENAME(voEVRCDec00000112  )
#define _nsub22                                        RENAME(voEVRCDec00000113  )
#define _nsub28                                        RENAME(voEVRCDec00000114  )
#define _nsub8                                         RENAME(voEVRCDec00000115  )
//#define pit_shrp                                    RENAME(  voEVRCDec00000116) 
#define _post_encode                                   RENAME(voEVRCDec00000117  )
#define _ppvq                                          RENAME(voEVRCDec00000118  )
#define _ppvq_mid                                      RENAME(voEVRCDec00000119  )
#define _pre_encode                                    RENAME(voEVRCDec00000120  )
#define _putacbc                                       RENAME(voEVRCDec00000121  )
//#define r_fft                                       RENAME(  voEVRCDec00000122) 
#define _ran0                                          RENAME(voEVRCDec00000123  )
#define _ran_g                                         RENAME(voEVRCDec00000124  )
//#define rnd_delay                                   RENAME(  voEVRCDec00000125) 
//#define select_rate                                 RENAME(  voEVRCDec00000126) 
#define _sin129_table                                  RENAME(voEVRCDec00000127  )
#define _sin_cosT                                      RENAME(voEVRCDec00000128  )
//#define sqroot                                      RENAME(  voEVRCDec00000129) 
#define _update_background                             RENAME(voEVRCDec00000130  )
#define _voEVRCCodecInv_sqrt                           RENAME(voEVRCDec00000131  )
#define _voEVRCCodec_cor_h_x                           RENAME(voEVRCDec00000132  )
//#define voEVRCCodec_mem_free                        RENAME(  voEVRCDec00000133) 
//#define voEVRCCodec_mem_malloc                      RENAME(  voEVRCDec00000134) 
#define _voEVRCDec_GetOutputData                       RENAME(voEVRCDec00000135  )
#define _voEVRCDec_GetParam                            RENAME(voEVRCDec00000136  )
#define _voEVRCDec_Init                                RENAME(voEVRCDec00000137  )
#define _voEVRCDec_SetInputData                        RENAME(voEVRCDec00000138  )
#define _voEVRCDec_SetParam                            RENAME(voEVRCDec00000139  )
#define _voEVRCDec_Uninit                              RENAME(voEVRCDec00000140  )
//#define voGetEVRCDecAPI                             RENAME(  voEVRCDec00000141) 
#define _w_long                                        RENAME(voEVRCDec00000142  )
#define _w_table                                       RENAME(voEVRCDec00000143  )
#define _weight                                        RENAME(voEVRCDec00000144  )
#define _wfac_ALPHA                                    RENAME(voEVRCDec00000145  )
#define _wfac_BETA                                     RENAME(voEVRCDec00000146  )
#define _wfac_GAMMA1                                   RENAME(voEVRCDec00000147  )
#define _wfac_GAMMA2                                   RENAME(voEVRCDec00000148  )
#define _wfac_Gamma_4                                  RENAME(voEVRCDec00000149  )

//#endif //#ifndef _DEBUG
#endif //#ifndef _WIN32_WCE
#endif //#ifndef __VOOGGDECID_H_