	/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003-2013			*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		Smil.h

	Contains:	Smil head file

	Written by:	Aiven

	Change History (most recent first):
	2013-07-08		Aiven			Create file

*******************************************************************************/
#pragma once
#ifndef _CSMIL_H_
#define _CSMI_H_

#include "voSource2.h"
#include "voXMLLoad.h"
#include "list_T.h"
#include "voSmil.h"
#include "voDSType.h"

#define TAG_LEVELONE_SMIL		"smil"

#define TAG_LEVELTWO_HEAD		"head"
#define TAG_LEVELTHREELAYOUT	"layout"
#define TAG_LEVELTHREEMETA		"meta"
#define TAG_LEVELTHREESWITCH	"switch"

#define TAG_LEVELTWO_BODY		"body"
#define ELEMENT_A				"a"
#define ELEMENT_ANIMATION		"animation"
#define ELEMENT_AUDIO			"audio"
#define ELEMENT_IMG				"img"
#define ELEMENT_PAR				"par"
#define ELEMENT_REF				"ref"
#define ELEMENT_SEQ				"seq"
#define ELEMENT_SWITCH			"switch"
#define ELEMENT_TEXT				"text"
#define ELEMENT_TEXTSTREAM		"textstream"
#define ELEMENT_VIDEO			"video"

#define ELEMENT_PARAM			"param"


#define SUB_ELEMENT_ANCHOR			"anchor"			
#define SUB_ELEMENT_PARAMGROUP	"paramGroup"		//The paramGroup element provides a convenience mechanism for defining a group of media parameters
#define SUB_ELEMENT_PARAM			"param"			//The param element allows a general parameter value to be sent to a media object
#define SUB_ELEMENT_BRUSH 			"brush"			//A new brush element allows the specification of solid color media objects with no associated media

#define ATTRIBUTE_ABSTRACT	"abstract"		//A brief description of the content contained in the element
#define ATTRIBUTE_AUTHOR	"author"			//The name of the author of the content contained in the element
#define ATTRIBUTE_BEGIN		"begin"			//This attribute specifies the time for the explicit begin of an element
#define ATTRIBUTE_COPYRIGHT	"copyright"		//The copyright notice of the content contained in the element
#define ATTRIBUTE_DUR		"dur"			//This attribute specifies the explicit duration of an element
#define ATTRIBUTE_END		"end"			//This attribute specifies the explicit end of an element
#define ATTRIBUTE_ENDSYNC	"endsync"
#define ATTRIBUTE_ID			"id"
#define ATTRIBUTE_REGION		"region"			//This attribute specifies an abstract rendering surface
#define ATTRIBUTE_REPEAT		"repeat"
#define ATTRIBUTE_SYSTEM_BITRATE	"system-bitrate"	//This attribute specifies the approximate bandwidth
#define ATTRIBUTE_SYSTEM_CAPTIONS	"system-captions"	//This attribute allows authors to distinguish between a redundant text equivalent of the audio portion of the presentation
#define ATTRIBUTE_SYSTEM_LANGUAGE	"system-language"		//The attribute value is a comma-separated list of language names
#define ATTRIBUTE_SYSTEM_OVERDUB_OR_CAPTION	"system-overdub-or-caption"	//This attribute is a setting which determines if users prefer overdubbing or captioning	
#define ATTRIBUTE_SYSTEM_REQUIRED	"system-required"				//This attribute specifies the name of an extension
#define ATTRIBUTE_SYSTEM_SCREEN_SIZE	"system-screen-size"
#define ATTRIBUTE_SYSTEM_SCREEN_DEPTH	"system-screen-depth"
#define ATTRIBUTE_ALT		"alt"				//For user agents that cannot display a particular media-object
#define ATTRIBUTE_CLIP_BEGIN	"clipBegin"		//The clip-begin attribute specifies the beginning of a sub-clip of a continuous media object as offset from the start of the media object
#define ATTRIBUTE_CLIP_END	"clipEnd"		//The clip-end attribute specifies the end of a sub-clip of a continuous media object
#define ATTRIBUTE_FILL		"fill"
#define ATTRIBUTE_LONGDESC	"longdesc"		//This attribute specifies a link (URI) to a long description of a media object
#define ATTRIBUTE_SRC		"src"				//The value of the src attribute is the URI of the media object
#define ATTRIBUTE_TYPE		"type"			//MIME type of the media object referenced by the "src" attribute
#define ATTRIBUTE_TITLE		"title"			//This attribute offers title information about the element

