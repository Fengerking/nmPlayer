#ifndef LINUX
#include <tchar.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


#ifdef LINUX
#pragma   pack(1)
#else
#pragma pack(push, 1)
#endif
typedef unsigned char       BYTE;
typedef unsigned long       DWORD;
typedef unsigned short      WORD;
typedef long LONG;
typedef char CHAR;
typedef CHAR *LPSTR, *PSTR;
#define WIDTHBYTES(bits)    (((bits) + 31) / 32 * 4)
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L
typedef struct tagRGBQUAD  {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
typedef struct tagBITMAPFILEHEADER {
        unsigned short    bfType;
        unsigned int   bfSize;
        unsigned short    bfReserved1;
        unsigned short    bfReserved2;
        unsigned int   bfOffBits;
} BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;
#ifdef LINUX
#pragma   pack() 
#else
#pragma pack(pop)
#endif

void writebmpfile(char * filename,unsigned char* image,long width,long height);