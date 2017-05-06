

#include "global.h"
#include "transform.h"
#define Clip255(data) (imgpel) ((data)<0?0:(data)>255?255:(data));

#if defined (VOSSSE3)

#ifdef __cplusplus
extern "C" {
#endif 	

extern void vo_Idct4x4Add_mmx(short *tblock, imgpel  *imgBuf1, int dst_stride);
extern void vo_Idct4x4DCAdd_mmx(int dc, imgpel  *imgBuf1, int dst_stride);
extern void vo_Idct8x8Add_sse2(short *tblock, imgpel  *imgBuf1, int dst_stride,unsigned char * buf);

void vo_Idct8x8Add_sse2_ex(short *tblock, imgpel  *imgBuf1, int dst_stride)
{
	DECLARE_ALIGNED_EXT(16, VO_U8, sIDCTBuffer[128]);
	memset(sIDCTBuffer, 0 , 128);
	vo_Idct8x8Add_sse2(tblock, imgBuf1, dst_stride, sIDCTBuffer);
}

#ifdef __cplusplus
}
#endif 	

#endif

void Idct4x4DCAdd_C(int dc, imgpel  *imgBuf1, int dst_stride)
{
  VO_S32 i;
  for(i = 0;i < 4;i++)
  {
    imgBuf1[0] = Clip255(imgBuf1[0]+dc);
	imgBuf1[1] = Clip255(imgBuf1[1]+dc);
	imgBuf1[2] = Clip255(imgBuf1[2]+dc);
	imgBuf1[3] = Clip255(imgBuf1[3]+dc);
	imgBuf1+=dst_stride;
  }
}

//#if !defined(VOARMV7)

void Idct8x8Add_C(short *tblock, imgpel  *imgBuf1, int dst_stride)
{
  int i;  
  int tmp[64];
  int *pTmp = tmp;
  int a0, a1, a2, a3;
  int p0, p1, p2, p3, p4, p5 ,p6, p7;  
  int b0, b1, b2, b3, b4, b5, b6, b7;
	 
  // Horizontal
  for (i = 0; i < 8; i++)
  {
	p0 = *(tblock++);
	p1 = *(tblock++);
	p2 = *(tblock++);
	p3 = *(tblock++);
	p4 = *(tblock++);
	p5 = *(tblock++);
	p6 = *(tblock++);
	p7 = *(tblock++);

	a0 = p0 + p4;
    a1 = p0 - p4;
    a2 = p6 - (p2 >> 1);
    a3 = p2 + (p6 >> 1);

    b0 =  a0 + a3;
    b2 =  a1 - a2;
    b4 =  a1 + a2;
    b6 =  a0 - a3;

    a0 = -p3 + p5 - p7 - (p7 >> 1);    
    a1 =  p1 + p7 - p3 - (p3 >> 1);    
    a2 = -p1 + p7 + p5 + (p5 >> 1);    
    a3 =  p3 + p5 + p1 + (p1 >> 1);

    
    b1 =  a0 + (a3>>2);    
    b3 =  a1 + (a2>>2);    
    b5 =  a2 - (a1>>2);
    b7 =  a3 - (a0>>2);

	*(pTmp++) = b0 + b7;
    *(pTmp++) = b2 - b5;
    *(pTmp++) = b4 + b3;
    *(pTmp++) = b6 + b1;
    *(pTmp++) = b6 - b1;
    *(pTmp++) = b4 - b3;
    *(pTmp++) = b2 + b5;
    *(pTmp++) = b0 - b7;
  }

	 //  Vertical 
  for (i = 0; i < 8; i++)
  {
	pTmp = tmp + i;
    p0 = *pTmp;
    p1 = *(pTmp += BLOCK_SIZE_8x8);
    p2 = *(pTmp += BLOCK_SIZE_8x8);
    p3 = *(pTmp += BLOCK_SIZE_8x8);
    p4 = *(pTmp += BLOCK_SIZE_8x8);
    p5 = *(pTmp += BLOCK_SIZE_8x8);
    p6 = *(pTmp += BLOCK_SIZE_8x8);
    p7 = *(pTmp += BLOCK_SIZE_8x8);

    a0 =  p0 + p4+32;
    a1 =  p0 - p4+32;
    a2 =  p6 - (p2>>1);
    a3 =  p2 + (p6>>1);

    b0 = a0 + a3;
    b2 = a1 - a2;
    b4 = a1 + a2;
    b6 = a0 - a3;

    a0 = -p3 + p5 - p7 - (p7 >> 1);
    a1 =  p1 + p7 - p3 - (p3 >> 1);
    a2 = -p1 + p7 + p5 + (p5 >> 1);
    a3 =  p3 + p5 + p1 + (p1 >> 1);


    b1 =  a0 + (a3 >> 2);
    b7 =  a3 - (a0 >> 2);
    b3 =  a1 + (a2 >> 2);
    b5 =  a2 - (a1 >> 2);

	*imgBuf1 = Clip255(((b0 + b7 ) >> 6)+imgBuf1[0]);
	*(imgBuf1 + dst_stride) = Clip255(((b2 - b5 ) >> 6)+imgBuf1[dst_stride]);
	*(imgBuf1 + 2*dst_stride) = Clip255(((b4 + b3 ) >> 6)+imgBuf1[2*dst_stride]);
	*(imgBuf1 + 3*dst_stride) = Clip255(((b6 + b1 ) >> 6)+imgBuf1[3*dst_stride]);
	*(imgBuf1 + 4*dst_stride) = Clip255(((b6 - b1 ) >> 6)+imgBuf1[4*dst_stride]);
	*(imgBuf1 + 5*dst_stride) = Clip255(((b4 - b3 ) >> 6)+imgBuf1[5*dst_stride]);
	*(imgBuf1 + 6*dst_stride) = Clip255(((b2 + b5 ) >> 6)+imgBuf1[6*dst_stride]);
	*(imgBuf1 + 7*dst_stride) = Clip255(((b0 - b7 ) >> 6)+imgBuf1[7*dst_stride]);
	imgBuf1++;
  }
}


