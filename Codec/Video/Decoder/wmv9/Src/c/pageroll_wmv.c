//*@@@+++@@@@*******************************************************************
//
// Microsoft Windows Media
// Copyright (C) 2003 Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@*******************************************************************
//
// File:    Pageroll_wmv.c
//
// Desc:    Implementation of pageroll effect in PhotoMotion Mode
//
// Author:  Peter X. Zuo (peterzuo@microsoft.com)
//          Port from pageroll.cpp desktop code.
// Date:    2/12/2004
//
//******************************************************************************
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_api.h"

#include "pageroll_wmv.h"
#include "sideroll_wmv.h"

#define WVP2_PAGEROLL_HORIZONTAL_FILL

#if defined(WMV_C_OPT_WVP2) && defined(WMV_OPT_WVP2_ARM)
extern void WMVImageMemCpy(U8_WMV*pDest, U8_WMV*pSrc, I32_WMV iSize);
#else
#define WMVImageMemCpy memcpy
#endif

#define SQRT2 1.41421356
#define PI    3.14159265

#ifdef WMV_C_OPT_WVP2

#ifdef EVAL_LOOPS
extern FILE	*g_FillLog, *g_LineLog, *g_QuickFillLog;
#endif

#if !defined(WMV_OPT_WVP2_ARM)
long ImageRoll_LineQuickFill(U8_WMV* pOutY, U8_WMV* pInY, long iDOffset_new, long loops)
{
    int offset;
    int lastOffset = iDOffset_new*loops;

#ifdef EVAL_LOOPS
	if (g_QuickFillLog)
		fprintf(g_QuickFillLog, "QuickFillLoops:%d\n", loops);
#endif

    for (offset = 0; offset < lastOffset; offset+=iDOffset_new)
    {
        pOutY[offset] = pInY[offset];
    }

    return lastOffset;
}

#endif


#if 1
#define ImageRoll_LineFillInsideOut(pOutY, pInY, iDOffset_new, loops)   \
    ImageRoll_LineQuickFill(pOutY, pInY, iDOffset_new, loops);
#else

long ImageRoll_LineFillInsideOut(U8_WMV* pOutY, U8_WMV* pInY, long iDOffset_new, long loops)
{
    int offset;
    int lastOffset = iDOffset_new*loops;

#ifdef EVAL_LOOPS
	if (g_LineLog)
		fprintf(g_LineLog, "LineLoops:%d\n", loops);
#endif

    for (offset = 0; offset < lastOffset; offset+=iDOffset_new)
    {
        pOutY[offset] = pInY[offset];
    }

    return lastOffset;
}

#endif

#if 1
void ImageRoll_FillInsideOut(U8_WMV *outY, U8_WMV*old1Y, U8_WMV*old2Y, U8_WMV*new1Y, U8_WMV*new2Y, long iDOffset_new, double dS_new, int K0, int K1, int K2, int K3, int loops)
{
    int k, ks, ke;
    long iOffset_new = 0;

#ifdef EVAL_LOOPS
	if (g_FillLog)
		fprintf(g_FillLog, "FillLoops:%d\n", loops);
#endif


    K3 = min(K3, loops);//K3<=loops
    K0 = max(0, K0);    //K0>=0
    K2 = max(0, K2);    //K2>=0



    k = 0;
    if (K2 < K3)
    {//DoP2
        K1 = min(K1, loops);
        if (K0 < K1)
        {
            //0, K0, Ks, K2, K3, Ke, K1, loops
            ks = min(K2, K1); //Ks <=K2
            ke = max(K3, K0); //Ke >=K0, K2, K3 

            if (K0<K2)
            {
                //DoP0
                if (k<K0)
                {
                    iOffset_new += ImageRoll_LineFillInsideOut(outY, new2Y, iDOffset_new, K0);
                    k = K0;
                }

                if (k < ks)
                {
                    iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, (dS_new>=0)?(old1Y+iOffset_new): (new1Y+iOffset_new), iDOffset_new, ks-k);
                    k = ks;
                }

                if (k<K2)
                {
                    iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, new2Y+iOffset_new, iDOffset_new, K2-k);
                    k = K2;
                }
            }
            else
            {
                if (k<K2)
                {
                    iOffset_new += ImageRoll_LineFillInsideOut(outY, new2Y, iDOffset_new, K2-k);
                    k = K2;
                }
            }

            //DoP2
            if (k<K3)
            {
                iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, old2Y+iOffset_new, iDOffset_new, K3-k);
                k = K3;
            }


            if (K1>K3)
            {
                //DoP0: [K3 - Ke]

                if (k<ke)
                {
                    iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, new2Y+iOffset_new, iDOffset_new, ke-k);
                    k = ke;
                }

                //DoP1: [Ke - K1]
                if (k < K1)
                {
                    iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, (dS_new>=0)?(old1Y+iOffset_new): (new1Y+iOffset_new), iDOffset_new, K1-k);
                    k = K1;
                }
            }
        }
        else
        {
            //DoP0
            if (k<K2)
            {
                iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, new2Y+iOffset_new, iDOffset_new, K2-k);
                k = K2;
            }
            //DoP2
            if (k<K3)
            {
                iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, old2Y+iOffset_new, iDOffset_new, K3-k);
                k = K3;
            }
        }
    }
    else
    {//Not Do P2
        K1 = min (K1, loops); //K1<=loops
        if (K0<K1)
        {
            //DoP0
            if (k<K0)
            {
                iOffset_new += ImageRoll_LineFillInsideOut(outY, new2Y, iDOffset_new, K0);
                k = K0;
            }

            //DoP1: [Ke - K1]
            if (k < K1)
            {
                iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, (dS_new>=0)?(old1Y+iOffset_new): (new1Y+iOffset_new), iDOffset_new, K1-k);
                k = K1;
            }
        }
    }

    //DoP0
    if (k<loops)
    {
        iOffset_new += ImageRoll_LineFillInsideOut(outY+iOffset_new, new2Y+iOffset_new, iDOffset_new, loops-k);
        k = loops;
    }
}

#else
void ImageRoll_FillInsideOut(U8_WMV *outY, U8_WMV*old1Y, U8_WMV*old2Y, U8_WMV*new1Y, U8_WMV*new2Y, long iDOffset_new, double dS_new, int K0, int K1, int K2, int K3, int loops)
{
    int k, ks, ke;
    long iOffset_new = 0;

    K3 = min(K3, loops);//K3<=loops
    K0 = max(0, K0);    //K0>=0
    K2 = max(0, K2);    //K2>=0

    if (K2 < K3)
    {//DoP2
        K1 = min(K1, loops);
        if (K0 < K1)
        {
            //0, K0, Ks, K2, K3, Ke, K1, loops
            ks = min(K2, K1); //Ks <=K2
            ke = max(K3, K0); //Ke >=K0, K2, K3 

            if (K0<K2)
            {
                //DoP0
                for (k=0; k<K0; k++)
                {
                    outY[iOffset_new] = new2Y[iOffset_new];
                    iOffset_new  += iDOffset_new;
                }

                //DoP1
                if (dS_new >= 0) 
                {
                    // inner part
                    for (; k<ks; k++)
                    {
                        outY[iOffset_new] = old1Y[iOffset_new];
                        iOffset_new  += iDOffset_new;
                    }
                }
                else
                {
                    for (; k<ks; k++)
                    {
                        outY[iOffset_new] = new1Y[iOffset_new];
                        iOffset_new  += iDOffset_new;
                    }
                }

                //DoP0 [Ks - K2]
                for (; k<K2; k++)
                {
                    outY[iOffset_new] = new2Y[iOffset_new];
                    iOffset_new  += iDOffset_new;
                }
            }
            else
            {
                //DoP0
                for (k=0; k<K2; k++)
                {
                    outY[iOffset_new] = new2Y[iOffset_new];
                    iOffset_new  += iDOffset_new;
                }
            }


            //DoP2
            for (; k<K3; k++)
            {
                outY[iOffset_new] = old2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }


            if (K1>K3)
            {
                //DoP0: [K3 - Ke]
                for (; k<ke; k++)
                {
                    outY[iOffset_new] = new2Y[iOffset_new];
                    iOffset_new  += iDOffset_new;
                }

                //DoP1: [Ke - K1]
                if (dS_new >= 0) 
                {
                    // inner part
                    for (; k<K1; k++)
                    {
                        outY[iOffset_new] = old1Y[iOffset_new];
                        iOffset_new  += iDOffset_new;
                    }
                }
                else
                {
                    for (; k<K1; k++)
                    {
                        outY[iOffset_new] = new1Y[iOffset_new];
                        iOffset_new  += iDOffset_new;
                    }
                }
            }


            //DoP0
            for (; k<loops; k++)
            {
                outY[iOffset_new] = new2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }
        }
        else
        {
            //DoP0
            for (k=0; k<K2; k++)
            {
                outY[iOffset_new] = new2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }
            //DoP2
            for (; k<K3; k++)
            {
                outY[iOffset_new] = old2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }
            //DoP0
            for (; k<loops; k++)
            {
                outY[iOffset_new] = new2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }

        }
    }
    else
    {//Not Do P2
        K1 = min (K1, loops); //K1<=loops
        if (K0<K1)
        {
            //DoP0
            for (k=0; k<K0; k++)
            {
                outY[iOffset_new] = new2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }

            //DoP1
            if (dS_new >= 0) 
            {
                // inner part
                for (; k<K1; k++)
                {
                    outY[iOffset_new] = old1Y[iOffset_new];
                    iOffset_new  += iDOffset_new;
                }
            }
            else
            {
                for (; k<K1; k++)
                {
                    outY[iOffset_new] = new1Y[iOffset_new];
                    iOffset_new  += iDOffset_new;
                }
            }

            //DoP0
            for (k=K1; k<loops; k++)
            {
                outY[iOffset_new] = new2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }
        }
        else
        {
            //DoP0
            for (k=0; k<loops; k++)
            {
                outY[iOffset_new] = new2Y[iOffset_new];
                iOffset_new  += iDOffset_new;
            }
        }
    }
}
#endif

#ifdef WVP2_PAGEROLL_HORIZONTAL_FILL

