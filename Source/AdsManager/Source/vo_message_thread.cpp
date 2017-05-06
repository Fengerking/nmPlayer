
#include "vo_message_thread.h"
#include "vo_thread.h"
#include "LIST_ENTRY.h"
#include "voCMutex.h"
#include "voCSemaphore.h"
#include "voAdsManager.h"

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

struct MESSAGE_INFO
{
	unsigned int msg_id;
	void * ptr_param1;
	void * ptr_param2;

	voCSemaphore * ptr_msg_signal;

	voLIST_ENTRY list;
};

MESSAGE_INFO * create_msg( unsigned int id , void * ptr_param1 , void * ptr_param2 , bool issync );
void destroy_msg( MESSAGE_INFO * ptr_msg );

struct MESSAGE_THREAD_INFO
{
	THREAD_HANDLE thread_handle;
	
	void * ptr_userdata;
	pMessageFunc ptr_func;

	voCMutex msg_list_lock;
	voCSemaphore msg_list_sem;
	voLIST_ENTRY msg_list;
};

unsigned int threadfunc( void * ptr_obj );
void destroy_threadinfo( MESSAGE_THREAD_INFO * ptr_info );
void add_message( MESSAGE_THREAD_INFO * ptr_info , MESSAGE_INFO * ptr_msg );

unsigned int begin_message_thread( void ** ptr_threadhandle , pMessageFunc ptr_func , void * ptr_userdata )
{
	MESSAGE_THREAD_INFO * ptr_info = new MESSAGE_THREAD_INFO;

	ptr_info->thread_handle = 0;

	ptr_info->ptr_func = ptr_func;
	ptr_info->ptr_userdata = ptr_userdata;

	InitializeListHead( &(ptr_info->msg_list) );

	unsigned int thread_id = 0;
	create_thread( &( ptr_info->thread_handle ) , &thread_id , threadfunc , ptr_info , 0 );

	*ptr_threadhandle = ptr_info;

	return 0;
}

unsigned int end_message_thread( void * ptr_threadhandle )
{
	MESSAGE_THREAD_INFO * ptr_thread_info = ( MESSAGE_THREAD_INFO * ) ptr_threadhandle;

	send_message( ptr_threadhandle , VO_MESSAGE_EXIT , 0 , 0 );

	wait_thread_exit( ptr_thread_info->thread_handle );

	destroy_threadinfo( ptr_thread_info );

	delete ptr_thread_info;

	return 0;
}

unsigned int send_message( void * ptr_threadhandle , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 )
{
	MESSAGE_THREAD_INFO * ptr_thread_info = ( MESSAGE_THREAD_INFO * ) ptr_threadhandle;

	MESSAGE_INFO * ptr_info = create_msg( msg_id , ptr_param1 , ptr_param2 , true );

	{
		voCAutoLock lock( &(ptr_thread_info->msg_list_lock) );
		InsertTailList( &( ptr_thread_info->msg_list ) , &( ptr_info->list ) );
	}

	ptr_thread_info->msg_list_sem.Up();

	ptr_info->ptr_msg_signal->Down();

	destroy_msg( ptr_info );

	return 0;
}

unsigned int post_message( void * ptr_threadhandle , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 )
{
	MESSAGE_THREAD_INFO * ptr_thread_info = ( MESSAGE_THREAD_INFO * ) ptr_threadhandle;

	MESSAGE_INFO * ptr_info = create_msg( msg_id , ptr_param1 , ptr_param2 , false );

	{
		voCAutoLock lock( &(ptr_thread_info->msg_list_lock) );
		InsertTailList( &( ptr_thread_info->msg_list ) , &( ptr_info->list ) );
	}

	ptr_thread_info->msg_list_sem.Up();

	return 0;
}