#define ATTRIBUTE_GUID		"guid"			//This attribute offers guid information about the element

#define PARAM_ATTRIBUTE_NAME		"name"				//defines the name of a run-time parameter
#define PARAM_ATTRIBUTE_VALUE		"value"			//This attribute specifies the value of a run-time parameter specified by name
#define PARAM_ATTRIBUTE_VALUETYPE	"valuetype"		//This attribute specifies the type of the value attribute
#define PARAM_ATTRIBUTE_TYPE		"type"			//This attribute specifies the content type of the resource designated by the value attribute

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif


typedef enum
{
	TYPE_ELEMENT_HEAD = 0,
	TYPE_ELEMENT_BODY, 
	TYPE_ELEMENT_A,
	TYPE_ELEMENT_ANIMATION, 	
	TYPE_ELEMENT_AUDIO,	
	TYPE_ELEMENT_IMG,	
	TYPE_ELEMENT_PAR,	
	TYPE_ELEMENT_REF,	
	TYPE_ELEMENT_SEQ,	
	TYPE_ELEMENT_SWITCH,	
	TYPE_ELEMENT_TEXT,	
	TYPE_ELEMENT_TEXTSTREAM,	
	TYPE_ELEMENT_VIDEO, 
	TYPE_ELEMENT_PARAM, 	
	TYPE_ELEMENT_MAX
}VO_ELEMENT_TYPE;

typedef struct
{
	VO_CHAR*	pName;
	VO_U32		nNamesize;
	VO_CHAR*	pValue;
	VO_U32		nValuesize;
}VO_ATTRIBUTE_STRUCT;

typedef struct VO_ELEMENT_STRUCT
{
	VO_ELEMENT_TYPE			Type;
	list_T<VO_ATTRIBUTE_STRUCT*>	AttributeList;
	list_T<VO_ELEMENT_STRUCT*>	ChildList;
}VO_ELEMENT_STRUCT;


typedef struct
{
	VO_ELEMENT_STRUCT*	head;
	VO_ELEMENT_STRUCT*	body;
}VO_SMIL_STRUCT;

class CSmil{
public:
	CSmil();
	~CSmil();
	VO_U32	Init(VO_U32 nFlag, VO_PTR pParam);
	VO_U32	Uninit();
	VO_U32 Parse(VO_PBYTE pBuffer, VO_U32 nSize, VO_PTR* ppSmilStruct);

	VO_BOOL    GetLicState();	
protected:
	VO_U32 FindAttribute(VO_ELEMENT_STRUCT* pElement, VO_CHAR* pName, VO_ATTRIBUTE_STRUCT** ppAttribute);
	VO_U32 GenerateElemenType(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement);
	VO_U32 ParseParamElement(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement);
	VO_U32 ParseBaseElement(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement);	
	VO_U32 ParseElement(VO_PTR pRoot, VO_ELEMENT_STRUCT* pElement);
	
	VO_U32 ReleaseElement(VO_ELEMENT_STRUCT** ppElement);
	VO_U32 ParseSmil(VO_PBYTE pBuffer, VO_U32 nSize, VO_SMIL_STRUCT* pSMIL);
	VO_U32 ReleaseSmil(VO_SMIL_STRUCT* pSMIL);
protected:
	CXMLLoad *m_pXmlLoad;

	VO_PBYTE  m_pBuffer;
	VO_U32 	  m_uSize;

//	VO_SMIL_DATACALLBACK	m_nCallback;
	VO_SMIL_STRUCT	m_nSMIL;	
};

#ifdef _VONAMESPACE
}
#endif

#endif//_CSMI_H_




