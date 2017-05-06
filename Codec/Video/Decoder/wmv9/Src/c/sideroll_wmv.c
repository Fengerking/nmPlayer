//*@@@+++@@@@*******************************************************************
//
// Microsoft Windows Media
// Copyright (C) 2003 Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@*******************************************************************
//
// File:    Sideroll_wmv.c
//
// Desc:    Implementation of sideroll effect in PhotoMotion Mode
//
// Author:  Peter X. Zuo (peterzuo@microsoft.com)
//          Port from sideroll.cpp desktop code.
// Date:    2/12/2004
//
//******************************************************************************
#include "xplatform_wmv.h"
#include "typedef.h"
#include "wmvdec_api.h"

#include "sideroll_wmv.h"

#define SQRT2 1.41421356
#define PI    3.14159265
#define WVP2_PAGEROLL_HORIZONTAL_FILL

#if defined(WMV_C_OPT_WVP2) && defined(WMV_OPT_WVP2_ARM)
extern void WMVImageMemCpy(U8_WMV*pDest, U8_WMV*pSrc, I32_WMV iSize);
#else
#define WMVImageMemCpy memcpy
#endif

#ifdef WMV_C_OPT_WVP2

#ifdef WVP2_PAGEROLL_HORIZONTAL_FILL

void SideRoll_Right(I32_WMV     iWidth,     // bitmap width
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

    int    Xe;              // end point
    double X0;              // center point
    int    x;                // current point
    int    dx1, dx2;  // delta x, y for old points

    double dS_new;
    double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

    long   iOffset_new, iOffset_old;
    long   iUVOffset_new, iUVOffset_old;

    int    bDoit = 0;
    double temp ;

    int    iUVWidth  = iWidth >> 1;
    int    iUVHeight = iHeight >> 1;

    int    qx1, qx2, i, loops;

    Xe = iWidth - 1;
    //Ye = iHeight - 1;

    X0 = (double)(Xe) - T;
    //Y0 = X0 - (double)(Xe) + (double)(Ye);

    //iDOffset_new   = iWidth;
    //iDUVOffset_new = iUVWidth;

    qx1 = (int)(X0-R); //include
    qx2 = (int)(X0+R); //not include

    //if right on the boundary. exclude that pixel
    if (X0-R-qx1==0.0)
        qx1--;

	for (i=0; i<iHeight; i++)
	{
		loops = min((qx1+1), iWidth);
		if (loops>0)
		{
			iOffset_new = i*iWidth;
			WMVImageMemCpy(pOutY+iOffset_new, pIn1Y+iOffset_new, loops);
		}
		loops = min(iWidth-qx2-1, iWidth);
		if (loops>0)
		{
			iOffset_new = i*iWidth+iWidth-loops;
			WMVImageMemCpy(pOutY+iOffset_new, pIn2Y+iOffset_new, loops);
		}
	}

	for (i=0; i<iHeight; i+=2)
	{
		loops = min( (qx1>>1) +1, iUVWidth);
		if (loops>0)
		{
			iOffset_new = (i>>1)*iUVWidth;
			WMVImageMemCpy(pOutU+iOffset_new, pIn1U+iOffset_new, loops);
			WMVImageMemCpy(pOutV+iOffset_new, pIn1V+iOffset_new, loops);
		}

		loops = min(iUVWidth-(qx2>>1)-1, iUVWidth);
		if (loops>0)
		{
			iOffset_new = (i>>1)*iUVWidth+iUVWidth-loops;
			WMVImageMemCpy(pOutU+iOffset_new, pIn2U+iOffset_new, loops);
			WMVImageMemCpy(pOutV+iOffset_new, pIn2V+iOffset_new, loops);
		}
	}


    for (x = max(0, qx1+1); x < min(qx2+1, iWidth); x++) 
    {
        iOffset_new  = x;

        dS_new = ((double)(x) - X0);
        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dx1 = (int)( dS_old1 - dS_new + 0.5 );
        dx2 = (int)( dS_old2 - dS_new + 0.5 );

        if ( (x+dx2)< iWidth) {
            bDoit = 1;
            iOffset_old = x + dx2;
            iUVOffset_old = (x>>1) + (dx2>>1);
        }
        else
        if ( (x+dx1) < iWidth ) 
        {
            if (dS_new > 0) 
            {
                bDoit = 1;
                iOffset_old = x + dx1;
                iUVOffset_old = (x>>1) + (dx1>>1);
            }
            else
                bDoit = 0;
        }
        else
            bDoit = 2;

        // Y
        if (bDoit == 0) 
        {
            ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iWidth, iHeight);
        }
        else if (bDoit == 1) {
            ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_old, iWidth, iHeight);
        }
        else
        { // if (bDoit == 2) 
            ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iWidth, iHeight);
        }
    
        // U, V
    
        if (x & 1)
          continue;
    
        iUVOffset_new = x>>1;

        if (bDoit == 0) 
        {
            ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iUVWidth, iUVHeight);
        } 
        else if (bDoit == 1) 
        {
            ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_old, pOutV+iUVOffset_new, pIn1V+iUVOffset_old, iUVWidth, iUVHeight);
        } 
        else if (bDoit == 2) 
        {
            ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iUVWidth, iUVHeight);
        }
    }
}


