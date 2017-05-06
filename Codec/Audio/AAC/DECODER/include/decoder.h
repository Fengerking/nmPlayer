#ifndef _AACDECODER_H__
#define _AACDECODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "struct.h"

/* private implementation-specific functions */

/* decoder.c */
VO_U32 VO_API voAACDecInit(VO_HANDLE * phCodec,VO_AUDIO_CODINGTYPE vType, VO_CODEC_INIT_USERDATA * pUserData);
VO_U32 VO_API voAACDecSetInputData(VO_HANDLE hCodec, VO_CODECBUFFER * pInput);
VO_U32 VO_API voAACDecGetOutputData(VO_HANDLE hCodec, VO_CODECBUFFER * pOutput, VO_AUDIO_OUTPUTINFO *pOutInfo);
VO_U32 VO_API voAACDecSetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
VO_U32 VO_API voAACDecGetParam(VO_HANDLE hCodec, VO_S32 uParamID, VO_PTR pData);
VO_U32 VO_API voAACDecUninit(VO_HANDLE hCodec);

/* decframe.c */
int DecodeOneFrame(AACDecoder *decoder, VO_S16* outbuf);

/* unit.c*/
int updateSampleRate(AACDecoder *decoder,int sampleRate);
int updateProfile(AACDecoder *decoder,int profile);
int EnableDecodeCurrChannel(AACDecoder *decoder,int ch);
int EnableDecodeCurrSBRChannel(AACDecoder *decoder,int ch);
void UpdateSeletedChDecoded(AACDecoder *decoder,int channels);
void UpdateSeletedSBRChDecoded(AACDecoder *decoder,int channels);
int voSignedDivide(AACDecoder* decoder,int divisor,int dividend);
unsigned int voUnsignedDivide(AACDecoder* decoder,unsigned int divisor,unsigned int dividend);
int error(AACDecoder *decoder,char *text, int code);
void Channelconfig(AACDecoder *decoder);
int tns_analysis_filter(AACDecoder* decoder,ICS_Data *ics,TNS_Data* tns, int* spec);

int raw_data_block(AACDecoder* decoder);
int dequant_rescale(AACDecoder* decoder,int channels);
int mi_decode(AACDecoder* decoder,int channels);
int pns_decode(AACDecoder* decoder,int channels);
int ltp_decode(AACDecoder* decoder,int channels);
int tns_decode(AACDecoder* decoder,int channels);
int filter_bank(AACDecoder* decoder,short *outbuf);
int ltp_update(AACDecoder* decoder,int channels);
int ic_prediction(AACDecoder* decoder,int channels);

int voSBRExtData(AACDecoder *decoder, int chBase);
int DecodeSBRData(AACDecoder *decoder, int chBase, short *outbuf);

/* Header.c */
int program_config_element(BitStream *bs,program_config* pce);

/* huffman.c */
int DecodeHuffmanScalar(const signed short *huffTab, const HuffInfo *huffTabInfo, unsigned int bitBuf, signed int *val);

/* downMatrix.c */
int DownMixto2Chs(AACDecoder* decoder,int chans,short* outbuf);
int Selectto2Chs(AACDecoder* decoder,int chans,short* outbuf);
int Stereo2Mono(AACDecoder* decoder,short *outbuf,int sampleSize);
int Mono2Stereo(AACDecoder* decoder,short *outbuf,int sampleSize);
int PostChannelProcess(AACDecoder* decoder,short *outbuf,int sampleSize);

/* Header.c */
int ParseADIFHeader(AACDecoder* decoder, BitStream *bs);
int ParseADTSHeader(AACDecoder* decoder);
int program_config_element(BitStream *bs,program_config* pce);

/* latmheader.c */
int ParserLatm(AACDecoder*	decoder);
int ReadMUXConfig(AACDecoder*	decoder, BitStream *bs);

/* lc_syntax.c */
int ltp_data(AACDecoder* decoder,ICS_Data *ics,LTP_Data* ltp);

/* lc_imdct.c */
void BitReverse(int *inout, int tabidx);
void voIMDCT(int tabidx, int *coef, int gb);
void R4Core(int *x, int bg, int gp, int *wtab);
void voRadix4FFT(int tabidx, int *x);

/* lc_mdct.c */
void MDCT(int tabidx, int *coef);

void *voAACDecAlignedMalloc(VO_MEM_OPERATOR *voMemop, int size);
void  voAACDecAlignedFree(VO_MEM_OPERATOR *voMemop, void *alignedPt);
#define SafeAlignedFree(a) {voAACDecAlignedFree(voMemop, a);(a)=NULL;}

int spectral_data(AACDecoder* decoder,ICS_Data *ics,int ch);
int sbr_init(AACDecoder* decoder);
void sbr_free(AACDecoder* decoder);
void tns_data(AACDecoder* decoder,BitStream *bs, int window_sequence, int ch);


#ifdef __cplusplus
}
#endif

#endif//DECODER
