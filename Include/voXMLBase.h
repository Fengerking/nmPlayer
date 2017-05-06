/************************************************************************
VisualOn Proprietary
Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/


#ifndef __voXMLBase_H__
#define __voXMLBase_H__


#include "voType.h"
#include "voMem.h"
	/*set param*/
#define VOXML_PID_PARSER_FLUSH 0x000001
#define VOXML_PID_SOURCE			  0x000002
#define VOXML_PID_PARSERMODE_SAX		0x000003
#define VOXML_PID_PARSERMODE_DOM	0x000004
/* source flag*/
#define voXML_FLAG_SOURCE_BUFFER  0x0000a0
#define voXML_FLAG_SOURCE_URL	  0x0000a1


typedef enum
{
	VO_XML_TYPE_NONE = 0,
	VO_XML_TYPE_TAG = 1,
	VO_XML_TYPE_ATTRIBUTE,
	VO_XML_TYPE_VALUE
}VO_XML_LABLE_TYPE;

typedef enum
{
	VO_XML_ACTION_STARTELEMENT=1,
	VO_XML_ACTION_ENDELEMENT,
	VO_XML_ACTION_STARTDOCUMENT,
	VO_XML_ACTION_ENDDOCUMENT,
	VO_XML_ACTION_CHARACTERS
}VO_XML_ACTION_MODE;


	
#endif