void SideRoll_Left(I32_WMV     iWidth,  // bitmap width
                   I32_WMV     iHeight, // bitmap height
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
                   int     iT
                   )
{
    double R = (double)iR;
    double T = (double)iT;

    int    Xe;              // end point
    double X0;              // center point
    int    x;                // current point
    int    dx1, dx2;  // delta x, y for old points

    double dS_new;
    double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

    long   iOffset_new, iOffset_old;
    long   iUVOffset_new, iUVOffset_old;

    int    bDoit = 0;
    double temp ;

    int    iUVWidth  = iWidth >> 1;
    int    iUVHeight = iHeight >> 1;

    int     qx1, qx2, i, loops;

    Xe = 0;

    X0 = (double)(Xe) + T;
  
    qx1 = (int)(X0-R); //include
    qx2 = (int)(X0+R); //not include

    //if right on the boundary. exclude that pixel
    if (X0-R-qx1==0.0)
        qx1--;

	for (i=0; i<iHeight; i++)
	{
		loops = min((qx1+1), iWidth);
		if (loops>0)
		{
			iOffset_new = i*iWidth;
			WMVImageMemCpy(pOutY+iOffset_new, pIn2Y+iOffset_new, loops);
		}
		loops = min(iWidth-qx2-1, iWidth);
		if (loops>0)
		{
			iOffset_new = i*iWidth+iWidth-loops;
			WMVImageMemCpy(pOutY+iOffset_new, pIn1Y+iOffset_new, loops);
		}
	}

	for (i=0; i<iHeight; i+=2)
	{
		loops = min( (qx1>>1) +1, iUVWidth);
		if (loops>0)
		{
			iOffset_new = (i>>1)*iUVWidth;
			WMVImageMemCpy(pOutU+iOffset_new, pIn2U+iOffset_new, loops);
			WMVImageMemCpy(pOutV+iOffset_new, pIn2V+iOffset_new, loops);
		}

		loops = min(iUVWidth-(qx2>>1)-1, iUVWidth);
		if (loops>0)
		{
			iOffset_new = (i>>1)*iUVWidth+iUVWidth-loops;
			WMVImageMemCpy(pOutU+iOffset_new, pIn1U+iOffset_new, loops);
			WMVImageMemCpy(pOutV+iOffset_new, pIn1V+iOffset_new, loops);
		}
	}


    for (x = max(0, qx1+1); x < min(qx2+1, iWidth); x++) 
    {
        iOffset_new  = x;

        dS_new = X0 - (double)(x);

        //
		//if assertion, means the horiontal fill didn't cover fully the scenario. 
		//May need add old shortcut to cover 1/2 pixels near the boundary.
		//
		assert( (dS_new >= -R) && (dS_new <= R));

        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dx1 = -(int)( dS_old1 - dS_new + 0.5 );
        dx2 = -(int)( dS_old2 - dS_new + 0.5 );

        if ( (x+dx2)>=0) 
        {
            bDoit = 1;
            iOffset_old = x + dx2;
            iUVOffset_old = (x>>1) + (dx2>>1);
        }
        else if ( (x+dx1)>=0 ) 
        {
            if (dS_new > 0) 
            {
              bDoit = 1;
              iOffset_old = x + dx1;
              iUVOffset_old = (x>>1) + (dx1>>1);
            }
            else
              bDoit = 0;
        }
        else
            bDoit = 2;

        // Y
        if (bDoit == 0) {
            ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iWidth, iHeight);
        } else if (bDoit == 1) {
            ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_old, iWidth, iHeight);
        } else if (bDoit == 2) {
            ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iWidth, iHeight);
        }
    
        // U, V
    
        if (x & 1)
          continue;
    
        iUVOffset_new = x>>1;

        if (bDoit == 0) 
        {
            ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iUVWidth, iUVHeight);
        } else if (bDoit == 1) {
            ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_old, pOutV+iUVOffset_new, pIn1V+iUVOffset_old, iUVWidth, iUVHeight);
        } else if (bDoit == 2) {
            ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iUVWidth, iUVHeight);
        }
    }
}

