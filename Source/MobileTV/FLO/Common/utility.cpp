#include "utility.h"
#include "network.h"
#include "mylog.h"
#include "voLog.h"
#define LOG_TAG "FLOEngine"

static char base64DecodeTable[256];

static void InitBase64DecodeTable()
{
	int i;
	for (i = 0; i < 256; ++i)
		base64DecodeTable[i] = (char)0x80;

	for (i = 'A'; i <= 'Z'; ++i)
		base64DecodeTable[i] = 0 + (i - 'A');
	for (i = 'a'; i <= 'z'; ++i)
		base64DecodeTable[i] = 26 + (i - 'a');
	for (i = '0'; i <= '9'; ++i)
		base64DecodeTable[i] = 52 + (i - '0');

	base64DecodeTable[(unsigned char)'+'] = 62;
	base64DecodeTable[(unsigned char)'/'] = 63;
	base64DecodeTable[(unsigned char)'='] = 0;
}

unsigned char * Base64Decode(char * in, unsigned int & resultSize, bool trimTrailingZeros)
{
	static bool haveInitedBase64DecodeTable = false;
	if(!haveInitedBase64DecodeTable)
	{
		InitBase64DecodeTable();
		haveInitedBase64DecodeTable = true;
	}

	unsigned char * out = new unsigned char[strlen(in) + 1];TRACE_NEW("Base64Decode out",out);
	int k = 0;
	int const jMax = (int)strlen(in) - 3;
	for (int j = 0; j < jMax; j += 4)
	{
		char inTmp[4], outTmp[4];
		for(int i = 0; i < 4; ++i)
		{
			inTmp[i] = in[i+j];
			outTmp[i] = base64DecodeTable[(unsigned char)inTmp[i]];
			if((outTmp[i]&0x80) != 0)
				outTmp[i] = 0;
		}

		out[k++] = (outTmp[0]<<2) | (outTmp[1]>>4);
		out[k++] = (outTmp[1]<<4) | (outTmp[2]>>2);
		out[k++] = (outTmp[2]<<6) | outTmp[3];
	}

	if (trimTrailingZeros)
	{
		while (k > 0 && out[k-1] == '\0') --k;
	}

	resultSize = k;
	unsigned char * result = new unsigned char[resultSize];TRACE_NEW("Base64Decode result",result);
	memmove(result, out, resultSize);
	SAFE_DELETE_ARRAY(out);

	return result;
}

//------------------------------------------------------------------------

int Value(const char c)
{
	int value = 0;
	if(c >= '0' && c <= '9')
	{
		value = c - '0';
	}
	else if(c >= 'A' && c <= 'F') 
	{
		value = 10 + c - 'A';
	} 
	else if(c >= 'a' && c <= 'f') 
	{
		value = 10 + c - 'a';
	}
	else
	{
		value = 0;
	}

	return value;
}

void HexStrToBytes(const char * hexString, unsigned char ** buffer, int * bufLen)
{
	*buffer = NULL;
	*bufLen = 0;

	do
	{
		size_t hexStringLen = strlen(hexString);
		if(hexStringLen == 0)
			break;

		*buffer = new unsigned char[hexStringLen];TRACE_NEW("HexStrToBytes buffer",*buffer);
		if(*buffer == NULL)
			break;

		unsigned char value1, value2;
		unsigned char * to = *buffer;
		for(unsigned int i=0; i<hexStringLen; ++i)
		{
			value1 = Value(hexString[i++]);
			if(i == hexStringLen)
			{
				value2 = 0;
				*to++ = (value1 << 4) | value2;
				break;
			}

			value2 = Value(hexString[i]);

			*to++ = (value1 << 4) | value2;
		}

		*bufLen = (int)(to - *buffer);

	}while(0);
}

