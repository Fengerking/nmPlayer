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

/* debug.c
 * --------------------------------------
 *
 * Platform-specific implementations of memory testing functions
 *
 * Jon Recker, November 2002
 */

#include "wrapper.h"

#if !defined (_DEBUG) || defined(_WINCE)

void DebugMemCheckInit(void)
{
}

void DebugMemCheckStartPoint(void)
{
}

void DebugMemCheckEndPoint(void)
{
}

void DebugMemCheckFree(void)
{
}

#elif defined (_WIN32)

#include <crtdbg.h>

#ifdef FORTIFY
#include "fortify.h"
#else
static 	_CrtMemState oldState, newState, stateDiff;
#endif

void DebugMemCheckInit(void)
{
	int tmpDbgFlag;

	/* Send all reports to STDOUT */
	_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
	_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );	

	tmpDbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
	tmpDbgFlag |= _CRTDBG_LEAK_CHECK_DF;
	tmpDbgFlag |= _CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag(tmpDbgFlag);
}

void DebugMemCheckStartPoint(void)
{
#ifdef FORTIFY
	Fortify_EnterScope();
#else
	_CrtMemCheckpoint(&oldState);
#endif
}

void DebugMemCheckEndPoint(void)
{
#ifdef FORTIFY
	{
		int i;
		Fortify_LeaveScope();
		if (i = Fortify_CheckAllMemory())
			LogFormattedString("\nFortify memory check: ERROR %d block%c failed memory check\n", i, (i == 1 ? ' ' : 's'));
		else
			LogFormattedString("\nFortify memory check: okay\n");
	}
#else
	_CrtMemCheckpoint(&newState);
	_CrtMemDifference(&stateDiff, &oldState, &newState);
	_CrtMemDumpStatistics(&stateDiff);
#endif
}

void DebugMemCheckFree(void)
{
	printf("\n");
	if (!_CrtDumpMemoryLeaks())
		LogFormattedString("Memory leak test:      no leaks\n");

	if (!_CrtCheckMemory())
		LogFormattedString("Memory integrity test: error!\n");
	else 
		LogFormattedString("Memory integrity test: okay\n");

#ifdef FORTIFY
	{
		int i;
		if (i = Fortify_CheckAllMemory())
			LogFormattedString("\nFortify memory check:  ERROR %d block%c failed memory check\n", i, (i == 1 ? ' ' : 's'));
		else
			LogFormattedString("\nFortify memory check:  okay\n");
	}
#endif
}

#else

void DebugMemCheckInit(void)
{
}

void DebugMemCheckStartPoint(void)
{
}

void DebugMemCheckEndPoint(void)
{
}

void DebugMemCheckFree(void)
{
}

#endif
