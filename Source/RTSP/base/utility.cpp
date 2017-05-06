#include "voLog.h"
#include "network.h"

#ifdef LINUX
#include <sys/stat.h>
#elif defined WIN32
//#include <direct.h>
//#elif defined UNDER_CE
//#include <direct.h>
#elif defined _IOS
#include <sys/stat.h>
#include "voOSFunc.h"
#endif//LINUX
#include "ParseSPS.h"
#include "utility.h"

#ifdef _VONAMESPACE
	namespace _VONAMESPACE {
#endif

void ShowWinInfo(char* info);
//------------------------------------------------------------------
const char* sgVersion="2012.0418.1732: add voSource2 support  ";
static char voBaseSixFourDecodeTable[256];

#define TARGET_ONE_FILE "c:/htcRTSPLog.txt"

#if MEMORY_TRACE
typedef struct  
{
	char name[256];
	int	 address;
	int  isAlloc;
}VOMemTableItem;
#define MAX_MEM_ITEM 2048
static VOMemTableItem voMemTable[MAX_MEM_ITEM]={0};
static int ItemSize=0;
void trace_new(char* name,int address)
{
	if(ItemSize>=MAX_MEM_ITEM)
	{
		//CLog::Log.MakeLog(LL_RTSP_ERR,"error.txt","ItemSize>MAX_MEM_ITEM\n");
		return;
	}
	VOMemTableItem* item = &(voMemTable[ItemSize]);
	strcpy(item->name,name);
	item->address = address;
	item->isAlloc = 1;
	ItemSize++;
}
void trace_delete(int address)
{
	for (int i=0;i<ItemSize;i++)
	{
		VOMemTableItem* item = &(voMemTable[i]);
		if(item->address==address)
		{
			item->isAlloc = 0;
		}
	}
}
void trace_memLeak()
{
	FILE* file=fopen("c:/visualon/memleak.txt","w");
	if(file)
	{
		for (int i=0;i<ItemSize;i++)
		{
			VOMemTableItem* item = &(voMemTable[i]);
			if(1)//item->isAlloc)
			{
				fprintf(file,"%d:%s,address=Ox%X,free=%d\n",i,item->name,item->address,item->isAlloc);
			}
		}
		fclose(file);
		ItemSize = 0;
	}
}
#endif//_DEBUG
#define XYSSL_ERR_BASE64_BUFFER_TOO_SMALL               -0x0010
#define XYSSL_ERR_BASE64_INVALID_CHARACTER              -0x0012

const BYTE Base64ValTab[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define AVal(x) Base64ValTab[x]
int EncodeBase64(char * pInput, char * pOutput)
{
	int i = 0;
	int loop = 0;
	int remain = 0;
	int iDstLen = 0;
	int iSrcLen = (int)strlen(pInput);

	loop = iSrcLen/3;
	remain = iSrcLen%3;

	// also can encode native char one by one as decode method
	// but because all of char in native string  is to be encoded so encode 3-chars one time is easier.

	for (i=0; i < loop; i++)
	{
		BYTE a1 = (pInput[i*3] >> 2);
		BYTE a2 = ( ((pInput[i*3] & 0x03) << 4) | (pInput[i*3+1] >> 4) );
		BYTE a3 = ( ((pInput[i*3+1] & 0x0F) << 2) | ((pInput[i*3+2] & 0xC0) >> 6) );
		BYTE a4 = (pInput[i*3+2] & 0x3F);

		pOutput[i*4] = AVal(a1);
		pOutput[i*4+1] = AVal(a2);
		pOutput[i*4+2] = AVal(a3);
		pOutput[i*4+3] = AVal(a4);
	}

	iDstLen = i*4;

	if (remain == 1)
	{
		// should pad two equal sign
		i = iSrcLen-1;
		BYTE a1 = (pInput[i] >> 2);
		BYTE a2 = ((pInput[i] & 0x03) << 4);

		pOutput[iDstLen++] = AVal(a1);
		pOutput[iDstLen++] = AVal(a2);
		pOutput[iDstLen++] = '=';
		pOutput[iDstLen++] = '=';
		pOutput[iDstLen] = 0x00;
	}
	else if (remain == 2)
	{
		// should pad one equal sign
		i = iSrcLen-2;
		BYTE a1 = (pInput[i] >> 2);
		BYTE a2 = ( ((pInput[i] & 0x03) << 4) | (pInput[i+1] >> 4));
		BYTE a3 = ( (pInput[i+1] & 0x0F) << 2);

		pOutput[iDstLen++] = AVal(a1);
		pOutput[iDstLen++] = AVal(a2);
		pOutput[iDstLen++] = AVal(a3);
		pOutput[iDstLen++] = '=';
		pOutput[iDstLen] = 0x00;
	}
	else
	{
		// just division by 3
		pOutput[iDstLen] = 0x00;
	}

	return iDstLen;
}

static const unsigned char base64_dec_map[128] =
{
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
	127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
	54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
	127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
	5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
	15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
	25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
	29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
	39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
	49,  50,  51, 127, 127, 127, 127, 127
};
int voBaseSixFourDecodeEx(unsigned char *in, unsigned char *dst, int *dlen)
{
	int var1, var2, var3;
	unsigned long var4;
	unsigned char *pVar1;
	int len1=strlen((char*)in);
	for( var1 = var2 = var3 = 0; var1 < len1; var1++ )
	{
		if( ( len1 - var1 ) >= 2 &&
			in[var1] == '\r' && in[var1 + 1] == '\n' )
			continue;

		if( in[var1] == '\n' )
			continue;

		if( in[var1] == '=' && ++var2 > 2 )
			return( XYSSL_ERR_BASE64_INVALID_CHARACTER );

		if( in[var1] > 127 || base64_dec_map[in[var1]] == 127 )
			return( XYSSL_ERR_BASE64_INVALID_CHARACTER );

		if( base64_dec_map[in[var1]] < 64 && var2 != 0 )
			return( XYSSL_ERR_BASE64_INVALID_CHARACTER );

		var3++;
	}

	if( var3 == 0 )
		return( 0 );

	var3 = ((var3 * 6) + 7) >> 3;

	if( *dlen < var3 )
	{
		*dlen = var3;
		return( XYSSL_ERR_BASE64_BUFFER_TOO_SMALL );
	}

	for( var2 = 3, var3 = var4 = 0, pVar1 = dst; var1 > 0; var1--, in++ )
	{
		if( *in == '\r' || *in == '\n' )
			continue;

		var2 -= ( base64_dec_map[*in] == 64 );
		var4  = (var4 << 6) | ( base64_dec_map[*in] & 0x3F );

		if( ++var3 == 4 )
		{
			var3 = 0;
			if( var2 > 0 ) *pVar1++ = (unsigned char)( var4 >> 16 );
			if( var2 > 1 ) *pVar1++ = (unsigned char)( var4 >>  8 );
			if( var2 > 2 ) *pVar1++ = (unsigned char)( var4       );
		}
	}

	*dlen = pVar1 - dst;

	return( 0 );
}

static void voInitBaseSixFourDecodeTable()
{
	int i;
	for (i = 0; i < 256; ++i)
		voBaseSixFourDecodeTable[i] = (char)0x80;

	for (i = 'A'; i <= 'Z'; ++i)
		voBaseSixFourDecodeTable[i] = 0 + (i - 'A');
	for (i = 'a'; i <= 'z'; ++i)
		voBaseSixFourDecodeTable[i] = 26 + (i - 'a');
	for (i = '0'; i <= '9'; ++i)
		voBaseSixFourDecodeTable[i] = 52 + (i - '0');

	voBaseSixFourDecodeTable[(unsigned char)'+'] = 62;
	voBaseSixFourDecodeTable[(unsigned char)'/'] = 63;
	voBaseSixFourDecodeTable[(unsigned char)'='] = 0;
}

unsigned char * voBaseSixFourDecode(char * inM, unsigned int & size, bool voZeros)
{
	static bool haveInited = false;
	if(!haveInited)
	{
		voInitBaseSixFourDecodeTable();
		haveInited = true;
	}

	unsigned char * get = new unsigned char[strlen(inM) + 1];
	int var1 = 0;
	int const iax = (int)strlen(inM) - 3;
	for (int var2 = 0; var2 < iax; var2 += 4)
	{
		char in[4], omp[4];
		for(int var3 = 0; var3 < 4; ++var3)
		{
			in[var3] = inM[var3+var2];
			omp[var3] = voBaseSixFourDecodeTable[(unsigned char)in[var3]];
			if((omp[var3]&0x80) != 0)
				omp[var3] = 0;
		}

		get[var1++] = (omp[0]<<2) | (omp[1]>>4);
		get[var1++] = (omp[1]<<4) | (omp[2]>>2);
		get[var1++] = (omp[2]<<6) | omp[3];
	}

	if (voZeros)
	{
		while (var1 > 0 && get[var1-1] == '\0') --var1;
	}

	size = var1;
	unsigned char * result = new unsigned char[size];
	memmove(result, get, size);
	SAFE_DELETE_ARRAY(get);

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


int NotifyEvent(int eventCode,int param)
{
	NotifyEventFunc* func=&(CUserOptions::UserOptions.notifyFunc);
	if(func->funtcion)
	{
		//CUserOptions::UserOptions.errorID = 0;//reset the errorID
		int previousEventCode = CUserOptions::UserOptions.eventCode;
		CUserOptions::UserOptions.eventCode = eventCode;
		if(CUserOptions::UserOptions.m_bMakeLog&&eventCode!=HS_EVENT_GET_REFCLOCK&&eventCode!=VO_EVENT_PACKET_LOST)
		{
			sprintf(CLog::formatString,"code=%d,param=%d\n",eventCode,param);
			CLog::Log.MakeLog(LL_BUF_STATUS,"event.txt",CLog::formatString);
		}
		if(eventCode == VO_EVENT_PLAY_FAIL||eventCode == VO_EVENT_SOCKET_ERR)
		{
			CUserOptions::UserOptions.m_exit=true;
		}

		if(eventCode == VO_EVENT_RTCP_RR)
		{
			return func->funtcion(eventCode,(long*)param,(long*)func->parent);
		}

		if(previousEventCode!=eventCode
			||eventCode==HS_EVENT_GET_REFCLOCK
			||eventCode==VO_EVENT_BUFFERING_PERCENT
			||eventCode==VO_EVENT_PACKET_LOST
			)
		{
			if(CUserOptions::UserOptions.sdkVersion==1)
				return func->funtcion(eventCode,(long*)&param,(long*)func->parent);
			else
				return func->funtcion(eventCode,(long*)param,(long*)func->parent);
		}
		else
		{
			CLog::Log.MakeLog(LL_RTSP_ERR,"event.txt","repeat event,disable to renotify it\n");
			return 0;
		}
	}
	return 0;
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
	//char * fmtpSpropParameterSets = strdup(spropParamSets);	//contented by zxs 0810
#if defined(_WIN32_WCE)
		char * fmtpSpropParameterSets = _strdup(spropParamSets);
#else
		char * fmtpSpropParameterSets = strdup(spropParamSets);
#endif
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
			unsigned char * prop = voBaseSixFourDecode(from, propSize);
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
		uc = Value(*from++) << 4;
		uc |= Value(*from++);

		audioMuxVersion = ((uc & 0x80) >> 7) != 0;
		if(audioMuxVersion != 0) 
			break;

		allStreamsSameTimeFraming = ((uc & 0x40) >> 6) != 0;
		subFrames = (uc & 0x3F);

		uc = Value(*from++) << 4;
		uc |= Value(*from++);
		
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

//------------------------------------------------------------------------

unsigned int Random32()
{
	static bool hasSetSeed = false;
	if(!hasSetSeed)
	{
		hasSetSeed = true;
		srand(timeGetTime());//clock());
	}
	
	return (unsigned int)rand();
}

//------------------------------------------------------------------------

/*buffer:the data  at the beginning of  NALU
  		1 : I frame
  		0 : P frame or other NALU
*/
int IsH264IntraFrame(BYTE * input, int bufLen)
{
	return GetFrameType((char*)input,bufLen)==0;//VO_VIDEO_FRAME_I
}

// 0x000001B6 00(bit) -> Intra Frame
int IsMPEG4IntraFrame(BYTE * buffer, int bufLen)
{
	if(bufLen < 5)
		return 0;

	if(buffer[0] == 0x0 && buffer[1] == 0x0 && buffer[2] == 0x1)
	{
		if(buffer[3] == 0xb6)//check vop
		{
			return (buffer[4] >>6)==0;
		}
		else//non vop
		{
#define MAX_MPEG4SEQHEADSIZE 600
			if(bufLen>MAX_MPEG4SEQHEADSIZE)//sometimes the I frame is combined with the other head data
				return 1;	
		}

	}

	return 0;
}

/*
 0000 0000 0000 0000 1000 00(22bit) 
 Skip 16 bit
 17 bit 0 -> Intra Frame
*/
int IsH263IntraFrame(BYTE * buffer, int bufLen)
{
	if(bufLen < 2)
		return 0;

	if(buffer[0] == 0x0 && buffer[1] == 0x0 && (buffer[2]&0xfc) == 0x80)
	{
		if((buffer[4] & 0x02) == 0)
		{
			return  1;
		}
	}
	else
	{
		CLog::Log.MakeLog(LL_RTP_ERR,"codec_err.txt","$invalid h263 frame\n");
	}

	return 0;
}
#define MAX_STRORED_PACKETS 30

#ifdef _VONAMESPACE
}
#endif

//--------------  class CUserOptions  -------------------

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CUserOptions CUserOptions::UserOptions;

CUserOptions::CUserOptions(void)
: m_nUDPPortFrom(PORT_MIN)
, m_nUDPPortTo(PORT_MAX)
, m_nMediaBuffTime(DEFAULT_BUFFER_TIME)
, m_bMakeLog(0)
,m_URLType(UT_RTSP_URL)
,m_exit(false)
,m_nResetLog(1)
,m_nAppendMode(0)
,m_nSeekToNextIFlag(0)
{
	m_nRTPTimeOut					= 60;//DEFAULT_RTP_TIMEOUT/1000;
	m_nRTSPTimeOut					= 30;//10;//DEFAULT_RTSP_TIMEOUT/1000;
	m_nConnectTimeOut				= 30;
	m_nReceiverReportInterval		= DEFAULT_RTCP_KEEPALIVE;
	m_nBandWidth					= 0;//16kbps
	//memset(m_sUserAgent,0,MAX_USER_INFO_SIZE);
	strcpy(m_sUserAgent, "VisualOn Streaming Player2.0");//RealMedia Player (HelixDNAClient)/10.0.0.0 (win32)
	//strcpy(m_sUAProfile, "http://www.openmobilealliance.org/UAProfile.xml");
	m_sUAProfile[0]='\0';
	m_nPortOpener					= 1;
	m_nSupportPause					= 1;
	m_nForceLivePause				= 0;
	m_bEnablePlaylistSupport			= false;
	m_currURL						= NULL;
	m_timeoutCounter				= 0;
	notifyFunc.funtcion = NULL;
	notifyFunc.parent	= NULL;
	m_nHKIcableAVOutOfSync = 0;
	memset(&outInfo,0,sizeof(HS_StreamingInfo));
	outInfo.codecType[0]= kCodecTypeUnknown;
	outInfo.codecType[1]= kCodecTypeUnknown;
	sdpURLLength = 0;
	streamEnd	 = false;
	status		 = Session_Shutdown;
	m_useTCP	 = 0;
	debugFlag	 = 0;
	isSeeking    = false;
	isBuffering  = false;
	preBufferingLocalTime = 0;
	streamType	 = ST_3GPP;
	m_nMediaBuffTime = 4;//
	//factory			= NULL;
	//HTC new request
	m_bDisableAVSyncNotification	=false;//For quality control
	m_bOpenOriginalUDPPorts			=false;//Some proxies would change the UDP ports which client sends to server in SETUP
	m_bOpenUAProEachRequest			=false;//Some Customer hope that UAProfile can be included in each request
	m_bUseRTSPProxy					=false;
	m_minimumLatency				=false;
	m_nSourceEndTime				=1000;
	m_szProxyIP[0]					='\0';
	m_CheckSessionAlive				=0;
	eventCode = 0;
	forcePause = false;
	portScale  = 1;
	prevStatus = Session_Shutdown;
	m_nRTCPInterval = 5000;
	m_nWaitForTeardown = 0;
	m_nEnableNullAudFrameWhenPacketloss = 0;
	m_nSinkFlushed	= 1;
	nReceivdPacks=0;
	nLostPacks=0;
	m_n3GPPAdaptive=1;
	m_nFirstIFrame=1;
	m_nBufferLow=0;
	m_3GPPLinkChart.mGBR=32;//kbps
	m_3GPPLinkChart.mMBR=128;
	m_3GPPLinkChart.mMaxDelay=2000;
	sdkVersion = 0;
	antiJitterBufferSize = MAX_STRORED_PACKETS;
	m_extraFields = NULL;
	endOfStreamFromServer = 0;
	initPlayPos = 0;
	m_nTimeMarker = 10;
	
	digestAuth.user = new char[128];
	memset(digestAuth.user,0,128);

	digestAuth.passwd = new char[128];
	memset(digestAuth.passwd,0,128);
	
	strcpy(digestAuth.passwd,"user");
	digestAuth.passwdLen = 4;
	
	strcpy(digestAuth.user,"user");
	digestAuth.userLen= 4;
	
	digestIsSet = false;

#ifdef _IOS
	memset(logDir,0,1024);
	memset(cfgPath,0,1024);
#endif // _IOS

	m_pRTSPTrans = NULL;
	m_pRTSPTransFunc = NULL;
	memset(m_apkworkpath,0,512);

	m_skipPos = 0;
	m_videoConnectionPort = 0;
	m_audioConnectionPort = 0;
	m_isOverHttpTunnel = 0;
	m_rtspOverHttpConnectionPort = 0;
	m_isLiveStream = false;
}

CUserOptions::~CUserOptions(void)
{
	if(m_extraFields)
	{
		free(m_extraFields);
		m_extraFields = NULL;
	}
	if(digestAuth.passwd)
	{
		delete [] digestAuth.passwd;
		digestAuth.passwd = NULL;
		digestAuth.passwdLen = 0;
	}
	if(digestAuth.user)
	{
		delete [] digestAuth.user;
		digestAuth.user = NULL;
		digestAuth.userLen = 0;
	}
	
}

bool CUserOptions::LoadUserOptionsFromReg()
{
	return false;
}

void CUserOptions::GetUDPPortRange(int & nUDPPortFrom, int & nUDPPortTo)
{
	 nUDPPortFrom = m_nUDPPortFrom;
	 nUDPPortTo = m_nUDPPortTo;
}

//---------------------  class CLog ---------------------

CLog CLog::Log;
char CLog::formatString[MAX_LOG_STRING_LEN];
CLog::CLog(void):
m_lastTime(0),
m_actualFile(0)
{
	CUserOptions::UserOptions.m_bMakeLog = LL_NONE;
	m_logLevel = LL_NONE;
	
#ifdef LINUX
	//LOGI("RTSP version %s",sgVersion);		//doncy 1106

	int ret=mkdir(LOG_DIRECTORY,0x020);//S_IWRITE);	
	if(ret==-1)
	{
		VOLOGI("create log directory in /sdcard/rtspLog fails");			
	}
#elif defined WIN32//LINUX
//#ifndef UNDER_CE
//	mkdir(LOG_DIRECTORY);
//#endif //UNDER_CE
	//mkdir(LOG_DIRECTORY);
#elif defined UNDER_CE
	//mkdir(LOG_DIRECTORY);
#elif defined _IOS

	voOS_GetAppFolder(CUserOptions::UserOptions.logDir,1024);
	voOS_GetAppFolder(CUserOptions::UserOptions.cfgPath,1024);
	strcat(CUserOptions::UserOptions.logDir,"rtspLog/");
	strcat(CUserOptions::UserOptions.cfgPath,"rtsp.cfg");
	
  //VOLOGI("The rtsplog path is: %s", CUserOptions::UserOptions.logDir);
  //VOLOGI("The cfg file path is: %s", CUserOptions::UserOptions.cfgPath);
  //mkdir(CUserOptions::UserOptions.logDir,0x020);    //should be 0777
#endif//LINUX
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
void CLog::UpdateLogLevel()
{
	int reg = CUserOptions::UserOptions.m_bMakeLog;
	if(reg>LOG_NONE)
	{
		if(reg<=LOG_RTSP)
		{
			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR);
		}
		if(reg>=LOG_RTP)
		{
			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR|LL_VIDEO_SAMPLE|LL_AUDIO_SAMPLE|LL_BUF_STATUS|LL_AUDIO_TIME|LL_VIDEO_TIME);//
		}
		if(reg>=LOG_DATA)
			m_logLevel = LOG_Level(LL_SOCKET_ERR|LL_RTSP_ERR|LL_RTCP_ERR|LL_RTP_ERR|LL_CODEC_ERR|LL_VIDEO_TIME|LL_AUDIO_TIME|LL_BUF_STATUS|LL_VIDEO_DATA|LL_AUDIO_DATA|LL_VIDEO_SAMPLE|LL_AUDIO_SAMPLE);
	}
	
}
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
	
	if(target&LT_FILE)
	{
		fprintf(logFile->m_logFile,"[VO_RTSP][%s]:",oriLogFile);
		fwrite(logText, 1, strlen(logText), logFile->m_logFile);
		fflush(logFile->m_logFile);
	}

	if(target&LT_STDOUT)
	{
		printf("[VO_RTSP][%s]:",oriLogFile);
		printf("%s",logText);		
	}
	
}

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
			fprintf(logFile->m_logFile,"version=%s,RTSP-URL=%s \n ",sgVersion,CUserOptions::UserOptions.m_currURL);
			
		}
	}
	
	int currTime = timeGetTime();
	fprintf(logFile->m_logFile,"%u:  ",currTime);
	fwrite(logText, 1, strlen(logText), logFile->m_logFile);
	fflush(logFile->m_logFile);
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
			fprintf(logFile->m_logFile,"version=%s,RTSP-URL=%s \n ",sgVersion,CUserOptions::UserOptions.m_currURL);
		}
	}
	//int currTime = timeGetTime();
	//if(m_lastTime==0)
	//	m_lastTime = currTime;
	//int	diff	 = currTime - m_lastTime;invalid RTP port
	//m_lastTime = currTime;
	//if(m_isPrintTime)
	//	fprintf(logFile->m_logFile,"%u:  ",currTime);

	fwrite(logData, 1, len, logFile->m_logFile);
	fflush(logFile->m_logFile);
}
void CLog::MakeLog(LOG_Level level,const char * logFileName, const unsigned char * logData, int len)
{
	if(!CUserOptions::UserOptions.m_bMakeLog)
		return;
	else if(m_logLevel == LL_NONE)
		UpdateLogLevel();
	
	if(m_logLevel&level)
	{
		MakeLog2(logFileName,logData, len);
	}
}

void CLog::MakeLog(LOG_Level level,const char * logFileName, const char * logData)
{
#ifndef WIN32
	if(level==LL_SOCKET_ERR)
	{
		VOLOGI("%u %s",timeGetTime(),logData);
	}
#endif

	if(!CUserOptions::UserOptions.m_bMakeLog)
		return;
	else if(m_logLevel == LL_NONE)
		UpdateLogLevel();
	
	if(m_logLevel&level)
		MakeLog2(logFileName,logData);
}
