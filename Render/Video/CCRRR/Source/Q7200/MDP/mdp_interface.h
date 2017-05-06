#ifndef __REX_INTERFACE_H__
#define __REX_INTERFACE_H__
/*===========================================================================
                           MDP Interface Header File
DESCRIPTION
  layer linking Windows display drivers to lower level MDP functions

EXTERNALIZED FUNCTIONS
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
  
  Copyright (c) 2006-2007 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                             Edit History

$Header: //depot/asic/Third_Party_OS/wince/WMSHARED/MAIN/Src/Drivers/display/DispTools/mdp_interface.h#7 $ $DateTime: 2008/04/21 17:41:14 $ $Author: axu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/13/07   trg     Removed overlay functions.
08/20/07   spw     Add support for nondestructive blt
02/13/07   SH      New driver design
11/27/06   cep     Add Disp 2D blit and update support
01/09/06   cep     First 7500 WinCE version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include <windows.h>
//#include <winddi.h> //removed by Jason, 7/8/2008

/*===========================================================================

                        Macros and defines

===========================================================================*/

#define NUMBER_OF_DISPLAYS  3

/*---------------------------------------------------------------------------
  Timeout (in ms) for the calling thread to wait for semaphores and mutexes
---------------------------------------------------------------------------*/
#define DISPLAY_FAILSAFE    5000


#if !defined(FEATURE_DISP_TASK)
  #error Windows CE display driver requires full MDP support!
#endif


/*===========================================================================
  Type Declarations
===========================================================================*/
class DisplayClass; //forward declaration

/*---------------------------------------------------------------------------
  Constants used to communicate to the power thread whether to change the
  Power state for the display or the backlight
---------------------------------------------------------------------------*/
typedef enum
{
  NO_CMD=0,
  DISPLAY_POWER,
  BACKLIGHT_POWER
} display_pwr_cmd_type;


/*---------------------------------------------------------------------------
  Constants used to communicate to the power thread whether to change the
  Power state for the display or the backlight
---------------------------------------------------------------------------*/
typedef struct ddt_struct
{
  disp_info_type LCD_Info;

  /* Pointers to display-specific AMSS functions */
  disp_info_type (*pfDisplayInfo)(void);
  void           (*pfDisplayOn)(void);
  void           (*pfDisplayOff)(void);
  void           (*pfDisplaySetBklt)(byte level);
} display_definition_type;


/*===========================================================================
                          Function Declarations
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================
FUNCTION DISPLAY_TAKEMUTEX

DESCRIPTION
  Takes the given named mutex, creates it if needed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May actually create the mutex

===========================================================================*/
extern void display_TakeMutex
(
  HANDLE  *mutexHandle, 
  LPCWSTR  mutexName 
);

/*===========================================================================
FUNCTION DISPLAY_SEMAPHOREWAIT

DESCRIPTION
  Waits for the given semaphore, creates it if needed

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May actually create the semaphore

===========================================================================*/
extern void display_SemaphoreWait
(
  HANDLE  *semaphoreHandle, 
  LPCWSTR  semaphoreName 
);

/*===========================================================================
FUNCTION ISCLAMSHELLCLOSED

DESCRIPTION
  Reads the status of the clamshell from the registry

DEPENDENCIES
  None

RETURN VALUE
  specs for connected displays

SIDE EFFECTS

===========================================================================*/
extern BOOL IsClamshellClosed
(
    void
);

/*===========================================================================
FUNCTION DISPLAY_GETINFO

DESCRIPTION
  Returns the specs for connected displays

DEPENDENCIES
  None

RETURN VALUE
  specs for connected displays

SIDE EFFECTS

===========================================================================*/
extern disp_info_type display_getinfo
(
  int iCard
);

/*===========================================================================
FUNCTION DISPLAY_INIT

DESCRIPTION
  WinCE and hardware initialization for the display driver

DEPENDENCIES
  None

RETURN VALUE
  Always returns 0

SIDE EFFECTS
  Initializes display drivers

===========================================================================*/
extern void display_init
(
  void
);