void SpropParameterSetsData(const char * spropParamSets, unsigned char ** buffer, int * bufLen)
{
	char * fmtpSpropParameterSets = _strdup(spropParamSets);
	*buffer = NULL;
	*bufLen = 0;

	do
	{
		int spropParameterSetsLen = strlen(fmtpSpropParameterSets);
		if(spropParameterSetsLen == 0)
			break;

		*buffer = new unsigned char[spropParameterSetsLen * 2]; TRACE_NEW("SpropParameterSetsData buffer",*buffer);
		if(*buffer == NULL)
			break;

		char * p;
		int spropParamCount = 1;
		for(p = fmtpSpropParameterSets; *p != '\0'; ++p) 
		{
			if(*p == ',')
			{
				++spropParamCount;
				*p = '\0';
			}
		}

		char * from = fmtpSpropParameterSets;
		unsigned char * to = *buffer;
		for(int i=0; i< spropParamCount; ++i)
		{
			unsigned int propSize = 0; 
			unsigned char * prop = Base64Decode(from, propSize);
			to[0] = propSize >> 8;
			to[1] = propSize;
			to += 2;
			memcpy(to, prop, propSize);
			to += propSize;

			SAFE_DELETE_ARRAY(prop);

			from += strlen(from) + 1;
		}

		*bufLen = to - *buffer;

		free(fmtpSpropParameterSets);

		return;

	}while(0);

	free(fmtpSpropParameterSets);
}


//------------------------------------------------------------------------

unsigned int Random32()
{
	static bool hasSetSeed = false;
	if(!hasSetSeed)
	{
		hasSetSeed = true;
		srand(voOS_GetSysTime());//clock());
	}
	
	return (unsigned int)rand();
}

//------------------------------------------------------------------------
int IsValidH264NALU(unsigned char * buffer,int size)
{
	int naluType = buffer[0]&0x0f;
	int result = 1;
	if(!(naluType>=1&&naluType<=12))
	{
		SLOG2(LL_SOCKET_ERR,"videoRTP.txt","this is Invalid NALU %d,size=%d\n",naluType,size);
		result = 0;
	}
	return result;
}

