#include "writebmp.h"

void writebmpfile(char * filename,unsigned char* image,long width,long height,long stride,long componet)
{
	int i=0;
	BITMAPINFO *info;
	BITMAPFILEHEADER bmphead;			
	FILE * fp = fopen(filename,"wb");

	bmphead.bfType = 0x4d42;
	bmphead.bfSize = width*height*componet+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO);//+1078;
	bmphead.bfReserved1 = 0;
	bmphead.bfReserved2 = 0;
	bmphead.bfOffBits = 54;//1078;

	info=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));//sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD));//1064
	info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); //40
	info->bmiHeader.biBitCount = componet*8;
	info->bmiHeader.biClrImportant =0;
	info->bmiHeader.biClrUsed = 0;
	info->bmiHeader.biWidth = width;
	info->bmiHeader.biHeight = -height;
	info->bmiHeader.biCompression = BI_RGB;
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biSizeImage = width*height*componet;//imgData.pBmpInfoHead->biSizeImage;
	info->bmiHeader.biXPelsPerMeter =0;
	info->bmiHeader.biYPelsPerMeter =0;	

	fwrite((LPSTR)&bmphead, sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(info,sizeof(BITMAPINFOHEADER),1, fp);

	for(i=0;i<height;i++)
	{
		unsigned char* prt= image+i*stride;
		fwrite(prt,stride,1,fp);
	}
	fclose(fp);
	free(info);
	return;
}

FILE* writebmphead(char * filename,long width,long height,long componet,unsigned char* pal,int pal_num)
{
	BITMAPINFO *info;
	BITMAPFILEHEADER bmphead;			
	FILE *fp = fopen(filename,"wb");

	bmphead.bfType = 0x4d42;
	bmphead.bfSize = width*height*componet+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFO);
	bmphead.bfReserved1 = 0;
	bmphead.bfReserved2 = 0;
	bmphead.bfOffBits = (pal==NULL)?54:54+pal_num*sizeof(RGBQUAD);

	if(pal==NULL&&componet!=1)
		info=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));
	else
	{
		bmphead.bfSize += pal_num*sizeof(RGBQUAD);
		info=(BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER)+pal_num*sizeof(RGBQUAD));//1064
	}
	info->bmiHeader.biSize=sizeof(BITMAPINFOHEADER); //40
	info->bmiHeader.biBitCount = componet*8;
	info->bmiHeader.biClrImportant =0;
	info->bmiHeader.biClrUsed = (pal==NULL)?0:pal_num;
	info->bmiHeader.biWidth = width;
	info->bmiHeader.biHeight = -height;
	info->bmiHeader.biCompression = BI_RGB;
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biSizeImage = width*height*componet;//imgData.pBmpInfoHead->biSizeImage;
	info->bmiHeader.biXPelsPerMeter =0;
	info->bmiHeader.biYPelsPerMeter =0;	

	if(info->bmiHeader.biBitCount==8&&pal==NULL)
	{
		bmphead.bfOffBits = 54+256*sizeof(RGBQUAD);
		info->bmiHeader.biClrUsed = 256;
	}

	fwrite((LPSTR)&bmphead, sizeof(BITMAPFILEHEADER),1,fp);
	fwrite(info,sizeof(BITMAPINFOHEADER),1, fp);
	if(pal)
		fwrite((unsigned char *)pal, pal_num*sizeof(RGBQUAD), 1, fp);

	if(info->bmiHeader.biBitCount==8&&pal==NULL)
	{
		int i;
		RGBQUAD def_palt[256];
		for(i=0;i<256;i++)
		{
			def_palt[i].rgbBlue=(unsigned char)i;
			def_palt[i].rgbGreen=(unsigned char)i;
			def_palt[i].rgbRed=(unsigned char)i;
			def_palt[i].rgbReserved=0;
		} 		
		fwrite((unsigned char *)def_palt, 256*sizeof(RGBQUAD), 1, fp);
	}

	free(info);

	return fp;
}
void writebmpdata(FILE * fp,unsigned char * data,long width,long height,long stride,long componet)
{
	int i;
	for(i=0;i<height;i++)
	{
		unsigned char* prt= data+i*stride;
		fwrite(prt,stride,1,fp);
		fwrite(prt,(((stride+3)&~3)-(stride)),1,fp);
	}
	return;
}

void writebmptail(FILE * fp)
{
	fclose(fp);
}