#else

void SideRoll_Right(I32_WMV     iWidth,     // bitmap width
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
  
  int    Xe;              // end point
  double X0;              // center point
  int    x;                // current point
  int    dx1, dx2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new, iUVOffset_old;
    
  int    bDoit = 0;
  double temp ;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = iWidth - 1;
  //Ye = iHeight - 1;

  X0 = (double)(Xe) - T;
  //Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  //iDOffset_new   = iWidth;
  //iDUVOffset_new = iUVWidth;
  
  for (x = 0; x < iWidth; x++) {
    iOffset_new  = x;

    dS_new = ((double)(x) - X0);

    if ( dS_new < -R ) 
        bDoit = 0;
    else 
    if ( dS_new > R ) 
        bDoit = 2;
    else 
    {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dx1 = (int)( dS_old1 - dS_new + 0.5 );
      dx2 = (int)( dS_old2 - dS_new + 0.5 );

      if ( (x+dx2)< iWidth) {
        bDoit = 1;
        iOffset_old = x + dx2;
        iUVOffset_old = (x>>1) + (dx2>>1);
      } else if ( (x+dx1) < iWidth ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old = x + dx1;
          iUVOffset_old = (x>>1) + (dx1>>1);
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) 
    {
        ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iWidth, iHeight);
    }
    else if (bDoit == 1) {
        ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_old, iWidth, iHeight);
    }
    else
    { // if (bDoit == 2) 
        ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iWidth, iHeight);
    }
    
    // U, V
    
    if (x & 1)
      continue;
    
    iUVOffset_new = x>>1;

    if (bDoit == 0) {
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iUVWidth, iUVHeight);
    } else if (bDoit == 1) {
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_old, pOutV+iUVOffset_new, pIn1V+iUVOffset_old, iUVWidth, iUVHeight);
    } else if (bDoit == 2) {
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iUVWidth, iUVHeight);
    }
  }
}


void SideRoll_Left(I32_WMV     iWidth,  // bitmap width
                   I32_WMV     iHeight, // bitmap height
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
  
  int    Xe;              // end point
  double X0;              // center point
  int    x;                // current point
  int    dx1, dx2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new, iUVOffset_old;
    
  int    bDoit = 0;
  double temp ;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;

  Xe = 0;

  X0 = (double)(Xe) + T;
  
  for (x = 0; x < iWidth; x++) {
    iOffset_new  = x;

    dS_new = X0 - (double)(x);

    if ( dS_new < -R ) {
      bDoit = 0;
    } else if ( dS_new > R ) {
      bDoit = 2;

    } else {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dx1 = -(int)( dS_old1 - dS_new + 0.5 );
      dx2 = -(int)( dS_old2 - dS_new + 0.5 );

      if ( (x+dx2)>=0) {
        bDoit = 1;
        iOffset_old = x + dx2;
        iUVOffset_old = (x>>1) + (dx2>>1);
      } else if ( (x+dx1)>=0 ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old = x + dx1;
          iUVOffset_old = (x>>1) + (dx1>>1);
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
        ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_new, iWidth, iHeight);
    } else if (bDoit == 1) {
        ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn1Y+iOffset_old, iWidth, iHeight);
    } else if (bDoit == 2) {
        ImageRoll_LineYQuickFill(pOutY+iOffset_new, pIn2Y+iOffset_new, iWidth, iHeight);
    }
    
    // U, V
    
    if (x & 1)
      continue;
    
    iUVOffset_new = x>>1;

    if (bDoit == 0) {
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iUVWidth, iUVHeight);
    } else if (bDoit == 1) {
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn1U+iUVOffset_old, pOutV+iUVOffset_new, pIn1V+iUVOffset_old, iUVWidth, iUVHeight);
    } else if (bDoit == 2) {
        ImageRoll_LineUVQuickFill(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iUVWidth, iUVHeight);
    }
  }
}

