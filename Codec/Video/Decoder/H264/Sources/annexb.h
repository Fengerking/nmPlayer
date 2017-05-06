
/*!
 *************************************************************************************
 * \file annexb.h
 *
It is used to store some global infomation now
 Number Huang
 2010-07-08
 *
 *************************************************************************************
 */

#ifndef _ANNEXB_H_
#define _ANNEXB_H_

#define 	LL_INFO		1
#define 	LL_WARN		2
#define 	LL_ERROR	4
#define		LL_FLOW		8
#define		LL_INOUT	16
#define 	LL_ALL		0xffffffff

#if defined(X86_TEST) 
#define DEBUG_INFO
#endif//#if defined(X86_TEST) 

#if defined(DEBUG_INFO)
#define   ENABLE_TRACE 1
#endif//DEBUG_INFO
#if !ENABLE_TRACE
#ifdef LINUX
#define AvdLog2(LL_INFO,...) 
#define TAvdLog(LL_INFO,...) //for mul-thread check
#define DUMPDataIn(a,b,c)
#define DUMPDataToLOG(a,b)
#else//_LINUX_ANDROID

static __inline void AvdLog2(int logId, char *format, ...){};
static __inline void DUMPDataIn(char* data,int size,int prefixAdd){};
static __inline void DUMPDataToLOG(char* data,int size){};

#endif//_LINUX_ANDROID
#else//ENABLE_TRACE
void AvdLog2(int level,char *format, ...);
//void TAvdLog(int level,char *format, ...);
#define TAvdLog  AvdLog2
void DUMPDataIn(char* data,int size,int prefixAdd);
void DUMPDataToLOG(char* data,int size);
#endif//ENABLE_TRACE
extern int gLogLevel;
#endif


