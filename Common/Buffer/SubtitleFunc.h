
#ifndef __SUBTITLEFUNC_H__

#define __SUBTITLEFUNC_H__

#include "voSubtitleType.h"
#include "vo_allocator.hpp"
#include "string.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

/********************************For Compare Subtitle Structure***********************************/
/*bool isequal_subtitletextinfolist( pvoSubtitleTextInfoEntry ptr_a , pvoSubtitleTextInfoEntry ptr_b )
{
	while( ptr_a && ptr_b )
	{
		if( ptr_a->nSize != ptr_b->nSize )
			return false;

		if( ptr_a->nSize > 0 )
		{
			if( memcmp( ptr_a->pString , ptr_b->pString , ptr_a->nSize ) != 0 )
				return false;
		}

		if( memcmp( &ptr_a->stStringInfo , &ptr_b->stStringInfo , sizeof( voSubtitleStringInfo ) ) != 0 )
			return false;

		ptr_a = ptr_a->pNext;
		ptr_b = ptr_b->pNext;
	}

	if( ptr_a == NULL && ptr_b == NULL )
		return true;

	return false;
}

bool isequal_subtitlerowinfolist( pvoSubtitleTextRowInfo ptr_a , pvoSubtitleTextRowInfo ptr_b )
{
	while( ptr_a && ptr_b )
	{
		if( memcmp( &ptr_a->stTextRowDes , &ptr_b->stTextRowDes , sizeof( voSubtitleTextRowDescriptor ) ) != 0 )
			return false;

		if( !isequal_subtitletextinfolist( ptr_a->pTextInfoEntry , ptr_b->pTextInfoEntry ) )
			return false;

		ptr_a = ptr_a->pNext;
		ptr_b = ptr_b->pNext;
	}

	if( ptr_a == NULL && ptr_b == NULL )
		return true;

	return false;
}

bool isequal_subtitleimageinfodata( voSubtitleImageInfoData * ptr_a , voSubtitleImageInfoData * ptr_b )
{
	if( ptr_a->nSize != ptr_b->nSize )
		return false;

	if( ptr_a->nSize > 0 )
	{
		if( memcmp( ptr_a->pPicData , ptr_b->pPicData , ptr_a->nSize ) != 0 )
			return false;
	}

	return true;
}

bool isequal_subtitleimageinfolist( pvoSubtitleImageInfo ptr_a , pvoSubtitleImageInfo ptr_b )
{
	while( ptr_a && ptr_b )
	{
		if( memcpy( &ptr_a->stImageDesp , &ptr_b->stImageDesp , sizeof( voSubtitleImageInfoDescriptor ) ) != 0 )
			return false;

		if( !isequal_subtitleimageinfodata( &ptr_a->stImageData , &ptr_b->stImageData ) )
			return false;

		ptr_a = ptr_a->pNext;
		ptr_b = ptr_b->pNext;
	}

	if( ptr_a == NULL && ptr_b == NULL )
		return true;

	return false;
}

bool isequal_subtitledisplayinfo( voSubtitleDisplayInfo * ptr_a , voSubtitleDisplayInfo * ptr_b )
{
	if( !isequal_subtitlerowinfolist( ptr_a->pTextRowInfo , ptr_b->pTextRowInfo ) )
		return false;

	if( memcmp( &ptr_a->stDispDescriptor , &ptr_b->stDispDescriptor , sizeof( voSubtitleTextDisplayDescriptor ) ) != 0 )
		return false;

	if( !isequal_subtitleimageinfolist( ptr_a->pImageInfo , ptr_a->pImageInfo ) )
		return false;

	return true;
}

bool isequal_subtitleinfolist( pvoSubtitleInfoEntry ptr_a , pvoSubtitleInfoEntry ptr_b )
{
	while( ptr_a && ptr_b )
	{
		if( memcmp( ptr_a , ptr_b , sizeof( voSubtitleInfoEntry ) - sizeof( SubtitleInfoEntry* ) - sizeof( voSubtitleDisplayInfo ) ) != 0)
			return false;

		if( !isequal_subtitledisplayinfo( &ptr_a->stSubtitleDispInfo , &ptr_b->stSubtitleDispInfo ) )
			return false;

		ptr_a = ptr_a->pNext;
		ptr_b = ptr_b->pNext;
	}

	if( ptr_a == NULL && ptr_b == NULL )
		return true;

	return false;
}

bool isequal_subtitleinfo( const voSubtitleInfo* ptr_a , const voSubtitleInfo* ptr_b )
{
	return isequal_subtitleinfolist( ptr_a->pSubtitleEntry , ptr_b->pSubtitleEntry );
}*/
/********************************End Compare Subtitle Structure***********************************/

/**********************************For Copy Subtitle Structure************************************/

