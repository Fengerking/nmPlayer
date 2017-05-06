/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voMpegIdct.h"


#define WEIGHT1 0xb19                 // 2048*sqrt(2)*cos(1*pi/16) 
#define WEIGHT2 0xa74                 // 2048*sqrt(2)*cos(2*pi/16) 
#define WEIGHT3 0x968                 // 2048*sqrt(2)*cos(3*pi/16) 
#define WEIGHT5 0x649                 // 2048*sqrt(2)*cos(5*pi/16) 
#define WEIGHT6 0x454                 // 2048*sqrt(2)*cos(6*pi/16) 
#define WEIGHT7 0x235                 // 2048*sqrt(2)*cos(7*pi/16) 

#define WRITE_COL_CONSTANT(a){\
	block[0*8] =\
	block[7*8] =\
	block[1*8] =\
	block[6*8] =\
	block[3*8] =\
	block[4*8] =\
	block[2*8] =\
	block[5*8] = (VO_S16)(a);\
}

#define WRITE_COL_DATA(Fd,Cd,Hd,Bdd,Ed,Dd,Gd,Add){\
	block[0*8] = (VO_S16)((Fd + Cd) >> 8);\
	block[7*8] = (VO_S16)((Fd - Cd) >> 8);\
	block[1*8] = (VO_S16)((Hd + Bdd) >> 8);\
	block[6*8] = (VO_S16)((Hd - Bdd) >> 8);\
	block[3*8] = (VO_S16)((Ed + Dd) >> 8);\
	block[4*8] = (VO_S16)((Ed - Dd) >> 8);\
	block[2*8] = (VO_S16)((Gd + Add) >> 8);\
	block[5*8] = (VO_S16)((Gd - Add) >> 8);\
}

#define CAL_COL_TWO_DATA(){\
	E = block[0*8] << 11;\
	E += 128;\
	A = WEIGHT1 * block[1*8];\
	B = WEIGHT7 * block[1*8];\
	Add = 181 * ((A - B) >> 8);\
	Bdd = 181 * ((A + B) >> 8);\
}

#define CAL_LINE_TWO_DATA(){\
	A = WEIGHT1 * block[1];\
	B = WEIGHT7 * block[1];\
	E = (block[0] + 32) << 11;\
	Add = 181 * ((A - B) >> 8);\
	Bdd = 181 * ((A + B) >> 8);\
}

#define CAL_BUTTERFLY_EIGHT(a){\
	A = WEIGHT7 * block[7*a] + WEIGHT1 * block[1*a];\
	B = WEIGHT7 * block[1*a] - WEIGHT1 * block[7*a];\
	C = WEIGHT3 * block[3*a] + WEIGHT5 * block[5*a];\
	D = WEIGHT3 * block[5*a] - WEIGHT5 * block[3*a];\
	G = WEIGHT6 * block[6*a] + WEIGHT2 * block[2*a];\
	H = WEIGHT6 * block[2*a] - WEIGHT2 * block[6*a];\
	Ad = A - C;\
	Bd = B - D;\
	Add = 181 * ((Ad - Bd) >> 8);\
	Bdd = 181 * ((Ad + Bd) >> 8);\
	Cd = A + C;\
	Dd = B + D;\
}

#define CAL_BUTTERFLY_FOUR(a){\
	A = WEIGHT1 * block[1*a];\
	B = WEIGHT7 * block[1*a];\
	C = WEIGHT3 * block[3*a];\
	D = - WEIGHT5 * block[3*a];\
	G = WEIGHT2 * block[2*a];\
	H = WEIGHT6 * block[2*a];\
	Ad = A - C;\
	Bd = B - D;\
	Add = (181 * (((Ad - Bd)) >> 8));\
	Bdd = (181 * (((Ad + Bd)) >> 8));\
	Cd = A + C;\
	Dd = B + D;\
}

