//
//  CListenSocketHLS.h
//  TestHLSPlayer
//
//  Created by Jim Lin on 9/11/12.
//
//

#ifndef __CListenSocketHLS__
#define __CListenSocketHLS__

#include "CListenSocket.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


class CHLSListenSocket : public CListenSocket
{
public:
	CHLSListenSocket (void * hUserData, int nPortNum);
	virtual ~CHLSListenSocket(void);
    
    virtual int Open (void * pSource, int nType);
        
protected:
	virtual int	OnRead (void);

private:
    
};
    
#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE


#endif /* defined(__CListenSocketHLS__) */