template< class MEM >
pvoSubtitleTextInfoEntry create_subtitletextinfolist( pvoSubtitleTextInfoEntry ptr_info , MEM& alloc )
{
	pvoSubtitleTextInfoEntry ptr_obj = 0;
	pvoSubtitleTextInfoEntry ptr_pos = 0;

	while( ptr_info )
	{
		pvoSubtitleTextInfoEntry ptr_temp = (pvoSubtitleTextInfoEntry)alloc.allocate( sizeof(voSubtitleTextInfoEntry ) );
		memcpy( ptr_temp , ptr_info , sizeof( voSubtitleTextInfoEntry ) );

		if( ptr_info->nSize > 0 && ptr_info->pString != NULL)
		{
			ptr_temp->pString = alloc.allocate( ptr_temp->nSize + 2 );
			memset( ptr_temp->pString , 0 , ptr_temp->nSize + 2 );
			memcpy( ptr_temp->pString , ptr_info->pString , ptr_temp->nSize );
		}
		else
			ptr_temp->pString = 0;

		ptr_temp->pNext = 0;

		if( ptr_obj == 0 )
			ptr_obj = ptr_pos = ptr_temp;
		else
		{
			ptr_pos->pNext = ptr_temp;
			ptr_pos = ptr_temp;
		}

		ptr_info = ptr_info->pNext;
	}

	return ptr_obj;
}

template< class MEM >
pvoSubtitleTextRowInfo create_subtitlerowinfolist( pvoSubtitleTextRowInfo ptr_info , MEM& alloc )
{
	pvoSubtitleTextRowInfo ptr_obj = 0;
	pvoSubtitleTextRowInfo ptr_pos = 0;

	while( ptr_info )
	{
		pvoSubtitleTextRowInfo ptr_temp = (pvoSubtitleTextRowInfo)alloc.allocate( sizeof( voSubtitleInfoEntry ) );

		memcpy( ptr_temp , ptr_info , sizeof(SubtitleTextRowInfo) );
		if(ptr_info->pTextInfoEntry)
			ptr_temp->pTextInfoEntry = create_subtitletextinfolist( ptr_info->pTextInfoEntry , alloc );
		else
			ptr_temp->pTextInfoEntry = NULL;

		ptr_temp->pNext = 0;

		if( ptr_obj == 0 )
			ptr_obj = ptr_pos = ptr_temp;
		else
		{
			ptr_pos->pNext = ptr_temp;
			ptr_pos = ptr_temp;
		}

		ptr_info = ptr_info->pNext;
	}

	return ptr_obj;
}

template< class MEM >
void cpy_subtitleimageinfodata( voSubtitleImageInfoData * ptr_dest , voSubtitleImageInfoData * ptr_src , MEM& alloc )
{
	if( ptr_src->nSize > 0 && ptr_src->pPicData != NULL)
	{
		ptr_dest->pPicData = alloc.allocate( ptr_dest->nSize );
		memcpy( ptr_dest->pPicData , ptr_src->pPicData , ptr_src->nSize );
	}
	else
		ptr_dest->pPicData = 0;
}

template< class MEM >
pvoSubtitleImageInfo create_subtitleimageinfolist( pvoSubtitleImageInfo ptr_info , MEM& alloc )
{
	pvoSubtitleImageInfo ptr_obj = 0;
	pvoSubtitleImageInfo ptr_pos = 0;

	while( ptr_info )
	{
		pvoSubtitleImageInfo ptr_temp = (pvoSubtitleImageInfo)alloc.allocate( sizeof( voSubtitleImageInfo ) );
		memcpy( ptr_temp , ptr_info , sizeof( voSubtitleImageInfo ) );
		cpy_subtitleimageinfodata( &ptr_temp->stImageData , &ptr_info->stImageData , alloc );
		ptr_temp->pNext = 0;

		if( ptr_obj == 0 )
			ptr_obj = ptr_pos = ptr_temp;
		else
		{
			ptr_pos->pNext = ptr_temp;
			ptr_pos = ptr_temp;
		}

		ptr_info = ptr_info->pNext;
	}

	return ptr_obj;
}

template< class MEM >
void cpy_subtitledisplayinfo( voSubtitleDisplayInfo * ptr_dest , voSubtitleDisplayInfo * ptr_src , MEM& alloc )
{
	if(ptr_src->pTextRowInfo)
		ptr_dest->pTextRowInfo = create_subtitlerowinfolist( ptr_src->pTextRowInfo , alloc );
	else
		ptr_dest->pTextRowInfo = NULL;

	ptr_dest->stDispDescriptor = ptr_src->stDispDescriptor;

	if(ptr_src->pImageInfo)
		ptr_dest->pImageInfo = create_subtitleimageinfolist( ptr_src->pImageInfo , alloc );
	else
		ptr_dest->pImageInfo = NULL;
}