VO_VOID IdctColunm8(VO_S16 *block)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	VO_U32 no_zero_flage;

	no_zero_flage = block[2*8] | block[3*8] | block[4*8] | block[5*8] | block[6*8] | block[7*8];

	if(!no_zero_flage){

		no_zero_flage |= block[1*8];
		
		if(!no_zero_flage){
			no_zero_flage |= block[0*8];
			
			if(no_zero_flage){//ip0
				WRITE_COL_CONSTANT(block[0*8]<< 3)		
			}
		}else{//ip0, 1
			CAL_COL_TWO_DATA()
			WRITE_COL_DATA(E,A,E,Bdd,E,B,E,Add)		
		}
	}else{
		CAL_BUTTERFLY_EIGHT(8)
		E = (block[0*8] + block[4*8]) << 11;
		F = (block[0*8] - block[4*8]) << 11;

		E += 128;
		F += 128;

		Ed = E - G;
		Fd = E + G;

		Gd = F - H;		
		Hd = F + H;

		WRITE_COL_DATA(Fd,Cd,Hd,Bdd,Ed,Dd,Gd,Add)
	}
}

VO_VOID IdctColunm4(VO_S16 *block)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	VO_U32 no_zero_flage;

	no_zero_flage = block[2*8] | block[3*8];

	if(!no_zero_flage){
		no_zero_flage |= block[1*8];
		if(!no_zero_flage){
			no_zero_flage |= block[0*8];
			if(no_zero_flage){	//ip0   					
				WRITE_COL_CONSTANT(block[0*8]<< 3)		
			}
		}else{//ip0, 1    		
			CAL_COL_TWO_DATA()
			WRITE_COL_DATA(E,A,E,Bdd,E,B,E,Add)		
		}
	}else{
		CAL_BUTTERFLY_FOUR(8)
		E = block[0*8] << 11;
		F = block[0*8] << 11;

		E += 128;
		F += 128;	

		Ed = E - G;
		Fd = E + G;

		Gd = F - H;		
		Hd = F + H;

		WRITE_COL_DATA(Fd,Cd,Hd,Bdd,Ed,Dd,Gd,Add)
	}
}

