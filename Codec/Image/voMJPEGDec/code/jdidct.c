/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "jcommon.h"
#include "jdecoder.h"

#define SAT2(Value) (VO_U8)(Value < 0 ? 0: (Value > 255 ? 255: Value))
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
	Add = (181 * (((A - B)) >> 8));\
	Bdd = (181 * (((A + B)) >> 8));\
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
	Add = (181 * (((Ad - Bd)) >> 8));\
    Bdd = (181 * (((Ad + Bd)) >> 8));\
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

					temp_data  = SAT2(E);
					temp_data |= (temp_data << 8);
					temp_data |= (temp_data << 16); 
					((VO_U32 *)dst)[0] =
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

					temp_data  =  SAT2(((E + A )>>17))|((SAT2(((E + Bdd )>>17))) << 8)|((SAT2(((E + Add )>>17))) << 16)|((SAT2(((E + B )>>17))) << 24);
					((VO_U32 *)dst)[0] = temp_data;
					temp_data  =  SAT2(((E - B )>>17))|((SAT2(((E - Add )>>17))) << 8)|((SAT2(((E - Bdd )>>17))) << 16)|((SAT2(((E - A )>>17))) << 24);
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

			temp_data  =  SAT2(((Fd + Cd)>>17))|((SAT2(((Hd + Bdd)>>17))) << 8)|((SAT2(((Gd + Add)>>17))) << 16)|((SAT2(((Ed + Dd)>>17))) << 24);
			((VO_U32 *)dst)[0] = temp_data;
			temp_data  =  SAT2(((Ed - Dd)>>17))|((SAT2(((Gd - Add)>>17))) << 8)|((SAT2(((Hd - Bdd)>>17))) << 16)|((SAT2(((Fd - Cd)>>17))) << 24);
			((VO_U32 *)dst)[1] = temp_data;
		}
	}
}


VO_VOID IdctLine8_4x4(VO_S16 *block, VO_U8 *dst, const VO_U8 *src)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, Dd, E, F, G, H;
	VO_S32 Ed, Gd, Add, Bdd, Fd, Hd;
	VO_U32 no_zero_flage;

	no_zero_flage = block[2] | block[3] | block[4] | block[5] | block[6] | block[7];

	if(!no_zero_flage)
	{
		no_zero_flage |= block[1];
		if(!no_zero_flage)
		{
			no_zero_flage |= block[0];
			if(no_zero_flage)
			{		//ip0  
				VO_U32 temp_data;
				E = (block[0] + 32)>>6;
				temp_data  = SAT2(E);
				temp_data |= (temp_data << 8);
				temp_data |= (temp_data << 16); 
				((VO_U32 *)dst)[0] = temp_data;
			}
			else
			{
				((VO_U32 *)dst)[0] = 0;
			}				
		}
		else
		{//ip0, 1
			VO_U32 temp_data;
			CAL_LINE_TWO_DATA()	

			temp_data  =  SAT2(((E + A )>>17))|((SAT2(((E + Bdd )>>17))) << 8)|((SAT2(((E + Add )>>17))) << 16)|((SAT2(((E + B )>>17))) << 24);
			((VO_U32 *)dst)[0] = temp_data;
		}
	}
	else
	{
		VO_U32 temp_data;
		CAL_BUTTERFLY_EIGHT(1)
		E = (block[0] + 32 + block[4]) << 11;
		F = (block[0] + 32 - block[4]) << 11;

		Ed = E - G;
		Fd = E + G;

		Gd = F - H;		
		Hd = F + H;		

		temp_data  =  SAT2(((Fd + Cd)>>17))|((SAT2(((Hd + Bdd)>>17))) << 8)|((SAT2(((Gd + Add)>>17))) << 16)|((SAT2(((Ed + Dd)>>17))) << 24);
		((VO_U32 *)dst)[0] = temp_data;
	}
}

VO_VOID IdctLine8_2x2(VO_S16 *block, VO_U8 *dst, const VO_U8 *src)
{
	VO_S32 A, B, C, D, Ad, Bd, Cd, E, F, G, H;
	VO_S32  Bdd, Fd, Hd;
	VO_U32 no_zero_flage;

	no_zero_flage = block[2] | block[3] | block[4] | block[5] | block[6] | block[7];

	if(!no_zero_flage)
	{
		no_zero_flage |= block[1];
		if(!no_zero_flage)
		{
			no_zero_flage |= block[0];
			if(no_zero_flage)
			{		//ip0  
				VO_U32 temp_data;
				E = (block[0] + 32)>>6;
				temp_data  = SAT2(E);
				temp_data |= (temp_data << 8);
				((VO_U16 *)dst)[0] = (VO_U16)temp_data;
			}
			else
			{
				((VO_U16 *)dst)[0] = 0;
			}				
		}
		else
		{//ip0, 1
			VO_U32 temp_data;
			A = WEIGHT1 * block[1];
			B = WEIGHT7 * block[1];
			E = (block[0] + 32) << 11;
			Bdd = 181 * ((A + B) >> 8);
			
			temp_data  =  SAT2(((E + A )>>17))|((SAT2(((E + Bdd )>>17))) << 8);
			((VO_U16 *)dst)[0] = (VO_U16)temp_data;
		}
	}
	else
	{
		VO_U32 temp_data;
		A = WEIGHT7 * block[7] + WEIGHT1 * block[1];
		B = WEIGHT7 * block[1] - WEIGHT1 * block[7];
		C = WEIGHT3 * block[3] + WEIGHT5 * block[5];
		D = WEIGHT3 * block[5] - WEIGHT5 * block[3];
		G = WEIGHT6 * block[6] + WEIGHT2 * block[2];
		H = WEIGHT6 * block[2] - WEIGHT2 * block[6];
		Ad = A - C;
		Bd = B - D;
		Bdd = (181 * (((Ad + Bd)) >> 8));
		Cd = A + C;
		E = (block[0] + 32 + block[4]) << 11;
		F = (block[0] + 32 - block[4]) << 11;

		Fd = E + G;		
		Hd = F + H;		
		
		temp_data  =  SAT2(((Fd + Cd)>>17))|((SAT2(((Hd + Bdd)>>17))) << 8);
		((VO_U16 *)dst)[0] = (VO_U16)temp_data;
	}
}

VO_VOID  IDCT_Block8x8_c(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	VO_S32 i;

	for(i = 0; i < 8; i++){
		IdctColunm8(block + i); //
	}

	for(i = 0; i < 8; i++){
		IdctLine8(block + i * 8,dest,src); //
		dest+=dest_stride;
		src+=src_stride;
	}
}

void  IDCT_Block8x8_c2(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{//zou
	int i=0;
	for(i = 0; i < 8; i++){
		IdctColunm8(block + i); //
	}

	IdctLine8_4x4(block,dest,src);
	dest+=dest_stride;
	IdctLine8_4x4(block+16,dest,src);
	dest+=dest_stride;
	IdctLine8_4x4(block+32,dest,src);
	dest+=dest_stride;
	IdctLine8_4x4(block+48,dest,src);
	dest+=dest_stride;
}

void  IDCT_Block8x8_c4(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	int i=0;
	for(i = 0; i < 8; i++){
		IdctColunm8(block + i); //
	}

	IdctLine8_2x2(block,dest,src);
	dest+=dest_stride;
	IdctLine8_2x2(block+32,dest,src);
	dest+=dest_stride;
}

void  IDCT_Block8x8_c8(VO_S16 *block, VO_U8 *dest, VO_S32 dest_stride, const VO_U8 *src, VO_S32 src_stride)
{
	dest[0] = SAT2((block[0] + 4)>>3);
}






















	