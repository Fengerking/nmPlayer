void voPackUV(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest)
{
	int i, j;
    unsigned char* pUV = (unsigned char*) dstUV;
    unsigned char* pU = (unsigned char*) srcU;
    unsigned char* pV = (unsigned char*) srcV;
    int offsetU = strideU - width;
    int offsetV = strideV - width;
    int offsetdest = stridedest - (width<<1);
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < width; j++)
        {
            *pUV++ = *pU++;
            *pUV++ = *pV++;
        }
        pU += offsetU;
        pV += offsetV;
		pUV+=offsetdest;
    }
} 


void voPackUV_FLIP_Y(unsigned char* dstUV, unsigned char* srcU, unsigned char* srcV, int strideU, int strideV, int rows, int width, int stridedest)
{
	int i, j;
    unsigned char* pUV = (unsigned char*) dstUV + (rows - 1)*stridedest;
    unsigned char* pU = (unsigned char*) srcU;
    unsigned char* pV = (unsigned char*) srcV;
    int offsetU = strideU - width;
    int offsetV = strideV - width;
    int offsetdest = stridedest + (width<<1);
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < width; j++)
        {
            *pUV++ = *pU++;
            *pUV++ = *pV++;
        }
        pU += offsetU;
        pV += offsetV;
		pUV-=offsetdest;
    }
}