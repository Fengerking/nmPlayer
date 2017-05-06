#ifndef __CHTTPs_Transaction_H__
#define __CHTTPs_Transaction_H__

#include "HTTP_Transaction.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class CHTTPs_Transaction :
	public CHTTP_Transaction
{
public:
	CHTTPs_Transaction(void);
	~CHTTPs_Transaction(void);

	virtual VO_S32 Start(RequestInfo *);
};

#ifdef _VONAMESPACE
}
#endif

#endif // __CHTTPs_Transaction_H__
