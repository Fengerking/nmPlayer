#include "stdio.h"
#include <stdlib.h> 

extern void voPackUV(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest);


void main()
{
	unsigned char *dstUV,*srcyuv,*srcu,*srcv;
	FILE* inFile = fopen ("/sdcard//Demo_VC1_VGA.yuv", "rb" );

	dstUV = (unsigned char*)malloc(sizeof(unsigned char)*640*240);
	srcyuv = (unsigned char*)malloc(sizeof(unsigned char)*640*480*1.5);

	fread(srcyuv, 1, 640*480*1.5, inFile);
	srcu = srcyuv+640*480;
	srcv = srcyuv+320*240;

printf(" U:%d %d %d %d \n",srcu[0],srcu[1],srcu[2],srcu[3]);
printf(" V:%d %d %d %d \n",srcv[0],srcv[1],srcv[2],srcv[3]);
	
	voPackUV(dstUV, srcu,srcv, 320, 320, 240, 320, 640);
	
printf(" UV:%d %d %d %d %d %d %d %d\n",dstUV[0],dstUV[1],dstUV[2],dstUV[3]);
	printf("nmnm \n");

	fclose(inFile);
	free(dstUV);
	free(srcyuv);


}