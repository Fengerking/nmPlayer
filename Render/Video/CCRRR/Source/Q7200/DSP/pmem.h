#ifndef PMEM_H
#define PMEM_H

/* =========================================================================

DESCRIPTION
  Implementation of routines for allocation/de-allocation of memory buffers.

  The PMEM system is a framework to manage heaps. Each heap has certain
  attributes associated with it. These attributes are opaque to the PMEM
  system and they are represented as the lower 16 bits of the 32 bit
  attribute word (the upper 16 bits are reserved for the PMEM system and
  future expansion).

  A specific memory area is introduced to the PMEM system through the
  pmem_init() function. This function allows for specification of a
  unique bit pattern to be associated with this memory area. This
  bit pattern (or an ored combination of patterns) are then used later
  when memory with certain properties are allocated through the
  pmem_alloc() function. The intention is that the bit pattern has a
  semantic value in relation to the memory area. This semantic value
  is target specific.

  Unique IDs are used in the pmem_alloc() request. This allows for
  binary compatibility with precompiled libraries. However, to maintain
  this compatibility, IDs are required to be unique over the lifetime
  of this implementation. This also requires a target specific
  translation between IDs and the attributes. This translation table
  is maintained in a target specific file. Furthermore, target specific
  attributes are maintained in a target specific header file.

  IDs are maintained in the "pmem_ids.h" file. The naming convention
  for IDs makes it pretty clear what ID to use. The PMEM_UNCACHED_ID
  ID can be used if there is not a more specific ID available.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The pmem_module_init() needs to be called before any other calls
  are made to the pmem system.

Copyright (c) 2006      by QUALCOMM Incorporated.  All Rights Reserved.
============================================================================ */

/*===========================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/Third_Party_OS/wince/WMSHARED/MAIN/csp/amss/services/pmem/pmem.h#2 $ $DateTime: 2006/10/18 11:42:01 $ $Author: charlesp $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/16/06   jc      Added dynamic heap add, delete, and reconfiguration.
05/05/06   jc      Added statistics and heap consistency checks.
05/05/06   jc      Added pmem_realloc() and PMEM_GET_PHYS_ADDR.
06/06/05   ma      Cleaned up comments
04/07/05   ma      Modified to use IDs instead.
03/17/05   ma      Used this file as a base for the 7500
02/02/05   kk      Original file created.
===========================================================================*/

/*============================================================================
                          INCLUDE FILES
============================================================================*/
#include <stddef.h>
#include "pmem_ids.h"


//#include "pmem_7500.h"

/*============================================================================
                          CONSTANT DEFINITIONS
============================================================================*/

/* Maximum number of heaps that PMEM can manage */
#ifndef PMEM_MAX_HEAPS
#define PMEM_MAX_HEAPS    8
#endif

/*============================================================================
                          DATA DECLARATIONS
============================================================================*/

/* PMEM heap usage report data */
typedef struct pmem_report_heap_s
{
  size_t total;
  size_t used;
  size_t max_used;
  size_t max_available;
} pmem_report_heap_s_type;

/* PMEM summary usage report data */
typedef struct pmem_report_s
{
  pmem_report_heap_s_type heap[PMEM_MAX_HEAPS];
} pmem_report_s_type;

/*============================================================================
                          MACRO DEFINITIONS
============================================================================*/

/* PMEM interface to support virtual memory addresses */
/* For L4, PMEM heaps are idempotent (1:1 virtual-to-physical MMU mapping),
 * so no translation is required.
 */
#define FEATURE_OS_MEMORY_PROTECTION
#ifndef FEATURE_OS_MEMORY_PROTECTION
  #define PMEM_GET_PHYS_ADDR(addr)    (addr)
#else /* FEATURE_OS_MEMORY_PROTECTION */
  //extern void *pmem_get_phys_addr(void *ptr);
  #define PMEM_GET_PHYS_ADDR(addr)    pmem_get_phys_addr(addr)
#endif /* FEATURE_OS_MEMORY_PROTECTION */

