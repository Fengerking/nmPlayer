#ifndef __MACRO_H__
#define __MACRO_H__


#ifndef SAFE_DELETE
    #define SAFE_DELETE( x )            \
       if( x )                          \
       {                                \
           delete x;                    \
		   TRACE_DELETE(x);				\
           x = NULL;                    \
       }
#endif //SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY( x )       \
       if( x )                          \
       {                                \
           delete [] x;                 \
		   TRACE_DELETE(x);				\
           x = NULL;                    \
       }
#endif //SAFE_DELETE_ARRAY



#ifndef FCC
#define FCC(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4) & 0xFF00) << 8) |    \
	(((DWORD)(ch4) & 0xFF0000) >> 8) |  \
	(((DWORD)(ch4) & 0xFF000000) >> 24))
#endif	//FCC


//#ifdef _VONAMESPACE
//namespace _VONAMESPACE{
//#endif
//
//#ifdef _VONAMESPACE
//}
//#endif


#endif //__MACRO_H__