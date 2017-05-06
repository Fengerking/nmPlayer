
/*!
 ************************************************************************
 * \file vlc.c
 *
 * \brief
 *    VLC support functions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Inge Lille-Langøy               <inge.lille-langoy@telenor.com>
 *    - Detlev Marpe                    <marpe@hhi.de>
 *    - Gabi Blaettermann               <blaetter@hhi.de>
 ************************************************************************
 */
#include "contributors.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "global.h"
#include "vlc.h"
#include "global.h"
#include "defines.h"
#include "loopFilter.h"
#include "global.h"
//#include "H264_C_Type.h"
#include "avd_neighbor.h"
#include "mbuffer.h"
#include "elements.h"
#include "errorconcealment.h"
#include "macroblock.h"
#include "fmo.h"
#include "cabac.h"
#include "vlc.h"
#include "image.h"
#include "mb_access.h"
#include "biaridecod.h"
#include "block.h"

extern const avdUInt8 SNGL_SCAN_TO_INDEX[16];
extern const avdUInt8 chromaScan[16];
extern const  	int T8x8KPos[4];
extern const avdUInt8 SNGL_SCAN8x8[64][2];
extern const	int T8x8Offset1[16];
extern const avdUInt8 avd_NCBP[2][48];
extern const avdUInt8 FIELD_SCAN_TO_INDEX[16];
extern const avdUInt8 FIELD_SCAN8x8[64][2];
// A little trick to avoid those horrible #if TRACE all over the source code
#if TRACE
#define SYMTRACESTRING(s) strncpy(sym->tracestring,s,TRACESTRING_SIZE)
#else
#define SYMTRACESTRING(s) // do nothing
#endif

extern void tracebits(const char *trace_str,  int len,  int info,int value1);


const avdUInt32 LsbOnes[33] = {0x0, 0x1, 0x3, 0x7, 
				   0xf, 0x1f, 0x3f, 0x7f,
				   0xff, 0x1ff, 0x3ff, 0x7ff,
				   0xfff, 0x1fff, 0x3fff, 0x7fff,
				   0xffff, 0x1ffff, 0x3ffff, 0x7ffff,
				   0xfffff, 0x1fffff, 0x3fffff, 0x7fffff,
				   0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff,
				   0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0x7fffffff};




/*!
 ************************************************************************
 * \brief
 *    Converts RBSP to string of data bits
 * \param streamBuffer
 *          pointer to buffer containing data
 *  \param last_byte_pos
 *          position of the last byte containing data.
 * \return last_byte_pos
 *          position of the last byte pos. If the last-byte was entirely a stuffing byte,
 *          it is removed, and the last_byte_pos is updated.
 *
************************************************************************/

int RBSPtoSODB(avdUInt8 *streamBuffer, int last_byte_pos)
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
      if(last_byte_pos == 0)
        AvdLog(LL_ERROR, "%s", "Panic: All zero data sequence in RBSP \n");
      --last_byte_pos;
      bitoffset = 0;
    }
    ctr_bit= streamBuffer[last_byte_pos - 1] & (0x01<<(bitoffset));
  }

  // We keep the stop bit for now
/*  if (remove_stop)
  {
    streamBuffer[last_byte_pos-1] -= (0x01<<(bitoffset));
    if(bitoffset == 7)
      return(last_byte_pos-1);
    else
      return(last_byte_pos);
  }
*/
  return(last_byte_pos);

}





void InitBitStream(Bitstream *str, avdUInt8 *buf, avdInt32 len)
{
	avdUInt32 tmp;
	avdNativeInt align4, i;
	str->streamBuffer = buf;
//	str->code_len = len;
    str->code_len = RBSPtoSODB(buf, len);
	str->ei_flag = 0;
#if (DUMP_VERSION & DUMP_SLICE)
	str->frame_bitoffset = 0;
#endif
	align4 = (avdNativeInt)(4 - ((avdUInt32)buf & 3));
#ifdef GETBITSMMX
	align4 += 4;
	str->buffer64 = 0;
	for (i = 0; i < align4; i++){
		str->buffer64 <<= 8;
		str->buffer64 |= buf[i];
	}
	str->buffer64 <<= ((8 - align4)<<3);
	str->currReadBuf = buf + align4;
#else //GETBITSMMX
	// make sure the memory is 4 aligned after first read;
	str->bufferA = 0;
	for (i = 0; i < align4; i++){
		str->bufferA <<= 8;
		str->bufferA |= buf[i];
	}

	buf += align4;
	tmp = *(avdUInt32*)buf;
	str->bufferB = VOI_BYTESWAP(tmp);
	str->currReadBuf = buf + 4;
#endif //GETBITSMMX
	str->bBitToGo = (align4<<3);

#if 0//(DUMP_VERSION & DUMP_BITS)
	img->currBitStream = str;
#endif //(DUMP_VERSION & DUMP_BITS)
}
/*! 
 *************************************************************************************
 * \brief
 *    avd_ue_v, reads an ue(v) syntax element, the length in bits is stored in 
 *    the global UsedBits variable
 *
 * \param tracestring
 *    the string for the trace file
 *
 * \param bitstream
 *    the stream to be read from
 *
 * \return
 *    the value of the coded syntax element
 *
 *************************************************************************************
 */
/*
avdNativeInt avd_ue_v (Bitstream *bitstream)
{
	return GetVLCSymbol_NEW (bitstream); 
}
*/

/*! 
 *************************************************************************************
 * \brief
 *    avd_ue_v, reads an se(v) syntax element, the length in bits is stored in 
 *    the global UsedBits variable
 *
 * \param tracestring
 *    the string for the trace file
 *
 * \param bitstream
 *    the stream to be read from
 *
 * \return
 *    the value of the coded syntax element
 *
 *************************************************************************************
 */
avdInt32 avd_se_v (Bitstream *bitstream)
{
	avdUInt32 n;
	avdInt32 value;
	n = GetVLCSymbol_NEW (bitstream);
	value = (n+1)>>1;
	if((n & 0x01)==0)                           // lsb is signed bit
		value = -value;
	return value;
}


/*! 
 *************************************************************************************
 * \brief
 *    avd_ue_v, reads an u(v) syntax element, the length in bits is stored in 
 *    the global UsedBits variable
 *
 * \param LenInBits
 *    length of the syntax element
 *
 * \param tracestring
 *    the string for the trace file
 *
 * \param bitstream
 *    the stream to be read from
 *
 * \return
 *    the value of the coded syntax element
 *
 *************************************************************************************
 */
/*
avdUInt32 avd_u_v (avdNativeInt LenInBits, Bitstream *bitstream)
{
	return (avdUInt32)GetBits(bitstream, LenInBits);
};
*/
                
/*! 
 *************************************************************************************
 * \brief
 *    avd_ue_v, reads an u(1) syntax element, the length in bits is stored in 
 *    the global UsedBits variable
 *
 * \param tracestring
 *    the string for the trace file
 *
 * \param bitstream
 *    the stream to be read from
 *
 * \return
 *    the value of the coded syntax element
 *
 *************************************************************************************
 */
/*
avdNativeUInt avd_u_1 (Bitstream *bitstream)
{
	return (avdNativeUInt)avd_u_v (1, bitstream);
}
*/
void readVLCSynElement_Ue(SyntaxElement *sym, Bitstream *currStream)
{
	sym->value1 = GetVLCSymbol_NEW (currStream); 
}
#if 1
#define CHECK_BITSTREAM_OVERFLOW(currStream,info) \
	if(currStream->currReadBuf-currStream->streamBuffer-(currStream->bBitToGo>>3) - 4>currStream->code_len)\
		AVD_ERROR_CHECK2(img,info,ERROR_InvalidBitstream);
#else
#define CHECK_BITSTREAM_OVERFLOW(currStream,info)
#endif
void readVLCSynElement_Se(ImageParameters *img,SyntaxElement *sym, Bitstream *currStream)
{
	avdInt32 n;
	avdInt32 value1;
	n = GetVLCSymbol_NEW (currStream); 
	value1 = (n+1)>>1;
	sym->value1 = (n&1) ? value1 : -value1;
	CHECK_BITSTREAM_OVERFLOW(currStream,"overflow the bitstream buf\n");//check
}
#if FEATURE_BFRAME
int more_rbsp_data (avdUInt8* buffer,int totbitoffset,int bytecount)
{
	long byteoffset = (totbitoffset >> 3);      // byte from start of buffer
	// there is more until we're in the last byte
	if (byteoffset < (bytecount - 1)) 
		return 1;
	else
	{
		int bitoffset   = (7 - (totbitoffset & 0x07));      // bit from start of byte
		avdUInt8 *cur_byte  = &(buffer[byteoffset]);
		// read one bit
		int ctr_bit     = ctr_bit = ((*cur_byte)>> (bitoffset--)) & 0x01;      // control bit for current bit posision

		//assert (byteoffset<bytecount);       

		// a stop bit has to be one
		if (ctr_bit==0) 
			return 1;  
		else
		{
			int cnt = 0;

			while (bitoffset>=0 && !cnt)
			{
				cnt |= ((*cur_byte)>> (bitoffset--)) & 0x01;   // set up control bit
			}

			return (cnt);
		}
	}
}
#endif//FEATURE_BFRAME
/*!
 ************************************************************************
 * \brief
 *    Check if there are symbols for the next MB
 ************************************************************************
 */
avdNativeInt uvlc_startcode_follows(ImageParameters *img,TMBsProcessor* info,avdNativeInt dummy)
{

	Bitstream   *currStream = img->currentSlice->partArr[0].bitstream;

	avdUInt8 numLastBits;
	int totalUsed = currStream->currReadBuf - currStream->streamBuffer - 
		(currStream->bBitToGo>>3) - 4;
	if (currStream->bBitToGo & 7)
		totalUsed--;

	if (totalUsed < currStream->code_len - 1)
		return VOI_FALSE;

	if (!ShowBits(currStream, 1)) // a stop bit has to be one
		return VOI_FALSE;

	// first bit is 1;
	numLastBits = (currStream->bBitToGo&7); // bits in last avdUInt8;
	if (!numLastBits)
		numLastBits = 8; // next byte;
	return (ShowBits(currStream, numLastBits) == (1<<(numLastBits-1)));// all 0s, except first bit is 1;
}

void FlushBits(Bitstream *bStr, avdNativeInt numBits)
{
	bStr->bBitToGo -= numBits;
#if (DUMP_VERSION & DUMP_SLICE)
	bStr->frame_bitoffset += numBits;
#endif//#if (DUMP_VERSION & DUMP_SLICE)
	if (bStr->bBitToGo < 0){
		avdUInt32 tmp = *(avdInt32*)bStr->currReadBuf;
		bStr->bufferA = bStr->bufferB;
		bStr->bufferB = VOI_BYTESWAP(tmp);
		bStr->currReadBuf += 4;
		bStr->bBitToGo += 32;
	}
}

avdUInt32 ShowBits(Bitstream *bStr, avdNativeInt numBits)
{
	int extraBits;
	const avdUInt32 lsbOnes = LsbOnes[numBits];
	extraBits = bStr->bBitToGo - numBits;
	if (extraBits >= 0){
		return (bStr->bufferA>>(extraBits))&lsbOnes;
	}
	return ((bStr->bufferA<<(-extraBits)) | (bStr->bufferB>>(32+extraBits)))
		&lsbOnes;
}

avdUInt32 GetBits(Bitstream *bStr, avdNativeInt numBits)
{
	// Show bits plus Flush bits;
	avdUInt32 tmp, value;
	const avdUInt32 lsbOnes = LsbOnes[numBits];
	StartRunTimeClock(DT_GETBITS);	
	bStr->bBitToGo -= numBits;
#if (DUMP_VERSION & DUMP_SLICE)
	bStr->frame_bitoffset += numBits;
#endif//#if (DUMP_VERSION & DUMP_SLICE)
	if (bStr->bBitToGo >= 0){
		EndRunTimeClock(DT_GETBITS);	
		return (bStr->bufferA>>bStr->bBitToGo)&lsbOnes;
	}

	bStr->bBitToGo += 32;
	value= (((avdUInt32)(((avdUInt64)bStr->bufferA<<(32 - bStr->bBitToGo))) | (bStr->bufferB>>bStr->bBitToGo)) 
		& lsbOnes);
	tmp = *(avdInt32*)bStr->currReadBuf;
	bStr->bufferA = bStr->bufferB;
	bStr->bufferB = VOI_BYTESWAP(tmp);
	bStr->currReadBuf += 4;
	EndRunTimeClock(DT_GETBITS);	
	return value;
}


#if 1//inline the VLCIP code for test
#include "vlc.h"
//#include "H264_C_Type.h"
#include "avd_neighbor.h"

#if PROTECT_NAME
//#include "ncNameTab_Core.h"
#endif

// old VLC method is to reduced 2-D search to 1-D search; a pure table look-up required a huge table;
// new VLC method uses the property of table number, and try to eliminate 1-D search;
#define AVD_NEW_VLC_METHOD

/********************************************************************
*		IP-2: New VCL code books									*
********************************************************************/



static const avdUInt8 VLC_TAB1[4] =
{0xff, 1, 0, 2}; // NumCoeff = Trailing Ones;
static const avdUInt8 VLC_TAB2[12] = 
{4, 3, 2, 3, 2, 1, //NumCoeff;
0, 0, 0, 3, 1, 0}; //Trailing Ones;

