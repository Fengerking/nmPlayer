#ifndef DISP2D_H
#define DISP2D_H
/*============================================================================

                           DISP2D API

  DESCRIPTION
  This file contains Display 2D API definitions

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/services/disp/disp2d/disp2d.h#10 $ $DateTime: 2008/04/21 19:47:02 $ $Author: axu $


when      who    what, where, why
--------  -----  ----------------------------------------------------------
04/21/08  axu	 Add Stub Interface for Cursor
01/14/08  trg    Add interface for ATI driver to access MDP registers.
11/08/07  spw    Add interface for disp2d_init() function
10/13/07  trg    Add tv out function and set fatal error mode to remove
                 MDP dependancy from WM.
08/20/07  spw    Add support for nondestructive blt
11/03/05  cjr    Initial code
============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

#include "comdef.h"
#include "disp.h"  // LCD_TYPE dependency

/* -----------------------------------------------------------------------
** DISP2D Bitmap Format Types
** ----------------------------------------------------------------------- */
typedef enum disp2d_bm_format_ {
   DISP2D_RGB_565,      // RGB 565 planar 
   DISP2D_XRGB_8888,    // XRGB 888 planar (X ="don't care" or alhpha value)
   DISP2D_RGB_888,      // RGB 888 planar   
   DISP2D_Y_CBCR_H2V2,  // Y and CbCr, pseudo planar w/ Cb is in MSB   
   DISP2D_Y_CRCB_H2V2,  // Y and CrCb, pseudo planar w/ Cr is in MSB
   DISP2D_Y_CRCB_H2V1,  // Y and CrCb, pseduo planar w/ Cr is in MSB
   DISP2D_Y_CBCR_H2V1,  // Y and CrCb, pseduo planar w/ Cr is in MSB
   DISP2D_YCRYCB_H2V1,  // YCrYCb interleave     
   DISP2D_COLOR_MAX     // Maximum number of color formats supported 
} disp2d_bm_format_enum;


/* -----------------------------------------------------------------------
** DISP2D Bitmap Rotations
** ----------------------------------------------------------------------- */
typedef enum disp2d_rotation_ {
  DISP2D_NOROT,         // Do not rotate
  DISP2D_ROT90,         // Rotate clockwise 90 degrees
  DISP2D_ROT180,        // Rotate clockwise 180 degrees
  DISP2D_ROT270,        // Rotate clockwise 270 degrees
  DISP2D_NOROT_XREF,    // Rotate 0 degrees, reflect about x axis
  DISP2D_NOROT_YREF,    // Rotate 0 degrees, reflect about y axis
  DISP2D_ROT90_XREF,    // Rotate 90 degrees, reflect about x axis
  DISP2D_ROT90_YREF,    // Rotate 90 degrees, reflect about y axis
  DISP2D_ROT180_XREF,   // Rotate 180 degrees, reflect about x axis
  DISP2D_ROT180_YREF,   // Rotate 180 degrees, reflect about y axis
  DISP2D_ROT270_XREF,   // Rotate 270 degrees, reflect about x axis
  DISP2D_ROT270_YREF,   // Rotate 270 degrees, reflect about y axis
  DISP2D_ROT_MAX        // Maximum number of rotating modes
} disp2d_rotation_enum;

/* -----------------------------------------------------------------------
** DISP2D Status
** ----------------------------------------------------------------------- */
typedef enum disp2d_status_ {
  DISP2D_SUCCESSFUL,
  DISP2D_ERROR_INVALID_USER_PARAM,  
  DISP2D_ERROR_HW_NOT_READY,   
  DISP2D_ERROR_OPERATION_NOT_SUPPORTED,  
  DISP2D_ERROR_HW_OPERATION_FAILURE,
  DISP2D_ERROR_HW_TIME_OUT,
  DISP2D_ERROR_OUT_OF_MEMORY,
  DISP2D_ERROR_SYSTEM_CRASH
} disp2d_status_enum;


