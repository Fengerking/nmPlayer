	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		CBaseScreen.cpp

	Contains:	CBaseScreen class file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2005-05-25		JBF			Create file
	#include <memory.h>
	#include <stdio.h>
	#include <string.h>
	#include "RTSPMediaStreamSyncEngine.h"
	#include "utility.h"

	#include "network.h"
	#include "RTSPSession.h"
	#include "RealTimeStreamingEngine.h"

	#include "RDTMediaStream.h"
	#include "RDTParser.h"
	#include "TaskSchedulerEngine.h"
	#include "RTSPClientEngine.h"
	#include "MediaStreamSocket.h"
	#include <MMSystem.h>
	#endif
	#include "RDTParser.h"
	#include "TaskSchedulerEngine.h"
	#include "RTSPClientEngine.h"
	#include "MediaStreamSocket.h"

	#include "mediastream.h"
	#include "helix_types.h"
*******************************************************************************/

#include "voCrossPlatformWrapper_windows.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>
#include <tchar.h>
#include "VOUtility.h"
//using namespace VOUtility;
//#include <sys/timeb.h>	//conmented by doncy 0813
#ifndef UNDER_CE
#include <sys/timeb.h>
#endif	//UNDER_CE

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif