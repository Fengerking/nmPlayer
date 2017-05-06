/************************************************************************
*									*									*
*		VisualOn, Inc. Confidential and Proprietary, 2004	*
*									*									*
************************************************************************/

#include "sbr_dec.h"

#ifdef SBR_DEC
const int vo_PrecsTabledelt[32] = 
{ 
	0x3f35b59d,   0xc26626cf,   
	0x3bf47cfc,   0xc5ba1e0a,   
	0x388e4a89,   0xc9320585,   
	0x350536f0,   0xcccbb9a8,   
	0x315b7063,   0xd0850204,   
	0x2d93393a,   0xd45b92ad,   
	0x29aee693,   0xd84d0dad,   
	0x25b0dee8,   0xdc57046e,   
	0x219b9883,   0xe076f944,   
	0x1d719810,   0xe4aa60f3,   
	0x19356efa,   0xe8eea441,   
	0x14e9b9e3,   0xed41218c,   
	0x10911f04,   0xf19f2e6e,   
	0x0c2e4c88,   0xf606195e,   
	0x07c3f6e9,   0xfa732b5e,   
	0x0354d741,   0xfee3a9a2,   
};

const int vo_PrecsTable[64] = {
	1086837437,   13176464, 1112535695, 1073014240, 1137563802,   65842639, 1161906684, 1069782521, 
	1185549677,  118350194, 1208478539, 1063973603, 1230679458,  170572633, 1252139062, 1055601479, 
	1272844425,  222384147, 1292783074, 1044686319, 1311942998,  273659918, 1330312657, 1031254418, 
	1347880985,  324276419, 1364637401, 1015338134, 1380571810,  374111709, 1395674614,  996975812, 
	1409936715,  423045732, 1423349524,  976211688, 1435904960,  470960600, 1447595461,  953095785, 
	1458413984,  517740883, 1468354013,  927683790, 1477409561,  563273883, 1485575172,  900036924, 
	1492845928,  607449906, 1499217450,  870221790, 1504685900,  650162530, 1509247982,  838310216, 
	1512900951,  691308855, 1515642604,  804379079, 1517471291,  730789757, 1518385910,  768510122, 
};

const int vo_postcstable64delt[17] = 
{ 
	1073741824, 1019762441,  963326361,  904569543,  843633538,  780665145,  715816062,  649242516, 
	581104887,  511567326,  440797355,  368965464,  296244703,  222810262,  148839052,   74509276, 
	0, 
};

const int vo_postcstable64[34] = {
	1073741824,          0, 1125134469,   52686014, 1173816567,  105245103, 1219670837,  157550647, 
	1262586814,  209476638, 1302461109,  260897982, 1339197660,  311690799, 1372707968,  361732726, 
	1402911301,  410903207, 1429734898,  459083786, 1453114139,  506158392, 1472992700,  552013618, 
	1489322693,  596538995, 1502064778,  639627258, 1511188256,  681174602, 1516671150,  721080937, 
	1518500250,  759250125,
};


static void vo_PreMDCTIV(int *inbuf)
{
	const int *tableptr, *deltptr;
	int k, temp1, temp2;
	int re1, im1, re2, im2;
	int temp; 
	int delcs, cstab1, cstab2, cstab3, cstab4;
	int *tmpbuf;
	int Mdct_Pixel = 64;

	tmpbuf   = inbuf + 63;
	tableptr = vo_PrecsTable;
	deltptr  = vo_PrecsTabledelt;

	for (k = Mdct_Pixel >> 2; k != 0; k--) {

		re1 = *(inbuf);
		im2 = *(inbuf + 1);
		im1 = *(tmpbuf);
		re2 = *(tmpbuf - 1);

		cstab1 = *tableptr++;	
		cstab2 = *tableptr++;
		cstab3 = *tableptr++;	
		cstab4 = *tableptr++;

		temp  = Vo_Multi32(cstab2, re1 + im1);
		temp2 = Vo_Multi32(cstab1, im1) - temp;
		delcs = *deltptr++;
		temp1 = Vo_Multi32(delcs, re1) + temp;
		*inbuf++ = temp1;	
		*inbuf++ = temp2;	

		temp  = Vo_Multi32(cstab4, re2 + im2);
		temp2 = Vo_Multi32(cstab3, im2) - temp;
		delcs = *deltptr++;
		temp1 = Vo_Multi32(delcs, re2) + temp;
		*tmpbuf-- = temp2;	
		*tmpbuf-- = temp1;	
	}
}

static void vo_PostMDCTIV(int *inBuf, 
						  int num)
{
	int i, re1, im1, re2, im2;
	int temp;
	int delcs, cstab1, cstab2;
	int *tmpBuf;
	const int *tabptr, *deltptr;

	tabptr  = vo_postcstable64;
	deltptr = vo_postcstable64delt;

	tmpBuf = inBuf + 63;

	cstab1  = *tabptr++;
	cstab2  = *tabptr++;
	delcs   = *deltptr++;

	for (i = (num + 3) >> 2; i != 0; i--) {
		re1 = *(inBuf + 0);
		im1 = *(inBuf + 1);
		re2 = *(tmpBuf - 1);
		im2 = *(tmpBuf + 0);

		temp = Vo_Multi32(cstab2, re1 + im1);
		*tmpBuf-- = temp - Vo_Multi32(cstab1, im1);
		*inBuf++  = temp + Vo_Multi32(delcs, re1);

		cstab1 = *tabptr++;
		cstab2 = *tabptr++;

		im2 = -im2;
		temp = Vo_Multi32(cstab2, re2 + im2);
		*tmpBuf-- = temp - Vo_Multi32(cstab1, im2);
		delcs = *deltptr++;
		*inBuf++ = temp + Vo_Multi32(delcs, re2);
	}
}

#if (!(defined(_IOS))) && ((defined (__arm) && defined (__ARMCC_VERSION)) || (defined (_WIN32) && defined (_WIN32_WCE) && defined (ARM)) || (defined(__GNUC__) && defined(__arm__)))
#ifdef __cplusplus
extern "C"
#endif
void QMFAnalysisConv(int *cTab, 
		     int *delay, 
		     int dIdx, 
		     int *pBuf
		     );
#else
void QMFAnalysisConv(int *cTab, 
		     int *delay,  
		     int dIdx, 
		     int *pBuf
		     )
{
	int k, nOffset;
	int *cPtr0, *cPtr1;
	U64 nTemp64lo, nTemp64hi;

	nOffset = dIdx*32 + 31;
	cPtr0 = cTab;
	cPtr1 = cTab + 164;

	nTemp64lo.w64 = 0;
	nTemp64hi.w64 = 0;
	nTemp64lo.w64 = MAC32(nTemp64lo.w64,  *cPtr0++,   delay[nOffset]);	
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64hi.w64 = MAC32(nTemp64hi.w64,  *cPtr0++,   delay[nOffset]);
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64lo.w64 = MAC32(nTemp64lo.w64,  *cPtr0++,   delay[nOffset]);
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64hi.w64 = MAC32(nTemp64hi.w64,  *cPtr0++,   delay[nOffset]);	
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64lo.w64 = MAC32(nTemp64lo.w64,  *cPtr0++,   delay[nOffset]);
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64hi.w64 = MAC32(nTemp64hi.w64,  *cPtr1--,   delay[nOffset]);	
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64lo.w64 = MAC32(nTemp64lo.w64, -(*cPtr1--), delay[nOffset]);
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64hi.w64 = MAC32(nTemp64hi.w64,  *cPtr1--,   delay[nOffset]);
	nOffset -= 32; 
	if (nOffset < 0) {nOffset += 320;}
	nTemp64lo.w64 = MAC32(nTemp64lo.w64, -(*cPtr1--), delay[nOffset]);
	nOffset -= 32;
       	if (nOffset < 0) {nOffset += 320;}
	nTemp64hi.w64 = MAC32(nTemp64hi.w64,  *cPtr1--,   delay[nOffset]);
	nOffset -= 32;
       	if (nOffset < 0) {nOffset += 320;}

	pBuf[0]  = nTemp64lo.r.hi32;
	pBuf[32] = nTemp64hi.r.hi32;
	pBuf++;
	nOffset--;

	for (k = 1; k <= 31; k++) {
		nTemp64lo.w64 = 0;
		nTemp64hi.w64 = 0;
		nTemp64lo.w64 = MAC32(nTemp64lo.w64, *cPtr0++, delay[nOffset]);	
		nOffset -= 32;
	       	if (nOffset < 0) {nOffset += 320;}
		nTemp64hi.w64 = MAC32(nTemp64hi.w64, *cPtr0++, delay[nOffset]);
		nOffset -= 32; 
		if (nOffset < 0) {nOffset += 320;}
		nTemp64lo.w64 = MAC32(nTemp64lo.w64, *cPtr0++, delay[nOffset]);	
		nOffset -= 32;
	       	if (nOffset < 0) {nOffset += 320;}
		nTemp64hi.w64 = MAC32(nTemp64hi.w64, *cPtr0++, delay[nOffset]);
		nOffset -= 32;
	       	if (nOffset < 0) {nOffset += 320;}
		nTemp64lo.w64 = MAC32(nTemp64lo.w64, *cPtr0++, delay[nOffset]);
		nOffset -= 32;
	       	if (nOffset < 0) {nOffset += 320;}
		nTemp64hi.w64 = MAC32(nTemp64hi.w64, *cPtr1--, delay[nOffset]);
		nOffset -= 32;
	       	if (nOffset < 0) {nOffset += 320;}
		nTemp64lo.w64 = MAC32(nTemp64lo.w64, *cPtr1--, delay[nOffset]);
		nOffset -= 32; 
		if (nOffset < 0) {nOffset += 320;}
		nTemp64hi.w64 = MAC32(nTemp64hi.w64, *cPtr1--, delay[nOffset]);
		nOffset -= 32; 
		if (nOffset < 0) {nOffset += 320;}
		nTemp64lo.w64 = MAC32(nTemp64lo.w64, *cPtr1--, delay[nOffset]);
		nOffset -= 32; 
		if (nOffset < 0) {nOffset += 320;}
		nTemp64hi.w64 = MAC32(nTemp64hi.w64, *cPtr1--, delay[nOffset]);
		nOffset -= 32;
	       	if (nOffset < 0) {nOffset += 320;}

		pBuf[0]  = nTemp64lo.r.hi32;
		pBuf[32] = nTemp64hi.r.hi32;
		pBuf++;
		nOffset--;
	}
}
#endif


int QMFAnalysis(int *inbuf, 
				int *delay, 
				int *XBuf, 
				int fBitsIn, 
				int *del_Idx, 
				int Qmf_bands
				)
{
	int i, temp1;
	int voGBMask=0;
	int temp = 0;
	int *delPtr;
	int *Pre_Buf;
	int *Post_Buf;
	int R_Val;


	delPtr = delay + (*del_Idx * 32);
	R_Val  = SBR_IN_QMFA - fBitsIn;
	Pre_Buf = XBuf;	
	Post_Buf = XBuf + 64;

	for(i = 32; i !=0; i--)
	{
		temp1 = *inbuf++;
		CLIP_2N_SHIFT(temp1, R_Val);
		*delPtr++ = temp1;
	}

	QMFAnalysisConv((int *)cTabA, delay, *del_Idx, Pre_Buf);

	Post_Buf[0] = Pre_Buf[0];
	Post_Buf[1] = Pre_Buf[1];
	for (i = 1; i < 31; i++) {
		temp = i << 1;
		Post_Buf[temp] = -Pre_Buf[64 - i];
		Post_Buf[temp + 1] =  Pre_Buf[i + 1];
	}
	Post_Buf[63] =  Pre_Buf[32];
	Post_Buf[62] = -Pre_Buf[33];

	vo_PreMDCTIV(Post_Buf);	      
	Radix4_FFT(Post_Buf);	
	vo_PostMDCTIV(Post_Buf, (Qmf_bands<<1));  

	for (i = 0; i < Qmf_bands; i++) {
		temp = i << 1;
		XBuf[temp] =  Post_Buf[i];
		voGBMask |= ABS(XBuf[temp]);
		XBuf[temp + 1] = -Post_Buf[63 - i];
		voGBMask |= ABS(XBuf[temp + 1]);
	}

	for (    ; i < 64; i++) {
		temp = i << 1;
		XBuf[temp] = 0;
		XBuf[temp + 1] = 0;
	}

	*del_Idx = (*del_Idx == 9? 0 : *del_Idx + 1);

	return voGBMask;
}

/* lose FBITS_LOST_DCT4_64 in DCT4, gain 6 for implicit scaling by 1/64, lose 1 for cTab multiply (Q31) */
#define FBITS_OUT_QMFS	(SBR_IN_QMFS - SBR_LOST_DCT4_64 + 6 - 1)
#define RND_VAL2			(1 << (FBITS_OUT_QMFS-1))

#if (!(defined(_IOS))) && ((defined (__arm) && defined (__ARMCC_VERSION)) || (defined (_WIN32) && defined (_WIN32_WCE) && defined (ARM)) || (defined(__GNUC__) && defined(__arm__)))
#ifdef __cplusplus
extern "C"
#endif
void QMFSynthesisConv(int *cPtr, int *delay, int dIdx, short *outbuf, int channelNum);
#else
void QMFSynthesisConv(int *cPtr, int *delay, int dIdx, short *outbuf, int channelNum)
{
	int k, nOffset0, nOffset1;
	U64 sum64;

	nOffset0 = (dIdx)*128;
	nOffset1 = nOffset0 - 1;
	if (nOffset1 < 0)
		nOffset1 += 1280;

	for (k = 0; k <= 63; k++) {
		sum64.w64 = 0;
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset0]);	
		nOffset0 -= 256; 
		if (nOffset0 < 0) {nOffset0 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset1]);
		nOffset1 -= 256;
	       	if (nOffset1 < 0) {nOffset1 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset0]);
		nOffset0 -= 256;
	       	if (nOffset0 < 0) {nOffset0 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset1]);
		nOffset1 -= 256;
	       	if (nOffset1 < 0) {nOffset1 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset0]);
		nOffset0 -= 256;
	       	if (nOffset0 < 0) {nOffset0 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset1]);
		nOffset1 -= 256; 
		if (nOffset1 < 0) {nOffset1 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset0]);
		nOffset0 -= 256; 
		if (nOffset0 < 0) {nOffset0 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset1]);
		nOffset1 -= 256; 
		if (nOffset1 < 0) {nOffset1 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset0]);
		nOffset0 -= 256;
	       	if (nOffset0 < 0) {nOffset0 += 1280;}
		sum64.w64 = MAC32(sum64.w64, *cPtr++, delay[nOffset1]);
		nOffset1 -= 256;
	       	if (nOffset1 < 0) {nOffset1 += 1280;}

		nOffset0++;
		nOffset1--;
		*outbuf = (short)CLIPTOSHORT((sum64.r.hi32 + RND_VAL2) >> FBITS_OUT_QMFS);
		outbuf += channelNum;
	}
}
#endif

void QMFSynthesis(int *input_buf, 
				  int *del, 
				  int *del_Idx, 
				  int qmfsBands, 
				  short *outbuf, 
				  int channelNum)
{
	int i;
	int Re1, Im1, Re2, Im2;
	int Offset1, Offset2, TempdIdx;
	int *LoBuf, *HiBuf;
	int temp = 128;

	TempdIdx = *del_Idx;
	LoBuf = del + TempdIdx*temp;
	HiBuf = del + TempdIdx*temp + temp - 1;
	for (i = 0; i < qmfsBands >> 1; i++) {
		Re1 = *input_buf++;
		Re2 = *input_buf++;
		Im1 = *input_buf++;
		Im2 = *input_buf++;
		*LoBuf++ = Re1;
		*LoBuf++ = Im1;
		*HiBuf-- = Re2;
		*HiBuf-- = Im2;
	}
	if (qmfsBands & 0x01) {
		Re1 = *input_buf++;
		Re2 = *input_buf++;
		*LoBuf++ = Re1;
		*HiBuf-- = Re2;
		*LoBuf++ = 0;
		*HiBuf-- = 0;
		i++;
	}
	for (     ; i < 32; i++) {
		*LoBuf++ = 0;
		*HiBuf-- = 0;
		*LoBuf++ = 0;
		*HiBuf-- = 0;
	}

	LoBuf = del + TempdIdx * temp;
	vo_PreMDCTIV(LoBuf);
	Radix4_FFT(LoBuf);
	vo_PostMDCTIV(LoBuf, (temp >> 1));

	HiBuf = del + TempdIdx * temp + (temp >> 1);
	vo_PreMDCTIV(HiBuf);
	Radix4_FFT(HiBuf);
	vo_PostMDCTIV(HiBuf, (temp >> 1));

	Offset1 = TempdIdx * temp;
	Offset2 = TempdIdx * temp + (temp >> 1);
	for (i = 32; i != 0; i-=2) {
		Re1 =  (*LoBuf++);
		Im1 =  (*LoBuf++);
		Re2 =  (*HiBuf++);
		Im2 = -(*HiBuf++);
		del[Offset1++] = (Re2 - Re1);
		del[Offset1++] = (Im2 - Im1);
		del[Offset2++] = (Re2 + Re1);
		del[Offset2++] = (Im2 + Im1);

		Re1 =  (*LoBuf++);
		Im1 =  (*LoBuf++);
		Re2 =  (*HiBuf++);
		Im2 = -(*HiBuf++);
		del[Offset1++] = (Re2 - Re1);
		del[Offset1++] = (Im2 - Im1);
		del[Offset2++] = (Re2 + Re1);
		del[Offset2++] = (Im2 + Im1);
	}	

	QMFSynthesisConv((int *)cTabS, del, TempdIdx, outbuf, channelNum);

	*del_Idx = (*del_Idx == NUM_QMF_DELAY_BUFS - 1 ? 0 : *del_Idx + 1);
}