/* -----------------------------------------------------------------------
** DISP2D Bitmap Operation Bits
**
**   NOP - No operation, do nothing.
**
**   ROT - Apply rotation and/or reflection based on the
**         disp2d_rotation_enum passed.  
**
**   CONST_ALPHA - Apply the constant alpha to the ROI specified.  This
**                 operation will be ignored for per pixel alpha color
**                 formats.
**
**   TRANSP - Apply transparency/color keying to the ROI for a single
**            pixel color defined in XRGB_8888.  This operation will be
**            ignored for non-RGB color formats as well as for per pixel
**            alpha color formats.
**
**   VSYNC_OFF - Update the display WITHOUT vertical syncing control.
**               This will be faster but may cause temporary visual
**               artifacts.  This operation is ignored for BLT calls.
**
** ----------------------------------------------------------------------- */
#define DISP2D_BIT(x)               (1<<(x))
#define DISP2D_OP_NOP               0              // No operation.

/* -----------------------------------------------------------------------
** DISP2D blt(blending) operation to be used by disp2d_bm_blt_op_type only
** ----------------------------------------------------------------------- */
#define DISP2D_BLT_OP_ROT_REFLECT       DISP2D_BIT(0)  // Rotate and reflect.
#define DISP2D_BLT_OP_CONST_FRAME_ALPHA DISP2D_BIT(1)  // Constant alpha.
#define DISP2D_BLT_OP_PER_PIXEL_ALPHA   DISP2D_BIT(2)  // Per Pixel alpha.
#define DISP2D_BLT_OP_TRANSP            DISP2D_BIT(3)  // Transparency.

/* -----------------------------------------------------------------------
** DISP2D Update peration to be used by disp2d_bm_update_op_type only
** ----------------------------------------------------------------------- */
#define DISP2D_UPDATE_OP_VSYNC_OFF      DISP2D_BIT(0)  // No display vertical sync.

/* -----------------------------------------------------------------------
** DISP2D Bitmap Operation Structure
** ----------------------------------------------------------------------- */
typedef uint32 DISP2D_OP;

/* -----------------------------------------------------------------------
** DISP2D Bitmap Blt Operation Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_blt_op_ {
    DISP2D_OP bitwiseOp;            // bitwise operations/set to use value

///////////////////////////////////////////////////////////////////////////
//  ALL VALUES BELOW IGNORED UNLESS THE APPROPRIATE OPERATION BIT SET ABOVE
///////////////////////////////////////////////////////////////////////////
    uint32 transp;                  // transparency color/color keying
    uint32 alphaPercent;            // alpha blending 0%-100%
    disp2d_rotation_enum rotation;  // rotation and reflection
} disp2d_bm_blt_op_type;

/* -----------------------------------------------------------------------
** DISP2D Bitmap Update Operation Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_update_op_ {
    DISP2D_OP bitwiseOp;            // bitwise operations/set to use value
} disp2d_bm_update_op_type;


/* -----------------------------------------------------------------------
** DISP2D Bitmap Properties Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_properties_ {
  disp2d_bm_format_enum colorFormat;    // bitmap type  

  uint8 * pBmPlane0;                    // bitmap plane 0 address 

  uint8 * pBmPlane1;                    // bitmap plane 1 address 
                                        // (Set to null if unused)

  uint8 * pBmPlane2;                    // bitmap plane 2 address 
                                        // (Set to null in unused)

  uint32 pixelWidth;                    // bitmap width in pixels
  
  uint32 pixelHeight;                   // bitmap height in pixels
  
  uint32 pitchInBytes;                  // bitmap offset from the last pixel 
                                        // of any row to the beginning pixel 
                                        // of row below it.                                        
} disp2d_bm_properties_type;
 
 
/* -----------------------------------------------------------------------
** DISP2D Bitmap Region Of Interest (ROI) Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_roi_ {
  uint32 x;     // x-coordinate of the upper left corner of ROI
  uint32 y;     // y-coordinate of the upper left corner of ROI
  uint32 dx;    // number of pixels in ROI width
  uint32 dy;    // number of pixels in ROI height
} disp2d_bm_roi_type;
  
 
/* -----------------------------------------------------------------------
** DISP2D Bitmap Information Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_ {
  disp2d_bm_properties_type bm;
  disp2d_bm_roi_type roi;
} disp2d_bm_type;

/* -----------------------------------------------------------------------
** DISP2D Callback Structure
** ----------------------------------------------------------------------- */
typedef struct
{  
  void (*cb_func) (void *, disp2d_status_enum); // user callback function ptr
  void * arg;                                   // user data pointer
} disp2d_cb_type;


