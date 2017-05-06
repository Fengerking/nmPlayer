/************************************************************************
 *																		*
 *		VisualOn, Inc. Confidential and Proprietary, 2003-    			*
 *																		*
 ************************************************************************/
/*******************************************************************************
 File:		voCapXMLParser.h
 
 Contains:	VisualOn cab xml parser header file
 
 Written by:	Jeff Huang
 
 Change History (most recent first):
 2012-06-09		Jeff			Create file
 *******************************************************************************/


#import <Foundation/Foundation.h>
#include "voOnStreamType.h"

@interface voCapXMLParser : NSObject<NSXMLParserDelegate>
{
    NSMutableArray *_pArrayElement;
    
    NSMutableArray *_pArrayItemList;
    
    NSMutableDictionary *_pItemAttribute;
    
    VOOSMP_PERF_DATA m_cvoPerfData;
}

-(id) init;
-(bool) parserXML:(const char *)pStrIn pInfo:(VOOSMP_CPU_INFO *)pInfo;
-(VOOSMP_PERF_DATA *) getPerfData;

@end

