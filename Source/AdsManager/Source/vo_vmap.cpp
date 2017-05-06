
#include "vo_vmap.h"
#include "voSource2.h"
#include "CDllLoad.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


struct VO_VMAP_INFO 
{
	CDllLoad loader;

	VO_HANDLE h_vmap;
	VO_VMAP_PARSER_API api;
};


VO_U32 vo_vmap_open( VO_PTR * ptr_h , VO_TCHAR * ptr_workingpath )
{
	VO_VMAP_INFO * ptr_info = new VO_VMAP_INFO;

	ptr_info->h_vmap = 0;
	memset( &(ptr_info->api) , 0 , sizeof(VO_VMAP_PARSER_API) );

	vostrcpy( ptr_info->loader.m_szDllFile , _T("voVMAPParser") );
	vostrcpy( ptr_info->loader.m_szAPIName , _T("voGetVMAPParserAPI") );

	ptr_info->loader.SetWorkPath( ptr_workingpath );

	if(ptr_info->loader.LoadLib(NULL) == 0)
		return VO_RET_SOURCE2_FAIL;

	VOGETVMAPPARSERAPI getapi = (VOGETVMAPPARSERAPI)ptr_info->loader.m_pAPIEntry;

	if( !getapi )
	{
		return VO_RET_SOURCE2_FAIL;
	}

	getapi( &(ptr_info->api) );

	VO_U32 ret = ptr_info->api.Open( &(ptr_info->h_vmap) , ptr_workingpath );

	if( ret == VO_RET_SOURCE2_OK )
		*ptr_h = ptr_info;

	return ret;
}

VO_U32 vo_vmap_close( VO_PTR h )
{
	VO_VMAP_INFO * ptr_info = ( VO_VMAP_INFO * ) h;

	if( !ptr_info )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = 0;

	if( !ptr_info->api.Close && ptr_info->h_vmap )
		ret = ptr_info->api.Close( ptr_info->h_vmap );

	delete ptr_info;

	return ret;
}

VO_U32 vo_vmap_process ( VO_PTR h , VO_PBYTE ptr_buffer , VO_U32 size )
{
	VO_VMAP_INFO * ptr_info = ( VO_VMAP_INFO * ) h;

	if( !ptr_info || !( ptr_info->api.Process ) || !( ptr_info->h_vmap ) )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_U32 ret = ptr_info->api.Process( ptr_info->h_vmap , ptr_buffer , size );

	return ret;
}

VO_U32 vo_vmap_get_contenturl( VO_PTR h , VO_CHAR * ptr_url )
{
	VO_VMAP_INFO * ptr_info = ( VO_VMAP_INFO * ) h;

	if( !ptr_info || !( ptr_info->api.GetParam ) || !( ptr_info->h_vmap ) )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	VO_CHAR * contenturl = 0;
	VO_U32 ret = ptr_info->api.GetParam( ptr_info->h_vmap , VO_PID_VMAP_CONTENT_URI , &contenturl );

	if( ret != VO_RET_SOURCE2_OK )
		return ret;

	strcpy( ptr_url , contenturl );

	return VO_RET_SOURCE2_OK;
}

VO_U32 vo_vmap_get_data( VO_PTR h , VOAdInfo** pptr_data )
{
	VO_VMAP_INFO * ptr_info = ( VO_VMAP_INFO * ) h;

	if( !ptr_info || !( ptr_info->api.GetData ) || !( ptr_info->h_vmap ) )
		return VO_RET_SOURCE2_EMPTYPOINTOR;

	return ptr_info->api.GetData( ptr_info->h_vmap , pptr_data );
}

#ifdef _VONAMESPACE
}
#endif