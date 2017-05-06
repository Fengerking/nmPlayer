/******************************************************************************
TDAS Implementation Kits

  Confidential Information - Limited distribution to authorized persons only.
  This material is protected under international copyright laws as an 
  unpublished work. Do not copy. 
  Copyright (C) 2004-2010 Dolby Laboratories Inc. All rights reserved.
  
	File:           control.h
	Contributors:	Roger Butler, Andrew Reilly
	Description:    C implementation for DSPs
	  
******************************************************************************/

#ifndef CONTROL_H
#define CONTROL_H

/* Set/get parameter values for each feature. The state_ptr will have been
 * allocated by an _open() function. The feature and parameter variables 
 * identify the parameter to change. The INTEGER type is the basic control
 * type for all parameters. Note the comment about includes above.
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void tdas_set(void* state_ptr, int feature, int parameter, INTEGER value);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
INTEGER tdas_get(const void* state_ptr, int feature, int parameter);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
#ifdef TDAS_DLL
IMPORT_C
#endif
void tdas_set_coef(void* state_ptr, int feature, int index, int len, const char *values);
#ifdef __cplusplus
}
#endif

#endif /* CONTROL_H */


