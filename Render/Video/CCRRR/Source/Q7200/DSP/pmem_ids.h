#ifndef PMEM_IDS_H
#define PMEM_IDS_H
/* =========================================================================

DESCRIPTION
  This file contains the list of unique IDs used when allocating
  "physical" memory using the pmem allocator.
  It has been determined that it is advantageous to use unique IDs since
  it more easily facilitates binary distribution of our software. The
  software doesn't have to be recompiled if attributes change for
  a particular platform. The ID serves as a fixed and platform independent API
  toward a platform dependent allocation policy.

  However, it is extremely important that IDs are unique. Old IDs are
  never retired. New IDs are created in strict sequential order.

*******************************************************************************
*** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING ***
*******************************************************************************
***                                                                         ***
***  Avoid creating multiple copies of this file (branching, orphans, etc.) ***
***  in order to manage the uniqueness of the IDs across all targets.       ***
***                                                                         ***
*******************************************************************************
*** WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING ***
*******************************************************************************

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The pmem_module_init() needs to be called before any other calls
  are made to the pmem system.

Copyright (c) 2005-2007 by QUALCOMM Incorporated.  All Rights Reserved.
============================================================================ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/Third_Party_OS/wince/WMSHARED/MAIN/csp/amss/services/pmem/pmem_ids.h#5 $ $DateTime: 2007/05/11 13:56:43 $ $Author: charlesp $

when       who     what, where, why
--------   ---     --------------------------------------------------------
12/13/06   jc      Added video and camcorder LP IDs, and audio ID.
11/20/06   jc      Added remote RPC interface for IMEM control.
11/09/06   jc      Added IMEM and VIDEO_ADSP IDs.
05/24/06   jc      Added PMEM_VIDEO_SLICE_ID.
05/22/06   jc      Added handset PMEM_HS_ID.
05/05/06   jc      Added PMEM_CAMCORDER_ID.
01/15/06   jc      Added support for ARM11 TCM.
08/26/05   jc      Added generic cached and uncached IDs.
06/06/05   ma      Added generic ID
04/06/05   ma      Original file created.
===========================================================================*/

/*============================================================================
*                         INCLUDE FILES
============================================================================*/

/*============================================================================
*                         CONSTANT DEFINITIONS
============================================================================*/

#define PMEM_ADSP_ID                        0
#define PMEM_GRAPHICS_COMMAND_ID            1
#define PMEM_GRAPHICS_VERTEX_ID             2
#define PMEM_GRAPHICS_FRAMEBUFFER_ID        3
#define PMEM_GRAPHICS_TEXTURE_ID            4
#define PMEM_VIDEO_ID                       5
#define PMEM_MDP_ID                         6
#define PMEM_USB_ID                         7
#define PMEM_MDDI_ID                        8
#define PMEM_CAMERA_ID                      9
#define PMEM_UNCACHED_ID                    10   /* Generic uncached memory */ 
#define PMEM_CACHED_ID                      11   /* Generic cached memory */ 
#define PMEM_VIDEO_CACHE_ID                 12
#define PMEM_CAMCORDER_ID                   13
#define PMEM_HS_ID                          14
#define PMEM_VIDEO_SLICE_ID                 15
#define PMEM_GRAPHICS_IMEM_ID               16
#define PMEM_VIDEO_IMEM_ID                  17
#define PMEM_VIDEO_ADSP_ID                  18
#define PMEM_VIDEO_LP_ID                    19
#define PMEM_CAMCORDER_LP_ID                20
#define PMEM_AUDIO_ID                       21
#define PMEM_NAND_ID                        22
/****** Increment PMEM_NUM_IDS when a new ID is added ******/
#define PMEM_NUM_IDS                        23
/***********************************************************/

/* The following ID is deprecated and should not be used.
 * Instead, use either PMEM_UNCACHED_ID or PMEM_CACHED_ID.
 */
#define PMEM_GENERIC_ID                     PMEM_UNCACHED_ID

/*============================================================================
                        DATA DECLARATIONS
============================================================================*/

/*============================================================================
*                         MACRO DEFINITIONS
============================================================================*/

/*============================================================================
*                         EXPORTED FUNCTIONS
============================================================================*/

#endif  /* PMEM_IDS_H */
