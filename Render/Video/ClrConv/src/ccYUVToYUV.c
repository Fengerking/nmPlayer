/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2005				*
*																		*
************************************************************************/

#include "ccYUVToYUV.h"
#include "ccConstant.h"
#include <stdlib.h>

CLRCONV_FUNC_PTR YUV420PLANAR_RESIZE[3][3] =
{
	{YUV420Planar_noresize, YUV420Planar_RT90,YUV420Planar_RT180},
	{YUV420Planar_resize, YUV420Planar_resize_RT90,YUV420Planar_resize_RT180},
	{YUV420Planar_resize_anti_aliasing,NULL,NULL}
};

CLRCONV_FUNC_PTR YUVPlanarToYUYV[2][3] =
{
	{YUVPlanarToYUYV_noresize, NULL,NULL},
	{NULL, NULL,NULL}
};

CLRCONV_FUNC_PTR YUVPlanarToUYVY[2][3] =
{
	{YUVPlanarToUYVY_noresize, NULL ,NULL},
	{NULL, NULL, NULL}
};

#ifdef COLCONVENABLE
CLRCONV_FUNC_PTR YUV420PLANARTOYUV420PACK_RESIZE[3][3] =
{
	{YUV420PlanarToYUV420Pack_noresize, YUV420PlanarToYUV420Pack_RT90,YUV420PlanarToYUV420Pack_RT180},
	{YUV420PlanarToYUV420Pack_resize, YUV420PlanarToYUV420Pack_resize_RT90,YUV420PLANARToYUV420PACK_resize_RT180},
	{NULL,NULL,NULL}
};

CLRCONV_FUNC_PTR YUV420PLANARTOYUV420PACK2_RESIZE[3][3] =
{
	{YUV420PlanarToYUV420Pack2_noresize, YUV420PlanarToYUV420Pack2_RT90,YUV420PlanarToYUV420Pack2_RT180},
	{YUV420PlanarToYUV420Pack2_resize, YUV420PlanarToYUV420Pack2_resize_RT90,YUV420PlanarToYUV420Pack2_resize_RT180},
	{NULL,NULL,NULL}
};

CLRCONV_FUNC_PTR YUV420PACKED_RESIZE[2][2] =
{
	{YUV420Pack_noresize, YUVPacked_RT90},
	{YUV420Packed_resize, YUV420Packed_resize_RT90}
};

CLRCONV_FUNC_PTR YUV422PACK_RESIZE[2][2] =
{
	{YUV422Pack_noresize, YUV422Pack_noresize_Rt90},
	{YUV422Packed_resize, YUV422Packed_resize_RT90}
};

static VO_U32 u_shift_tab[4] = {1, 3, 0, 2};
static VO_U32 v_shift_tab[4] = {3, 1, 2, 0};
#endif //COLCONVENABLE

#ifdef COLCONVENABLE
static VOCCRETURNCODE UVPlanarToNV12(ClrConvData *conv_data, VO_U8 *src_u, VO_U8 *src_v,VO_S32 out_width, VO_S32 out_height)
{
	VO_S32 i, j;
	const VO_S32 out_stride = conv_data->nOutStride;

	for(i = 0; i < out_height; i++){
		VO_U8 *uv_buf = (VO_U8 *)(conv_data->pOutBuf[1] + i * out_stride);
		VO_U8 *in_u = (VO_U8 *)(src_u + (i * out_stride >> 1));
		VO_U8 *in_v = (VO_U8 *)(src_v + (i * out_stride >> 1));

		for(j = 0; j < out_width; j++){
			*(uv_buf++) = *(in_u++);
			*(uv_buf++) = *(in_v++);
		}
	}
	return VO_ERR_NONE;
}
#endif //COLCONVENABLE

VOCCRETURNCODE YUV420Planar_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32  vy;//vx = 0,
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 out_stride = conv_data->nOutStride;	

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[0] + vy * out_stride;
		psrc = conv_data->pInBuf[0] + vy * in_stride;

		memcpy(pdst, psrc, out_width);
	}

	//u
	in_stride >>= 1;
	out_stride >>= 1;
	out_width >>= 1;
	out_height >>= 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[1] + vy * out_stride;
		psrc = conv_data->pInBuf[1] + vy * in_stride;
		
		memcpy(pdst, psrc, out_width);
	}
	//v
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[2] + vy * out_stride;
		psrc = conv_data->pInBuf[2] + vy * in_stride;

		memcpy(pdst, psrc, out_width);
	}

	return VO_ERR_NONE;
}

static void YUV420_YUYV422_C(VO_U8 *  Dst, VO_U8 *  Y, VO_U8 *  U, VO_U8 *  V, VO_S32 YStride, VO_S32 UVStride, VO_S32 w, VO_S32 h, VO_S32 OutStride)
{
	VO_U8 *Des, *Des1, *Ybuf, *Ybuf1, *Ubuf, *Vbuf;
	VO_S32 i, j;

	for (i=0; i<h; i+=2) 
	{
		Des = Dst;	Des1 = Dst + OutStride;
		Ybuf = Y;	Ybuf1 = Y + YStride;     
		Ubuf = U;                              
		Vbuf = V;                                      
		for (j = 0; j < w; j+=2) 
		{
			*Des++ = *Ybuf++;
			*Des++ = *Ubuf;
			*Des++ = *Ybuf++;
			*Des++ = *Vbuf;

			*Des1++ = *Ybuf1++;
			*Des1++ = *Ubuf++;
			*Des1++ = *Ybuf1++;
			*Des1++ = *Vbuf++;
		}
		Dst += (OutStride<<1);
		Y += (YStride<<1);
		U += UVStride;
		V += UVStride;
	}
}

static void YUV420_UYVY422_C(VO_U8 *  Dst, VO_U8 *  Y, VO_U8 *  U, VO_U8 *  V, VO_S32 YStride, VO_S32 UVStride, VO_S32 w, VO_S32 h, VO_S32 OutStride)
{
	VO_U8 *Des, *Des1, *Ybuf, *Ybuf1, *Ubuf, *Vbuf;
	VO_S32 i, j;

	for (i=0; i<h; i+=2) 
	{
		Des = Dst;	Des1 = Dst + OutStride;
		Ybuf = Y;	Ybuf1 = Y + YStride;     
		Ubuf = U;                              
		Vbuf = V;                                      
		for (j = 0; j < w; j+=2) 
		{
			*Des++ = *Ubuf;
			*Des++ = *Ybuf++;
			*Des++ = *Vbuf;
			*Des++ = *Ybuf++;

			*Des1++ = *Ubuf++;
			*Des1++ = *Ybuf1++;
			*Des1++ = *Vbuf++;
			*Des1++ = *Ybuf1++;
		}
		Dst += (OutStride<<1);
		Y += (YStride<<1);
		U += UVStride;
		V += UVStride;
	}
} 

#ifdef VOARMV7
extern void YUV420_YUYV422_32(VO_U8 *  Dst, VO_U8 *  Y, VO_U8 *  U, VO_U8 *  V, VO_S32 YStride, VO_S32 UVStride, VO_S32 w, VO_S32 h, VO_S32 OutStride);
extern void YUV420_YUYV422_16(VO_U8 *  Dst, VO_U8 *  Y, VO_U8 *  U, VO_U8 *  V, VO_S32 YStride, VO_S32 UVStride, VO_S32 w, VO_S32 h, VO_S32 OutStride);
#endif

