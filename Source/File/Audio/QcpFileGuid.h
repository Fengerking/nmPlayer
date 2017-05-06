/************************************************************************
*																		*
*		VisualOn, Inc. Confidential and Proprietary, 2003				*
*																		*
************************************************************************/
/*******************************************************************************
File:		QcpFileGuid.h

Contains:	Internal GUID Of QCP File

Written by:	East

Reference:	

Change History (most recent first):
2006-08-22		East			Create file

*******************************************************************************/
#ifndef __QCP_File_GUID_H
#define __QCP_File_GUID_H

#include "voType.h"
#include "fMacros.h"


#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

// {5E7F6D41-B115-11D0-BA91-00805FB4B97E}
VO_GUID GUID_CODEC_QCELP13 = {0x5E7F6D41, 0xB115, 0x11D0, {0xBA, 0x91, 0x00, 0x80, 0x5F, 0xB4, 0xB9, 0x7E}};

// {5E7F6D42-B115-11D0-BA91-00805FB4B97E}
VO_GUID GUID_CODEC_QCELP13_1 = {0x5E7F6D42, 0xB115, 0x11D0, {0xBA, 0x91, 0x00, 0x80, 0x5F, 0xB4, 0xB9, 0x7E}};

// {E689D48D-9076-46B5-91EF-736A5100CEB4}
VO_GUID GUID_CODEC_EVRC = {0xE689D48D, 0x9076, 0x46B5, {0x91, 0xEF, 0x73, 0x6A, 0x51, 0x00, 0xCE, 0xB4}};

// {8D7C2B75-A797-ED49-985E-D53C8CC75F84}
VO_GUID GUID_CODEC_SMV = {0x8D7C2B75, 0xA797, 0xED49, {0x98, 0x5E, 0xD5, 0x3C, 0x8C, 0xC7, 0x5F, 0x84}};

#ifdef _VONAMESPACE
}
#endif

#endif	//__QCP_File_GUID_H