void ImageRoll_RightBottom(I32_WMV     iWidth,     // bitmap width
                           I32_WMV     iHeight,    // bitmap height
                           U8_WMV*     pIn1Y,   // input 1
                           U8_WMV*     pIn1U,
                           U8_WMV*     pIn1V,
                           U8_WMV*     pIn2Y,   // input 2
                           U8_WMV*     pIn2U,
                           U8_WMV*     pIn2V,
                           U8_WMV*     pOutY,   // output
                           U8_WMV*     pOutU,
                           U8_WMV*     pOutV,
                           int     iR,
                           int     iT)
{
	double R = (double)iR;
	double T = (double)iT;

	int    Xe, Ye;              // end point
	double X0, Y0;              // center point
	int    x, y;                // current point
	int    dx1, dy1, dx2, dy2;  // delta x, y for old points

	double dS_new;
	double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

	long   iOffset_new, iOffset_old1, iOffset_old2;
	long   iDOffset_new, iDOffset_old1, iDOffset_old2;
	long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
	long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;

	int    bDoit = 0;
	int    bUVDoit = 0;
	double temp ;
	int    i;
	int    loops;

	int    iUVWidth  = iWidth >> 1;
	int    iUVHeight = iHeight >> 1;

	int		qx1, qx2;

	Xe = iWidth - 1;
	Ye = iHeight - 1;

	X0 = (double)(Xe) - T/SQRT2;
	Y0 = X0 - (double)(Xe) + (double)(Ye);

	qx1 = (int)((X0+Y0) - R*SQRT2);
	qx2 = (int)((X0+Y0) + R*SQRT2);

	for (i=0; i<iHeight; i++)
	{
		loops = min((qx1-i+1), iWidth);
		if (loops>0)
		{
			iDOffset_new = i*iWidth;
			WMVImageMemCpy(pOutY+iDOffset_new, pIn1Y+iDOffset_new, loops);
		}
		loops = min(iWidth-qx2+i, iWidth);
		if (loops>0)
		{
			iDOffset_new = i*iWidth+iWidth-loops;
			WMVImageMemCpy(pOutY+iDOffset_new, pIn2Y+iDOffset_new, loops);
		}
	}

	for (i=0; i<iHeight; i+=2)
	{
		loops = min( ((qx1-i)>>1) +1, iUVWidth);
		if (loops>0)
		{
			iDOffset_new = (i>>1)*iUVWidth;
			WMVImageMemCpy(pOutU+iDOffset_new, pIn1U+iDOffset_new, loops);
			WMVImageMemCpy(pOutV+iDOffset_new, pIn1V+iDOffset_new, loops);
		}

		loops = min(iUVWidth-((qx2-i)>>1), iUVWidth);
		if (loops>0)
		{
			iDOffset_new = (i>>1)*iUVWidth+iUVWidth-loops;
			WMVImageMemCpy(pOutU+iDOffset_new, pIn2U+iDOffset_new, loops);
			WMVImageMemCpy(pOutV+iDOffset_new, pIn2V+iDOffset_new, loops);
		}
	}

	iDOffset_new   = iWidth - 1;
	iDUVOffset_new = iUVWidth -1;


	for (i = max(0, qx1+1); i < min(qx2, iWidth); i++) 
	{
		x = i;  y = 0;
    
		iOffset_new  = i;
		iOffset_old1 = iOffset_old2 = iOffset_new;

		dS_new = ((double)(x + y) - X0 - Y0) / SQRT2;

        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

		temp = asin( dS_new / R );
		dS_old1 = R * temp;
		dS_old2 = R * (PI - temp);

		dDS1 = dS_old1 - dS_new;
		dDS2 = dS_old2 - dS_new;

		dx1 = (int)( dDS1 / SQRT2 + 0.5 );
		dy1 = dx1;
		iDOffset_old1 = dy1 * iWidth + dx1;
		iOffset_old1 += iDOffset_old1;

		dx2 = (int)( dDS2 / SQRT2 + 0.5 );
		dy2 = dx2;
		iDOffset_old2 = dy2 * iWidth + dx2;
		iOffset_old2 += iDOffset_old2;

		{
			U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

			outY = pOutY + iOffset_new;
			old1Y = pIn1Y + iOffset_old1;
			old2Y = pIn1Y + iOffset_old2;
			new1Y = pIn1Y + iOffset_new;
			new2Y = pIn2Y + iOffset_new;
			ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, i+dx1+1-iWidth, iHeight-dx1,i+dx2+1-iWidth, iHeight-dx2, min(i+1, iHeight));
		}
		// U, V

		if (i & 1)
		  continue;

		x = i>>1; y = 0;
		dx1 = dx1 >> 1;
		dy1 = dx1;
		dx2 = dx2 >> 1;
		dy2 = dx2;

		iUVOffset_new = x;

		iDUVOffset_old1 = dy1 * iUVWidth + dx1;
		iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
		iDUVOffset_old2 = dy2 * iUVWidth + dx2;
		iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

		{
			U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

			outUV = pOutU + iUVOffset_new;
			old1UV = pIn1U + iUVOffset_old1;
			old2UV = pIn1U + iUVOffset_old2;
			new1UV = pIn1U + iUVOffset_new;
			new2UV = pIn2U + iUVOffset_new;
			ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, x+dx1+1-iUVWidth, iUVHeight-dx1,x+dx2+1-iUVWidth, iUVHeight-dx2, min(x+1, iUVHeight));

			outUV = pOutV + iUVOffset_new;
			old1UV = pIn1V + iUVOffset_old1;
			old2UV = pIn1V + iUVOffset_old2;
			new1UV = pIn1V + iUVOffset_new;
			new2UV = pIn2V + iUVOffset_new;
			ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, x+dx1+1-iUVWidth, iUVHeight-dx1,x+dx2+1-iUVWidth, iUVHeight-dx2, min(x+1, iUVHeight));

		}
	}
  
	for ( i = max(1, qx1-iWidth+2); i < min(iHeight, qx2-iWidth+2); i++) 
	{
		x = Xe; y = i;

		iOffset_new = i * iWidth + x;
		iOffset_old1 = iOffset_old2 = iOffset_new;

		dS_new = ((double)(x + y) - X0 - Y0) / SQRT2;

		//
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

		temp = asin( dS_new / R );
		dS_old1 = R * temp;
		dS_old2 = R * (PI - temp);

		dDS1 = dS_old1 - dS_new;
		dDS2 = dS_old2 - dS_new;

		dx1 = (int)( dDS1 / SQRT2 + 0.5 );
		dy1 = dx1;
		iDOffset_old1 = dy1 * iWidth + dx1;
		iOffset_old1 += iDOffset_old1;

		dx2 = (int)( dDS2 / SQRT2 + 0.5 );
		dy2 = dx2;
		iDOffset_old2 = dy2 * iWidth + dx2;
		iOffset_old2 += iDOffset_old2;

		{
			U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

			outY = pOutY + iOffset_new;
			old1Y = pIn1Y + iOffset_old1;
			old2Y = pIn1Y + iOffset_old2;
			new1Y = pIn1Y + iOffset_new;
			new2Y = pIn2Y + iOffset_new;
			ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, dx1, iHeight-dx1-i,dx2, iHeight-dx2-i, min(iWidth, iHeight-i));
		}

		if ( (Xe+i) & 1) continue;

		x = Xe;
		y = i;

		dx1 >>= 1; 
		dx2 >>= 1;
		dy1 >>= 1;
		dy2 >>= 1;

		iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
		iDUVOffset_old1 = dy1 * iUVWidth + dx1;
		iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
		iDUVOffset_old2 = dy2 * iUVWidth + dx2;
		iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

		x >>= 1;
		y = (y+1)>>1;

		{
			U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

			outUV = pOutU + iUVOffset_new;
			old1UV = pIn1U + iUVOffset_old1;
			old2UV = pIn1U + iUVOffset_old2;
			new1UV = pIn1U + iUVOffset_new;
			new2UV = pIn2U + iUVOffset_new;
			ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVHeight-dx1-y,dx2, iUVHeight-dx2-y, min(x+1, iUVHeight-y));

			outUV = pOutV + iUVOffset_new;
			old1UV = pIn1V + iUVOffset_old1;
			old2UV = pIn1V + iUVOffset_old2;
			new1UV = pIn1V + iUVOffset_new;
			new2UV = pIn2V + iUVOffset_new;
			ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVHeight-dx1-y,dx2, iUVHeight-dx2-y, min(x+1, iUVHeight-y));

		}
	}
}

#else

void ImageRoll_RightBottom(I32_WMV     iWidth,     // bitmap width
                           I32_WMV     iHeight,    // bitmap height
                           U8_WMV*     pIn1Y,   // input 1
                           U8_WMV*     pIn1U,
                           U8_WMV*     pIn1V,
                           U8_WMV*     pIn2Y,   // input 2
                           U8_WMV*     pIn2U,
                           U8_WMV*     pIn2V,
                           U8_WMV*     pOutY,   // output
                           U8_WMV*     pOutU,
                           U8_WMV*     pOutV,
                           int     iR,
                           int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1, dy1, dx2, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;
  int    loops;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = iWidth - 1;
  Ye = iHeight - 1;

  X0 = (double)(Xe) - T/SQRT2;
  Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  iDOffset_new   = iWidth - 1;
  iDUVOffset_new = iUVWidth -1;
  
  for (i = 0; i < iWidth; i++) 
  {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x + y) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min(iHeight, i+1);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, x + 1);

        iUVOffset_new = x;
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else if ( dS_new > R ) 
    {
        loops = min(iHeight, i+1);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, x + 1);

        iUVOffset_new = x;
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else 
    {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dx1 = (int)( dDS1 / SQRT2 + 0.5 );
        dy1 = dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = (int)( dDS2 / SQRT2 + 0.5 );
        dy2 = dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;
#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, i+dx1+1-iWidth, iHeight-dx1,i+dx2+1-iWidth, iHeight-dx2, min(i+1, iHeight));
        }
#else
        // Y
        while ( x >= 0 && y < iHeight) 
        {
            if ( (x+dx2)<iWidth && (y+dy2)<iHeight) {
              // outer part
              pOutY[iOffset_new] = pIn1Y[iOffset_old2];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
            } else if ( (x+dx1)<iWidth && (y+dy1)<iHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutY[iOffset_new] = pIn1Y[iOffset_old1];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
              } else {
                pOutY[iOffset_new] = pIn1Y[iOffset_new];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
              }
            } else {
              // mapping point out of range.
              pOutY[iOffset_new] = pIn2Y[iOffset_new];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
            }

            x--; y++;
            iOffset_new  += iDOffset_new;
            iOffset_old1 += iDOffset_new;
            iOffset_old2 += iDOffset_new;
        }
#endif
        // U, V

        if (i & 1)
          continue;
    
        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = dx1;
        dx2 = dx2 >> 1;
        dy2 = dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

