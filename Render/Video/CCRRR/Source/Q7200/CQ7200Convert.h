#pragma once
#include "disptools.h"

typedef struct _BufPoint 
{
	BYTE *pBuf1;
	BYTE *pBuf2;
	BYTE *pBuf3;
}BufPoint;

typedef struct _BmpInfo
{
	DWORD	dwColor;
	int		nBmpWidth;
	int		nBmpHeight;
	int		nLeft;
	int		nTop;
	int		nPicWidth;
	int		nPicHeight;
	BufPoint bufPoint;
}BmpInfo;

class CQ7200Convert
{
public:
	CQ7200Convert(void);
	virtual ~CQ7200Convert(void);

	virtual bool StartConvert(BmpInfo *pIn , BmpInfo *pOut , int nAngle);

protected:
	bool Init();
	bool UnInit();

protected:
	BYTE*					mpParams;
	disp2d_bm_type*			mpSrcBmp;
	disp2d_bm_type*			mpDstBmp;
	disp2d_bm_blt_op_type*	mpOp;
};