VOCCRETURNCODE YUVPlanarToYUYV_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *pYSrc, *pUSrc, *pVSrc;
	VO_U8 *pDst = conv_data->pOutBuf[0];
	VO_U32 in_stride = conv_data->nInStride;
	VO_U32 in_UVStride = conv_data->nInUVStride;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_U32 in_width = conv_data->nInWidth;
	VO_S32 out_stride = conv_data->nOutStride;

	pYSrc = conv_data->pInBuf[0];
	pUSrc = conv_data->pInBuf[1];
	pVSrc = conv_data->pInBuf[2];

	if(!(out_width % 32)){
#ifdef VOARMV7
		YUV420_YUYV422_32(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#else
		YUV420_YUYV422_C(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#endif
	}else{
		VO_S32 w_offset;

		w_offset = out_width%16;
		out_width = out_width - w_offset;

#ifdef VOARMV7
		YUV420_YUYV422_16(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#else
		YUV420_YUYV422_C(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#endif

		if(w_offset){
			pDst  = pDst + (out_width<<1);
			pYSrc = pYSrc + in_width;
			pUSrc = pUSrc + (in_width>>1);
			pVSrc = pVSrc + (in_width>>1);  	   	 	
			YUV420_YUYV422_C(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, w_offset, out_height, out_stride);
		} 	 	
	}
	return VO_ERR_NONE;
}

#ifdef VOARMV7
extern void YUV420_UYVY422_32(VO_U8 *  Dst, VO_U8 *  Y, VO_U8 *  U, VO_U8 *  V, VO_S32 YStride, VO_S32 UVStride, VO_S32 w, VO_S32 h, VO_S32 OutStride);
extern void YUV420_UYVY422_16(VO_U8 *  Dst, VO_U8 *  Y, VO_U8 *  U, VO_U8 *  V, VO_S32 YStride, VO_S32 UVStride, VO_S32 w, VO_S32 h, VO_S32 OutStride);
#endif
VOCCRETURNCODE YUVPlanarToUYVY_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *pYSrc, *pUSrc, *pVSrc;
	VO_U8 *pDst = conv_data->pOutBuf[0];
	VO_U32 in_stride = conv_data->nInStride;
	VO_U32 in_UVStride = conv_data->nInUVStride;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_U32 in_width = conv_data->nInWidth;
	VO_S32 out_stride = conv_data->nOutStride;

	pYSrc = conv_data->pInBuf[0];
	pUSrc = conv_data->pInBuf[1];
	pVSrc = conv_data->pInBuf[2];

	if(!(out_width%32)){
#ifdef VOARMV7
		YUV420_UYVY422_32(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#else
		YUV420_UYVY422_C(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#endif
	}else{
		int w_offset;
		w_offset = out_width%16;
		out_width = out_width - w_offset;

#ifdef VOARMV7
		YUV420_UYVY422_16(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#else
		YUV420_UYVY422_C(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, out_width, out_height, out_stride);
#endif

		if(w_offset){
			pDst = pDst + (out_width<<1);
			pYSrc = pYSrc + in_width;
			pUSrc = pUSrc + (in_width>>1);
			pVSrc = pVSrc + (in_width>>1);  	   	 	
			YUV420_UYVY422_C(pDst, pYSrc, pUSrc, pVSrc, in_stride, in_UVStride, w_offset, out_height, out_stride);
		} 	 	
	}
	return VO_ERR_NONE;
}

#ifdef COLCONVENABLE
VOCCRETURNCODE YUV420PlanarToYUV420Pack_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 out_stride = conv_data->nOutStride;	

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[0] + vy * out_stride;
		psrc = conv_data->pInBuf[0] + vy * in_stride;

		memcpy(pdst, psrc, out_width);
	}

	//uv pack
	in_stride  >>= 1;
	out_width  >>= 1;
	out_height >>= 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *pUsrc, *pVsrc, *pdst;

		pdst  = conv_data->pOutBuf[1] + vy * out_stride;
		pUsrc = conv_data->pInBuf[1]  + vy * in_stride;
		pVsrc = conv_data->pInBuf[2]  + vy * in_stride;

		for(vx = 0; vx < out_width; vx++) {
			*(pdst++) = *(pUsrc++);
			*(pdst++) = *(pVsrc++);
		}
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack2_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 out_stride = conv_data->nOutStride;	

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[0] + vy * out_stride;
		psrc = conv_data->pInBuf[0] + vy * in_stride;

		memcpy(pdst, psrc, out_width);
	}

	//uv pack
	in_stride  >>= 1;
	out_width  >>= 1;
	out_height >>= 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *pUsrc, *pVsrc, *pdst;

		pdst  = conv_data->pOutBuf[1] + vy * out_stride;
		pUsrc = conv_data->pInBuf[1]  + vy * in_stride;
		pVsrc = conv_data->pInBuf[2]  + vy * in_stride;

		for(vx = 0; vx < out_width; vx++) {
			*(pdst++) = *(pVsrc++);
			*(pdst++) = *(pUsrc++);
		}
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422PackedToYUV420Planar_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;//, rx = 0xFFFFFFFF, ry = 0xFFFFFFFF;
	const VO_U32 in_stride = conv_data->nInStride, out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 out_stride = conv_data->nOutStride;
	VO_U32 y_shift, u_shift, v_shift ;
	VO_U32 y0 = 0, y1 = 2;
	VO_S32 YUV422Id = 0;

	if (conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YUYV422_PACKED_2){
		const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YUYV422_PACKED ? 0 : 4;
		YUV422Id = IDYUYV422 + Y_switch;
	}else if (conv_data->nInType == VO_COLOR_YVYU422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2){
		const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_YVYU422_PACKED ? 0 : 4;
		YUV422Id = IDYVYU422 + Y_switch;
	}else if (conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED_2){
		const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_UYVY422_PACKED ? 0 : 4;
		YUV422Id = IDUYVY422 + Y_switch;
	}else if (conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED_2){
		const VO_S32 Y_switch = conv_data->nInType == VO_COLOR_VYUY422_PACKED ? 0 : 4;
		YUV422Id = IDVYUY422 + Y_switch;
	}

	if(YUV422Id >= 4){
		YUV422Id -= 4;
		y0 = 2;
		y1 = 0;
	}
	y_shift = (YUV422Id > 1);
	u_shift = u_shift_tab[YUV422Id];
	v_shift = v_shift_tab[YUV422Id];

	/*start of output address*/
	//	if(conv_data->nRotationType == ROTATION_DISABLE){/*disable rotation*/
	//		step_y = 3;
	//		step_x = out_stride;
	//		start_outbuf = conv_data->pOutBuf[0];
	//	}else if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
	//		step_y = -out_stride;
	//		step_x = 3;
	//		start_outbuf = conv_data->pOutBuf[0] - (out_width - 1)* step_y;
	//	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
	//		step_y = out_stride;
	//		step_x = -3;
	//		start_outbuf = conv_data->pOutBuf[0] + (out_height*3) - 3;
	//	}

	for(vy = 0; vy < out_height; vy+=2) {
		VO_U8 *outbuf_y = conv_data->pOutBuf[0] + vy*out_stride;
		VO_U8	*outbuf_u = conv_data->pOutBuf[1] + ((vy*out_stride)>>2);
		VO_U8	*outbuf_v = conv_data->pOutBuf[2] + ((vy*out_stride)>>2);

		VO_U8 *psrc  = conv_data->pInBuf[0] + vy*in_stride;
		VO_U8 *psrc1 = psrc + in_stride;	

		for(vx = 0; vx < out_width; vx += 4) {
			VO_S32 a0, a1, a2, a3;

			a0 = psrc[y_shift+y0]|(psrc[y_shift+y1]<<8)|(psrc[y_shift+y0+4]<<16)|(psrc[y_shift+y1+4]<<24);
			a3 = psrc1[y_shift+y0]|(psrc1[y_shift+y1]<<8)|(psrc1[y_shift+y0+4]<<16)|(psrc1[y_shift+y1+4]<<24);

			a1 = psrc[u_shift]|(psrc[4+u_shift]<<8);
			a2 = psrc[v_shift]|(psrc[4+v_shift]<<8);


			(*(VO_U32*)outbuf_y) = a0;
			(*(VO_U32*)(outbuf_y+out_stride)) = a3;
			(*(VO_U16*)outbuf_u) = a1;
			(*(VO_U16*)outbuf_v) = a2;
			outbuf_y += 4;
			outbuf_u += 2;
			outbuf_v += 2;
			psrc += 8;
			psrc1 += 8;

		}
	}
	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420Pack_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vy;// vx = 0,
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;

	VO_S32 out_stride = conv_data->nOutStride;
	
	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[0] + vy * out_stride;
		psrc = conv_data->pInBuf[0] + vy * in_stride;		

		memcpy(pdst, psrc, out_width);
	}

	//u
	out_height >>= 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[1] + vy * out_stride;
		psrc = conv_data->pInBuf[1] + vy * in_stride;		

		memcpy(pdst, psrc, out_width);
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422Pack_noresize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32  vy;//vx = 0,
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;

	VO_S32 out_stride = conv_data->nOutStride;
	

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[0] + vy * out_stride;
		psrc = conv_data->pInBuf[0] + vy * in_stride;
		
		memcpy(pdst, psrc, out_width<<1);
	}


	return VO_ERR_NONE;
}
#endif //COLCONVENABLE

//bilinear
VOCCRETURNCODE YUV420Planar_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_S32 out_stride = conv_data->nOutStride;
	VO_S32 in_width;// = conv_data->nInWidth - 1;
	VO_S32 in_height;// = conv_data->nInHeight - 1;

	//	if(conv_data->nRotationType == FLIP_Y){
	//		step_y = -out_stride;
	//		start_out_buf = conv_data->pOutBuf[0]+((out_height-1)*out_stride);
	//	}else{
	//		step_y = out_stride;
	//		start_out_buf = conv_data->pOutBuf[0];
	//	}

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[0] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = (VO_U8)a1;
			x_tab += 3;
		}		
		y_tab += 3;
	}


	//u
	y_tab = (VO_S32 *)y_resize_tab;
	in_stride >>= 1;
	out_stride >>= 1;
	out_height >>= 1;
	out_width >>= 1;
	in_width = (conv_data->nInWidth>>1) - 1;
	in_height = (conv_data->nInHeight>>1) - 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		aa0 = y_tab[0];
		if(aa0 >= in_height){
			aa0 = in_height-1;
		}

		psrc = conv_data->pInBuf[1] + aa0 * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[1] + vy * out_stride;

		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			if(a0 >= in_width){
				a0 = in_width-1;
				aa = 512;
				bb = 512;
			}else{
				aa = x_tab[1];
				bb = x_tab[2];
			}

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = (VO_U8)a1;
			x_tab += 3;
		}	
		y_tab += 3;
	}		

	//v
	y_tab = (VO_S32 *)y_resize_tab;
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		aa0 = y_tab[0];
		if(aa0 >= in_height)
			aa0 = in_height-1;

		psrc = conv_data->pInBuf[2] + aa0 * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[2] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			if(a0 >= in_width){
				a0 = in_width-1;
				aa = 512;
				bb = 512;
			}else{
				aa = x_tab[1];
				bb = x_tab[2];
			}

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = (VO_U8)a1;
			x_tab += 3;
		}
		y_tab += 3;
	}

	return VO_ERR_NONE;
}