void Idct4x4Add_C(short *tblock, imgpel  *imgBuf1, int dst_stride)
 {
	 int i;  
	 int tmp[16];
	 int *pTmp = tmp;
	 int p0,p2,t0,t1,t2,t3;
	 



	 // Horizontal
	 for (i = 0; i < 4; i++){
		 t0 = *(tblock++);
		 t1 = *(tblock++);
		 t2 = *(tblock++);
		 t3 = *(tblock++);

		 p0 =  t0 + t2;
		 t0 =  t0 - t2;
		 p2 = (t1 >> 1) - t3;
		 t1 =  t1 + (t3 >> 1);

		 *(pTmp++) = p0 + t1;
		 *(pTmp++) = t0 + p2;
		 *(pTmp++) = t0 - p2;
		 *(pTmp++) = p0 - t1;
		 //tblock += MB_BLOCK_SIZE;
	 }

	 //  Vertical 
	 for (i = 0; i < 4; i++){
		 pTmp = tmp + i;
		 t0 = *pTmp+32;
		 t2 = *(pTmp += 4);
		 t1 = *(pTmp += 4);
		 t3 = *(pTmp += 4);

		 p0 = t0 + t1;
		 t0 = t0 - t1;
		 p2 =(t2 >> 1) - t3;
		 t2 = t2 + (t3 >> 1);

	//	 ii = i + pos_x;

		 //t1 = ((p0 + t2 ) >> 6);//0--1
		 //t3 = ((t0 + p2 ) >> 6);
		 //t0 = ((t0 - p2 ) >> 6);
		 //t2 = ((p0 - t2 ) >> 6);

		 *imgBuf1 = Clip255(((p0 + t2 ) >> 6)+imgBuf1[0]);
		 *(imgBuf1 + dst_stride) = Clip255(((t0 + p2 ) >> 6)+imgBuf1[dst_stride]);
		 *(imgBuf1 + 2*dst_stride) = Clip255(((t0 - p2 ) >> 6)+imgBuf1[2*dst_stride]);
		 *(imgBuf1 + 3*dst_stride) = Clip255(((p0 - t2 ) >> 6)+imgBuf1[3*dst_stride]);
		 imgBuf1++;
	 }
 }
//#endif
void ihadamard4x4(short *tblock)
{
  int i;  
  int tmp[16];
  int *pTmp = tmp;
  short *pblock;
  int p0,p1,p2,p3;
  int t0,t1,t2,t3;

  // Horizontal
  for (i = 0; i < BLOCK_SIZE; i++)
  {
    pblock = tblock+i*4;
    t0 = *(pblock++);
    t1 = *(pblock++);
    t2 = *(pblock++);
    t3 = *(pblock  );

    p0 = t0 + t2;
    p1 = t0 - t2;
    p2 = t1 - t3;
    p3 = t1 + t3;

    *(pTmp++) = p0 + p3;
    *(pTmp++) = p1 + p2;
    *(pTmp++) = p1 - p2;
    *(pTmp++) = p0 - p3;
  }

  //  Vertical 
  for (i = 0; i < BLOCK_SIZE; i++)
  {
    pTmp = tmp + i;
    t0 = *pTmp;
    t1 = *(pTmp += BLOCK_SIZE);
    t2 = *(pTmp += BLOCK_SIZE);
    t3 = *(pTmp += BLOCK_SIZE);

    p0 = t0 + t2;
    p1 = t0 - t2;
    p2 = t1 - t3;
    p3 = t1 + t3;
    
    tblock[i] = p0 + p3;
    tblock[4+i] = p1 + p2;
    tblock[8+i] = p1 - p2;
    tblock[12+i] = p0 - p3;
  }
}


void ihadamard2x2(short tblock[4], VO_S16* cof,VO_S32 mul,VO_S32 qp)
{
  int t0,t1,t2,t3;

  t0 = tblock[0] + tblock[1];
  t1 = tblock[0] - tblock[1];
  t2 = tblock[2] + tblock[3];
  t3 = tblock[2] - tblock[3];

  cof[0] =  (short)((((t0 + t2) * mul)<<qp)>>5);
  cof[16] = (short)((((t1 + t3) * mul)<<qp)>>5);
  cof[32] = (short)((((t0 - t2) * mul)<<qp)>>5);
  cof[48] = (short)((((t1 - t3) * mul)<<qp)>>5);         
}


