#include "VOUtility.h"
#include <memory.h>
//using std::vector;
using namespace VOUtility;
#include <string.h>
#include "voPDPort.h"
#include <time.h>
#if ENABLE_LOG
#include 		<stdarg.h>
#ifdef LINUX
#include <sys/stat.h>
#else//LINUX

#ifdef _WIN32_WCE
void mkdir(char* dir);
#else//_WIN32_WCE
#include <direct.h>
#endif//_WIN32_WCE
#endif//LINUX

PDGlobalData PDGlobalData::data;
//static char tmpBuf[1024*10];//TODO:assume the max buf size is 10240
//void  voLog(int id,char* fileName,char* logInfo)
//{
//	CVOLog* log=CVOLog::CreateVOLog();
//	log->Log2File(id,fileName,logInfo);
//}
static char tmpBuf[8192];
void  VOAPI VOUtility::voLog(int id,const char* fileName,const char* logInfo,...)
{
	if(g_nLog<1) return;
		
	if(id==LL_DEBUG)
	{
		//char* logInfo2=(char*)logInfo;
		va_list arg;
		va_start(arg, logInfo);
		vsprintf(tmpBuf, logInfo, arg);
		va_end(arg);
		voLog_android((char*)fileName,(char*)tmpBuf);
	}
	
	CVOLog* log=CVOLog::CreateVOLog();
#if 0
	va_list arg;
	va_start(arg, logInfo);
	vsprintf(tmpBuf, logInfo, arg);
	va_end(arg);
	log->Log2File(id,fileName,tmpBuf);
	if(g_nLog==3)
		printf(tmpBuf);
#else
	FILE* file=log->GetFileByName((char*)fileName);
	if(file)
	{
		
		fprintf(file,"[%u]:", voGetCurrentTime());
		va_list arg;
		va_start(arg, logInfo);
		vfprintf(file,logInfo,arg);
		va_end(arg);
		fflush(file);
	}
	
#endif
}
void  VOAPI VOUtility::voLogData(int id,const char* fileName,const char* data,int size)
{
	if(g_nLog<1)
		return;
	CVOLog* log=CVOLog::CreateVOLog();
#if 0
	if(data&&size>0)
		log->Log2File(id,fileName,data,size);
#else
	if(data&&size>0)
	{
		FILE* file=log->GetFileByName((char*)fileName);
		fwrite(data,1,size,file);
		fflush(file);
	}
#endif
}
#endif//ENABLE_LOG
#if ENABLE_TRACE_MEM
#define PADDING_LEN 4
#else//ENABLE_TRACE_MEM
#define PADDING_LEN 0
#endif//ENABLE_TRACE_MEM
#define PADDING_VALUE 0xfdfdfdfd

#if ENABLE_TRACE_MEM
list_T<CVOMemoryManager::MemInfo>  CVOMemoryManager::bufInfoList;
#endif//
#if 1//ENABLE_TRACE_MEM
void voTraceNew(void* buf,size_t size,const char* funcName,const char* fileName,int line,int flag,char* info)
{
	CVOMemoryManager::voTraceNewImp(buf,size,funcName, fileName, line, flag, info);
};
void voTraceDelete(void* buf)
{
	CVOMemoryManager::voTraceDeleteImp(buf);
};

