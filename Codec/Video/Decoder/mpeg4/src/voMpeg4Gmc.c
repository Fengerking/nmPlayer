/************************************************************************
*									                                    *
*	VisualOn, Inc. Confidential and Proprietary, 2005		            *
*								 	                                    *
************************************************************************/
#include "voMpeg4Gmc.h"
#include "voMpeg4Parser.h"
#include "voMpeg4MB.h"

#define RDIV(a,b) (((a)>0 ? (a) + ((b)>>1) : (a) - ((b)>>1))/(b))
#define RSHIFT(a,b) ( (a)>0 ? ((a) + (1<<((b)-1)))>>(b) : ((a) + (1<<((b)-1))-1)>>(b))

#define GMCMUL(i)  (((16-(i))<<16) + (i))
static VOCONST VO_U32 GMCMulTab[16] = {
	GMCMUL( 0), GMCMUL( 1), GMCMUL( 2), GMCMUL( 3),
	GMCMUL( 4), GMCMUL( 5), GMCMUL( 6), GMCMUL( 7),
	GMCMUL( 8), GMCMUL( 9), GMCMUL(10), GMCMUL(11), 
	GMCMUL(12), GMCMUL(13), GMCMUL(14), GMCMUL(15)
};



void GMCLuma_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc, VO_U8 *dst, VOCONST VO_U8 *src,
			   VO_S32 dst_stride, VO_S32 src_stride, VO_S32 x, VO_S32 y, VO_S32 rounding)
{
	VOCONST VO_S32 W    = gmc->width;
	VOCONST VO_S32 H	= gmc->height;
	VOCONST VO_S32 rho = 3 - gmc->gmc_accuracy;
	VOCONST VO_S32 Rounder = ( (1<<7) - (rounding<<(2*rho)) ) << 16;

	VOCONST VO_S32 dUx = gmc->dU[0];
	VOCONST VO_S32 dVx = gmc->dV[0];
	VOCONST VO_S32 dUy = gmc->dU[1];
	VOCONST VO_S32 dVy = gmc->dV[1];

	VO_S32 Uo = gmc->Uo + ((dUy * y + dUx * x) << 4);
	VO_S32 Vo = gmc->Vo + ((dVy * y + dVx * x) << 4);

	VO_S32 i, j;

	for (j=16; j>0; --j) {
		VO_S32 U = Uo, V = Vo;
		Uo += dUy; Vo += dVy;
		for (i=0; i<16; i++) {
			VO_U32 f0, f1, ri = 16, rj = 16;
			VO_S32 Offset;
			VO_S32 u = ( U >> 16 ) << rho;
			VO_S32 v = ( V >> 16 ) << rho;

			if (u > 0 && u <= W) { 
				ri = GMCMulTab[u&15]; 
				Offset = u >> 4;	
			} else {
				if (u > W) 
					Offset = W >> 4;
				else 
					Offset = 0;
				ri = GMCMulTab[0];
			}

			if (v > 0 && v <= H) { 
				rj = GMCMulTab[v&15]; 
				Offset += (v >> 4) * src_stride; 
			} else {
				if (v > H) 
					Offset += (H >> 4) * src_stride;
				rj = GMCMulTab[0];
			}

			f0	= src[Offset + 0] | (src[Offset + 1] << 16);
			f1	= src[Offset + src_stride + 0] | (src[Offset + src_stride + 1] << 16);
			f0 = (ri * f0) >> 16;
			f1 = (ri * f1) & 0x0fff0000;
			f0 |= f1;
			f0 = (rj * f0 + Rounder) >> 24;

			dst[i] = (VO_U8)f0;

			U += dUx; 
			V += dVx;
		}
		dst += dst_stride;
	}
}