VO_VOID IdctLine8(VO_S16 *block, VO_U8 *dst, const VO_U8 *src)
{
    VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
    VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	VO_U32 no_zero_flage;

	no_zero_flage = block[2] | block[3] | block[4] | block[5] | block[6] | block[7];
    
    	if(!no_zero_flage){

			no_zero_flage |= block[1];
    		
    		if(!no_zero_flage){
				no_zero_flage |= block[0];
    			
    			if(no_zero_flage){		//ip0   					
					E = (block[0] + 32)>>6;

					if(!src){
						VO_U32 temp_data;

						temp_data  = SAT(E);
						temp_data |= (temp_data << 8);
						temp_data |= (temp_data << 16); 
						((VO_U32 *)dst)[0] =
						((VO_U32 *)dst)[1] = temp_data;
					}else{
						VO_U32 temp_data;

						temp_data  =  SAT((src[0] + E))|(SAT((src[1] + E)) << 8)|(SAT((src[2] + E)) << 16)|(SAT((src[3] + E)) << 24);
						((VO_U32 *)dst)[0] = temp_data;
						temp_data  =  SAT((src[4] + E))|(SAT((src[5] + E)) << 8)|(SAT((src[6] + E)) << 16)|(SAT((src[7] + E)) << 24);
						((VO_U32 *)dst)[1] = temp_data;
					}
    			}else{
					if(!src){
						((VO_U32 *)dst)[0] =
						((VO_U32 *)dst)[1] = 0;
					}else{
						((VO_U32 *)dst)[0] = ((VO_U32 *)src)[0];
						((VO_U32 *)dst)[1] = ((VO_U32 *)src)[1];
					}
    			}				
    		}else{//ip0, 1
				CAL_LINE_TWO_DATA()

				if(!src){
					VO_U32 temp_data;

					temp_data  =  SAT(((E + A )>>17))|((SAT(((E + Bdd )>>17))) << 8)|((SAT(((E + Add )>>17))) << 16)|((SAT(((E + B )>>17))) << 24);
					((VO_U32 *)dst)[0] = temp_data;
					temp_data  =  SAT(((E - B )>>17))|((SAT(((E - Add )>>17))) << 8)|((SAT(((E - Bdd )>>17))) << 16)|((SAT(((E - A )>>17))) << 24);
					((VO_U32 *)dst)[1] = temp_data;
				}else{
					VO_U32 temp_data;

					temp_data  =  SAT(((src[0] + ((E + A )>>17))))|((SAT(((src[1] + ((E + Bdd )>>17))))) << 8)|
						((SAT(((src[2] + ((E + Add )>>17))))) << 16)|((SAT(((src[3] + ((E + B )>>17))))) << 24);
					((VO_U32 *)dst)[0] = temp_data;
					temp_data  =  SAT(((src[4] + ((E - B )>>17))))|((SAT(((src[5] + ((E - Add )>>17))))) << 8)|
						((SAT(((src[6] + ((E - Bdd )>>17))))) << 16)|((SAT(((src[7] + ((E - A )>>17))))) << 24);
					((VO_U32 *)dst)[1] = temp_data;
				}		
    		}
    	}else{
			CAL_BUTTERFLY_EIGHT(1)
			E = (block[0] + 32 + block[4]) << 11;
			F = (block[0] + 32 - block[4]) << 11;

			Ed = E - G;
			Fd = E + G;

			Gd = F - H;		
			Hd = F + H;
			
			if(!src){
				VO_U32 temp_data;

				temp_data  =  SAT(((Fd + Cd)>>17))|((SAT(((Hd + Bdd)>>17))) << 8)|((SAT(((Gd + Add)>>17))) << 16)|((SAT(((Ed + Dd)>>17))) << 24);
				((VO_U32 *)dst)[0] = temp_data;
				temp_data  =  SAT(((Ed - Dd)>>17))|((SAT(((Gd - Add)>>17))) << 8)|((SAT(((Hd - Bdd)>>17))) << 16)|((SAT(((Fd - Cd)>>17))) << 24);
				((VO_U32 *)dst)[1] = temp_data;
			}else{
				VO_U32 temp_data;

				temp_data  =  SAT(((src[0] + ((Fd + Cd )>>17))))|((SAT(((src[1] + ((Hd + Bdd )>>17))))) << 8)|
					((SAT(((src[2] + ((Gd + Add )>>17))))) << 16)|((SAT(((src[3] + ((Ed + Dd )>>17))))) << 24);
				((VO_U32 *)dst)[0] = temp_data;
				temp_data  =  SAT(((src[4] + ((Ed - Dd )>>17))))|((SAT(((src[5] + ((Gd - Add )>>17))))) << 8)|
					((SAT(((src[6] + ((Hd - Bdd )>>17))))) << 16)|((SAT(((src[7] + ((Fd - Cd )>>17))))) << 24);
				((VO_U32 *)dst)[1] = temp_data;
			}
		}
}

