#include "CQ7200Convert.h"
#include "pmem_ids.h"
#include "pmem.h"

CQ7200Convert::CQ7200Convert(void)
: mpParams (NULL)
, mpDstBmp (NULL)
, mpSrcBmp (NULL)
, mpOp (NULL)
{
	Init();
}

CQ7200Convert::~CQ7200Convert(void)
{
	UnInit();
}

bool CQ7200Convert::Init()
{
	int sizeOfParams = sizeof(disp2d_bm_type) * 2 + sizeof(disp2d_bm_blt_op_type);
	mpParams =  (BYTE *)pmem_malloc(sizeOfParams, PMEM_GRAPHICS_TEXTURE_ID);
	if (mpParams == NULL)
		return false;

	memset(mpParams, 0, sizeOfParams);

	mpSrcBmp = (disp2d_bm_type*)mpParams;
	mpDstBmp = (disp2d_bm_type*)((BYTE*)mpSrcBmp + sizeof(disp2d_bm_type));
	mpOp = (disp2d_bm_blt_op_type*)((BYTE*)mpDstBmp + sizeof(disp2d_bm_type));

	return true;
}

bool CQ7200Convert::UnInit()
{
	if (mpParams)
	{
		pmem_free(mpParams);
		mpParams = NULL;
	}

	return true;
}

bool CQ7200Convert::StartConvert(BmpInfo *pIn , BmpInfo *pOut , int nAngle)
{
	mpOp->bitwiseOp       = DISP2D_BLT_OP_ROT_REFLECT ;
	if(nAngle == 90)
	{
		mpOp->rotation = DISP2D_ROT90;
	}
	else if(nAngle == 270)
	{
		mpOp->rotation = DISP2D_ROT270;
	}
	else
	{
		mpOp->rotation = DISP2D_NOROT;
	}

	mpSrcBmp->bm.colorFormat = DISP2D_Y_CRCB_H2V2;
	mpSrcBmp->bm.pBmPlane0  = pIn->bufPoint.pBuf1;
	mpSrcBmp->bm.pBmPlane1  = pIn->bufPoint.pBuf2;
	mpSrcBmp->bm.pBmPlane2  = pIn->bufPoint.pBuf3;
	mpSrcBmp->bm.pitchInBytes = 0;
	mpSrcBmp->bm.pixelWidth = pIn->nBmpWidth;
	mpSrcBmp->bm.pixelHeight = pIn->nBmpHeight;
	mpSrcBmp->roi.x = pIn->nLeft;
	mpSrcBmp->roi.y = pIn->nTop;
	mpSrcBmp->roi.dx = pIn->nPicWidth;
	mpSrcBmp->roi.dy = pIn->nPicHeight;

	mpDstBmp->bm.colorFormat = DISP2D_RGB_565;
	mpDstBmp->bm.pBmPlane0  = pOut->bufPoint.pBuf1;
	mpDstBmp->bm.pBmPlane1  = pOut->bufPoint.pBuf2;
	mpDstBmp->bm.pBmPlane2  = pOut->bufPoint.pBuf3;
	mpDstBmp->bm.pitchInBytes = 0;
	mpDstBmp->bm.pixelWidth = pOut->nBmpWidth;
	mpDstBmp->bm.pixelHeight = pOut->nBmpHeight;
	mpDstBmp->roi.x = pOut->nLeft;
	mpDstBmp->roi.y = pOut->nTop;
	mpDstBmp->roi.dx = pOut->nPicWidth;
	mpDstBmp->roi.dy = pOut->nPicHeight;

	//printf("%d %d %d %d %d %d => %d %d %d %d %d %d %d \r\n" , pIn->nBmpWidth , pIn->nBmpHeight , pIn->nLeft , pIn->nTop , pIn->nPicWidth , pIn->nPicHeight ,
	//	pOut->nBmpWidth , pOut->nBmpHeight , pOut->nLeft , pOut->nTop , pOut->nPicWidth , pOut->nPicHeight , nAngle);

	BOOL async = FALSE;
	disp2d_status_enum status = ddi_disp2d_blt(mpSrcBmp, mpDstBmp, mpOp, async);
	if(status == DISP2D_SUCCESSFUL)
	{
		return true;
	}

	return false;
}
