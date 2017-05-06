/******************************************************************************
TDAS Implementation Kit

  Confidential Information - Limited distribution to authorized persons only.
  This material is protected under international copyright laws as an 
  unpublished work. Do not copy. 
  Copyright (C) 2004-2010 Dolby Laboratories Inc. All rights reserved.

	File:           target.h
	Contributors:	Andrew Reilly, Roger Butler
	Description:    C reference for DSPs

******************************************************************************/

#ifndef TARGET_H
#define TARGET_H 1

/* Include definitions for compilation environment */
#include "compiler.h"

/* IO_DATUM is the sample type used by the external API */
typedef INT16_TYPE IO_DATUM;
#define IO_DATUM_MAX 0x7FFF
#define IO_DATUM_MIN -0x8000
#define IO_BITSIZE 16

/*
 * An INTEGER is the integer that is the same size as COEF on the
 * platform. Thus more accurate platforms may have more resolution for
 * control values. A full scale control value will be 2^bits_in_word(-1LSB)
 * on any platform.
 */

/* Build mixed 16x32 fixed point version */
typedef INT16_TYPE INTEGER;
typedef INT16_TYPE COEF;
typedef INT32_TYPE REAL;
typedef UINT32_TYPE uREAL;

/*
 * This is the short mixed fixed representation. It uses 16-bit coefficients
 * and 32-bit data, with a 32-bit accumulator. 
 */
typedef INT32_TYPE PRODUCT;
typedef INT32_TYPE ACCUMULATOR;

#define BITS_IN_REAL (27)
#define BITS_IN_COEF (15)
#define BITS_IN_INTEGER BITS_IN_COEF
#if (BITS_IN_INTEGER-BITS_IN_COEF)
	#error "INTEGER and COEF must be the same word size"
#endif

/*
 * If defined, the simplified initialisation API is enabled. The simplified API
 * allows use of tdas_open and tdas_close to initialise and allocate memory. 
 * If not defined, tdas_init, tdas_query_memory and tdas_uninit must be used
 * instead.
 */
#define ENABLE_SIMPLIFIED_API

#endif /* TARGET_H */

