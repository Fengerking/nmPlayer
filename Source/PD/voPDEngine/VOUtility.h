#ifndef _VOSSUTILITY_H
#define _VOSSUTILITY_H
//#include <vector>
#include <stdio.h>
#include <stdlib.h>
extern int g_nLog;
#ifdef X86_PC1
#define ENABLE_TRACE_MEM 1
#endif//X86_PC
void voTraceNew(void* buf,size_t size,const char* funcName,const char* fileName=NULL,int line=0,int flag=0,char* info=NULL);
void voTraceDelete(void* buf);
#include	"list_T.h"
#ifdef LINUX
#define __cdecl 
#endif//LINUX
#ifdef WIN32
#define POINTER_64 __ptr64
#endif//WIN32
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif//_MAX_PATH
#if 0
#ifdef  UNICODE                     // r_winnt

#ifndef _TCHAR_DEFINED
typedef WCHAR TCHAR, *PTCHAR;
typedef WCHAR TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */
#else//UNICODE
#ifndef _TCHAR_DEFINED
typedef char TCHAR, *PTCHAR;
typedef unsigned char TBYTE , *PTBYTE ;
#define _TCHAR_DEFINED
#endif /* !_TCHAR_DEFINED */
#endif//UNICODE
#endif//0
namespace VOUtility{
//using namespace std;

typedef enum{
	LL_FATAL			= 1,   /// A fatal error. The application will most likely terminate. This is the highest priority.
	LL_CRITICAL,    /// A critical error. The application might not be able to continue running successfully.
	LL_ERROR,       /// An error. An operation did not complete successfully, but the application as a whole is not affected.
	LL_WARNING,     /// A warning. An operation completed with an unexpected result.
	LL_NOTICE,      /// A notice, which is an information with just a higher priority.
	LL_INFORMATION, /// An informational message, usually denoting the successful completion of an operation.
	LL_DEBUG,       /// A debugging message.
	LL_TRACE        /// A tracing message. This is the lowest priority.
}LOG_Level;
#define  ENABLE_LOG 1

#if defined(WIN32)||defined(_WIN32)
#ifdef _WIN32_WCE
#define LOG_DIRECTORY "\\My Documents\\VOPD\\"
#else
#define LOG_DIRECTORY "C:/visualon/VOPD/"
#endif//_WIN32_WCE
#endif//WIN32

#ifdef LINUX
#ifdef G1
//#include "voLog_android.h"
#define LOG_DIRECTORY    "/data/local/visualonLog_pd/"	//"/sdcard/visualonLog_pd/" //
#else//G1
#define LOG_DIRECTORY    "/visualonLog_pd/"
//#define voLog_android(a,b)
#endif//G1
#endif//LINUX
#ifdef G1
#include "voLog_android.h"
//#define LOG_DIRECTORY    "/data/local/visualonLog_pd/"	//"/sdcard/visualonLog_pd/" //
#else//G1
//#define LOG_DIRECTORY    "/visualonLog_pd/"
#define voLog_android(a,b)
#endif//G1
#define MIN_FILE_NUM  12
#define VOAPI __cdecl
	
#if ENABLE_LOG
	void  VOAPI voLog(int id,const char* fileName,const char* logInfo,...);
	void  VOAPI voLogData(int id,const char* fileName,const char* data,int size);
	//void  voLog(int id,char* fileName,char* logInfo);
#else//ENABLE_LOG
	__inline void  voLog(int id,char* fileName,char* logInfo,...){};
	//__inline void  voLog(int id,char* fileName,char* logInfo){};
#endif//ENABLE_LOG

#define MIN_BUF_NUM 1024
	
	typedef enum
	{
		VOMM_CLEAR = 1,

	}VOMM_FLAG;
	typedef enum
	{
		VOMR_LEAK			= 1,
		VOMR_MULTIPLE_FREE	= 1<<1,
		VOMR_OVERWRITE		= 1<<2,
		VOMR_ALL			= 0xffffffff,
	}MEM_ReposrtFlag;
	typedef unsigned long u_int32;
	typedef unsigned short u_int16;
	typedef unsigned char u_int8;
	typedef long	int32;
	typedef short	int16;
	typedef char	int8;

#define GETMMTYPE(flag) ((flag)&0xff000000)
#define SETMMTYPE(flag,type) ((flag)|((type<<24)|0xff000000))
#define GETMMALIGNMENT(flag) ((flag)&0x00ff0000)
#define SETMMALIGNMENT(flag,align) ((flag)|(((align)<<16)|0x00ff0000))

#define MEM_CHECK __FUNCTION__,__FILE__,__LINE__	
#define MEM_MANAGER public 	CVOMemoryManager
	class CVOMemoryManager
	{
	public:

