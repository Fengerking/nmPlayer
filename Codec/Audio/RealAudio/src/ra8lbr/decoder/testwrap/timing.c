/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0 and Exhibits. 
 * REALNETWORKS CONFIDENTIAL--NOT FOR DISTRIBUTION IN SOURCE CODE FORM 
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. 
 * All Rights Reserved. 
 * 
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Community Source 
 * License Version 1.0 (the "RCSL"), including Attachments A though H, 
 * all available at http://www.helixcommunity.org/content/rcsl. 
 * You may also obtain the license terms directly from RealNetworks. 
 * You may not use this file except in compliance with the RCSL and 
 * its Attachments. There are no redistribution rights for the source 
 * code of this file. Please see the applicable RCSL for the rights, 
 * obligations and limitations governing use of the contents of the file. 
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 * 
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 * https://rarvcode-tck.helixcommunity.org 
 * 
 * Contributor(s): 
 * 
 * ***** END LICENSE BLOCK ***** */ 

/* timing.c
 * --------------------------------------
 *
 * Jon Recker, November 2002
 */

#include "wrapper.h"

#if (defined _WIN32 && !defined _WIN32_WCE) || defined __GNUC__ || defined ARM_ADS

#include <time.h>
#define CLOCKS_PER_MSEC		(CLOCKS_PER_SEC / 1000)

int InitTimer(void)
{
    return 0;
}

unsigned int ReadTimer(void)
{
    return clock();
}

int FreeTimer(void)
{
    return 0;
}

unsigned int GetClockFrequency(void)
{
    return CLOCKS_PER_SEC;
}

unsigned int CalcTimeDifference(unsigned int startTime, unsigned int endTime)
{
    /* timer counts up on x86, 32-bit counter */
    if (endTime < startTime)
		return (0x7fffffff - (startTime - endTime) );
    else
		return (endTime - startTime);
}

#elif 0
/*#elif defined (ARM_ADS)*/

/* see definitions in ADSv1_2/bin/peripherals.ami */
#define TIMER_BASE		0x0a800000
#define TIMER_VALUE_1	(TIMER_BASE + 0x04)
#define TIMER_CONTROL_1	(TIMER_BASE + 0x08)

/* make sure this matches your ARMulator setting and the control word in InitTimer()!!! */	
#define CLOCKS_PER_SEC	(120000000 / 256)		

int InitTimer(void)
{
	volatile unsigned int *timerControl1 = (volatile unsigned int *)TIMER_CONTROL_1;
	unsigned int control1;
	
	/* see ARMulator Reference, pg 4-78 
	 * bits [3:2] = clock divisor factor (00 = 1, 01 = 16, 10 = 256, 11 = undef)
	 * bit  [6]   = free-running mode (0) or periodic mode (1)
	 * bit  [7]   = timer disabled (0) or enabled (1)
	 */
	control1 = 0x00000088;
	*timerControl1 = control1;
	
	return 0;
}

unsigned int ReadTimer(void)
{
	volatile unsigned int *timerValue1 = (volatile unsigned int *)TIMER_VALUE_1;
	unsigned int value;
	
	value = *timerValue1 & 0x0000ffff;
	
	return (unsigned int)value;
}

int FreeTimer(void)
{
    return 0;
}

unsigned int GetClockFrequency(void)
{
    return CLOCKS_PER_SEC;
}

unsigned int CalcTimeDifference(unsigned int startTime, unsigned int endTime)
{
	
    /* timer counts down int ARMulator, 16-bit counter */
    if (endTime > startTime)
		return (startTime + 0x00010000 - endTime);
    else
		return (startTime - endTime);
}

#else

int InitTimer(void)
{
	return -1;
}

unsigned int ReadTimer(void)
{
	return 0;
}

int FreeTimer(void)
{
    return 0;
}

unsigned int GetClockFrequency(void)
{
    return 0;
}

unsigned int CalcTimeDifference(unsigned int startTime, unsigned int endTime)
{
	
	return 0;
}

#endif