#if 0 //old tables for reference;
int lentab[3][4][17] = 
{
	{   // 0702
		{ 1, 6, 8, 9,10,11,13,13,13,14,14,15,15,16,16,16,16},
		{ 0, 2, 6, 8, 9,10,11,13,13,14,14,15,15,15,16,16,16},
		{ 0, 0, 3, 7, 8, 9,10,11,13,13,14,14,15,15,16,16,16},
		{ 0, 0, 0, 5, 6, 7, 8, 9,10,11,13,14,14,15,15,16,16},
	},                                                 
	{                                                  
		{ 2, 6, 6, 7, 8, 8, 9,11,11,12,12,12,13,13,13,14,14},
		{ 0, 2, 5, 6, 6, 7, 8, 9,11,11,12,12,13,13,14,14,14},
		{ 0, 0, 3, 6, 6, 7, 8, 9,11,11,12,12,13,13,13,14,14},
		{ 0, 0, 0, 4, 4, 5, 6, 6, 7, 9,11,11,12,13,13,13,14},
		},                                                 
		{                                                  
			{ 4, 6, 6, 6, 7, 7, 7, 7, 8, 8, 9, 9, 9,10,10,10,10},
			{ 0, 4, 5, 5, 5, 5, 6, 6, 7, 8, 8, 9, 9, 9,10,10,10},
			{ 0, 0, 4, 5, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9,10,10,10},
			{ 0, 0, 0, 4, 4, 4, 4, 4, 5, 6, 7, 8, 8, 9,10,10,10},
		},

};
int codtab[3][4][17] = 
{
	{
		{ 1, 5, 7, 7, 7, 7,15,11, 8,15,11,15,11,15,11, 7,4}, 
		{ 0, 1, 4, 6, 6, 6, 6,14,10,14,10,14,10, 1,14,10,6}, 
		{ 0, 0, 1, 5, 5, 5, 5, 5,13, 9,13, 9,13, 9,13, 9,5}, 
		{ 0, 0, 0, 3, 3, 4, 4, 4, 4, 4,12,12, 8,12, 8,12,8},
	},
	{
		{ 3,11, 7, 7, 7, 4, 7,15,11,15,11, 8,15,11, 7, 9,7}, 
		{ 0, 2, 7,10, 6, 6, 6, 6,14,10,14,10,14,10,11, 8,6}, 
		{ 0, 0, 3, 9, 5, 5, 5, 5,13, 9,13, 9,13, 9, 6,10,5}, 
		{ 0, 0, 0, 5, 4, 6, 8, 4, 4, 4,12, 8,12,12, 8, 1,4},
		},
		{
			{15,15,11, 8,15,11, 9, 8,15,11,15,11, 8,13, 9, 5,1}, 
			{ 0,14,15,12,10, 8,14,10,14,14,10,14,10, 7,12, 8,4},
			{ 0, 0,13,14,11, 9,13, 9,13,10,13, 9,13, 9,11, 7,3},
			{ 0, 0, 0,12,11,10, 9, 8,13,12,12,12, 8,12,10, 6,2},
		},
};
#endif //0
// for extra bits less than 2; first 2 bits are TrailingOnes, third bit is the flag
// to deduct one bit from extra bit, and the last 5 bits are Total Coeff;
static const avdUInt8 VLC_TAB3[16] = {
	(0<<3)+0,(0<<3)+0,(1<<3)+1,(1<<3)+1,(2<<3)+2,(2<<3)+2,(13<<3)+1,(13<<3)+1,
	(15<<3)+3,(15<<3)+3,(16<<3)+0,(16<<3)+0,(1<<3)+1,(0<<3)+0,(16<<3)+3,(16<<3)+2};

	// for extra bits less than 2; first 2 bits are TrailingOnes, third bit is the flag
	// to deduct one bit from extra bit, and the last 5 bits are Total Coeff;
#define EB_MINUS1_IDX		(1<<2)
static const avdUInt8 VLC_TAB4[4*14] = {
	(2<<3)+1,(1<<3)+0,(3<<3)+3+EB_MINUS1_IDX,(3<<3)+3+EB_MINUS1_IDX,(5<<3)+3,(3<<3)+2,(4<<3)+3+EB_MINUS1_IDX,(4<<3)+3+EB_MINUS1_IDX,
	(6<<3)+3,(4<<3)+2,(3<<3)+1,(2<<3)+0,(7<<3)+3,(5<<3)+2,(4<<3)+1,(3<<3)+0,
	(8<<3)+3,(6<<3)+2,(5<<3)+1,(4<<3)+0,(9<<3)+3,(7<<3)+2,(6<<3)+1,(5<<3)+0,
	(16<<3)+0,(16<<3)+2,(16<<3)+1,(15<<3)+0,(4<<3)+3,(3<<3)+3,(2<<3)+2+EB_MINUS1_IDX,(2<<3)+2+EB_MINUS1_IDX,
	(7<<3)+3,(4<<3)+2,(4<<3)+1,(2<<3)+0,(8<<3)+3,(5<<3)+2,(5<<3)+1,(3<<3)+0,
	(5<<3)+0,(6<<3)+2,(6<<3)+1,(4<<3)+0,(9<<3)+3,(7<<3)+2,(7<<3)+1,(6<<3)+0,
	(16<<3)+3,(16<<3)+2,(16<<3)+1,(16<<3)+0,(16<<3)+1,(15<<3)+0,(15<<3)+3,(15<<3)+2};

static const avdUInt8 VLC_TAB5[16*8] = {
	(8<<3)+0,(9<<3)+2,(8<<3)+1,(7<<3)+0,(10<<3)+3,(8<<3)+2,(7<<3)+1,(6<<3)+0, //0
	(12<<3)+3,(11<<3)+2,(10<<3)+1,(10<<3)+0,(11<<3)+3,(10<<3)+2,(9<<3)+1,(9<<3)+0, //1
	(14<<3)+3,(13<<3)+2,(12<<3)+1,(12<<3)+0,(13<<3)+3,(12<<3)+2,(11<<3)+1,(11<<3)+0, //2
	(16<<3)+3,(15<<3)+2,(15<<3)+1,(14<<3)+0,(15<<3)+3,(14<<3)+2,(14<<3)+1,(13<<3)+0, //3
	(6<<3)+3,(3<<3)+2,(3<<3)+1,(1<<3)+0,(5<<3)+3+EB_MINUS1_IDX,(5<<3)+3+EB_MINUS1_IDX,(2<<3)+1+EB_MINUS1_IDX,(2<<3)+1+EB_MINUS1_IDX, //4
	(11<<3)+3,(9<<3)+2,(9<<3)+1,(8<<3)+0,(10<<3)+3,(8<<3)+2,(8<<3)+1,(7<<3)+0, //5
	(11<<3)+0,(11<<3)+2,(11<<3)+1,(10<<3)+0,(12<<3)+3,(10<<3)+2,(10<<3)+1,(9<<3)+0, //6
	(14<<3)+3,(13<<3)+2,(13<<3)+1,(13<<3)+0,(13<<3)+3,(12<<3)+2,(12<<3)+1,(12<<3)+0, //7
	(15<<3)+1,(15<<3)+0,(15<<3)+2,(14<<3)+1,(14<<3)+2+EB_MINUS1_IDX,(14<<3)+2+EB_MINUS1_IDX,(14<<3)+0+EB_MINUS1_IDX,(14<<3)+0+EB_MINUS1_IDX, //8
	(7<<3)+3,(6<<3)+3,(5<<3)+3,(4<<3)+3,(3<<3)+3,(2<<3)+2,(1<<3)+1,(0<<3)+0, //9
	(5<<3)+1,(5<<3)+2,(4<<3)+1,(4<<3)+2,(3<<3)+1,(8<<3)+3,(3<<3)+2,(2<<3)+1, //10
	(3<<3)+0,(7<<3)+2,(7<<3)+1,(2<<3)+0,(9<<3)+3,(6<<3)+2,(6<<3)+1,(1<<3)+0, //11
	(7<<3)+0,(6<<3)+0,(9<<3)+2,(5<<3)+0,(10<<3)+3,(8<<3)+2,(8<<3)+1,(4<<3)+0, //12
	(12<<3)+3,(11<<3)+2,(10<<3)+1,(9<<3)+0,(11<<3)+3,(10<<3)+2,(9<<3)+1,(8<<3)+0, //13
	(12<<3)+0,(13<<3)+2,(12<<3)+1,(11<<3)+0,(13<<3)+3,(12<<3)+2,(11<<3)+1,(10<<3)+0, //14
	(15<<3)+1,(14<<3)+0,(14<<3)+3,(14<<3)+2,(14<<3)+1,(13<<3)+0,(13<<3)+1+EB_MINUS1_IDX,(13<<3)+1+EB_MINUS1_IDX}; //15

// first bits 0-1 show how many extra bit needed, bits 2-7 show the offset of table; 
// For extra bits <= 2, use the table index as offset.
// For extra bits = 3, use (table index)>>3 as offset;
// index of the table is based on (VLC_TAB11[]<<4) + NumLeadingZeros;
static const avdUInt8 VLC_TAB6[44] = {
	//VLC_TAB11[] = 0; 15 valid numbers;
	(0<<2)+0,(2<<2)+0,(4<<2)+0,(0<<2)+2,(4<<2)+2,(8<<2)+2,(12<<2)+2,(16<<2)+2,
	(20<<2)+2,(0<<2)+3,(1<<2)+3,(2<<2)+3,(3<<2)+3,(24<<2)+2,(6<<2)+0,0xff,
	//VLC_TAB11[] = 1; 13 valid numbers;
	(12<<2)+1,(28<<2)+2,(4<<2)+3,(32<<2)+2,(36<<2)+2,(40<<2)+2,(44<<2)+2,(5<<2)+3,
	(6<<2)+3,(7<<2)+3,(8<<2)+3,(48<<2)+2,(8<<2)+0,0xff,0xff,0xff,
	//VLC_TAB11[] = 2; 10 valid numbers;
	(9<<2)+3,(10<<2)+3,(11<<2)+3,(12<<2)+3,(13<<2)+3,(14<<2)+3,(15<<2)+3,(52<<2)+2,
	(14<<2)+1,(10<<2)+0,0xff,0xff};

// the first 3 bits are not zeros;
// for those 8 number needs the 4th bit, save them at [(ShowBits(3)<<2 + vlcNum)<<3] as shown
// in 0X..;
static const avdUInt8 VLC_TAB7[8*14] = { // [(vlcnum<<3)+ShowBits(3)]
	0xff, 0x48, 0x46, 0x34, 0x33, 0x32, 0x31, 0x30,
	0xff, 0x48, 0x44, 0x37, 0x36, 0x33, 0x32, 0x31,
	0xff, 0x49, 0x43, 0x38, 0x36, 0x35, 0x34, 0x31,
	0xff, 0x48, 0x41, 0x37, 0x36, 0x35, 0x34, 0x33,
	0X47, 0x39, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32,
	0X45, 0x38, 0x36, 0x34, 0x33, 0x32, 0x25, 0x25, 
	0X47, 0x37, 0x36, 0x33, 0x25, 0x25, 0x24, 0x24,
	0X42, 0x35, 0x26, 0x26, 0x24, 0x24, 0x23, 0x23,
	0X45, 0x32, 0x25, 0x25, 0x24, 0x24, 0x23, 0x23,
	0X40, 0x32, 0x33, 0x35, 0x14, 0x14, 0x14, 0x14,
	0X42, 0x34, 0x22, 0x22, 0x13, 0x13, 0x13, 0x13,
	0X40, 0x31, 0x23, 0x23, 0x12, 0x12, 0x12, 0x12,
	0xff, 0x20, 0x21, 0x21, 0x12, 0x12, 0x12, 0x12,
	0xff, 0x10, 0x10, 0x10, 0x11, 0x11, 0x11, 0x11};

// the first 3 bits are zeros;
static const avdUInt8 VLC_TAB8[8*11] = { // [(vlcnum<<3)+ShowBitsInline(6)]
	0x6e, 0x6d, 0x6c, 0x6b, 0x5a, 0x5a, 0x59, 0x59,
	0x6d, 0x6b, 0x5c, 0x5c, 0x5a, 0x5a, 0x59, 0x59,
	0x5c, 0x5c, 0x5b, 0x5b, 0x5a, 0x5a, 0x50, 0x50,
	0x5b, 0x5b, 0x59, 0x59, 0x4a, 0x4a, 0x4a, 0x4a,
	0x6a, 0x60, 0x51, 0x51, 0x48, 0x48, 0x48, 0x48,
	0x69, 0x60, 0x51, 0x51, 0x47, 0x47, 0x47, 0x47,
	0x68, 0x60, 0x52, 0x52, 0x41, 0x41, 0x41, 0x41,
	0x61, 0x60, 0x57, 0x57, 0x42, 0x42, 0x42, 0x42,
	0x51, 0x51, 0x50, 0x50, 0x46, 0x46, 0x46, 0x46,
	0x40, 0x40, 0x40, 0x40, 0x41, 0x41, 0x41, 0x41,
	0x40, 0x40, 0x40, 0x40, 0x41, 0x41, 0x41, 0x41};





// first 4 bits = length; last 4 bits = code;
static const avdUInt8 VLC_TAB9[24] = { //[(vlcNum<<3)+ShowBit(3)]
	0x33, 0x32, 0x21, 0x21, 0x10, 0x10, 0x10, 0x10,
	0x22, 0x22, 0x21, 0x21, 0x10, 0x10, 0x10, 0x10,
	0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0x10};





// first 4 bits = length; last 4 bits = code;
static const avdUInt8 VLC_TAB10[48] = { //[(vlcNum<<3)+ShowBit(3)][CODE/LENGTH]
	// VLC0
	0x11, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0x10,
	// VLC1
	0x22, 0x22, 0x21, 0x21, 0x10, 0x10, 0x10, 0x10,
	// VLC2
	0x23, 0x23, 0x22, 0x22, 0x21, 0x21, 0x20, 0x20,
	// VLC3
	0x34, 0x33, 0x22, 0x22, 0x21, 0x21, 0x20, 0x20,
	// VLC4
	0x35, 0x34, 0x33, 0x32, 0x21, 0x21, 0x20, 0x20,
	// VLC5
	0x31, 0x32, 0x34, 0x33, 0x36, 0x35, 0x20, 0x20};