void GMCLuma1Pt_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,
				  VO_U8 *Dst, VOCONST VO_U8 *Src,
				  VO_S32 dst_stride, VO_S32 src_stride, VO_S32 x, VO_S32 y, VO_S32 rounding)
{
	VOCONST VO_S32 W	 = gmc->width;
	VOCONST VO_S32 H	 = gmc->height;
	VOCONST VO_S32 rho = 3-gmc->gmc_accuracy;
	VOCONST VO_S32 Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;


	VO_S32 uo = gmc->Uo + (x<<8);	 
	VO_S32 vo = gmc->Vo + (y<<8);
	VO_U32 ri = GMCMulTab[uo & 15];
	VO_U32 rj = GMCMulTab[vo & 15];
	VO_S32 i, j;

	VO_S32 Offset;

	if (vo>=(-16<<4) && vo<=H){
		Offset = (vo>>4)*src_stride;
	} else {
		if (vo>H) Offset = ( H>>4)*src_stride;
		else Offset =-16*src_stride;
		rj = GMCMulTab[0];
	}
	if (uo>=(-16<<4) && uo<=W) {
		Offset += (uo>>4);
	} else {
		if (uo>W) Offset += (W>>4);
		else Offset -= 16;
		ri = GMCMulTab[0];
	}


	for(j=16; j>0; --j, Offset+=src_stride-16){
		for(i=0; i<16; i++, ++Offset){
			VO_U32 f0, f1;

			f0	= Src[ Offset		+0 ] | (Src[ Offset		+1 ] << 16);
			f1	= Src[ Offset+src_stride +0 ] | (Src[ Offset+src_stride +1 ] << 16);

			f0 = (ri*f0)>>16;
			f1 = (ri*f1) & 0x0fff0000;
			f0 |= f1;
			f0 = ( rj*f0 + Rounder ) >> 24;
			Dst[i] = (VO_U8)f0;
		}
		Dst += dst_stride;
	}
}


void GMCChroma_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,VO_U8 *uDst, VOCONST VO_U8 *uSrc,
				 VO_U8 *vDst, VOCONST VO_U8 *vSrc,VO_S32 dst_stride, VO_S32 src_stride, VO_S32 x, VO_S32 y, VO_S32 rounding)
{
	VOCONST VO_S32 W	 = gmc->width >> 1;
	VOCONST VO_S32 H	 = gmc->height >> 1;
	VOCONST VO_S32 rho = 3-gmc->gmc_accuracy;
	VOCONST VO_S32 Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;

	VOCONST VO_S32 dUx = gmc->dU[0];
	VOCONST VO_S32 dVx = gmc->dV[0];
	VOCONST VO_S32 dUy = gmc->dU[1];
	VOCONST VO_S32 dVy = gmc->dV[1];

	VO_S32 Uo = gmc->Uco + 8*(dUy*y + dUx*x);
	VO_S32 Vo = gmc->Vco + 8*(dVy*y + dVx*x);

	VO_S32 i, j;

	for (j=8; j>0; --j) {
		VO_S32 U = Uo, V = Vo;
		Uo += dUy; Vo += dVy;

		for (i=0; i<8;i ++) {
			VO_S32 Offset;
			VO_U32 f0, f1, ri, rj;
			VO_S32 u, v;

			u = ( U >> 16 ) << rho;
			v = ( V >> 16 ) << rho;


			if (u > 0 && u <= W) {
				ri = GMCMulTab[u&15];
				Offset = u>>4;
			} else {
				if (u>W) Offset = W>>4;
				else Offset = 0;
				ri = GMCMulTab[0];
			}

			if (v > 0 && v <= H) {
				rj = GMCMulTab[v&15];
				Offset += (v>>4)*src_stride;
			} else {
				if (v>H) Offset += (H>>4)*src_stride;
				rj = GMCMulTab[0];
			}

			f0	= uSrc[Offset + 0];
			f0 |= uSrc[Offset + 1] << 16;
			f1	= uSrc[Offset + src_stride + 0];
			f1 |= uSrc[Offset + src_stride + 1] << 16;
			f0 = (ri*f0)>>16;
			f1 = (ri*f1) & 0x0fff0000;
			f0 |= f1;
			f0 = (rj*f0 + Rounder) >> 24;

			uDst[i] = (VO_U8)f0;

			f0	= vSrc[Offset + 0];
			f0 |= vSrc[Offset + 1] << 16;
			f1	= vSrc[Offset + src_stride + 0];
			f1 |= vSrc[Offset + src_stride + 1] << 16;
			f0 = (ri*f0)>>16;
			f1 = (ri*f1) & 0x0fff0000;
			f0 |= f1;
			f0 = (rj*f0 + Rounder) >> 24;

			vDst[i] = (VO_U8)f0;

			U += dUx; 
			V += dVx;
		}
		uDst += dst_stride;
		vDst += dst_stride;
	}
}

