/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "voMpeg2Decoder.h"
#include "voMpegPort.h"
#include "voMpegBuf.h"
#include "voMpegMem.h"

VO_S32
image_create(Image * image,
			 VO_U32 edged_width,
			 VO_U32 edged_height,
			 MpegDecode* dec)
{
	const VO_U32 edged_width2 = edged_width / 2;
	const VO_U32 edged_height2 = edged_height / 2;

	image->y = (VO_U8 *)(MallocMem(edged_width * (edged_height + 1) + SAFETY, CACHE_LINE, &dec->memoryOperator));

    if (image->y == NULL) {
		return -1;
	}

	image->u = (VO_U8 *)(MallocMem(edged_width2 * edged_height2*2 + SAFETY, CACHE_LINE, &dec->memoryOperator));
	 
	 if (image->u == NULL) {
		 FreeMem(image->y, &dec->memoryOperator);		 
		 image->y = NULL;
		 return -1;
	 }

	 if(dec->uv_interlace_flage){
		 image->v = image->u + edged_width2;
	 }else{
		 image->v = image->u + edged_width2 * edged_height2;
	 }
	 

	image->y += EDGE_SIZE * edged_width + EDGE_SIZE;
	image->u += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;
	image->v += EDGE_SIZE2 * edged_width2 + EDGE_SIZE2;

	return VO_ERR_NONE;
}



VO_VOID
image_destroy(Image * image,
			  VO_U32 edged_width,
			  VO_U32 edged_height,
			  MpegDecode* dec)
{
	//const VO_U32 edged_width2 = edged_width / 2;

	if (image->y){
		FreeMem(image->y, &dec->memoryOperator);
		image->y = NULL;
	}

	if (image->u){
		FreeMem(image->u, &dec->memoryOperator);
		image->u = NULL;
		image->v = NULL;
	}
}


VO_VOID
image_swap(Image * image1,Image * image2)
{
	Image tmp;
	
	tmp = *image1;
	*image1 = *image2;
	*image2 = tmp;
}

/* setedges bug was fixed in this BS version */
#define SETEDGES_BUG_BEFORE		18