static const avdNativeInt VLC_TAB12[8] = {0,3,6,12,24,48,32768};    // maximum vlc = 6
//static const avdUInt8 ClipRunBefore[TOTRUN_NUM+1] = {0,0,1,2,3,4,5,6,6,6,6,6,6,6,6,6};
static const avdUInt8 VLC_TAB11[8] = {0, 0, 1, 1, 2, 2, 2, 2};

static __inline void FlushBitsInline(Bitstream *bStr, avdNativeInt numBits)
{
#if (DUMP_VERSION & DUMP_SLICE)
	bStr->frame_bitoffset += numBits;
#endif//#if (DUMP_VERSION & DUMP_SLICE)
    
	bStr->bBitToGo -= numBits;
	if (bStr->bBitToGo < 0){
		avdUInt32 tmp = *(avdInt32*)bStr->currReadBuf;
		bStr->bufferA = bStr->bufferB;
		bStr->bufferB = VOI_BYTESWAP(tmp);
		bStr->currReadBuf += 4;
		bStr->bBitToGo += 32;
	}

}

static __inline avdUInt32 ShowBitsInline(Bitstream *bStr, avdNativeInt numBits)
{

	const avdUInt32 lsbOnes = LsbOnes[numBits];
	avdNativeInt extraBits = bStr->bBitToGo - numBits;
	if (bStr->bBitToGo >= numBits)
		//if (extraBits >= 0), Intel compiler generated error code;
		return ((bStr->bufferA>>extraBits)&lsbOnes);

	return (((bStr->bufferA<<(-extraBits)) | (bStr->bufferB>>(32+extraBits)))
		&lsbOnes);

}

static __inline avdUInt32 GetBitsInline(Bitstream *bStr, avdNativeInt numBits)
{


	avdUInt32 tmp, value;
	const avdUInt32 lsbOnes = LsbOnes[numBits];
#if (DUMP_VERSION & DUMP_SLICE)
	bStr->frame_bitoffset += numBits;
#endif//#if (DUMP_VERSION & DUMP_SLICE)
	bStr->bBitToGo -= numBits;
	if (bStr->bBitToGo >= 0)
		return (bStr->bufferA>>bStr->bBitToGo)&lsbOnes;

	bStr->bBitToGo += 32;
	value = ((bStr->bufferA<<(32 - bStr->bBitToGo)) | (bStr->bufferB>>bStr->bBitToGo))
		&lsbOnes;
	tmp = *(avdInt32*)bStr->currReadBuf;
	bStr->bufferA = bStr->bufferB;
	bStr->bufferB = VOI_BYTESWAP(tmp);
	bStr->currReadBuf += 4;
	return value;

}

/********************************************************************
*		IP-1: multiple steps VLC table lookup						*
********************************************************************/

// multiple steps VLC table lookup;
__inline void PeekBitStreamLeadingOne(Bitstream *str, avdNativeInt *numBits)
{
	avdNativeUInt bit16; //16bits value for table lookup;
	bit16 = ShowBitsInline(str, 16);
	*numBits = avdNumLeadingZerosPlus1[bit16>>8]; // leading one bits(include one);
	if (*numBits == 9){ // not find 1;
		*numBits = avdNumLeadingZerosPlus1[bit16 & 0xff] + 8; // leading one bits(include one);
	}
}

__inline void GetBitStreamLeadingOne(Bitstream *str, avdNativeInt *numBits)
{
	PeekBitStreamLeadingOne(str, numBits);
	FlushBitsInline(str, *numBits); // flush out all 0s and leading 1;
	--*numBits; // returned bit num does not include leading 1;
}

avdNativeUInt GetVLCSymbol_NEW (Bitstream *str)
{
#if 1// slower, 31 may be too big;
	avdNativeUInt bit31, numBits, totalBits; //16bits value for table lookup;
	bit31 = ShowBitsInline(str, 31);
	numBits = avdNumLeadingZerosPlus1[bit31>>23] - 1; // leading one bits(not include one);
	if (numBits & 0x8){ // same as numBits = 8; not find 1;
		numBits = avdNumLeadingZerosPlus1[(bit31>>15) & 0xff] + 7; // leading one bits(not include one);
	}
	
	totalBits = (numBits<<1) + 1;
	FlushBitsInline(str, totalBits);
	return ((bit31>>(31 - totalBits))) - 1;
#else
	avdNativeInt numBits;
	GetBitStreamLeadingOne(str, &numBits); // find how many 0s before leading one;
	return GetBitsInline(str, numBits) + LsbOnes[numBits]; // return code len including prefix and value;
#endif
}



/*!
************************************************************************
* \brief
*    Reads coeff of an 4x4 block (CAVLC)
*
************************************************************************
*/
avdNativeInt readCoeff4x4_CAVLC (ImageParameters *img,TMBsProcessor *info, avdNativeUInt codingBlockIdx, avdNativeUInt block_type, 
								 avdNativeInt j, avdNativeInt *levarr)
{
	//SyntaxElement currSE;
	//TMBsProcessor *info  = img->mbsProcessor;
	MacroBlock   *currMBInfo = info->currMB;
	MacroBlock	*mbAInfo, *mbBInfo;
	//AVCNeighbor4x4Boundary *mbBdryInfo;
	avdNativeInt iOff, jOff, tmpI, tmpJ, chromaOffset, cdc, b8x8Idx, a8x8Idx, 
		bNumNonZeros, aNumNonZeros;
	avdNativeInt i, k, code, level_two_or_higher, vlcnum, bitShift;
	avdNativeInt level, zerosleft, max_coeff_num, nnz, numcoeff, numtrailingones;
	avdUInt32    bitCache; //31bits cache;
	Bitstream   *currStream;
	avdNativeInt startLen, endLen, sign;
	TMBBitStream	*mbBits = GetMBBits(info->currMB);
	int					leftMBYForIntra = -1;
	// notice in core lib readCoeff4x4_CAVLC, it assumes
	// runarr = levarr + 16 to save a function argument;
	avdNativeInt *runarr = levarr + 16; 
#ifndef AVD_NEW_VLC_METHOD
	const avdUInt8  *codeTab8, *offsetTab, *valTab;
#endif //AVD_NEW_VLC_METHOD



	StartRunTimeClock(DT_READCAVLC4x4);
	i = (j&3);
	j >>= 2;
	cdc = a8x8Idx = b8x8Idx = 0;
	switch (block_type)
	{
	case LUMA:
	case LUMA_INTRA16x16AC:
		max_coeff_num = (block_type == LUMA) ? 16 : 15;
		// Get neighborB
		if (j){
			// (j - 1, i) at currMB 
			mbBInfo = currMBInfo;
			tmpI = inv_decode_scan[((j-1)<<2) | i];
			if (!niIsCavlcSkippedOrLumaAllACZeros(currMBInfo, tmpI>>2))
				b8x8Idx = info->mbsParser->numRuns[tmpI + COEFF_LUMA_START_IDX];
		}
		else {
			mbBInfo = GetUpMB(info);
			if (mbBInfo && !niIsCavlcSkippedOrLumaAllACZeros(mbBInfo, 
				inv_decode_scan[12+i]>>2))
				b8x8Idx = niGetCavlcNumNonZerosLuma(mbBInfo, i);
		}

		// Get neighborA
		if (i){
			mbAInfo = currMBInfo;
			tmpI = inv_decode_scan[(j<<2) | (i - 1)];
			if (!niIsCavlcSkippedOrLumaAllACZeros(mbAInfo, tmpI>>2))
				a8x8Idx = info->mbsParser->numRuns[tmpI + COEFF_LUMA_START_IDX];
		}
		else {
			leftMBYForIntra = j*4;
			mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,leftMBYForIntra,16,&leftMBYForIntra):GetLeftMB(info);
			leftMBYForIntra >>=2;
			if (mbAInfo && !niIsCavlcSkippedOrLumaAllACZeros(mbAInfo, 
				inv_decode_scan[(leftMBYForIntra<<2) + 3]>>2))
				a8x8Idx = niGetCavlcNumNonZerosLumaLeft(mbAInfo, leftMBYForIntra);
		}
		break;
	case CHROMA_AC:
		max_coeff_num = 15;
		// Get neighborB
		if (j&1){
			mbBInfo = currMBInfo;
			if (!niIsCavlcSkippedOrChromaAllACZeros(mbBInfo))
				b8x8Idx = info->mbsParser->numRuns[codingBlockIdx - 2];
		}
		else {
			mbBInfo = GetUpMB(info);
			if (mbBInfo && !niIsCavlcSkippedOrChromaAllACZeros(mbBInfo)){
				// map blockIdx (0, 1) to (0, 1) for U, and (2, 3) for V;
				b8x8Idx = niGetCavlcNumNonZerosChroma(mbBInfo, i);
			}
		}
		// Get neighborA
		if (i&1){
			mbAInfo = currMBInfo;
			if (!niIsCavlcSkippedOrChromaAllACZeros(mbAInfo))
				a8x8Idx = info->mbsParser->numRuns[codingBlockIdx - 1];
		}
		else {
			leftMBYForIntra = (j-4)*4;
			mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,leftMBYForIntra,8,&leftMBYForIntra):GetLeftMB(info);
			leftMBYForIntra >>= 2;
			if (mbAInfo && !niIsCavlcSkippedOrChromaAllACZeros(mbAInfo)){
				// map blockIdx (1, 3) to (0, 1) for U, (2, 3) for V;
				a8x8Idx = niGetCavlcNumNonZerosChromaLeft(mbAInfo, leftMBYForIntra + i);
			}
		}
		break;
	case LUMA_INTRA16x16DC:
		max_coeff_num = 16;
		// treat codingBlockIdx as COEFF_LUMA_START_IDX, at (0, 0);
		mbBInfo = GetUpMB(info);
		if (mbBInfo && !niIsCavlcSkippedOrLumaAllACZeros(mbBInfo, 2))
			b8x8Idx = niGetCavlcNumNonZerosLuma(mbBInfo, 0);
		leftMBYForIntra = 0;
		mbAInfo = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,leftMBYForIntra,16,&leftMBYForIntra):GetLeftMB(info);
		leftMBYForIntra >>=2;
		if (mbAInfo && !niIsCavlcSkippedOrLumaAllACZeros(mbAInfo, 1))
			a8x8Idx = niGetCavlcNumNonZerosLumaLeft(mbAInfo, leftMBYForIntra);

		break;
	case CHROMA_DC:
		max_coeff_num = 4;
		cdc = 1;
		break;
#ifdef _DEBUG
	default:
		//error (img, "readCoeff4x4_CAVLC: invalid block type", 600);
		break;
