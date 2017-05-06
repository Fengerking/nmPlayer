#include "httpget.h"
#include "CWMSHttpCmdHandle.h"
#include "ASFHeadParser.h"
void DumpLog_NetGet(LPCSTR);

CWMSHttpCmdHandle::CWMSHttpCmdHandle()
	: CHttpCmdHandle()
	,m_netDownType(VO_NETDOWN_TYPE_WMS)
	,m_counts(0)
	,m_clientID(0)
	,m_remainedDataLen(0)
	,m_foundHead(-1)
	,m_pGet(NULL)
	,m_playListID(0)
	,m_headSize(0)
	,m_curPackPos(NULL)
	,m_prevRemainedSize(0)
	,m_prevLastCopySize(0)
	,m_contentLength(0)
	,m_packSize(0)
	,m_loopNum(0)
	,m_status(STATUS_STOP)
	,m_headBuf(NULL)
{
	memset(&m_pack,0,sizeof(TASFPacket));
}

CWMSHttpCmdHandle::~CWMSHttpCmdHandle()
{
	VO_SAFE_DELETE(m_headBuf);
}
#define DUMP_LOCAL 0
int	CWMSHttpCmdHandle::GetRemainedDataInCMDBuf(char* p)
{
	if(m_foundHead>0&&m_headSize>0&&m_remainedDataLen<MAX_RESPONSE_BUFFER)
	{
	
		m_status=STATUS_PLAY;
#if DUMP_LOCAL
		voLogData(2,"WMSHead.asf",m_headBuf,m_headSize);
#endif//DUMP_LOCAL
		memcpy(p,m_headBuf,m_headSize);	
		voLog(1,"WMSHead.txt","CWMSHttpCMD:Get Head,size=%d\n",m_headSize);
		CheckSocketActivity();//on some devices,the server will close the session after play
		m_foundHead=0;
		return m_headSize;
	}
	return 0;
}
int CWMSHttpCmdHandle::CheckSocketActivity()
{
	int nReaded=0;
	int totalSize=0;
	int leftSie=256;
	int isHTTPResponse=0;
	
	voLog(1,"http.txt","!!!HGRC_NETWORK_CLOSE:wms replay,reconnect again\n");
	m_pGet->ReConnect();
	Play();
	return 0;
}
#define LE_LONG(p) (((p[3]<<24)&0xff000000)|((p[2]<<16)&0x00ff0000)|((p[1]<<8)&0x0000ff00)|(p[0]&0x000000ff))
#define LE_SHORT(p) (((p[1]<<8)&0x0000ff00)|(p[0]&0x000000ff))
#define MAX_PADDING 256
static const char tmpPadding[MAX_PADDING]={0};
static int GetAsfPackSize(const char* pos)
{
#define ASF_PACK_SIZE_LEN 3
	char tmpBuf[2]={0};
	char  value[ASF_PACK_SIZE_LEN+1]={0};
	char* pValue=&(value[2]);
	for(int i=0;i<ASF_PACK_SIZE_LEN;i++)
	{
		tmpBuf[0]=pos[i];
		//*pValue=atoi(tmpBuf);pValue++;
		int value2;
		sscanf(tmpBuf, "%X", &value2);
		*pValue--=value2;
	}
	int size= ((value[2]<<8)&0x0f00)|(((value[1]<<4)&0x0f0)|value[0]);//LE_LONG(value);
	return size;
}
HTTPGETRETURNVALUE		CWMSHttpCmdHandle::ReceiveTest(CHttpGet* pGet)
{
	int frameCount=0;
	if (m_curPackPos==NULL)
	{
		m_curPackPos =m_responseBuffer;
		m_prevRemainedSize=0;
	}
	int nRecved=0;
	do 
	{
		pGet->Receive(m_curPackPos,MAX_RESPONSE_BUFFER-m_prevRemainedSize,&nRecved);
		
		if(nRecved)
		{
			voLogData(2,"WMSHead.data",m_curPackPos,nRecved);
#define OFFSET3 16
			char* actualEnd=m_curPackPos+nRecved;
			char* end=actualEnd-OFFSET3;
			char* pos=m_responseBuffer+2;
			voLog(1,"PDData.txt","begin size:%d(%d+%d)\n",m_prevRemainedSize+nRecved,m_prevRemainedSize,nRecved);

			int i=0;

			char* curPackBegin=NULL;
			char* curPackEnd=NULL;
			
			char* lastFrame=NULL;
			int totalSize=m_prevRemainedSize+nRecved-12;
			
			
			for (;pos<end;pos++)
			{
				if(pos[-2]==0xD&&pos[-1]==0xA&&(pos[0]&0x7f)==0x24&&pos[1]==0x44)//find one $D
				{
					char* pFlag=pos+4;
					char* pPackSize=pos+10;
					int packSize=LE_SHORT(pPackSize);
					int asfPackSize=GetAsfPackSize(pos-5);
					int packNum=(pos[9]&0x00ff);
					packNum+=m_loopNum*256;
					if(packNum==0x00ff)
					{
						m_loopNum++;
					}
					
					curPackBegin=pos+12;
					packSize-=8;//the 
					if(curPackBegin+packSize<end)
					{
						int padding=m_packSize-packSize;
						
						voLogData(2,"WMSHead.asf",curPackBegin,packSize);
						
						if(padding>0)
						{
							if(padding>MAX_PADDING)
							{
								char* strPad=(char*)calloc(1,padding);
								voLogData(2,"WMSHead.asf",strPad,padding);	
								free(strPad);
							}
							else
							{
								voLogData(2,"WMSHead.asf",tmpPadding,padding);
							}
							
						}
						lastFrame=curPackBegin+packSize;
						if(packNum!=frameCount)
							voLog(1,"PDData.txt","!!!packNum(%X)!=frameCount(%X)\n",packNum,frameCount);
						voLog(1,"PDData.txt","pack=%X,asfSize=%d,size=%d(%X-%X),pad=%d,%X,%X,%X,%X,end=%X,%X,%X,%X\n",packNum,asfPackSize,packSize,lastFrame,curPackBegin,padding,
							curPackBegin[0],curPackBegin[1],curPackBegin[2],curPackBegin[3],
							curPackBegin[packSize-4],curPackBegin[packSize-3],curPackBegin[packSize-2],curPackBegin[packSize-1]);
						pos=lastFrame;
						frameCount++;
					}
				}
			}
			if(lastFrame)
			{
				m_prevRemainedSize=actualEnd-lastFrame;
				m_curPackPos=m_responseBuffer+m_prevRemainedSize;
				memmove(m_responseBuffer,lastFrame,m_prevRemainedSize);
				voLog(1,"PDData.txt","remained=%d\n",m_prevRemainedSize);
			}
			else
			{
				m_prevRemainedSize=actualEnd-m_responseBuffer;
				m_curPackPos=m_responseBuffer+m_prevRemainedSize;
				voLog(1,"PDData.txt","remained2=%d\n",m_prevRemainedSize);
			}
		}
	} while(frameCount<2000);
	return HGRC_OK;
}
HTTPGETRETURNVALUE		CWMSHttpCmdHandle::Receive(CHttpGet* pGet,void* pBuffer1, int nToRecv, int* pnRecved)
{
#if DUMP_LOCAL
	return ReceiveTest(pGet);
#endif//DUMP_LOCAL
	int actaulSize=0;
	*pnRecved=0;
	char* pBuffer2=(char*)pBuffer1;
	int nRecved=0;

	HTTPGETRETURNVALUE rc=HGRC_OK;
	int totalSize=0;
	char* end;
	char* pos;
	int i=0;
	if (m_pack.begin==NULL)//have not found the first packet
	{
		m_pack.begin =m_responseBuffer;
		m_pack.currentSize=0;
		m_pack.dataSize=m_packSize?m_packSize/2:128;
		
		rc=pGet->Receive(m_pack.begin,m_pack.dataSize,&nRecved);
#define ENABLE_DUMP_DATA 0		
		//find the packeSize
		while(rc==HGRC_OK&&nRecved>0)
		{
#if ENABLE_DUMP_DATA
			voLogData(2,"PDSource3.data",m_pack.begin,nRecved);
#endif//ENABLE_DUMP_DATA
			m_pack.currentSize+=nRecved;
			totalSize+=nRecved;
			pos=m_pack.begin;
			if(totalSize>32)
			{
				for (;i<totalSize;i++,pos++)
				{
					if(pos[-2]==0xD&&pos[-1]==0xA&&(pos[0]&0x7f)==0x24&&pos[1]==0x44)//find one $D
					{
						
						char* pFlag=pos+4;
						char* pPackSize=pos+10;
						int packSize=LE_SHORT(pPackSize)-8;
						int offset=pos+12-m_pack.begin;
						int packNum=(pos[9]&0x00ff);
						packNum+=m_loopNum*256;
						if(packNum==0x00ff)
						{
							m_loopNum++;
						}
						m_pack.begin+=offset;
						m_pack.dataSize=packSize;
						m_pack.currentSize-=offset;
						int padSize=m_packSize-packSize;
						if(padSize>0)
							m_pack.paddingSize=padSize;
						else
							m_pack.paddingSize=0;
						if(packNum!=m_pack.packNum)
						{
							voLog(1,"PDData.txt","!!!packNum(%X)!=frameCount(%X)\n",packNum,m_pack.packNum);
							m_pack.packNum=packNum;
						}
						voLog(1,"PDData.txt","pack_%d,curSize=%d,packSize=%d,padSize=%d,(%X,%X,%X,%X)\n",
							m_pack.packNum,m_pack.currentSize,m_pack.dataSize,m_pack.paddingSize,
							m_pack.begin[0],m_pack.begin[1],m_pack.begin[2],m_pack.begin[3]);
						break;
					}
				}
			}
			else
			{
				
				IVOThread::Sleep(10);
				rc=pGet->Receive(m_pack.begin+m_pack.currentSize,m_pack.dataSize,&nRecved);
				voLog(1,"PDData.txt","###m_pack.currentSize=%d,try recv(%d),actual=%d\n",m_pack.currentSize,m_pack.dataSize,nRecved);
				continue;
			}
			if(i>=totalSize)
			{
				m_pack.begin=NULL;
				//voLog(1,"WMSHead.txt","have not found the first packet,size=%d\n",nRecved);
				return rc;
			}
			break;
		}
	}
	
	rc=pGet->Receive(m_pack.begin+m_pack.currentSize,m_pack.dataSize-m_pack.currentSize,&nRecved);
	
	while(rc==HGRC_OK&&nRecved>0)
	{
		//voLog(1,"WMSHead.txt","Get,total=%d,expect=%d,actual=%d\n",m_packSize,m_pack.dataSize-m_pack.currentSize,nRecved);
#if ENABLE_DUMP_DATA
		voLogData(2,"PDSource3.data",m_pack.begin+m_pack.currentSize,nRecved);
#endif//ENABLE_DUMP_DATA
		m_pack.currentSize+=nRecved;
		voLog(1,"PDData.txt","pack_%d,curSize=%d,recv=%d\n",m_pack.packNum,m_pack.currentSize,nRecved);
		if(m_pack.currentSize==m_pack.dataSize)
		{
			memcpy(pBuffer2,m_pack.begin,m_pack.dataSize);
			if(m_pack.paddingSize>0&&m_pack.paddingSize<=MAX_PADDING)
				memcpy(pBuffer2+m_pack.dataSize,tmpPadding,m_pack.paddingSize);
			*pnRecved=m_packSize;
			m_pack.begin=NULL;
			m_pack.packNum++;
			break;
		}
		IVOThread::Sleep(10);
		rc=pGet->Receive(m_pack.begin+m_pack.currentSize,m_pack.dataSize-m_pack.currentSize,&nRecved);
	}
	return rc;
}
bool CWMSHttpCmdHandle::FindHead(char* str,char* end)
{
	int actualSize=end-str;
	
	for (;str!=end;str++)
	{
		if(str[0]=='$'&&str[1]=='H')
		{
			int size=(((str[3]<<8)&0x0000ff00)|(str[2]&0x000000ff))-8;
			int actualSize=end-str-12;
			int leftSize;
			int nReaded=0;
			if(actualSize<0)
			{
				leftSize=size-actualSize;
				m_pGet->Receive(m_responseBuffer, leftSize, &nReaded);
				str=m_responseBuffer-(12+actualSize);
				actualSize=nReaded;
			}
			if(m_headBuf==NULL)
				m_headBuf=(char*)allocate(size,MEM_CHECK);
			if(actualSize<size)
			{
				voLog(1,"WMSHead.txt","looking for WMA head,size=%d,actual=%d\n",size,actualSize);
				
				memcpy(m_headBuf,str+12,actualSize);
				char* tmp=m_headBuf+actualSize;
				leftSize=size-actualSize;
				
				
				do
				{
					m_pGet->Receive(m_responseBuffer, leftSize, &nReaded);
					voLog(1,"WMSHead.txt","head:receive more,size=%d(%X,%X,%X,%X),(%X,%X,%X,%X)\n",nReaded,
						m_responseBuffer[0],m_responseBuffer[1],m_responseBuffer[2],m_responseBuffer[3],
						m_responseBuffer[nReaded-4],m_responseBuffer[nReaded-3],m_responseBuffer[nReaded-2],m_responseBuffer[nReaded-1]);
					memcpy(tmp,m_responseBuffer,nReaded);
					tmp+=nReaded;
					leftSize-=nReaded;	
				}while(leftSize>0);
				
				voLog(1,"WMSHead.txt","Eventually get WMA head,actual=%d\n",size-leftSize);
				//memcpy(m_responseBuffer,tmpBuf,size);
				
			}
			else if(size>0)
			{
				//memmove(m_responseBuffer,str+12,size);
				memcpy(m_headBuf,str+12,size);
			}
			m_headSize=size;
			m_foundHead=1;
			voLog(1,"WMSHead.txt","CWMSHttpCMD:Find Head\n");
			return true;
			
		}
	}
	return false;
}
bool CWMSHttpCmdHandle::FindMeta(char* str,char* end)
{
	for(;str!=end;str++)
	{
		//find $M
		if(str[0]=='$'&&str[1]=='M')
		{
			int size=(((str[3]<<8)&0x0000ff00)|(str[2]&0x000000ff))+4;
			str[size-1]='/0';
			char* str2;
			if ((str2=strstr(str+12,"playlist-gen-id=")))
			{
				sscanf(str2,"playlist-gen-id=%u",&m_playListID);
			}
			str+=size;
			return true;
		}
	}
	return false;
}
HTTPGETRETURNVALUE CWMSHttpCmdHandle::CMD_GET(CHttpGet* pGet, const char* szHost, const char* szObject, const char* szUserAgent, const char* szRange, bool bUseProxy,int flag)
{
	char * _cmdFmt;
	HTTPGETRETURNVALUE rc;
	if(m_foundHead!=1)
	{
		m_pGet = pGet;
		char* str=NULL;
		char* host=(char*)szHost;
		char* object=(char*)szObject;
		do 
		{
			_cmdFmt = "GET %s HTTP/%s\r\n"
				"Host: %s\r\n"
				"Accept: */*\r\n"
				"User-Agent: NSPlayer/10.0.0.3802\r\n"
				"X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
				"Pragma: no-cache,rate=1.000,stream-time=0,stream-offset=0:0,packet-num=4294967295,max-duration=0\r\n"
				"Pragma: version11-enabled=1\r\n"
				"Pragma: packet-pair-experiment=1\r\n"
				"Pragma: pipeline-experiment=1\r\n"
				"Pragma: xClientGUID={11223344-1122-1122-1122-AABBCCDDEEFF}\r\n"
				"Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\n"
				"Connection: keep-alive\r\n"
				"\r\n";
			//"Connection: keep\r\n"

			sprintf(m_cmdBuffer, _cmdFmt, object, (0 == pGet->m_nHttpProtocol) ? "1.0" : "1.1", host);

			rc = SendRequest(pGet);
			if(HGRC_OK != rc)
				return rc;
			rc = RecvResponse(pGet);
			if(HGRC_OK != rc)
				return rc;
			m_status = STATUS_SETUP;
			//If the response is an asx file,notify the high level AP to handle it.
			if (str=strstr(m_responseBuffer,"<ASX version="))
			{
				pGet->HandleASXBuf(str);
				voLog(LL_TRACE,"http.txt","###handle asx buf\n ");
				return HGRC_USER_ABORT;
			}
			
		} while(0);
		
		//voLog(1,"WMSHead.txt","Get Describe response,size=%d\n",m_remainedDataLen);
		m_counts++;
		int playListID=0;
		
		if (str=strstr(m_responseBuffer,"client-id="))
		{
			sscanf(str,"client-id=%d",&m_clientID);
		}
		else
		{
			str = m_responseBuffer;
		}
		m_contentLength=LIVE_SIZE;
		if(strstr(m_responseBuffer,"seekable"))
		{
			char* strSize=strstr(m_responseBuffer,"x-wms-content-size=");
			if(strSize)
				sscanf(strSize,"x-wms-content-size=%u",&m_contentLength);
		}
		char* end=m_responseBuffer+m_remainedDataLen;
		bool find_meta=FindMeta(str,end);
		
		int tryCount=0;
		if(!find_meta)
		{
			voLog(1,"WMSHead.txt","CWMSHttpCMD:No meta,recieve again\n");
			RecvResponse(pGet);
		}
		bool find_head=false;
		while(tryCount++<100)
		{
			if(!find_meta)
				find_meta=FindMeta(m_responseBuffer,m_responseBuffer+m_remainedDataLen);
			find_head=FindHead(m_responseBuffer,m_responseBuffer+m_remainedDataLen);
			if(find_head)
				break;
			IVOThread::Sleep(50);
			RecvResponse(pGet);
		}
		if(m_foundHead!=1)
		{
			voLog(1,"WMSHead.txt","CWMSHttpCMD:Failed to get the head,try again\n");
			return HGRC_UNKNOWN_ERROR;
		}
		else
		{
			CASFHeadParser parser;
			parser.Parse((unsigned char*)m_headBuf,m_headSize);
			m_packSize=parser.GetPacketSize();
			if(m_packSize>0)
				pGet->SetParam(VOID_NETDOWN_RECV_BYTES_ONCE,m_packSize);
			else
				voLog(1,"WMSHead.txt","!!!m_packSize==0\n");
		}

		_cmdFmt = "GET %s HTTP/%s\r\n"
			"Host: %s\r\n"
			"Accept: */*\r\n"
			"User-Agent: NSPlayer/10.0.0.3802\r\n"
			"X-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\n"
			"Pragma: no-cache,rate=1.000,stream-time=0,stream-offset=4294967295:4294967295,packet-num=4294967295,max-duration=0\r\n"
			"Pragma: xPlayStrm=1\r\n"
			"Pragma: client-id=%u\r\n"
			"Pragma: LinkBW=2147483647, AccelBW=2147483647, AccelDuration=18000\r\n"
			"Pragma: playlist-gen-id=%u\r\n"
			"Pragma: version11-enabled=1\r\n"
			"Pragma: stream-switch-count=2\r\n"
			"Pragma: stream-switch-entry=ffff:1:0 ffff:2:0\r\n"
			"Pragma: xClientGUID={11223344-1122-1122-1122-AABBCCDDEEFF}\r\n"
			"Supported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\n"
			"Connection: keep-alive\r\n"
			"\r\n";
		//"Connection: keep\r\n"

		sprintf(m_cmdBuffer, _cmdFmt, szObject, (0 == pGet->m_nHttpProtocol) ? "1.0" : "1.1", szHost,m_clientID,m_playListID);

		Play();
	}
	
	return HGRC_OK;//RecvResponse(pGet);
}
void CWMSHttpCmdHandle::Play()
{
	HTTPGETRETURNVALUE rc = SendRequest(m_pGet);
	if(HGRC_OK != rc)
	{	
		voLog(1,"WMSHead.txt","CWMSHttpCMD:Play fail\n");
		return;
	}
	voLog(1,"WMSHead.txt","CWMSHttpCMD:Play\n");
	m_status = STATUS_PLAY_REQUEST;
}
bool CWMSHttpCmdHandle::	ParseStatusCode(unsigned int* _statusCode)
{
	if(m_status==STATUS_PLAY_REQUEST)
	{
		*_statusCode=200;
		return true;
	}
	else
		return CHttpCmdHandle::ParseStatusCode(_statusCode);
}
HTTPGETRETURNVALUE CWMSHttpCmdHandle::RecvResponse(CHttpGet* pGet)
{
//	CAutoForbidDumpLog afdl;
	m_remainedDataLen = 0;
	memset(m_responseBuffer, 0, MAX_RESPONSE_BUFFER);

	int nReaded = 0;
	char* pPos = m_responseBuffer;
	char* p = NULL;
	//while(true)
	{
		//receive byte one by one
		HTTPGETRETURNVALUE rc = pGet->Receive(pPos, MAX_RESPONSE_BUFFER, &nReaded);
		if(HGRC_OK != rc)
			return rc;

		if(nReaded <=0||nReaded>MAX_RESPONSE_BUFFER)
			return HGRC_UNKNOWN_ERROR;
		pPos[nReaded]='\0';	
		m_remainedDataLen = nReaded;
		//m_headSize+=m_remainedDataLen;
		voLog(LL_TRACE,"http.txt",m_responseBuffer);
		voLog(1,"WMSHead.txt","Get response,size=%d\n",m_remainedDataLen);
	}

	return HGRC_OK;
}
bool CWMSHttpCmdHandle::ParseContentLength(unsigned int* _contentLength)
{
	*_contentLength = m_contentLength;
	
	return true;
}