VO_VOID IdctLine4(VO_S16 *block, VO_U8 *dst, const VO_U8 *src)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	
	if(!(block[2] | block[3])){		
		if(!block[1]){		
			if(block[0]){//ip0				
				E = (block[0] + 32)>>6;
				if(!src){
					VO_U32 temp_data;

					temp_data  = SAT(E);
					temp_data |= (temp_data << 8);
					temp_data |= (temp_data << 16); 
					((VO_U32 *)dst)[0] =
					((VO_U32 *)dst)[1] = temp_data;
				}else{
					VO_U32 temp_data;

					temp_data  =  SAT((src[0] + E))|(SAT((src[1] + E)) << 8)|(SAT((src[2] + E)) << 16)|(SAT((src[3] + E)) << 24);
					((VO_U32 *)dst)[0] = temp_data;
					temp_data  =  SAT((src[4] + E))|(SAT((src[5] + E)) << 8)|(SAT((src[6] + E)) << 16)|(SAT((src[7] + E)) << 24);
					((VO_U32 *)dst)[1] = temp_data;
				}
			}else{
				if(!src){
					((VO_U32 *)dst)[0] =
					((VO_U32 *)dst)[1] = 0;
				}else{
					((VO_U32 *)dst)[0] = ((VO_U32 *)src)[0];
					((VO_U32 *)dst)[1] = ((VO_U32 *)src)[1];
				}
			}				
		}else{//ip0, 1

			CAL_LINE_TWO_DATA()

			if(!src){
				VO_U32 temp_data;

				temp_data  =  SAT(((E + A )>>17))|((SAT(((E + Bdd )>>17))) << 8)|((SAT(((E + Add )>>17))) << 16)|((SAT(((E + B )>>17))) << 24);
				((VO_U32 *)dst)[0] = temp_data;
				temp_data  =  SAT(((E - B )>>17))|((SAT(((E - Add )>>17))) << 8)|((SAT(((E - Bdd )>>17))) << 16)|((SAT(((E - A )>>17))) << 24);
				((VO_U32 *)dst)[1] = temp_data;
			}else{
				VO_U32 temp_data;

				temp_data  =  SAT(((src[0] + ((E + A )>>17))))|((SAT(((src[1] + ((E + Bdd )>>17))))) << 8)|
					((SAT(((src[2] + ((E + Add )>>17))))) << 16)|((SAT(((src[3] + ((E + B )>>17))))) << 24);
				((VO_U32 *)dst)[0] = temp_data;
				temp_data  =  SAT(((src[4] + ((E - B )>>17))))|((SAT(((src[5] + ((E - Add )>>17))))) << 8)|
					((SAT(((src[6] + ((E - Bdd )>>17))))) << 16)|((SAT(((src[7] + ((E - A )>>17))))) << 24);
				((VO_U32 *)dst)[1] = temp_data;
			}		
		}
	}else{
		CAL_BUTTERFLY_FOUR(1)
		E = (block[0] + 32) << 11;
		F = (block[0] + 32) << 11;

		Ed = E - G;
		Fd = E + G;

		Gd = F - H;		
		Hd = F + H;

		if(!src){
			VO_U32 temp_data;

			temp_data  =  SAT(((Fd + Cd)>>17))|((SAT(((Hd + Bdd)>>17))) << 8)|((SAT(((Gd + Add)>>17))) << 16)|((SAT(((Ed + Dd)>>17))) << 24);
			((VO_U32 *)dst)[0] = temp_data;
			temp_data  =  SAT(((Ed - Dd)>>17))|((SAT(((Gd - Add)>>17))) << 8)|((SAT(((Hd - Bdd)>>17))) << 16)|((SAT(((Fd - Cd)>>17))) << 24);
			((VO_U32 *)dst)[1] = temp_data;
		}else{
			VO_U32 temp_data;

			temp_data  =  SAT(((src[0] + ((Fd + Cd )>>17))))|((SAT(((src[1] + ((Hd + Bdd )>>17))))) << 8)|
				((SAT(((src[2] + ((Gd + Add )>>17))))) << 16)|((SAT(((src[3] + ((Ed + Dd )>>17))))) << 24);
			((VO_U32 *)dst)[0] = temp_data;
			temp_data  =  SAT(((src[4] + ((Ed - Dd )>>17))))|((SAT(((src[5] + ((Gd - Add )>>17))))) << 8)|
				((SAT(((src[6] + ((Hd - Bdd )>>17))))) << 16)|((SAT(((src[7] + ((Fd - Cd )>>17))))) << 24);
			((VO_U32 *)dst)[1] = temp_data;
		}
	}
}

// VO_VOID  IdctBlock1x1_C(VO_S32 out_data, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
// {
//    VO_S32 loop;
// // 	VO_S32 out_data = (block[0]+4)>>3;
// 
// 	if(!src){
// 		out_data = SAT(out_data);
// 
// 		for ( loop=0; loop<8; loop++ ){	
// 			VO_U32 temp_data;
// 
// 			temp_data  = (VO_U8)out_data;
// 			temp_data |= (temp_data << 8);
// 			temp_data |= (temp_data << 16); 
// 			((VO_U32 *)dest)[0] =
// 			((VO_U32 *)dest)[1] = temp_data;
// 			dest += dest_stride;	
// 		}
// 	}else{
// 	    for ( loop=0; loop<8; loop++ ){	
// 			VO_U32 temp_data;
// 
// 			temp_data  =  SAT((src[0] + out_data))|(SAT((src[1] + out_data)) << 8)|(SAT((src[2] + out_data)) << 16)|(SAT((src[3] + out_data)) << 24);
// 			((VO_U32 *)dest)[0] = temp_data;
// 			temp_data  =  SAT((src[4] + out_data))|(SAT((src[5] + out_data)) << 8)|(SAT((src[6] + out_data)) << 16)|(SAT((src[7] + out_data)) << 24);
// 			((VO_U32 *)dest)[1] = temp_data;
// 
// 			src += src_stride;
// 			dest += dest_stride;	
// 		}	
// 	}
// // 	block[0] = block[63] = 0;
// }