#if 1
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, x+dx1+1-iUVWidth, iUVHeight-dx1,x+dx2+1-iUVWidth, iUVHeight-dx2, min(x+1, iUVHeight));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, x+dx1+1-iUVWidth, iUVHeight-dx1,x+dx2+1-iUVWidth, iUVHeight-dx2, min(x+1, iUVHeight));

        }
#else

        while ( x >= 0 && y < iUVHeight) {
            if ( (x+dx2)<iUVWidth && (y+dy2)<iUVHeight) {
              // outer part
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
            } else if ( (x+dx1)<iUVWidth && (y+dy1)<iUVHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
              } else {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
              }
            } else {
              // mapping point out of range.
              pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
            }

            x--; y++;
            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;
        }     
#endif

    }
  }
  
  for ( i = 1; i < iHeight; i++) {
    x = Xe; y = i;

    iOffset_new = i * iWidth + x;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x + y) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min( iWidth, (iHeight-i));
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if ((x+y)&1) continue;

        iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
        loops = min( iWidth, (iHeight-i))>>1;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else if ( dS_new > R ) 
    {
        loops = min( iWidth, (iHeight-i));
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if ((x+y)&1) continue;

        iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
        loops = min( iWidth, (iHeight-i))>>1;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dx1 = (int)( dDS1 / SQRT2 + 0.5 );
        dy1 = dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = (int)( dDS2 / SQRT2 + 0.5 );
        dy2 = dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, dx1, iHeight-dx1-i,dx2, iHeight-dx2-i, min(iWidth, iHeight-i));
        }
#else
        while ( x >= 0 && y < iHeight) 
        {
            if ( (x+dx2)<iWidth && (y+dy2)<iHeight) 
            {
              // outer part
              pOutY[iOffset_new] = pIn1Y[iOffset_old2];
            } else if ( (x+dx1)<iWidth && (y+dy1)<iHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutY[iOffset_new] = pIn1Y[iOffset_old1];
              } else {
                pOutY[iOffset_new] = pIn1Y[iOffset_new];
              }
            } else {
              // mapping point out of range.
              pOutY[iOffset_new] = pIn2Y[iOffset_new];
            }

            x--; y++;
            iOffset_new  += iDOffset_new;
            iOffset_old1 += iDOffset_new;
            iOffset_old2 += iDOffset_new;
        }
#endif

        if ( (Xe+i) & 1) continue;

        x = Xe;
        y = i;

        dx1 >>= 1; 
        dx2 >>= 1;
        dy1 >>= 1;
        dy2 >>= 1;

        iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

        x >>= 1;
        y = (y+1)>>1;

#if 1
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVHeight-dx1-y,dx2, iUVHeight-dx2-y, min(x+1, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVHeight-dx1-y,dx2, iUVHeight-dx2-y, min(x+1, iUVHeight-y));

        }
#else

        iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
        iDUVOffset_old1 = (dy1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = (dy2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

        if (y & 1)
        {
            x--;
            y++;
        }

        while ( x >= 0 && y < iHeight) 
        {
            if ( (x+dx2)<iWidth && (y+dy2)<iHeight) 
            {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
            }
            else if ( (x+dx1)<iWidth && (y+dy1)<iHeight ) 
            {
                // inner part
                if (dS_new >= 0) 
                {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
                }
                else
                {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
                }
            }
            else 
            {
                // mapping point out of range.
                pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
            }

            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;

            x-=2; y+=2;
        }
#endif
    }
  }
 
}

#endif

#ifdef WVP2_PAGEROLL_HORIZONTAL_FILL

void ImageRoll_RightTop(I32_WMV     iWidth,     // bitmap width
                        I32_WMV     iHeight,    // bitmap height
                        U8_WMV*     pIn1Y,      // input 1
                        U8_WMV*     pIn1U,
                        U8_WMV*     pIn1V,
                        U8_WMV*     pIn2Y,      // input 2
                        U8_WMV*     pIn2U,
                        U8_WMV*     pIn2V,
                        U8_WMV*     pOutY,      // output
                        U8_WMV*     pOutU,
                        U8_WMV*     pOutV,
                        int     iR,
                        int     iT)
{
    double R = (double)iR;
    double T = (double)iT;

    int    Xe, Ye;              // end point
    double X0, Y0;              // center point
    int    x, y;                // current point
    int    dx1, dy1, dx2, dy2;  // delta x, y for old points

    double dS_new;
    double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

    long   iOffset_new, iOffset_old1, iOffset_old2;
    long   iDOffset_new, iDOffset_old1, iDOffset_old2;
    long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
    long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;

    int    bDoit = 0;
    int    bUVDoit = 0;
    double temp ;
    int    i;
    int    loops;

    int    iUVWidth  = iWidth >> 1;
    int    iUVHeight = iHeight >> 1;

    int    qx1, qx2;

    Xe = iWidth - 1;
    Ye = 0;

    temp = T/SQRT2;
    X0 = (double)(Xe) - temp;
    Y0 = (double)(Ye) + temp;

    qx1 = (int)((X0-Y0) - R*SQRT2);
    qx2 = (int)((X0-Y0) + R*SQRT2);

    if (qx1<0)
        qx1-=1;

    if (qx2<0)
        qx2-=1;

    for (i=0; i<iHeight; i++)
    {
        loops = min(iWidth, qx1+i+1);
        if (loops>0)
        {
            iDOffset_new = iWidth * i;
            WMVImageMemCpy(pOutY+iDOffset_new, pIn1Y+iDOffset_new, loops);
        }

        loops = min(iWidth, iWidth-qx2-i);
        if (loops>0)
        {
            iDOffset_new = iWidth * i + max(qx2 + i, 0);
            WMVImageMemCpy(pOutY+iDOffset_new, pIn2Y+iDOffset_new, loops);
        }

    }

    for (i=0; i<iHeight; i+=2)
    {
        loops = min( ((qx1+i)>>1) + 1, iUVWidth);
        if (loops>0)
        {
            iDOffset_new = iUVWidth * (i>>1);
            WMVImageMemCpy(pOutU+iDOffset_new, pIn1U+iDOffset_new, loops);
            WMVImageMemCpy(pOutV+iDOffset_new, pIn1V+iDOffset_new, loops);
        }

        loops = min(iUVWidth-((qx2+i)>>1), iUVWidth);
        if (loops>0)
        {
            iDOffset_new = iUVWidth * (i>>1) + ((qx2+i)>>1);
            WMVImageMemCpy(pOutU+iDOffset_new, pIn2U+iDOffset_new, loops);
            WMVImageMemCpy(pOutV+iDOffset_new, pIn2V+iDOffset_new, loops);
        }
    }


    iDOffset_new   = iWidth   + 1;
    iDUVOffset_new = iUVWidth + 1;
  
    for (i = max(0, qx1+1); i < min(qx2, iWidth); i++) 
    {
        U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;
        U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

        x = i;  y = 0;

        iOffset_new  = i;
        iOffset_old1 = iOffset_old2 = iOffset_new;

        dS_new = ((double)(x - y) - X0 + Y0) / SQRT2;

		//
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new; 
        dDS2 = dS_old2 - dS_new;

        dx1 = (int)( dDS1 / SQRT2 + 0.5 );
        dy1 = -dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = (int)( dDS2 / SQRT2 + 0.5 );
        dy2 = -dx2;


        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

        outY = pOutY + iOffset_new;
        old1Y = pIn1Y + iOffset_old1;
        old2Y = pIn1Y + iOffset_old2;
        new1Y = pIn1Y + iOffset_new;
        new2Y = pIn2Y + iOffset_new;
        ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, dx1, iWidth-i-dx1,dx2, iWidth-i-dx2, min(iWidth-i, iHeight));

        // U, V
        
        if (i & 1)
          continue;
    
        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = -dx1;
        dx2 = dx2 >> 1;
        dy2 = -dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;


        outUV = pOutU + iUVOffset_new;
        old1UV = pIn1U + iUVOffset_old1;
        old2UV = pIn1U + iUVOffset_old2;
        new1UV = pIn1U + iUVOffset_new;
        new2UV = pIn2U + iUVOffset_new;
        ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVWidth-x-dx1, dx2, iUVWidth-x-dx2, min(iUVWidth-x, iUVHeight));

        outUV = pOutV + iUVOffset_new;
        old1UV = pIn1V + iUVOffset_old1;
        old2UV = pIn1V + iUVOffset_old2;
        new1UV = pIn1V + iUVOffset_new;
        new2UV = pIn2V + iUVOffset_new;
        ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVWidth-x-dx1, dx2, iUVWidth-x-dx2, min(iUVWidth-x, iUVHeight));
    }
  
    for ( i = max(-qx2+1, 1); i < min(-qx1, iHeight); i++) 
    {
        U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

        x = 0;  y = i;

        iOffset_new = i * iWidth;
        iOffset_old1 = iOffset_old2 = iOffset_new;

        dS_new = ((double)(x - y) - X0 + Y0) / SQRT2;

		//
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dx1 = (int)( dDS1 / SQRT2 + 0.5 );
        dy1 = -dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = (int)( dDS2 / SQRT2 + 0.5 );
        dy2 = -dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;


        outY = pOutY + iOffset_new;
        old1Y = pIn1Y + iOffset_old1;
        old2Y = pIn1Y + iOffset_old2;
        new1Y = pIn1Y + iOffset_new;
        new2Y = pIn2Y + iOffset_new;
        ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, dx1-i, iWidth-dx1,dx2-i, iWidth-dx2, min(iWidth, iHeight-i));

        if ( i & 1) continue;

        x = 0;
        y = i;

        iUVOffset_new = (y>>1) * iUVWidth;
        iDUVOffset_old1 = -(dx1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = -(dx2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

//Not as accurate as dx1 and dx2 since both shift to right.
#if 0
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            y >>= 1;
            dx1 >>= 1;
            dx2 >>= 1;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1-y, iUVWidth-dx1, dx2-y, iUVWidth-dx2, min(iUVWidth, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1-y, iUVWidth-dx1, dx2-y, iUVWidth-dx2, min(iUVWidth, iUVHeight-y));
        }
#else

        while ( x < iWidth && y < iHeight) 
        {
            if ( (x+dx2)<iWidth && (y+dy2)>=0) {
              // outer part
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
            } else if ( (x+dx1)<iWidth && (y+dy1)>=0 ) {
              // inner part
              if (dS_new >= 0) {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
              } else {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
              }
            } else {
                pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
            }

            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;

            x+=2; y+=2;
        }
#endif

    }
}


