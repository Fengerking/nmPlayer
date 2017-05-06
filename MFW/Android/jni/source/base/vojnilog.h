/************************************************************************
	*																		*
	*		VisualOn, Inc. Confidential and Proprietary, 2003				*
	*																		*
	************************************************************************/
/*******************************************************************************
	File:		 vojnilog.h

	Contains:	 vojnilog haeder file

	Written by:	Tom Yu Wei

	Change History (most recent first):
	2010-03-18		Tom			Create file

*******************************************************************************/


#ifndef _VOJNI_LOG_H
#define _VOJNI_LOG_H

#ifndef _VOJNIDEBUG
#define LOG_JNINDEBUG 1
#define LOG_JNINOLOG 1
#else
#define LOG_JNINDEBUG 0
#define LOG_JNINOLOG 0
#endif

#ifdef _WIN32
#include "windows.h"

#pragma warning (disable : 4996)
#pragma warning (disable : 4003)
#elif defined _LINUX_ANDROID
#include <utils/Log.h>
#endif // _WIN32


#ifdef __cplusplus
extern "C" {
#endif

#define VOJNI_TAG  "VOJNI"



#ifdef _WIN32

#if  LOG_JNINOLOG
#define JNILOGW(...)
#else
#define JNILOGW(...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "!!!%s [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if LOG_JNINOLOG
#define JNILOGI(...)
#else
#define JNILOGI(...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "%s [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if  LOG_JNINOLOG
#define JNILOGE(...)
#else
#define JNILOGE(...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "!!!!!!%s [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if  LOG_JNINOLOG
#define JNILOGD(...)
#else
#define JNILOGD(...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "%s [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if LOG_JNINDEBUG || LOG_JNINOLOG
#define JNILOGV(...)
#else
#define JNILOGV(...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "%s [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if LOG_JNINOLOG
#define VOLOGW2(fmt, ...) 
#else
#define VOLOGW2(fmt, ...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "!!! "fmt" [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if  LOG_JNINOLOG
#define JNILOGI2(fmt, ...) 
#else
#define JNILOGI2(fmt, ...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, fmt" [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if  LOG_JNINOLOG
#define JNILOGE2(fmt, ...) 
#else
#define JNILOGE2(fmt, ...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, "!!!!!! "fmt" [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if  LOG_JNINOLOG
#define JNILOGD2(fmt, ...) 
#else
#define JNILOGD2(fmt, ...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, fmt" [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#if LOG_JNINDEBUG || LOG_JNINOLOG
#define JNILOGV2(fmt, ...) 
#else
#define JNILOGV2(fmt, ...) \
	{ \
	char		szLog[256]; \
	VO_TCHAR	wzLog[256]; \
	sprintf(szLog, fmt" [%s.%s:%d]\r\n", __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__); \
	MultiByteToWideChar (CP_ACP, 0, szLog, -1, wzLog, sizeof (wzLog)); \
	OutputDebugString (wzLog); \
}
#endif

#elif defined _LINUX_ANDROID

#ifndef JNILOGV
#if LOG_JNINDEBUG || LOG_JNINOLOG
#define JNILOGV(...)   ((void)0)
#else
#define JNILOGV(...) { (void)LOG(LOG_VERBOSE, VOJNI_TAG,  "%s [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif



#ifndef JNILOGV2
#if LOG_JNINDEBUG || LOG_JNINOLOG
#define JNILOGV2(...)   ((void)0)
#else
#define JNILOGV2(format, ...) { (void)LOG(LOG_VERBOSE, VOJNI_TAG,  format" [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif


#ifndef JNILOGD 
#if LOG_JNINOLOG
#define JNILOGD(...)   ((void)0)
#else
#define JNILOGD(...) { (void)LOG(LOG_DEBUG, VOJNI_TAG,  "%s [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif


#ifndef JNILOGD2
#if LOG_JNINOLOG
#define JNILOGD2(format, ...)   ((void)0)
#else
#define JNILOGD2(format, ...) { (void)LOG(LOG_DEBUG, VOJNI_TAG,  format" [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif

#ifndef JNILOGI
#if LOG_JNINOLOG
#define JNILOGI(...)   ((void)0)
#else
#define JNILOGI(...) { (void)LOG(LOG_INFO, VOJNI_TAG,  "%s [%s.%s:%d]" ,  __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif


#ifndef JNILOGI2
#if LOG_JNINOLOG
#define JNILOGI2(format, ...)   ((void)0)
#else
#define JNILOGI2(format, ...) { (void)LOG(LOG_INFO, VOJNI_TAG,  format" [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif


#ifndef JNILOGW
#if LOG_JNINOLOG
#define JNILOGW(...)   ((void)0)
#else
#define JNILOGW(...) { (void)LOG(LOG_WARN, VOJNI_TAG,  "%s [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif

#ifndef JNILOGW2
#if LOG_JNINOLOG
#define JNILOGW2(format, ...)   ((void)0)
#else
#define JNILOGW2(format,...) { (void)LOG(LOG_WARN, VOJNI_TAG,  format" [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif


#ifndef JNILOGE
#if LOG_JNINOLOG
#define JNILOGE(...)   ((void)0)
#else
#define JNILOGE(...) { (void)LOG(LOG_ERROR, VOJNI_TAG,  "%s [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif

#ifndef JNILOGE2
#if LOG_JNINOLOG
#define JNILOGE2(format, ...)   ((void)0)
#else
#define JNILOGE2(format, ...) { (void)LOG(LOG_ERROR, VOJNI_TAG,  format" [%s.%s:%d]" , __VA_ARGS__, LOG_TAG, __FUNCTION__, __LINE__);}
#endif
#endif

#endif

// just for convenient
#define VOINFO(format, ...) do { \
	LOGI("%s::%s()->%d: " format, __FUNCTION__, __LINE__, ##__VA_ARGS__); } while(0);

#ifdef __cplusplus
}
#endif

#endif // _VOJNI_LOG_H


