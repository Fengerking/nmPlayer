#include "stdio.h"
#include "PackUV.h"

/*
// R = 1.164 * (y - 16) + 1.596 * (v - 128);
// G = 1.164 * (y - 16) - 0.813 * (v - 128) - 0.392 * (u - 128);
// B = 1.164 * (y - 16) + 2.017 * (u - 128);
// 8-bit (RGB24/32) ranges from -277 to 537; use -384 to 639 array to clip [0, 255];

#define ConstV1 1673527
#define ConstV2 852492
#define ConstU1 2114977
#define ConstU2 411041
#define ConstY	1220542

	a0 = psrc_u[num] - 128;
	a1 = psrc_v[num] - 128;
	a3 = (psrc_y[2*num] - 16)*ConstY;
	a2 = (a0 * ConstU1);
	a0 = (a1 * ConstV2 + a0 *ConstU2);
	a1 = (a1 * ConstV1);

	a4  = (ccClip63[((a3 + a1)>>22)])>>1;
	a5  = (ccClip63[((a3 - a0)>>22)]);
	a3  = (ccClip63[((a3 + a2)>>22)])>>1;
	a4 = (a4<<11)|(a5<<5)|a3;
*/
#define ConstV1 (0x198937>>8) //1673527
#define ConstV2 (0xD020C>>8)  //852492
#define ConstU1 (0x2045A1>>8) //2114977
#define ConstU2 (0x645A1>>8)  //411041
#define ConstY	(0x129FBE>>8) //1220542
//
//#define ConstV1 1673527
//#define ConstV2 852492
//#define ConstU1 2114977
//#define ConstU2 411041
//#define ConstY	1220542