MESSAGE_INFO * get_message( MESSAGE_THREAD_INFO * ptr_info )
{
	ptr_info->msg_list_sem.Down();

	voCAutoLock lock( &(ptr_info->msg_list_lock) );
	PvoLIST_ENTRY ptr_entry = RemoveHeadList( &( ptr_info->msg_list ) );

	MESSAGE_INFO * ptr_msg = GET_OBJECTPOINTER( ptr_entry , MESSAGE_INFO , list );

	return ptr_msg;
}

unsigned int dispatch_message( MESSAGE_THREAD_INFO * ptr_info , MESSAGE_INFO * ptr_msg )
{
	ptr_info->ptr_func( ptr_info->ptr_userdata , ptr_msg->msg_id , ptr_msg->ptr_param1 , ptr_msg->ptr_param2 );

	unsigned int ret = 0;

	if( ptr_msg->msg_id == VO_MESSAGE_EXIT )
		ret = 0xffffffff;

	if( ptr_msg->ptr_msg_signal )
		ptr_msg->ptr_msg_signal->Up();
	else
	{
		destroy_msg( ptr_msg );
	}

	return ret;
}

unsigned int threadfunc( void * ptr_obj )
{
	MESSAGE_THREAD_INFO * ptr_info = ( MESSAGE_THREAD_INFO * )ptr_obj;

	unsigned int ret = 0;

	while( ret != 0xffffffff )
	{
		MESSAGE_INFO * ptr_msg = get_message( ptr_info );
		ret = dispatch_message( ptr_info , ptr_msg );
	}

	exit_thread();

	return 0;
}

void destroy_threadinfo( MESSAGE_THREAD_INFO * ptr_info )
{
	voCAutoLock lock( &(ptr_info->msg_list_lock) );

	while( !IsListEmpty( &( ptr_info->msg_list ) ) )
	{
		PvoLIST_ENTRY ptr_entry = RemoveHeadList( &( ptr_info->msg_list ) );

		MESSAGE_INFO * ptr_msg = GET_OBJECTPOINTER( ptr_entry , MESSAGE_INFO , list );

		if( ptr_msg->ptr_msg_signal )
			ptr_msg->ptr_msg_signal->Up();
		else
			destroy_msg( ptr_msg );
	}
}

void add_message( MESSAGE_THREAD_INFO * ptr_info , MESSAGE_INFO * ptr_msg )
{
	voCAutoLock lock( &( ptr_info->msg_list_lock ) );

	/*if( ptr_msg->msg_id == VO_MESSAGE_EXIT )
	{
		InsertHeadList( &( ptr_info->msg_list ) , &( ptr_msg->list ) );
	}
	else*/
	{
		InsertTailList( &( ptr_info->msg_list ) , &( ptr_msg->list ) );
	}
}

