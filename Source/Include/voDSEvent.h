
//value indicate for DATA SOURCE TEAM 
#ifndef __VO_DSEVENT_H__
#define __VO_DSEVENT_H__

//common event callback

typedef struct
{
//callback instance
	VO_PTR pUserData;
/**
 * The source will notify client via this function for some events.
 * \param pUserData [in] The user data which was set by Open().
 * \param nID [in] The status type.
 * \param nParam1 [in] status specific parameter 1.
 * \param nParam2 [in] status specific parameter 2.
 */
	VO_S32 (VO_API * SendEvent) (VO_PTR pUserData, VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2);

}VO_DATASOURCE_CALLBACK;//this is a sync callback func, not aync.


//BASE event id
#define VO_DATASOURCE_EVENTID_BASE								0xDA000000
#define VO_DATASOURCE_EVENTID_ASP_BASE							( VO_DATASOURCE_EVENTID_BASE | 0X00100000 )
#define VO_DATASOURCE_EVENTID_POP_BASE							( VO_DATASOURCE_EVENTID_BASE | 0X00200000 )
#define VO_DATASOURCE_EVENTID_IO_BASE							( VO_DATASOURCE_EVENTID_BASE | 0X00300000 )

//PROGRAMINFO OPERATOR EVENT ID
#define VO_DATASOURCE_EVENTID_POP_PROGRAMINFOCHANGED			(VO_DATASOURCE_EVENTID_POP_BASE | 0X00000001) //indicate that programinfo has been changed in programinfo operator.
#define VO_DATASOURCE_EVENTID_POP_PROGRAMINFORESET				(VO_DATASOURCE_EVENTID_POP_BASE | 0X00000002) //indicate that programinfo has been reseted in programinfo operator.
#define VO_DATASOURCE_EVENTID_POP_NEWPROGRAM					(VO_DATASOURCE_EVENTID_POP_BASE | 0X00000003)
//ADAPTIVESTREAMING EVENT ID
#define VO_DATASOURCE_EVENTID_ASP_PROGRAMINFOCHANGED			(VO_DATASOURCE_EVENTID_ASP_BASE | 0X00000001) //indicate that programinfo has been changed in adaptiveStreaming parser.
//SOURCEIO EVENT ID
#define VO_DATASOURCE_EVENTID_IO_DOWNLOADSLOW					(VO_DATASOURCE_EVENTID_IO_BASE  | 0X00000001) //indicate that download slow 
#endif