template< class MEM >
pvoSubtitleInfoEntry create_subtitleinfolist( pvoSubtitleInfoEntry ptr_list , MEM& alloc )
{
	pvoSubtitleInfoEntry ptr_obj = 0;
	pvoSubtitleInfoEntry ptr_pos = 0;

	while( ptr_list )
	{
		pvoSubtitleInfoEntry ptr_temp = (pvoSubtitleInfoEntry)alloc.allocate( sizeof(voSubtitleInfoEntry) );
		memcpy( ptr_temp , ptr_list , sizeof( voSubtitleInfoEntry ) );
		cpy_subtitledisplayinfo( &ptr_temp->stSubtitleDispInfo , &ptr_list->stSubtitleDispInfo , alloc );
		ptr_temp->pNext = 0;

		if( ptr_obj == 0 )
			ptr_obj = ptr_pos = ptr_temp;
		else
		{
			ptr_pos->pNext = ptr_temp;
			ptr_pos = ptr_temp;
		}

		ptr_list = ptr_list->pNext;
	}

	return ptr_obj;
}

template< class MEM >
voSubtitleInfo* create_subtitleinfo( const voSubtitleInfo* ptr_info , MEM& alloc )
{
	voSubtitleInfo* ptr_obj = (voSubtitleInfo*)alloc.allocate( sizeof( voSubtitleInfo ) );
	memcpy( ptr_obj , ptr_info , sizeof( voSubtitleInfo ) );
	if(ptr_info->pSubtitleEntry)
		ptr_obj->pSubtitleEntry = create_subtitleinfolist( ptr_info->pSubtitleEntry , alloc );
	else
		ptr_obj->pSubtitleEntry = NULL;

	return ptr_obj;
}

/**********************************End For Copy Subtitle Structure************************************/


/**********************************For Destroy Subtitle Structure*************************************/

template< class MEM >
void destroy_subtitletextinfolist( pvoSubtitleTextInfoEntry ptr_info , MEM& alloc )
{
	while( ptr_info )
	{
		pvoSubtitleTextInfoEntry ptr_temp = ptr_info;
		ptr_info = ptr_info->pNext;

		if(ptr_temp->pString)
			alloc.deallocate( (VO_PBYTE)ptr_temp->pString );

		alloc.deallocate( (VO_PBYTE)ptr_temp );
	}
}

template< class MEM >
void destroy_subtitlerowinfolist( pvoSubtitleTextRowInfo ptr_info , MEM& alloc )
{
	while( ptr_info )
	{
		pvoSubtitleTextRowInfo ptr_temp = ptr_info;
		ptr_info = ptr_info->pNext;

		destroy_subtitletextinfolist( ptr_temp->pTextInfoEntry , alloc );
		alloc.deallocate( (VO_PBYTE)ptr_temp );

	}
}

template< class MEM >
void destroy_subtitleimageinfodata( voSubtitleImageInfoData * ptr_info , MEM& alloc )
{
	if(ptr_info->pPicData)
		alloc.deallocate( (VO_PBYTE)ptr_info->pPicData );
}

template< class MEM >
void destroy_subtitleimageinfolist( pvoSubtitleImageInfo ptr_info , MEM& alloc )
{
	while( ptr_info )
	{
		pvoSubtitleImageInfo ptr_temp = ptr_info;
		ptr_info = ptr_info->pNext;

		destroy_subtitleimageinfodata( &ptr_temp->stImageData , alloc );
		alloc.deallocate( (VO_PBYTE)ptr_temp );
	}
}

template< class MEM >
void destroy_subtitledisplayinfo( voSubtitleDisplayInfo * ptr_info , MEM& alloc )
{
	destroy_subtitlerowinfolist( ptr_info->pTextRowInfo , alloc );
	destroy_subtitleimageinfolist( ptr_info->pImageInfo , alloc );
}

template< class MEM >
void destroy_subtitleinfolist( pvoSubtitleInfoEntry ptr_list , MEM& alloc )
{
	while( ptr_list )
	{
		pvoSubtitleInfoEntry ptr_temp = ptr_list;
		ptr_list = ptr_list->pNext;

		destroy_subtitledisplayinfo( &ptr_temp->stSubtitleDispInfo , alloc );
		alloc.deallocate( (VO_PBYTE)ptr_temp );
	}
}

template< class MEM >
void destroy_subtitleinfo( const voSubtitleInfo* ptr_info , MEM& alloc )
{
	destroy_subtitleinfolist( ptr_info->pSubtitleEntry , alloc );
	alloc.deallocate( (VO_PBYTE)ptr_info );
}
/**********************************End For Destroy Subtitle Structure*********************************/
    
#ifdef _VONAMESPACE
}
#endif

#endif