#ifdef COLCONVENABLE
VOCCRETURNCODE YUV420PlanarToYUV420Pack_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_S32 out_stride = conv_data->nOutStride;
	VO_S32 in_width;
	VO_S32 in_height;

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[0] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = a1;
			x_tab += 3;
		}		
		y_tab += 3;
	}


	//u
	y_tab = (VO_S32 *)y_resize_tab;
	in_stride >>= 1;
	out_stride >>= 1;
	out_height >>= 1;
	out_width >>= 1;
	in_width = (conv_data->nInWidth>>1) - 1;
	in_height = (conv_data->nInHeight>>1) - 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		aa0 = y_tab[0];
		if(aa0 >= in_height){
			aa0 = in_height-1;
		}

		psrc = conv_data->pInBuf[1] + aa0 * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = cc_hnd->yuv_nv12[0] + vy * out_stride;

		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			if(a0 >= in_width){
				a0 = in_width-1;
				aa = 512;
				bb = 512;
			}else{
				aa = x_tab[1];
				bb = x_tab[2];
			}

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = a1;
			x_tab += 3;
		}	
		y_tab += 3;
	}		

	//v
	y_tab = (VO_S32 *)y_resize_tab;
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		aa0 = y_tab[0];
		if(aa0 >= in_height)
			aa0 = in_height-1;

		psrc = conv_data->pInBuf[2] + aa0 * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = cc_hnd->yuv_nv12[1] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			if(a0 >= in_width){
				a0 = in_width-1;
				aa = 512;
				bb = 512;
			}else{
				aa = x_tab[1];
				bb = x_tab[2];
			}

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = a1;
			x_tab += 3;
		}
		y_tab += 3;
	}

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[0], cc_hnd->yuv_nv12[1],out_width,out_height);

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack2_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_S32 out_stride = conv_data->nOutStride;
	VO_S32 in_width;
	VO_S32 in_height;

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[0] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = a1;
			x_tab += 3;
		}		
		y_tab += 3;
	}


	//u
	y_tab = (VO_S32 *)y_resize_tab;
	in_stride >>= 1;
	out_stride >>= 1;
	out_height >>= 1;
	out_width >>= 1;
	in_width = (conv_data->nInWidth>>1) - 1;
	in_height = (conv_data->nInHeight>>1) - 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		aa0 = y_tab[0];
		if(aa0 >= in_height){
			aa0 = in_height-1;
		}

		psrc = conv_data->pInBuf[1] + aa0 * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = cc_hnd->yuv_nv12[0] + vy * out_stride;

		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			if(a0 >= in_width){
				a0 = in_width-1;
				aa = 512;
				bb = 512;
			}else{
				aa = x_tab[1];
				bb = x_tab[2];
			}

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = a1;
			x_tab += 3;
		}	
		y_tab += 3;
	}		

	//v
	y_tab = (VO_S32 *)y_resize_tab;
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		aa0 = y_tab[0];
		if(aa0 >= in_height)
			aa0 = in_height-1;

		psrc = conv_data->pInBuf[2] + aa0 * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = cc_hnd->yuv_nv12[1] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_S32 a0, a1;
			VO_S32 aa, bb;

			a0 = x_tab[0];
			if(a0 >= in_width){
				a0 = in_width-1;
				aa = 512;
				bb = 512;
			}else{
				aa = x_tab[1];
				bb = x_tab[2];
			}

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;
			*(pdst++) = a1;
			x_tab += 3;
		}
		y_tab += 3;
	}

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[1], cc_hnd->yuv_nv12[0],out_width,out_height);

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420Packed_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;

	VO_S32 out_stride = conv_data->nOutStride;

	//	if(conv_data->nRotationType == FLIP_Y){
	//		step_y = -out_stride;
	//		start_out_buf = conv_data->pOutBuf[0]+((out_height-1)*out_stride);
	//	}else{
	//		step_y = out_stride;
	//		start_out_buf = conv_data->pOutBuf[0];
	//	}

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;

		pdst = conv_data->pOutBuf[0] + vy * out_stride;
		psrc = conv_data->pInBuf[0] + y_resize_tab[vy] * in_stride;

		for(vx = 0; vx < out_width; vx++) {
			VO_U8 a0;
			a0 = psrc[x_resize_tab[vx]];	
			*(pdst++) = a0;
		}		
	}


	//uv
	for(vy = 0; vy < out_height; vy += 2) {
		VO_U16 *psrc, *pdst;

		pdst = (VO_U16 *)(conv_data->pOutBuf[1] + (vy >> 1) * out_stride);
		psrc = (VO_U16 *)(conv_data->pInBuf[1] + (y_resize_tab[vy] >> 1) * in_stride);

		for(vx = 0; vx < out_width; vx += 2) {
			VO_U16 a0;
			a0 = psrc[x_resize_tab[vx]>>1];
			*(pdst++) = a0;
		}		
	}		


	return VO_ERR_NONE;
}

static VOCCRETURNCODE YUV422Packed_1_resize_anti_alias(ClrConvData *conv_data, const VO_S32 * const x_resize_tab, const VO_S32 * const y_resize_tab)
{
	VO_S32 vx = 0;
	const VO_U32 in_stride = conv_data->nInStride ;
	VO_S32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight, shift_c;
	const VO_S32 out_stride = conv_data->nOutStride;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_U8 *psrc, *pdst = conv_data->pOutBuf[0] ;

	if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED){
		shift_c = 1;
	}else{
		shift_c = 0;
	}

	do{
		VO_S32 aa0, bb0;// *x_tab = (VO_S32 *)x_resize_tab,

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		for(vx = 0; vx < out_width; vx +=2 ) {
			VO_U32 a0, a1, a2, a3, a4;
			VO_S32 aa, bb, aa1, bb1;
			VO_U8 *psrc1;

			////////uv
			a1 = (15*vx)>>1;
			a0 = x_resize_tab[a1];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 1];
			bb = x_resize_tab[a1 + 2];

			//0
			a0 += shift_c;
			a2 = ((psrc[a0]*bb + psrc[a0+4]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+4]*aa)*aa0)>>20;
			a3 = ((psrc[a0+2]*bb + psrc[a0+6]*aa)*bb0 + (psrc[a0+in_stride+2]*bb + psrc[a0+in_stride+6]*aa)*aa0)>>20;

			//1
			psrc1 = conv_data->pInBuf[0] + y_tab[3] * in_stride;
			aa1 = y_tab[4];
			bb1 = y_tab[5];

			a0 = x_resize_tab[a1 + 3];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 4];
			bb = x_resize_tab[a1 + 5];

			a0 += shift_c;
			a2 += ((psrc1[a0]*bb + psrc1[a0+4]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+4]*aa)*aa1)>>20;
			a3 += ((psrc1[a0+2]*bb + psrc1[a0+6]*aa)*bb1 + (psrc1[a0+in_stride+2]*bb + psrc1[a0+in_stride+6]*aa)*aa1)>>20;

			//2
			psrc1 = conv_data->pInBuf[0] + y_tab[6] * in_stride;
			aa1 = y_tab[7];
			bb1 = y_tab[8];

			a0 = x_resize_tab[a1 + 6];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 7];
			bb = x_resize_tab[a1 + 8];

			a0 += shift_c;
			a2 += ((psrc1[a0]*bb + psrc1[a0+4]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+4]*aa)*aa1)>>20;
			a3 += ((psrc1[a0+2]*bb + psrc1[a0+6]*aa)*bb1 + (psrc1[a0+in_stride+2]*bb + psrc1[a0+in_stride+6]*aa)*aa1)>>20;

			//3
			psrc1 = conv_data->pInBuf[0] + y_tab[9] * in_stride;
			aa1 = y_tab[10];
			bb1 = y_tab[11];

			a0 = x_resize_tab[a1 + 9];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 10];
			bb = x_resize_tab[a1 + 11];

			a0 += shift_c;
			a2 += ((psrc1[a0]*bb + psrc1[a0+4]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+4]*aa)*aa1)>>20;
			a3 += ((psrc1[a0+2]*bb + psrc1[a0+6]*aa)*bb1 + (psrc1[a0+in_stride+2]*bb + psrc1[a0+in_stride+6]*aa)*aa1)>>20;

			//4
			psrc1 = conv_data->pInBuf[0] + y_tab[12] * in_stride;
			aa1 = y_tab[13];
			bb1 = y_tab[14];

			a0 = x_resize_tab[a1 + 12];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 13];
			bb = x_resize_tab[a1 + 14];

			a0 += shift_c;
			a2 += ((psrc1[a0]*bb + psrc1[a0+4]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+4]*aa)*aa1)>>20;
			a3 += ((psrc1[a0+2]*bb + psrc1[a0+6]*aa)*bb1 + (psrc1[a0+in_stride+2]*bb + psrc1[a0+in_stride+6]*aa)*aa1)>>20;

			a4 = (shift_c << 3);
			a2 = ((a2/5) << a4) | ((a3/5) << (a4 + 16));

			////y
			a1 = a1 << 1;

			//0
			a0 = x_resize_tab[a1];
			a0 <<= 1;
			a0 += (1 - shift_c);
			a4 = (8 - a4);
			aa = x_resize_tab[a1 + 1];
			bb = x_resize_tab[a1 + 2];

			aa1 = ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;
			//			a2 |= (a3 << a4);

			a0 = x_resize_tab[a1 + 15];
			aa = x_resize_tab[a1 + 16];
			bb = x_resize_tab[a1 + 17];
			a0 <<= 1;
			a0 += (1 - shift_c);

			a3 = ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;
			//			a2 |= (a3 << (a4 + 16));

			//1

			psrc = conv_data->pInBuf[0] + y_tab[3] * in_stride;
			aa0 = y_tab[4];
			bb0 = y_tab[5];

			a0 = x_resize_tab[a1 + 3];
			a0 <<= 1;
			a0 += (1 - shift_c);

			aa = x_resize_tab[a1 + 4];
			bb = x_resize_tab[a1 + 5];

			aa1 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;

			a0 = x_resize_tab[a1 + 18];
			aa = x_resize_tab[a1 + 19];
			bb = x_resize_tab[a1 + 20];
			a0 <<= 1;
			a0 += (1 - shift_c);

			a3 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;


			//2

			psrc = conv_data->pInBuf[0] + y_tab[6] * in_stride;
			aa0 = y_tab[7];
			bb0 = y_tab[8];

			a0 = x_resize_tab[a1 + 6];
			a0 <<= 1;
			a0 += (1 - shift_c);

			aa = x_resize_tab[a1 + 7];
			bb = x_resize_tab[a1 + 8];

			aa1 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;

			a0 = x_resize_tab[a1 + 21];
			aa = x_resize_tab[a1 + 22];
			bb = x_resize_tab[a1 + 23];
			a0 <<= 1;
			a0 += (1 - shift_c);

			a3 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;

			//3

			psrc = conv_data->pInBuf[0] + y_tab[9] * in_stride;
			aa0 = y_tab[10];
			bb0 = y_tab[11];

			a0 = x_resize_tab[a1 + 9];
			a0 <<= 1;
			a0 += (1 - shift_c);

			aa = x_resize_tab[a1 + 10];
			bb = x_resize_tab[a1 + 11];

			aa1 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;

			a0 = x_resize_tab[a1 + 24];
			aa = x_resize_tab[a1 + 25];
			bb = x_resize_tab[a1 + 26];
			a0 <<= 1;
			a0 += (1 - shift_c);

			a3 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;

			//4

			psrc = conv_data->pInBuf[0] + y_tab[12] * in_stride;
			aa0 = y_tab[13];
			bb0 = y_tab[14];

			a0 = x_resize_tab[a1 + 12];
			a0 <<= 1;
			a0 += (1 - shift_c);

			aa = x_resize_tab[a1 + 13];
			bb = x_resize_tab[a1 + 14];

			aa1 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;
			a2 |= ((aa1/5) << a4);
			a0 = x_resize_tab[a1 + 27];
			aa = x_resize_tab[a1 + 28];
			bb = x_resize_tab[a1 + 29];
			a0 <<= 1;
			a0 += (1 - shift_c);

			a3 += ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;
			a2 |= ((a3/5) << (a4 + 16));
			*((VO_U32*)pdst) = a2;
			pdst += 4;
		}	
		pdst -= (out_width<<1);
		pdst += out_stride;
		y_tab += 15;
	}while(--out_height != 0);

	return VO_ERR_NONE;
}

