
	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/

/*!
************************************************************************
*
* \file		x86Cabac.c
*
* \brief
*		defines short MMX inline function for local usage;
*
************************************************************************
*/

#include "global.h"
#include "biaridecod.h"
#include "vlc.h"

//#define TEST_BDS_ASSEMBLY
#ifdef TEST_BDS_ASSEMBLY
#include <memory.h>
#endif TEST_BDS_ASSEMBLY
//! struct to characterize the state of the arithmetic coding engine
// NOTICE: bo not change the order data in Bitstream and BiContextType;
// assumption: offsets of Drange and Dvalue are 40 and 42 in biari_decode_symbol
avdNativeUInt biari_decode_symbol(Bitstream *dep, BiContextTypePtr bi_ct)
{
#ifdef TEST_BDS_ASSEMBLY // for bit compliant test;
	Bitstream testDep;
	BiContextType testBict;
	avdUInt8 bit, rLPS;
	int teLen = dep->currReadBuf - dep->streamBuffer;
	static int ccc = 0;
	ccc++;
	if (ccc == 0x2)
		printf("here");
	memcpy (&testDep, dep, sizeof (Bitstream));
	memcpy (&testBict, bi_ct, sizeof (BiContextType));
	testDep.currReadBuf = testDep.streamBuffer + teLen;
#endif TEST_BDS_ASSEMBLY


	__asm {
		mov			edx, dep 
		mov			edx, [edx + 40] /*range at low word, value at high word*/
		mov			esi, edx
		shr			esi, 6
		and			esi, 3 /*(dep->Drange>>6) & 0x03*/
		xor			eax, eax
		mov			edi, bi_ct
		mov			ax, [edi] /*state at al, MPS at ah*/
		mov			edi, eax 
		and			edi, 0xff /*state*/
		xor			ecx, ecx
		mov			cl, BYTE PTR avd_rLPS_table_64x4[esi + 4 * edi]
		sub			edx, ecx /*dep->Drange - rLPS*/
		mov			esi, edx
		shr			esi, 16 /*value*/
		and			edx, 0xffff /*range*/
		cmp			esi, edx
		jae			BDS_LPS
		mov			cl, BYTE PTR avd_AC_next_state_MPS_64[edi]
		jmp			BDS_CHECKQUARTER
BDS_LPS:
		sub			esi, edx /*dep->Dvalue -= dep->Drange*/
		mov			edx, ecx /*dep->Drange = rLPS*/
		xor			ah, 1
		test		edi, edi
		jnz			BDS_NO_MPS_UPDATE
		mov			ecx, bi_ct
		mov			[ecx+1], ah /*save MPS*/
BDS_NO_MPS_UPDATE:
		mov			cl, BYTE PTR avd_AC_next_state_LPS_64[edi]
BDS_CHECKQUARTER:
		mov			edi, bi_ct
		mov			[edi], cl /*save state*/
		mov			edi, dep /*edi is free now*/
		cmp			edx, 256
		jae			BDS_DONE
		xor			ecx, ecx
		mov			cl, BYTE PTR avdNumLeadingZerosPlus1[edx] /*shift*/
		shl			edx, cl
		shl			esi, cl
		push		edx /* save values of esi, edx, eax due to function call*/
		push		eax 
		push		ecx /* pass augument 2 of GetBits*/
		push		edi /* pass augument 1 of GetBits*/
		call		GetBits
		or			esi, eax
		/* For __stdcall, the code is good*/
		/* However, for __cdecl, the following two lines of code need to be added*/
		/*pop		ecx*/
		/*pop		ecx*/
		pop			eax /* reload values of esi, edx, eax due to function call*/
		pop			edx
BDS_DONE:
		shl			esi, 16
		or			edx, esi /*range in low word, value in high word*/
		mov			[edi + 40], edx /*save range and value*/
		shr			eax, 8 /*shift "bit" at ah for return value*/
	}


#ifdef TEST_BDS_ASSEMBLY
	bit = testBict.MPS;
	rLPS = avd_rLPS_table_64x4[testBict.state][(testDep.Drange>>6) & 0x03];

	testDep.Drange -= rLPS;
	if (testDep.Dvalue < testDep.Drange) /* MPS */ 
		testBict.state = avd_AC_next_state_MPS_64[testBict.state]; // next state
	else						  /* LPS */
	{
		testDep.Dvalue -= testDep.Drange;
		testDep.Drange = rLPS;
		bit ^= 1;
		if (!testBict.state)			 // switch meaning of MPS if necessary	
			testBict.MPS ^= 1;              
		testBict.state = avd_AC_next_state_LPS_64[testBict.state]; // next state 
	}

	if (testDep.Drange < QUARTER){
		avdUInt8 shift = avdNumLeadingZerosPlus1[testDep.Drange];
		testDep.Drange <<= shift;
		testDep.Dvalue = (testDep.Dvalue<<shift) | GetBits(&testDep, shift);
	}
	if (testDep.Dvalue != dep->Dvalue || testDep.Drange != dep->Drange 
		|| testDep.bBitToGo != dep->bBitToGo
		|| testDep.bufferA != dep->bufferA || 
		(testDep.currReadBuf - testDep.streamBuffer)!= (dep->currReadBuf - dep->streamBuffer))
		printf("\nstop1, ccc = %d", ccc);
	if (testBict.MPS != bi_ct->MPS || testBict.state != bi_ct->state)
		printf("\nstop2, ccc = %d", ccc);
	return(bit);
#endif TEST_BDS_ASSEMBLY
}