#else
void ImageRoll_RightTop(I32_WMV     iWidth,     // bitmap width
                        I32_WMV     iHeight,    // bitmap height
                        U8_WMV*     pIn1Y,      // input 1
                        U8_WMV*     pIn1U,
                        U8_WMV*     pIn1V,
                        U8_WMV*     pIn2Y,      // input 2
                        U8_WMV*     pIn2U,
                        U8_WMV*     pIn2V,
                        U8_WMV*     pOutY,      // output
                        U8_WMV*     pOutU,
                        U8_WMV*     pOutV,
                        int     iR,
                        int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1, dy1, dx2, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;
  int    loops;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = iWidth - 1;
  Ye = 0;

  temp = T/SQRT2;
  X0 = (double)(Xe) - temp;
  Y0 = (double)(Ye) + temp;
  
  iDOffset_new   = iWidth   + 1;
  iDUVOffset_new = iUVWidth + 1;
  
  for (i = 0; i < iWidth; i++) 
  {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x - y) - X0 + Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min(iHeight, iWidth - i);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, iUVWidth - x);

        iUVOffset_new = x;
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);

    }
    else if ( dS_new > R ) 
    {
        loops = min(iHeight, iWidth - i);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, iUVWidth - x);

        iUVOffset_new = x;
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else 
    {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new; 
        dDS2 = dS_old2 - dS_new;

        dx1 = (int)( dDS1 / SQRT2 + 0.5 );
        dy1 = -dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = (int)( dDS2 / SQRT2 + 0.5 );
        dy2 = -dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, dx1, iWidth-i-dx1,dx2, iWidth-i-dx2, min(iWidth-i, iHeight));
        }
#else
        // Y
        while ( x < iWidth && y < iHeight) {
            if ( (x+dx2)<iWidth && (y+dy2)>=0) {
              // outer part
              pOutY[iOffset_new] = pIn1Y[iOffset_old2];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
            } else if ( (x+dx1)<iWidth && (y+dy1)>=0 ) {
              // inner part
              if (dS_new >= 0) {
                pOutY[iOffset_new] = pIn1Y[iOffset_old1];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
              } else {
                pOutY[iOffset_new] = pIn1Y[iOffset_new];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
              }
            } else {
              // mapping point out of range.
              pOutY[iOffset_new] = pIn2Y[iOffset_new];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
            }

            x++; y++;
            iOffset_new  += iDOffset_new;
            iOffset_old1 += iDOffset_new;
            iOffset_old2 += iDOffset_new;
        }
#endif    
        // U, V
        
        if (i & 1)
          continue;
    
        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = -dx1;
        dx2 = dx2 >> 1;
        dy2 = -dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

#if 1
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVWidth-x-dx1, dx2, iUVWidth-x-dx2, min(iUVWidth-x, iUVHeight));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1, iUVWidth-x-dx1, dx2, iUVWidth-x-dx2, min(iUVWidth-x, iUVHeight));
        }
#else
        while ( x < iUVWidth && y < iUVHeight) {
            if ( (x+dx2)<iUVWidth && (y+dy2)>=0) {
              // outer part
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
            } else if ( (x+dx1)<iUVWidth && (y+dy1)>=0 ) {
              // inner part
              if (dS_new >= 0) {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
              } else {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
              }
            } else {
              // mapping point out of range.
              pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
            }

            x++; y++;
            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;
        }
#endif
    }

  }
  
  for ( i = 1; i < iHeight; i++) {
    x = 0;  y = i;

    iOffset_new = i * iWidth;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x - y) - X0 + Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min( iWidth, (iHeight-i));
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (i & 1) continue;

        iUVOffset_new = (y>>1) * iUVWidth;
        loops = min( iWidth, (iHeight-i))>>1;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);

    }
    else if ( dS_new > R ) 
    {
        loops = min( iWidth, (iHeight-i));
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if (i & 1) continue;

        iUVOffset_new = (y>>1) * iUVWidth;
        loops = min( iWidth, (iHeight-i))>>1;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);

    }
    else 
    {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dx1 = (int)( dDS1 / SQRT2 + 0.5 );
        dy1 = -dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = (int)( dDS2 / SQRT2 + 0.5 );
        dy2 = -dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, dx1-i, iWidth-dx1,dx2-i, iWidth-dx2, min(iWidth, iHeight-i));
        }
#else
        while ( x < iWidth && y < iHeight) 
        {
            if ( (x+dx2)<iWidth && (y+dy2)>=0) {
              // outer part
              pOutY[iOffset_new] = pIn1Y[iOffset_old2];
            } else if ( (x+dx1)<iWidth && (y+dy1)>=0 ) {
              // inner part
              if (dS_new >= 0) {
                pOutY[iOffset_new] = pIn1Y[iOffset_old1];
              } else {
                pOutY[iOffset_new] = pIn1Y[iOffset_new];
              }
            } else {
              // mapping point out of range.
              pOutY[iOffset_new] = pIn2Y[iOffset_new];
            }

            x++; y++;
            iOffset_new  += iDOffset_new;
            iOffset_old1 += iDOffset_new;
            iOffset_old2 += iDOffset_new;
        }
#endif

        if ( i & 1) continue;

        x = 0;
        y = i;

        iUVOffset_new = (y>>1) * iUVWidth;
        iDUVOffset_old1 = -(dx1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = -(dx2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

//Not as accurate as dx1 and dx2 since both shift to right.
#if 0
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            y >>= 1;
            dx1 >>= 1;
            dx2 >>= 1;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1-y, iUVWidth-dx1, dx2-y, iUVWidth-dx2, min(iUVWidth, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, dx1-y, iUVWidth-dx1, dx2-y, iUVWidth-dx2, min(iUVWidth, iUVHeight-y));
        }
#else

        while ( x < iWidth && y < iHeight) 
        {
            if ( (x+dx2)<iWidth && (y+dy2)>=0) {
              // outer part
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
            } else if ( (x+dx1)<iWidth && (y+dy1)>=0 ) {
              // inner part
              if (dS_new >= 0) {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
              } else {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
              }
            } else {
                pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
            }

            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;

            x+=2; y+=2;
        }
#endif

    }
  }
}
#endif


#ifdef WVP2_PAGEROLL_HORIZONTAL_FILL
void ImageRoll_LeftTop(I32_WMV     iWidth,     // bitmap width
                       I32_WMV     iHeight,    // bitmap height
                       U8_WMV*     pIn1Y,   // input 1
                       U8_WMV*     pIn1U,
                       U8_WMV*     pIn1V,
                       U8_WMV*     pIn2Y,   // input 2
                       U8_WMV*     pIn2U,
                       U8_WMV*     pIn2V,
                       U8_WMV*     pOutY,   // output
                       U8_WMV*     pOutU,
                       U8_WMV*     pOutV,
                       int     iR,
                       int     iT)
{

    double R = (double)iR;
    double T = (double)iT;

    int    Xe, Ye;              // end point
    double X0, Y0;              // center point
    int    x, y;                // current point
    int    dx1, dy1, dx2, dy2;  // delta x, y for old points

    double dS_new;
    double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

    long   iOffset_new, iOffset_old1, iOffset_old2;
    long   iDOffset_new, iDOffset_old1, iDOffset_old2;
    long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
    long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;

    int    bDoit = 0;
    int    bUVDoit = 0;
    double temp ;
    int    i;
    int    loops;

    int    iUVWidth  = iWidth >> 1;
    int    iUVHeight = iHeight >> 1;

    int    qx1, qx2;

    Xe = 0;
    Ye = 0;

    X0 = T/SQRT2;
    Y0 = X0;

	qx1 = (int)((X0+Y0) - R*SQRT2);
	qx2 = (int)((X0+Y0) + R*SQRT2);

	for (i=0; i<iHeight; i++)
	{
		loops = min((qx1-i+1), iWidth);
		if (loops>0)
		{
			iDOffset_new = i*iWidth;
			WMVImageMemCpy(pOutY+iDOffset_new, pIn2Y+iDOffset_new, loops);
		}
		loops = min(iWidth-qx2+i, iWidth);
		if (loops>0)
		{
			iDOffset_new = i*iWidth+iWidth-loops;
			WMVImageMemCpy(pOutY+iDOffset_new, pIn1Y+iDOffset_new, loops);
		}
	}

	for (i=0; i<iHeight; i+=2)
	{
		loops = min( ((qx1-i)>>1) +1, iUVWidth);
		if (loops>0)
		{
			iDOffset_new = (i>>1)*iUVWidth;
			WMVImageMemCpy(pOutU+iDOffset_new, pIn2U+iDOffset_new, loops);
			WMVImageMemCpy(pOutV+iDOffset_new, pIn2V+iDOffset_new, loops);
		}

		loops = min(iUVWidth-((qx2-i)>>1), iUVWidth);
		if (loops>0)
		{
			iDOffset_new = (i>>1)*iUVWidth+iUVWidth-loops;
			WMVImageMemCpy(pOutU+iDOffset_new, pIn1U+iDOffset_new, loops);
			WMVImageMemCpy(pOutV+iDOffset_new, pIn1V+iDOffset_new, loops);
		}
	}

    iDOffset_new   = iWidth - 1;
    iDUVOffset_new = iUVWidth -1;

    for (i = max(0, qx1+1); i < min(qx2, iWidth); i++) 
    {
        x = i;  y = 0;

        iOffset_new  = i;
        iOffset_old1 = iOffset_old2 = iOffset_new;

        dS_new = -((double)(x) - X0 - Y0) / SQRT2;

        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        //calculate angle
        temp = asin( dS_new / R );

        //length of the first/second curve
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        //distance of first/second line and center line
        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        //x distance
        dx1 = -(int)( dDS1 / SQRT2 + 0.5 );
        dy1 = dx1;
        dx2 = -(int)( dDS2 / SQRT2 + 0.5 );
        dy2 = dx2;

        //mapping offset distance
        iDOffset_old1 = dx1 * iWidth + dx1;
        iDOffset_old2 = dx2 * iWidth + dx2;
        iOffset_old1 += iDOffset_old1;
        iOffset_old2 += iDOffset_old2;

        //The far points should be always far than the nearer points
        assert( -dx2 >= -dx1 );

        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1, dx1+i+1, -dx2, dx2+i+1, min(i+1, iHeight));
        }

        if (i & 1)
          continue;

        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = dx1;
        dx2 = dx2 >> 1;
        dy2 = dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+x+1, -dx2, dx2+x+1, min(x+1, iUVHeight));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+x+1, -dx2, dx2+x+1, min(x+1, iUVHeight));
        }
    }

	for ( i = max(1, qx1-iWidth+2); i < min(iHeight, qx2-iWidth+2); i++) 
    {
        x = iWidth - 1; y = i;

        iOffset_new = i * iWidth + x;
        iOffset_old1 = iOffset_old2 = iOffset_new;

        dS_new = -((double)(x + y) - X0 - Y0) / SQRT2;
        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dx1 = -(int)( dDS1 / SQRT2 + 0.5 );
        dy1 = dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = -(int)( dDS2 / SQRT2 + 0.5 );
        dy2 = dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1-i, dx1+iWidth, -dx2-i, dx2+iWidth, min(iWidth, iHeight-i));
        }

        if (!(i & 1)) continue;

        x = iWidth - 1;
        y = i;

        dx1 = dx1>>1;
        dy1 = dx1;
        dx2 = dx2>>1;
        dy2 = dx2;

        x=(x-1)>>1;
        y=(y+1)>>1;

        iUVOffset_new = y * iUVWidth + x;
        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-y, dx1+iUVWidth, -dx2-y, dx2+iUVWidth, min(iUVWidth, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-y, dx1+iUVWidth, -dx2-y, dx2+iUVWidth, min(iUVWidth, iUVHeight-y));
        }
    }
}
#else