#endif
	}
	currStream = img->currentSlice->partArr[0].bitstream;


	bitCache = ShowBitsInline(currStream, 31); // 16 bits leading one + 3;
	// at most 19 bits for number trailing one;
	if (!cdc)
	{

		nnz = a8x8Idx + b8x8Idx;
		if (mbBInfo && mbAInfo)
			nnz = ((nnz + 1)>>1);

		if (nnz < 8){
			
			avdNativeUInt numBits, tOffset;
			// max 19 bits used;
			// Get the number of leading zeros to determine how many more
			// bits to read.
			k = avdNumLeadingZerosPlus1[bitCache>>23]; // leading one bits(not include one);
			if (k == 9){ // not find 1;
				k = avdNumLeadingZerosPlus1[(bitCache>>15) & 0xff] + 8; // leading one bits(not include one);
			}
			numcoeff = VLC_TAB11[nnz];
			tmpI = VLC_TAB6[(numcoeff<<4) + k - 1];
			numBits = (tmpI & 0x3);
			k += numBits;
			code = (bitCache>>(31 - k));
			tOffset = (tmpI>>2);
			if (numBits < 2){
				code = VLC_TAB3[tOffset + (code&0x1)];
			}
			else {
				code = (numBits == 2) ? VLC_TAB4[tOffset + (code&0x3)]
				: VLC_TAB5[(tOffset<<3) + (code&0x7)];
				if (code & EB_MINUS1_IDX) // minus one bit;
					k--;
			}
			numcoeff = (code>>3);
			numtrailingones = (code&3);


		}
		else{
			// read 6 bit FLC
			code = (bitCache>>25)&0x3f;
			if (code != 3){
				numcoeff = (code >> 2) + 1;
				numtrailingones = code & 3;
			}
			else {
				// #c = 0, #t1 = 3 =>  #c = 0
				numcoeff = 0;
#if (DUMP_VERSION & DUMP_SLICE)
				numtrailingones = 0;
#endif // (DUMP_VERSION & DUMP_SLICE)
			}
			k = 6;
		}
		////////////////////// end read number of trailing ones;

#if (DUMP_VERSION & DUMP_SLICE)
		//AvdLog(DUMP_SLICE, "\nnnzPred(%d), numTrailOnes(%d), nzCoeff(%d)", nnz, numtrailingones, numcoeff);
#endif // (DUMP_VERSION & DUMP_SLICE)
	}
	else
	{
		
		// max 17 bits used;
		k = avdNumLeadingZerosPlus1[bitCache>>23]; // leading one bits(include one);
		if (k == 9) // not find 1;
			k = avdNumLeadingZerosPlus1[(bitCache>>15) & 0xff] + 8; // leading one bits(include one);

		switch (k){
		case 1:
		case 2:
		case 3:
			numcoeff = numtrailingones = VLC_TAB1[k];
			break;
		case 4:
		case 5:
			k = (bitCache>>25) - 2;
			numcoeff = VLC_TAB2[k];
			numtrailingones = VLC_TAB2[6+k];
			k = 6;
			break;
		case 6:
		case 7:
			numcoeff = 3 + (k-6); // 3,3,4,4 
			numtrailingones = 4 - (bitCache>>(30-k)); // 2,1,2,1 
			k++;
			break;
		default: //endLen >= 8;
			numcoeff = 4;
			numtrailingones = 3;
			k = 7;
		}


#if (DUMP_VERSION & DUMP_SLICE)
		//AvdLog(DUMP_SLICE, "\nChromaDC:numTrailOnes(%d), nzCoeff(%d)", numtrailingones, numcoeff);
#endif // (DUMP_VERSION & DUMP_SLICE)
	}

	if (!numcoeff){
		FlushBitsInline(currStream, k);
		return 0;
	}

	// max k = 19 bits used;
	bitShift = 30 - k;
	//numones = numtrailingones;
	code = numcoeff - 1;
	if (numtrailingones){
		bitShift -= numtrailingones;
		tmpI = (bitCache>>(bitShift + 1));
		i = numtrailingones;
		do {
			levarr[code--] = 1 - (((tmpI>>(--i))&1)<<1);
		} while (i);
	}

	// bitCache: max 19 + numtrailingones bits used;
	// decode levels
	if (code >= 0){
		level_two_or_higher = 1;
		vlcnum = 0;
		if (numtrailingones == 3){
			if (numcoeff > 3)
				level_two_or_higher = 0; 
		}
		else {
			if (numcoeff > 10)
				vlcnum = 1;
		}
		k = code;
		do {
			// read pre zeros
			if (bitShift < 7 + vlcnum){
				// need vlcnum bits for non escape code + 8 bit checking avdNumLeadingZerosPlus1;
				FlushBitsInline(currStream, 30 - bitShift);
				bitCache = ShowBitsInline(currStream, 31); 
				bitShift = 30;
			}

			endLen = avdNumLeadingZerosPlus1[(bitCache>>(bitShift - 7))&0xff] - 1; // leading one bits(not include one);
			if (endLen & 0x8){ // same as endLen = 8; not find 1;	
				if (bitShift < 15 + vlcnum){
					// need 6 + 8 + 8 bits for non escape code;
					FlushBits(currStream, 38 - bitShift); // also flush out 8 bits zeros;
					bitCache = ShowBits(currStream, 31); 
					bitShift = 38;
				}
				endLen = avdNumLeadingZerosPlus1[(bitCache>>(bitShift - 15))&0xff] + 7; // leading one bits(not include one);
			}

			if (endLen < 15 - (vlcnum == 0)){
				if (vlcnum){
					// max 6 bits used inside;
					tmpJ  = ((bitCache>>(bitShift - endLen - vlcnum))&LsbOnes[vlcnum]);
					level = ((endLen++)<<(vlcnum - 1)) + (tmpJ>>1) + 1;
					endLen += vlcnum;
				}
				else{
					level = (endLen>>1) + 1;
					tmpJ  = endLen++;
				}
			}
			else {
				// escape code;
				if (bitShift < 27){
					// not happen often, use non-inline;
					FlushBits(currStream, 30 - bitShift);
					bitCache = ShowBits(currStream, 31); 
					bitShift = 30;
				}
				if (!vlcnum){
					// escape code; endLen = 14 or 15;
					i = ((endLen - 14)<<3);
					tmpI = 4 + i; // 4 or 12;
					tmpJ = ((bitCache>>(bitShift - endLen - tmpI))&LsbOnes[tmpI]);
					level = (tmpJ >> 1) + tmpI + 4;//+8 or +16;
					endLen += tmpI + 1;
				}
				else {
					// read 11 bits -> levabs, and 1 bit sign;
					tmpJ = ((bitCache>>(bitShift - 27))&LsbOnes[12]);
					level = (tmpJ>>1) + (15<<(vlcnum - 1)) + 1;
					endLen = 28;
				}
			}

			if (k == code && ((level += level_two_or_higher) > 3))
				vlcnum = 2;
			else if (level > VLC_TAB12[vlcnum])
				vlcnum++;

			levarr[k] = (tmpJ & 1) ? -level : level;
			bitShift -= endLen;
		} while (--k >= 0);
	}

	i = numcoeff-1;
	if (numcoeff < max_coeff_num)
	{

		// decode total run
		if (bitShift < 8){
			// need 7+2 bits;;
			FlushBits(currStream, 30 - bitShift);
			bitCache = ShowBits(currStream, 31); 
			bitShift = 30;
		}

		if (!cdc){
			if (i){
				k = ((bitCache>>(bitShift - 5))&0x3f); // take 6 bits;
				if (k >= 8){
					code = (k>>3); // the first 3 bits;
					k = ((i - 1)<<3) + ((numcoeff > 5 || code > 2 || !(k&0x4)) ? code : (code<<5));
					k = VLC_TAB7[k];
					zerosleft = (k&0xf);
					k >>= 4;
				}
				else {
					if (numcoeff < 13){
						k = VLC_TAB8[((i - 1)<<3)+k];
						zerosleft = (k&0xf);
						k >>= 4;
					}
					else {
						zerosleft = 0;
						k = 16 - numcoeff;
					}
				}
			}
			else {
				k = avdNumLeadingZerosPlus1[(bitCache>>(bitShift-7))&0xff]; // leading one bits(include one);
				if (k == 1)
					zerosleft = 0;
				else if (k < 9){
					zerosleft = (k<<1) - ((bitCache>>(bitShift-k))&0x3); // take 2 bits (including leading one);
					k++;
				}
				else {// k == 9
					zerosleft = 15;
				}
			}
		}
		else{
			k = ((bitCache>>(bitShift-2))&0x7); // take 3 bits;
			k = VLC_TAB9[(i<<3)+k];
			zerosleft = (k&0xf);
			k >>= 4;
		}
		bitShift -= k;


		// decode run before each coefficient
		while (zerosleft != 0 && i != 0) 
		{
			// select VLC for runbefore
			
			if (bitShift < 2){
				// need 3 bits;
				FlushBits(currStream, 30 - bitShift);
				bitCache = ShowBits(currStream, 31); 
				bitShift = 30;
			}

			k = ((bitCache>>(bitShift - 2))&0x7); // show 3 bits;
			if (zerosleft < 7){
				// select VLC for runbefore
				vlcnum    = ((zerosleft - 1)<<3); 
				k = VLC_TAB10[vlcnum+k];
				runarr[i] = (k&0xf);
				k >>= 4;
			}
			else if (k){ // vlcnum == 6, and ShowBit(3)
				runarr[i] = 7 - k;
				k = 3;
			}
			else {
				if (bitShift < 15){
					// need 16 bits; not happen often, use non-inline;
					FlushBits(currStream, 30 - bitShift);
					bitCache = ShowBits(currStream, 31); 
					bitShift = 30;
				}
				k = avdNumLeadingZerosPlus1[(bitCache>>(bitShift - 7))&0xff]; // leading one bits(include one);
				if (k == 9) // not find 1;	
					k = avdNumLeadingZerosPlus1[(bitCache>>(bitShift - 15))&0xff] + 8;

				runarr[i] = k + 3;
			}
			bitShift -= k;


			zerosleft -= runarr[i];
			i--;
		}
	}
	else
		zerosleft = 0;

	runarr[i] = zerosleft;
	while (--i >= 0){
		runarr[i] = 0;
	}
	FlushBitsInline(currStream, 30 - bitShift);	
	//CHECK_BITSTREAM_OVERFLOW(currStream,"read4x4VLC overflow");
	return 	numcoeff;
}
avdNativeInt read_one_macroblock_i_slice_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream *bs)
{
	// TBD: allocate one SE at img to reuse in everywhere;
	SyntaxElement currSE;
	Macroblock *currMB = info->currMB,*topMB;
	Slice *currSlice = img->currentSlice;
	TFuncInfo	*funcInfo = img->funcInfo;
	avdNativeInt i, j;
	avdNativeInt listIdx; // for list 0 & 1;
	avdInt32 *tmp32Pt;

	DataPartition *dP = &(currSlice->partArr[0]);
	Bitstream *currStream = dP->bitstream;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TPosInfo	*posInfo = GetPosInfo(info);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	avdNativeInt check_bottom, read_bottom, read_top,prevMbSkipped;



#if (DUMP_VERSION & DUMP_SLICE)

	AvdLog(DUMP_SLICE,DUMP_DCORE "\n\n*** MB=%d", GetCurPos(info,sizeInfo));
#endif //(DUMP_VERSION & DUMP_SLICE)

	mbUnMarkSkipped(currMB);
	//mbSetQP(currMB, img->qp);
#if FEATURE_INTERLACE
	if (GetMbAffFrameFlag(img))
	{
		if(CurrMBisTop(info))
		{
			TRACE_STRING("mb_field_decoding_flag");
			currSE.value1 = (avdNativeUInt)GetBits(currStream, 1);
			mbSetMbFieldValue(currMB, currSE.value1);
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nread mbFieldFlag(%d)", currSE.value1);
#endif //(DUMP_VERSION & DUMP_SLICE)
		}
	
		Init_MB_Neighbor_InfoMBAff(img,info, 1);
	}
#endif //VOI_H264D_NON_BASELINE
	
	currSE.value1 = GetVLCSymbol_NEW (currStream);
	mbSetMbType(currMB, currSE.value1);


	CHECK_BITSTREAM_OVERFLOW(currStream,"vlc1 overflow");

	funcInfo->interpret_mb_mode(img,info);

	CHECK_ERR_RET_INT


#if FEATURE_T8x8
		mbUnMarkMbT8x8Flag(currMB);
	if (IS_4x4_INTRA(currMB)&& img->Transform8x8Mode)//VOTO8x8
	{

		TRACE_STRING("transform_size_8x8_flag");


		currSE.value1 = avd_u_1(currStream);

		if (currSE.value1)
		{
			mbMarkMbT8x8Flag(currMB);
			mbSetMbType(currMB,I8MB);
			MEMSET_B8MODE(I8MB);
			MEMSET_B8PDIR(B8NOPRED);
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB->I8MB(%d)\n ", currSE.value1);

		}
	}
#endif//FEATURE_T8x8
	//--- init macroblock data ---
#if FEATURE_BFRAME// VOI_INPLACE_DEBLOCKING
	mbSetMbB8FromImgB8(currMB, mbBits->uMBS.b8Info.b8Mode);
#endif //VOI_INPLACE_DEBLOCKING
	mbResetItransFlag(mbBits); // only 24 bits used now;


	if(mbGetMbType(currMB) != IPCM)		
	{
		// intra prediction modes for a macroblock **********************************************
		if (IS_INTRA(currMB)){
			read_ipred_modes_CAVLC(img,info,bs);CHECK_ERR_RET_INT
		}

		funcInfo->read_CBP_and_coeffs_from_NAL (img,info,bs);CHECK_ERR_RET_INT

	}
	else
	{
		readIPCMcoeffsFromNAL_CAVLC(img,info,bs);CHECK_ERR_RET_INT
	}
	return DECODE_MB;
}



