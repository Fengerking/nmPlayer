#ifndef _DRVDISPLAY_H_
#define _DRVDISPLAY_H_
// drvDispConversion.h : main header file for the drvDisplay DLL
//
 #include <windows.h>
//==============================================================================

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
** DISP2D Bitmap Properties Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_properties_ {
  disp2d_bm_format_enum colorFormat;    // bitmap type  

  UINT8 * pBmPlane0;                    // bitmap plane 0 address 

  UINT8 * pBmPlane1;                    // bitmap plane 1 address 
                                        // (Set to null if unused)

  UINT8 * pBmPlane2;                    // bitmap plane 2 address 
                                        // (Set to null in unused)

  UINT32 pixelWidth;                    // bitmap width in pixels
  
  UINT32 pixelHeight;                   // bitmap height in pixels
  
  UINT32 pitchInBytes;                  // bitmap offset from the last pixel 
                                        // of any row to the beginning pixel 
                                        // of row below it.                                        
} disp2d_bm_properties_type;
 
 
/* -----------------------------------------------------------------------
** DISP2D Bitmap Region Of Interest (ROI) Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_roi_ {
  UINT32 x;     // x-coordinate of the upper left corner of ROI
  UINT32 y;     // y-coordinate of the upper left corner of ROI
  UINT32 dx;    // number of pixels in ROI width
  UINT32 dy;    // number of pixels in ROI height
} disp2d_bm_roi_type;
  
 
/* -----------------------------------------------------------------------
** DISP2D Bitmap Information Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_ {
  disp2d_bm_properties_type bm;
  disp2d_bm_roi_type roi;
} disp2d_bm_type;

/* -----------------------------------------------------------------------
** DISP2D Bitmap Operation Structure
** ----------------------------------------------------------------------- */
typedef UINT32 DISP2D_OP;


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
** DISP2D Bitmap Blt Operation Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_blt_op_ {
    DISP2D_OP bitwiseOp;            // bitwise operations/set to use value

///////////////////////////////////////////////////////////////////////////
//  ALL VALUES BELOW IGNORED UNLESS THE APPROPRIATE OPERATION BIT SET ABOVE
///////////////////////////////////////////////////////////////////////////
    UINT32 transp;                  // transparency color/color keying
    UINT32 alphaPercent;            // alpha blending 0%-100%
    disp2d_rotation_enum rotation;  // rotation and reflection
} disp2d_bm_blt_op_type;


extern "C" disp2d_status_enum ddi_disp2d_blt
(
  disp2d_bm_type *fgBmp, 
    /*  foreground bitmap info */
  disp2d_bm_type *bgBmp, 
    /* background bitmap info to contain the result of fgBmp+bgBmp */
  disp2d_bm_blt_op_type *fgBmpOp,      
    /* foreground bitmap operation */
  BOOL     async
  /* If TRUE indicates that we can pipeline the operation and return 
     immediately after the command is passed to the MDP.
     If FALSE indicates that this function should not return until the
     MDP operation has actually been completed. */
);


/* -----------------------------------------------------------------------
** DISP2D Bitmap Update Operation Structure
** ----------------------------------------------------------------------- */
typedef struct disp2d_bm_update_op_ {
    DISP2D_OP bitwiseOp;            // bitwise operations/set to use value
} disp2d_bm_update_op_type;


#define DISP2D_OP_NOP               0              // No operation.


typedef unsigned int LCD_TYPE;

#define DISPLAY_TYPE_1	0      /* logical first display*/
#define DISPLAY_TYPE_2  1      /* logical second display */
#define DISPLAY_TYPE_3  2      /* logical third display */
#define DISPLAY_TYPE_4  3      /* logical fourth display */
#define MAX_LCD_NUM     4 

#define PRIMARY_LCD_TYPE    DISPLAY_TYPE_1
#define SECONDARY_LCD_TYPE  DISPLAY_TYPE_2

typedef LCD_TYPE disp2d_disp_type;

/*===========================================================================
FUNCTION DDI_DISP2D_UPDATE

DESCRIPTION
  Sends the given update to the MDP to be performed

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern "C" disp2d_status_enum ddi_disp2d_update
(
  disp2d_bm_type *outBmp, 
    /* output bitmap image to be updated to the destination */
  disp2d_bm_update_op_type *outBmpOp,      
    /* output bitmap operation */
  disp2d_disp_type dest,
    /* output destination */
  UINT32 startX,
    /* starting x location on destination */
  UINT32 startY,       
    /* starting y location on destination */
  BOOL     async
  /* If TRUE indicates that we can pipeline the operation and return 
     immediately after the command is passed to the MDP.
     If FALSE indicates that this function should not return until the
     MDP operation has actually been completed. */
);


#endif