/*buffer:the data  at the beginning of  NALU
  		1 : I frame
  		0 : P frame or other NALU
*/
int IsH264IntraFrame(unsigned char * buffer, int /* bufLen */)
{
	int inf,i;
	long byteoffset;      // byte from start of buffer
	int bitoffset;      // bit from start of byte
	int ctr_bit=0;      // control bit for current bit posision
	int bitcounter=1;
	int len,value;
	int info_bit;
	int totbitoffset = 0;
	int naluType = buffer[0]&0x0f;
	if(naluType==5)
		return 1;
	if(naluType==1)//need continuous check
		buffer++;
	else//the nalu type is params info
		return 0;
	for(i=0;i<2;i++)
	{
		byteoffset= totbitoffset/8;
		bitoffset= 7-(totbitoffset%8);
		ctr_bit = (buffer[byteoffset] & (0x01<<bitoffset));   // set up control bit
		
		len=1;
		while (ctr_bit==0)
		{                 // find leading 1 bit
			len++;
			bitoffset-=1;           
			bitcounter++;
			if (bitoffset<0)
			{                 // finish with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			ctr_bit=buffer[byteoffset] & (0x01<<(bitoffset));
		}
		// make infoword
		inf=0;                          // shortest possible code is 1, then info is always 0
		for(info_bit=0;(info_bit<(len-1)); info_bit++)
		{
			bitcounter++;
			bitoffset-=1;
			if (bitoffset<0)
			{                 // finished with current byte ?
				bitoffset=bitoffset+8;
				byteoffset++;
			}
			
			inf=(inf<<1);
			if(buffer[byteoffset] & (0x01<<(bitoffset)))
				inf |=1;
		}
		totbitoffset+=len*2-1;
		if(totbitoffset>48)
			return 0;
	}
	//(int)pow(2,(bitsUsed/2))+info-1;//pow(2,x)==1<<x
	len = (len*2-1)/2;
	inf = (1<<len)+inf-1;
	if(inf==2||inf==7)//I frame
		return 1;
/*	
	else if (inf!=0&&inf!=5)//P frame
	{
		//parsing error!!!!!
		printf("Parsing the IsIntraH264 data fail\n");
		return 0;
	}
*/
	return 0;           
}


//--------------  class CUserOptions  -------------------

CUserOptions CUserOptions::UserOptions;

CUserOptions::CUserOptions(void)
: m_nUDPPortFrom(PORT_MIN)
, m_nUDPPortTo(PORT_MAX)
, m_nMediaBuffTime(DEFAULT_BUFFER_TIME)
,m_URLType(UT_RTSP_URL)
,m_exit(false)
,m_nResetLog(1)
,m_bMakeLog (0)
{
	m_nRTPTimeOut					= 10;//DEFAULT_RTP_TIMEOUT/1000;
	m_nRTSPTimeOut					= 10;//10;//DEFAULT_RTSP_TIMEOUT/1000;
	m_nConnectTimeOut				= 5;
	m_nReceiverReportInterval		= 1000;
	m_nMaxErrorRetry				= 50;

	strcpy(m_sUserAgent, "FLO Player");


	m_currURL						= NULL;
	m_timeoutCounter				= 0;
	notifyFunc.funtcion = NULL;
	notifyFunc.parent	= NULL;
	memset(&outInfo,0,sizeof(HS_StreamingInfo));
	outInfo.codecType[0]= kCodecTypeUnknown;
	outInfo.codecType[1]= kCodecTypeUnknown;
	sdpURLLength = 0;

	m_useTCP	 = 0;



	m_nMediaBuffTime = 0;//

	//factory			= NULL;
	//HTC new request

	eventCode = 0;

	portScale  = 1;

	m_nRTCPInterval = 1000;




	nLostPacks=0;


	status = prevStatus = Session_Stopped;
	disableNotify=true;

	memset(&m_mfCallBackFuncSet,0,sizeof(VO_FLOENGINE_CALLBACK));
}

CUserOptions::~CUserOptions(void)
{
}

int NotifyEvent(int eventCode,int param)
{
        VOLOGI("Event: %d, %d", eventCode, param);

#if ENABLE_LOG
		TheEventLogger.Dump(LOGFMT_EVENT, GetAppTick(), eventCode, param);
#endif //ENABLE_LOG
		SLOG2(LL_RTSP_ERR,"event.txt","code=%d,param=%d\n",eventCode,param);
		

		if(CUserOptions::UserOptions.m_mfCallBackFuncSet.OnEvent)
			CUserOptions::UserOptions.m_mfCallBackFuncSet.OnEvent(eventCode, param, 0);
		
		CUserOptions::UserOptions.eventCode = 0;
	

		return 0;
}



//---------------------  class CLog ---------------------
CLog CLog::Log;
char CLog::formatString[1024];
void CreateLogDirectory();
CLogFile::CLogFile(const char * logFileName)
: m_logFile(NULL)

{
	char* modes[]={"wb","a"};
	int modeIndex=0;//CUserOptions::UserOptions.m_nAppendMode;
	if(CUserOptions::UserOptions.m_bMakeLog<10)
	{				
		strcpy(m_logFileName, GetLogDir2());
		strcat(m_logFileName, logFileName);
	}
	else
		strcpy(m_logFileName, logFileName);
#ifdef  _LINUX_ANDROID
	m_logFile = fopen(m_logFileName, modes[modeIndex]);
	if(m_logFile==NULL)
	{
		VOLOGI("faile to create %s ",m_logFileName);
	}
#else//_LINUX_ANDROID
	m_logFile = fopen(m_logFileName, "wb");
#endif//_LINUX_ANDROID
	strcpy(m_logFileName, logFileName);
	
}

CLogFile::~CLogFile()
{
	if(m_logFile != NULL)
	{
		fclose(m_logFile);
		m_logFile = NULL;
	}
}
CLog::CLog(void):
m_lastTime(0),
m_actualFile(0)
{

	//CreateDirectory(_T("\\My Documents\\VisualOn\\"), NULL);
	//_mkdir("\\My Documents\\VisualOn\\");
	m_logLevel = LL_NONE;
}
#ifdef LINUX
#include <sys/stat.h>
#else//LINUX
#ifdef _WIN32_WCE
void mkdir(char* dirname);
#else//_WIN32_WCE
#include <direct.h>
#endif//_WIN32_WCE
#endif//LINUX
void CLog::UpdateLogLevel()
{
	int reg = CUserOptions::UserOptions.m_bMakeLog&0x0f;
	int target=CUserOptions::UserOptions.m_bMakeLog>>12;
	if(reg>LOG_NONE)
	{
		if(reg<=LOG_RTSP)
		{
			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR);
		}
		if(reg>=LOG_RTP)
		{
			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR|LL_VIDEO_SAMPLE|LL_AUDIO_SAMPLE|LL_BUF_STATUS);//
			if(target==0)
				m_logLevel=LOG_Level(m_logLevel|LL_RTP_ERR|LL_RTP_ERR);
		}
		if(reg>=LOG_DATA)
			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR|LL_RTP_ERR|LL_RTP_ERR|LL_BUF_STATUS|LL_VIDEO_DATA|LL_AUDIO_DATA|LL_VIDEO_SAMPLE|LL_AUDIO_SAMPLE);