void ImageRoll_LeftTop(I32_WMV     iWidth,     // bitmap width
                       I32_WMV     iHeight,    // bitmap height
                       U8_WMV*     pIn1Y,   // input 1
                       U8_WMV*     pIn1U,
                       U8_WMV*     pIn1V,
                       U8_WMV*     pIn2Y,   // input 2
                       U8_WMV*     pIn2U,
                       U8_WMV*     pIn2V,
                       U8_WMV*     pOutY,   // output
                       U8_WMV*     pOutU,
                       U8_WMV*     pOutV,
                       int     iR,
                       int     iT)
{

  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1, dy1, dx2, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;
  int    loops;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = 0;
  Ye = 0;

  X0 = T/SQRT2;
  Y0 = X0;
  iDOffset_new   = iWidth - 1;
  iDUVOffset_new = iUVWidth -1;


  for (i = 0; i < iWidth; i++) 
  {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = -((double)(x) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min(iHeight, i+1);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, x + 1);

        iUVOffset_new = x;
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);
        
    }
    else if ( dS_new > R ) 
    {
        loops = min(iHeight, i+1);

        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        x = i>>1;
        loops = min(iUVHeight, x + 1);

        iUVOffset_new = x;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else 
    {
//        long eqlpoint, endx;

        //calculate angle
        temp = asin( dS_new / R );

        //length of the first/second curve
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        //distance of first/second line and center line
        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        //x distance
        dx1 = -(int)( dDS1 / SQRT2 + 0.5 );
        dy1 = dx1;
        dx2 = -(int)( dDS2 / SQRT2 + 0.5 );
        dy2 = dx2;

        //mapping offset distance
        iDOffset_old1 = dx1 * iWidth + dx1;
        iDOffset_old2 = dx2 * iWidth + dx2;
        iOffset_old1 += iDOffset_old1;
        iOffset_old2 += iDOffset_old2;

        //The far points should be always far than the nearer points
        assert( -dx2 >= -dx1 );

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1, dx1+i+1, -dx2, dx2+i+1, min(i+1, iHeight));
        }
#else
        //where x = y
        eqlpoint = (x+y+1) >> 1;

        if (eqlpoint>=iHeight)
            eqlpoint = iHeight;

        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;

            iOffset_new = ImageRoll_FillYPass1(outY, old1Y, old2Y, new1Y, new2Y, y, dy1, dy2, eqlpoint, 0, iDOffset_new, dS_new);

            if (eqlpoint > 0)
                x -= eqlpoint;

            //x<y
            endx = max(x+1 - (iHeight-eqlpoint), 0);

            ImageRoll_FillYPass2(outY, old1Y, old2Y, new1Y, new2Y, x, dx1, dx2, endx,iOffset_new, iDOffset_new, dS_new);
        }
#endif

        if (i & 1)
          continue;

        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = dx1;
        dx2 = dx2 >> 1;
        dy2 = dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

#if 1
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+x+1, -dx2, dx2+x+1, min(x+1, iUVHeight));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+x+1, -dx2, dx2+x+1, min(x+1, iUVHeight));
        }
#else
        //where x = y
        eqlpoint = (x+y+1) >> 1;

        if (eqlpoint>=iUVHeight)
            eqlpoint = iUVHeight;

        {
            U8_WMV *outU, *old1U, *old2U, *new1U, *new2U;
            U8_WMV *outV, *old1V, *old2V, *new1V, *new2V;

            outU  = pOutU + iUVOffset_new;
            old1U = pIn1U + iUVOffset_old1;
            old2U = pIn1U + iUVOffset_old2;
            new1U = pIn1U + iUVOffset_new;
            new2U = pIn2U + iUVOffset_new;

            outV  = pOutV + iUVOffset_new;
            old1V = pIn1V + iUVOffset_old1;
            old2V = pIn1V + iUVOffset_old2;
            new1V = pIn1V + iUVOffset_new;
            new2V = pIn2V + iUVOffset_new;

            iUVOffset_new = ImageRoll_FillUVPass1(outU, old1U, old2U, new1U, new2U,
                               outV, old1V, old2V, new1V, new2V,
                               y, dy1, dy2, eqlpoint, 0, iDUVOffset_new, dS_new);

            if (eqlpoint > 0)
                x -= eqlpoint;

            //x<y
            endx = max(x+1 - (iUVHeight-eqlpoint), 0);

            ImageRoll_FillUVPass2(outU, old1U, old2U, new1U, new2U,
                               outV, old1V, old2V, new1V, new2V,
                               x, dx1, dx2, endx, iUVOffset_new, iDUVOffset_new, dS_new);
        }
#endif
    }
  }

  for ( i = 1; i < iHeight; i++) {

    x = iWidth - 1; y = i;

    iOffset_new = i * iWidth + x;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = -((double)(x + y) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
        loops = min( iWidth, (iHeight-i));
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (!(i & 1)) continue;

        iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
        loops = min( iWidth, (iHeight-i))>>1;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);

    } else if ( dS_new > R ) {
        loops = min( iWidth, (iHeight-i));
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if (!(i & 1)) continue;

        iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
        loops = min( iWidth, (iHeight-i))>>1;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);

    }
    else 
    {
//        long eqlpoint, endx;

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dx1 = -(int)( dDS1 / SQRT2 + 0.5 );
        dy1 = dx1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dx2 = -(int)( dDS2 / SQRT2 + 0.5 );
        dy2 = dx2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1-i, dx1+iWidth, -dx2-i, dx2+iWidth, min(iWidth, iHeight-i));
        }
#else

        //where x = y
        eqlpoint = (x+y+1) >> 1;

        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;

            iOffset_new = ImageRoll_FillYPass1(outY, old1Y, old2Y, new1Y, new2Y, y, dy1, dy2, eqlpoint, 0, iDOffset_new, dS_new);

            if (eqlpoint > i)
                x -= (eqlpoint-i);

            //x<y
            endx = max(x+1 - (iHeight-eqlpoint), 0);

            ImageRoll_FillYPass2(outY, old1Y, old2Y, new1Y, new2Y, x, dx1, dx2, endx,iOffset_new, iDOffset_new, dS_new);
        }
#endif

        if (!(i & 1)) continue;

        x = iWidth - 1;
        y = i;

        dx1 = dx1>>1;
        dy1 = dx1;
        dx2 = dx2>>1;
        dy2 = dx2;

        x=(x-1)>>1;
        y=(y+1)>>1;

        iUVOffset_new = y * iUVWidth + x;
        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

#if 1
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-y, dx1+iUVWidth, -dx2-y, dx2+iUVWidth, min(iUVWidth, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-y, dx1+iUVWidth, -dx2-y, dx2+iUVWidth, min(iUVWidth, iUVHeight-y));
        }
#else
        //where x = y
        eqlpoint = (x+y+1) >> 1;

        if (eqlpoint>iUVHeight)
            eqlpoint = iUVHeight;

        {
            U8_WMV *outU, *old1U, *old2U, *new1U, *new2U;
            U8_WMV *outV, *old1V, *old2V, *new1V, *new2V;

            outU  = pOutU + iUVOffset_new;
            old1U = pIn1U + iUVOffset_old1;
            old2U = pIn1U + iUVOffset_old2;
            new1U = pIn1U + iUVOffset_new;
            new2U = pIn2U + iUVOffset_new;

            outV  = pOutV + iUVOffset_new;
            old1V = pIn1V + iUVOffset_old1;
            old2V = pIn1V + iUVOffset_old2;
            new1V = pIn1V + iUVOffset_new;
            new2V = pIn2V + iUVOffset_new;

            iUVOffset_new = ImageRoll_FillUVPass1(outU, old1U, old2U, new1U, new2U,
                               outV, old1V, old2V, new1V, new2V,
                               y, dy1, dy2, eqlpoint, 0, iDUVOffset_new, dS_new);

            if (eqlpoint > ((i+1)>>1) )
                x -= (eqlpoint-((i+1)>>1));

            //x<y
            endx = max(x+1 - (iUVHeight-eqlpoint), 0);

            ImageRoll_FillUVPass2(outU, old1U, old2U, new1U, new2U,
                               outV, old1V, old2V, new1V, new2V,
                               x, dx1, dx2, endx, iUVOffset_new, iDUVOffset_new, dS_new);
        }
#endif

    }
  }
}

#endif

#ifdef WVP2_PAGEROLL_HORIZONTAL_FILL

