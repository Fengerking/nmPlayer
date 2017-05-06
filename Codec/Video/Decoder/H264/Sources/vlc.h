
/*!
 ************************************************************************
 * \file vlc.h
 *
 * \brief
 *    header for (CA)VLC coding functions
 *
 * \author
 *    Karsten Suehring
 *
 ************************************************************************
 */

#ifndef _VLC_H_
#define _VLC_H_

#define avd_ue_v(bsPt)			GetVLCSymbol_NEW(bsPt) 
#define avd_u_v(len, bsPt)		GetBits(bsPt, len)
#define avd_u_1(bsPt)			GetBits(bsPt, 1)

avdInt32 avd_se_v (Bitstream *bitstream);

// UVLC mapping
void readVLCSynElement_Se(ImageParameters *img,SyntaxElement *sym, Bitstream *currStream);
void readVLCSynElement_Ue(SyntaxElement *sym, Bitstream *currStream);

//int readSyntaxElement_FLC(SyntaxElement *sym, Bitstream *currStream);
avdNativeUInt GetVLCSymbol_NEW (Bitstream *str);
avdNativeInt readCoeff4x4_CAVLC (ImageParameters *img,TMBsProcessor *info,avdNativeUInt codingBlockIdx, avdNativeUInt block_type, avdNativeInt j, avdNativeInt *levarr);

#ifdef _WIN32_WINNT

extern __inline avdUInt32 VOI_BYTESWAP(a) 
{ 
	__asm mov		eax, a 
	__asm bswap		eax 
}

#elif (defined _WIN32_WCE)// for WinCE, _byteswap_ulong(a) & _byteswap_uint64(a); _CountLeadingZeros(long arg1), _CountLeadingOnes64(long arg1); 

	#include <Cmnintrin.h>
	#if _INTRINSIC__byteswap_ulong // _INTRINSIC_IS_SUPPORTED(_byteswap_ulong) //&& _INTRINSIC_IS_INLINE(_byteswap_ulong)
		#define VOI_BYTESWAP(a) _byteswap_ulong(a);
	#else
		#define VOI_BYTESWAP(a) ((a<<24) | ((a<<8)&0x00ff0000) | ((a>>8)&0x0000ff00) | (a>>24));
	#endif
#else
#if  defined(ARM_ASM) && defined(LINUX)
static inline  avdUInt32 VOI_BYTESWAP(avdUInt32 a)
{
       __asm__  __volatile__("rev %0, %0" : "+r"(a));
	return a;
}
#else
	#define VOI_BYTESWAP(a) ((a<<24) | ((a<<8)&0x00ff0000) | ((a>>8)&0x0000ff00) | (a>>24));
#endif	
#endif


#if 0 // for ARM
 VOI_BYTESWAP:								// r0 = A,B,C,D;
	eor		r1, r0, r0, ror #16			// r1 = A^C, B^D, C^A, D^B;	
	bic		r1, r1, #0xff0000			// r1 = A^C, 0,   C^A, D^B;
	mov		r0, r0, ror #8				// r0 = D,   A,     B,   C;
	eor		r0, r0, r1, LSR #8			// r0 = D,   C,     B,   A;
// signed integer divided by 2^n;
 SINGED_DIV:
	mov		r1, r0, ASR #31
	add		r0, r0, r1, LSR #(32-n)
	mov		r0, r0, ASR #n
#endif

extern const avdUInt32 LsbOnes[33];

void FlushBits(Bitstream *bStr, avdNativeInt numBits);
avdUInt32 ShowBits(Bitstream *bStr, avdNativeInt numBits);
avdUInt32 GetBits(Bitstream *bStr, avdNativeInt numBits);

#if 0//(DUMP_VERSION & DUMP_BITS)
extern __inline avdInt32 GetSliceBitsRead(ImageParameters *img,TMBsProcessor *info)
{
	return (((img->currBitStream->currReadBuf - img->currBitStream->streamBuffer)<<3) 
		- img->currBitStream->bBitToGo - 32); // add bufferB
}
#endif //(DUMP_VERSION & DUMP_BITS)

#endif