		static void* allocate(size_t size,const char* funcName,const char* fileName=NULL,int line=0,int flag=0,char* info=NULL);
		static void  deAllocate(void* buf);
		void* operator new(size_t size,const char* funcName,const char* fileName=NULL,int line=0,int flag=0,char* info=NULL);
		void  operator delete(void* buf);
		void* operator new[](size_t size,const char* funcName,const char* fileName=NULL,int line=0,int flag=0,char* info=NULL);
		void  operator delete[](void* buf);
		static void  reportMemStatus(int flag);
		static void voTraceNewImp(void* buf,size_t size,const char* funcName,const char* fileName,int line,int flag,char* info);
		static void voTraceDeleteImp(void* buf);
		typedef struct  
		{
			char* fileName;
			char* funcName;
			int	   line;
			char*  info;
			u_int32 size;
			u_int32  flag;
			unsigned char*  buf;
			int	   allocated;//the flag can detect allocate status,1:allocated,0:free,<0:multiple free(error)
		}MemInfo;
	protected:
		CVOMemoryManager(void){};
		~CVOMemoryManager(void){};
	private:

#if ENABLE_TRACE_MEM
		static list_T<MemInfo> bufInfoList;
		typedef	list_T<MemInfo>::iterator  TBufInfoListIter;
#endif//ENABLE_TRACE_MEM
	

	};
#ifndef VO_SAFE_DELETE
#define VO_SAFE_DELETE( x )            \
	if( x )                          \
	{                                \
	deAllocate(x);                    \
	x = NULL;                    \
	}
#endif //SAFE_DELETE

	class CVOLog:MEM_MANAGER
	{
	public:
		static CVOLog* CreateVOLog();
		static void	   DestroyVOLog();
	public:
		void Log2File(int logID,const char* fileName,const char* logInfo,int size=0);
		FILE*	GetFileByName(char* name);
	private:
		class CLogFile:MEM_MANAGER
		{
		public:
			CLogFile(const char * logFileName)
				: m_logFile(NULL)

			{

				strcpy(m_logFileName, LOG_DIRECTORY);
				strcat(m_logFileName, logFileName);
				if (g_nLog==1)
					m_logFile = fopen(m_logFileName, "a+");
				else
					m_logFile = fopen(m_logFileName, "w");
				if(m_logFile==NULL)
				{
					voLog_android(m_logFileName,"create log file fails");
				}
				strcpy(m_logFileName, logFileName);
			}
			~CLogFile()
			{
				if(m_logFile != NULL)
				{
					fclose(m_logFile);
					m_logFile = NULL;
				}
			}

			char m_logFileName[_MAX_PATH];
			FILE * m_logFile;

		};
		static CVOLog* m_log;
		CVOLog();
		~CVOLog();
		list_T<CLogFile*> logFileList;
		typedef	list_T<CLogFile*>::iterator  TLogFileListIter;
	};
	class IVOSSObject:MEM_MANAGER
	{
	public:
		virtual ~IVOSSObject(){};
		virtual  int	ProcessEvent(int eventID,void* param1){return 0;};
	};//IEventHandler
#define VOSSOBJECT public IVOSSObject
	enum
	{
		VOSS_OK		   = 0,
		VOSS_ERR_COMMON=-1,
	};

	typedef  int  (__cdecl * CALLBACKNOTIFY2)(long EventCode, long EventParam1, long* EventParam2);
	typedef struct tagNotifyEventFunc:MEM_MANAGER
	{
		CALLBACKNOTIFY2 funtcion;
		void* parent;
	} NotifyEventFunc, *PNotifyEventFunc;
}//VOUtility
class PDGlobalData
{
public:
	static PDGlobalData data;
	PDGlobalData()
	{
		version = 0;
	}
	int version;
};
#endif//_VOSSUTILITY_H