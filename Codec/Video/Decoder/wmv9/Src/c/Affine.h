//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************
tWMVDecodeStatus Affine_Init (  HWMVDecoder* phWMVDecoder,
                    int iOldWidth, 
                    int iOldHeight, 
                    int iNewWidth, 
                    int iNewHeight);
tWMVDecodeStatus Affine_PanC (HWMVDecoder* phWMVDecoder, double fX, double fY);

tWMVDecodeStatus Affine_PanCFading (HWMVDecoder* phWMVDecoder, double fX, double fY, double fFading);

tWMVDecodeStatus Affine_PanZoomC ( HWMVDecoder* phWMVDecoder,double fZoom, double fX, double fY);
tWMVDecodeStatus Affine_PanZoomCFading ( HWMVDecoder* phWMVDecoder,double fZoom, double fX, double fY, double fFading);

tWMVDecodeStatus Affine_PanStretchC ( HWMVDecoder* phWMVDecoder, double fZoomX, double fX, double fZoomY, double fY);
tWMVDecodeStatus Affine_PanStretchCFading ( HWMVDecoder* phWMVDecoder,double fZoomX, double fX, double fZoomY, double fY, double fFading);

//tWMVDecodeStatus Affine_ResizeInt2 ( HWMVDecoder* phWMVDecoder,double fA, double fC, double fE, double fF);
tWMVDecodeStatus Affine_TransformFloat2 ( HWMVDecoder* phWMVDecoder,double fA, double fB, double fC, double fD, double fE, double fF);

tWMVDecodeStatus Affine_Transform(HWMVDecoder* phWMVDecoder,
                      const unsigned char* pucSrcY,
                      const unsigned char* pucSrcU,
                      const unsigned char* pucSrcV,
                      unsigned char *pFrameY,
                      unsigned char *pFrameU,
                      unsigned char *pFrameV,
                      double fA, double fB, double fC,
                      double fD, double fE, double fF, double fFadding);

#ifdef WMV_OPT_SPRITE

Void_WMV Affine_Add (const U8_WMV* pucSrcY, const U8_WMV* pucSrcU, const U8_WMV* pucSrcV, U8_WMV* pucDstY, U8_WMV* pucDstU, U8_WMV* pucDstV, I32_WMV iSize);
Void_WMV Affine_PanYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY);
Void_WMV Affine_PanFadingYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY);
Void_WMV Affine_PanUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV);
Void_WMV Affine_PanFadingUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV);
Void_WMV Affine_StretchYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY);
Void_WMV Affine_StretchFadingYLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcY, U8_WMV* pDstY);
Void_WMV Affine_StretchUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV);
Void_WMV Affine_StretchFadingUVLineC (tWMVDecInternalMember *pWMVDec, const U8_WMV* pSrcU, const U8_WMV* pSrcV, U8_WMV* pDstU, U8_WMV* pDstV);

#endif