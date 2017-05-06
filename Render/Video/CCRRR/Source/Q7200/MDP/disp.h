#ifndef DISP_H
#define DISP_H


/*============================================================================

                   Display Logical Device Driver Header File

   DESCRIPTION
     This file contains the definitions needed for the display logical
     device driver (LDD).

   Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $PVCSPath:  L:/src/asw/MSM6050/vcs/disp.h_v   1.3   10 Apr 2002 11:50:14   dlee  $
 $Header: //depot/asic/msm7500/drivers/lcd/disp.h#18 $ $DateTime: 2007/06/26 19:47:50 $ $Author: davidwon $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 06/26/07  dw     Added support for display updates with powered-down lcd
 04/17/07  cjr    Moved TVOut defines to TVEnc driver 
 01/23/07  cl     Added support for display under error fatal
 12/15/06  dw     Added support to get/lock/unlock backlight levels 
 08/14/06  tb     Merge in WINCE changes.
 04/29/05  cjr    Added LCD auto-detect support 
 11/08/04   tb    Changes to support multple MDDI clients.
 10/29/04  trc    Bump up EFFA backlight brightness levels
 08/13/04  trc    Added support for EFFA
 06/07/04  ap     Added FFA support.
 05/21/04  cjr    Added MDP/MDDI support
 05/02/04  cjr    Added dual LCD support
 04/22/04  cjr    Added drvLib support
 07/16/03  dle    Added disp_capture16 and disp_capture32.
 06/23/03  dle    Added support for DISP_DEVICE_18BPP.
 02/27/03  rtp    Added disp_off() and disp_on() functions to turn off
                  and turn on display respectively
 10/24/02  dle    Made disp_crit_sect global so can be shared by GRP
                  device driver.
 07/23/02  dle    Replaced disp_copy with disp_copy16, disp_copy32
 07/16/02  dle    Added disp_copy().
 04/25/02  dle    Added memset16.
 04/10/00  dle    Added _clear_screen() and _clear_screen_area().
 03/15/02  lz     Removed gs.h from list of includes and any references to
                  it in the code.
 03/12/02  dl     Initial release.

============================================================================*/

//============================================================================
//                        INCLUDE FILES
//============================================================================

#include "comdef.h"

//? Jason #include "target.h"
//? Jason #include "customer.h"
//? Jason #include "disp_options.h"
//? Jason #include "rex.h"
//? Jason #include "msm.h"

typedef unsigned char byte;
typedef unsigned short word;

#ifdef FEATURE_WINCE
  #include "bsp_base_reg_cfg.h"
#endif // FEATURE_WINCE

//============================================================================
//                        LCD TYPE DEFINITIONS
//============================================================================

// physical lcd type
typedef enum
{
  TMD_QVGA,              /* EBI2 TMD QVGA LCD */
  EPSON_QCIF,            /* EBI2 EPSON QCIF LCD */
  MDDI_LCD_PRIM,         /* MDDI Prim LCD */
  MDDI_LCD_SECD,         /* MDDI Secd LCD */
  EXT_MDDI_LCD_TYPE,     /* external mddi lcd type */
  TVOUT_TYPE,            /* TVOut display */ 
  MEM_WR,                /* WR Memory */
  LCD_NONE = 0xFFFF      /* No LCD */
}LCD_TYPE_PHYS;

//============================================================================
//                        CONSTANT DEFINITIONS
//============================================================================

// logical lcd type
//typedef enum
//{
//  DISPLAY_TYPE_1,      /* logical prim/main LCD */
//  DISPLAY_TYPE_2,     /* logical secd LCD */
//  DISPLAY_TYPE_3,      /* logical secd LCD */
//  DISPLAY_TYPE_4,     /* logical secd LCD */
//  MAX_LCD_NUM    
//}LCD_TYPE;

typedef unsigned int LCD_TYPE;

