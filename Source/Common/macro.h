#ifndef __MACRO_H__
#define __MACRO_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE{
#endif

#ifndef SAFE_DELETE
    #define SAFE_DELETE( x )            \
       if( x )                          \
       {                                \
           delete x;                    \
           x = NULL;                    \
       }
#endif //SAFE_DELETE


#ifndef SAFE_DELETE_ARRAY
    #define SAFE_DELETE_ARRAY( x )       \
       if( x )                          \
       {                                \
           delete [] x;                 \
           x = NULL;                    \
       }
#endif //SAFE_DELETE_ARRAY

#ifdef _VONAMESPACE
}
#endif

#endif //__MACRO_H__
