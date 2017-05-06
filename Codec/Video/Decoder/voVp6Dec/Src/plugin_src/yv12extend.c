/****************************************************************************
 *        
 *   Module Title : yv12extend.c   
 *
 *   Despcription  : functions for extending frame buffer into motion 
 *                  border 
 *
 *   Copyright (c) 1999 - 2005  On2 Technologies Inc. All Rights Reserved.
 *
 ***************************************************************************/

/****************************************************************************
*  Header Files
****************************************************************************/
#include "yv12config.h"
#include "on2_mem.h"

/****************************************************************************
*  Exports
****************************************************************************/

/****************************************************************************
 *
 ****************************************************************************/
void 
On2YV12_ExtendFrameBorders (YV12_BUFFER_CONFIG *ybf)
{
	int i;
	unsigned char *SrcPtr1, *SrcPtr2;
	unsigned char *DestPtr1, *DestPtr2;

    unsigned int Border;
	int PlaneStride;
	int PlaneHeight;
	int PlaneWidth;

    /***********/
    /* Y Plane */
    /***********/
    Border = ybf->border;
	PlaneStride = ybf->YStride;
	PlaneHeight = ybf->YHeight;
	PlaneWidth = ybf->YWidth;

    // copy the left and right most columns out 
	SrcPtr1 = ybf->YBuffer;
	SrcPtr2 = SrcPtr1 + PlaneWidth - 1;
	DestPtr1= SrcPtr1 - Border;
	DestPtr2= SrcPtr2 + 1;

    for ( i=0; i<PlaneHeight; i++ )
    {
        on2_memset ( DestPtr1, SrcPtr1[0], Border );
        on2_memset ( DestPtr2, SrcPtr2[0], Border );
        SrcPtr1  += PlaneStride;
        SrcPtr2  += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    // Now copy the top and bottom source lines into each line of the respective borders
	SrcPtr1 = ybf->YBuffer - Border;
	SrcPtr2 = SrcPtr1 + (PlaneHeight * PlaneStride) - PlaneStride;
	DestPtr1= SrcPtr1 - (Border * PlaneStride);
	DestPtr2= SrcPtr2 + PlaneStride;

    for ( i=0; i<(int)Border; i++ )
    {
        on2_memcpy ( DestPtr1, SrcPtr1, PlaneStride );
        on2_memcpy ( DestPtr2, SrcPtr2, PlaneStride );
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

	PlaneStride /= 2;
	PlaneHeight /= 2;
    PlaneWidth /= 2;
    Border /= 2;

    /***********/
    /* U Plane */
    /***********/

    // copy the left and right most columns out 
	SrcPtr1 = ybf->UBuffer;
	SrcPtr2 = SrcPtr1 + PlaneWidth - 1;
	DestPtr1= SrcPtr1 - Border;
	DestPtr2= SrcPtr2 + 1;

    for ( i=0; i<PlaneHeight; i++ )
    {
        on2_memset ( DestPtr1, SrcPtr1[0], Border );
        on2_memset ( DestPtr2, SrcPtr2[0], Border );
        SrcPtr1  += PlaneStride;
        SrcPtr2  += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    // Now copy the top and bottom source lines into each line of the respective borders
	SrcPtr1 = ybf->UBuffer - Border;
	SrcPtr2 = SrcPtr1 + (PlaneHeight * PlaneStride) - PlaneStride;
	DestPtr1= SrcPtr1 - (Border * PlaneStride);
	DestPtr2= SrcPtr2 + PlaneStride;

    for ( i=0; i<(int)(Border); i++ )
    {
        on2_memcpy ( DestPtr1, SrcPtr1, PlaneStride );
        on2_memcpy ( DestPtr2, SrcPtr2, PlaneStride );
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    /***********/
    /* V Plane */
    /***********/
    
    // copy the left and right most columns out 
	SrcPtr1 = ybf->VBuffer;
	SrcPtr2 = SrcPtr1 + PlaneWidth - 1;
	DestPtr1= SrcPtr1 - Border;
	DestPtr2= SrcPtr2 + 1;

    for ( i=0; i<PlaneHeight; i++ )
    {
        on2_memset ( DestPtr1, SrcPtr1[0], Border );
        on2_memset ( DestPtr2, SrcPtr2[0], Border );
        SrcPtr1  += PlaneStride;
        SrcPtr2  += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    // Now copy the top and bottom source lines into each line of the respective borders
	SrcPtr1 = ybf->VBuffer - Border;
	SrcPtr2 = SrcPtr1 + (PlaneHeight * PlaneStride) - PlaneStride;
	DestPtr1= SrcPtr1 - (Border * PlaneStride);
	DestPtr2= SrcPtr2 + PlaneStride;

    for ( i=0; i<(int)(Border); i++ )
    {
        on2_memcpy ( DestPtr1, SrcPtr1, PlaneStride );
        on2_memcpy ( DestPtr2, SrcPtr2, PlaneStride );
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }
}


void 
On2YV12_ExtendFrameBordersYOnly (YV12_BUFFER_CONFIG *ybf)
{
	int i;
	unsigned char *SrcPtr1, *SrcPtr2;
	unsigned char *DestPtr1, *DestPtr2;

    unsigned int Border;
	int PlaneStride;
	int PlaneHeight;
	int PlaneWidth;

    /***********/
    /* Y Plane */
    /***********/
    Border = ybf->border;
	PlaneStride = ybf->YStride;
	PlaneHeight = ybf->YHeight;
	PlaneWidth = ybf->YWidth;

    // copy the left and right most columns out 
	SrcPtr1 = ybf->YBuffer;
	SrcPtr2 = SrcPtr1 + PlaneWidth - 1;
	DestPtr1= SrcPtr1 - Border;
	DestPtr2= SrcPtr2 + 1;

    for ( i=0; i<PlaneHeight; i++ )
    {
        on2_memset ( DestPtr1, SrcPtr1[0], Border );
        on2_memset ( DestPtr2, SrcPtr2[0], Border );
        SrcPtr1  += PlaneStride;
        SrcPtr2  += PlaneStride;
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

    // Now copy the top and bottom source lines into each line of the respective borders
	SrcPtr1 = ybf->YBuffer - Border;
	SrcPtr2 = SrcPtr1 + (PlaneHeight * PlaneStride) - PlaneStride;
	DestPtr1= SrcPtr1 - (Border * PlaneStride);
	DestPtr2= SrcPtr2 + PlaneStride;

    for ( i=0; i<(int)Border; i++ )
    {
        on2_memcpy ( DestPtr1, SrcPtr1, PlaneStride );
        on2_memcpy ( DestPtr2, SrcPtr2, PlaneStride );
        DestPtr1 += PlaneStride;
        DestPtr2 += PlaneStride;
    }

	PlaneStride /= 2;
	PlaneHeight /= 2;
    PlaneWidth /= 2;
    Border /= 2;

}