//
//  CHLSHttpRespond.h
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#ifndef __CHLSHttpRespond_H__
#define __CHLSHttpRespond_H__

#include "CHttpRespond.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CHLSHttpRequest;

class CHLSHttpRespond : public CHttpRespond
{
public:
	CHLSHttpRespond (CHttpBaseStream * pStream);
	virtual ~CHLSHttpRespond(void);
    
public:
    virtual bool	Create (CHttpRequest * pRequest, unsigned char * pBuff, int * nSize);
    
private:
    bool CheckRequestSupport(CHLSHttpRequest * pRequest, unsigned char * pBuff, int * nSize);
    
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif /* defined(__CHLSHttpRespond_H__) */