void ImageRoll_LeftBottom(I32_WMV     iWidth,     // bitmap width
                          I32_WMV     iHeight,    // bitmap height
                          U8_WMV*     pIn1Y,      // input 1
                          U8_WMV*     pIn1U,
                          U8_WMV*     pIn1V,
                          U8_WMV*     pIn2Y,      // input 2
                          U8_WMV*     pIn2U,
                          U8_WMV*     pIn2V,
                          U8_WMV*     pOutY,      // output
                          U8_WMV*     pOutU,
                          U8_WMV*     pOutV,
                          int     iR,
                          int     iT)
{
    double R = (double)iR;
    double T = (double)iT;

    int    Xe, Ye;              // end point
    double X0, Y0;              // center point
    int    x, y;                // current point
    int    dx1, dy1, dx2, dy2;  // delta x, y for old points

    double dS_new;
    double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

    long   iOffset_new, iOffset_old1, iOffset_old2;
    long   iDOffset_new, iDOffset_old1, iDOffset_old2;
    long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
    long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;

    double temp ;
    int    i;

    int    iUVWidth  = iWidth >> 1;
    int    iUVHeight = iHeight >> 1;
    int    loops;
    int    bDoit = 0;
    int    bUVDoit = 0;

    int    qx1, qx2;

    Xe = 0;
    Ye = iHeight-1;

    temp = T/SQRT2;
    X0 = temp;
    Y0 = (double)(Ye) - temp;

 
    qx1 = (int)((X0-Y0) - R*SQRT2); //included
    qx2 = (int)((X0-Y0) + R*SQRT2); //not

    if (qx1<0)
        qx1-=1;

    if (qx2<0)
        qx2-=1;

    for (i=0; i<iHeight; i++)
    {
        loops = min(iWidth, qx1+i+1);
        if (loops>0)
        {
            iDOffset_new = iWidth * i;
            WMVImageMemCpy(pOutY+iDOffset_new, pIn2Y+iDOffset_new, loops);
        }

        loops = min(iWidth, iWidth-qx2-i);
        if (loops>0)
        {
            iDOffset_new = iWidth * i + max(qx2 + i, 0);
            WMVImageMemCpy(pOutY+iDOffset_new, pIn1Y+iDOffset_new, loops);
        }

    }

    for (i=0; i<iHeight; i+=2)
    {
        loops = min( ((qx1+i)>>1) + 1, iUVWidth);
        if (loops>0)
        {
            iDOffset_new = iUVWidth * (i>>1);
            WMVImageMemCpy(pOutU+iDOffset_new, pIn2U+iDOffset_new, loops);
            WMVImageMemCpy(pOutV+iDOffset_new, pIn2V+iDOffset_new, loops);
        }

        loops = min(iUVWidth-((qx2+i)>>1), iUVWidth);
        if (loops>0)
        {
            iDOffset_new = iUVWidth * (i>>1) + ((qx2+i)>>1);
            WMVImageMemCpy(pOutU+iDOffset_new, pIn1U+iDOffset_new, loops);
            WMVImageMemCpy(pOutV+iDOffset_new, pIn1V+iDOffset_new, loops);
        }
    }

    iDOffset_new   = iWidth   + 1;
    iDUVOffset_new = iUVWidth + 1;
  
    for (i = max(0, qx1+1); i < min(qx2, iWidth); i++) 
    {
        x = i;  y = 0;
    
        iOffset_new  = i;
        iOffset_old1 = iOffset_old2 = iOffset_new;

        dS_new = ((double)(-x + y) + X0 - Y0) / SQRT2;
        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new; 
        dDS2 = dS_old2 - dS_new;

        dy1 = (int)( dDS1 / SQRT2 + 0.5 );
        dx1 = -dy1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dy2 = (int)( dDS2 / SQRT2 + 0.5 );
        dx2 = -dy2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1-i, dx1+iHeight, -dx2-i, dx2+iHeight, min(iWidth-i, iHeight));
        }

        // U, V

        if (i & 1)
          continue;

        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = -dx1;
        dx2 = dx2 >> 1;
        dy2 = -dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
        
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-x, dx1+iUVHeight, -dx2-x, dx2+iUVHeight, min(iUVWidth-x, iUVHeight));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-x, dx1+iUVHeight, -dx2-x, dx2+iUVHeight, min(iUVWidth-x, iUVHeight));
        }
    }

    for ( i = max(-qx2+1, 1); i < min(-qx1, iHeight); i++) 
    {
        x = 0;  y = i;

        iOffset_new = i * iWidth;
        iOffset_old1 = iOffset_old2 = iOffset_new;

        dS_new = ((double)(-x + y) + X0 - Y0) / SQRT2;
        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dy1 = (int)( dDS1 / SQRT2 + 0.5 );
        dx1 = -dy1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dy2 = (int)( dDS2 / SQRT2 + 0.5 );
        dx2 = -dy2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1, dx1+iHeight-i, -dx2, dx2+iHeight-i, min(iWidth, iHeight-i));
        }

        if (i&1) continue;

        x = 0;
        y = i;

        iUVOffset_new = (y>>1) * iUVWidth;
        iDUVOffset_old1 = (dy1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = (dy2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

//Accuracy because of dx1, dx2 being minimize as 2.
#if 0
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            y >>= 1;
            dx1 >>= 1;
            dx2 >>= 1;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+iUVHeight-y, -dx2, dx2+iUVHeight-y, min(iUVWidth, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+iUVHeight-y, -dx2, dx2+iUVHeight-y, min(iUVWidth, iUVHeight-y));
        }
#else
        while ( x < iWidth && y < iHeight) 
        {
            if ( (x+dx2)>=0 && (y+dy2)<iHeight) {
              // outer part
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
            } else if ( (x+dx1)>=0 && (y+dy1)<iHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
              } else {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
              }
            } else {
              // mapping point out of range.
                pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
            }

            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;

            x+=2; y+=2;
        }
#endif
    }
}

#else

void ImageRoll_LeftBottom(I32_WMV     iWidth,     // bitmap width
                          I32_WMV     iHeight,    // bitmap height
                          U8_WMV*     pIn1Y,      // input 1
                          U8_WMV*     pIn1U,
                          U8_WMV*     pIn1V,
                          U8_WMV*     pIn2Y,      // input 2
                          U8_WMV*     pIn2U,
                          U8_WMV*     pIn2V,
                          U8_WMV*     pOutY,      // output
                          U8_WMV*     pOutU,
                          U8_WMV*     pOutV,
                          int     iR,
                          int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1, dy1, dx2, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new, iUVOffset_old1, iUVOffset_old2;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  int    loops;
  int    bDoit = 0;
  int    bUVDoit = 0;
  
  Xe = 0;
  Ye = iHeight-1;

  temp = T/SQRT2;
  X0 = temp;
  Y0 = (double)(Ye) - temp;
  
  iDOffset_new   = iWidth   + 1;
  iDUVOffset_new = iUVWidth + 1;
  
  for (i = 0; i < iWidth; i++) 
  {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(-x + y) + X0 - Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min(iHeight, iWidth - i);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, iUVWidth - x);

        iUVOffset_new = x;
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);

    }
    else if ( dS_new > R ) 
    {
        loops = min(iHeight, iWidth - i);

        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        x = i>>1;
        loops = min(iUVHeight, iUVWidth - x);

        iUVOffset_new = x;

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else 
    {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new; 
        dDS2 = dS_old2 - dS_new;

        dy1 = (int)( dDS1 / SQRT2 + 0.5 );
        dx1 = -dy1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dy2 = (int)( dDS2 / SQRT2 + 0.5 );
        dx2 = -dy2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1-i, dx1+iHeight, -dx2-i, dx2+iHeight, min(iWidth-i, iHeight));
        }
#else
        // Y
        while ( x < iWidth && y < iHeight) 
        {
            if ( (x+dx2)>=0 && (y+dy2)< iHeight) {
              // outer part
              pOutY[iOffset_new] = pIn1Y[iOffset_old2];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
            } else if ( (x+dx1)>=0 && (y+dy1)<iHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutY[iOffset_new] = pIn1Y[iOffset_old1];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
              } else {
                pOutY[iOffset_new] = pIn1Y[iOffset_new];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
              }
            } else {
              // mapping point out of range.
              pOutY[iOffset_new] = pIn2Y[iOffset_new];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
            }

          x++; y++;
          iOffset_new  += iDOffset_new;
          iOffset_old1 += iDOffset_new;
          iOffset_old2 += iDOffset_new;
        }
    
#endif
        // U, V

        if (i & 1)
          continue;

        x = i>>1; y = 0;
        dx1 = dx1 >> 1;
        dy1 = -dx1;
        dx2 = dx2 >> 1;
        dy2 = -dx2;

        iUVOffset_new = x;

        iDUVOffset_old1 = dy1 * iUVWidth + dx1;
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = dy2 * iUVWidth + dx2;
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
        
#if 1
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-x, dx1+iUVHeight, -dx2-x, dx2+iUVHeight, min(iUVWidth-x, iUVHeight));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1-x, dx1+iUVHeight, -dx2-x, dx2+iUVHeight, min(iUVWidth-x, iUVHeight));
        }
#else
        while ( x < iUVWidth && y < iUVHeight) {
            if ( (x+dx2)>=0 && (y+dy2)<iUVHeight ) {
              // outer part
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
            } else if ( (x+dx1)>=0 && (y+dy1)<iUVHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
              } else {
                  pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                  pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
                //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
              }
            } else {
              // mapping point out of range.
              pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
              //WMVImageMemCpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
            }

          x++; y++;
          iUVOffset_new  += iDUVOffset_new;
          iUVOffset_old1 += iDUVOffset_new;
          iUVOffset_old2 += iDUVOffset_new;
        }     
#endif
    }
  }

  for ( i = 1; i < iHeight; i++) {
    x = 0;  y = i;

    iOffset_new = i * iWidth;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(-x + y) + X0 - Y0) / SQRT2;

    if ( dS_new < -R ) 
    {
        loops = min(iHeight-i, iWidth);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        y=y>>1;
        iUVOffset_new = y * iUVWidth;
        loops = min(iUVHeight-y, iUVWidth);

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iDUVOffset_new, loops);
    }
    else if ( dS_new > R ) 
    {
        loops = min(iHeight-i, iWidth);
        ImageRoll_LineQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iDOffset_new, loops);

        if (i & 1)
          continue;

        y=y>>1;
        iUVOffset_new = y * iUVWidth;
        loops = min(iUVHeight-y, iUVWidth);

        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iDUVOffset_new, loops);

    }
    else {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dDS1 = dS_old1 - dS_new;
        dDS2 = dS_old2 - dS_new;

        dy1 = (int)( dDS1 / SQRT2 + 0.5 );
        dx1 = -dy1;
        iDOffset_old1 = dy1 * iWidth + dx1;
        iOffset_old1 += iDOffset_old1;

        dy2 = (int)( dDS2 / SQRT2 + 0.5 );
        dx2 = -dy2;
        iDOffset_old2 = dy2 * iWidth + dx2;
        iOffset_old2 += iDOffset_old2;

#if 1
        {
            U8_WMV *outY, *old1Y, *old2Y, *new1Y, *new2Y;

            outY = pOutY + iOffset_new;
            old1Y = pIn1Y + iOffset_old1;
            old2Y = pIn1Y + iOffset_old2;
            new1Y = pIn1Y + iOffset_new;
            new2Y = pIn2Y + iOffset_new;
            ImageRoll_FillInsideOut(outY, old1Y, old2Y, new1Y, new2Y, iDOffset_new, dS_new, -dx1, dx1+iHeight-i, -dx2, dx2+iHeight-i, min(iWidth, iHeight-i));
        }
#else
        while ( x < iWidth && y < iHeight) {
            if ( (x+dx2)>=0 && (y+dy2)<iHeight) {
              // outer part
              pOutY[iOffset_new] = pIn1Y[iOffset_old2];
            } else if ( (x+dx1)>=0 && (y+dy1)<iHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutY[iOffset_new] = pIn1Y[iOffset_old1];
              } else {
                pOutY[iOffset_new] = pIn1Y[iOffset_new];
              }
            } else {
              // mapping point out of range.
              pOutY[iOffset_new] = pIn2Y[iOffset_new];
            }

          x++; y++;
          iOffset_new  += iDOffset_new;
          iOffset_old1 += iDOffset_new;
          iOffset_old2 += iDOffset_new;
        }