#define ADDSAT32(a,Dst,Add32)		\
	b = a + Add32;					\
	c = a & Add32;					\
	a ^= Add32;						\
	a &= ~b;						\
	a |= c;							\
	a &= MaskCarry;					\
	c = a << 1;						\
	b -= c;	/* adjust neighbour */	\
	b |= c - (a >> 7); /* mask */	\
	Dst = b;						

#define SUBSAT32(a,Dst,Add32)		\
	a = ~a;							\
	b = a + Add32;					\
	c = a & Add32;					\
	a ^= Add32;						\
	a &= ~b;						\
	a |= c;							\
	a &= MaskCarry;					\
	c = a << 1;						\
	b -= c;	/* adjust neighbour */	\
	b |= c - (a >> 7); /* mask */	\
	Dst = ~b;

VO_VOID  IdctBlock1x1_C(VO_S32 v, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	const VO_U8* SrcEnd = src + 8 * src_stride;
	VO_U32 MaskCarry = 0x80808080U;
	VO_U32 a,b,c,d;

	if (v>0)
	{
		v |= v << 8;
		v |= v << 16;

		do
		{
			a = ((VO_U32*)src)[0];
			d = ((VO_U32*)src)[1];
			ADDSAT32(a,((VO_U32*)dest)[0],v);
			ADDSAT32(d,((VO_U32*)dest)[1],v);
			dest += dest_stride;
			src  += src_stride;
		}
		while (src != SrcEnd);
	}
	else
	if (v<0)
	{
		v = -v;
		v |= v << 8;
		v |= v << 16;

		do
		{
			a = ((VO_U32*)src)[0];
			d = ((VO_U32*)src)[1];
			SUBSAT32(a,((VO_U32*)dest)[0],v);
			SUBSAT32(d,((VO_U32*)dest)[1],v);
			dest += dest_stride;
			src  += src_stride;
		}
		while (src != SrcEnd);
	}
	else
		CopyBlock8x8((VO_U8 *)src,dest,src_stride,dest_stride, 8);
}

VO_VOID  IdctBlock8x8_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 *block_ptr=(VO_S32*)block;
	VO_S32 i;

	for(i = 0; i < 8; i++){
		IdctColunm8(block + i);
	}

	for(i = 0; i < 8; i++){
		IdctLine8(block + i * 8,dest,src);
		dest+=dest_stride;
		src+=src_stride;
	}

	
	block_ptr[0] = block_ptr[1] = block_ptr[2] = block_ptr[3] = 
	block_ptr[4] = block_ptr[5] = block_ptr[6] = block_ptr[7] = 
	block_ptr[8] = block_ptr[9] = block_ptr[10] = block_ptr[11] = 
	block_ptr[12] = block_ptr[13] = block_ptr[14] = block_ptr[15] = 
	block_ptr[16] = block_ptr[17] = block_ptr[18] = block_ptr[19] = 
	block_ptr[20] = block_ptr[21] = block_ptr[22] = block_ptr[23] = 
	block_ptr[24] = block_ptr[25] = block_ptr[26] = block_ptr[27] = 
	block_ptr[28] = block_ptr[29] = block_ptr[30] = block_ptr[31] = 0;
}