typedef LCD_TYPE disp2d_disp_type;

/* -----------------------------------------------------------------------
** TVout Support
** ----------------------------------------------------------------------- */
typedef struct
{
  int32 x1;         /* starting column */
  int32 x2;         /* ending column */
  int32 y1;         /* starting row */
  int32 y2;         /* ending row */
} DISP2D_TV_CWIN;

typedef struct
{ 
  DISP2D_TV_CWIN        cwin;         /* display window */
  uint32                rotOp;        /* supports flip and rotation only */
} DISP2D_MAP_INFO;

/* -----------------------------------------------------------------------
** ATI Graphics Core Support
** ----------------------------------------------------------------------- */

/* Interface to be used by the ATI graphics core only! */
typedef enum
{ 
  DISP2D_GRP_BUFFER_COMPLETE   /* MDP Bypass Word 43. */
} disp2d_register_type;

void   disp2d_writeRegister(disp2d_register_type reg, uint32 val);
uint32 disp2d_readRegister (disp2d_register_type reg);

/* -----------------------------------------------------------------------
** DISP2D Inititialization Modes
** ----------------------------------------------------------------------- */
typedef enum disp2d_init_mode_ {
   DISP2D_INIT_MODE_DEFAULT,          /* Normal Operational mode       */
   DISP2D_INIT_MODE_UPDATELOADER      /* Initialization for uldrui.dll */
} disp2d_init_mode;

disp2d_status_enum disp2d_map_dest(LCD_TYPE         src,
                                   LCD_TYPE         dst, 
                                   DISP2D_MAP_INFO *mapInfo);

void disp2d_set_fatal_err(void);

//////////////////////////////////////////////////////////////////////////////
// new API - disp2d_blt(), disp2d_dma_update()
//////////////////////////////////////////////////////////////////////////////
 
disp2d_status_enum disp2d_blt(disp2d_bm_type * fgBmp, 
                   disp2d_bm_type * bgBmp,       
                   disp2d_bm_blt_op_type * fgBmpOp,      
                   disp2d_cb_type * cbPtr);      
                                                 
disp2d_status_enum disp2d_update(disp2d_bm_type * outBmp, 
                   disp2d_bm_update_op_type * outBmpOp,      
                   disp2d_disp_type dest,
                   uint32 startX,
                   uint32 startY,       
                   disp2d_cb_type * cbPtr);                                                    

#ifdef FEATURE_WINCE
disp2d_status_enum disp2d_blt_nondestruct(disp2d_bm_type * fgBmp, 
                   disp2d_bm_type * bgBmp,       
                   disp2d_bm_blt_op_type * fgBmpOp,      
                   disp2d_cb_type * cbPtr,
                   disp2d_bm_properties_type *finalBmp);   
#endif /* FEATURE_WINCE */

disp2d_status_enum disp2d_init(disp2d_init_mode mode); 

disp2d_status_enum disp2d_SetCursorOverlay(disp2d_bm_type * pCursorBmp);
disp2d_status_enum disp2d_SetCursorPos(int32 x, int32 y);