#endif

        if (i&1) continue;

        x = 0;
        y = i;

        iUVOffset_new = (y>>1) * iUVWidth;
        iDUVOffset_old1 = (dy1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = (dy2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;

//Accuracy because of dx1, dx2 being minimize as 2.
#if 0
        {
            U8_WMV *outUV, *old1UV, *old2UV, *new1UV, *new2UV;

            y >>= 1;
            dx1 >>= 1;
            dx2 >>= 1;

            outUV = pOutU + iUVOffset_new;
            old1UV = pIn1U + iUVOffset_old1;
            old2UV = pIn1U + iUVOffset_old2;
            new1UV = pIn1U + iUVOffset_new;
            new2UV = pIn2U + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+iUVHeight-y, -dx2, dx2+iUVHeight-y, min(iUVWidth, iUVHeight-y));

            outUV = pOutV + iUVOffset_new;
            old1UV = pIn1V + iUVOffset_old1;
            old2UV = pIn1V + iUVOffset_old2;
            new1UV = pIn1V + iUVOffset_new;
            new2UV = pIn2V + iUVOffset_new;
            ImageRoll_FillInsideOut(outUV, old1UV, old2UV, new1UV, new2UV, iDUVOffset_new, dS_new, -dx1, dx1+iUVHeight-y, -dx2, dx2+iUVHeight-y, min(iUVWidth, iUVHeight-y));
        }
#else
        while ( x < iWidth && y < iHeight) 
        {
            if ( (x+dx2)>=0 && (y+dy2)<iHeight) {
              // outer part
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
            } else if ( (x+dx1)>=0 && (y+dy1)<iHeight ) {
              // inner part
              if (dS_new >= 0) {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
              } else {
                pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
              }
            } else {
              // mapping point out of range.
                pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
                pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
            }

            iUVOffset_new  += iDUVOffset_new;
            iUVOffset_old1 += iDUVOffset_new;
            iUVOffset_old2 += iDUVOffset_new;

            x+=2; y+=2;
        }
#endif
    }
  }
}

#endif






#else //WVP2_C_OPT


