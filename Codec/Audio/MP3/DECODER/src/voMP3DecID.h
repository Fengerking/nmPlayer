#ifndef __VOOGGDECID_H_
#define __VOOGGDECID_H_

#ifndef _WIN32_WCE
#ifndef _DEBUG

#define RENAME(func)	_##func

#define AliasReduce                                   voMP3Dec00000000
#define _AliasReduce                                 _voMP3Dec00000000
#define IDCT9_D                                       voMP3Dec00000003 
#define _IDCT9_D                                     _voMP3Dec00000003 
#define csa_table                                     voMP3Dec00000079 
#define _csa_table                                   _voMP3Dec00000079 
#define dctIdx                                        voMP3Dec00000080 
#define _dctIdx                                      _voMP3Dec00000080 
#define dcttab                                        voMP3Dec00000081 
#define _dcttab                                      _voMP3Dec00000081 
#define dcttab_v6                                     voMP3Dec00000082 
#define fastWin36                                     voMP3Dec00000083 
#define g_memOP                                       voMP3Dec00000085 
#define huff_pair_table                               voMP3Dec00000086 
#define pHuff_quad_table                              voMP3Dec00000087 
#define polyCoef                                      voMP3Dec00000088 
#define voMP3DecBits_Crc                              voMP3Dec00000112 
#define voMP3DecBrTab                                 voMP3Dec00000113 
#define voMP3DecCalcBitsUsed                          voMP3Dec00000114 
#define voMP3DecDCT32                                 voMP3Dec00000115 
#define _voMP3DecDCT32                               _voMP3Dec00000115
#define voMP3DecGetBits                               voMP3Dec00000116 
#define voMP3DecGetOutputData                         voMP3Dec00000117 
#define voMP3DecGetParam                              voMP3Dec00000118 
#define voMP3DecHeaderDecode                          voMP3Dec00000119 
#define voMP3DecHeaderSync                            voMP3Dec00000120 
#define voMP3DecInit                                  voMP3Dec00000121 
#define voMP3DecInitBits                              voMP3Dec00000122 
#define voMP3DecLayerI                                voMP3Dec00000123 
#define voMP3DecLayerII                               voMP3Dec00000124 
#define voMP3DecLayerIII                              voMP3Dec00000125 
#define voMP3DecMem_free                              voMP3Dec00000126 
#define voMP3DecMem_malloc                            voMP3Dec00000127 
#define voMP3DecRqTab                                 voMP3Dec00000128 
#define voMP3DecSetInputData                          voMP3Dec00000129 
#define voMP3DecSetParam                              voMP3Dec00000130 
#define voMP3DecSkipBits                              voMP3Dec00000131 
#define voMP3DecSrTab                                 voMP3Dec00000132 
#define voMP3DecStreamInit                            voMP3Dec00000133 
#define voMP3DecSubbandFrame                          voMP3Dec00000134 
#define voMP3DecSynthMono                             voMP3Dec00000135 
#define _voMP3DecSynthMono                           _voMP3Dec00000135 
#define voMP3DecSynthStereo                           voMP3Dec00000136 
#define _voMP3DecSynthStereo                         _voMP3Dec00000136 
#define voMP3DecUninit                                voMP3Dec00000137 
#define voMP3Decidct9_2                               voMP3Dec00000138 
#define _voMP3Decidct9_2                             _voMP3Dec00000138 


#endif //#ifndef _DEBUG
#endif //#ifndef _WIN32_WCE
#endif //#ifndef __VOOGGDECID_H_