#ifdef PS_DEC
void QMFSynthesisAfterPS(int *input_buf, 
						 int *del, 
						 int *del_Idx, 
						 int qmfsBands, 
						 short *outbuf, 
						 int channelNum)
{
	int i;
	int Re1, Im1, Re2, Im2;
	int OffSet1, OffSet2, TempdIdx;
	int *LoBuf, *HiBuf;
	int temp = 128;

	TempdIdx = *del_Idx;
	LoBuf = del + TempdIdx*temp;
	HiBuf = del + TempdIdx*temp + (temp - 1);

	for(i = 0; i < 32; i+=2)
	{
		Re1 = *input_buf++;
		Re2 = *input_buf++;
		Im1 = *input_buf++;
		Im2 = *input_buf++;
		*LoBuf++ = Re1;
		*LoBuf++ = Im1;
		*HiBuf-- = Re2;
		*HiBuf-- = Im2;

		Re1 = *input_buf++;
		Re2 = *input_buf++;
		Im1 = *input_buf++;
		Im2 = *input_buf++;
		*LoBuf++ = Re1;
		*LoBuf++ = Im1;
		*HiBuf-- = Re2;
		*HiBuf-- = Im2;
	}
	LoBuf = del + TempdIdx*temp;
	vo_PreMDCTIV(LoBuf);
	Radix4_FFT(LoBuf);
	vo_PostMDCTIV(LoBuf, (temp >> 1));

	HiBuf = del + TempdIdx*temp + (temp >> 1);
	vo_PreMDCTIV(HiBuf);
	Radix4_FFT(HiBuf);
	vo_PostMDCTIV(HiBuf, (temp >> 1));

	OffSet1 = TempdIdx*temp;
	OffSet2 = TempdIdx*temp + (temp >> 1);
	for (i = 32; i != 0; i-=2) {
		Re1 =  (*LoBuf++);
		Im1 =  (*LoBuf++);
		Re2 =  (*HiBuf++);
		Im2 = -(*HiBuf++);
		del[OffSet1++] = (Re2 - Re1);
		del[OffSet1++] = (Im2 - Im1);
		del[OffSet2++] = (Re2 + Re1);
		del[OffSet2++] = (Im2 + Im1);

		Re1 =  (*LoBuf++);
		Im1 =  (*LoBuf++);
		Re2 =  (*HiBuf++);
		Im2 = -(*HiBuf++);
		del[OffSet1++] = (Re2 - Re1);
		del[OffSet1++] = (Im2 - Im1);
		del[OffSet2++] = (Re2 + Re1);
		del[OffSet2++] = (Im2 + Im1);
	}

	QMFSynthesisConv((int *)cTabS, del, TempdIdx, outbuf, channelNum);

	*del_Idx = (*del_Idx == NUM_QMF_DELAY_BUFS - 1 ? 0 : *del_Idx + 1);
}
#endif
#endif
