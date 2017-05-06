/************************************************************************
VisualOn Proprietary
Copyright (c) 2003, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/
/************************************************************************
* @file voFormatConversion.mm
*         used for ios chinese charact convert
* @author  Chris Qian
* 
* Change History
* 2012-11-28    Create File
************************************************************************/

#import "voFormatConversion.h"
#import <Foundation/Foundation.h>

voFormatConversion* voFormatConversion::m_cFormatConversion = NULL;

voFormatConversion* voFormatConversion::instance() {
	if (NULL == m_cFormatConversion) {
		m_cFormatConversion = new voFormatConversion();
	}
	return m_cFormatConversion;
}

voFormatConversion::voFormatConversion(){
    m_szOutput = NULL;
}

voFormatConversion::~voFormatConversion() {
}

void voFormatConversion::OnDestory()
{
    if (m_szOutput != NULL) {
        free(m_szOutput);
        m_szOutput = NULL;
    }
    
	if (NULL != m_cFormatConversion) {
		delete m_cFormatConversion;
		m_cFormatConversion = NULL;
	}
}

void voFormatConversion::FormatConversion(const char* szInput, char **szOutput, int *length, ENCODING_TYPE nType)
{
   
    NSStringEncoding enc;
    switch (nType) {
        case GB_2312_80:
            enc = CFStringConvertEncodingToNSStringEncoding(kCFStringEncodingGB_18030_2000);
            break;
        default:
            break;
    }
    int len = strlen(szInput) * 2;
    NSData *pData = [[NSData alloc] initWithBytes:szInput length:len];
    NSString *pString= [[NSString alloc] initWithData:pData encoding:enc];
    
    [pData release];
    pData = nil;
    
    if (*szOutput != NULL) {
        free(*szOutput);
        *szOutput = NULL;
    }
    len = [pString length];
    *szOutput = (char *)malloc(len);
    
    memcpy(*szOutput, [pString cStringUsingEncoding:NSUTF16StringEncoding], len);
    *length = len;
    m_szOutput = *szOutput;
//    printf("pString:%s\r\n length:%d,%d",[pString UTF8String],[pString length],len);
//    printf("szOutput:%s,*length:%d\r\n",*szOutput,*length);
    [pString release];
    pString = nil;
}