#endif //WVP2_PAGEROLL_HORIZONTAL_FILL

void SideRoll_Bottom(I32_WMV     iWidth,     // bitmap width
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
  
  int    Ye;              // end point
  double Y0;              // center point
  int    y;                // current point
  int    dy1, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new, iUVOffset_old;
    
  int    bDoit = 0;
  double temp ;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Ye = iHeight - 1;
  Y0 = (double)(Ye) - T;
  //Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  for (y = 0; y < iHeight; y++) {
    iOffset_new = y * iWidth;
    
    dS_new = ((double)(y) - Y0);

    if ( dS_new < -R ) 
    {
      bDoit = 0;
    }
    else if ( dS_new > R ) 
    {
      bDoit = 2;
    } 
    else 
    {
        temp = asin( dS_new / R );
        dS_old1 = R * temp;
        dS_old2 = R * (PI - temp);

        dy1 = (int)( dS_old1 - dS_new + 0.5 );
        dy2 = (int)( dS_old2 - dS_new + 0.5 );

        if ( (y+dy2)< iHeight) {
            bDoit = 1;
            iOffset_old = iOffset_new + dy2*iWidth;
        }
        else if ( (y+dy1) < iHeight ) 
        {
            if (dS_new > 0) {
                bDoit = 1;
                iOffset_old   = iOffset_new+dy1*iWidth;
            }
            else
              bDoit = 0;
        }
        else
            bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
        WMVImageMemCpy(pOutY+iOffset_new, pIn1Y+iOffset_new, iWidth);
    } else if (bDoit == 1) {
        WMVImageMemCpy(pOutY+iOffset_new, pIn1Y+iOffset_old, iWidth);
    } else if (bDoit == 2) {
        WMVImageMemCpy(pOutY+iOffset_new, pIn2Y+iOffset_new, iWidth);
    }
    
    // U, V
    
    if (y & 1)
      continue;
    
    iUVOffset_new = (y>>1)*iUVWidth;

    if (bDoit == 1)
    {
        if ( (y+dy2)< iHeight) {
            iUVOffset_old = iUVOffset_new+ (dy2>>1)*iUVWidth;
        }
        else
        {
            iUVOffset_old = iUVOffset_new + (dy1>>1)*iUVWidth;
        }
    }

    if (bDoit == 0) {
        WMVImageMemCpy(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, iUVWidth);
        WMVImageMemCpy(pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iUVWidth);
    } else if (bDoit == 1) {
        WMVImageMemCpy(pOutU+iUVOffset_new, pIn1U+iUVOffset_old, iUVWidth);
        WMVImageMemCpy(pOutV+iUVOffset_new, pIn1V+iUVOffset_old, iUVWidth);
    } else if (bDoit == 2) {
        WMVImageMemCpy(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, iUVWidth);
        WMVImageMemCpy(pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iUVWidth);
    }
  }
}

