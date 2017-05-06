
//value indicate for DATA SOURCE TEAM 
#ifndef __VO_SHARELIBTYPE_H__
#define __VO_SHARELIBTYPE_H__

#include "voDSType.h"

typedef void (*COPY_FUNC)(VO_PTR pDest, VO_PTR pSource);

template< class T>
class DeepCopy
{
public:
	void operator()(T *pTarget, T *pSource){ memcpy( pTarget, pSource, sizeof(T) ); }
};

 template<>
 class DeepCopy<VO_SOURCE2_VERIFICATIONINFO>
 {
 public:
 	void operator()(VO_SOURCE2_VERIFICATIONINFO *pTarget, VO_SOURCE2_VERIFICATIONINFO *pSource)
 	{ 
 		pTarget->pUserData = pSource->pUserData;
 
 
 		pTarget->pData = new VO_BYTE[ pSource->uDataSize + 1 ];
 		memset( pTarget->pData , 0 , pSource->uDataSize + 1 );
 		memcpy( pTarget->pData , pSource->pData , pSource->uDataSize );
 		pTarget->uDataSize = pSource->uDataSize;
 		pTarget->uDataFlag = 0;
 
 		pTarget->szResponse = new VO_CHAR[ pSource->uResponseSize + 1 ];
 		memset( pTarget->szResponse , 0 , pSource->uResponseSize + 1 );
 		memcpy( pTarget->szResponse , pSource->szResponse , pSource->uResponseSize );
 		pTarget->uResponseSize = pSource->uResponseSize;
 
 		pTarget->uDataFlag = pSource->uDataFlag;
 	}
	void free(VO_SOURCE2_VERIFICATIONINFO *pTarget)
	{
		if(pTarget->pData)
			delete []pTarget->pData;
		pTarget->pData = NULL;
		if(pTarget->szResponse)
			delete []pTarget->szResponse;
		pTarget->szResponse = NULL;
	}
 };


enum PARAMETER_OPTION 
{ 
	OPTION_GET = 1,
	OPTION_SET = 2
};
template < class A >
void _GETSET_PTR( A *a, VO_PTR b, PARAMETER_OPTION op)
{
	if( op == OPTION_SET ) 
	{ 
		a = (A*)b; 
	} 
	else if (op == OPTION_GET )
	{ 
		A **c = ( A **)b;
		*c = a;
	} 
}

template < class A >
void _GETSET_STRUCT( A *a, VO_PTR b, PARAMETER_OPTION op, COPY_FUNC pFunc)
{
	if( op == OPTION_SET ) 
	{ 

		DeepCopy<A> cp;
		cp.free( a );
		cp( a, (A*)b ); 
	} 
	else if (op == OPTION_GET )
	{ 
		if(pFunc)
			pFunc( b, a );
	} 
}




/*For general*/
#define VODS_PID_SHARELIB_GENERAL_BASE			(VO_DATASOURCE_PID_SHARELIB_BASE | 0X000A0000)	
#define VODS_PID_SHARELIB_GENERAL_LIBOP			(VODS_PID_SHARELIB_GENERAL_BASE  | 0X0001 )
#define VODS_PID_SHARELIB_GENERAL_LOGOP			(VODS_PID_SHARELIB_GENERAL_BASE  | 0X0002 )


/*FOR IO*/
#define VODS_PID_SHARELIB_IO_BASE				(VO_DATASOURCE_PID_SHARELIB_BASE | 0X000B0000)
#define VODS_PID_SHARELIB_IO_PTR				(VODS_PID_SHARELIB_IO_BASE | 0X0001 )
#define VODS_PID_SHARELIB_IO_HTTPHCB			(VODS_PID_SHARELIB_IO_BASE | 0X0002 )
#define VODS_PID_SHARELIB_IO_VERIFICATION		(VODS_PID_SHARELIB_IO_BASE | 0X0003)

/*for AdaptiveStreaming*/
#define VODS_PID_SHARELIB_AS_BASE				(VO_DATASOURCE_PID_SHARELIB_BASE | 0X000C0000)
#define VODS_PID_SHARELIB_AS_PROGRAMTYPE		(VODS_PID_SHARELIB_AS_BASE | 0X0001 )
#define VODS_PID_SHARELIB_AS_STREAMTYPE			(VODS_PID_SHARELIB_AS_BASE | 0X0002 )

#endif