#ifdef LINUX	
		mkdir(GetLogDir2(),0755);
#endif
	}

}
CLog::~CLog(void)
{
	Reset();
}

void CLog::Reset()
{
	list_T<CLogFile *>::iterator iter;
	int size = m_logFileSet.size();
	if(size!=m_actualFile)
	{
		printf("error size!=m_actualFile\n");
	}
	for(iter=m_logFileSet.begin(); iter!=m_logFileSet.end(); ++iter)
	{
		//CLogFile * file=*iter;
		TRACE_DELETE(*iter);
		SAFE_DELETE(*iter);
	}
	m_logFileSet.clear();
	memset(&(CUserOptions::UserOptions.outInfo),0,sizeof(HS_StreamingInfo));
	m_actualFile=0;
	CUserOptions::UserOptions.m_currURL=NULL;
}

//void CLog::UpdateLogLevel()
//{
//	int reg = CUserOptions::UserOptions.m_bMakeLog&0x0f;
//	int target=CUserOptions::UserOptions.m_bMakeLog>>12;
//	if(reg>LOG_NONE)
//	{
//		if(reg<=LOG_RTSP)
//		{
//			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR);
//		}
//		if(reg>=LOG_RTP)
//		{
//			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR|LL_VIDEO_SAMPLE|LL_AUDIO_SAMPLE|LL_BUF_STATUS);//
//			if(target==0)
//			  m_logLevel=LOG_Level(m_logLevel|LL_RTP_ERR|LL_RTP_ERR);
//		}
//		if(reg>=LOG_DATA)
//			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR|LL_RTP_ERR|LL_RTP_ERR|LL_BUF_STATUS|LL_VIDEO_DATA|LL_AUDIO_DATA|LL_VIDEO_SAMPLE|LL_AUDIO_SAMPLE);
//		if(target==0)
//			CreateLogDirectory();
//	}
//	
//}
bool CLog::m_isPrintTime=true;
void CLog::MakeLog3(int target,const char * logFileName, const char* oriLogFile,const char * logText)
{

	CLogFile * logFile = NULL;
	list_T<CLogFile *>::iterator iter;
	list_T<CLogFile *>::iterator begin = m_logFileSet.begin();
	list_T<CLogFile *>::iterator end = m_logFileSet.end();
	for(iter=begin; iter!=end; ++iter)
	{
		if(strcmp((*iter)->m_logFileName, logFileName) == 0)
		{
			logFile = *iter;
			break;
		}
	}
	if(logFile == NULL)
	{
		logFile = new CLogFile(logFileName);TRACE_NEW(logFileName,logFile);
		if(logFile&&logFile->m_logFile==NULL)
		{
			SAFE_DELETE(logFile);TRACE_DELETE(logFile);
			return;
		}
		m_actualFile++;

		m_logFileSet.push_back(logFile);
	}
	
	//if(target&LT_FILE)
	//{
	//	fprintf(logFile->m_logFile,"[VO_RTSP][%s]:",oriLogFile);
	//	fwrite(logText, 1, strlen(logText), logFile->m_logFile);
	//	fflush(logFile->m_logFile);
	//}

}
int gLevel = 0;
void CLog::MakeLog2(const char * logFileName, const char * logText)
{

	
	CLogFile * logFile = NULL;
	list_T<CLogFile *>::iterator iter;
	list_T<CLogFile *>::iterator begin = m_logFileSet.begin();
	list_T<CLogFile *>::iterator end = m_logFileSet.end();
	for(iter=begin; iter!=end; ++iter)
	{
		if(strcmp((*iter)->m_logFileName, logFileName) == 0)
		{
			logFile = *iter;
			break;
		}
	}
	if(logFile == NULL)
	{
		logFile = new CLogFile(logFileName);TRACE_NEW(logFileName,logFile);
		if(logFile&&logFile->m_logFile==NULL)
		{
			SAFE_DELETE(logFile);TRACE_DELETE(logFile);
			return;
		}
		m_actualFile++;

		m_logFileSet.push_back(logFile);
		if(CUserOptions::UserOptions.m_bMakeLog<LOG_DATA&&CUserOptions::UserOptions.m_currURL&&m_isPrintTime)
		{
//			fprintf(logFile->m_logFile,"version=%s,RTSP-URL=%s \n ",sgVersion,CUserOptions::UserOptions.m_currURL);
			
		}
	}
	/*
	for(iter=m_logFileSet.begin(); iter!=m_logFileSet.end(); ++iter)
	{
		fprintf(logFile->m_logFile,"mm check=%s \n ",(*iter)->m_logFileName);
	}*/
	unsigned int currTime = voOS_GetSysTime();
	//if(m_lastTime==0)
	//	m_lastTime = currTime;
	//int	diff	 = currTime - m_lastTime;
	//m_lastTime = currTime;
	if(m_isPrintTime)
		fprintf(logFile->m_logFile,"%u:  ",currTime);
	
	fwrite(logText, 1, strlen(logText), logFile->m_logFile);
	fflush(logFile->m_logFile);
	if(CUserOptions::UserOptions.status == Session_Connectting&&gLevel!=LL_SOCKET_ERR)
		VOLOGI("%u:%s",currTime,logText);
	
}