void GMCChroma1Pt_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc,
					VO_U8 *uDst, VOCONST VO_U8 *uSrc,
					VO_U8 *vDst, VOCONST VO_U8 *vSrc,
					VO_S32 dst_stride, VO_S32 src_stride, VO_S32 x, VO_S32 y, VO_S32 rounding)
{
	VOCONST VO_S32 W	 = gmc->width >> 1;
	VOCONST VO_S32 H	 = gmc->height >> 1;
	VOCONST VO_S32 rho = 3-gmc->gmc_accuracy;
	VOCONST VO_S32 Rounder = ( 128 - (rounding<<(2*rho)) ) << 16;

	VO_S32 uo = gmc->Uco + (x<<7);
	VO_S32 vo = gmc->Vco + (y<<7);
	VO_U32 rri = GMCMulTab[uo & 15];
	VO_U32 rrj = GMCMulTab[vo & 15];
	VO_S32 i, j;

	VO_S32 Offset;
	if (vo>=(-8<<4) && vo<=H){
		Offset	= (vo>>4)*src_stride;
	} else {
		if (vo>H) Offset = ( H>>4)*src_stride;
		else Offset =-8*src_stride;
		rrj = GMCMulTab[0];
	}
	if (uo>=(-8<<4) && uo<=W){
		Offset	+= (uo>>4);
	} else {
		if (uo>W) Offset += ( W>>4);
		else Offset -= 8;
		rri = GMCMulTab[0];
	}

	for(j=8; j>0; --j, Offset+=src_stride-8){
		for(i=0; i<8; i++, Offset++){
			VO_U32 f0, f1;

			f0	= uSrc[ Offset + 0 ]| (uSrc[ Offset + 1 ] << 16);
			f1	= uSrc[ Offset + src_stride + 0 ] | (uSrc[ Offset + src_stride + 1 ] << 16);

			f0 = (rri*f0)>>16;
			f1 = (rri*f1) & 0x0fff0000;
			f0 |= f1;
			f0 = ( rrj*f0 + Rounder ) >> 24;
			uDst[i] = (VO_U8)f0;

			f0	= vSrc[ Offset + 0 ] | (vSrc[ Offset + 1 ] << 16);
			f1	= vSrc[ Offset + src_stride + 0 ] | (vSrc[ Offset + src_stride + 1 ] << 16);

			f0 = (rri*f0)>>16;
			f1 = (rri*f1) & 0x0fff0000;
			f0 |= f1;
			f0 = ( rrj*f0 + Rounder ) >> 24;
			vDst[i] = (VO_U8)f0;
		}
		uDst += dst_stride;
		vDst += dst_stride;
	}
}


void GMCGetAvgMV_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc, VO_S32* mvx, VO_S32 *mvy,
				   VO_S32 x, VO_S32 y, VO_S32 qpel)
{
	VO_S32 i, j;
	VO_S32 vx = 0, vy = 0;
	VO_S32 uo = gmc->Uo + 16*(gmc->dU[1]*y + gmc->dU[0]*x);
	VO_S32 vo = gmc->Vo + 16*(gmc->dV[1]*y + gmc->dV[0]*x);

	for (j=16; j>0; --j){
		VO_S32 U, V;

		U = uo; 
		uo += gmc->dU[1];
		V = vo; 
		vo += gmc->dV[1];
		for (i=16; i>0; --i){
			VO_S32 u,v;

			u = U >> 16;
			U += gmc->dU[0]; 
			vx += u;
			v = V >> 16; 
			V += gmc->dV[0]; 
			vy += v;
		}
	}

	vx -= (256*x+120) << (5+gmc->gmc_accuracy);	
	vy -= (256*y+120) << (5+gmc->gmc_accuracy);

	*mvx = RSHIFT( vx, 8+gmc->gmc_accuracy - qpel );
	*mvy = RSHIFT( vy, 8+gmc->gmc_accuracy - qpel );
}




void GMCGetAvgMV1pt_C(VOCONST VO_VIDEO_GMC_DATA * VOCONST gmc, VO_S32* mvx, VO_S32 *mvy,
					  VO_S32 x, VO_S32 y, VO_S32 qpel)
{
	*mvx = RSHIFT(gmc->Uo<<qpel, 3);
	*mvy = RSHIFT(gmc->Vo<<qpel, 3);
}