void SideRoll_Top(I32_WMV     iWidth,     // bitmap width
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
  
  int    Ye;              // end point
  double Y0;              // center point
  int    y;                // current point
  int    dy1, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new, iUVOffset_old;
    
  int    bDoit = 0;
  double temp ;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Ye = 0;
  //Ye = iHeight - 1;

  Y0 = T;
  
  for (y = 0; y < iHeight; y++) {
    iOffset_new = y * iWidth;
    
    dS_new = (Y0 - (double)(y));

    if ( dS_new < -R ) {
      bDoit = 0;

    } else if ( dS_new > R ) {
      bDoit = 2;

    } else {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dy1 = -(int)( dS_old1 - dS_new + 0.5 );
      dy2 = -(int)( dS_old2 - dS_new + 0.5 );

      if ( (y+dy2) >= 0) {
        bDoit = 1;
        iOffset_old = iOffset_new + dy2*iWidth;
      } else if ( (y+dy1) >= 0 ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old = iOffset_new + dy1*iWidth;
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
        WMVImageMemCpy(pOutY+iOffset_new, pIn1Y+iOffset_new, iWidth);
    } else if (bDoit == 1) {
        WMVImageMemCpy(pOutY+iOffset_new, pIn1Y+iOffset_old, iWidth);
    } else if (bDoit == 2) {
        WMVImageMemCpy(pOutY+iOffset_new, pIn2Y+iOffset_new, iWidth);
    }
    
    // U, V
    
    if (y & 1)
      continue;
    
    iUVOffset_new = (y>>1)*iUVWidth;

    if (bDoit == 1)
    {
      if ( (y+dy2) >= 0) 
      {
        iUVOffset_old = iUVOffset_new+(dy2>>1)*iUVWidth;
      }
      else
      {
        iUVOffset_old = iUVOffset_new + (dy1>>1)*iUVWidth;
      }
    }

    if (bDoit == 0) {
        WMVImageMemCpy(pOutU+iUVOffset_new, pIn1U+iUVOffset_new, iUVWidth);
        WMVImageMemCpy(pOutV+iUVOffset_new, pIn1V+iUVOffset_new, iUVWidth);
    } else if (bDoit == 1) {
        WMVImageMemCpy(pOutU+iUVOffset_new, pIn1U+iUVOffset_old, iUVWidth);
        WMVImageMemCpy(pOutV+iUVOffset_new, pIn1V+iUVOffset_old, iUVWidth);
    } else if (bDoit == 2) {
        WMVImageMemCpy(pOutU+iUVOffset_new, pIn2U+iUVOffset_new, iUVWidth);
        WMVImageMemCpy(pOutV+iUVOffset_new, pIn2V+iUVOffset_new, iUVWidth);
    }
  }
}

#else //WMV_C_OPT_WVP2

void SideRoll_Right(I32_WMV     iWidth,     // bitmap width
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
  
  int    Xe;              // end point
  double X0;              // center point
  int    x, y;                // current point
  int    dx1, dx2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new=0, iUVOffset_old=0;
    
  int    bDoit = 0;
  double temp ;
  int i;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  
  Xe = iWidth - 1;
  //Ye = iHeight - 1;

  X0 = (double)(Xe) - T;
  //Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  //iDOffset_new   = iWidth;
  //iDUVOffset_new = iUVWidth;
  
  for (i = 0; i < iWidth; i++) {
    x = i;  y = 0;
    iOffset_new  = i;

    dS_new = ((double)(x) - X0);

    if (i == 313)
      x = x;
    
    if ( dS_new < -R ) {
      bDoit = 0;

    } else if ( dS_new > R ) {
      bDoit = 2;

    } else {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dx1 = (int)( dS_old1 - dS_new + 0.5 );
      dx2 = (int)( dS_old2 - dS_new + 0.5 );

      if ( (x+dx2)< iWidth) {
        bDoit = 1;
        iOffset_old = x + dx2;
        iUVOffset_old = (x>>1) + (dx2>>1);
      } else if ( (x+dx1) < iWidth ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old = x + dx1;
          iUVOffset_old = (x>>1) + (dx1>>1);
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
      while (y<iHeight) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        iOffset_new += iWidth;
        y++;
      }
    } else if (bDoit == 1) {
      while (y<iHeight) {
        pOutY[iOffset_new] = pIn1Y[iOffset_old];
        iOffset_new += iWidth;
        iOffset_old += iWidth;
        y++;
      }
    } else if (bDoit == 2) {
      while (y<iHeight) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        iOffset_new += iWidth;
        y++;
      }
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    x = i>>1; y = 0;
    iUVOffset_new = x;

    if (bDoit == 0) {
      while (y<iUVHeight) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        iUVOffset_new += iUVWidth;
        y++;
      }
    } else if (bDoit == 1) {
      while (y<iUVHeight) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_old];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_old];
        iUVOffset_new += iUVWidth;
        iUVOffset_old += iUVWidth;
        y++;
      }
    } else if (bDoit == 2) {
      while (y<iUVHeight) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        iUVOffset_new += iUVWidth;
        y++;
      }
    }
  }
}