void CLog::MakeLog2(const char * logFileName, const unsigned char * logData, int len)
{
	CLogFile * logFile = NULL;
	list_T<CLogFile *>::iterator iter;
	for(iter=m_logFileSet.begin(); iter!=m_logFileSet.end(); ++iter)
	{
		if(strcmp((*iter)->m_logFileName, logFileName) == 0)
		{
			logFile = *iter;
			break;
		}
	}
	if(logFile == NULL)
	{
		logFile = new CLogFile(logFileName);TRACE_NEW(logFileName,logFile);
		if(logFile&&logFile->m_logFile==NULL)
		{
			SAFE_DELETE(logFile);TRACE_DELETE(logFile);
			return;
		}
		m_actualFile++;

		m_logFileSet.push_back(logFile);
		if(CUserOptions::UserOptions.m_bMakeLog<LOG_DATA&&CUserOptions::UserOptions.m_currURL&&m_isPrintTime)
		{
//			fprintf(logFile->m_logFile,"version=%s,RTSP-URL=%s \n ",sgVersion,CUserOptions::UserOptions.m_currURL);
		}
	}
	unsigned int currTime = voOS_GetSysTime();
	//if(m_lastTime==0)
	//	m_lastTime = currTime;
	//int	diff	 = currTime - m_lastTime;invalid RTP port
	//m_lastTime = currTime;
	if(m_isPrintTime)
		fprintf(logFile->m_logFile,"%u:  ",currTime);

	fwrite(logData, 1, len, logFile->m_logFile);
	fflush(logFile->m_logFile);
	
}
void CLog::MakeLog(LOG_Level level, const char * logFileName, const unsigned char * logData, int len)
{
	if(!CUserOptions::UserOptions.m_bMakeLog)
		return;
	else if(m_logLevel == LL_NONE)
		UpdateLogLevel();
	gLevel = level;
	if(m_logLevel&level)
	{
		MakeLog2(logFileName,logData, len);
	}
}