VO_VOID  IdctBlock4x8_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 *block_ptr=(VO_S32*)block;
	VO_S32 i;

	for(i = 0; i < 4; i++){
		IdctColunm8(block + i);
	}

	for(i = 0; i < 8; i++){
		IdctLine4(block + i * 8,dest,src);
		dest+=dest_stride;
		src+=src_stride;
	}

	block_ptr[0] = block_ptr[1] =
	block_ptr[4] = block_ptr[5] =
	block_ptr[8] = block_ptr[9] =
	block_ptr[12] = block_ptr[13] =
	block_ptr[16] = block_ptr[17] =
	block_ptr[20] = block_ptr[21] =
	block_ptr[24] = block_ptr[25] =
	block_ptr[28] = block_ptr[29] = 0;
	block_ptr[31] = 0;
}
VO_VOID  IdctBlock4x4_C(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 *block_ptr=(VO_S32*)block;
	VO_S32 i;

	for(i = 0; i < 4; i++){
		IdctColunm4(block + i);
	}

	for(i = 0; i < 8; i++){
		IdctLine4(block + i * 8,dest,src);
		dest+=dest_stride;
		src+=src_stride;
	}

	block_ptr[0] = block_ptr[1] =
	block_ptr[4] = block_ptr[5] =
	block_ptr[8] = block_ptr[9] =
	block_ptr[12] = block_ptr[13] =
	block_ptr[16] = block_ptr[17] =
	block_ptr[20] = block_ptr[21] =
	block_ptr[24] = block_ptr[25] =
	block_ptr[28] = block_ptr[29] = 0;
	block_ptr[31] = 0;
}



static VO_VOID Bit16_IDCT_Row8(VO_S16 *block, VO_S16 *dst, const VO_U8 *src)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	VO_U32 no_zero_flage;
	VO_U32 temp_data;

	no_zero_flage = block[2] | block[3] | block[4] | block[5] | block[6] | block[7];

	if(!no_zero_flage){

		no_zero_flage |= block[1];

		if(!no_zero_flage){
			E = (block[0] + 32)>>6;
			temp_data  = E & 0xFFFF;
			temp_data |= (temp_data << 16);
			((VO_U32 *)dst)[0] =
			((VO_U32 *)dst)[1] = 
			((VO_U32 *)dst)[2] =
			((VO_U32 *)dst)[3] = temp_data;
		}else{//ip0, 1
			CAL_LINE_TWO_DATA()
			temp_data  = SAT((E + A )>>17)|((SAT((E + Bdd )>>17)) << 16);
			((VO_U32 *)dst)[0] = temp_data;
			temp_data  = SAT((E + Add )>>17)|((SAT((E + B )>>17)) << 16);
			((VO_U32 *)dst)[1] = temp_data;
			temp_data  =  SAT((E - B )>>17)|((SAT((E - Add )>>17)) << 16);
			((VO_U32 *)dst)[2] = temp_data;
			temp_data  = SAT((E - Bdd )>>17)|((SAT((E - A )>>17)) << 16);
			((VO_U32 *)dst)[3] = temp_data;
		}
	}else{
		CAL_BUTTERFLY_EIGHT(1)
		E = (block[0] + 32 + block[4]) << 11;
		F = (block[0] + 32 - block[4]) << 11;

		Ed = E - G;
		Fd = E + G;

		Gd = F - H;		
		Hd = F + H;

		temp_data  =  SAT((Fd + Cd)>>17)|((SAT((Hd + Bdd)>>17)) << 16);
		((VO_U32 *)dst)[0] = temp_data;
		temp_data  =  SAT((Gd + Add)>>17)|((SAT((Ed + Dd)>>17)) << 16);
		((VO_U32 *)dst)[1] = temp_data;
		temp_data  =  SAT((Ed - Dd)>>17)|((SAT((Gd - Add)>>17)) << 16);
		((VO_U32 *)dst)[2] = temp_data;
		temp_data  =  SAT((Hd - Bdd)>>17)|((SAT((Fd - Cd)>>17)) << 16);
		((VO_U32 *)dst)[3] = temp_data;
	}
}