//from MB.c
avdNativeInt read_one_macroblock_pb_slice_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream *bs)
{
	// TBD: allocate one SE at img to reuse in everywhere;
	SyntaxElement currSE;
	Macroblock *currMB = info->currMB;
	Slice *currSlice = img->currentSlice;
	avdNativeInt i, j;
	avdNativeInt listIdx; // for list 0 & 1;
	avdNativeInt prevMbSkipped = 0;
	

	pic_parameter_set_rbsp_t *active_pps = img->active_pps;
	avdInt32 *tmp32Pt;
	MacroBlock   *currMBInfo, *leftMBInfo,*topMB;
	//avdNativeInt isVLC=1,isEI=1,isVLCOREI=1; 
	TFuncInfo	*funcInfo = img->funcInfo;
	//DataPartition *dP = &(currSlice->partArr[0]);
	Bitstream *currStream = bs;//dP->bitstream;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	TMBsParser *parser = GetMBsParser(info);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
#if FEATURE_INTERLACE
	avdNativeInt check_bottom, read_bottom, read_top,isTopMB= CurrMBisTop(info);
#endif//#if FEATURE_INTERLACE

#if (DUMP_VERSION & DUMP_SLICE)
	AvdLog(DUMP_SLICE,DUMP_DCORE "\n\n*** MB=%d", GetCurPos(info,sizeInfo));
#endif //(DUMP_VERSION & DUMP_SLICE)

	mbUnMarkSkipped(currMB);
	//mbSetQP(currMB, img->qp);
	CHECK_BITSTREAM_OVERFLOW(currStream,"vlc2 overflow");
	topMB = GetTopMB(info,info->mb_x);
	prevMbSkipped = mbIsSkipped(topMB);


	{
		if(img->cod_counter == -1)
		{
			TRACE_STRING("mb_skip_run");
			img->cod_counter = GetVLCSymbol_NEW (currStream);
		}
		if (img->cod_counter==0)
		{
			
#if FEATURE_INTERLACE
			// read MB aff
		
			if (GetMbAffFrameFlag(img))
			{
				if(isTopMB || (!isTopMB && prevMbSkipped))
				{
					TRACE_STRING("mb_field_decoding_flag");
					mbSetMbFieldValue(currMB, (avdNativeUInt)GetBits(currStream, 1));
					AvdLog(DUMP_SLICE, DUMP_DCORE"\nread mbFieldFlag(%d)", mbIsMbField(currMB));
				}
			
				Init_MB_Neighbor_InfoMBAff(img,info,1); 
			}			
#endif //VOI_H264D_NON_BASELINE
			
			currSE.value1 = GetVLCSymbol_NEW (currStream);
#if FEATURE_BFRAME
			if(img->type == P_SLICE || img->type == SP_SLICE)
				currSE.value1++;
			mbSetMbType(currMB, currSE.value1);

#else //VOI_H264D_NON_BASELINE
			mbSetMbType(currMB, currSE.value1 + 1);
			//mbBits->isDirect16x16 =0;
#endif //VOI_H264D_NON_BASELINE

			img->cod_counter--;
		} 
		else
		{
			img->cod_counter--;
			mbSetMbType(currMB, 0);
			mbMarkSkipped(currMB);
#if FEATURE_INTERLACE
			// read field flag of bottom block
			mbUnMarkEi(currMB);
			if(GetMbAffFrameFlag(img))
			{
				if(!img->cod_counter && isTopMB)
				{
					TRACE_STRING("mb_field_decoding_flag (of coded bottom mb)");
					// be careful, here use ShowBits, instead of GetBits;
					mbSetMbFieldValue(currMB, (avdNativeUInt)GetBits(currStream, 1));//IL TBD? is it ShowBits? I think it should be Get1Bit
					AvdLog(DUMP_SLICE, DUMP_DCORE"\nmbFieldFlag(%d) from bottom MB", mbIsMbField(currMB));
				}
				else if(img->cod_counter > 0 && isTopMB)
				{
					int tmp = (sizeInfo->PicWidthInMbs<<1);
					// check left macroblock pair first
					if (info->mb_x >= 1)//IL TBD:
					{
						Macroblock* mbX = GetTopMB(info,info->mb_x-1);
						mbSetMbFieldValue(currMB, mbIsMbField(mbX));
					}
					else
					{
						Macroblock* mbX = GetTopMB(info->anotherProcessor,info->mb_x);
						if (info->mb_y){
							mbSetMbFieldValue(currMB, GetFieldByPos(img,info,info->mb_y-2,info->mb_x));//IsUpField
						}
						else
							mbUnMarkMbField(currMB);
					}
					AvdLog(DUMP_SLICE, DUMP_DCORE"\nmbFieldFlag(%d), neighbor inferred", mbIsMbField(currMB));
					
				}
				Init_MB_Neighbor_InfoMBAff(img,info,1); 
			}
#endif//FEATURE_INTERLACE

		}
		// need mb_field flag to set up Neighbor;
		//Init_MB_Neighbor_Info(img);
	}
	funcInfo->interpret_mb_mode(img,info);

	//mbBits->isDirect16x16 = (img->type == B_SLICE && !currSE.value1);
	CHECK_ERR_RET_INT


#if FEATURE_BFRAME//||FEATURE_BFRAME//TODOBframe
		currMBInfo = info->currMB;
	//leftMBInfo = info->leftMB;
#endif//FEATURE_BFRAME
#if FEATURE_T8x8
	mbUnMarkMbT8x8Flag(currMB);
	if (IS_4x4_INTRA(currMB)&& img->Transform8x8Mode)//VOTO8x8
	{

		TRACE_STRING("transform_size_8x8_flag");


		currSE.value1 = avd_u_1(currStream);


		if (currSE.value1)
		{
			mbMarkMbT8x8Flag(currMB);
			mbSetMbType(currMB,I8MB);
			//memset(&currMB->b8mode, I8MB, 4 * sizeof(char));
			//memset(&currMB->b8pdir, -1, 4 * sizeof(char));
			MEMSET_B8MODE(I8MB);
			MEMSET_B8PDIR(B8NOPRED);
			AvdLog(DUMP_SLICE,DUMP_DCORE "\nI4MB->I8MB(%d)\n ", currSE.value1);

		}
	}
#endif//FEATURE_T8x8
	//====== READ 8x8 SUB-PARTITION MODES (modes of 8x8 blocks) and Intra VBST block modes ======
	if (IS_P8x8 (currMB))
	{



		for (i = 0; i < 4; i++)
		{
			TRACE_STRING("sub_mb_type");

			currSE.value1 = GetVLCSymbol_NEW(currStream);

			SetB8Mode (img,info,currMB, currSE.value1, i);CHECK_ERR_RET_INT
#if (DUMP_VERSION & DUMP_SLICE)
				AvdLog(DUMP_SLICE,DUMP_DCORE "subMbType(%d/%d), ", i, currSE.value1);
#endif
		}
	}

	//! End TO
#if FEATURE_BFRAME

	if (IS_DIRECT (currMB))
	{

		if (currSlice->direct_type!=B_DIRECT_TEMPORAL)
			read_motion_info_of_BDirect(img,info);
		else
			read_motion_info_of_BTempol(img,info);

		if(img->cod_counter >= 0)
		{
			parser->cbp = 0;
			return DECODE_MB;
		}			
	}

#endif //VOI_H264D_NON_BASELINE ||FEATURE_BFRAME
	CHECK_ERR_RET_INT
		if (IS_COPY (currMB)){ //keep last macroblock
			parser->cbp = 0;
			return GetPSliceSkippedMV(img,info);
		}

		//--- init macroblock data ---
#if FEATURE_BFRAME// VOI_INPLACE_DEBLOCKING
		mbSetMbB8FromImgB8(currMB, mbBits->uMBS.b8Info.b8Mode);
#endif //VOI_INPLACE_DEBLOCKING
		mbResetItransFlag(mbBits); // only 24 bits used now;


		if(mbGetMbType(currMB) != IPCM)
		{
			// intra prediction modes for a macroblock **********************************************
			if (IS_INTRA(currMB)){

				read_ipred_modes_CAVLC(img,info,bs);

				CHECK_ERR_RET_INT
					if(img->notSaveAllMV&&img->type!=I_SLICE)
					{
						AVDStoredPicMotionInfo *motionInfo = img->dec_picture->motionInfo;
						//AvdLog(DUMP_SEQUENCE,"RIM2_intra prediction\n");
						//CHECK_MOTIONINFO(img,motionInfo);
						i = (posInfo->block_x>>1);
						motionInfo->ref_idx0[0][i] = 
							motionInfo->ref_idx0[0][i+1] = 
							motionInfo->ref_idx0[1][i] = 
							motionInfo->ref_idx0[1][i+1] = AVD_INVALID_REF_IDX;
						// only update MB boundary;
						tmp32Pt = (avdInt32 *)&motionInfo->mv0[0][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
						tmp32Pt = (avdInt32 *)&motionInfo->mv0[1][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[3] = 0;
						tmp32Pt = (avdInt32 *)&motionInfo->mv0[2][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[3] = 0;
						tmp32Pt = (avdInt32 *)&motionInfo->mv0[3][posInfo->block_x].x;
						tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
#if FEATURE_BFRAME
						if (img->type == B_SLICE){
							motionInfo->ref_idx1[0][i] = 
								motionInfo->ref_idx1[0][i+1] = 
								motionInfo->ref_idx1[1][i] = 
								motionInfo->ref_idx1[1][i+1] = AVD_INVALID_REF_IDX;
							// only update MB boundary;
							tmp32Pt = (avdInt32 *)&motionInfo->mv1[0][posInfo->block_x].x;
							tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
							tmp32Pt = (avdInt32 *)&motionInfo->mv1[1][posInfo->block_x].x;
							tmp32Pt[0] = tmp32Pt[3] = 0;
							tmp32Pt = (avdInt32 *)&motionInfo->mv1[2][posInfo->block_x].x;
							tmp32Pt[0] = tmp32Pt[3] = 0;
							tmp32Pt = (avdInt32 *)&motionInfo->mv1[3][posInfo->block_x].x;
							tmp32Pt[0] = tmp32Pt[1] = tmp32Pt[2] = tmp32Pt[3] = 0;
						}
#endif //VOI_H264D_BASELINE_PLUS_BFRAME

					}




			}


			else if (mbGetMbType(currMB) != 0)
			{

				funcInfo->read_motion_info_from_NAL (img,info,bs);CHECK_ERR_RET_INT

			}	

			// read CBP and Coeffs  ***************************************************************

			funcInfo->read_CBP_and_coeffs_from_NAL (img,info,bs);CHECK_ERR_RET_INT

		}
		else
		{


			readIPCMcoeffsFromNAL_CAVLC(img,info,bs);
			CHECK_ERR_RET_INT
		}


		return DECODE_MB;
}

/*!
************************************************************************
* \brief
*    read next VLC codeword for 4x4 Intra Prediction Mode and
*    map it to the corresponding Intra Prediction Direction
************************************************************************
*/
void read_ipred_modes_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream *bs)
{
	SyntaxElement currSE;
	const avdUInt8 *partMap;
	Slice *currSlice;
	DataPartition *dP;
	Macroblock *currMB;
	pic_parameter_set_rbsp_t *active_pps = img->active_pps;
	avdNativeInt isVLC=1;
	avdNativeInt isEI,isVLCOREI,is8x8=0,is4x4=0; 
	TMBBitStream	*mbBits = GetMBBits(info->currMB);
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	int c_ipred_mode;
#if (DUMP_VERSION & DUMP_SLICE)
	int dumpI4Mode[4][4];
	int dumpFlag = 0;
	int i, j;
	for (j = 0; j < 4; j++)
		for (i = 0; i < 4; i++)
			dumpI4Mode[j][i] = 9;
#endif //(DUMP_VERSION & DUMP_SLICE)


	currMB = info->currMB;
	currSlice = img->currentSlice;
	isEI  =	GetEIFlag(bs);
	isVLCOREI = isVLC||isEI;
#if FEATURE_T8x8
	is8x8 = IS_8x8_INTRA(currMB);
#endif
	is4x4 = IS_4x4_INTRA(currMB);
	//checkbits(16,info);
	if (is4x4||is8x8){ 

		MacroBlock *mbA, *mbB;
		avdNativeInt k, bx, by, addr;
		avdNativeInt mostProbableIntraPredMode;
		avdNativeInt upIntraPredMode;
		avdNativeInt leftIntraPredMode, bitShift;
		//TMBsProcessor *info  = img->mbsProcessor;
		avdUInt32			mode4x4;
		avdNativeInt		step = is4x4?1:4;
		//avdUInt32        Coef8x8[4]={0,2,8,10};
		int					leftMBYForIntra = -1;
		mbA = GetMbAffFrameFlag(img)? GetLeftMBAffMB2(img,info,GetPosInfo(info)->subblock_y*4,16,&leftMBYForIntra):GetLeftMB(info);
		
		mbB = GetUpMB(info);
		if (active_pps->constrained_intra_pred_flag){
			//change definition of neighbor A, B;
			if (mbA && !IS_INTRA(currMB - 1))
				mbA = NULL;

			if (mbB && !niIsIntraMb(info, GetMBX(info)))
				mbB = NULL;
		}
		if (isVLC){
			// get multiple to share;
			mode4x4 = ShowBits(bs, 31); 
			bitShift = 30;
		}

		for (k = 0; k < 16; k+=step){ 
			avdNativeInt value;
			//avdNativeInt k2=Coef8x8[k];
			//get from stream

			if (bitShift < 3){
				FlushBits(bs, 30 - bitShift);
				mode4x4 = ShowBits(bs, 31); 
				bitShift = 30;
			}
			if ((mode4x4>>bitShift)&0x1){
				value = -1;
			}
			else {
				bitShift -= 3;
				value = ((mode4x4>>bitShift)&0x7);
			}
			bitShift--;


			bx = decode_scan[k]; 
			by = decode_scan[16+k]; 
			

			if (by){
				upIntraPredMode = mbBits->uMBS.mbIntra4x4Mode[((by - 1)<<2) | bx];
			}
			else{
#if FEATURE_T8x8
				if (mbB){
					upIntraPredMode = (!active_pps->constrained_intra_pred_flag || 
						IS_INTRA_MB_ADDRESS2(mbB)) ?
						(IS_4x4_INTRA_MB_ADDRESS3(mbB) ? 
						niGetIntra4x4Mode(mbB, bx) : DC_PRED) : AVD_INVALID_INTRAMODE;
				}
				else 
					upIntraPredMode = AVD_INVALID_INTRAMODE;
#else//FEATURE_CABAC
				upIntraPredMode = mbB ? (niIsIntra4x4Mb(info, GetMBX(info)) ?niGetIntra4x4Mode(mbB, bx) : DC_PRED) : AVD_INVALID_INTRAMODE;
#endif//FEATURE_CABAC
			}

			if (bx){
				leftIntraPredMode = mbBits->uMBS.mbIntra4x4Mode[(by<<2) | (bx - 1)];
			}
			else{
#if FEATURE_T8x8
				if (mbA){
					int idx = by;
					MacroBlock* leftMB = mbA;
					if(GetMbAffFrameFlag(img))
					{
						leftMB = GetLeftMBAffMB2(img,info,by*4,16,&leftMBYForIntra);
						idx = leftMBYForIntra>>2;	
					}
					leftIntraPredMode =  
						(!active_pps->constrained_intra_pred_flag || 
						IS_INTRA_MB_ADDRESS2(leftMB)) ?
						(IS_4x4_INTRA_MB_ADDRESS3(leftMB) ? 
						niGetIntra4x4ModeLeft(leftMB, idx) : DC_PRED) : AVD_INVALID_INTRAMODE;
					//AvdLog(DUMP_SLICE,DUMP_DCORE"\n%d,%d,%d",idx,leftMB->mb_x,leftMB->mb_y);
				}
				else
					leftIntraPredMode = AVD_INVALID_INTRAMODE;
#else//FEATURE_CABAC
				leftIntraPredMode = mbA ? (IS_4x4_INTRA(currMB - 1) ?niGetIntra4x4ModeLeft(mbA, by) : DC_PRED) : AVD_INVALID_INTRAMODE;
#endif//FEATURE_CABAC
			}
			mostProbableIntraPredMode  = (upIntraPredMode == AVD_INVALID_INTRAMODE
				|| leftIntraPredMode == AVD_INVALID_INTRAMODE) ? 
DC_PRED : upIntraPredMode < leftIntraPredMode ? upIntraPredMode : 
		  leftIntraPredMode;
			{
				avdNativeInt index=(by<<2) | bx;
				avdNativeInt value2=(value == -1) ? mostProbableIntraPredMode : value + (value >= mostProbableIntraPredMode);
				mbBits->uMBS.mbIntra4x4Mode[index]=value2;
				//AvdLog(DUMP_SLICE,DUMP_DCORE"\n%v=d,u=%d,l=%d",value,upIntraPredMode,leftIntraPredMode);
#if FEATURE_T8x8
				if(is8x8)
				{
					mbBits->uMBS.mbIntra4x4Mode[index+1]=value2; index+=4;
					mbBits->uMBS.mbIntra4x4Mode[index]=value2; 
					mbBits->uMBS.mbIntra4x4Mode[index+1]=value2; 
				}
#endif//#if FEATURE_T8x8

			}
			if (mostProbableIntraPredMode<0||mostProbableIntraPredMode>8)
			{
				mostProbableIntraPredMode = 0;
				AVD_ERROR_CHECK2(img,"mostProbableIntraPredMode<0||mostProbableIntraPredMode>8!", ERROR_InvalidIntraPredMode);
			}
#if (DUMP_VERSION & DUMP_SLICE)
			dumpFlag = 1;
#if FEATURE_T8x8
			if(is8x8)
			{
				dumpI4Mode[by][bx] =
					dumpI4Mode[by][bx+1] =
					dumpI4Mode[by+1][bx] =
					dumpI4Mode[by+1][bx+1] = mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx];
				_dumpIP4[mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx]]+=4;
			}
			else
#endif
			{
				dumpI4Mode[by][bx] = mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx];
				_dumpIP4[mbBits->uMBS.mbIntra4x4Mode[(by<<2)|bx]]++;

			}

#endif //(DUMP_VERSION & DUMP_SLICE)

		}

		FlushBits(bs, 30 - bitShift);

		// for 4x4 Intra;
		niClearIntra4x4Mode(info->currMB);
		niSetAllIntra4x4Mode(info->currMB, mbBits->uMBS.mbIntra4x4Mode[12], 
			mbBits->uMBS.mbIntra4x4Mode[13], mbBits->uMBS.mbIntra4x4Mode[14], mbBits->uMBS.mbIntra4x4Mode[15]);

		//niClearIntra4x4Mode(info->leftMB);
		niSetAllIntra4x4ModeLeft(info->currMB, mbBits->uMBS.mbIntra4x4Mode[3], 
			mbBits->uMBS.mbIntra4x4Mode[7], mbBits->uMBS.mbIntra4x4Mode[11], mbBits->uMBS.mbIntra4x4Mode[15]);
	}