#define DISPLAY_TYPE_1	0      /* logical first display*/
#define DISPLAY_TYPE_2  1      /* logical second display */
#define DISPLAY_TYPE_3  2      /* logical third display */
#define DISPLAY_TYPE_4  3      /* logical fourth display */
#define MAX_LCD_NUM     4 

#define PRIMARY_LCD_TYPE    DISPLAY_TYPE_1
#define SECONDARY_LCD_TYPE  DISPLAY_TYPE_2

#define MDDI_MC4_PRIM MDDI_LCD_PRIM
#define MDDI_MC4_SECD MDDI_LCD_SECD

#define LCD_BACKLIGHT_LVL_0 PM_LED_LCD_SETTING__LEVEL0
#define LCD_BACKLIGHT_LVL_1 PM_LED_LCD_SETTING__LEVEL1
#define LCD_BACKLIGHT_LVL_2 PM_LED_LCD_SETTING__LEVEL2
#define LCD_BACKLIGHT_LVL_3 PM_LED_LCD_SETTING__LEVEL3
#define LCD_BACKLIGHT_LVL_4 PM_LED_LCD_SETTING__LEVEL4

/***********************************************/
/* Primarny/Secondary LCD definition           */
/***********************************************/

#define PRIMARY_LCD_NAME  "/dev/lcd01"
#define SECONDARY_LCD_NAME  "/dev/lcd02"
#define MDP_DEV_NAME    "/dev/mdp"

#define MDDI_PRIM_LCD_DIMENSION_WIDTH       175      /* 1.75 inch */
#define MDDI_PRIM_LCD_DIMENSION_HEIGHT      192     /* 1.92 inch */
#define MDDI_SECD_LCD_DIMENSION_WIDTH       88
#define MDDI_SECD_LCD_DIMENSION_HEIGHT      63

#define TMD_QVGA_LCD_DIMENSION_WIDTH        120
#define TMD_QVGA_LCD_DIMENSION_HEIGHT       160

#define EPSON_QCIF_LCD_DIMENSION_WIDTH      137
#define EPSON_QCIF_LCD_DIMENSION_HEIGHT     172

/***********************************************/
/* External MDDI definition                    */
/***********************************************/
#define EXT_MDDI_WIDTH    800
#define EXT_MDDI_HEIGHT   600

//============================================================================
//                        DATA DECLARATIONS
//============================================================================

#define LCD_API_EXTENSION_01

/*--- Type Definitions --*/

/* Definition of bits per pixel */
typedef enum
{
  DISP_8BPP,
  DISP_16BPP,
  DISP_18BPP
} disp_bpp_type;

// LCD API Extension return type
typedef enum
{
  LCD_FAIL = 0,
  LCD_SUCCESSFUL
}LCD_STATUS;

typedef struct
{
  word                    disp_width;
  word                    disp_height;
  disp_bpp_type           bpp;
  boolean                 palette_support;
  boolean                 contrast_support;
  byte                    contrast_min;
  byte                    contrast_max;
  byte                    contrast_default;
  boolean                 backlight_support;
  byte                    backlight_min;
  byte                    backlight_max;
  byte                    backlight_default;

  LCD_TYPE_PHYS           lcd_type;
  word                    phys_width;
  word                    phys_height;   
}  disp_info_type;


enum
{
  IOCTL_DISP_GET_INFO,
  IOCTL_DISP_POWERUP,
  IOCTL_DISP_UPDATE,
  IOCTL_DISP_UPDATE_UNDER_ERR_FATAL,
  IOCTL_DISP_UPDATE_WITH_LCD_OFF,
  IOCTL_DISP_SET_CONTRAST,
  IOCTL_DISP_SET_BACKLIGHT,
  IOCTL_DISP_GET_BACKLIGHT,
  IOCTL_DISP_CLEAR_WHOLE_SCREEN,
  IOCTL_DISP_CLEAR_SCREEN_AREA,
  IOCTL_DISP_CAPTURE_SCREEN,
  IOCTL_DISP_OFF,
  IOCTL_DISP_ON
};