void ImageRoll_RightBottom(I32_WMV     iWidth,     // bitmap width
                           I32_WMV     iHeight,    // bitmap height
                           U8_WMV*     pIn1Y,   // input 1
                           U8_WMV*     pIn1U,
                           U8_WMV*     pIn1V,
                           U8_WMV*     pIn2Y,   // input 2
                           U8_WMV*     pIn2U,
                           U8_WMV*     pIn2V,
                           U8_WMV*     pOutY,   // output
                           U8_WMV*     pOutU,
                           U8_WMV*     pOutV,
                           int     iR,
                           int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1, dy1, dx2=0, dy2=0;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new=0, iUVOffset_old1=0, iUVOffset_old2=0;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = iWidth - 1;
  Ye = iHeight - 1;

  X0 = (double)(Xe) - T/SQRT2;
  Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  iDOffset_new   = iWidth - 1;
  iDUVOffset_new = iUVWidth -1;
  
  for (i = 0; i < iWidth; i++) {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x + y) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new;
      dDS2 = dS_old2 - dS_new;

      dx1 = (int)( dDS1 / SQRT2 + 0.5 );
      dy1 = dx1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dx2 = (int)( dDS2 / SQRT2 + 0.5 );
      dy2 = dx2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    // Y
    
    while ( x >= 0 && y < iHeight) {
      
      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)<iWidth && (y+dy2)<iHeight) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)<iWidth && (y+dy1)<iHeight ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x--; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    x = i>>1; y = 0;
    dx1 = dx1 >> 1;
    dy1 = dx1;
    dx2 = dx2 >> 1;
    dy2 = dx2;

    iUVOffset_new = x;

    if (bDoit == 1) {
      iDUVOffset_old1 = dy1 * iUVWidth + dx1;
      iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
      iDUVOffset_old2 = dy2 * iUVWidth + dx2;
      iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
    } else {
      iUVOffset_old1 = iUVOffset_old2 = iUVOffset_new;
    }
        
    while ( x >= 0 && y < iUVHeight) {
      if ( bDoit == 0 ) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)<iUVWidth && (y+dy2)<iUVHeight) {
          // outer part
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)<iUVWidth && (y+dy1)<iUVHeight ) {
          // inner part
          if (dS_new >= 0) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x--; y++;
      iUVOffset_new  += iDUVOffset_new;
      iUVOffset_old1 += iDUVOffset_new;
      iUVOffset_old2 += iDUVOffset_new;
    }     

  }
  
  for ( i = 1; i < iHeight; i++) {
    x = Xe; y = i;

    iOffset_new = i * iWidth + x;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x + y) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new;
      dDS2 = dS_old2 - dS_new;

      dx1 = (int)( dDS1 / SQRT2 + 0.5 );
      dy1 = dx1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dx2 = (int)( dDS2 / SQRT2 + 0.5 );
      dy2 = dx2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    if ((x+y) & 1)
      bUVDoit = 0;
    else {
      bUVDoit = 1;
      iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
      if (bDoit == 1) {
        iDUVOffset_old1 = (dy1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = (dy2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
      }
    }

    while ( x >= 0 && y < iHeight) {

      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        if ( bUVDoit && !(y&1) ) {
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        if ( bUVDoit && !(y&1) ) {
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)<iWidth && (y+dy2)<iHeight) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          if ( bUVDoit && !(y&1) ) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)<iWidth && (y+dy1)<iHeight ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            if ( bUVDoit && !(y&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            if ( bUVDoit && !(y&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          if ( bUVDoit && !(y&1) ) {
            pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
            pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      if ( bUVDoit && !(y&1) ) {
        iUVOffset_new  += iDUVOffset_new;
        if (bDoit == 1) {
          iUVOffset_old1 += iDUVOffset_new;
          iUVOffset_old2 += iDUVOffset_new;
        }
      }
      
      x--; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
  }
 
}



void ImageRoll_RightTop(I32_WMV     iWidth,     // bitmap width
                        I32_WMV     iHeight,    // bitmap height
                        U8_WMV*     pIn1Y,      // input 1
                        U8_WMV*     pIn1U,
                        U8_WMV*     pIn1V,
                        U8_WMV*     pIn2Y,      // input 2
                        U8_WMV*     pIn2U,
                        U8_WMV*     pIn2V,
                        U8_WMV*     pOutY,      // output
                        U8_WMV*     pOutU,
                        U8_WMV*     pOutV,
                        int     iR,
                        int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1, dy1, dx2=0, dy2=0;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new=0, iUVOffset_old1=0, iUVOffset_old2=0;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = iWidth - 1;
  Ye = 0;

  temp = T/SQRT2;
  X0 = (double)(Xe) - temp;
  Y0 = (double)(Ye) + temp;
  
  iDOffset_new   = iWidth   + 1;
  iDUVOffset_new = iUVWidth + 1;
  
  for (i = 0; i < iWidth; i++) {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x - y) - X0 + Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new; 
      dDS2 = dS_old2 - dS_new;

      dx1 = (int)( dDS1 / SQRT2 + 0.5 );
      dy1 = -dx1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dx2 = (int)( dDS2 / SQRT2 + 0.5 );
      dy2 = -dx2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    // Y
    
    while ( x < iWidth && y < iHeight) {
      
      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)<iWidth && (y+dy2)>=0) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)<iWidth && (y+dy1)>=0 ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x++; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    x = i>>1; y = 0;
    dx1 = dx1 >> 1;
    dy1 = -dx1;
    dx2 = dx2 >> 1;
    dy2 = -dx2;

    iUVOffset_new = x;

    if (bDoit == 1) {
      iDUVOffset_old1 = dy1 * iUVWidth + dx1;
      iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
      iDUVOffset_old2 = dy2 * iUVWidth + dx2;
      iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
    } else {
      iUVOffset_old1 = iUVOffset_old2 = iUVOffset_new;
    }
        
    while ( x < iUVWidth && y < iUVHeight) {
      if ( bDoit == 0 ) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)<iUVWidth && (y+dy2)>=0) {
          // outer part
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)<iUVWidth && (y+dy1)>=0 ) {
          // inner part
          if (dS_new >= 0) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x++; y++;
      iUVOffset_new  += iDUVOffset_new;
      iUVOffset_old1 += iDUVOffset_new;
      iUVOffset_old2 += iDUVOffset_new;
    }     

  }
  
  for ( i = 1; i < iHeight; i++) {
    x = 0;  y = i;

    iOffset_new = i * iWidth;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(x - y) - X0 + Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new;
      dDS2 = dS_old2 - dS_new;

      dx1 = (int)( dDS1 / SQRT2 + 0.5 );
      dy1 = -dx1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dx2 = (int)( dDS2 / SQRT2 + 0.5 );
      dy2 = -dx2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    if (y & 1)
      bUVDoit = 0;
    else {
      bUVDoit = 1;
      iUVOffset_new = (y>>1) * iUVWidth;
      if (bDoit == 1) {
        iDUVOffset_old1 = -(dx1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = -(dx2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
      }
    }

    while ( x < iWidth && y < iHeight) {

      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        if ( bUVDoit && !(x&1) ) {
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        if ( bUVDoit && !(x&1) ) {
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)<iWidth && (y+dy2)>=0) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          if ( bUVDoit && !(x&1) ) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)<iWidth && (y+dy1)>=0 ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            if ( bUVDoit && !(x&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            if ( bUVDoit && !(x&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          if ( bUVDoit && !(x&1) ) {
            pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
            pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      if ( bUVDoit && !(x&1) ) {
        iUVOffset_new  += iDUVOffset_new;
        if (bDoit == 1) {
          iUVOffset_old1 += iDUVOffset_new;
          iUVOffset_old2 += iDUVOffset_new;
        }
      }
      
      x++; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
  }
 
}



void ImageRoll_LeftTop(I32_WMV     iWidth,     // bitmap width
                       I32_WMV     iHeight,    // bitmap height
                       U8_WMV*     pIn1Y,   // input 1
                       U8_WMV*     pIn1U,
                       U8_WMV*     pIn1V,
                       U8_WMV*     pIn2Y,   // input 2
                       U8_WMV*     pIn2U,
                       U8_WMV*     pIn2V,
                       U8_WMV*     pOutY,   // output
                       U8_WMV*     pOutU,
                       U8_WMV*     pOutV,
                       int     iR,
                       int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1=0, dy1=0, dx2=0, dy2=0;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new=0, iUVOffset_old1=0, iUVOffset_old2=0;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = 0;
  Ye = 0;

  X0 = T/SQRT2;
  Y0 = X0;
  
  iDOffset_new   = iWidth - 1;
  iDUVOffset_new = iUVWidth -1;
  
  for (i = 0; i < iWidth; i++) {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = -((double)(x) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new;
      dDS2 = dS_old2 - dS_new;

      dx1 = -(int)( dDS1 / SQRT2 + 0.5 );
      dy1 = dx1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dx2 = -(int)( dDS2 / SQRT2 + 0.5 );
      dy2 = dx2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    // Y
    
    while ( x >= 0 && y < iHeight) {
      
      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)>=0 && (y+dy2)>=0 ) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)>=0 && (y+dy1)>=0 ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x--; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    x = i>>1; y = 0;
    dx1 = dx1 >> 1;
    dy1 = dx1;
    dx2 = dx2 >> 1;
    dy2 = dx2;

    iUVOffset_new = x;

    if (bDoit == 1) {
      iDUVOffset_old1 = dy1 * iUVWidth + dx1;
      iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
      iDUVOffset_old2 = dy2 * iUVWidth + dx2;
      iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
    } else {
      iUVOffset_old1 = iUVOffset_old2 = iUVOffset_new;
    }
        
    while ( x >= 0 && y < iUVHeight) {
      if ( bDoit == 0 ) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)>=0 && (y+dy2)>=0) {
          // outer part
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)>=0 && (y+dy1)>=0 ) {
          // inner part
          if (dS_new >= 0) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x--; y++;
      iUVOffset_new  += iDUVOffset_new;
      iUVOffset_old1 += iDUVOffset_new;
      iUVOffset_old2 += iDUVOffset_new;
    }     

  }
  
  for ( i = 1; i < iHeight; i++) {
    x = iWidth - 1; y = i;

    iOffset_new = i * iWidth + x;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = -((double)(x + y) - X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new;
      dDS2 = dS_old2 - dS_new;

      dx1 = -(int)( dDS1 / SQRT2 + 0.5 );
      dy1 = dx1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dx2 = -(int)( dDS2 / SQRT2 + 0.5 );
      dy2 = dx2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    if ( !(y & 1))
      bUVDoit = 0;
    else {
      bUVDoit = 1;
      iUVOffset_new = ((y+1)>>1) * iUVWidth + ((x-1)>>1);
      if (bDoit == 1) {
        iDUVOffset_old1 = (dy1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = (dy2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
      }
    }

    while ( x >= 0 && y < iHeight) {

      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        if ( bUVDoit && !(y&1) ) {
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        if ( bUVDoit && !(y&1) ) {
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)>=0 && (y+dy2)>=0) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          if ( bUVDoit && !(y&1) ) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)>=0 && (y+dy1)>=0 ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            if ( bUVDoit && !(y&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            if ( bUVDoit && !(y&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          if ( bUVDoit && !(y&1) ) {
            pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
            pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      if ( bUVDoit && !(y&1) ) {
        iUVOffset_new  += iDUVOffset_new;
        if (bDoit == 1) {
          iUVOffset_old1 += iDUVOffset_new;
          iUVOffset_old2 += iDUVOffset_new;
        }
      }
      
      x--; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
  }
}


void ImageRoll_LeftBottom(I32_WMV     iWidth,     // bitmap width
                          I32_WMV     iHeight,    // bitmap height
                          U8_WMV*     pIn1Y,      // input 1
                          U8_WMV*     pIn1U,
                          U8_WMV*     pIn1V,
                          U8_WMV*     pIn2Y,      // input 2
                          U8_WMV*     pIn2U,
                          U8_WMV*     pIn2V,
                          U8_WMV*     pOutY,      // output
                          U8_WMV*     pOutU,
                          U8_WMV*     pOutV,
                          int     iR,
                          int     iT)
{
  double R = (double)iR;
  double T = (double)iT;
  
  int    Xe, Ye;              // end point
  double X0, Y0;              // center point
  int    x, y;                // current point
  int    dx1=0, dy1=0, dx2=0, dy2=0;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2, dDS1, dDS2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old1, iOffset_old2;
  long   iDOffset_new, iDOffset_old1, iDOffset_old2;
  long   iUVOffset_new=0, iUVOffset_old1=0, iUVOffset_old2=0;
  long   iDUVOffset_new, iDUVOffset_old1, iDUVOffset_old2;
    
  int    bDoit = 0;
  int    bUVDoit = 0;
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = 0;
  Ye = iHeight-1;

  temp = T/SQRT2;
  X0 = temp;
  Y0 = (double)(Ye) - temp;
  
  iDOffset_new   = iWidth   + 1;
  iDUVOffset_new = iUVWidth + 1;
  
  for (i = 0; i < iWidth; i++) {
    x = i;  y = 0;
    
    iOffset_new  = i;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(-x + y) + X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      dx2 = 0; dy2 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new; 
      dDS2 = dS_old2 - dS_new;

      dy1 = (int)( dDS1 / SQRT2 + 0.5 );
      dx1 = -dy1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dy2 = (int)( dDS2 / SQRT2 + 0.5 );
      dx2 = -dy2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    // Y
    
    while ( x < iWidth && y < iHeight) {
      
      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)>=0 && (y+dy2)< iHeight) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)>=0 && (y+dy1)<iHeight ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x++; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    x = i>>1; y = 0;
    dx1 = dx1 >> 1;
    dy1 = -dx1;
    dx2 = dx2 >> 1;
    dy2 = -dx2;

    iUVOffset_new = x;

    if (bDoit == 1) {
      iDUVOffset_old1 = dy1 * iUVWidth + dx1;
      iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
      iDUVOffset_old2 = dy2 * iUVWidth + dx2;
      iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
    } else {
      iUVOffset_old1 = iUVOffset_old2 = iUVOffset_new;
    }
        
    while ( x < iUVWidth && y < iUVHeight) {
      if ( bDoit == 0 ) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)>=0 && (y+dy2)<iUVHeight ) {
          // outer part
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)>=0 && (y+dy1)<iUVHeight ) {
          // inner part
          if (dS_new >= 0) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      x++; y++;
      iUVOffset_new  += iDUVOffset_new;
      iUVOffset_old1 += iDUVOffset_new;
      iUVOffset_old2 += iDUVOffset_new;
    }     

  }
  
  for ( i = 1; i < iHeight; i++) {
    x = 0;  y = i;

    iOffset_new = i * iWidth;
    iOffset_old1 = iOffset_old2 = iOffset_new;

    dS_new = ((double)(-x + y) + X0 - Y0) / SQRT2;

    if ( dS_new < -R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 0;

    } else if ( dS_new > R ) {
      dx1 = 0; dy1 = 0;
      bDoit = 2;

    } else {
      bDoit = 1;

      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dDS1 = dS_old1 - dS_new;
      dDS2 = dS_old2 - dS_new;

      dy1 = (int)( dDS1 / SQRT2 + 0.5 );
      dx1 = -dy1;
      iDOffset_old1 = dy1 * iWidth + dx1;
      iOffset_old1 += iDOffset_old1;
      
      dy2 = (int)( dDS2 / SQRT2 + 0.5 );
      dx2 = -dy2;
      iDOffset_old2 = dy2 * iWidth + dx2;
      iOffset_old2 += iDOffset_old2;
    }

    if (y & 1)
      bUVDoit = 0;
    else {
      bUVDoit = 1;
      iUVOffset_new = (y>>1) * iUVWidth;
      if (bDoit == 1) {
        iDUVOffset_old1 = (dy1>>1) * iUVWidth + (dx1>>1);
        iUVOffset_old1  = iUVOffset_new + iDUVOffset_old1;
        iDUVOffset_old2 = (dy2>>1) * iUVWidth + (dx2>>1);
        iUVOffset_old2  = iUVOffset_new + iDUVOffset_old2;
      }
    }

    while ( x < iWidth && y < iHeight) {

      if ( bDoit == 0 ) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        if ( bUVDoit && !(x&1) ) {
          pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
      } else if ( bDoit == 2 ) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        if ( bUVDoit && !(x&1) ) {
          pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
          pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        }
        //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
      } else {
        if ( (x+dx2)>=0 && (y+dy2)<iHeight) {
          // outer part
          pOutY[iOffset_new] = pIn1Y[iOffset_old2];
          if ( bUVDoit && !(x&1) ) {
            pOutU[iUVOffset_new] = pIn1U[iUVOffset_old2];
            pOutV[iUVOffset_new] = pIn1V[iUVOffset_old2];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old2, 3);
        } else if ( (x+dx1)>=0 && (y+dy1)<iHeight ) {
          // inner part
          if (dS_new >= 0) {
            pOutY[iOffset_new] = pIn1Y[iOffset_old1];
            if ( bUVDoit && !(x&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_old1];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_old1];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_old1, 3);
          } else {
            pOutY[iOffset_new] = pIn1Y[iOffset_new];
            if ( bUVDoit && !(x&1) ) {
              pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
              pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
            }
            //memcpy(pImgOut+iOffset_new, pImgIn1+iOffset_new, 3);
          }
        } else {
          // mapping point out of range.
          pOutY[iOffset_new] = pIn2Y[iOffset_new];
          if ( bUVDoit && !(x&1) ) {
            pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
            pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
          }
          //memcpy(pImgOut+iOffset_new, pImgIn2+iOffset_new, 3);
        }
      }

      if ( bUVDoit && !(x&1) ) {
        iUVOffset_new  += iDUVOffset_new;
        if (bDoit == 1) {
          iUVOffset_old1 += iDUVOffset_new;
          iUVOffset_old2 += iDUVOffset_new;
        }
      }
      
      x++; y++;
      iOffset_new  += iDOffset_new;
      iOffset_old1 += iDOffset_new;
      iOffset_old2 += iDOffset_new;
    }
  }
 
}
#endif //WMV_C_OPT_WVP2

