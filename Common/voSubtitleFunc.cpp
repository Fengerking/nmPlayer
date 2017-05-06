#include "voSubtitleFunc.h"
#include "stdlib.h"
#include "string.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

bool isequal_subtitletextinfolist( pvoSubtitleTextInfoEntry ptr_a , pvoSubtitleTextInfoEntry ptr_b )
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
}
/********************************End Compare Subtitle Structure***********************************/

/**********************************For Copy Subtitle Structure************************************/
/*
pvoSubtitleTextInfoEntry create_subtitletextinfolist( pvoSubtitleTextInfoEntry ptr_info )
{
	pvoSubtitleTextInfoEntry ptr_obj = 0;
	pvoSubtitleTextInfoEntry ptr_pos = 0;

	while( ptr_info )
	{
		pvoSubtitleTextInfoEntry ptr_temp = (pvoSubtitleTextInfoEntry)malloc( sizeof(voSubtitleTextInfoEntry ) );
		memcpy( ptr_temp , ptr_info , sizeof( voSubtitleTextInfoEntry ) );

		if( ptr_info->nSize > 0 )
		{
			ptr_temp->pString = (VO_PBYTE)malloc( ptr_temp->nSize + 2 );
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

pvoSubtitleTextRowInfo create_subtitlerowinfolist( pvoSubtitleTextRowInfo ptr_info )
{
	pvoSubtitleTextRowInfo ptr_obj = 0;
	pvoSubtitleTextRowInfo ptr_pos = 0;

	while( ptr_info )
	{
		pvoSubtitleTextRowInfo ptr_temp = (pvoSubtitleTextRowInfo)malloc( sizeof( voSubtitleInfoEntry ) );

		memcpy( ptr_temp , ptr_info , sizeof(SubtitleTextRowInfo) );
		ptr_temp->pTextInfoEntry = create_subtitletextinfolist( ptr_info->pTextInfoEntry );
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

void cpy_subtitleimageinfodata( voSubtitleImageInfoData * ptr_dest , voSubtitleImageInfoData * ptr_src )
{
	if( ptr_src->nSize > 0 )
	{
		ptr_dest->pPicData = (VO_PBYTE)malloc( ptr_dest->nSize );
		memcpy( ptr_dest->pPicData , ptr_src->pPicData , ptr_src->nSize );
	}
	else
		ptr_dest->pPicData = 0;
}

pvoSubtitleImageInfo create_subtitleimageinfolist( pvoSubtitleImageInfo ptr_info )
{
	pvoSubtitleImageInfo ptr_obj = 0;
	pvoSubtitleImageInfo ptr_pos = 0;

	while( ptr_info )
	{
		pvoSubtitleImageInfo ptr_temp = (pvoSubtitleImageInfo)malloc( sizeof( voSubtitleImageInfo ) );
		memcpy( ptr_temp , ptr_info , sizeof( voSubtitleImageInfo ) );
		cpy_subtitleimageinfodata( &ptr_temp->stImageData , &ptr_info->stImageData );
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

void cpy_subtitledisplayinfo( voSubtitleDisplayInfo * ptr_dest , voSubtitleDisplayInfo * ptr_src )
{
	ptr_dest->pTextRowInfo = create_subtitlerowinfolist( ptr_src->pTextRowInfo );
	ptr_dest->stDispDescriptor = ptr_src->stDispDescriptor;
	ptr_dest->pImageInfo = create_subtitleimageinfolist( ptr_src->pImageInfo );
}

pvoSubtitleInfoEntry create_subtitleinfolist( pvoSubtitleInfoEntry ptr_list )
{
	pvoSubtitleInfoEntry ptr_obj = 0;
	pvoSubtitleInfoEntry ptr_pos = 0;

	while( ptr_list )
	{
		pvoSubtitleInfoEntry ptr_temp = (pvoSubtitleInfoEntry)malloc( sizeof(voSubtitleInfoEntry) );
		memcpy( ptr_temp , ptr_list , sizeof( voSubtitleInfoEntry ) );
		cpy_subtitledisplayinfo( &ptr_temp->stSubtitleDispInfo , &ptr_list->stSubtitleDispInfo );
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

voSubtitleInfo* create_subtitleinfo( const voSubtitleInfo* ptr_info )
{
	voSubtitleInfo* ptr_obj = (voSubtitleInfo*)malloc( sizeof( voSubtitleInfo ) );
	memcpy( ptr_obj , ptr_info , sizeof( voSubtitleInfo ) );
	ptr_obj->pSubtitleEntry = create_subtitleinfolist( ptr_info->pSubtitleEntry );

	return ptr_obj;
}
*/
/**********************************End For Copy Subtitle Structure************************************/


/**********************************For Destroy Subtitle Structure*************************************/
/*
void destroy_subtitletextinfolist( pvoSubtitleTextInfoEntry ptr_info )
{
	while( ptr_info )
	{
		pvoSubtitleTextInfoEntry ptr_temp = ptr_info;
		ptr_info = ptr_info->pNext;

		if(ptr_temp->pString)
			free( (VO_PBYTE)ptr_temp->pString );

		free( (VO_PBYTE)ptr_temp );
	}
}


void destroy_subtitlerowinfolist( pvoSubtitleTextRowInfo ptr_info )
{
	while( ptr_info )
	{
		pvoSubtitleTextRowInfo ptr_temp = ptr_info;
		ptr_info = ptr_info->pNext;

		destroy_subtitletextinfolist( ptr_temp->pTextInfoEntry );
		free( (VO_PBYTE)ptr_temp );

	}
}


void destroy_subtitleimageinfodata( voSubtitleImageInfoData * ptr_info )
{
	if(ptr_info->pPicData)
		free( (VO_PBYTE)ptr_info->pPicData );
}


void destroy_subtitleimageinfolist( pvoSubtitleImageInfo ptr_info )
{
	while( ptr_info )
	{
		pvoSubtitleImageInfo ptr_temp = ptr_info;
		ptr_info = ptr_info->pNext;

		destroy_subtitleimageinfodata( &ptr_temp->stImageData );
		free( (VO_PBYTE)ptr_temp );
	}
}


void destroy_subtitledisplayinfo( voSubtitleDisplayInfo * ptr_info )
{
	destroy_subtitlerowinfolist( ptr_info->pTextRowInfo );
	destroy_subtitleimageinfolist( ptr_info->pImageInfo );
}


void destroy_subtitleinfolist( pvoSubtitleInfoEntry ptr_list )
{
	while( ptr_list )
	{
		pvoSubtitleInfoEntry ptr_temp = ptr_list;
		ptr_list = ptr_list->pNext;

		destroy_subtitledisplayinfo( &ptr_temp->stSubtitleDispInfo );
		free( (VO_PBYTE)ptr_temp );
	}
}


void destroy_subtitleinfo( const voSubtitleInfo* ptr_info )
{
	destroy_subtitleinfolist( ptr_info->pSubtitleEntry );
	free( (VO_PBYTE)ptr_info );
}
*/
/**********************************End For Destroy Subtitle Structure*********************************/

	
#ifdef _VONAMESPACE
}
#endif
