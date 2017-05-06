#include "voSource2SourceOther.h"
#include "assert.h"


typedef VO_S32 ( VO_API * pvoGetAPI) (VO_SOURCE2_API * pReadHandle, VO_U32 uFlag);

voSource2SourceOther::voSource2SourceOther( const VO_TCHAR * str_modulename , const VO_TCHAR * str_apiname )
:voSource2Source( str_modulename , str_apiname )
{
	m_loader.LoadLib(NULL);

	pvoGetAPI get_api = ( pvoGetAPI )m_loader.m_pAPIEntry;
	assert( get_api != 0 );

	get_api( &m_api , 0 );
	assert( m_api.Init != 0 );
}

voSource2SourceOther::~voSource2SourceOther(void)
{
}

VO_U32 voSource2SourceOther::Init ( VO_PTR pSource , VO_U32 nFlag , VO_SOURCE2_INITPARAM * pInitParam )
{
	unsigned short temp[2048];
	VO_CHAR * ptr_url = ( VO_CHAR * )pSource;

	unsigned short * ptr = temp;

	while( *ptr_url )
	{
		*ptr = *ptr_url;
		ptr_url++;
		ptr++;
	}

	*ptr = 0;

	return voSource2Source::Init( temp , nFlag , pInitParam );
}