//bilinear
static VOCCRETURNCODE YUV422Packed_1_resize(ClrConvData *conv_data, const VO_S32 * const x_resize_tab, const VO_S32 * const y_resize_tab)
{
	VO_S32 vx = 0;
	const VO_U32 in_stride = conv_data->nInStride ;
	VO_S32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight, shift_c;
	const VO_S32 out_stride = conv_data->nOutStride;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_U8 *psrc, *pdst = conv_data->pOutBuf[0] ;

	if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED){
		shift_c = 1;
	}else{
		shift_c = 0;
	}

	do{
		VO_S32 aa0, bb0;//*x_tab = (VO_S32 *)x_resize_tab, 

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		for(vx = 0; vx < out_width; vx +=2 ) {
			VO_U32 a0, a1, a2, a3, a4;
			VO_S32 aa, bb;

			a1 = (vx >> 1) + vx;
			a0 = x_resize_tab[a1];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 1];
			bb = x_resize_tab[a1 + 2];

			a0 += shift_c;
			a4 = (shift_c << 3);
			a2 = ((psrc[a0]*bb + psrc[a0+4]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+4]*aa)*aa0)>>20;
			a3 = ((psrc[a0+2]*bb + psrc[a0+6]*aa)*bb0 + (psrc[a0+in_stride+2]*bb + psrc[a0+in_stride+6]*aa)*aa0)>>20;
			a2 = (a2 << a4) | (a3 << (a4 + 16));

			a1 = a1 << 1;
			a0 = x_resize_tab[a1];
			a0 <<= 1;
			a0 += (1 - shift_c);
			a4 = (8 - a4);
			aa = x_resize_tab[a1 + 1];
			bb = x_resize_tab[a1 + 2];

			a3 = ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;
			a2 |= (a3 << a4);

			a0 = x_resize_tab[a1 + 3];
			aa = x_resize_tab[a1 + 4];
			bb = x_resize_tab[a1 + 5];
			a0 <<= 1;
			a0 += (1 - shift_c);

			a3 = ((psrc[a0]*bb + psrc[a0+2]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+2]*aa)*aa0)>>20;
			a2 |= (a3 << (a4 + 16));

			*((VO_U32*)pdst) = a2;
			pdst += 4;
		}	
		pdst -= (out_width<<1);
		pdst += out_stride;
		y_tab += 3;
	}while(--out_height != 0);

	return VO_ERR_NONE;
}

static VOCCRETURNCODE YUV422Packed_2_resize(ClrConvData *conv_data, const VO_S32 * const x_resize_tab, const VO_S32 * const y_resize_tab)
{
	VO_S32 vx = 0;
	const VO_U32 in_stride = conv_data->nInStride ;
	VO_S32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight, shift_c;
	const VO_S32 out_stride = conv_data->nOutStride;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_U8 *psrc, *pdst = conv_data->pOutBuf[0] ;

	if(conv_data->nInType == VO_COLOR_YUYV422_PACKED_2 || conv_data->nInType == VO_COLOR_YVYU422_PACKED_2){
		shift_c = 1;
	}else{
		shift_c = 0;
	}

	do{
		VO_S32 aa0, bb0;//*x_tab = (VO_S32 *)x_resize_tab, 

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		for(vx = 0; vx < out_width; vx +=2 ) {
			VO_U32 a0, a1, a2, a3, a4, a5;
			VO_S32 aa, bb;

			a1 = 3*(vx >> 1);
			a0 = x_resize_tab[a1];
			a0 <<= 2;
			aa = x_resize_tab[a1 + 1];
			bb = x_resize_tab[a1 + 2];

			a0 += shift_c;
			a4 = (shift_c << 3);
			a2 = ((psrc[a0]*bb + psrc[a0+4]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+4]*aa)*aa0)>>20;
			a3 = ((psrc[a0+2]*bb + psrc[a0+6]*aa)*bb0 + (psrc[a0+in_stride+2]*bb + psrc[a0+in_stride+6]*aa)*aa0)>>20;
			a2 = (a2 << a4) | (a3 << (a4 + 16));

			a1 = vx*3;
			a0 = x_resize_tab[a1];
			a5 = a0 + 1;
			a0 += ((a0&1) ? -1 : 1);  
			a5 += ((a5&1) ? -1 : 1); 

			a0 <<= 1;
			a0 += (1 - shift_c);
			a5 <<= 1;
			a5 += (1 - shift_c);

			a4 = (8 - a4);
			aa = x_resize_tab[a1 + 1];
			bb = x_resize_tab[a1 + 2];


			a3 = ((psrc[a0]*bb + psrc[a5]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a5+in_stride]*aa)*aa0)>>20;
			a2 |= (a3 << (a4 + 16));//(a3 << a4);//;


			a0 = x_resize_tab[a1 + 3];
			aa = x_resize_tab[a1 + 4];
			bb = x_resize_tab[a1 + 5];
			a5 = a0 + 1;
			a0 += ((a0&1) ? -1 : 1);  
			a5 += ((a5&1) ? -1 : 1); 

			a0 <<= 1;
			a0 += (1 - shift_c);
			a5 <<= 1;
			a5 += (1 - shift_c);


			a3 = ((psrc[a0]*bb + psrc[a5]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a5+in_stride]*aa)*aa0)>>20;
			a2 |= (a3 << a4);//(a3 << (a4 + 16));//;

			*((VO_U32*)pdst) = a2;
			pdst += 4;
		}	
		pdst -= (out_width<<1);
		pdst += out_stride;
		y_tab += 3;
	}while(--out_height != 0);

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422Packed_resize(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED ||
		conv_data->nInType == VO_COLOR_VYUY422_PACKED || conv_data->nInType == VO_COLOR_UYVY422_PACKED ){
			if(conv_data->nAntiAliasLevel){
				YUV422Packed_1_resize_anti_alias(conv_data, x_resize_tab, y_resize_tab);
			}else{
				YUV422Packed_1_resize(conv_data, x_resize_tab, y_resize_tab);
			}
	}else{
		YUV422Packed_2_resize(conv_data, x_resize_tab, y_resize_tab);
	}

	return VO_ERR_NONE;
}
#endif //COLCONVENABLE

void Yuv2yuv_R90_inter_c (VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride)
{
	unsigned int * pSourBuf1, * pSourBuf2, * pSourBuf3, * pSourBuf4;
	unsigned int nPix1, nPix2, nPix3, nPix4;
	int i, j;

	unsigned int * pDestStart	= (unsigned int * )(pOutput + nOutStride - 4);
	unsigned int * pDestOut	=  pDestStart;

	for (i = 0; i < nHeight; i+=4)
	{
		pSourBuf1 = (unsigned int *)(pInput + i * nInStride);
		pSourBuf2 = (unsigned int *)(pInput + (i + 1) * nInStride);
		pSourBuf3 = (unsigned int *)(pInput + (i + 2) * nInStride);
		pSourBuf4 = (unsigned int *)(pInput + (i + 3) * nInStride);

		pDestOut = pDestStart - (i >> 2);

		for (j = 0; j < nWidth; j+=4)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;
			nPix3 = *pSourBuf3++;
			nPix4 = *pSourBuf4++;

			*pDestOut = (nPix4 & 0XFF) + ((nPix3 & 0XFF) << 8) + ((nPix2 & 0XFF) << 16) + ((nPix1 & 0XFF) << 24);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix4 & 0XFF00) >> 8) + ((nPix3 & 0XFF00)) + ((nPix2 & 0XFF00) << 8) + ((nPix1 & 0XFF00) << 16);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix4 & 0XFF0000) >> 16) + ((nPix3 & 0XFF0000) >> 8) + ((nPix2 & 0XFF0000)) + ((nPix1 & 0XFF0000) << 8);
			pDestOut += nOutStride / 4;

			*pDestOut = ((nPix4 & 0XFF000000) >> 24) + ((nPix3 & 0XFF000000) >> 16) + ((nPix2 & 0XFF000000) >> 8) + ((nPix1 & 0XFF000000));
			pDestOut += nOutStride / 4;
		}
	}
}