/*===========================================================================
FUNCTION DDI_DISP2D_UPDATE

DESCRIPTION
  Sends the given update to the MDP to be performed

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern disp2d_status_enum ddi_disp2d_update
(
  disp2d_bm_type *outBmp, 
    /* output bitmap image to be updated to the destination */
  disp2d_bm_update_op_type *outBmpOp,      
    /* output bitmap operation */
  disp2d_disp_type dest,
    /* output destination */
  uint32 startX,
    /* starting x location on destination */
  uint32 startY,       
    /* starting y location on destination */
  BOOL     async
  /* If TRUE indicates that we can pipeline the operation and return 
     immediately after the command is passed to the MDP.
     If FALSE indicates that this function should not return until the
     MDP operation has actually been completed. */
);

/*===========================================================================
FUNCTION DDI_DISP2D_BLT

DESCRIPTION
  Sends the given blt to the MDP to be performed

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern disp2d_status_enum ddi_disp2d_blt
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

/*===========================================================================
FUNCTION DDI_DISP2D_BLT_NONDESTRUCT

DESCRIPTION
  Sends the given blt to the MDP to be performed and outputs to separate buf

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern disp2d_status_enum ddi_disp2d_blt_nondestruct
(
  disp2d_bm_type *fgBmp, 
    /*  foreground bitmap info */
  disp2d_bm_type *bgBmp, 
    /* background bitmap info to contain the result of fgBmp+bgBmp */
  disp2d_bm_blt_op_type *fgBmpOp,      
    /* foreground bitmap operation */
  BOOL     async,
  /* If TRUE indicates that we can pipeline the operation and return 
     immediately after the command is passed to the MDP.
     If FALSE indicates that this function should not return until the
     MDP operation has actually been completed. */
  disp2d_bm_properties_type *finalBmp
  /* Pointer to output buffer area. */
);


/*=====================================================================
FUNCTION DISPLAY_DOPOWER

DESCRIPTION
  Sends the given power command to the power thread to be applied

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void display_doPower
(
    int                  PowerObject,
    display_pwr_cmd_type command,
    BOOL                 currentStatus
);

/*===========================================================================
FUNCTION DISPLAY_TVOUT

DESCRIPTION
  Wrapper for mdp TV mirroring function

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void display_TVOut
(
    LCD_TYPE src,
    BOOL    goalStatus, 
    DISP2D_MAP_INFO *mapInfo
);


/*===========================================================================
FUNCTION DISPLAY_SWITCHSCREENS

DESCRIPTION
  Switches between primary and secondary displays using IOCTLs

DEPENDENCIES
  None

RETURN VALUE
  non-zero on success. zero on failure

SIDE EFFECTS
  None

===========================================================================*/
extern int display_SwitchScreens
(
    UINT32 KeyStateFlags
);

/*===========================================================================
FUNCTION DISPLAY_INVBLANK

DESCRIPTION
  Checks if the display is in vertical blank

DEPENDENCIES
  None

RETURN VALUE
  TRUE if in vertical blank. FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern int  display_InVBlank
(
    void
);

/*===========================================================================
FUNCTION DISPLAY_ErrorFatal

DESCRIPTION
  Sets MDP to error fatal mode allowing the display to continue working , even
   after the ARM9 has suffered a fatal error

DEPENDENCIES
  None

RETURN VALUE
  TRUE on success. FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
extern int display_ErrorFatal
(
    void
);

/*===========================================================================
FUNCTION display_SignalGRP

DESCRIPTION
  Provide access for the ATI driver to write the MDP HW register
  MDP_FULL_BYPASS_WORD43.  Use to signal when the ATI buffer is finished.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void display_SignalGRP
(
    UINT32 grpRegisterVal
);


disp2d_status_enum display_SetCursorOverlay(disp2d_bm_type * pCursorBmp);
disp2d_status_enum display_SetCursorPos(int32 x, int32 y);

#ifdef __cplusplus
}
#endif

#endif /* __MDP_INTERFACE_H__ */

