//*@@@+++@@@@******************************************************************
//
// Microsoft Windows Media
// Copyright (C) Microsoft Corporation. All rights reserved.
//
//*@@@---@@@@******************************************************************

#ifndef _WMSPECSTRING_CE_H_
#define _WMSPECSTRING_CE_H_

#ifndef __null
  #define __null
  #define __notnull
  #define __maybenull
  #define __reserved
  #define __readonly
  #define __notreadonly
  #define __maybereadonly
  #define __valid
  #define __novalid
  #define __maybevalid
  #define __checkReturn
  #define __readableTo(size)
  #define __writableTo(size)
  #define __typefix(ctype)        
  #define __deref
  #define __pre
  #define __post
  #define __exceptthat
  #define __override
  #ifndef __fallthrough
      #define __fallthrough
  #endif
  #define __callback
  #define __in
  #define __out
  #define __inout
  #define __opt
  #define __out_ecount(size)
  #define __in_ecount(size)
  #define __in_ecount_opt(size)
  #define __out_ecount_opt(size)
  #define __inout_ecount(size)  
  #define __out_bcount(size)
  #define __in_bcount(size)
  #define __inout_bcount(size)
  #define __format_string
  #define __blocksOn(resource)
#endif //__null

#ifndef __deref_out
  #define __deref_out
#endif

#ifndef __ecount
  #define __ecount(size)
#endif

#if (UNDER_CE && UNDER_CE < 600) || defined(LINUX)

#ifndef __allocator
  #define __allocator
  #define __in_bound                          
  #define __out_bound                         
  #define __bound                         
  #define __deref_out_bound                   
  #define __in_range(lb,ub)                   
  #define __out_range(lb,ub)                  
  #define __deref_out_range(lb,ub)            
#endif

#endif // UNDER_CE

#ifndef __bound
  #define __bound                         
#endif

#ifndef __field_ecount
  #define __field_ecount(size)
  #define __field_bcount(size)
  #define __field_xcount(size)
#endif

#ifndef __struct_bcount
  #define __struct_bcount(size)
  #define __struct_xcount(size)
#endif

#ifndef __field_bcount_opt
  #define __field_bcount_opt(size)
#endif

#endif //_WMSPECSTRING_CE_H_
