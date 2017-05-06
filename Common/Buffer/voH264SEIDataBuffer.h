#pragma once

#include "voVideoParser.h"
#include "vo_doublelink_list.hpp"
#include "voSource2.h"
#include "voCMutex.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voH264SEIDataBuffer
{
public:
	voH264SEIDataBuffer(int nCount);
	virtual ~voH264SEIDataBuffer();

	virtual VO_S32		AddBuffer (VO_PARSER_SEI_INFO * pBuffer);
	virtual VO_S32		GetBuffer (VO_PARSER_SEI_INFO * pBuffer);

	virtual VO_U64		GetBuffTime ();
	virtual VO_S32		GetBuffCount ();

	virtual VO_S32		Flush ();

	virtual VO_S32		RemoveTo( VO_U64 timestamp );

protected:
	virtual vo_doublelink_list< VO_PARSER_SEI_INFO* >::iterator FindNearestItem( VO_U64 time );

	VO_PARSER_SEI_INFO* CreateSEIInfo( const VO_PARSER_SEI_INFO* ptr_info);

	int					DestrpySEIInfo( const VO_PARSER_SEI_INFO* ptr_info);

protected:
	vo_doublelink_list< VO_PARSER_SEI_INFO* > m_list;

	VO_PARSER_SEI_INFO * m_ptr_cache;

	int					m_nSETCount;

	voCMutex			m_lock;
};

class TimeStampComp
{
public:
	int operator()( VO_PARSER_SEI_INFO* ptr_sample1 , VO_PARSER_SEI_INFO* ptr_sample2 )
	{
		return (int)(ptr_sample1->time - ptr_sample2->time);
	}
};
    
#ifdef _VONAMESPACE
}
#endif
