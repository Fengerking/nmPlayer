

#ifndef __VO_VMAP_H__

#define __VO_VMAP_H__

#include "voVMAPParser.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


VO_U32 vo_vmap_open( VO_PTR * ptr_h , VO_TCHAR * ptr_workingpath );
VO_U32 vo_vmap_close( VO_PTR h );
VO_U32 vo_vmap_process ( VO_PTR h , VO_PBYTE ptr_buffer , VO_U32 size );
VO_U32 vo_vmap_get_contenturl( VO_PTR h , VO_CHAR * ptr_url );
VO_U32 vo_vmap_get_data( VO_PTR h , VOAdInfo** pptr_data );


#ifdef _VONAMESPACE
}
#endif

#endif