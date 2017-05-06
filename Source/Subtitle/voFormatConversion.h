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
* @file voFormatConversion.h
*
* @author  Chris Qian
* 
* Change History
* 2012-11-28    Create File
************************************************************************/
typedef enum {
    GB_2312_80		 = 0
} ENCODING_TYPE;

class voFormatConversion
{
public:
    static voFormatConversion* instance();
	virtual ~voFormatConversion();
    
    void OnDestory();
    void FormatConversion(const char* szInput, char **szOutput, int *length, ENCODING_TYPE nType);
    
private:
    voFormatConversion();
	voFormatConversion& operator=(const voFormatConversion&);
    
    static voFormatConversion *m_cFormatConversion;
    char *m_szOutput;
};

