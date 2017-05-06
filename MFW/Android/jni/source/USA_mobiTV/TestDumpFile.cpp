  #include "TestDumpFile.h"
#include "voLog.h"
#include "voOSFunc.h"
#define MAX_VIDEO_BUF_SIZE1 256*1024
#define MAX_AUDIO_BUF_SIZE1 32*1024
static unsigned char videoBuf[MAX_VIDEO_BUF_SIZE1];
static unsigned char audioBuf[MAX_AUDIO_BUF_SIZE1];
CTestDumpFile::CTestDumpFile(void):
m_bEnabled(false),
m_fAudio(NULL),
m_fVideo(NULL)
{
	memset(&m_videoBuf,0,sizeof(VOMP_BUFFERTYPE));
	memset(&m_audioBuf,0,sizeof(VOMP_BUFFERTYPE));
	
}
void	CTestDumpFile::	Reset()
{
	if(m_audioBuf.pBuffer)
		delete[] m_audioBuf.pBuffer;
	if(m_videoBuf.pBuffer)
		delete[] m_videoBuf.pBuffer;
	memset(&m_videoBuf,0,sizeof(VOMP_BUFFERTYPE));
	memset(&m_audioBuf,0,sizeof(VOMP_BUFFERTYPE));
	m_audioBuf.pBuffer = new unsigned char[MAX_AUDIO_BUF_SIZE1];
	m_videoBuf.pBuffer = new unsigned char[MAX_VIDEO_BUF_SIZE1];
	if(m_fAudio)
		fclose(m_fAudio);
	if(m_fVideo)
		fclose(m_fVideo);
	m_fVideo = m_fAudio = NULL;
}

CTestDumpFile::~CTestDumpFile(void)
{
	if(m_audioBuf.pBuffer)
		delete[] m_audioBuf.pBuffer;
	if(m_videoBuf.pBuffer)
		delete[] m_videoBuf.pBuffer;
	if(m_fAudio)
		fclose(m_fAudio);
	if(m_fVideo)
		fclose(m_fVideo);
	m_fVideo = m_fAudio = NULL;
}
#define CHECK_READ(read,desire)\
	if(read!=desire)\
{\
	VOLOGE("actaulRead=%d,desire=%d",read,desire);\
	return -3;\
}
int CTestDumpFile::ReadFrame(FILE* file,VOMP_BUFFERTYPE * pBuf,VOMP_BUFFERTYPE * target)
{
	if(file)
	{
		if(feof(file))
			return -1;
		unsigned long  rcvTime;
		unsigned long  localTime = voOS_GetSysTime();
		int actaulRead	=	fread(&pBuf->nSize,1,4,file);
		CHECK_READ(actaulRead,4);
		actaulRead		=	fread(&pBuf->llTime,1,4,file);
		CHECK_READ(actaulRead,4);
		actaulRead		=	fread(&rcvTime,1,4,file);
		CHECK_READ(actaulRead,4);
		actaulRead		=	fread(pBuf->pBuffer,1,pBuf->nSize,file);
		CHECK_READ(actaulRead,pBuf->nSize);
		if(0)//pBuf->nReserve>0)
		{
			unsigned long diffLocalTime = localTime -pBuf->nReserve;
			unsigned long diffRcvTime = rcvTime-(unsigned long)pBuf->nFlag;
			int sleepTime	  = diffRcvTime - diffLocalTime-5;
			if(sleepTime>0&&sleepTime<1000)
				voOS_Sleep(sleepTime);
		}
		
		pBuf->nReserve  =	localTime;
		pBuf->nFlag		=	(long)rcvTime;
		target->llTime  =	pBuf->llTime;
		target->nSize	=	pBuf->nSize;
		target->pBuffer	=	pBuf->pBuffer;
	}
	else
	{
		return -2;
	}
}

int		CTestDumpFile::	ReadAudio (VOMP_BUFFERTYPE * pBuffer)
{
	int ret = ReadFrame(m_fAudio,&m_videoBuf,pBuffer);
	return ret;
}
int		CTestDumpFile::	ReadVideo (VOMP_BUFFERTYPE * pBuffer)
{
	int ret = ReadFrame(m_fVideo,&m_videoBuf,pBuffer);
	return ret;
}
void	CTestDumpFile::	SetMediaFile(char* audioFile,char* videoFile)
{
	Reset();

	if(audioFile)
	{
		m_fAudio = fopen(audioFile,"rb");
		if(m_fAudio==NULL)
			VOLOGE("faile to open %s ",audioFile);
		
	}
	
	if(videoFile)
	{
		m_fVideo = fopen(videoFile,"rb");
		if(m_fVideo==NULL)
			VOLOGE("faile to open %s ",videoFile);
	}
	


}