void cc_yuv420torgb24_c_16bit_old(unsigned char *psrc_y, unsigned char *psrc_u, unsigned char *psrc_v, const signed long in_stridey, const signed long in_strideu, const signed long in_stridev, 
				        unsigned char *out_buf, const signed long out_stride, signed long width, signed long height)
{
	signed long a0, a1, a2, a3, a4, a5, a7, a33, a77;
	unsigned long i, j;
	for(i = 0; i < height; i += 2){

		for(j = 0; j < width; j += 2){
			a0 = psrc_u[0] - 128;
			a1 = psrc_v[0] - 128;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a3 = (psrc_y[0] - 16)*ConstY;
			a33 = (psrc_y[1] - 16)*ConstY;

			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;

			a4  = SAT((a3 + a1)>>12);
			a5  = SAT((a3 - a0)>>12);
			a3  = SAT((a3 + a2)>>12);
			*(out_buf) = a3;
			*(out_buf+1) = a5;
			*(out_buf+2) = a4;

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+3) = a33;
			*(out_buf+4) = a5;
			*(out_buf+5) = a4;

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+0+out_stride) = a7;
			*(out_buf+1+out_stride) = a5;
			*(out_buf+2+out_stride) = a4;

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+3+out_stride) = a77;
			*(out_buf+4+out_stride) = a5;
			*(out_buf+5+out_stride) = a4;

			out_buf += 6;
			psrc_y += 2;
			psrc_u++;
			psrc_v++;

		}
		psrc_y -= width;
		psrc_y += (in_stridey<<1);

		psrc_u -= (width>>1);
		psrc_u += in_strideu;

		psrc_v -= (width>>1);
		psrc_v += in_stridev;

		out_buf -= (width*3);
		out_buf += (out_stride<<1);
	}
}
void cc_yuv420torgb24_c_16bit(unsigned char *psrc_y, unsigned char *psrc_u, unsigned char *psrc_v, const signed long in_stridey, const signed long in_strideu, const signed long in_stridev, 
				        unsigned char *out_buf, const signed long out_stride, signed long width, signed long height)
{
	signed long a0, a1, a2, a3, a4, a5, a7, a33, a77;
	unsigned long i, j;
	for(i = 0; i < height; i += 2){

		for(j = 0; j < width; j += 2){
			a0 = psrc_u[0] - 128;
			a1 = psrc_v[0] - 128;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a3 = (psrc_y[0] - 16)*ConstY;
			a33 = (psrc_y[1] - 16)*ConstY;

			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;

			a4  = SAT((a3 + a1)>>12);
			a5  = SAT((a3 - a0)>>12);
			a3  = SAT((a3 + a2)>>12);
			*(out_buf) = a4;//a3;
			*(out_buf+1) = a5;//a5;
			*(out_buf+2) = a3;//a4;
			*(out_buf+3) = 255;  //zou
			

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+4) = a4;//a33;
			*(out_buf+5) = a5;//a5;
			*(out_buf+6) = a33;//a4;
			*(out_buf+7) = 255;  //zou
			

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+0+out_stride) = a4;//a7;
			*(out_buf+1+out_stride) = a5;//a5;
			*(out_buf+2+out_stride) = a7;//a4;
			*(out_buf+3+out_stride) = 255;
			

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+4+out_stride) = a4;//a77;
			*(out_buf+5+out_stride) = a5;//a5;
			*(out_buf+6+out_stride) = a77;//a4;
			*(out_buf+7+out_stride) = 255;			

			out_buf += 8;
			psrc_y += 2;
			psrc_u++;
			psrc_v++;

		}
		psrc_y -= width;
		psrc_y += (in_stridey<<1);

		psrc_u -= (width>>1);
		psrc_u += in_strideu;

		psrc_v -= (width>>1);
		psrc_v += in_stridev;

		out_buf -= (width*4);
		out_buf += (out_stride<<1);
	}
	/*if(0){
		FILE *fp =fopen("/data/local/log.txt","a");
		fprintf(fp,"yuv420torgb24 %d %d\n",i,j);
		fclose(fp);
	}*/
}
//void cc_yuv420torgb24_c_32bit(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, const int32_t in_stridey, const int32_t in_strideu, const int32_t in_stridev, 
//				        uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height)
//{
//	int32_t a0, a1, a2, a3, a4, a5, a7, a33, a77;
//	uint32_t i, j;
//	for(i = 0; i < height; i += 2){
//
//		for(j = 0; j < width; j += 2){
//			a0 = psrc_u[0] - 128;
//			a1 = psrc_v[0] - 128;
//			a2 = (a0 * ConstU1);
//			a0 = (a1 * ConstV2 + a0 *ConstU2);
//			a1 = (a1 * ConstV1);
//
//			a3 = (psrc_y[0] - 16)*ConstY;
//			a33 = (psrc_y[1] - 16)*ConstY;
//
//			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
//			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;
//
//			a4  = SAT((a3 + a1)>>20);
//			a5  = SAT((a3 - a0)>>20);
//			a3  = SAT((a3 + a2)>>20);
//			*(out_buf) = a3;
//			*(out_buf+1) = a5;
//			*(out_buf+2) = a4;
//
//			a4  = SAT((a33 + a1)>>20);
//			a5  = SAT((a33 - a0)>>20);
//			a33  = SAT((a33 + a2)>>20);
//			*(out_buf+3) = a33;
//			*(out_buf+4) = a5;
//			*(out_buf+5) = a4;
//
//			a4  = SAT((a7 + a1)>>20);
//			a5  = SAT((a7 - a0)>>20);
//			a7  = SAT((a7 + a2)>>20);
//			*(out_buf+0+out_stride) = a7;
//			*(out_buf+1+out_stride) = a5;
//			*(out_buf+2+out_stride) = a4;
//
//			a4  = SAT((a77 + a1)>>20);
//			a5  = SAT((a77 - a0)>>20);
//			a77  = SAT((a77 + a2)>>20);
//			*(out_buf+3+out_stride) = a77;
//			*(out_buf+4+out_stride) = a5;
//			*(out_buf+5+out_stride) = a4;
//
//			out_buf += 6;
//			psrc_y += 2;
//			psrc_u++;
//			psrc_v++;
//
//		}
//		psrc_y -= width;
//		psrc_y += (in_stridey<<1);
//
//		psrc_u -= (width>>1);
//		psrc_u += in_strideu;
//
//		psrc_v -= (width>>1);
//		psrc_v += in_stridev;
//
//		out_buf -= (width*3);
//		out_buf += (out_stride<<1);
//	}
//}
//
//void cc_yuv420torgb32_c_16bit(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, const int32_t in_stridey, const int32_t in_strideu, const int32_t in_stridev, 
//				        uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height)
//{
//	int32_t a0, a1, a2, a3, a4, a5, a7, a33, a77;
//	uint32_t i, j;
//	for(i = 0; i < height; i += 2){
//
//		for(j = 0; j < width; j += 2){
//			a0 = psrc_u[0] - 128;
//			a1 = psrc_v[0] - 128;
//			a2 = (a0 * ConstU1);
//			a0 = (a1 * ConstV2 + a0 *ConstU2);
//			a1 = (a1 * ConstV1);
//
//			a3 = (psrc_y[0] - 16)*ConstY;
//			a33 = (psrc_y[1] - 16)*ConstY;
//
//			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
//			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;
//
//			a4  = SAT((a3 + a1)>>12);
//			a5  = SAT((a3 - a0)>>12);
//			a3  = SAT((a3 + a2)>>12);
//			*(out_buf) = a3;
//			*(out_buf+1) = a5;
//			*(out_buf+2) = a4;
//			*(out_buf+3) = 0;
//
//			a4  = SAT((a33 + a1)>>12);
//			a5  = SAT((a33 - a0)>>12);
//			a33  = SAT((a33 + a2)>>12);
//			*(out_buf+4) = a33;
//			*(out_buf+5) = a5;
//			*(out_buf+6) = a4;
//			*(out_buf+7) = 0;
//
//			a4  = SAT((a7 + a1)>>12);
//			a5  = SAT((a7 - a0)>>12);
//			a7  = SAT((a7 + a2)>>12);
//			*(out_buf+0+out_stride) = a7;
//			*(out_buf+1+out_stride) = a5;
//			*(out_buf+2+out_stride) = a4;
//			*(out_buf+3+out_stride) = 0;
//
//			a4  = SAT((a77 + a1)>>12);
//			a5  = SAT((a77 - a0)>>12);
//			a77  = SAT((a77 + a2)>>12);
//			*(out_buf+4+out_stride) = a77;
//			*(out_buf+5+out_stride) = a5;
//			*(out_buf+6+out_stride) = a4;
//			*(out_buf+7+out_stride) = 0;
//
//			out_buf += 8;
//			psrc_y += 2;
//			psrc_u++;
//			psrc_v++;
//
//		}
//		psrc_y -= width;
//		psrc_y += (in_stridey<<1);
//
//		psrc_u -= (width>>1);
//		psrc_u += in_strideu;
//
//		psrc_v -= (width>>1);
//		psrc_v += in_stridev;
//
//		out_buf -= (width*4);
//		out_buf += (out_stride<<1);
//	}
//}
//
//
//void cc_yuv420torgb24_c_simple(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, const int32_t in_stridey, const int32_t in_strideu, const int32_t in_stridev, 
//				        uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height)
//{
//
//	uint32_t i, j;
//	int32_t u, v, rdif, invgdif, bdif;
//	int32_t r, g, b;
//
//	for(i = 0; i < height; i += 2){
//
//		for(j = 0; j < width; j += 2){
//
//        u = psrc_u[0] - 128;
//        v = psrc_v[0] - 128;
//
//        rdif = v + ((v * 103) >> 8);					//-180~+178
//        invgdif = ((u * 88) >> 8) +((v * 183) >> 8);	//-136~+133
//        bdif = u +( (u*198) >> 8);						//-227~+225
////1
//        r = psrc_y[0] + rdif;
//        g = psrc_y[0] - invgdif;
//        b = psrc_y[0] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//		*(out_buf) = b;
//		*(out_buf+1) = g;
//		*(out_buf+2) = r;
//
//        r = psrc_y[1] + rdif;
//        g = psrc_y[1] - invgdif;
//        b = psrc_y[1] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//		*(out_buf+3) = b;
//		*(out_buf+4) = g;
//		*(out_buf+5) = r;
////2
//        r = psrc_y[in_stridey+0] + rdif;
//        g = psrc_y[in_stridey+0] - invgdif;
//        b = psrc_y[in_stridey+0] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//		*(out_buf+0+out_stride) = b;
//		*(out_buf+1+out_stride) = g;
//		*(out_buf+2+out_stride) = r;
//
//        r = psrc_y[in_stridey+1] + rdif;
//        g = psrc_y[in_stridey+1] - invgdif;
//        b = psrc_y[in_stridey+1] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//		*(out_buf+3+out_stride) = b;
//		*(out_buf+4+out_stride) = g;
//		*(out_buf+5+out_stride) = r;
//
//		out_buf += 6;
//		psrc_y += 2;
//		psrc_u++;
//		psrc_v++;
//
//		}
//		psrc_y -= width;
//		psrc_y += (in_stridey<<1);
//
//		psrc_u -= (width>>1);
//		psrc_u += in_strideu;
//
//		psrc_v -= (width>>1);
//		psrc_v += in_stridev;
//
//		out_buf -= (width*3);
//		out_buf += (out_stride<<1);
//	}
//}
//
//#if 1
//#define SAT_64(Value) (Value) < 0 ? 0: ((Value) > 63 ? 63: (Value));
//#define SAT_32(Value) (Value) < 0 ? 0: ((Value) > 31 ? 31: (Value));
//void cc_yuv420torgb565_c_16bit22(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, const int32_t in_stridey, const int32_t in_strideu, const int32_t in_stridev, 
//				        uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height)
//{
//	int32_t a0, a1, a2, a3, a4, a5, a7, a33, a77, a16_f, a16_l;
//	uint32_t i, j;
//	for(i = 0; i < height; i += 2){
//		for(j = 0; j < width; j += 2){
//			a0 = psrc_u[0] - 128;
//			a1 = psrc_v[0] - 128;
//			a2 = (a0 * ConstU1);
//			a0 = (a1 * ConstV2 + a0 *ConstU2);
//			a1 = (a1 * ConstV1);
//
//			a3 = (psrc_y[0] - 16)*ConstY;
//			a33 = (psrc_y[1] - 16)*ConstY;
//
//			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
//			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;
//
//			a4  = SAT_32((a3 + a1)>>15);
//			a5  = SAT_64((a3 - a0)>>14);
//			a3  = SAT_32((a3 + a2)>>15);
//			a16_f = (a4<<11)|(a5<<5)|a3;
//			//*((uint32_t*)out_buf) = a16_f;
//			//*(out_buf) = a3;
//			//*(out_buf+1) = a5;
//			//*(out_buf+2) = a4;
//
//			a4  = SAT_32((a33 + a1)>>15);
//			a5  = SAT_64((a33 - a0)>>14);
//			a33  = SAT_32((a33 + a2)>>15);
//			a16_l = (a4<<11)|(a5<<5)|a33;
//
//			a16_f = a16_f | (a16_l<<16);
//			*((uint32_t*)(out_buf)) = a16_f;
//			//*(out_buf+3) = a33;
//			//*(out_buf+4) = a5;
//			//*(out_buf+5) = a4;
//
//			a4  = SAT_32((a7 + a1)>>15);
//			a5  = SAT_64((a7 - a0)>>14);
//			a7  = SAT_32((a7 + a2)>>15);
//			a16_f = (a4<<11)|(a5<<5)|a7;
//			//*((uint32_t*)(out_buf+out_stride)) = a16_f;
//			//*(out_buf+0+out_stride) = a7;
//			//*(out_buf+1+out_stride) = a5;
//			//*(out_buf+2+out_stride) = a4;
//
//			a4  = SAT_32((a77 + a1)>>15);
//			a5  = SAT_64((a77 - a0)>>14);
//			a77  = SAT_32((a77 + a2)>>15);
//			a16_l = (a4<<11)|(a5<<5)|a77;
//			a16_f = a16_f | (a16_l<<16);
//			*((uint32_t*)(out_buf+out_stride)) = a16_f;
//			//*(out_buf+3+out_stride) = a77;
//			//*(out_buf+4+out_stride) = a5;
//			//*(out_buf+5+out_stride) = a4;
//
//			out_buf += 4;
//			psrc_y += 2;
//			psrc_u++;
//			psrc_v++;
//
//		}
//		psrc_y -= width;
//		psrc_y += (in_stridey<<1);
//
//		psrc_u -= (width>>1);
//		psrc_u += in_strideu;
//
//		psrc_v -= (width>>1);
//		psrc_v += in_stridev;
//
//		out_buf -= (width*2);
//		out_buf += (out_stride<<1);
//	}
//}
void cc_yuv420torgb565_c_16bit(unsigned char *psrc_y, unsigned char *psrc_u, unsigned char *psrc_v, const signed long in_stridey, const signed long in_strideu, const signed long in_stridev, 
				        unsigned char *out_buf, const signed long out_stride, signed long width, signed long height)
{
	signed long a0, a1, a2, a3, a4, a5, a7, a33, a77;
	signed long i, j;
	for(i = 0; i < height; i += 2){
		for(j = 0; j < width; j += 2){
			a0 = psrc_u[0] - 128;
			a1 = psrc_v[0] - 128;
			a2 = (a0 * ConstU1);
			a0 = (a1 * ConstV2 + a0 *ConstU2);
			a1 = (a1 * ConstV1);

			a3 = (psrc_y[0] - 16)*ConstY;
			a33 = (psrc_y[1] - 16)*ConstY;

			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;

			a4  = SAT((a3 + a1)>>12);
			a5  = SAT((a3 - a0)>>12);
			a3  = SAT((a3 + a2)>>12);
			*(out_buf+1) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf) =( ((a5 & 0x1C) << 3) | ( a3 >> 3) );
			//*(out_buf) = a3;
			//*(out_buf+1) = a5;
			//*(out_buf+2) = a4;

			a4  = SAT((a33 + a1)>>12);
			a5  = SAT((a33 - a0)>>12);
			a33  = SAT((a33 + a2)>>12);
			*(out_buf+3) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf+2) =( ((a5 & 0x1C) << 3) | ( a33 >> 3) );
			//*(out_buf+3) = a33;
			//*(out_buf+4) = a5;
			//*(out_buf+5) = a4;

			a4  = SAT((a7 + a1)>>12);
			a5  = SAT((a7 - a0)>>12);
			a7  = SAT((a7 + a2)>>12);
			*(out_buf+1+out_stride) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf+0+out_stride) =( ((a5 & 0x1C) << 3) | ( a7 >> 3) );
			//*(out_buf+0+out_stride) = a7;
			//*(out_buf+1+out_stride) = a5;
			//*(out_buf+2+out_stride) = a4;

			a4  = SAT((a77 + a1)>>12);
			a5  = SAT((a77 - a0)>>12);
			a77  = SAT((a77 + a2)>>12);
			*(out_buf+3+out_stride) =( (a4 & 0xF8)  | ( a5 >> 5) );
			*(out_buf+2+out_stride) =( ((a5 & 0x1C) << 3) | ( a77 >> 3) );
			//*(out_buf+3+out_stride) = a77;
			//*(out_buf+4+out_stride) = a5;
			//*(out_buf+5+out_stride) = a4;

			out_buf += 4;
			psrc_y += 2;
			psrc_u++;
			psrc_v++;

		}
		psrc_y -= width;
		psrc_y += (in_stridey<<1);

		psrc_u -= (width>>1);
		psrc_u += in_strideu;

		psrc_v -= (width>>1);
		psrc_v += in_stridev;

		out_buf -= (width*2);
		out_buf += (out_stride<<1);
	}
	/*if(0){
		FILE *fp =fopen("/data/local/log.txt","a");
		fprintf(fp,"yuv420torgb565 %d %d\n",i,j);
		fclose(fp);
	}*/
}
//void cc_yuv420torgb565_c_32bit(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, const int32_t in_stridey, const int32_t in_strideu, const int32_t in_stridev, 
//				        uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height)
//{
//	int32_t a0, a1, a2, a3, a4, a5, a7, a33, a77;
//	uint32_t i, j;
//	for(i = 0; i < height; i += 2){
//
//		for(j = 0; j < width; j += 2){
//			a0 = psrc_u[0] - 128;
//			a1 = psrc_v[0] - 128;
//			a2 = (a0 * ConstU1);
//			a0 = (a1 * ConstV2 + a0 *ConstU2);
//			a1 = (a1 * ConstV1);
//
//			a3 = (psrc_y[0] - 16)*ConstY;
//			a33 = (psrc_y[1] - 16)*ConstY;
//
//			a7 = (psrc_y[in_stridey+0] - 16)*ConstY;
//			a77 = (psrc_y[in_stridey+1] - 16)*ConstY;
//
//			a4  = SAT((a3 + a1)>>20);
//			a5  = SAT((a3 - a0)>>20);
//			a3  = SAT((a3 + a2)>>20);
//
//			*(out_buf+1) =( (a4 & 0xF8)  | ( a5 >> 5) );
//			*(out_buf) =( ((a5 & 0x1C) << 3) | ( a3 >> 3) );
//			//*(out_buf) = a3;
//			//*(out_buf+1) = a5;
//			//*(out_buf+2) = a4;
//
//			a4  = SAT((a33 + a1)>>20);
//			a5  = SAT((a33 - a0)>>20);
//			a33  = SAT((a33 + a2)>>20);
//
//			*(out_buf+3) =( (a4 & 0xF8)  | ( a5 >> 5) );
//			*(out_buf+2) =( ((a5 & 0x1C) << 3) | ( a33 >> 3) );
//			//*(out_buf+3) = a33;
//			//*(out_buf+4) = a5;
//			//*(out_buf+5) = a4;
//
//			a4  = SAT((a7 + a1)>>20);
//			a5  = SAT((a7 - a0)>>20);
//			a7  = SAT((a7 + a2)>>20);
//			*(out_buf+1+out_stride) =( (a4 & 0xF8)  | ( a5 >> 5) );
//			*(out_buf+0+out_stride) =( ((a5 & 0x1C) << 3) | ( a7 >> 3) );
//			//*(out_buf+0+out_stride) = a7;
//			//*(out_buf+1+out_stride) = a5;
//			//*(out_buf+2+out_stride) = a4;
//
//			a4  = SAT((a77 + a1)>>20);
//			a5  = SAT((a77 - a0)>>20);
//			a77  = SAT((a77 + a2)>>20);
//			*(out_buf+3+out_stride) =( (a4 & 0xF8)  | ( a5 >> 5) );
//			*(out_buf+2+out_stride) =( ((a5 & 0x1C) << 3) | ( a77 >> 3) );
//			//*(out_buf+3+out_stride) = a77;
//			//*(out_buf+4+out_stride) = a5;
//			//*(out_buf+5+out_stride) = a4;
//
//			out_buf += 4;
//			psrc_y += 2;
//			psrc_u++;
//			psrc_v++;
//
//		}
//		psrc_y -= width;
//		psrc_y += (in_stridey<<1);
//
//		psrc_u -= (width>>1);
//		psrc_u += in_strideu;
//
//		psrc_v -= (width>>1);
//		psrc_v += in_stridev;
//
//		out_buf -= (width*2);
//		out_buf += (out_stride<<1);
//	}
//}
//
//void cc_yuv420torgb565_c_simple(uint8_t *psrc_y, uint8_t *psrc_u, uint8_t *psrc_v, const int32_t in_stridey, const int32_t in_strideu, const int32_t in_stridev, 
//				        uint8_t *out_buf, const int32_t out_stride, int32_t width, int32_t height)
//{
//
//	uint32_t i, j;
//	int32_t u, v, rdif, invgdif, bdif;
//	int32_t r, g, b;
//
//	for(i = 0; i < height; i += 2){
//
//		for(j = 0; j < width; j += 2){
//
//        u = psrc_u[0] - 128;
//        v = psrc_v[0] - 128;
//
//        rdif = v + ((v * 103) >> 8);					//-180~+178
//        invgdif = ((u * 88) >> 8) +((v * 183) >> 8);	//-136~+133
//        bdif = u +( (u*198) >> 8);						//-227~+225
////1
//        r = psrc_y[0] + rdif;
//        g = psrc_y[0] - invgdif;
//        b = psrc_y[0] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//
//		*(out_buf+1) =( (r & 0xF8)  | ( g >> 5) );
//        *(out_buf) =( ((g & 0x1C) << 3) | ( b >> 3) );
//
//		//*(out_buf) = b;
//		//*(out_buf+1) = g;
//		//*(out_buf+2) = r;
//
//        r = psrc_y[1] + rdif;
//        g = psrc_y[1] - invgdif;
//        b = psrc_y[1] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//
//		*(out_buf+3) =( (r & 0xF8)  | ( g >> 5) );
//        *(out_buf+2) =( ((g & 0x1C) << 3) | ( b >> 3) );
//
//		//*(out_buf+3) = b;
//		//*(out_buf+4) = g;
//		//*(out_buf+5) = r;
////2
//        r = psrc_y[in_stridey+0] + rdif;
//        g = psrc_y[in_stridey+0] - invgdif;
//        b = psrc_y[in_stridey+0] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//
//		*(out_buf+1+out_stride) =( (r & 0xF8)  | ( g >> 5) );
//        *(out_buf+0+out_stride) =( ((g & 0x1C) << 3) | ( b >> 3) );
//
//		//*(out_buf+0+out_stride) = b;
//		//*(out_buf+1+out_stride) = g;
//		//*(out_buf+2+out_stride) = r;
//
//        r = psrc_y[in_stridey+1] + rdif;
//        g = psrc_y[in_stridey+1] - invgdif;
//        b = psrc_y[in_stridey+1] + bdif;
//
//		r  = SAT(r);
//		g  = SAT(g);
//		b  = SAT(b);
//
//		*(out_buf+3+out_stride) =( (r & 0xF8)  | ( g >> 5) );
//        *(out_buf+2+out_stride) =( ((g & 0x1C) << 3) | ( b >> 3) );
//
//		//*(out_buf+3+out_stride) = b;
//		//*(out_buf+4+out_stride) = g;
//		//*(out_buf+5+out_stride) = r;
//
//		out_buf += 4;
//		psrc_y += 2;
//		psrc_u++;
//		psrc_v++;
//
//		}
//		psrc_y -= width;
//		psrc_y += (in_stridey<<1);
//
//		psrc_u -= (width>>1);
//		psrc_u += in_strideu;
//
//		psrc_v -= (width>>1);
//		psrc_v += in_stridev;
//
//		out_buf -= (width*2);
//		out_buf += (out_stride<<1);
//	}
//}

//#endif