void SideRoll_Left(I32_WMV     iWidth,  // bitmap width
                   I32_WMV     iHeight, // bitmap height
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
  
  int    Xe;              // end point
  double X0;              // center point
  int    x, y;                // current point
  int    dx1, dx2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new=0, iUVOffset_old =0;
    
  int    bDoit = 0;
  double temp ;

  int    iUVWidth  = iWidth >> 1;
  int    iUVHeight = iHeight >> 1;
  int    i;

  Xe = 0;
  //Ye = iHeight - 1;

  X0 = (double)(Xe) + T;
  //Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  //iDOffset_new   = iWidth;
  //iDUVOffset_new = iUVWidth;
  
  for (i = 0; i < iWidth; i++) {
    x = i;  y = 0;
    iOffset_new  = i;

    dS_new = X0 - (double)(x);

    if (i == 313)
      x = x;
    
    if ( dS_new < -R ) {
      bDoit = 0;

    } else if ( dS_new > R ) {
      bDoit = 2;

    } else {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dx1 = -(int)( dS_old1 - dS_new + 0.5 );
      dx2 = -(int)( dS_old2 - dS_new + 0.5 );

      if ( (x+dx2)>=0) {
        bDoit = 1;
        iOffset_old = x + dx2;
        iUVOffset_old = (x>>1) + (dx2>>1);
      } else if ( (x+dx1)>=0 ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old = x + dx1;
          iUVOffset_old = (x>>1) + (dx1>>1);
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
      while (y<iHeight) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        iOffset_new += iWidth;
        y++;
      }
    } else if (bDoit == 1) {
      while (y<iHeight) {
        pOutY[iOffset_new] = pIn1Y[iOffset_old];
        iOffset_new += iWidth;
        iOffset_old += iWidth;
        y++;
      }
    } else if (bDoit == 2) {
      while (y<iHeight) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        iOffset_new += iWidth;
        y++;
      }
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    x = i>>1; y = 0;
    iUVOffset_new = x;

    if (bDoit == 0) {
      while (y<iUVHeight) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        iUVOffset_new += iUVWidth;
        y++;
      }
    } else if (bDoit == 1) {
      while (y<iUVHeight) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_old];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_old];
        iUVOffset_new += iUVWidth;
        iUVOffset_old += iUVWidth;
        y++;
      }
    } else if (bDoit == 2) {
      while (y<iUVHeight) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        iUVOffset_new += iUVWidth;
        y++;
      }
    }
  }
}



void SideRoll_Bottom(I32_WMV     iWidth,     // bitmap width
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
  
  int    Ye;              // end point
  double Y0;              // center point
  int    x, y;                // current point
  int    dy1, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new, iUVOffset_old;
    
  int    bDoit = 0;
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  //int    iUVHeight = iHeight >> 1;
  
  Ye = iHeight - 1;
  //Ye = iHeight - 1;

  Y0 = (double)(Ye) - T;
  //Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  //iDOffset_new   = iWidth;
  //iDUVOffset_new = iUVWidth;
  
  for (i = 0; i < iHeight; i++) {
    x = 0;  y = i;
    iOffset_new = i * iWidth;
    iOffset_old = iOffset_new;
    iUVOffset_new = (i>>1)*iUVWidth;
    iUVOffset_old = iUVOffset_new;
    
    dS_new = ((double)(y) - Y0);

    if (i == 313)
      x = x;
    
    if ( dS_new < -R ) {
      bDoit = 0;

    } else if ( dS_new > R ) {
      bDoit = 2;

    } else {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dy1 = (int)( dS_old1 - dS_new + 0.5 );
      dy2 = (int)( dS_old2 - dS_new + 0.5 );

      if ( (y+dy2)< iHeight) {
        bDoit = 1;
        iOffset_old   += dy2*iWidth;
        iUVOffset_old += (dy2>>1)*iUVWidth;
      } else if ( (y+dy1) < iHeight ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old   += dy1*iWidth;
          iUVOffset_old += (dy1>>1)*iUVWidth;
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
      while (x<iWidth) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        iOffset_new ++;
        x++;
      }
    } else if (bDoit == 1) {
      while (x<iWidth) {
        pOutY[iOffset_new] = pIn1Y[iOffset_old];
        iOffset_new ++;
        iOffset_old ++;
        x++;
      }
    } else if (bDoit == 2) {
      while (x<iWidth) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        iOffset_new ++;
        x++;
      }
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    y = i>>1; x = 0;

    if (bDoit == 0) {
      while (x<iUVWidth) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        iUVOffset_new ++;
        x++;
      }
    } else if (bDoit == 1) {
      while (x<iUVWidth) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_old];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_old];
        iUVOffset_new ++;
        iUVOffset_old ++;
        x++;
      }
    } else if (bDoit == 2) {
      while (x<iUVWidth) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        iUVOffset_new ++;
        x++;
      }
    }
  }
}



