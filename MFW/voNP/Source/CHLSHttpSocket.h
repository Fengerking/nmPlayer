//
//  CHLSHttpSocket.h
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#ifndef __CHLSHttpSocket_H__
#define __CHLSHttpSocket_H__

#include "CHttpSocket.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CHLSHttpSocket : public CHttpSocket
{
public:
	CHLSHttpSocket(CHttpBaseStream * pStream);
	virtual ~CHLSHttpSocket(void);
        
protected:
   	virtual int		DoneRead (void);
};

#ifdef _VONAMESPACE
}
#endif // End _VONAMESPACE

#endif /* defined(__CHLSHttpSocket_H__) */
