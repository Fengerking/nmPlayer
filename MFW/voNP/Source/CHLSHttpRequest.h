//
//  CHLSHttpRequest.h
//  TestHLSPlayer
//
//  Created by Jim Lin on 9/11/12.
//
//

#ifndef __CHLSHttpRequest_H__
#define __CHLSHttpRequest_H__

#include "CHttpRequest.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CHLSHttpRequest : public CHttpRequest
{
public:
    CHLSHttpRequest (void);
    virtual ~CHLSHttpRequest(void);

public:
    virtual bool	ParseLine (char* pLine, int nSize);
    
public:
    char    m_szAcceptEncoding[256];
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif /* defined(__CHLSHttpRequest_H__) */