void CLog::MakeLog(LOG_Level level, const char * logFileName, const char * logData)
{
	if(level==LL_SOCKET_ERR)
	{
		VOLOGI("%u %s",voOS_GetSysTime(),logData);
	}
	if(!CUserOptions::UserOptions.m_bMakeLog)
		return;
	else if(m_logLevel == LL_NONE)
		UpdateLogLevel();
	gLevel = level;
	if(m_logLevel&level)
	{

		MakeLog2(logFileName,logData);

	}

}

void ParseMPEG4LatmAudioFmtpConfig(const char * fmtpConfig, unsigned char ** buffer, int * bufLen)
{
	bool audioMuxVersion = false;
	bool allStreamsSameTimeFraming = true;
	unsigned int subFrames = 0;
	unsigned int program = 0;
	unsigned int layer = 0;

	*buffer = NULL;
	*bufLen = 0;

	do 
	{
		const char * from = fmtpConfig;
		unsigned char uc = 0;
		uc = (Value(*from++) << 4) | Value(*from++);

		audioMuxVersion = ((uc & 0x80) >> 7) != 0;
		if(audioMuxVersion != 0) 
			break;

		allStreamsSameTimeFraming = ((uc & 0x40) >> 6) != 0;
		subFrames = (uc & 0x3F);

		uc = (Value(*from++) << 4) | Value(*from++);
		program = (uc & 0xF0) >> 4;

		layer = (uc & 0x0E) >> 1;


		unsigned char lastBit = uc & 1;

		int configLen = (strlen(from) + 1) / 2 + 1;

		*buffer = new unsigned char[configLen];TRACE_NEW("ParseMPEG4LatmAudioFmtpConfig buffer",*buffer);
		if(*buffer == NULL)
			break;
		memset(*buffer, 0, configLen);

		unsigned char * to = *buffer;
		do 
		{
			uc = Value(*from++) << 4; 
			uc |= Value(*from++);

			*to++ = (lastBit << 7) | ((uc & 0xFE) >> 1);
			lastBit = uc & 1;
		} while(from < fmtpConfig + strlen(fmtpConfig));

		*bufLen = configLen;
		//*bufLen = 2;

	} while (0);
}



#if 0//def LINUX
int _strnicmp(const char *dst,const char *src,int count)
{
	int ch1, ch2;
	do
	{
		if ( ((ch1 = (unsigned char)(*(dst++))) >= 'A') &&(ch1 <= 'Z') )
			ch1 += 0x20;
		if ( ((ch2 = (unsigned char)(*(src++))) >= 'A') &&(ch2 <= 'Z') )
			ch2 += 0x20;
	} while ( --count && ch1 && (ch1 == ch2) );
	return (ch1 - ch2);
}

int _stricmp(const char *dst,const char *src)
{
	return strnicmp(dst,src,strlen(src));
}

#endif//LINUX
