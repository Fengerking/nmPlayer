/*****************************************************************************
*																			*
*		VisualOn, Inc. Confidential and Proprietary, 2011					*
*																			*
*****************************************************************************/
#ifndef _VOASMPORT_H_
#define _VOASMPORT_H_

#include "../voRVDecID.h"

#if defined(VOANDROID)
#define VOASMFUNCNAME(name)                  name
#define VOGLOBAL                             .global
#define VOSECTION_CODE                       .section .text
#define VOEQU(name, value)                   .equ (name), (value)
#elif defined(_IOS)
#define VOASMFUNCNAME(name)                  _##name
#define VOGLOBAL  .globl
#define VOSECTION_CODE                       .text  
#define VOEQU(name, value)                   .set (name), (value)
#else
#define VOASMFUNCNAME(name)                  name
#define VOGLOBAL  EXPORT
#define VOSECTION_CODE                       AREA    |.text|, CODE, READONLY
#define VOEQU(name, value)                   (name) EQU (value)
//#define VOASMFUNCNAME(name)                  name
//#define VOGLOBAL                             .global
//#define VOSECTION_CODE                       .section .text
#endif


#endif