#if (DUMP_VERSION & DUMP_SLICE)
	if (dumpFlag)
	{
		//AvdLog(DUMP_SLICE,DUMP_DCORE "\nleftIntra=%d",info->currMB->niFlag1);
		for (i = 0; i < 4; i++)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\n%5s(%d) %5s(%d) %5s(%d) %5s(%d)", 
			IPredName4x4[dumpI4Mode[i][0]], dumpI4Mode[i][0],
			IPredName4x4[dumpI4Mode[i][1]],dumpI4Mode[i][1],IPredName4x4[dumpI4Mode[i][2]],
			dumpI4Mode[i][2],IPredName4x4[dumpI4Mode[i][3]],dumpI4Mode[i][3]);
	}
#endif //(DUMP_VERSION & DUMP_SLICE)



	c_ipred_mode = GetVLCSymbol_NEW (bs);
	mbSetCPredMode(mbBits,c_ipred_mode);

	if (c_ipred_mode > PLANE_8 || c_ipred_mode<DC_PRED_8)
	{
		//c_ipred_mode = DC_PRED_8;
		AVD_ERROR_CHECK2(img,"illegal chroma intra pred mode!\n", ERROR_InvalidIntraPredCMode);
	}

#if (DUMP_VERSION & DUMP_SLICE)
	AvdLog(DUMP_SLICE,DUMP_DCORE "\nchrm8: %s(%d)", IPredName8x8[c_ipred_mode], c_ipred_mode);
	_dumpIP8[c_ipred_mode]++;
#endif //(DUMP_VERSION & DUMP_SLICE)

}

void read_motion_info_from_NAL_pb_slice_CAVLC (ImageParameters *img,TMBsProcessor *info,Bitstream *bs)
{
	Macroblock *currMB  = info->currMB;
	SyntaxElement currSE;
	Slice *currSlice    = img->currentSlice;
	StorablePicture	*dec_picture = img->dec_picture;
	AVDStoredPicMotionInfo	*motionInfo = dec_picture->motionInfo;
	AVDMotionVector pmv;
	avdNativeInt partmode        = (IS_P8x8(currMB) ? 4 : mbGetMbType(currMB));
	avdNativeInt step_h0         = BLOCK_STEP[partmode];
	avdNativeInt step_v0         = BLOCK_STEP[8+partmode];
	avdNativeInt i,j,k,step_h,step_v,curr_mvd, curr_mvd_y, mvRefStartY4x4;
	avdNativeInt mv_mode, i0, j0;
	avdNativeInt j8, i8, j4, i4, ii,jj, xTmp, yTmp;
	avdNativeInt ref_idx,vec,iTRb,iTRp,isMovingList1;
	avdNativeInt mv_scale = 0;
	avdNativeInt flag_mode,imgblock_y, imgblock_x;
	SliceType ipbType;
	avdUInt8 refframe;
	avdNativeUInt motionInfoNotExist;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdUInt8 *b8Pdir = mbBits->uMBS.b8Info.b8Pdir;	
	avdUInt8 *b8Mode = mbBits->uMBS.b8Info.b8Mode; 
	avdUInt8 **ref_idx0, **ref_idx1;
	AVDMotionVector	**mv0, **mv1;
	TSizeInfo	*sizeInfo = img->sizeInfo;
	TPosInfo*  posInfo  = GetPosInfo(info);
	//TMBBitStream	*mbBits = GetMBBits(currMB);
	int mbType=mbGetMbType(currMB);

#if FEATURE_BFRAME

	avdNativeInt bframe          = (img->type==B_SLICE);
#else//FEATURE_BFRAME
	const avdNativeInt bframe          = 0;
#endif//FEATURE_BFRAME
	imgblock_y = posInfo->block_y;


	if(img->notSaveAllMV)
		mvRefStartY4x4 = 0;
	else //SAVE_ALL_MV
		mvRefStartY4x4 = posInfo->block_y;


	imgblock_x = posInfo->block_x;
	ref_idx0 = motionInfo->ref_idx0;
	mv0 = motionInfo->mv0;
#if FEATURE_BFRAME
	ref_idx1 = motionInfo->ref_idx1;
	mv1 = motionInfo->mv1;
	//mbBits->hasDirect8x8 = IS_B8x8(currMB) && !(b8Mode[0] && b8Mode[1]&& b8Mode[2] && b8Mode[3]);
	// TBD: combine with P8x8 mode reading if there is no 4x4 IBlcok;
	if (IS_B8x8(currMB) && !(b8Mode[0] && b8Mode[1]&& b8Mode[2] && b8Mode[3]))//mbBits->hasDirect8x8)
	{
		if (currSlice->direct_type!=B_DIRECT_TEMPORAL)
		{
			read_motion_info_of_BDirect(img,info);					
		}
		else
		{
			read_motion_info_of_BTempol(img,info);
		}
	} 
#endif //VOI_H264D_NON_BASELINE

	//  If multiple ref. frames, read reference frame for the MB *********************************
	if(img->num_ref_idx_l0_active < 2) 
	{
		const avdNativeUInt i8 = (imgblock_x>>1);
#if FEATURE_BFRAME//hxy2010
		k = 0;
		do {
			if ((b8Mode[k] && HasB8FwPred(b8Pdir[k])))
				ref_idx0[(mvRefStartY4x4 + k)>>1][i8 + (k&1)] = 0;
		} while (++k < 4);
#else //FEATURE_BFRAME

		{
			int j8 = mvRefStartY4x4>>1;
			ref_idx0[j8][i8] = ref_idx0[j8][i8 + 1] = 
				ref_idx0[j8+1][i8] = ref_idx0[j8+1][i8 + 1] = 0; 
		}
#endif //FEATURE_BFRAME
	}
	else
	{
		avdNativeInt getRef = !IS_P8x8 (currMB) || bframe || !info->mbsParser->allrefzero;
		flag_mode = (img->num_ref_idx_l0_active == 2 ? 1 : 0);
		k = 0;
		do {
			avdNativeUInt tmpX, tmpY;
#if FEATURE_BFRAME
			if (b8Mode[k]==0 || HasNoB8FwPred(b8Pdir[k]))
				continue;
#endif //VOI_H264D_NON_BASELINE

			TRACE_STRING("ref_idx_l0");
			tmpX = ((k&1)<<1);
			tmpY = ((k>>1)<<1);          
			if (getRef)
			{

				if (!flag_mode){
					refframe = GetVLCSymbol_NEW (bs);
				}
				else
					refframe = 1 - GetBits(bs, 1);

			}
			else
				refframe = 0;

			if(refframe<0||refframe>=img->num_ref_idx_l0_active)//valid [0,img->num_ref_idx_l0_active)
			{
				AVD_ERROR_CHECK2(img,"refframe<0||refframe>img->num_ref_idx_l0_active",ERROR_InvalidRefIndex);
			}


			i0 = ((imgblock_x + tmpX)>>1);
			if(img->notSaveAllMV)
				j0 = (tmpY>>1);
			else //SAVE_ALL_MV
				j0 = ((posInfo->block_y + tmpY)>>1);

			ref_idx0[j0][i0] = refframe;
			if (step_h0 == 4){
				ref_idx0[j0][i0 + 1] = refframe;
				if (step_v0 == 4){ //16x16
					ref_idx0[j0 + 1][i0] = 
						ref_idx0[j0 + 1][i0 + 1] = refframe;
					break;
				}
				k++; //16x8;
			}
			else if (step_v0 == 4){ //8x16;
				ref_idx0[j0 + 1][i0] = refframe;
				if ((k == 0 && EqualBwPred(b8Pdir[k+1])) || k == 1)
					break;
			}
		} while (++k < 4);
	}

	//  If backward multiple ref. frames, read backward reference frame for the MB *********************************

#if FEATURE_BFRAME
	if(img->num_ref_idx_l1_active < 2)
	{
		for (k = 0; k < 4; k++){
			if (b8Mode[k]!=0 && HasB8BwPred(b8Pdir[k]))
				ref_idx1[(mvRefStartY4x4 + k)>>1][(imgblock_x>>1) + (k&1)] = 0;
		}
	}
	else
	{

		flag_mode = (img->num_ref_idx_l1_active == 2 ? 1 : 0);
		for (k = 0; k < 4; k++) {
			if (b8Mode[k]==0 || HasNoB8BwPred(b8Pdir[k]))
				continue;

			TRACE_STRING("ref_idx_l1");
			posInfo->subblock_x = ((k&1)<<1);
			posInfo->subblock_y = ((k>>1)<<1);          


			if (!flag_mode){
				//currSE.value2 = LIST_1;
				currSE.value1 = GetVLCSymbol_NEW (bs);
			}
			else
				currSE.value1 = 1 - (avdNativeUInt)GetBits(bs, 1);

			refframe = currSE.value1;

			i0 = ((imgblock_x + posInfo->subblock_x)>>1);
			j0 = ((mvRefStartY4x4 + posInfo->subblock_y)>>1);
			ref_idx1[j0][i0] = refframe;
			if (step_h0 == 4){
				ref_idx1[j0][i0 + 1] = refframe;
				if (step_v0 == 4){ //16x16
					ref_idx1[j0 + 1][i0] = 
						ref_idx1[j0 + 1][i0 + 1] = refframe;
					break;
				}
				k++; //16x8;
			}
			else if (step_v0 == 4){ //8x16;
				ref_idx1[j0 + 1][i0] = refframe;
				if ((k == 0 && EqualFwPred(b8Pdir[k+1])) || k == 1)
					break;
			}
		}
	}

#endif //VOI_H264D_NON_BASELINE

	//CHECK_BITSTREAM_OVERFLOW(bs,"vlc3 overflow\n");
	//=====  READ FORWARD MOTION VECTORS =====
	j0 = 0;
	do {
		i0 = 0;
		do {

			k=((j0>>1)<<1)|(i0>>1);

#if FEATURE_BFRAME
			if ((b8Mode[k] && HasB8FwPred(b8Pdir[k])))
#endif //VOI_H264D_NON_BASELINE
			{
				avdNativeUInt blkIdx;
				mv_mode  = b8Mode[k];
				step_h   = BLOCK_STEP[mv_mode];
				step_v   = BLOCK_STEP[8+mv_mode]; 
				// see definition in SetMotionVectorPredictor();
				blkIdx = (step_h<<4)|(step_v<<12);

				refframe = ref_idx0[(mvRefStartY4x4+j0)>>1][(imgblock_x+i0)>>1];        
				// below use | to replace "+", because if j = 1, jj = 0 Only;
				// j = 2, jj = 0, 1 only; j = 3, jj = 0, only;
				j = j0;
				do {
					j4 = mvRefStartY4x4 | j;

					i = i0;
					do {
						i4 = imgblock_x | i;            
						// first make mv-prediction
						SetMotionVectorPredictor (img,info,(blkIdx|(j<<8)|i), LIST_0, refframe, &pmv);
						jj = GetVLCSymbol_NEW (bs); 
						CHECK_BITSTREAM_OVERFLOW(bs,"vlc3 overflow\n");
						ii = (jj+1)>>1;
						curr_mvd = (jj&1) ? ii : -ii;
						pmv.x += curr_mvd;//(jj&1) ? ii : -ii;
						jj = GetVLCSymbol_NEW (bs); 
						ii = (jj+1)>>1;
						curr_mvd_y = (jj&1) ? ii : -ii;
						pmv.y += curr_mvd_y;//(jj&1) ? ii : -ii;


						jj = 0;
						do {
							avdUInt32 *tmpMV = (avdUInt32 *)&mv0[j4|jj][i4];
							ii = 0;
							do {
								*tmpMV++ = *(avdUInt32 *)&pmv;
							} while(++ii < step_h); 
						} while (++jj < step_v);
#if (DUMP_VERSION & DUMP_SLICE)
						AvdLog(DUMP_SLICE,DUMP_DCORE "\n(enc)l0mvd(%d,%d),mv(%d,%d) j,i=(%d,%d),refidx=%d", 
							curr_mvd, curr_mvd_y, 
							mv0[j4][i4].x,mv0[j4][i4].y, j, i,refframe);
#endif
					} while ((i+=step_h) < i0 + step_h0);
				} while ((j+=step_v) < j0 + step_v0); 

			}

		} while ((i0+=step_h0) < 4);
	} while ((j0+=step_v0) < 4);
	//CHECK_BITSTREAM_OVERFLOW(bs,"vlc4 overflow\n");
	//=====  READ BACKWARD MOTION VECTORS =====
#if FEATURE_BFRAME
	for (j0=0; j0<4; j0+=step_v0)
		for (i0=0; i0<4; i0+=step_h0)
		{
			avdNativeUInt blkIdx;
			k=((j0>>1)<<1)|(i0>>1);
			if (b8Mode[k]==0 || HasNoB8BwPred(b8Pdir[k]))
				continue;

			mv_mode  = b8Mode[k];
			step_h   = BLOCK_STEP[mv_mode];
			step_v   = BLOCK_STEP[8+mv_mode];
			blkIdx = (step_h<<4)|(step_v<<12);
			refframe = ref_idx1[(mvRefStartY4x4+j0)>>1][(imgblock_x+i0)>>1];
			for (j=j0; j<j0+step_v0; j+=step_v)
			{
				j4 = mvRefStartY4x4 | j;
				posInfo->subblock_y = j; // position used for context determination
				CHECK_ERR_RET_VOID
				for (i=i0; i<i0+step_h0; i+=step_h)
				{
					i4 = imgblock_x | i;            
					posInfo->subblock_x = i; // position used for context determination
					// first make mv-prediction
					SetMotionVectorPredictor (img,info,(blkIdx|(j<<8)|i), LIST_1, refframe, &pmv);
					currSE.value2 = 1; 
					//readFunPt(img,&currSE, dP->bitstream);

					readVLCSynElement_Se(img,&currSE,bs);

					curr_mvd = currSE.value1;               
					currSE.value2 = 3; 
					//readFunPt(img,&currSE, bs);

					readVLCSynElement_Se(img,&currSE,bs);

					curr_mvd_y = currSE.value1;               
					pmv.x += curr_mvd;
					pmv.y += curr_mvd_y;
					for(jj=0;jj<step_v;jj++){
						avdUInt32 *tmpMV = (avdUInt32 *)&mv1[j4|jj][i4];
						for(ii=0;ii<step_h;ii++)
						{
							*tmpMV++ = *(avdUInt32 *)&pmv;
						}
					}
					AvdLog(DUMP_SLICE,DUMP_DCORE "\n(enc)l1mvd(%d,%d),mv(%d,%d) j,i=(%d,%d),refidx=%d", 
						curr_mvd, curr_mvd_y, 
						mv1[j4][i4].x,mv1[j4][i4].y,j,i,refframe);
				}
			
			}
		}

#endif //FEATURE_BFRAME
#if 0//FEATURE_BFRAME// need to recalculate the mv if 
		if (IS_B8x8(currMB) && !(b8Mode[0] && b8Mode[1]&& b8Mode[2] && b8Mode[3]))//mbBits->hasDirect8x8)
		{
			if (currSlice->direct_type!=B_DIRECT_TEMPORAL)
			{
				read_motion_info_of_BDirect(img,info);					
			}
			else
			{
				read_motion_info_of_BTempol(img,info);
			}
		} 
#endif //VOI_H264D_NON_BASELINE
}