void Yuv2yuv_L90_inter_c (VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride)
{
	unsigned int * pSourBuf1, * pSourBuf2, * pSourBuf3, * pSourBuf4;
	unsigned int nPix1, nPix2, nPix3, nPix4;
	int i, j;

	unsigned int * pDestStart	= (unsigned int * )(pOutput + (nWidth - 1) * nOutStride);
	unsigned int * pDestOut	=  pDestStart;

	for (i = 0; i < nHeight; i+=4)
	{
		pSourBuf1 = (unsigned int *)(pInput + i * nInStride);
		pSourBuf2 = (unsigned int *)(pInput + (i + 1) * nInStride);
		pSourBuf3 = (unsigned int *)(pInput + (i + 2) * nInStride);
		pSourBuf4 = (unsigned int *)(pInput + (i + 3) * nInStride);

		pDestOut = pDestStart + (i >> 2);

		for (j = 0; j < nWidth; j+=4)
		{
			nPix1 = *pSourBuf1++;
			nPix2 = *pSourBuf2++;
			nPix3 = *pSourBuf3++;
			nPix4 = *pSourBuf4++;

			*pDestOut = (nPix1 & 0XFF) + ((nPix2 & 0XFF) << 8) + ((nPix3 & 0XFF) << 16) + ((nPix4 & 0XFF) << 24);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFF00) >> 8) + ((nPix2 & 0XFF00)) + ((nPix3 & 0XFF00) << 8) + ((nPix4 & 0XFF00) << 16);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFF0000) >> 16) + ((nPix2 & 0XFF0000) >> 8) + ((nPix3 & 0XFF0000)) + ((nPix4 & 0XFF0000) << 8);
			pDestOut -= nOutStride / 4;

			*pDestOut = ((nPix1 & 0XFF000000) >> 24) + ((nPix2 & 0XFF000000) >> 16) + ((nPix3 & 0XFF000000) >> 8) + ((nPix4 & 0XFF000000));
			pDestOut -= nOutStride / 4;
		}
	}
}

static void Yuv2yuv_L90 (VO_U8 *y, VO_U8 *u, VO_U8 *v,VO_S32 in_width, VO_S32 in_height, ClrConvData *conv_data, VO_S32 out_width,
						 VO_S32 out_height, VO_U8 *out_buf, VO_S32 out_stride, VO_U8 *mb_skip)
{
	VO_S32 in_stride, uin_stride, vin_stride;

	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;
	//add end
	Yuv2yuv_L90_inter (y, in_width, in_height, in_stride, conv_data->pOutBuf[0], conv_data->nOutStride);
	Yuv2yuv_L90_inter (u, in_width/2, in_height/2, uin_stride, conv_data->pOutBuf[1], conv_data->nOutStride/2);
	Yuv2yuv_L90_inter (v, in_width/2, in_height/2, vin_stride, conv_data->pOutBuf[2], conv_data->nOutStride/2);

}
static void Yuv2yuv_R90  (VO_U8 *y, VO_U8 *u, VO_U8 *v,VO_S32 in_width, VO_S32 in_height, ClrConvData *conv_data, VO_S32 out_width,
						  VO_S32 out_height, VO_U8*out_buf, VO_S32 out_stride, VO_U8 *mb_skip)
{
	VO_S32 in_stride, uin_stride, vin_stride;

	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;
	//add end
	Yuv2yuv_R90_inter (y, in_width, in_height, in_stride, conv_data->pOutBuf[0], conv_data->nOutStride);
	Yuv2yuv_R90_inter (u, in_width/2, in_height/2, uin_stride, conv_data->pOutBuf[1], conv_data->nOutStride/2);
	Yuv2yuv_R90_inter (v, in_width/2, in_height/2, vin_stride, conv_data->pOutBuf[2], conv_data->nOutStride/2);
}
VOCCRETURNCODE YUV420Planar_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *y, *u, *v;
	VO_S32 in_width  = conv_data->nInWidth;
	VO_S32 in_height = conv_data->nInHeight;
	VO_S32 out_width = conv_data->nOutWidth;
	VO_S32 out_height= conv_data->nOutHeight;
	//VO_S32 in_stride  = conv_data->nInStride;

	y = conv_data->pInBuf[0];
	u = conv_data->pInBuf[1];
	v = conv_data->pInBuf[2];

	if(ROTATION_90L == conv_data->nRotationType){
		Yuv2yuv_L90 (y, u, v, in_width, in_height, conv_data, out_width, out_height,
			conv_data->pOutBuf[0], conv_data->nOutStride, conv_data->mb_skip);
	}else{
		Yuv2yuv_R90 (y, u, v, in_width, in_height, conv_data, out_width, out_height,
			conv_data->pOutBuf[0], conv_data->nOutStride, conv_data->mb_skip);
	}


	return VO_ERR_NONE;
}

void Yuv2yuv_180_inter_c (VO_U8 * pInput, VO_S32 nWidth, VO_S32 nHeight, VO_S32 nInStride, VO_U8 * pOutput, VO_S32 nOutStride)
{
	int * pSourBuf = (int *)pInput;
	int * pDestBuf = (int *)pOutput;

	int nPix = 0;
	int i, j;

	for (i = 0; i < nHeight; i++)
	{
		pSourBuf = (int *)(pInput + i * nInStride + nWidth - 4);
		pDestBuf = (int *)(pOutput + (nHeight -i -1) * nOutStride);

		for (j = 0; j < nWidth; j+=4)
		{
			nPix = *pSourBuf--;
			*pDestBuf++ = (nPix & 0XFF) << 24 | (nPix & 0XFF00) << 8 | (nPix & 0XFF0000) >> 8 | (nPix & 0XFF000000) >> 24;
		}
	}
}

VOCCRETURNCODE YUV420Planar_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 in_stride, uin_stride, vin_stride;
	VO_S32 in_width = conv_data->nInWidth;
	VO_S32 in_height= conv_data->nInHeight;
	//VO_U8 *out_buf = conv_data->pOutBuf[0];


	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;	
	//add end
	Yuv2yuv_180_inter (conv_data->pInBuf[0], in_width, in_height, in_stride, conv_data->pOutBuf[0], in_stride);
	Yuv2yuv_180_inter (conv_data->pInBuf[1], in_width/2, in_height/2, uin_stride, conv_data->pOutBuf[1], uin_stride);
	Yuv2yuv_180_inter (conv_data->pInBuf[2], in_width/2, in_height/2, vin_stride, conv_data->pOutBuf[2], vin_stride);
	return VO_ERR_NONE;

}

static VOCCRETURNCODE YPlanar_resize_RT90(ROTATIONTYPE Rt, VO_U8 *src, const VO_U32 in_height, const VO_U32 in_stride, VO_U8 *dst,
										  const VO_U32 out_width, const VO_U32 out_height, const VO_U32 out_stride,
										  const VO_S32 *x_resize_tab, const VO_S32 *y_resize_tab)
{
	VO_U32 vx = 0, vy;

	VO_U8 *out_buf, *start_outbuf=NULL;

	VO_S32 step_y=0, step_x=0;

	/*start of output address*/
	if(Rt == ROTATION_90L){/*rotation90L*/
		step_x = 4;
		step_y = -((VO_S32)out_stride);
		start_outbuf = dst + out_stride * (out_width - 1);
	}else if(Rt == ROTATION_90R){/*rotation90R*/
		step_x = -4;
		step_y = out_stride;
		start_outbuf = dst + out_height - 4 ;
	}

	//y
	//out_height: multiple of 4
	for(vy = 0; vy < out_height; vy+=4) {
		VO_U8 *psrc0, *psrc1, *psrc2, *psrc3;
		out_buf = start_outbuf;

		if(Rt == ROTATION_90L){
			psrc0 = src + y_resize_tab[vy] * in_stride;
			psrc1 = src + y_resize_tab[vy + 1] * in_stride;
			psrc2 = src + y_resize_tab[vy + 2] * in_stride;
			psrc3 = src + y_resize_tab[vy + 3] * in_stride;
		}else{
			psrc3 = src + y_resize_tab[vy] * in_stride;
			psrc2 = src + y_resize_tab[vy + 1] * in_stride;
			psrc1 = src + y_resize_tab[vy + 2] * in_stride;
			psrc0 = src + y_resize_tab[vy + 3] * in_stride;
		}

		for(vx = 0; vx < out_width;) {
			VO_U32 a0, a1;
			a0 = x_resize_tab[vx];

			a1 = psrc0[a0];
			a1 |= (psrc1[a0] << 8);
			a1 |= (psrc2[a0] << 16);
			a1 |= (psrc3[a0] << 24);

			do{
				*((VO_U32*)out_buf) = a1;
				out_buf += step_y;
			}while((x_resize_tab[++vx] == a0) && (vx < out_width));

		}	
		start_outbuf += step_x;
	}


	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420Planar_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//U
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		conv_data->pOutBuf[1], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	//V
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[2], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		conv_data->pOutBuf[2], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);
	return VO_ERR_NONE;
}