MESSAGE_INFO * create_msg( unsigned int id , void * ptr_param1 , void * ptr_param2 , bool issync )
{
	MESSAGE_INFO * ptr_info = new MESSAGE_INFO;

	memset( ptr_info , 0 , sizeof( MESSAGE_INFO ) );

	ptr_info->msg_id = id;

	if( ptr_info->msg_id == VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE || 
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE )
	{
		ptr_info->ptr_param1 = new VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO;
		memcpy( ptr_info->ptr_param1 , ptr_param1 , sizeof( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO ) );
		ptr_info->ptr_param2 = ptr_param2;
		if( (( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * )ptr_info->ptr_param1)->ullElapsedTime & 0x8000000000000000ll )
			(( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * )ptr_info->ptr_param1)->ullElapsedTime = 0;
	}
	else if( ptr_info->msg_id == VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED )
	{
		ptr_info->ptr_param1 = new VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO;
		memcpy( ptr_info->ptr_param1 , ptr_param1 , sizeof( VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO ) );
		ptr_info->ptr_param2 = ptr_param2;
	}
	else if( ptr_info->msg_id == VO_ADSMANAGER_EVENT_TRACKING_ACTION||
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_CONTENT_START || 
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_CONTENT_END ||
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_AD_START ||
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_AD_END ||
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_WHOLECONTENT_START ||
		ptr_info->msg_id == VO_ADSMANAGER_EVENT_WHOLECONTENT_END  )
	{
		ptr_info->ptr_param1 = new VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO;
		memcpy( ptr_info->ptr_param1 , ptr_param1 , sizeof( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO ) );
		ptr_info->ptr_param2 = ptr_param2;
		if( (( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO * )ptr_info->ptr_param1)->ullElapsedTime & 0x8000000000000000ll )
			(( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO * )ptr_info->ptr_param1)->ullElapsedTime = 0;
	}
	else if( ptr_info->msg_id >= VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE && ptr_info->msg_id <= VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MAX )
	{
		VO_ADSMANAGER_TRACKINGEVENT_INFO * ptr_trackingevent_info = new VO_ADSMANAGER_TRACKINGEVENT_INFO;
		VO_ADSMANAGER_TRACKINGEVENT_INFO * ptr_org = ( VO_ADSMANAGER_TRACKINGEVENT_INFO * )ptr_param1;
		ptr_trackingevent_info->uPeriodID = ptr_org->uPeriodID;
		ptr_trackingevent_info->uUrlCount = ptr_org->uUrlCount;
		ptr_trackingevent_info->ppUrl = new VO_CHAR*[ptr_trackingevent_info->uUrlCount];
		for( VO_U32 i = 0 ; i < ptr_trackingevent_info->uUrlCount ; i++ )
		{
			ptr_trackingevent_info->ppUrl[i] = new VO_CHAR[MAXURLLEN];
			strcpy( ptr_trackingevent_info->ppUrl[i] , ptr_org->ppUrl[i] );
		}

		ptr_info->ptr_param1 = ptr_trackingevent_info;
		ptr_info->ptr_param2 = ptr_param2;

	}
	else
	{
		ptr_info->ptr_param1 = ptr_param1;
		ptr_info->ptr_param2 = ptr_param2;
	}

	if( issync )
		ptr_info->ptr_msg_signal = new voCSemaphore;

	return ptr_info;
}

void destroy_msg( MESSAGE_INFO * ptr_msg )
{
	if( ptr_msg->msg_id == VO_ADSMANAGER_EVENT_TRACKING_WHOLECONTENTPRECENTAGE || 
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_TRACKING_PERIODPRECENTAGE )
	{
		VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRACKING_PERCENTAGEINFO * ) ptr_msg->ptr_param1;
		delete ptr_info;
	}

	if( ptr_msg->msg_id == VO_ADSMANAGER_EVENT_TRACKING_ACTION||
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_CONTENT_START || 
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_CONTENT_END ||
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_AD_START ||
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_AD_END ||
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_WHOLECONTENT_START ||
		ptr_msg->msg_id == VO_ADSMANAGER_EVENT_WHOLECONTENT_END )
	{
		VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRAKCING_ACTIONINFO * ) ptr_msg->ptr_param1;
		delete ptr_info;
	}

	if( ptr_msg->msg_id == VO_ADSMANAGER_EVENT_TRACKING_TIMEPASSED )
	{
		VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO * ptr_info = ( VO_ADSMANAGER_EVENT_TRAKCING_TIMEPASSEDINFO * ) ptr_msg->ptr_param1;
		delete ptr_info;
	}
	
	if( ptr_msg->msg_id >= VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_BASE && ptr_msg->msg_id <= VO_ADSMANAGER_EVENT_TRACKING_TRACKINGEVENTS_MAX )
	{
		VO_ADSMANAGER_TRACKINGEVENT_INFO * ptr_item = ( VO_ADSMANAGER_TRACKINGEVENT_INFO * )ptr_msg->ptr_param1;
		for( VO_U32 i = 0 ; i < ptr_item->uUrlCount ; i++ )
		{
			delete []ptr_item->ppUrl[i];
		}

		delete []ptr_item;
	}

	if( ptr_msg->ptr_msg_signal )
		delete ptr_msg->ptr_msg_signal;

	delete ptr_msg;
}

#ifdef _VONAMESPACE
}
#endif