#endif//ENABLE_TRACE_MEM
void CVOMemoryManager::voTraceNewImp(void* buf,size_t size,const char* funcName,const char* fileName,int line,int flag,char* info)
{
#if ENABLE_TRACE_MEM
	if(buf)
	{


		MemInfo meminfo;
		if(info!=NULL)
			meminfo.info=strdup(info);
		else
			meminfo.info=strdup("No Info");
		if(funcName!=NULL)
			meminfo.funcName= strdup(funcName);
		else
			meminfo.funcName= strdup("No Func Name");

		if(fileName!=NULL)
			meminfo.fileName=strdup(fileName);
		else
			meminfo.fileName=strdup("No fileName");
		meminfo.line	= line;
		meminfo.buf		=(u_int8*)buf;
		meminfo.size	=size;
		meminfo.flag	=flag;
		meminfo.allocated=1;
		bufInfoList.push_back(meminfo);
	}
#endif
}
void CVOMemoryManager::voTraceDeleteImp(void* buf)
{
#if ENABLE_TRACE_MEM
	TBufInfoListIter iter=bufInfoList.begin();
	TBufInfoListIter end=bufInfoList.end();
	for (;iter!=end;++iter)
	{
		if(buf==(*iter).buf)
		{
			(*iter).allocated--;
			u_int32* pad=(u_int32*)((*iter).buf+(*iter).size);
			if(*pad!=PADDING_VALUE)
			{
				(*iter).allocated=VOMR_OVERWRITE;
			}
			(*iter).flag=(u_int32)buf;
			(*iter).buf=0;
			if ((*iter).fileName)
				free((*iter).fileName);
			if((*iter).funcName)
				free((*iter).funcName);
			if((*iter).info)
				free((*iter).info);	
		}
	}
#endif
};
void* CVOMemoryManager::allocate(size_t size,const char* funcName,const char* fileName,int line,int flag,char* info)
{
	
	int actualSize=size+PADDING_LEN;
	void* buf=NULL;
	if(flag&VOMM_CLEAR)
	{
		buf=calloc(1,actualSize);
	}
	else
	{
		buf=malloc(actualSize);
	}
#if ENABLE_TRACE_MEM
	if(buf)
	{
		u_int32* pad=(u_int32*)((u_int8*)buf+size);
		*pad=PADDING_VALUE;
	}
	voTraceNew(buf,size,funcName, fileName, line, flag, info);
#endif//	ENABLE_TRACE_MEM
	return buf;
}
void  CVOMemoryManager::reportMemStatus(int flag)
{
#if ENABLE_TRACE_MEM
	TBufInfoListIter iter=bufInfoList.begin();
	TBufInfoListIter end=bufInfoList.end();
	for (;iter!=end;++iter)
	{
		//check leak
		
		const char* strInfo	=(*iter).funcName;
		const char* info		=(*iter).info;
		const char* file		=(*iter).fileName;
		int		line					=(*iter).line;
		if((flag&VOMR_LEAK)&&(*iter).allocated==1)
		{
			voLog(0,"memory.txt","leak:%s,file=%s,line=%d,info=%s\n",strInfo,file,line,info);
		}
		
		//check multiple free
		if((flag&VOMR_MULTIPLE_FREE)&&(*iter).allocated<0)
		{
			voLog(0,"memory.txt","multiple free:%s,file=%s,line=%d,info=%s\n",strInfo,file,line,info);
		}

		if((flag&VOMR_OVERWRITE))
		{
			if((*iter).allocated==1)
			{
				//check overwrite
				u_int32* pad=(u_int32*)((*iter).buf+(*iter).size);
				if(*pad!=PADDING_VALUE)
				{
					voLog(0,"memory.txt","overwrite:%s,file=%s,line=%d,info=%s\n",strInfo,file,line,info);
				}
			}
			else//the memory has been freed
			{
				if((*iter).allocated==VOMR_OVERWRITE)
				{
					voLog(0,"memory.txt","overwrite2:%s,file=%s,line=%d,info=%s\n",strInfo,file,line,info);
				}
			}
			
		}
		
	}
	bufInfoList.clear();
#endif//ENABLE_TRACE_MEM
}
void  CVOMemoryManager::deAllocate(void* buf)
{
	
	voTraceDelete(buf);
	if(buf)
		free(buf);
}
void* CVOMemoryManager::operator new(size_t size,const char* funcName,const char* fileName,int line,int flag,char* info)
{
	return allocate(size,funcName,fileName,line,flag,info);
}
void  CVOMemoryManager::operator delete(void* buf)
{
	return deAllocate(buf);
}
void* CVOMemoryManager::operator new[](size_t size,const char* funcName,const char* fileName,int line,int flag,char* info)
{
	return allocate(size,funcName,fileName,line,flag,info);
}
void  CVOMemoryManager::operator delete[](void* buf)
{
	return deAllocate(buf);
}


CVOLog* CVOLog::m_log=NULL;
CVOLog* CVOLog::CreateVOLog()
{
	if(m_log==NULL)
	{
		m_log=new(MEM_CHECK) CVOLog();
	}
	return m_log;
}

void CVOLog::DestroyVOLog()
{
	delete m_log;
	m_log=NULL;
}
CVOLog::CVOLog()
{
	//logFileList.reserve(MIN_FILE_NUM);
#ifndef _HTC
#ifdef LINUX
	int ret=mkdir(LOG_DIRECTORY,0x020);//S_IWRITE);	
	if(ret==-1)
		voLog_android(LOG_DIRECTORY,"create log directory fails");
	else
		voLog_android(LOG_DIRECTORY,"create log directory OK");			
#else//LINUX
	mkdir(LOG_DIRECTORY);
#endif//LINUX
#endif//_HTC
}
CVOLog::~CVOLog()
{
	TLogFileListIter iter	=logFileList.begin();
	TLogFileListIter end =logFileList.end();
	for (;iter!=end;++iter)
	{
		CLogFile* file=(*iter);
		delete file;
		file=NULL;
	}
   logFileList.clear();
}
FILE*	CVOLog::GetFileByName(char* fileName)
{
	int size=logFileList.size();
	CLogFile* file=NULL;
	TLogFileListIter iter	=logFileList.begin();
	TLogFileListIter end =logFileList.end();
	for (;iter!=end;++iter)
	{

		if(strcmp((*iter)->m_logFileName,fileName)==0)
		{
			file=(*iter);
			break;
		}		
	}
	if(file==NULL)
	{

		file=new(MEM_CHECK) CLogFile(fileName);
		logFileList.push_back(file);
	}
	if(file)
		return file->m_logFile;
	else
		return NULL;
}
void CVOLog::Log2File(int logID,const char* fileName,const char* logInfo,int sizeOfInfo)
{
	
	
	FILE* file=GetFileByName((char*)fileName);
	if(file)
	{
		if(sizeOfInfo==0)
		{
			
			fprintf(file,"[%d]:", voGetCurrentTime());
			fwrite(logInfo, 1, strlen(logInfo), file);
		}
		else
		{
			fwrite(logInfo, 1, sizeOfInfo, file);
		}
		fflush(file);
		
	}
}

int __cdecl voGetCurrentTime()
{
	return IVOSocket::GetCurrTime();
}


#define UNIT_TEST	0
#if	UNIT_TEST
void TestCVOMemoryManager()
{
	class TestClass:CVOMemoryManager
	{

	public:
		char* array;
		TestClass* test2;
		void CreateTestClass(){
			test2=new(MEM_CHECK) TestClass();
			array=(char*)allocate(128*sizeof(char),MEM_CHECK,VOMM_CLEAR,"hello");
		}
		void Destroy()
		{
			delete(test2);
			deAllocate(array);
		}
	};
	if(1)
	{
		TestClass test;
		test.CreateTestClass();
		test.array[128]=1;
		test.Destroy();
		CVOLog::DestroyVOLog();
		CVOMemoryManager::reportMemStatus(VOMR_ALL);

	}
}


#endif//UNIT_TEST