VO_VOID  Bit16_IDCT_Block8x8_c(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride)
{
	IdctColunm8(Block);
	IdctColunm8(Block+1);
	IdctColunm8(Block+2);
	IdctColunm8(Block+3);
	IdctColunm8(Block+4);
	IdctColunm8(Block+5);
	IdctColunm8(Block+6);
	IdctColunm8(Block+7);

	Bit16_IDCT_Row8(Block,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+8,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+16,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+24,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+32,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+40,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+48,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row8(Block+56,Dest,Src);
}

static VO_VOID Bit16_IDCT_Row4(VO_S16 *block, VO_S16 *dst, const VO_U8 *src)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	VO_U32 temp_data;

	if(!(block[2] | block[3])){		
		if(!block[1]){		
			E = (block[0] + 32)>>6;

			temp_data  = E & 0xFFFF;
			temp_data |= (temp_data << 16); 
			((VO_U32 *)dst)[0] =
			((VO_U32 *)dst)[1] =
			((VO_U32 *)dst)[2] =
			((VO_U32 *)dst)[3] = temp_data;
		}else{//ip0, 1
			CAL_LINE_TWO_DATA()

			temp_data  =  SAT((E + A )>>17)|((SAT((E + Bdd )>>17)) << 16);
			((VO_U32 *)dst)[0] = temp_data;
			temp_data  =  SAT((E + Add )>>17)|((SAT((E + B )>>17)) << 16);
			((VO_U32 *)dst)[1] = temp_data;
			temp_data  =  SAT((E - B )>>17)|((SAT((E - Add )>>17)) << 16);
			((VO_U32 *)dst)[2] = temp_data;
			temp_data  =  SAT((E - Bdd )>>17)|((SAT((E - A )>>17)) << 16);
			((VO_U32 *)dst)[3] = temp_data;
		}
	}else{
		CAL_BUTTERFLY_FOUR(1)
		E = (block[0] + 32) << 11;
		F = (block[0] + 32) << 11;

		Ed = E - G;
		Fd = E + G;

		Gd = F - H;		
		Hd = F + H;

		temp_data  =  SAT((Fd + Cd)>>17)|((SAT((Hd + Bdd)>>17)) << 16);
		((VO_U32 *)dst)[0] = temp_data;
		temp_data  =  SAT((Gd + Add)>>17)|((SAT((Ed + Dd)>>17)) << 16);
		((VO_U32 *)dst)[1] = temp_data;
		temp_data  =  SAT((Ed - Dd)>>17)|((SAT((Gd - Add)>>17)) << 16);
		((VO_U32 *)dst)[2] = temp_data;
		temp_data  =  SAT((Hd - Bdd)>>17)|((SAT((Fd - Cd)>>17)) << 16);
		((VO_U32 *)dst)[3] = temp_data;
	}
}


VO_VOID  Bit16_IDCT_Block4x8_c(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride)
{
	IdctColunm8(Block);
	IdctColunm8(Block+1);
	IdctColunm8(Block+2);
	IdctColunm8(Block+3);

	Bit16_IDCT_Row4(Block,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+8,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+16,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+24,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+32,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+40,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+48,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+56,Dest,Src);
}

VO_VOID  Bit16_IDCT_Block4x4_Block8x8_c(VO_S16 *Block, VO_S16 *Dest, VO_S32 DestStride, const VO_U8 *Src, VO_S32 SrcStride)
{
	IdctColunm4(Block);
	IdctColunm4(Block+1);
	IdctColunm4(Block+2);
	IdctColunm4(Block+3);

	Bit16_IDCT_Row4(Block,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+8,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+16,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+24,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+32,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+40,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+48,Dest,Src);
	Dest+=DestStride;
	Src+=SrcStride;
	Bit16_IDCT_Row4(Block+56,Dest,Src);
}

VO_VOID Bit16_IDCT_Const8x8_c(VO_S32 v, VO_S16 * Dst, VO_S32 DstPitch, VO_U8 * Src, VO_S32 SrcPitch)
{
	VO_U32 i;
	VO_U32 x0;

	x0  = v & 0xffff;
	x0 |= x0 << 16;

	for(i = 0; i < 8; i++){
		((VO_U32*)Dst)[0] = x0;
		((VO_U32*)Dst)[1] = x0;
		((VO_U32*)Dst)[2] = x0;
		((VO_U32*)Dst)[3] = x0;
		Dst += DstPitch;
	}
}