void IniGMCParam(VO_S32 warp_pt_num, VOCONST VO_S32 gmc_accuracy, VO_S32 *warp_point,
				 VOCONST VO_S32 width, VOCONST VO_S32 height, VO_VIDEO_GMC_DATA *VOCONST gmc)

{
	gmc->width  = width  << 4;
	gmc->height = height << 4;
	gmc->gmc_accuracy = gmc_accuracy;

	/* refine pt num*/
	if (warp_pt_num<2 || (warp_point[2]==0  && warp_point[1]==0)) {
		if (warp_pt_num<2 || (warp_point[1]==0)) {
			if (warp_pt_num<1 || (warp_point[0]==0)) {
				warp_pt_num = 0;
			} else {
				warp_pt_num = 1;
			}
		} else {
			warp_pt_num = 2;
		}
	}

	if (warp_pt_num <= 1) {
		if (warp_pt_num==0) {//0
			gmc->Uo	= gmc->Vo	= 0;
			gmc->Uco = gmc->Vco = 0;
		} else {//1
			VO_S32 x = MVX(warp_point[0], 0);
			VO_S32 y = MVY(warp_point[0], 0);

			gmc->Uo = x << gmc_accuracy;
			gmc->Vo = y << gmc_accuracy;
			gmc->Uco = ((x>>1) | (x&1)) << gmc_accuracy;	
			gmc->Vco = ((y>>1) | (y&1)) << gmc_accuracy;	
		}


		gmc->GMCLuma	= GMCLuma1Pt_C;
		gmc->GMCChroma	= GMCChroma1Pt_C;
		gmc->GMCGetAvgMV = GMCGetAvgMV1pt_C;
	} else { //2, 3
		VOCONST VO_S32 rho = 3 - gmc_accuracy;	
		VO_S32 Alpha = _log2(width-1);
		VO_S32 Ws = 1 << Alpha;
		VO_S32 x = MVX(warp_point[1], 0);
		VO_S32 y = MVY(warp_point[1], 0);

		gmc->dU[0] = 16*Ws + RDIV( 8*Ws*x, width );	
		gmc->dV[0] =		 RDIV( 8*Ws*y, width );	 

		if (warp_pt_num==2) {
			gmc->dU[1] = -gmc->dV[0];	
			gmc->dV[1] = gmc->dU[0] ;	
		} else {
			VOCONST VO_S32 Beta = _log2(height-1);
			VOCONST VO_S32 Hs = 1<<Beta;

			x = MVX(warp_point[2], 0);
			y = MVY(warp_point[2], 0);

			gmc->dU[1] =		 RDIV( 8*Hs*x, height );	
			gmc->dV[1] = 16*Hs + RDIV( 8*Hs*y, height );	 
			if (Beta>Alpha) {
				gmc->dU[0] <<= (Beta-Alpha);
				gmc->dV[0] <<= (Beta-Alpha);
				Alpha = Beta;
				Ws = Hs;
			} else {
				gmc->dU[1] <<= Alpha - Beta;
				gmc->dV[1] <<= Alpha - Beta;
			}
		}

		gmc->dU[0] <<= (16-Alpha - rho);
		gmc->dU[1] <<= (16-Alpha - rho);
		gmc->dV[0] <<= (16-Alpha - rho);
		gmc->dV[1] <<= (16-Alpha - rho);

		x = MVX(warp_point[0], 0);
		y = MVY(warp_point[0], 0);

		gmc->Uo	= ( x	 <<(16+ gmc_accuracy)) + (1<<15);
		gmc->Vo	= ( y	 <<(16+ gmc_accuracy)) + (1<<15);
		gmc->Uco = ((x-1)<<(17+ gmc_accuracy)) + (1<<17);
		gmc->Vco = ((y-1)<<(17+ gmc_accuracy)) + (1<<17);
		gmc->Uco = (gmc->Uco + gmc->dU[0] + gmc->dU[1])>>2;
		gmc->Vco = (gmc->Vco + gmc->dV[0] + gmc->dV[1])>>2;

		gmc->GMCLuma	= GMCLuma_C;
		gmc->GMCChroma	= GMCChroma_C;
		gmc->GMCGetAvgMV = GMCGetAvgMV_C;
	}
}

