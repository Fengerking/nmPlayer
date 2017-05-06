#ifndef __VO_READBITS_H__
#define __VO_READBITS_H__

#include "voType.h"
#include "voVideoParserDecID.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	VO_U32 nBuf;
	VO_S32 nLen;
	const VO_U8 *pEnd;
	VO_S32 nPos;
} PARSER_BITSTREAM;


//Init bits
void ParserInitBits(PARSER_BITSTREAM *pBS, const VO_U8 *pBuf, VO_U32 nLen);

//Flush bits
//Note: nBits must be no more than 16
void  ParserFlushBits(PARSER_BITSTREAM *pBS, VO_U32 nBits);

//Show bits
//Return the value just show and stream will not flush automatically
//Note: nBits must be no more than 32
VO_U32 ParserShowBits(PARSER_BITSTREAM *pBS, VO_U32 nBits);

//Read bits
//Return the value just read and stream will flush automatically
//Note: nBits must be no more than 16
VO_U32 ParserReadBits(PARSER_BITSTREAM *pBS, VO_U32 nBits);

//End of bits
//Return zero if stream is not end, otherwise return non-zero
VO_U32 ParserEndOfBits(PARSER_BITSTREAM *pBS);

//Return the point of next byte 
const VO_U8 *ParserGetNextBytePos(PARSER_BITSTREAM *pBS);

//ue_v, reads an ue(v) syntax element
VO_S32 p_ue_v (PARSER_BITSTREAM *pBS);

//se_v, reads an se(v) syntax element
VO_S32 p_se_v (PARSER_BITSTREAM *pBS);

//u_v, reads an u(v) syntax element
VO_S32 p_u_v (int LenInBits, PARSER_BITSTREAM *pBS);

VO_S32 p_i_v (int LenInBits, PARSER_BITSTREAM *pBS);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
