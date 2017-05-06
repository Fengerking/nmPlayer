#ifndef __MACRO_H__
#define __MACRO_H__


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



#endif //__MACRO_H__