/* For backward compatibility */
#define pmem_init(start, size, attr)  pmem_heap_init((start), (size), (attr))

/*============================================================================
                          EXPORTED FUNCTIONS
============================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION pmem_module_init

DESCRIPTION
  Initializes internal data structures used by the pmem system

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

ARGUMENTS
  None

===========================================================================*/
void pmem_module_init(void);

void *pmem_get_phys_addr( void *ptr);
/*===========================================================================

FUNCTION pmem_heap_init

DESCRIPTION
  Register a heap with the PMEM system. The heap is associated with the
  target specific attributes provided. Subsequent allocations made with these
  attributes will allocate memory from this heap.

DEPENDENCIES
  None

RETURN VALUE
  int - 0 on success and -1 on failure

SIDE EFFECTS
  None

===========================================================================*/
int pmem_heap_init
(
  void *start,        /* Start address of heap */
  size_t size,        /* Size of heap */

  /* Target-specific bit fields of attributes that are associated with this
   * heap.  They are matched against an allocation request to deterimine
   * if the heap satisfy the request.
   */
  unsigned int attr
);

/*===========================================================================

FUNCTION pmem_heap_delete

DESCRIPTION
  Deregister a heap with the PMEM system.  This operation will fail if
  the heap is not completely free, or if it is corrupted.

DEPENDENCIES
  None

RETURN VALUE
  int - 0 on success and -1 on failure

SIDE EFFECTS
  None

===========================================================================*/
int pmem_heap_delete
(
  void *start         /* Start address of heap */
);

/*===========================================================================

FUNCTION pmem_heap_resize

DESCRIPTION
  Resize a heap that is already registered with the PMEM system, and may
  have allocated blocks.  This operation will fail if all the allocated
  blocks do not reside within the new region size.

DEPENDENCIES
  None

RETURN VALUE
  int - 0 on success and -1 on failure

SIDE EFFECTS
  None

===========================================================================*/
int pmem_heap_resize
(
  void *start,        /* Start address of heap */
  size_t size         /* New size of heap */
);

/*===========================================================================

FUNCTION pmem_malloc

DESCRIPTION
  Allocates a block of size bytes from the heap that matches the requested
  flags/attributes. 

  The caller must specify one of the PMEM indentifiers defined in pmem_id.h.
  PMEM uses this indentifier to determine the specific attributes associated
  with the requested memory allocation.

DEPENDENCIES
  None

RETURN VALUE
  void* - A pointer to the newly allocated block, or NULL if the block
  could not be allocated. The pointer is 1k aligned.

SIDE EFFECTS
  None

===========================================================================*/
void *pmem_malloc
(
  size_t size,      /* Size of allocation request */
  unsigned int id   /* PMEM indentifier, see PMEM_*_ID in pmem_ids.h */
);

/*===========================================================================

FUNCTION pmem_realloc

DESCRIPTION
  Resizes the block of memory to be the new size while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.

  In general, this function is not supported for PMEM heaps.  It requires
  a target-specific implementation.  Depending on the target implementation
  and the PMEM identifier, the resized memory block may or may not have the
  same base address as the original block.

DEPENDENCIES
  None

RETURN VALUE
  void* - Returns a pointer to the beginning of the resized block of memory
  (which may be different than the original) or NULL if the block cannot be
  resized.

SIDE EFFECTS
  None

===========================================================================*/
void *pmem_realloc
(
  void *ptr,        /* Pointer to the memory block to reallocate */
  size_t size       /* New size of for the memory block */
);

/*===========================================================================

FUNCTION pmem_free

DESCRIPTION
  Deallocates a block of memory and returns it to the right heap.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_free
(
  void *ptr   /* Pointer to the memory block that needs to be deallocated */
);

/*===========================================================================

FUNCTION pmem_report

DESCRIPTION
  Provide PMEM usage report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_report
(
  pmem_report_s_type *report_ptr  /* Pointer to buffer for report data */
);

/*===========================================================================

FUNCTION pmem_log

DESCRIPTION
  Logs heap statistics.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pmem_log(void);

#ifdef __cplusplus
}
#endif

#endif
