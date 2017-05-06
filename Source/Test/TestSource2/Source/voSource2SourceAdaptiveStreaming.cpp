#include "voSource2SourceAdaptiveStreaming.h"
#include "assert.h"

typedef VO_S32 ( VO_API * pvoGetAPI)( VO_SOURCE2_API* pHandle );

voSource2SourceAdaptiveStreaming::voSource2SourceAdaptiveStreaming( const VO_TCHAR * str_modulename , const VO_TCHAR * str_apiname )
:voSource2Source( str_modulename , str_apiname )
{
	m_loader.LoadLib(NULL);

	pvoGetAPI get_api = ( pvoGetAPI )m_loader.m_pAPIEntry;
	assert( get_api != 0 );

	get_api( &m_api );
	assert( m_api.Init != 0 );
}

voSource2SourceAdaptiveStreaming::~voSource2SourceAdaptiveStreaming(void)
{
}
