

#ifndef __VO_MESSAGE_THREAD_H__


#define __VO_MESSAGE_THREAD_H__

#define VO_MESSAGE_BASE 0x23000000
#define VO_MESSAGE_EXIT ( VO_MESSAGE_BASE | 0X00000001 )

#define VO_MESSAGE_USER ( VO_MESSAGE_BASE | 0X00100000 )

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

typedef unsigned int ( * pMessageFunc )( void * ptr_userdata , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 );

unsigned int begin_message_thread( void ** ptr_threadhandle , pMessageFunc ptr_func , void * ptr_userdata );
unsigned int end_message_thread( void * ptr_threadhandle );

unsigned int send_message( void * ptr_threadhandle , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 );
unsigned int post_message( void * ptr_threadhandle , unsigned int msg_id , void * ptr_param1 , void * ptr_param2 );

#ifdef _VONAMESPACE
}
#endif

#endif