static VOCCRETURNCODE YPlanar_resize_RT180(ROTATIONTYPE Rt, VO_U8 *src, const VO_U32 in_height, const VO_U32 in_stride,
										   VO_U8 *dst, const VO_U32 out_width, const VO_U32 out_height, const VO_U32 out_stride,
										   const VO_S32 *x_resize_tab, const VO_S32 *y_resize_tab)
{
	VO_U32 vx = 0, vy;
	VO_U8 *out_buf, *start_outbuf;
	VO_S32 step_y, step_x;

	step_x = -((VO_S32)out_stride);
	step_y = -4;
	start_outbuf = dst + out_stride*out_height - 4 ;

	//y
	//out_height: multiple of 4
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc;
		out_buf = start_outbuf;

		psrc = src + y_resize_tab[vy] * in_stride;

		for(vx = 0; vx < out_width; vx += 4) {
			VO_U32 a0,a1,a2,a3; 
			VO_U32 b0;
			a0 = x_resize_tab[vx];
			a1 = x_resize_tab[vx+1];
			a2 = x_resize_tab[vx+2];
			a3 = x_resize_tab[vx+3];

			b0  = (psrc[a0] << 24);
			b0 |= (psrc[a1] << 16);
			b0 |= (psrc[a2] << 8);
			b0 |= (psrc[a3]);

			*((VO_U32*)out_buf) = b0;
			out_buf += step_y;
		}	
		start_outbuf += step_x;
	}

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420Planar_resize_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//U
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		conv_data->pOutBuf[1], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	//V
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[2], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		conv_data->pOutBuf[2], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);
	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420Planar_resize_anti_aliasing(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	VO_U32 in_stride = conv_data->nInStride ;
	VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	VO_S32 *y_tab = (VO_S32 *)y_resize_tab;
	VO_S32 out_stride = conv_data->nOutStride;

	//	if(conv_data->nRotationType == FLIP_Y){
	//		step_y = -out_stride;
	//		start_out_buf = conv_data->pOutBuf[0]+((out_height-1)*out_stride);
	//	}else{
	//		step_y = out_stride;
	//		start_out_buf = conv_data->pOutBuf[0];
	//	}

	//y
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		psrc = conv_data->pInBuf[0] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[0] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1, a2, a3, a4;
			VO_S32 aa, bb, aa1, bb1;
			VO_U8 *psrc1;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;

			//1
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[0] + y_tab[3] * in_stride;
			aa1 = y_tab[4];
			bb1 = y_tab[5];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a2 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//2
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[0] + y_tab[6] * in_stride;
			aa1 = y_tab[7];
			bb1 = y_tab[8];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a3 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//3
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[0] + y_tab[9] * in_stride;
			aa1 = y_tab[10];
			bb1 = y_tab[11];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a4 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//4
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[0] + y_tab[12] * in_stride;
			aa1 = y_tab[13];
			bb1 = y_tab[14];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a0 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;


			*(pdst++) = (VO_U8)((a0 + a1 + a2 + a3 + a4) / 5);
			x_tab += 3;	

		}		
		y_tab += 15;
	}


	//u
	y_tab = (VO_S32 *)y_resize_tab;
	in_stride >>= 1;
	out_stride >>= 1;
	out_height >>= 1;
	out_width >>= 1;

	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab =(VO_S32 *) x_resize_tab, aa0, bb0;

		psrc = conv_data->pInBuf[1] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[1] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1, a2, a3, a4;
			VO_S32 aa, bb, aa1, bb1;
			VO_U8 *psrc1;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;

			//1
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[1] + y_tab[3] * in_stride;
			aa1 = y_tab[4];
			bb1 = y_tab[5];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a2 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//2
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[1] + y_tab[6] * in_stride;
			aa1 = y_tab[7];
			bb1 = y_tab[8];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a3 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//3
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[1] + y_tab[9] * in_stride;
			aa1 = y_tab[10];
			bb1 = y_tab[11];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a4 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//4
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[1] + y_tab[12] * in_stride;
			aa1 = y_tab[13];
			bb1 = y_tab[14];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a0 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;


			*(pdst++) = (VO_U8)((a0 + a1 + a2 + a3 + a4) / 5);
			x_tab += 3;	

		}		
		y_tab += 15;
	}		

	//v
	y_tab = (VO_S32 *)y_resize_tab;
	for(vy = 0; vy < out_height; vy++) {
		VO_U8 *psrc, *pdst;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab, aa0, bb0;

		psrc = conv_data->pInBuf[2] + y_tab[0] * in_stride;
		aa0 = y_tab[1];
		bb0 = y_tab[2];

		pdst = conv_data->pOutBuf[2] + vy * out_stride;


		for(vx = 0; vx < out_width; vx++) {
			VO_U32 a0, a1, a2, a3, a4;
			VO_S32 aa, bb, aa1, bb1;
			VO_U8 *psrc1;

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];

			a1 = ((psrc[a0]*bb + psrc[a0+1]*aa)*bb0 + (psrc[a0+in_stride]*bb + psrc[a0+in_stride+1]*aa)*aa0)>>20;

			//1
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[2] + y_tab[3] * in_stride;
			aa1 = y_tab[4];
			bb1 = y_tab[5];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a2 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//2
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[2] + y_tab[6] * in_stride;
			aa1 = y_tab[7];
			bb1 = y_tab[8];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a3 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//3
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[2] + y_tab[9] * in_stride;
			aa1 = y_tab[10];
			bb1 = y_tab[11];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a4 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;

			//4
			x_tab += 3;	
			psrc1 = conv_data->pInBuf[2] + y_tab[12] * in_stride;
			aa1 = y_tab[13];
			bb1 = y_tab[14];

			a0 = x_tab[0];
			aa = x_tab[1];
			bb = x_tab[2];
			a0 = ((psrc1[a0]*bb + psrc1[a0+1]*aa)*bb1 + (psrc1[a0+in_stride]*bb + psrc1[a0+in_stride+1]*aa)*aa1)>>20;


			*(pdst++) = (VO_U8)((a0 + a1 + a2 + a3 + a4) / 5);
			x_tab += 3;	

		}		
		y_tab += 15;
	}

	return VO_ERR_NONE;
}

#ifdef COLCONVENABLE
static void YuvToyuvPack_L90 (VO_U8 *y, VO_U8 *u, VO_U8 *v,VO_S32 in_width, VO_S32 in_height, ClrConvData *conv_data, VO_S32 out_width,
							  VO_S32 out_height, VO_U8 *out_buf, VO_S32 out_stride, VO_U8 *mb_skip,struct CC_HND * cc_hnd)
{
	VO_S32 in_stride, uin_stride, vin_stride;

	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;
	//add end
	Yuv2yuv_L90_inter (y, in_width, in_height, in_stride, conv_data->pOutBuf[0], conv_data->nOutStride);
	Yuv2yuv_L90_inter (u, in_width/2, in_height/2, uin_stride, cc_hnd->yuv_nv12[0], conv_data->nOutStride/2);
	Yuv2yuv_L90_inter (v, in_width/2, in_height/2, vin_stride, cc_hnd->yuv_nv12[1], conv_data->nOutStride/2);

}
static void YuvToyuvPack_R90  (VO_U8 *y, VO_U8 *u, VO_U8 *v,VO_S32 in_width, VO_S32 in_height, ClrConvData *conv_data, VO_S32 out_width,
							   VO_S32 out_height, VO_U8*out_buf, VO_S32 out_stride, VO_U8 *mb_skip,struct CC_HND * cc_hnd)
{
	VO_S32 in_stride, uin_stride, vin_stride;

	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;
	//add end
	Yuv2yuv_R90_inter (y, in_width, in_height, in_stride, conv_data->pOutBuf[0], conv_data->nOutStride);
	Yuv2yuv_R90_inter (u, in_width/2, in_height/2, uin_stride, cc_hnd->yuv_nv12[0], conv_data->nOutStride/2);
	Yuv2yuv_R90_inter (v, in_width/2, in_height/2, vin_stride, cc_hnd->yuv_nv12[1], conv_data->nOutStride/2);
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *y, *u, *v;
	VO_S32 in_width  = conv_data->nInWidth;
	VO_S32 in_height = conv_data->nInHeight;
	VO_S32 out_width = conv_data->nOutWidth;
	VO_S32 out_height= conv_data->nOutHeight;
	//VO_S32 in_stride  = conv_data->nInStride;

	y = conv_data->pInBuf[0];
	u = conv_data->pInBuf[1];
	v = conv_data->pInBuf[2];

	if(ROTATION_90L == conv_data->nRotationType){
		YuvToyuvPack_L90 (y, u, v, in_width, in_height, conv_data, out_width, out_height,
			conv_data->pOutBuf[0], conv_data->nOutStride, conv_data->mb_skip,cc_hnd);
	}else{
		YuvToyuvPack_R90 (y, u, v, in_width, in_height, conv_data, out_width, out_height,
			conv_data->pOutBuf[0], conv_data->nOutStride, conv_data->mb_skip,cc_hnd);
	}

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[0], cc_hnd->yuv_nv12[1], out_height >> 1, out_width >> 1);


	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 in_stride, uin_stride, vin_stride;
	VO_S32 in_width = conv_data->nInWidth;
	VO_S32 in_height= conv_data->nInHeight;
	//VO_U8 *out_buf = conv_data->pOutBuf[0];


	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;	
	//add end
	Yuv2yuv_180_inter (conv_data->pInBuf[0], in_width, in_height, in_stride, conv_data->pOutBuf[0], in_stride);
	Yuv2yuv_180_inter (conv_data->pInBuf[1], in_width/2, in_height/2, uin_stride, cc_hnd->yuv_nv12[0], uin_stride);
	Yuv2yuv_180_inter (conv_data->pInBuf[2], in_width/2, in_height/2, vin_stride, cc_hnd->yuv_nv12[1], vin_stride);

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[0], cc_hnd->yuv_nv12[1], conv_data->nOutWidth >> 1, conv_data->nOutHeight >> 1);

	return VO_ERR_NONE;

}