typedef struct
{
  void *buf_ptr;
  int16 src_width;
  int16 src_starting_row;
  int16 src_starting_column;
  int16 num_of_rows;
  int16 num_of_columns;
  int16 dst_starting_row;
  int16 dst_starting_column;
  LCD_TYPE dest;
} disp_update_type;


typedef struct
{
  word start_row;
  word end_row;
  word start_column;
  word end_column;
  LCD_TYPE dest;
} disp_cls_type;


extern rex_crit_sect_type disp_crit_sect;

//============================================================================
//                        MACRO DEFINITIONS
//============================================================================

//============================================================================
//                        EXPORTED FUNCTIONS
//============================================================================

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION      DISP_GET_INFO

DESCRIPTION
              This function returns display information

DEPENDENCIES
  None

RETURN VALUE
  Display information

SIDE EFFECTS
  None

===========================================================================*/
  disp_info_type disp_get_info_ex(LCD_TYPE);

/*===========================================================================

FUNCTION      DISP_OFF

DESCRIPTION
              This function turns off the display.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it's successful, otherwise returns FALSE

SIDE EFFECTS
  None

===========================================================================*/
  LCD_STATUS disp_off_ex(LCD_TYPE);

/*===========================================================================

FUNCTION      DISP_ON

DESCRIPTION
              This function turns on the display.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if it's successful, otherwise returns FALSE  

SIDE EFFECTS
  None

===========================================================================*/
  LCD_STATUS disp_on_ex(LCD_TYPE);


/*===========================================================================

FUNCTION      DISP_INIT

DESCRIPTION
              This function initialize the display panel

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_init(void);
  void disp_init2(void);

#ifndef IMAGE_MODEM_PROC     
  boolean disp_is_active(void);
#endif

/*===========================================================================

FUNCTION      DISP_GET_INFO

DESCRIPTION
              This function returns display information

DEPENDENCIES
  None

RETURN VALUE
  Display information

SIDE EFFECTS
  None

===========================================================================*/

  disp_info_type disp_get_info(void);
  disp_info_type disp_get_info2(void);

/*===========================================================================

FUNCTION      DISP_POWERUP

DESCRIPTION
              This function powers up the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/

  void disp_powerup(void);
  void disp_powerup2(void);

/*===========================================================================

FUNCTION      DISP_POWERDOWN

DESCRIPTION
              This function powers down the device.

DEPENDENCIES
  None

RETURN VALUE
  Display buffer address

SIDE EFFECTS
  None

===========================================================================*/

  void disp_powerdown(void);
  void disp_powerdown2(void);


/*===========================================================================

FUNCTION      DISP_UPDATE

DESCRIPTION
              This function update the display screen from the off screen
              buffer.

              For execution efficiency, the buffer pointer must be at
              4-byte boundary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_update
  (
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Source image width */
  int16 src_width,
  /* Source rectangle starting row */
  int16 src_starting_row,
  /* Source rectangle starting column */
  int16 src_starting_column,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns,
  /* Device rectangle starting row */
  int16 dst_starting_row,
  /* Device rectangle starting column */
  int16 dst_starting_column
  );

  void disp_update2
  (
  /* The buffer pointer point to the first byte of the whole buffer.
  */
  void *buf_ptr,
  /* Source image width */
  int16 src_width,
  /* Source rectangle starting row */
  int16 src_starting_row,
  /* Source rectangle starting column */
  int16 src_starting_column,
  /* Number of rows to update */
  int16 num_of_rows,
  /* Number of columns to update */
  int16 num_of_columns,
  /* Device rectangle starting row */
  int16 dst_starting_row,
  /* Device rectangle starting column */
  int16 dst_starting_column
  );

/*===========================================================================

FUNCTION      DISP_SET_CONTRAST

DESCRIPTION
              This function sets the contrast of the display device.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_set_contrast(
                        word contrast /* Contrast value to set */
                        );

  void disp_set_contrast2(
                         word contrast /* Contrast value to set */
                         );