/*============================================================================
   DISP2D API DOCUMENTATION
==============================================================================

FUNCITON NAME: disp2d_blt()

DESCRIPTION: This function is to blend the foreground bitmap on top of 
             the background bitmap and store the result into the output bitmap
	
PARAMETERS:	
  disp2d_bm_type * fgBmp    
    foreground bitmap info

  disp2d_bm_type * bgBmp    
    background bitmap info to contain the result of fgBmp+bgBmp

  disp2d_bm_blt_op_type fgBmpOp
    foreground bitmap operation

  disp2d_cb_type * cbPtr    
   callback pointer for callback notification.   
   
   If it equals to NULL, API will operate in synchronous mode and blocks the 
   calling task until it finishes the job.  Otherwise, API will operate in 
   asynchronous mode and returns to the calling task immediately.  
   
   For asynchronous mode, callback notifcation will get executed in display 
   task context.

EXAMPLE:

  // RGB888 QVGA (240x320) fg_img[] blits into out_img[] as scaled+rotate90 
  // RGB565 VGA (640x480)
  
  disp2d_bm_type fgBmp;
  disp2d_bm_type bgBmp;  
  disp2d_bm_blt_op_type fgBmpOp;
  
  fgBmp.bm.colorFormat = DISP2D_RGB_888;
  fgBmp.bm.pBmPlane0 = (uint8 *)&fg_img[0];  
  fgBmp.bm.pixelWidth = 240; 
  fgBmp.bm.pixelHeight = 320; 
  fgBmp.bm.pitchInBytes = 0;       
  fgBmp.roi.x = 0;
  fgBmp.roi.dx = 240;
  fgBmp.roi.y = 0;
  fgBmp.roi.dy = 320;  
  
  bgBmp.bm.colorFormat = DISP2D_RGB_565;
  bgBmp.bm.pBmPlane0 = (uint8 *)out_img[0];  
  bgBmp.bm.pixelWidth = 640; 
  bgBmp.bm.pixelHeight = 480;   
  bgBmp.bm.pitchInBytes = 0;     
  bgBmp.roi.x = 0;
  bgBmp.roi.dx = 640;
  bgBmp.roi.y = 0;
  bgBmp.roi.dy = 480;

  fgBmpOp.bitwiseOp = DISP2D_BLT_OP_ROT_REFLECT;
  fgBmpOp.rotation = DISP2D_ROT90;
  
  disp2d_blt(&fgBmp, &bgBmp, &fgBmpOp, NULL);


RETURN VALUE:
  disp2d_status_enum

==============================================================================
==============================================================================
FUNCITON NAME: disp2d_update()

DESCRIPTION: This function is to update the output bitmap to the destination
	
PARAMETERS:	
  disp2d_bm_type * outBmp
    output bitmap image to be updated to the destination

  disp2d_bm_update_op_type outBmpOp
    output bitmap operation

  disp2d_disp_type dest
    output destination defined as LCD_TYPE in disp.h

  uint32 startX
    starting x location
    
  uint32 startY
    starting y location

  disp2d_cb_type * cbPtr    
   callback pointer for callback notification.   
   
   If it equals to NULL, API will operate in synchronous mode and blocks the 
   calling task until it finishes the job.  Otherwise, API will operate in 
   asynchronous mode and returns to the calling task immediately.  
   
   For asynchronous mode, callback notifcation will get executed in display 
   task context.

EXAMPLE:
  // RGB565 VGA (640x480) out_img[] update to the primary display target

  disp2d_bm_type outBmp;  
  disp2d_bm_blt_op_type outBmpOp;
  
  outBmp.bm.colorFormat = DISP2D_RGB_565;
  outBmp.bm.pBmPlane0 = (uint8 *)out_img[0];  
  outBmp.bm.pixelWidth = 640; 
  outBmp.bm.pixelHeight = 480;   
  outBmp.bm.pitchInBytes = 0;     
  outBmp.roi.x = 0;
  outBmp.roi.dx = 640;
  outBmp.roi.y = 0;
  outBmp.roi.dy = 480;
  
  outBmpOp.bitwiseOp = DISP2D_OP_NOP;
  
  disp2d_update(&outBmp, &outBmpOp, DISPLAY_TYPE_1,NULL);


RETURN VALUE:
  disp2d_status_enum

==============================================================================
==============================================================================

FUNCITON NAME: disp2d_blt_nondestruct()

DESCRIPTION: This function is to blend the foreground bitmap along with 
             the background bitmap and store the result a separate output
			 bitmap buffer (ie. nondestructive to first two surfaces).
	
PARAMETERS:	
  disp2d_bm_type * fgBmp    
    foreground bitmap info

  disp2d_bm_type * bgBmp    
    background bitmap info to contain the result of fgBmp+bgBmp

  disp2d_bm_blt_op_type fgBmpOp
    foreground bitmap operation

  disp2d_cb_type * cbPtr    
   callback pointer for callback notification.   
   
   If it equals to NULL, API will operate in synchronous mode and blocks the 
   calling task until it finishes the job.  Otherwise, API will operate in 
   asynchronous mode and returns to the calling task immediately.  
   
   For asynchronous mode, callback notifcation will get executed in display 
   task context.

  disp2d_bm_properties_type * finalBmp
   output bitmap buffer information

   The finalBmp struct specifies where to put the output.  This is specified
   in the finalBmp->pBmPlane0 pointer.    Note that there are other fields in
   the finalBmp struct and currently we have the requirement that they must
   be the same as what is passed in on the bgBmp pointer.

   For example, we must have:
     finalBmp->colorFormat == bgBmp->bm.colorFormat  &&
     finalBmp->pixelWidth == bgBmp->bm.pixelWidth &&
	 finalBmp->pixelHeight != bgBmp->bm.pixelHeight) &&
     finalBmp->pitchInBytes != bgBmp->bm.pitchInBytes

	 This restriction may be removed in the future if we find a need to have
	 these values different.
     

EXAMPLE:

  // RGB888 QVGA (240x320) fg_img[] blits with bg_img[] into outbuf_addr[].
  //                       Scaled and rotated90.  fg and bg buffers preserved.
  // RGB565 VGA (640x480) is size of the output image.
  
  disp2d_bm_type fgBmp;
  disp2d_bm_type bgBmp;  
  disp2d_bm_blt_op_type fgBmpOp;
  disp2d_bm_properties outBmp;
  
  fgBmp.bm.colorFormat = DISP2D_RGB_888;
  fgBmp.bm.pBmPlane0 = (uint8 *)&fg_img[0];  
  fgBmp.bm.pixelWidth = 240; 
  fgBmp.bm.pixelHeight = 320; 
  fgBmp.bm.pitchInBytes = 0;       
  fgBmp.roi.x = 0;
  fgBmp.roi.dx = 240;
  fgBmp.roi.y = 0;
  fgBmp.roi.dy = 320;  
  
  bgBmp.bm.colorFormat = DISP2D_RGB_565;
  bgBmp.bm.pBmPlane0 = (uint8 *)&bg_img[0];  
  bgBmp.bm.pixelWidth = 640; 
  bgBmp.bm.pixelHeight = 480;   
  bgBmp.bm.pitchInBytes = 0;     
  bgBmp.roi.x = 0;
  bgBmp.roi.dx = 640;
  bgBmp.roi.y = 0;
  bgBmp.roi.dy = 480;

  outBmp.colorFormat = bgBmp.bm.colorFormat;
  outBmp.pBmPlane0 = (uint*) &outbuf_addr[0];
  outBmp.pixelWidth = bgBmp.bm.pixelWidth;
  outBmp.pixelHeigth = bgBmp.bm.pixelHeight;
  outBmp.pitchInBytes = bgBmp.bm.pitchInBytes;

  fgBmpOp.bitwiseOp = DISP2D_BLT_OP_ROT_REFLECT;
  fgBmpOp.rotation = DISP2D_ROT90;
  
  disp2d_blt_nondestruct(&fgBmp, &bgBmp, &fgBmpOp, NULL, &outBmp);


RETURN VALUE:
  disp2d_status_enum

==============================================================================
==============================================================================
FUNCTION NAME: disp2d_init()

DESCRIPTION: This function is to initialize the disp2d interface
	
PARAMETERS:	
disp2d_status_enum disp2d_init(disp2d_init_mode mode); 
  disp2d_init_mode mode
    which mode to initialize the disp2d interface for (normal or uldr)
   
   If the mode to be initialized is normal mode, then the disp2d_init routine
   will setup the mddi clock rate to be fastest rate supported for this lcd.

RETURN VALUE:
  disp2d_status_enum

==============================================================================
============================================================================*/
 
#endif /* DISP2D_H */