VOCCRETURNCODE YUV420PlanarToYUV420Pack_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//U
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[0], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	//V
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[2], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[1], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[0], cc_hnd->yuv_nv12[1], conv_data->nOutHeight >> 1, conv_data->nOutWidth >> 1);

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PLANARToYUV420PACK_resize_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//U
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[0], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	//V
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[2], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[1], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[0], cc_hnd->yuv_nv12[1], conv_data->nOutWidth >> 1, conv_data->nOutHeight >> 1);

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack2_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U8 *y, *u, *v;
	VO_S32 in_width  = conv_data->nInWidth;
	VO_S32 in_height = conv_data->nInHeight;
	VO_S32 out_width = conv_data->nOutWidth;
	VO_S32 out_height= conv_data->nOutHeight;
	//VO_S32 in_stride  = conv_data->nInStride;

	y = conv_data->pInBuf[0];
	u = conv_data->pInBuf[1];
	v = conv_data->pInBuf[2];

	if(ROTATION_90L == conv_data->nRotationType){
		YuvToyuvPack_L90 (y, u, v, in_width, in_height, conv_data, out_width, out_height,
			conv_data->pOutBuf[0], conv_data->nOutStride, conv_data->mb_skip,cc_hnd);
	}else{
		YuvToyuvPack_R90 (y, u, v, in_width, in_height, conv_data, out_width, out_height,
			conv_data->pOutBuf[0], conv_data->nOutStride, conv_data->mb_skip,cc_hnd);
	}

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[1], cc_hnd->yuv_nv12[0], out_height >> 1, out_width >> 1);


	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack2_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_S32 in_stride, uin_stride, vin_stride;
	VO_S32 in_width = conv_data->nInWidth;
	VO_S32 in_height= conv_data->nInHeight;
	//VO_U8 *out_buf = conv_data->pOutBuf[0];


	in_stride  = conv_data->nInStride;
	uin_stride = conv_data->nInUVStride;
	vin_stride = conv_data->nInUVStride;	
	//add end
	Yuv2yuv_180_inter (conv_data->pInBuf[0], in_width, in_height, in_stride, conv_data->pOutBuf[0], in_stride);
	Yuv2yuv_180_inter (conv_data->pInBuf[1], in_width/2, in_height/2, uin_stride, cc_hnd->yuv_nv12[0], uin_stride);
	Yuv2yuv_180_inter (conv_data->pInBuf[2], in_width/2, in_height/2, vin_stride, cc_hnd->yuv_nv12[1], vin_stride);

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[1], cc_hnd->yuv_nv12[0], conv_data->nOutWidth >> 1, conv_data->nOutHeight >> 1);

	return VO_ERR_NONE;

}


VOCCRETURNCODE YUV420PlanarToYUV420Pack2_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//U
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[0], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	//V
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[2], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[1], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[1], cc_hnd->yuv_nv12[0], conv_data->nOutHeight >> 1, conv_data->nOutWidth >> 1);

	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV420PlanarToYUV420Pack2_resize_RT180(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//U
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[0], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	//V
	YPlanar_resize_RT180(conv_data->nRotationType, conv_data->pInBuf[2], conv_data->nInHeight>>1 , conv_data->nInStride>>1 , 
		cc_hnd->yuv_nv12[1], conv_data->nOutWidth>>1, conv_data->nOutHeight>>1, conv_data->nOutStride>>1,
		x_resize_tab, y_resize_tab);

	UVPlanarToNV12(conv_data, cc_hnd->yuv_nv12[1], cc_hnd->yuv_nv12[0], conv_data->nOutWidth >> 1, conv_data->nOutHeight >> 1);

	return VO_ERR_NONE;
}

void YUVPlanarRt90L(unsigned char *src, unsigned char *dst, const int width, const int height, const int in_stride, const int out_stride)
{

	unsigned char *p_src0 = src;
	unsigned char *p_src1 = src + (in_stride<<1);

	unsigned char *start_out = dst + (width - 1)*out_stride;

	unsigned int a, b, c, d, e, f;
	VO_S32 vx, vy;

	for(vy = 0; vy < height; vy+=4) {
		unsigned char *out_dst = start_out + vy;

		for(vx = 0; vx < width; vx+=4) {

			a = *(unsigned int*)p_src0;			//4p
			c = *(unsigned int*)p_src1;
			b = *(unsigned int*)(p_src0 + in_stride);			
			d = *(unsigned int*)(p_src1 + in_stride);

			p_src0 += 4;
			p_src1 += 4;

			e = a & 0xFF00FF;
			f = b & 0xFF00FF;
			f = (f << 0x8);
			e = e | f;
			a = a & 0xFF00FF00;
			b = b & 0xFF00FF00;
			a = (a >> 0x8);
			f = a | b;

			a = c & 0xFF00FF;
			b = d & 0xFF00FF;
			b = (b << 0x8);
			a = a | b;
			c = c & 0xFF00FF00;
			d = d & 0xFF00FF00;
			c = (c >> 0x8);
			b = c | d;		

			c = e & 0xFFFF;
			d = a & 0xFFFF;

			d = (d << 16);
			c = c | d;					//0

			e = (e >> 16);
			a = a & 0xFFFF0000;
			e = e | a;					//1


			d = f & 0xFFFF;
			a = b & 0xFFFF;

			a = (a << 16);
			d = d | a;					//2

			f = (f >> 16);
			b = b & 0xFFFF0000;
			f = f | b;					//3

			(*(unsigned int *)out_dst) = c;	
			out_dst -= out_stride;
			(*(unsigned int *)out_dst) = d;	
			out_dst -= out_stride;
			(*(unsigned int *)out_dst) = e;	
			out_dst -= out_stride;
			(*(unsigned int *)out_dst) = f;	
			out_dst -= out_stride;
		}
		p_src0 -= width;
		p_src0 += in_stride << 2;
		p_src1 = (p_src0 + (in_stride<<1));
	}
}

void YUVPlanarRt90R(unsigned char *src, unsigned char *dst, const int width, const int height, const int in_stride, const int out_stride)
{

	unsigned char *p_src0 = src + in_stride * (height - 1);
	unsigned char *p_src1 = p_src0 - (in_stride<<1);

	unsigned char *start_out = dst;

	unsigned int a, b, c, d, e, f;
	VO_S32 vx, vy;

	for(vy = 0; vy < height; vy+=4) {
		unsigned char *out_dst = start_out + vy;

		for(vx = 0; vx < width; vx+=4) {

			a = *(unsigned int*)p_src0;			//4p
			c = *(unsigned int*)p_src1;
			b = *(unsigned int*)(p_src0 - in_stride);			
			d = *(unsigned int*)(p_src1 - in_stride);

			p_src0 += 4;
			p_src1 += 4;

			e = a & 0xFF00FF;
			f = b & 0xFF00FF;
			f = (f << 0x8);
			e = e | f;
			a = a & 0xFF00FF00;
			b = b & 0xFF00FF00;
			a = (a >> 0x8);
			f = a | b;

			a = c & 0xFF00FF;
			b = d & 0xFF00FF;
			b = (b << 0x8);
			a = a | b;
			c = c & 0xFF00FF00;
			d = d & 0xFF00FF00;
			c = (c >> 0x8);
			b = c | d;		

			c = e & 0xFFFF;
			d = a & 0xFFFF;

			d = (d << 16);
			c = c | d;					//0

			e = (e >> 16);
			a = a & 0xFFFF0000;
			e = e | a;					//1


			d = f & 0xFFFF;
			a = b & 0xFFFF;

			a = (a << 16);
			d = d | a;					//2

			f = (f >> 16);
			b = b & 0xFFFF0000;
			f = f | b;					//3

			(*(unsigned int *)out_dst) = c;	
			out_dst += out_stride;
			(*(unsigned int *)out_dst) = d;	
			out_dst += out_stride;
			(*(unsigned int *)out_dst) = e;	
			out_dst += out_stride;
			(*(unsigned int *)out_dst) = f;	
			out_dst += out_stride;
		}
		p_src0 -= width;
		p_src0 -= (in_stride << 2);
		p_src1 = (p_src0 - (in_stride<<1));
	}
}

/* four pixel together*/
static void YUVPlanarRt90(ROTATIONTYPE Rt, VO_U8 *src, const VO_U32 in_height, const VO_U32 in_stride, VO_U8 *dst,
						  const VO_U32 out_width, const VO_U32 out_height, const VO_U32 out_stride)
{

	VO_U8 *p_src0=NULL;
	VO_U8 *p_src1;
	VO_U8 *start_out=NULL;
	VO_S32 step_x=0, step_y=0;
	VO_U32 vx, vy;

	/*start of output address*/
	if(Rt == ROTATION_90L){/*rotation90L*/
		step_x = in_stride;
		step_y = -((VO_S32)out_stride);
		p_src0 = src;
		start_out = dst + out_stride * (out_width - 1);
	}else if(Rt == ROTATION_90R){/*rotation90R*/
		step_x = -((VO_S32)in_stride);
		step_y = out_stride;
		p_src0 = src + in_stride * (in_height - 1);
		start_out = dst;
	}

	p_src1 = p_src0 + (step_x<<1);

	for(vy = 0; vy < out_height; vy+=4) {
		VO_U8 *out_dst = start_out + vy;
		VO_U32 a, b, c, d, e, f;		
		for(vx = 0; vx < out_width; vx+=4) {

			a = *(VO_U32*)p_src0;			//4p
			c = *(VO_U32*)p_src1;
			b = *(VO_U32*)(p_src0 + step_x);			
			d = *(VO_U32*)(p_src1 + step_x);

			p_src0 += 4;
			p_src1 += 4;

			e = a & 0xFF00FF;
			f = b & 0xFF00FF;
			f = (f << 0x8);
			e = e | f;
			a = a & 0xFF00FF00;
			b = b & 0xFF00FF00;
			a = (a >> 0x8);
			f = a | b;

			a = c & 0xFF00FF;
			b = d & 0xFF00FF;
			b = (b << 0x8);
			a = a | b;
			c = c & 0xFF00FF00;
			d = d & 0xFF00FF00;
			c = (c >> 0x8);
			b = c | d;		

			c = e & 0xFFFF;
			d = a & 0xFFFF;

			d = (d << 16);
			c = c | d;					//0

			e = (e >> 16);
			a = a & 0xFFFF0000;
			e = e | a;					//1


			d = f & 0xFFFF;
			a = b & 0xFFFF;

			a = (a << 16);
			d = d | a;					//2

			f = (f >> 16);
			b = b & 0xFFFF0000;
			f = f | b;					//3

			(*(VO_U32 *)out_dst) = c;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = d;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = e;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = f;	
			out_dst += step_y;
		}
		p_src0 -= out_width;
		p_src0 += (step_x<<2);
		p_src1 = p_src0 + (step_x<<1);
	}
}