void SideRoll_Top(I32_WMV     iWidth,     // bitmap width
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
  
  int    Ye;              // end point
  double Y0;              // center point
  int    x, y;                // current point
  int    dy1, dy2;  // delta x, y for old points
  
  double dS_new;
  double dS_old1, dS_old2;   // S: projection distance, dDS = dS_old-dS_new 

  long   iOffset_new, iOffset_old;
  long   iUVOffset_new, iUVOffset_old;
    
  int    bDoit = 0;
  double temp ;
  int    i;

  int    iUVWidth  = iWidth >> 1;
  //int    iUVHeight = iHeight >> 1;
  
  Ye = 0;
  //Ye = iHeight - 1;

  Y0 = T;
  //Y0 = X0 - (double)(Xe) + (double)(Ye);
  
  //iDOffset_new   = iWidth;
  //iDUVOffset_new = iUVWidth;
  
  for (i = 0; i < iHeight; i++) {
    x = 0;  y = i;
    iOffset_new = i * iWidth;
    iOffset_old = iOffset_new;
    iUVOffset_new = (i>>1)*iUVWidth;
    iUVOffset_old = iUVOffset_new;
    
    dS_new = (Y0 - (double)(y));

    if (i == 313)
      x = x;
    
    if ( dS_new < -R ) {
      bDoit = 0;

    } else if ( dS_new > R ) {
      bDoit = 2;

    } else {
      temp = asin( dS_new / R );
      dS_old1 = R * temp;
      dS_old2 = R * (PI - temp);

      dy1 = -(int)( dS_old1 - dS_new + 0.5 );
      dy2 = -(int)( dS_old2 - dS_new + 0.5 );

      if ( (y+dy2) >= 0) {
        bDoit = 1;
        iOffset_old   += dy2*iWidth;
        iUVOffset_old += (dy2>>1)*iUVWidth;
      } else if ( (y+dy1) >= 0 ) {
        if (dS_new > 0) {
          bDoit = 1;
          iOffset_old   += dy1*iWidth;
          iUVOffset_old += (dy1>>1)*iUVWidth;
        } else
          bDoit = 0;
      } else
        bDoit = 2;
    }

    // Y
    if (bDoit == 0) {
      while (x<iWidth) {
        pOutY[iOffset_new] = pIn1Y[iOffset_new];
        iOffset_new ++;
        x++;
      }
    } else if (bDoit == 1) {
      while (x<iWidth) {
        pOutY[iOffset_new] = pIn1Y[iOffset_old];
        iOffset_new ++;
        iOffset_old ++;
        x++;
      }
    } else if (bDoit == 2) {
      while (x<iWidth) {
        pOutY[iOffset_new] = pIn2Y[iOffset_new];
        iOffset_new ++;
        x++;
      }
    }
    
    // U, V
    
    if (i & 1)
      continue;
    
    y = i>>1; x = 0;

    if (bDoit == 0) {
      while (x<iUVWidth) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_new];
        iUVOffset_new ++;
        x++;
      }
    } else if (bDoit == 1) {
      while (x<iUVWidth) {
        pOutU[iUVOffset_new] = pIn1U[iUVOffset_old];
        pOutV[iUVOffset_new] = pIn1V[iUVOffset_old];
        iUVOffset_new ++;
        iUVOffset_old ++;
        x++;
      }
    } else if (bDoit == 2) {
      while (x<iUVWidth) {
        pOutU[iUVOffset_new] = pIn2U[iUVOffset_new];
        pOutV[iUVOffset_new] = pIn2V[iUVOffset_new];
        iUVOffset_new ++;
        x++;
      }
    }
  }
}

#endif //WMV_C_OPT_WVP2