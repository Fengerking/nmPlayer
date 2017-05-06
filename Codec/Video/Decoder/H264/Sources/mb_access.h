
/*!
 *************************************************************************************
 * \file mb_access.h
 *
 * \brief
 *    Functions for macroblock neighborhoods
 *
 * \author
 *     Main contributors (see contributors.h for copyright, address and affiliation details)
 *     - Karsten Sühring          <suehring@hhi.de>
 *************************************************************************************
 */

#ifndef _MB_ACCESS_H_
#define _MB_ACCESS_H_

#ifdef DEBUG_INFO
#ifdef X86_TEST1
#define DEBUG_DUAL_CORE 1
#elif defined(ARM)//for ARM, debug it by default
#define DEBUG_DUAL_CORE 1
#endif//X86_TEST
#else
#define DEBUG_DUAL_CORE 0
#endif//DEBUG_INFO

#if !DEBUG_DUAL_CORE
#define TAvdLog(LL_INFO,...)
#endif//DEBUG_DUAL_CORE


#endif//_MB_ACCESS_H_