static void UVPacked_Rt90(ROTATIONTYPE Rt, VO_U8 *src, const VO_U32 in_height, const VO_U32 in_stride, VO_U8 *dst,
						  const VO_U32 out_width, const VO_U32 out_height, const VO_U32 out_stride)
{

	VO_U8 *p_src0;
	VO_U8 *p_src1;
	VO_U8 *start_out;
	VO_S32 step_x, step_y;
	VO_U32 vx, vy;

	/*start of output address*/
	if(Rt == ROTATION_90L){/*rotation90L*/
		step_x = in_stride;
		step_y = -((VO_S32)out_stride);
		p_src0 = src;
		start_out = dst + out_stride * (out_width - 1);
	}else if(Rt == ROTATION_90R){/*rotation90R*/
		step_x = -((VO_S32)in_stride);
		step_y = out_stride;
		p_src0 = src + in_stride * (in_height - 1);
		start_out = dst;
	}

	p_src1 = p_src0 + step_x;


	for(vy = 0; vy < out_height; vy+=2) {
		VO_U8 *out_dst = start_out + vy*2;
		VO_U32 a, b, c;		
		for(vx = 0; vx < out_width; vx+=2) {

			a = *(VO_U32*)p_src0;			//4p
			b = *(VO_U32*)p_src1;

			p_src0 += 4;
			p_src1 += 4;

			c = (a & 0xFFFF) | (b << 16);
			a = (a >> 16) | (b & 0xFFFF0000);

			(*(VO_U32 *)out_dst) = c;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = a;	
			out_dst += step_y;
		}
		p_src0 -= (out_width<<1);
		p_src0 += (step_x<<1);
		p_src1 = p_src0 + step_x;
	}
}


VOCCRETURNCODE YUVPacked_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YUVPlanarRt90(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight, conv_data->nInStride,
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride);
	//U
	UVPacked_Rt90(conv_data->nRotationType, conv_data->pInBuf[1], (conv_data->nInHeight>>1), conv_data->nInStride,
		conv_data->pOutBuf[1], (conv_data->nOutWidth>>1), (conv_data->nOutHeight>>1), conv_data->nOutStride);
	//V
	//	YUVPlanarRt90(conv_data->nRotationType, conv_data->pInBuf[2], (conv_data->nInHeight>>1), (conv_data->nInStride>>1),
	//		conv_data->pOutBuf[2], (conv_data->nOutWidth>>1), (conv_data->nOutHeight>>1), (conv_data->nOutStride>>1));
	return VO_ERR_NONE;
}

static void UVPacked_resize_Rt90(ROTATIONTYPE Rt, VO_U8 *src, const VO_U32 in_height, const VO_U32 in_stride, VO_U8 *dst,
								 const VO_U32 out_width, const VO_U32 out_height, const VO_U32 out_stride,
								 const VO_S32 *x_resize_tab, const VO_S32 *y_resize_tab)
{

	VO_U16 *p_src0;
	VO_U16 *p_src1;
	VO_U8 *start_out=NULL;
	VO_S32 step_x=0, step_y=0, step_y2=0;
	VO_U32 vx, vy;

	/*start of output address*/
	if(Rt == ROTATION_90L){/*rotation90L*/
		step_x = 1;
		step_y2 = 0;
		step_y = -((VO_S32)out_stride);
		start_out = dst + out_stride * (out_width - 1);
	}else if(Rt == ROTATION_90R){/*rotation90R*/
		step_x = -1;
		step_y2 = out_height - 1;
		step_y = out_stride;
		start_out = dst;
	}


	for(vy = 0; vy < out_height; vy+=2) {
		VO_U8 *out_dst = start_out + vy*2;
		VO_U32 a, b, c, d;		
		p_src0 = (VO_U16*)(src + y_resize_tab[step_y2]*in_stride);
		p_src1 = (VO_U16*)(src + y_resize_tab[step_y2 + step_x]*in_stride);

		for(vx = 0; vx < out_width; vx+=2) {

			a = p_src0[x_resize_tab[vx]];			//4p
			b = p_src1[x_resize_tab[vx]];
			c = p_src0[x_resize_tab[vx+1]];			//4p
			d = p_src1[x_resize_tab[vx+1]];

			a = (a & 0xFFFF) | (b << 16);
			b = (c & 0xFFFF) | (d << 16);

			(*(VO_U32 *)out_dst) = a;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = b;	
			out_dst += step_y;
		}
		step_y2 += (step_x<<1);
	}
}

VOCCRETURNCODE YUV420Packed_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	//Y
	YPlanar_resize_RT90(conv_data->nRotationType, conv_data->pInBuf[0], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[0], conv_data->nOutWidth, conv_data->nOutHeight, conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	//uv
	UVPacked_resize_Rt90(conv_data->nRotationType, conv_data->pInBuf[1], conv_data->nInHeight , conv_data->nInStride , 
		conv_data->pOutBuf[1], (conv_data->nOutWidth>>1), (conv_data->nOutHeight>>1), conv_data->nOutStride,
		x_resize_tab, y_resize_tab);
	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422Pack_noresize_Rt90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{

	const VO_U32 out_width = conv_data->nOutWidth;
	const VO_U32 out_height = conv_data->nOutHeight;
	VO_U8 *p_src0=NULL;
	VO_U8 *p_src1=NULL;
	VO_U8 *start_out=NULL;
	VO_S32 step_x=0, step_y=0;
	VO_U32 vx, vy;
	VO_U32 mask=0;

	if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED){
		mask = 0xff00ff00;
	}else if(conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED){
		mask = 0x00ff00ff;
	}

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_x = conv_data->nInStride;
		step_y = -conv_data->nOutStride;
		p_src0 = conv_data->pInBuf[0];
		p_src1 = p_src0 + step_x;
		start_out = conv_data->pOutBuf[0] + conv_data->nOutStride * (out_width - 1);
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -conv_data->nInStride;
		step_y = conv_data->nOutStride;
		p_src0 = conv_data->pInBuf[0] + conv_data->nInStride * (conv_data->nInHeight - 1);
		p_src1 = p_src0 + step_x;
		start_out = conv_data->pOutBuf[0];
	}


	for(vy = 0; vy < out_height; vy+=2) {
		VO_U8 *out_dst = start_out + (vy<<1);
		VO_U32 a, b, c, d;		
		for(vx = 0; vx < out_width; vx+=2) {

			a = *(VO_U32*)p_src0;			//4p
			b = *(VO_U32*)p_src1;

			p_src0 += 4;
			p_src1 += 4;

			//u v
			d = a & mask;

			c = (b << 16) | (a & 0xFFFF);
			b = (b & 0xFFFF0000) | (a >> 16);

			c = (c & (~mask)) | d;
			b = (b & (~mask)) | d;


			(*(VO_U32 *)out_dst) = c;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = b;	

			out_dst += step_y;
		}
		p_src0 -= (out_width<<1);
		p_src0 += (step_x<<1);
		p_src1 = p_src0 + step_x;
	}
	return VO_ERR_NONE;
}

VOCCRETURNCODE YUV422Packed_resize_RT90(ClrConvData *conv_data, VO_S32 * const x_resize_tab, VO_S32 * const y_resize_tab,struct CC_HND * cc_hnd)
{
	VO_U32 vx = 0, vy;
	const VO_U32 in_stride = conv_data->nInStride ;
	const VO_U32 out_width = conv_data->nOutWidth, out_height = conv_data->nOutHeight;
	//const VO_S32 out_stride = conv_data->nOutStride;
	VO_U32 mask=0;
	VO_U8 *start_out=NULL;

	VO_S32 step_x=0, step_y=0, start_x=0;

	if(conv_data->nInType == VO_COLOR_YUYV422_PACKED || conv_data->nInType == VO_COLOR_YVYU422_PACKED){
		mask = 0xff00ff00;
	}else if(conv_data->nInType == VO_COLOR_UYVY422_PACKED || conv_data->nInType == VO_COLOR_VYUY422_PACKED){
		mask = 0x00ff00ff;
	}

	/*start of output address*/
	if(conv_data->nRotationType == ROTATION_90L){/*rotation90L*/
		step_x = 1;
		step_y = -conv_data->nOutStride;
		start_x = 0;
		start_out = conv_data->pOutBuf[0] + conv_data->nOutStride * (out_width - 1);
	}else if(conv_data->nRotationType == ROTATION_90R){/*rotation90R*/
		step_x = -1;
		step_y = conv_data->nOutStride;
		start_x = out_height - 1;
		start_out = conv_data->pOutBuf[0];
	}


	for(vy = 0; vy < out_height; vy+=2) {
		VO_U32 *psrc0, *psrc1;
		VO_S32 *x_tab = (VO_S32 *)x_resize_tab;
		VO_U8 *out_dst = start_out + (vy<<1);



		psrc0 = (VO_U32*)(conv_data->pInBuf[0] + y_resize_tab[start_x] * in_stride);
		psrc1 = (VO_U32*)(conv_data->pInBuf[0] + y_resize_tab[start_x + step_x] * in_stride);

		for(vx = 0; vx < out_width; vx +=2 ) {
			VO_U32 a, b, c, d;
			a = psrc0[*(x_tab)];
			b = psrc1[*(x_tab)];
			x_tab++;

			//u v
			d = a & mask;

			c = (b << 16) | (a & 0xFFFF);
			b = (b & 0xFFFF0000) | (a >> 16);

			c = (c & (~mask)) | d;
			b = (b & (~mask)) | d;


			(*(VO_U32 *)out_dst) = c;	
			out_dst += step_y;
			(*(VO_U32 *)out_dst) = b;	

			out_dst += step_y;
		}	
		start_x += (step_x<<1);
	}

	return VO_ERR_NONE;
}
#endif //COLCONVENABLE