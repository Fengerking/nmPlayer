
#include "vo_ads_utility.h"
#include "CSourceIOUtility.h"
#include "CDllLoad.h"
#include "voSource2_IO.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif


typedef VO_VOID ( VO_API * pvoGetSourceIOAPI)( VO_SOURCE2_IO_API * ptr_api );

VO_U32 vo_download_by_url( VO_CHAR * ptr_url , VO_BYTE ** pptr_data , VO_U32 * ptr_size , VO_TCHAR * ptr_workingpath , VO_BOOL * ptr_is_stop )
{
	CDllLoad loader;
	vostrcpy( loader.m_szDllFile , _T("voSourceIO") );
	vostrcpy( loader.m_szAPIName , _T("voGetSourceIOAPI") );

	loader.SetWorkPath( ptr_workingpath );

	if(loader.LoadLib(NULL) == 0)
		return VO_RET_SOURCE2_FAIL;

	pvoGetSourceIOAPI getapi = (pvoGetSourceIOAPI)loader.m_pAPIEntry;

	if( !getapi )
	{
		return VO_RET_SOURCE2_FAIL;
	}

	VO_SOURCE2_IO_API api = {0};
	getapi( &api );

	VO_HANDLE h;

	voAutoIOInit init_obj( &api , ptr_url , &h );

	if( init_obj.m_ret != VO_SOURCE2_IO_OK )
		return init_obj.m_ret;

	voAutoIOOpen open_obj( &api , h , VO_TRUE );

	if( open_obj.m_ret != VO_SOURCE2_IO_OK )
		return open_obj.m_ret;

	VO_U32 ret = 0;

	VO_U64 size = 0;
	while( !*ptr_is_stop )
	{
		ret = api.GetSize( h , &size );

		if(VO_SOURCE2_IO_FAIL == ret)
			return ret;
		if(VO_SOURCE2_IO_OK == ret )
			break;
		voOS_Sleep(20);
	}

	(*pptr_data) = new VO_BYTE[ size ];
	*ptr_size = size;

	memset( (*pptr_data) , 0 ,  *ptr_size  );

	ret = ReadFixedSize( &api , h , *pptr_data , ptr_size , ptr_is_stop );

	return ret;
}

#ifdef _VONAMESPACE
}
#endif