/*===========================================================================

FUNCTION      DISP_SET_DISPLAY_AREA

DESCRIPTION
              This function sets the display area.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_set_display_area(
                            word start_row,
                            word end_row
                            );

  void disp_set_display_area2(
                             word start_row,
                             word end_row
                             );

/*===========================================================================

FUNCTION      DISP_LOCK_BACKLIGHT

DESCRIPTION
              This function locks the backlight level 
              and prevents any user from changing it.
              
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
              Only one application can lock backlight controls

===========================================================================*/

  word disp_lock_backlight(void);
  word disp_lock_backlight2(void);

/*===========================================================================

FUNCTION      DISP_UNLOCK_BACKLIGHT

DESCRIPTION
              This function unlocks the backlight level 
              so that user can control backlight.
              
DEPENDENCIES
  None

RETURN VALUE
  LCD_FAIL if client failed to lock backlight level otherwise LCD_SUCCESSFUL

SIDE EFFECTS
              Only the last application that locked the 
              backlight controls can unlock it

===========================================================================*/

  LCD_STATUS disp_unlock_backlight(word client_pin);
  LCD_STATUS disp_unlock_backlight2(word client_pin);


/*===========================================================================

FUNCTION      DISP_SET_BACKLIGHT

DESCRIPTION
              This function sets the backlight level.
              If level is 0, backlight is turned off.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_set_backlight(byte level);
  void disp_set_backlight2(byte level);

/*===========================================================================

FUNCTION      DISP_SET_BACKLIGHT

DESCRIPTION
              This function sets the backlight level.
              If level is 0, backlight is turned off.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
  void disp_get_backlight(byte *level);
  void disp_get_backlight2(byte *level);

/*===========================================================================

FUNCTION      DISP_CLEAR_WHOLE_SCREEN

DESCRIPTION
              This function clears the whole screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_clear_whole_screen(void);
  void disp_clear_whole_screen2(void);

/*===========================================================================

FUNCTION      DISP_CLEAR_SCREEN_AREA

DESCRIPTION
              This function clears an area of the screen.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_clear_screen_area
  (
  word start_row,
  word end_row,
  word start_column,
  word end_column
  );

  void disp_clear_screen_area2
  (
  word start_row,
  word end_row,
  word start_column,
  word end_column
  );


/*===========================================================================

FUNCTION      DISP_OFF

DESCRIPTION
              This function turns off the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_off(void);
  void disp_off2(void);

/*===========================================================================

FUNCTION      DISP_ON

DESCRIPTION
              This function turns on the display.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_on(void);
  void disp_on2(void);

/*===========================================================================

FUNCTION      DISP_COPY16

DESCRIPTION
              This function copies the bitmap block to LCD device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_copy16
  (
  void  *src_ptr,
  void  *dst_ptr,
  dword count
  );

/*===========================================================================

FUNCTION      DISP_COPY32

DESCRIPTION
              This function copies the bitmap block to LCD device

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_copy32
  (
  void  *src_ptr,
  void  *dst_ptr,
  dword count
  );

/*===========================================================================

FUNCTION      MEMSET16

DESCRIPTION
              This function sets 16-bit word string

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void memset16
  (
  void  *ptr,
  dword value,
  dword count
  );

/*===========================================================================

FUNCTION      DISP_CAPTURE16

DESCRIPTION
              This function copies from LCD device buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_capture16
  (
  void  *src_ptr,
  void  *dst_ptr,
  dword count
  );

/*===========================================================================

FUNCTION      DISP_CAPTURE32

DESCRIPTION
              This function copies from LCD device buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

  void disp_capture32
  (
  void  *src_ptr,
  void  *dst_ptr,
  dword count
  );

#ifdef __cplusplus
}
#endif

#endif    /* DISP_H */