void read_CBP_and_coeffs_from_NAL_CAVLC(ImageParameters *img,TMBsProcessor *info,Bitstream *bs)
{
	Macroblock *currMB = info->currMB;
	Slice *currSlice = img->currentSlice;
	TMBBitStream	*mbBits = GetMBBits(currMB);
	avdUInt8 *numRuns = &info->mbsParser->numRuns[0]; 
	avdInt16 *cofPt;
	avdInt32 *tmp32Pt;
	avdInt64 *tmp64Pt;
	const avdUInt16 *dequanPt;
	avdNativeInt numcoeff;
	avdNativeInt i,j,k,ll;
	avdNativeInt cbp;
	avdNativeInt coef_ctr, i0, j0;
	avdNativeInt yTmp, qp_per, qp_rem;
	avdNativeInt *tmpLevarr, *levarr = (avdNativeInt *)&info->mbsParser->m7[0][0];
	avdNativeInt *tmpRunarr, *runarr = levarr + 16;

	avdNativeInt newIntra = IS_NEWINTRA (currMB);
	avdNativeInt Is4x4Intra, cbTypeIdx;
	SyntaxElement currSE;
	avdUInt8 *coefLocation;
	TCLIPInfo	*clipInfo = img->clipInfo;
	TPosInfo	*posInfo  = GetPosInfo(info);
	int		 div_qp_per;
	TMBsParser *parser = GetMBsParser(info);
	int		IsFrame = img->dec_picture->structure==FRAME;
#if FEATURE_MATRIXQUANT

	TDequantArray*		dequantcoef		= img->dequant_coef_shift;
	TDequant8Array*		dequantcoef8	= img->dequant_coef_shift8;
	TDequantArray_coef* InvLevelScale4x4_Intra	= dequantcoef;
	TDequantArray_coef* InvLevelScale4x4_Inter	= (avdUInt8*)dequantcoef + sizeof(TDequantArray_coef);
	TDequant8Array_coef* InvLevelScale8x8_Intra = dequantcoef8;
	TDequant8Array_coef* InvLevelScale8x8_Inter = (avdUInt8*)dequantcoef8 + sizeof(TDequant8Array_coef);
	avdUInt16 (*InvLevelScale4x4)[4] = NULL;
	avdUInt16 (*InvLevelScale8x8)[8] = NULL;
	int intra = IS_INTRA (currMB);
#endif

	//const avdNativeInt smb = 0;
	int mbType=mbGetMbType(currMB);
#if FEATURE_INTERLACE
	avdUInt8 *coefScan = ((img->specialInfo->structure == FRAME) && !mbIsMbField(currMB)) ? 
SNGL_SCAN_TO_INDEX : FIELD_SCAN_TO_INDEX;
#else//FEATURE_INTERLACE
	avdUInt8 *coefScan =SNGL_SCAN_TO_INDEX;
#endif//FEATURE_INTERLACE

	assert(((avdInt32)numRuns & 7) == 0);
	tmp64Pt = (avdInt64 *)numRuns; // size is NUMBER_RUN_SIZE * sizeof(avdUInt8);
	tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = 0;
	*(avdInt32 *)(numRuns + 24) = 0;


	// read CBP if not new intra mode
	if (!newIntra)
	{
		avdNativeInt value;
		Is4x4Intra = (IS_4x4_INTRA2 (mbType) || IS_8x8_INTRA2(mbType));
		TRACE_STRING("coded_block_pattern");
		{
			int index2 = GetVLCSymbol_NEW(bs);
			if(index2>=48||index2<0)
				AVD_ERROR_CHECK2(img,"index2>=48||index2<0",ERROR_InvalidBitstream);
			value = avd_NCBP[Is4x4Intra ? 0 : 1][index2];
		}


		parser->cbp = cbp = value;
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\ncbp(%d)",  cbp);
#endif
		// Delta quant only if nonzero coeffs
		if (cbp)
		{

#if FEATURE_T8x8

			int direct_8x8_inference_flag=img->active_sps->direct_8x8_inference_flag;
			int isDirect=IS_DIRECT(currMB);
			int need_transform_size_flag;
			int NoMbPartLessThan8x8Flag = ( isDirect&& !(direct_8x8_inference_flag))? 0: 1;
			if (mbType==P8x8)
			{
				int i;
				for (i = 0; i < 4; ++i)
				{
					int b8Mode=mbBits->uMBS.b8Info.b8Mode[i]; //mbGetB8Mode(currMB,i);
					NoMbPartLessThan8x8Flag &= (b8Mode==0 && direct_8x8_inference_flag) ||
						(b8Mode==4);
				}
			}
			need_transform_size_flag = (((mbType >= 1 && mbType <= 3)||
				(isDirect && direct_8x8_inference_flag) ||
				NoMbPartLessThan8x8Flag)
				&&(cbp&15)
				&& mbType != I8MB && mbType != I4MB
				&& img->Transform8x8Mode);
			if (need_transform_size_flag)
			{
				AvdLog(DUMP_SLICE,DCORE_DUMP"need_transform_size_flag:mbType=%d,isDirect=%d,direct_8x8_inference_flag=%d,NoMbPartLessThan8x8Flag=%d,8x8Mode=%d\n",mbType,isDirect,direct_8x8_inference_flag,NoMbPartLessThan8x8Flag,img->Transform8x8Mode);
				currSE.value1 = avd_u_1(bs);
				if (currSE.value1)
				{
					mbMarkMbT8x8Flag(currMB);
				}
			}
#endif//FEATURE_T8x8

			i = GetVLCSymbol_NEW (bs); 
			value = (i+1)>>1;//hxy2010:ref code uses the value directly
			if (!(i&1))
				value = -value;
			if(value>25||value<-26)
			{
				currSE.value1 = 0;
				AVD_ERROR_CHECK2(img,"mb_qp_delta out of range",ERROR_InvalidMbQPDelta);
			}
			
			{
				int tmp = img->qp+value+52;
				while(tmp>=52)
					tmp -= 52;
				if(tmp<0)
					tmp = 0;
				img->qp = tmp;
			}
#if (DUMP_VERSION & DUMP_SLICE)
			AvdLog(DUMP_SLICE,DUMP_DCORE "\ndeltaQP(%d), QP(%d)", value, img->qp);
#endif
		}

	}
	else {
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\n16x16 LumaDC");
#endif

		cbp = parser->cbp;//currMB->flags.cbp;
		// read DC coeffs for new intra modes
		readVLCSynElement_Se(img,&currSE, bs);

		//img->qp= clipInfo->clipQP51[img->qp+currSE.value1];
		{
			int tmp = img->qp+currSE.value1+52;
			while(tmp>=52)
				tmp -= 52;
			if(tmp<0)
				tmp = 0;
			img->qp = tmp;
		}
#if (DUMP_VERSION & DUMP_SLICE)
		AvdLog(DUMP_SLICE,DUMP_DCORE "\ndeltaQP(%d), QP(%d)", currSE.value1, img->qp);
#endif
		cbTypeIdx = COEFF_LUMA_INTRA16x16DC_IDX;
		numcoeff = readCoeff4x4_CAVLC(img,info,cbTypeIdx, LUMA_INTRA16x16DC, 0,
			levarr);


		if (numcoeff || (cbp&15)){
			// GetMBCofBuf(info) is always 8 bytes aligned;
			// TBD: Intel compiler does not generate ldrd;
			tmp64Pt = (avdInt64 *)GetMBCofBuf(info); // size is 384 * sizeof(avdInt16) * 2/3 for luma;
			i = 8; // 384 * 2 / 8 * 2/3 = 64
			do {
				tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 
					tmp64Pt[4] = tmp64Pt[5] = tmp64Pt[6] = tmp64Pt[7] = 0;
				tmp64Pt += 8;
			} while (--i);

			if (numcoeff){
				cofPt = GetMBCofBuf(info);
				tmpLevarr = levarr;
				tmpRunarr = runarr;
				coefLocation = (coefScan - 1);
				k = numcoeff;
				do {
					avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
					AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif
					run = *tmpRunarr++;
					lev = *tmpLevarr++;
					// use cofPt[6][4][4][4] offset;
					coefLocation += (run+1);
					// get j<<6 + i<<4

					if((coefLocation - coefScan) & 0xffff0000) // <0 or > 15;
						AVD_ERROR_CHECK2(img,"0: coef_ctr<0||coef_ctr>15",ERROR_InvalidBitstream);

					cofPt[((*coefLocation<<6)|(*coefLocation<<2))&0xf0]= lev;// add new intra DC coeff
				} while (--k);

				numRuns[cbTypeIdx] = (avdUInt8)numcoeff;
				itrans_2(img,info);// transform new intra DC

			}
		}


	}
	mbSetQP(currMB, img->qp);

	niSetCavlcSkippedOrLumaChromaAllACZeros(info->currMB, mbIsSkipped(currMB), cbp);

	if (!cbp)
		return;


	if (cbp&15)

	{
		// img->qp may be changed;
		yTmp	  = img->qp-MIN_QP;
		div_qp_per = DIVIDE6[yTmp];
		qp_per    = div_qp_per * 6;
		qp_rem    = yTmp - qp_per;
		// QPI
#if FEATURE_MATRIXQUANT
		if(img->initMatrixDequantDone)
		{
			InvLevelScale4x4 = intra? (*InvLevelScale4x4_Intra)[0][qp_rem] : (*InvLevelScale4x4_Inter)[0][qp_rem];
		}
		else
#endif//FEATURE
			dequanPt  = &img->dequant_coef_shift[DEQUANT_IDX(qp_rem,0,0)] + (qp_per<<4);

		// luma coefficients
		//if (IsVLCCoding(img->active_pps) || GetEIFlag(dP->bitstream))
		{
			avdNativeUInt block_type = newIntra ? LUMA_INTRA16x16AC : LUMA;
#if FEATURE_T8x8
			if (mbIsMbT8x8Flag(currMB))
			{
				//avdUInt8* dequanPt8  = &dequant_coef8[DEQUANT_IDX8(qp_rem,0,0)];// + (qp_per<<6);
				int kPos;	
				int coef_ctr,offset,j1;
#if FEATURE_MATRIXQUANT
				if(img->initMatrixDequantDone)
				{
					InvLevelScale8x8 = intra? (*InvLevelScale8x8_Intra)[0][qp_rem] : (*InvLevelScale8x8_Inter)[0][qp_rem];
				}
				else
#endif//FEATURE
					dequanPt  = &img->dequant_coef_shift8[DEQUANT_IDX8((qp_rem+qp_per),0,0)];// + (qp_per<<6);

				memset(GetMBCofBuf(info),0,sizeof(short)*256);

				for (ll = 0; ll < 16; ll++){
					kPos = ll>>2;
					j1 = T8x8KPos[kPos];
					if (!(cbp & (1<<(kPos)))){  /* are there any coeff in current block at all */
						ll += 3;
						continue;
					} //if (!(ll&3))
#if (DUMP_VERSION & DUMP_SLICE)
					//AvdLog(DUMP_SLICE, "\nCoef: type(Luma/%d)", ll);
#endif		

					i = decode_scan[ll];
					j = decode_scan[16+ll];
					j0 = (j<<2) | i;
					cbTypeIdx = ll + COEFF_LUMA_START_IDX;
					// notice in core lib readCoeff4x4_CAVLC, it assumes
					// runarr = levarr + 16 to save a function argument;
					//checkbits(31);
					numcoeff = readCoeff4x4_CAVLC(img,info,cbTypeIdx, block_type, j0,
						levarr);
					if (numcoeff){
						numRuns[cbTypeIdx] = (avdUInt8)numcoeff;

						mbBits->iTransFlags |= (1<<inv_decode_scan[j0]);
						mbSetCBPBlk(currMB,(51<<j1));//|(1<<(j1+1))|(1<<(j1+4))|(1<<(j1+5)));
						cofPt = &GetMBCofBuf(info)[kPos<<6];//[j0<<4];
						coefLocation =  IsFrame&&!mbIsMbField(info->currMB)?SNGL_SCAN8x8[0]:FIELD_SCAN8x8[0];
						coef_ctr = -1;
						offset = T8x8Offset1[ll];


						tmpLevarr = levarr;
						tmpRunarr = runarr;
						do {
							avdNativeInt lev, run,b4,i1;
							avdUInt8* coefLocation2;
#if (DUMP_VERSION & DUMP_SLICE)
							AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif
							run = *tmpRunarr++;
							lev = *tmpLevarr++;
							coef_ctr += (run+1);
							coefLocation2 = coefLocation+(coef_ctr*4+offset)*2;

							i1=coefLocation2[1]*8+coefLocation2[0];
							i0=i1;//(coefLocation2[1]&3)*4+(coefLocation2[0]&3);//coefScan[coef_ctr];//
#if FEATURE_MATRIXQUANT
							if(img->initMatrixDequantDone)
							{
								cofPt[i0] = rshift_rnd_sf((lev * InvLevelScale8x8[coefLocation2[1]][coefLocation2[0]])<<div_qp_per, 6);
#if (DUMP_VERSION & DUMP_SLICE)
								AvdLog(DUMP_SLICE,DUMP_DCORE "\noffset(%d,%d),value(%d,%d),deq=%d(%d,%d)", 
									coef_ctr << 2, offset,coefLocation2[0],coefLocation2[1],cofPt[i0],lev , InvLevelScale8x8[coefLocation2[1]][coefLocation2[0]]);		
#endif
							}
							else

#endif//FEATURE_MATRIXQUANT
								cofPt[i0] = (lev * dequanPt[i1]+3)>>2;

						} while (--numcoeff);
					}
				}//for
			}
			else
#endif//T8x8
				for (ll = 0; ll < 16; ll++){
					if (!(cbp & (1<<(ll>>2)))){  /* are there any coeff in current block at all */
						ll += 3;
						continue;
					} //if (!(ll&3))
#if (DUMP_VERSION & DUMP_SLICE)
					//AvdLog(DUMP_SLICE, "\nCoef: type(Luma/%d)", ll);
#endif

					i = decode_scan[ll];
					j = decode_scan[16+ll];
					j0 = (j<<2) | i;
					cbTypeIdx = ll + COEFF_LUMA_START_IDX;
					// notice in core lib readCoeff4x4_CAVLC, it assumes
					// runarr = levarr + 16 to save a function argument;
					//checkbits(31,info);
					numcoeff = readCoeff4x4_CAVLC(img,info,cbTypeIdx, block_type, j0,
						levarr);
					if (numcoeff){
						numRuns[cbTypeIdx] = (avdUInt8)numcoeff;

						mbBits->iTransFlags |= (1<<j0);
						mbSetCBPBlk(currMB,1<<j0);//currMB->cbp_blk |= (1<<j0);
						cofPt = &GetMBCofBuf(info)[j0<<4];
						coefLocation = coefScan;
						if (!newIntra){
							coefLocation--;
							// GetMBCofBuf(info) is always 8 bytes aligned;
							tmp64Pt = (avdInt64 *)cofPt; // size is 384 * sizeof(avdInt16) * 2/3;
							tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
						}

						tmpLevarr = levarr;
						tmpRunarr = runarr;
						do {
							avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
							AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif
							run = *tmpRunarr++;
							lev = *tmpLevarr++;
							coefLocation += (run+1);

							if((coefLocation - coefScan)  & 0xffff0000)
								AVD_ERROR_CHECK2(img,"1:coef_ctr<0||coef_ctr>15",ERROR_InvalidBitstream);

							i0 = *coefLocation; // get columnIdx*4+rowIdx;
#if FEATURE_MATRIXQUANT
							if(img->initMatrixDequantDone)
							{

								cofPt[i0] = rshift_rnd_sf((lev * InvLevelScale4x4[i0/4][i0&3])<<div_qp_per, 4);
#if (DUMP_VERSION & DUMP_SLICE)
								AvdLogDeq(DUMP_SLICE,info, "\n(%d,%d),deq=%d(%d,%d)", i0/4,i0&3,cofPt[i0],lev , InvLevelScale4x4[i0/4][i0&3]);		
#endif
							}
							else
#endif//FEATURE_MATRIXQUANT
								cofPt[i0] = lev * dequanPt[i0];

						} while (--numcoeff);
					}


				} // for ll;
		}


	}



	if(cbp>15)
	{
		// chroma 2x2 DC coeff
		avdNativeInt qp_uv = img->qp + img->active_pps->chroma_qp_index_offset;
		avdNativeInt *uPt;
		avdNativeInt   coFu[4];
		qp_uv = clipInfo->clipQP51[qp_uv];
		div_qp_per = DIVIDE6[QP_SCALE_CR[qp_uv-MIN_QP]];
		qp_per =  div_qp_per* 6;
		qp_rem = QP_SCALE_CR[qp_uv-MIN_QP] - qp_per;

		dequanPt  = &img->dequant_coef_shift[DEQUANT_IDX(qp_rem,0,0)] + (qp_per<<4);

		yTmp = dequanPt[0];

		uPt = coFu;
		for (ll=0;ll<3;ll+=2)
		{
			int uv = ll>>1; 

#if (DUMP_VERSION & DUMP_SLICE)
			//AvdLog(DUMP_SLICE, "\nCoef: type(ChromaDC/%s)", ll==0 ? "U" : "V");
#endif
			cbTypeIdx = (ll == 0) ? COEFF_CHROMA_U_DC_IDX : COEFF_CHROMA_V_DC_IDX;
#if FEATURE_MATRIXQUANT
			if(img->initMatrixDequantDone)
			{
				InvLevelScale4x4 = intra ? (*InvLevelScale4x4_Intra)[uv + 1][qp_rem] :  (*InvLevelScale4x4_Inter)[uv + 1][qp_rem];

			}
#endif//FEATURE
			//if (IsVLCCoding(img->active_pps) || GetEIFlag(dP->bitstream))
			{
				numcoeff = readCoeff4x4_CAVLC(img,info,cbTypeIdx, CHROMA_DC, 0, levarr);
			}

			cofPt = (avdInt16*)&GetMBCofBuf(info)[(16+ll)*16];
			if (numcoeff){
				avdUInt32 flag;
				avdNativeInt a,b,c,d, e;
				tmpLevarr = levarr;
				tmpRunarr = runarr;
				coef_ctr=-1;
				uPt[0] = uPt[1] = uPt[2] = uPt[3] = 0;
				k = numcoeff;
				do {
					avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
					AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif //(DUMP_VERSION & DUMP_SLICE)
					run = *tmpRunarr++;
					lev = *tmpLevarr++;
					coef_ctr += 1 + run;
#if FEATURE_MATRIXQUANT
					if(img->initMatrixDequantDone)
					{
						uPt[coef_ctr] = (((lev  * InvLevelScale4x4[0][0])<<div_qp_per)>>4);
					}
					else
#endif//FEATURE
						uPt[coef_ctr] = lev * yTmp;
				} while (--k);


				numRuns[cbTypeIdx] = (avdUInt8)numcoeff;

				//if (!smb) // check to see if MB type is SPred or SIntra4x4 
				{
					b = uPt[0]+uPt[1];
					d = uPt[2]+uPt[3];
					a = ((b + d)>>1);
					c = ((b - d)>>1);
					e = uPt[0]-uPt[1];
					d = uPt[2]-uPt[3];
					b = ((e + d)>>1);
					d = ((e - d)>>1);
				}

#if FEATURE_MATRIXQUANT
				if(img->initMatrixDequantDone)
				{
#if (DUMP_VERSION & DUMP_SLICE)
					AvdLogDeq(DUMP_SLICE,info, "\nihadamard2x2(%d,%d,%d,%d),%d)",a,b,c,d,InvLevelScale4x4[0][0]);		
#endif
				}
#endif//FEATURE
				if (a){
					tmp64Pt = (avdInt64 *)cofPt;
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(16+ll));
				}
				// alias problem in GCC compiler if put below into if(a){};
				cofPt[0] = a;

				if (b){
					tmp64Pt = (avdInt64 *)(cofPt + 16);
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(17+ll));
				}
				cofPt[16] = b;

				if (c){
					tmp64Pt = (avdInt64 *)(cofPt + 64);
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(20+ll));
				}
				cofPt[64] = c;

				if (d){
					tmp64Pt = (avdInt64 *)(cofPt + 80);
					tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					mbBits->iTransFlags |= (1<<(21+ll));
				}
				cofPt[80] = d;
			}
			else
				cofPt[0] = cofPt[16] = cofPt[64] = cofPt[80] = 0;


		}


		// chroma AC coeff, all zero from start_scan
		if (cbp>31){

			for (ll = 0; ll < 8; ll++){
#if (DUMP_VERSION & DUMP_SLICE)
				//AvdLog(DUMP_SLICE, "\nCoef: type(ChromaAC/%s%d)", (ll<4?"U":"V"), (ll&3));
#endif //(DUMP_VERSION & DUMP_SLICE)
#if FEATURE_MATRIXQUANT
				if(img->initMatrixDequantDone)
				{
					int uv=ll>3;
					InvLevelScale4x4 = intra ? (*InvLevelScale4x4_Intra)[uv + 1][qp_rem] :  (*InvLevelScale4x4_Inter)[uv + 1][qp_rem];

				}
#endif//FEATURE
				i = chromaScan[ll];
				j = chromaScan[8+ll];	
				j0 = (j<<2)|i;
				//j1 = (1 << (((j - 4)<<1) + 16 + (i&1) + ((i>>1)<<2)));
				cbTypeIdx = COEFF_CHROMA_U_AC_START_IDX + ll;


				//if (IsVLCCoding(img->active_pps) || GetEIFlag(dP->bitstream))
				{
					numcoeff = readCoeff4x4_CAVLC(img,info,cbTypeIdx, CHROMA_AC, j0, levarr);
				}


				if (numcoeff){
					avdNativeInt index = j0<<4;

					//	if(i0+index>=24*16)//hbfTODO20061013:over the bound
					const avdNativeInt errIdx = 24*16 - index;

					mbBits->iTransFlags |= (1<<j0);
					cofPt = &GetMBCofBuf(info)[index];
					if (!cofPt[0]){
						tmp64Pt = (avdInt64 *)cofPt;
						tmp64Pt[0] = tmp64Pt[1] = tmp64Pt[2] = tmp64Pt[3] = 0;
					}
					tmpLevarr = levarr;
					tmpRunarr = runarr;
					coefLocation = coefScan;
					k = numcoeff;
					do {
						avdNativeInt lev, run;
#if (DUMP_VERSION & DUMP_SLICE)
						AvdLogLevel(img,info,DUMP_SLICE, "\nlevel(%d), run(%d)", *tmpLevarr, *tmpRunarr);
#endif
						run = *tmpRunarr++;
						lev = *tmpLevarr++;
						coefLocation += (run+1);

						if((coefLocation - coefScan) & 0xffff0000)
						{
							AVD_ERROR_CHECK2(img,"4:coef_ctr<0||coef_ctr>15",ERROR_InvalidBitstream);
						}
						i0 = *coefLocation; // get columnIdx*4+rowIdx;


						if(i0 >= errIdx)//hbfTODO20061013:over the bound
						{
							AVD_ERROR_CHECK2(img,"bitstream error,memmory overbound",ERROR_NULLPOINT);
						}
#if FEATURE_MATRIXQUANT
						if(img->initMatrixDequantDone)
						{
							cofPt[i0] = rshift_rnd_sf((lev * InvLevelScale4x4[i0&3][i0/4])<<div_qp_per, 4);
#if (DUMP_VERSION & DUMP_SLICE)
							AvdLogDeq(DUMP_SLICE,info, "\n(%d,%d),deq=%d(%d,%d)", i0&3,i0/4,cofPt[i0],lev , InvLevelScale4x4[i0&3][i0/4]);		
#endif
						}
						else
#endif//FEATURE

							cofPt[i0] = lev * dequanPt[i0];
					} while (--k);					
					numRuns[cbTypeIdx] = (avdUInt8)numcoeff;

				}


			} // for ll;

		} //if (cbp<=31)
	}

}

#endif//inline the VLCIP

