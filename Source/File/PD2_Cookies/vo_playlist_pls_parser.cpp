#include "vo_playlist_pls_parser.h"
#include "voLog.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

vo_playlist_pls_parser::vo_playlist_pls_parser(void)
:m_playlist_items_head(0)
,m_playlist_items_tail(0)
{
}

vo_playlist_pls_parser::~vo_playlist_pls_parser(void)
{
}


VO_BOOL vo_playlist_pls_parser::get_url(VO_S32 index, VO_PCHAR strurl)
{
	PLS_MEDIA_ITEM * ptr_item = getitem_by_sequencebnumber( index );
	if(ptr_item->path)
	{
		strcpy(strurl, ptr_item->path);
		return VO_TRUE;
	}
	else
	{
		return VO_FALSE;
	}
}

VO_BOOL vo_playlist_pls_parser::parse()
{
	VO_CHAR str[2048];
	VO_BOOL suc = VO_FALSE;

	memset( str , 0 , 2048 );
	suc = read_line( (VO_PBYTE)str);

	VOLOGI("the first line read_line is: %s", str);
	
	if( memcmp( str , "[playlist]" , strlen("[playlist]")) ||  !suc)
		return VO_FALSE;

	while( suc )
	{
		memset( str , 0 , 2048 );
		suc = read_line( (VO_PBYTE)str);

		if( memcmp( str , "NumberOfEntries" , strlen("NumberOfEntries") ) == 0 )
		{
			;
		}
		else if( memcmp( str , "Version" , strlen("Version") ) == 0 )
		{
			;
		}
		else if( memcmp( str , "File" , strlen("File") ) == 0 )
		{
			VO_S32 sequence_number;
			sscanf( str , "File%ld=" , &sequence_number );
			PLS_MEDIA_ITEM * ptr_item = getitem_by_sequencebnumber( sequence_number );

			VO_PCHAR ptr = strchr( str , '=' );
			ptr++;

			strcpy( ptr_item->path , ptr );
		}
		else if( memcmp( str , "Title" , strlen("Title") ) == 0 )
		{
			VO_S32 sequence_number;
			sscanf( str , "Title%ld=" , &sequence_number );
			PLS_MEDIA_ITEM * ptr_item = getitem_by_sequencebnumber( sequence_number );

			VO_PCHAR ptr = strchr( str , '=' );
			ptr++;

			strcpy( ptr_item->title , ptr );
		}
		else if( memcmp( str , "Length" , strlen("Length") ) == 0 )
		{
			VO_S32 sequence_number;
			sscanf( str , "Length%ld=" , &sequence_number );
			PLS_MEDIA_ITEM * ptr_item = getitem_by_sequencebnumber( sequence_number );

			VO_PCHAR ptr = strchr( str , '=' );
			ptr++;

			sscanf( ptr , "%ld" , &(ptr_item->duration) );
		}
	}

	return VO_TRUE;
}

PLS_MEDIA_ITEM * vo_playlist_pls_parser::getitem_by_sequencebnumber( VO_S32 sequence_number )
{
	PLS_MEDIA_ITEM * ptr_item = m_playlist_items_head;

	while( ptr_item )
	{
		if( ptr_item->sequence_number == sequence_number )
			return ptr_item;

		ptr_item = ptr_item->ptr_next;
	}

	ptr_item = new PLS_MEDIA_ITEM;
	if( !ptr_item )
	{
		VOLOGE( "new obj failed" );
		return 0;
	}
	memset( ptr_item , 0 , sizeof( PLS_MEDIA_ITEM ) );
	ptr_item->sequence_number = sequence_number;

	if( m_playlist_items_head == 0 && m_playlist_items_tail == 0 )
		m_playlist_items_head = m_playlist_items_tail = ptr_item;
	else
	{
		m_playlist_items_tail->ptr_next = ptr_item;
		m_playlist_items_tail = ptr_item;
	}

	return ptr_item;
}

VO_VOID vo_playlist_pls_parser::print()
{
	PLS_MEDIA_ITEM * ptr_item = m_playlist_items_head;

	while( ptr_item )
	{
		printf( "\n\n" );
		printf( "Sequence Number: %ld\n" , ptr_item->sequence_number );
		printf( "File Path: %s\n" , ptr_item->path );
		printf( "Title: %s\n" , ptr_item->title );

		ptr_item = ptr_item->ptr_next;
	}
}
