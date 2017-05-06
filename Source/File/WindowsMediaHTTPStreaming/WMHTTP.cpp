#include "WMHTTP.h"
#include <stdio.h>
#include <string.h>
#include "voLog.h"
#include "voOSFunc.h"
#include "voSource2.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

CWMHTTP::CWMHTTP(CWMHTTP::Delegate *pDelegate)
: CThread("CWMHTTP")
, m_eStateWMHTTP(WMHTTP_UnInit)
, m_pDelegate(pDelegate)
, m_uiPacketSize(0)
, m_LinkBW(0x7FFFFFFF)
, m_bBroadcast(VO_FALSE)
, m_bPipelined(VO_FALSE)
, m_llSeekSkipSize(0)
, m_llfromTime(0)
{
	memset(m_szURL, 0, 1024);
	memset(m_szGUIDClient, 0, 40);

	srand(0);

	for (int i = 0; i < 8; i++)
		sprintf((&m_szGUIDClient[i]), "%X", rand() % 16);

	memcpy(&m_szGUIDClient[8], "-", 1);

	for (int i = 0; i < 4; i++)
		sprintf((&m_szGUIDClient[9 + i]), "%X", rand() % 16);

	memcpy(&m_szGUIDClient[13], "-", 1);

	for (int i = 0; i < 4; i++)
		sprintf((&m_szGUIDClient[14 + i]), "%X", rand() % 16);

	memcpy(&m_szGUIDClient[18], "-", 1);

	for (int i = 0; i < 4; i++)
		sprintf((&m_szGUIDClient[19 + i]), "%X", rand() % 16);

	memcpy(&m_szGUIDClient[23], "-", 1);

	for (int i = 0; i < 4; i++)
		sprintf((&m_szGUIDClient[24 + i]), "%X", rand() % 16);

	sprintf( &m_szGUIDClient[28], "%X", (unsigned int)voOS_GetSysTime() );

	char*p = m_szGUIDClient;
	for (int i = 28; i < 8; i++)
	{
		if (0 == p[i])
			p[i] = '0';
	}
}


CWMHTTP::~CWMHTTP(void)
{
}


VO_VOID CWMHTTP::ThreadMain()
{
	VO_S32 iRet= doDescribe();
	if (iRet)
	{
		VOLOGE("!doDescribe failed");

		NotifyEvent(iRet, 0, 0);
		m_pDelegate->some(NULL, 0);
		m_eStateWMHTTP = WMHTTP_UnInit;

		return;
	}
	VOLOGR("doDescribe OK");

	iRet = doPipeline();
	if (iRet)
	{
		VOLOGE("!doPipeline failed");

		NotifyEvent(iRet, 0, 0);
		m_pDelegate->some(NULL, 0);
		m_eStateWMHTTP = WMHTTP_UnInit;

		return;
	}
	VOLOGR("doPipeline OK");

	VO_U32 nWaitTime = VOVO_SEM_MAXTIME;
	if (m_bPipelined)
		nWaitTime = 59500;

	do
	{
		m_eStateWMHTTP = WMHTTP_Inited;

		if (m_eventThreadLoop.Down(nWaitTime) == VOVO_SEM_TIMEOUT)
		{
			iRet = doKeepAlive();
			if (iRet)
			{
				VOLOGE("!doKeepAlive failed");

				NotifyEvent(iRet, 0, 0);
				m_pDelegate->some(NULL, 0);
				m_eStateWMHTTP = WMHTTP_UnInit;

				return;
			}
			VOLOGR("doKeepAlive OK");
		}
		else if (WMHTTP_Running == m_eStateWMHTTP)
		{
			iRet = doStart();
			if (iRet)
			{
				VOLOGE("!doStart failed");

				NotifyEvent(iRet, 0, 0);
				m_pDelegate->some(NULL, 0);
				m_eStateWMHTTP = WMHTTP_UnInit;

				return;
			}
			VOLOGR("doStart OK");

			iRet = ArrangeData();
			if (iRet)
			{
				VOLOGE("!ArrangeData");
				NotifyEvent(iRet, 0, 0);
				m_pDelegate->some(NULL, 0);
			}
			else
			{
				VOLOGR("ArrangeData OK");
				
				if (!m_bPipelined)
				{
					RequestInfo request;
					VO_CHAR szExtendHeaders[1024] = {0};

					m_HTTPTransaction.Reset();
					/*VO_S32 iReqSize = */sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nPragma: xClientGUID={%s}\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: client-id=%sPragma: client-lag=0\r\nPragma: stream-switch-count=1\r\nPragma: stream-switch-entry=ffff:2:1 \r\nPragma: playlist-gen-id=%s\r\nContent-Length: 0\r\n", m_szGUIDClient, m_clientID, m_playlist_gen_id);

					memset( &request, 0, sizeof(RequestInfo) );
					request.method			= HTTP_POST;
					request.szURL			= m_szURL;
					request.szExtendHeaders = szExtendHeaders;

					VO_S32 iRet = m_HTTPTransaction.Start(&request);
					if (iRet)
					{
						VOLOGE("HTTP Start");
						NotifyEvent(iRet, 0, 0);
						m_eStateWMHTTP = WMHTTP_UnInit;
						return;
					}

					VO_CHAR* szRespose = NULL;
					iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
					if (iRet) {
						VOLOGE("HTTP getResposeInfo %d", iRet);
						NotifyEvent(iRet, 0, 0);
						m_pDelegate->some(NULL, 0);
						m_eStateWMHTTP = WMHTTP_UnInit;
						return;
					}
				}
			}

			iRet = doLog();
			if (iRet)
			{
				VOLOGE("!doLog failed");

				NotifyEvent(iRet, 0, 0);
				m_pDelegate->some(NULL, 0);
				m_eStateWMHTTP = WMHTTP_UnInit;
			}
		}

		VOLOGR("-Loop %x", m_eStateWMHTTP);
	} while (WMHTTP_UnInit != m_eStateWMHTTP);

	m_eStateWMHTTP = WMHTTP_UnInit;
}

VO_S32 CWMHTTP::open(VO_CHAR *szURL)
{
	strcpy(m_szURL, szURL);

	return CThread::ThreadStart();
}

VO_S32 CWMHTTP::close()
{
	m_eStateWMHTTP = WMHTTP_UnInit;

	m_eventThreadLoop.Up();

	return WaitingThreadExit();
}


VO_S32 CWMHTTP::play(VO_S64 llfromTime/* = 0*/, VO_U64 llfromPos/* = 0xFFFFFFFFFFFFFFFF*/)
{
	m_llfromTime	= llfromTime;
	m_llfromPos		= (0xFFFFFFFFFFFFFFFFLL == llfromPos) ? llfromPos : llfromPos + m_llSeekSkipSize;

	VO_S32 iTimes = 0;
	while (WMHTTP_Inited != m_eStateWMHTTP)
	{
		voOS_Sleep(20);

		iTimes++;
		if (iTimes > 400)
		{
			VOLOGE ("The State is not Inited!");
			return -1;
		}
	}

	m_eStateWMHTTP = WMHTTP_Running;

	m_eventThreadLoop.Up();

	return 0;
}

VO_S32 CWMHTTP::stop(VO_S32 iMaxTime/* = 0x7fffffff*/)
{
	if (WMHTTP_Running != m_eStateWMHTTP) {
		VOLOGR("Not Playing");
		return 0;
	}

	if (m_bPipelined)
	{
		RequestInfo request;
		VO_CHAR szExtendHeaders[1024] = {0};

		/*VO_S32 iReqSize = */sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nPragma: xClientGUID={%s}\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: client-id=%s\r\nPragma: xStopStrm=1\r\nContent-Length: 0\r\n", m_szGUIDClient, m_clientID);

		memset( &request, 0, sizeof(RequestInfo) );
		request.method			= HTTP_POST;
		request.szURL			= m_szURL;
		request.szExtendHeaders = szExtendHeaders;
		
		VO_S32 iRet = m_HTTPTransaction.Start(&request);
		if (iRet) {
			VOLOGE("HTTP Start");
			NotifyEvent(iRet, 0, 0);
			return iRet;
		}
	}
	else
	{
		m_eStateWMHTTP = WMHTTP_Stopping;
	}

	VO_S32 iTimes = 0;
	while (WMHTTP_Running == m_eStateWMHTTP || WMHTTP_Stopping == m_eStateWMHTTP)
	{
		voOS_Sleep(20);

		iTimes++;
		if (20 * iTimes > iMaxTime)
		{
			VOLOGE ("The State is still Running!");
			return -1;
		}
	}

	return 0;
}

VO_S32 CWMHTTP::doDescribe()
{
	RequestInfo request;
	VO_CHAR szExtendHeaders[1024] = {0};

	memset(&request, 0, sizeof(RequestInfo));
	sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: version11-enabled=1\r\nPragma: no-cache,rate=1.000,stream-time=0,stream-offset=0:0,packet-num=4294967295,max-duration=0\r\nPragma: packet-pair-experiment=1\r\nPragma: pipeline-experiment=1\r\nSupported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\nPragma: xClientGUID={%s}\r\nAccept-Language: zh-CN, *;q=0.1\r\n", m_szGUIDClient);

	request.szURL			= m_szURL;
	request.szExtendHeaders	= szExtendHeaders;

	m_HTTPTransaction.Reset();
	VO_S32 iRet = m_HTTPTransaction.Start(&request);
	if (iRet) {
		VOLOGE("HTTP Start");
		return iRet;
	}

	VO_CHAR* szRespose = NULL;
	iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
	if (iRet) {
		VOLOGE("HTTP getResposeInfo %d", iRet);
		return iRet;
	}

	memset(m_clientID, 0, 32);
	VO_CHAR *p = strstr(szRespose, "client-id=");
	if (p)
		sscanf(p, "client-id=%[0-9],", m_clientID);
	else
		return -1;

	if ( strstr(szRespose, "broadcast") )
	{
		VOLOGR("broadcast/live");
		m_bBroadcast = VO_TRUE;
	}
	else
	{
		VOLOGR("on-demand");
	}

	p = strstr(szRespose, "Content-Type: ");
	p += strlen("Content-Type: ");
	if (0 != strncmp( p, "application/vnd.ms.wms-hdr.asfv1", strlen("application/vnd.ms.wms-hdr.asfv1") ) ) {
		VOLOGE("!application/vnd.ms.wms-hdr.asfv1");
		return -1;
	}

	iRet = PacketPairPacket(); //$P
	if (iRet) {
		VOLOGE("PacketPairPacket");
		return iRet;
	}

	iRet = MustRead(szRespose, 2); //$M
	if (iRet)
		return iRet;

	if (0x4d != szRespose[1])
		return -1;

	VO_S32 iLen = 0;
	iRet = MustRead((VO_CHAR *)&iLen, 2);
	if (iRet)
		return iRet;

	iRet = MustRead(szRespose, iLen);
	if (iRet)
		return iRet;

	for (VO_S32 i = 0; i < iLen; i++)
	{
		if ('p' == szRespose[i])
		{
			p = strstr(szRespose + i, "playlist-gen-id=");

			if (p)
				break;
		}
	}

	if (p)
		sscanf(p, "playlist-gen-id=%[0-9],", m_playlist_gen_id);
	else
		return -1;

	iRet = 0;
	do  {	//$H
		iRet = m_HTTPTransaction.ReadData(szExtendHeaders, 1024);
	} while (iRet > 0);

	return iRet;
}

VO_S32 CWMHTTP::DataPacket_forVOReader()
{
	VO_S32 iRet = MustSkip(8);
	if (iRet)
		return iRet;

	VO_S32 iPacketSize = 0;
	iRet = MustRead((char *)&iPacketSize, 2);
	if (iRet)
		return iRet;

	iRet = MustSave(iPacketSize - 8);
	if (iRet)
		return iRet;

	return 0;
}

VO_S32 CWMHTTP::DataPacket()
{
	VO_S32 iRet = 0;

	if (m_llfromTime)
	{
		iRet = MustSkip(2);
		if (iRet)
			return iRet;

		VO_S32 iNumASFPacket = 0;
		iRet = MustRead((VO_CHAR *)&iNumASFPacket, 4);
		if (iRet)
			return iRet;

		m_llSeekSkipSize = iNumASFPacket * m_uiPacketSize;

		iRet = MustSkip(4);
		if (iRet)
			return iRet;

		m_llfromTime = 0;
	}
	else
	{
		iRet = MustSkip(10);
		if (iRet)
			return iRet;
	}


	VO_CHAR LengthTypeFlags = 0;
	iRet = MustRead(&LengthTypeFlags, 1);
	if (iRet)
		return iRet;

	if (m_pDelegate)
		m_pDelegate->some(&LengthTypeFlags, 1);
	else
		return -1;

	if (LengthTypeFlags & 0x80)
	{
		iRet = MustSave(LengthTypeFlags & 0xF);
		if (iRet)
			return iRet;

		iRet = MustRead(&LengthTypeFlags, 1);
		if (iRet)
			return iRet;
	}

	if (LengthTypeFlags & 0x60)
	{
		VO_CHAR PropertyFlags = 0;
		iRet = MustRead(&PropertyFlags, 1) ;
		if (iRet)
			return iRet;

		unsigned uiPacketLen = 0;
		iRet = MustRead((VO_CHAR *)&uiPacketLen, (LengthTypeFlags >> 5) & 0x3);
		if (iRet)
			return iRet;

		unsigned PaddingLen = m_uiPacketSize - uiPacketLen + ((LengthTypeFlags >> 5) & 0x3);
		VO_CHAR i = 0;
		if (PaddingLen > 65537)
		{
			PaddingLen -= 4;
			i = 0x18;
		}
		else if (PaddingLen > 256)
		{
			PaddingLen -= 2;
			i = 0x10;
		}
		else if (PaddingLen > 0)
		{
			PaddingLen -= 1;
			i = 0x8;
		}

		i += (LengthTypeFlags & 1);
		m_pDelegate->some(&i, 1);

		m_pDelegate->some(&PropertyFlags, 1); //5d

		m_pDelegate->some( (VO_CHAR *)&PaddingLen, ((i >> 3) & 0x3) );

		iRet = MustSave(uiPacketLen - 7);
		if (iRet)
			return iRet;

		VO_CHAR sz[1024] = {0};
		while (PaddingLen)
		{
			VO_U32 iLen = PaddingLen < 1024 ? PaddingLen : 1024;

			m_pDelegate->some(sz, iLen);
			PaddingLen -= iLen;
		}
	}
	else
	{
		m_pDelegate->some(&LengthTypeFlags, 1);

		iRet = MustSave(m_uiPacketSize - 4);
		if (iRet)
			return iRet;
	}

	return 0;
}

VO_S32 CWMHTTP::EndPacket()
{
	return 0;
}

VO_S32 CWMHTTP::HeaderPacket()
{
	VO_S32 iLen = 0;
	VO_S32 iRet = MustRead((VO_CHAR *)&iLen, 2);
	if (iRet)
		return iRet;

	iRet = MustSkip(8);
	if (iRet)
		return iRet;

	VO_CHAR *pBuf = new VO_CHAR[iLen - 8];

	iRet = MustRead(pBuf, iLen - 8);
	if (iRet) {
		delete []pBuf;
		return iRet;
	}

	VO_BYTE *pBegin = (VO_BYTE *)pBuf;
	VO_BYTE *pEnd = pBegin + (iLen - 8);
	do //a1 dc ab 8c 47 a9 cf 11 8e e4 00 c0 0c 20 53 65 
	{
		if (0xA1 != pBegin[0] ) {
			pBegin++;
			continue;//second
		}

		if (0xDC != pBegin[1]) {
			pBegin++;
			continue;//second
		}

		if (0xAB != pBegin[2]) {
			pBegin += 2; //third
			continue;
		}

		if (0x8C != pBegin[3]) {
			pBegin += 3;//forth
			continue;
		}

		if (0x47 != pBegin[4]) {
			pBegin += 4;//fifth
			continue;
		}

		if (0xA9 != pBegin[5]) {
			pBegin += 5;//six
			continue;
		}

		if (0xCF != pBegin[6]) {
			pBegin += 6;//seven
			continue;
		}

		if (0x11 != pBegin[7]) {
			pBegin += 7;//eight
			continue;
		}

		if (0x8E != pBegin[8]) {
			pBegin += 8;//nine
			continue;
		}

		if (0xE4 != pBegin[9]) {
			pBegin += 9;//ten
			continue;
		}

		if (0x00 != pBegin[10]) {
			pBegin += 10;//fifth
			continue;
		}

		if (0xC0 != pBegin[11]) {
			pBegin += 11;//six
			continue;
		}

		if (0x0C != pBegin[12]) {
			pBegin += 12;//seven
			continue;
		}

		if (0x20 != pBegin[13]) {
			pBegin += 13;//eight
			continue;
		}

		if (0x53 != pBegin[14]) {
			pBegin += 14;//nine
			continue;
		}

		if (0x65 != pBegin[15]) {
			pBegin += 15;//ten
			continue;;
		}
		else
			break;

	} while (pBegin + 16 != pEnd);

	pBegin += 92;

	unsigned min_data_packet_size = 0;
	unsigned max_data_packet_size = 0;

	memcpy(&min_data_packet_size, pBegin, 4);
	memcpy(&max_data_packet_size, pBegin + 4, 4);

	if (min_data_packet_size == max_data_packet_size)
		m_uiPacketSize = min_data_packet_size;
	else
	{
		delete []pBuf;
		return -1;
	}

	if (m_pDelegate)
		m_pDelegate->some(pBuf, iLen - 8);//header len
	else
	{
		delete []pBuf;
		return -1;
	}

	delete []pBuf;

	return 0;
}

VO_S32 CWMHTTP::MetadataPacket()
{
	VO_S32 iLen = 0;
	VO_S32 iRet = MustRead((VO_CHAR *)&iLen, 2);
	if (iRet)
		return iRet;

	iRet = MustSkip(iLen);
	if (iRet)
		return iRet;

	return 0;
}

VO_S32 CWMHTTP::PacketPairPacket()
{
	VO_S32 iRet = MustSkip(8); //$P1
	if (iRet)
		return iRet;

	VO_U32 t0 = voOS_GetSysTime();//millisecond

	iRet = MustSkip(8 + 504); //$P2
	if (iRet)
		return iRet;

	VO_U32 D_Value = voOS_GetSysTime() - t0;
	if (D_Value)
		m_LinkBW = 512 * 8 * 1000 / D_Value;

	iRet = MustSkip(8 + 1048 + 504); //$P3
	if (iRet)
		return iRet;

	return 0;
}

VO_S32 CWMHTTP::TestPacket()
{
	return 0;
}

VO_S32 CWMHTTP::doPipeline()
{
	RequestInfo request;
	VO_CHAR szExtendHeaders[1024] = {0};

	memset( &request, 0, sizeof(RequestInfo) );
	/*VO_S32 iReqSize = */sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nPragma: no-cache\r\nPragma: pipeline-request=1\r\nPragma: client-id=%s\r\nConnection: keep-alive\r\n", m_clientID);
	request.szURL			= m_szURL;
	request.szExtendHeaders = szExtendHeaders;

	VO_S32 iRet = m_HTTPTransaction.Start(&request);
	if (iRet) {
		VOLOGE("HTTP Start");
		return iRet;
	}

	VO_CHAR* szRespose = NULL;
	iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
	if (iRet) {
		VOLOGE("HTTP getResposeInfo %d", iRet);
		return iRet;
	}

	//iRet = m_HTTPTransaction.ReadData(szRespose, 1024 * 16); //$T
	//---
	iRet = m_HTTPTransaction.Start(&request);
	if (iRet) {
		VOLOGE("HTTP Start");
		return iRet;
	}

	iRet = 0;
	do {
		iRet = m_HTTPTransaction.ReadData(szExtendHeaders, 1024);
	} while (iRet > 0);

	//iRet = m_HTTPTransaction.ReadData(szRespose, 1024 * 16);//End: 30 0d 0a 0d 0a
	iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
	if (iRet) {
		VOLOGE("HTTP getResposeInfo %d", iRet);
		return iRet;
	}

	if ( strstr(szRespose, "pipeline-result=1") )
		m_bPipelined = VO_TRUE;

	return 0;
}

VO_S32 CWMHTTP::doStart()
{
	RequestInfo request;
	VO_CHAR szExtendHeaders[1024] = {0};

	unsigned i1, i2;
	i1 = i2 = 0;
	char *pfromPos = (char*)&m_llfromPos;

	memcpy(&i1, pfromPos + 4, 4);
	memcpy(&i2, pfromPos, 4);

	memset( &request, 0, sizeof(RequestInfo) );
	if (m_bBroadcast)
	{
		if (m_bPipelined)
		{
			sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: version11-enabled=1\r\nPragma: no-cache,rate=1.000,stream-time=0,stream-offset=4294967295:4294967295,packet-num=4294967295,max-duration=0\r\nPragma: xPlayStrm=1\r\nPragma: client-id=%s\r\nPragma: LinkBW=%d, AccelBW=2147483647, AccelDuration=18000\r\nSupported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\nPragma: playlist-gen-id=%s\r\nPragma: xClientGUID={%s}\r\nPragma: stream-switch-count=2\r\nPragma: stream-switch-entry=ffff:1:0 ffff:2:0 \r\nAccept-Language: zh-cn, *;q=0.1\r\n", m_clientID, m_LinkBW, m_playlist_gen_id, m_szGUIDClient);
		}
		else
		{
			VOLOGR("nonepipeline mode");
			m_HTTPTransaction.Reset();

			m_LinkBW = 3670016;
			sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: no-cache,rate=1.000,stream-time=0,stream-offset=4294967295:4294967295,packet-num=4294967295,max-duration=0\r\nPragma: xPlayStrm=1\r\nPragma: client-id=%s\r\nPragma: LinkBW=%d, AccelBW=2147483647, AccelDuration=18000\r\nSupported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.predstrm, com.microsoft.wm.startupprofile\r\nPragma: playlist-gen-id=%s\r\nPragma: xClientGUID={%s}\r\nPragma: stream-switch-count=2\r\nPragma: stream-switch-entry=ffff:1:0 ffff:2:0 \r\nAccept-Language: zh-cn, *;q=0.1\r\n", m_clientID, m_LinkBW, m_playlist_gen_id, m_szGUIDClient);
		}
	}
	else
	{
		if (m_bPipelined)
		{
			sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: version11-enabled=1\r\nPragma: no-cache,rate=1.000,stream-time=%lld,stream-offset=%u:%u,packet-num=4294967295,max-duration=0\r\nPragma: xPlayStrm=1\r\nPragma: client-id=%s\r\nPragma: LinkBW=%d, AccelBW=2147483647, AccelDuration=18000, Speed=1.070\r\nSupported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.startupprofile\r\nPragma: playlist-gen-id=%s\r\nPragma: xClientGUID={%s}\r\nPragma: stream-switch-count=2\r\nPragma: stream-switch-entry=ffff:1:0 ffff:2:0 \r\nAccept-Language: zh-cn, *;q=0.1\r\n", m_llfromTime, i1, i2, m_clientID, m_LinkBW, m_playlist_gen_id, m_szGUIDClient);
		}
		else
		{
			VOLOGR("nonepipeline mode");
			m_HTTPTransaction.Reset();

			m_LinkBW = 3670016;
			sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: no-cache,rate=1.000,stream-time=%lld,stream-offset=%u:%u,packet-num=4294967295,max-duration=0\r\nPragma: xPlayStrm=1\r\nPragma: client-id=%s\r\nPragma: LinkBW=%d, AccelBW=2147483647, AccelDuration=10000\r\nSupported: com.microsoft.wm.srvppair, com.microsoft.wm.sswitch, com.microsoft.wm.startupprofile\r\nPragma: playlist-gen-id=%s\r\nPragma: xClientGUID={%s}\r\nPragma: stream-switch-count=2\r\nPragma: stream-switch-entry=ffff:1:0 ffff:2:0 \r\nAccept-Language: zh-cn, *;q=0.1\r\n", m_llfromTime, i1, i2, m_clientID, m_LinkBW, m_playlist_gen_id, m_szGUIDClient);
		}
	}

	request.szURL			= m_szURL;
	request.szExtendHeaders = szExtendHeaders;

	VO_S32 iRet = m_HTTPTransaction.Start(&request);
	if (iRet) {
		VOLOGE("HTTP Start");
		return iRet;
	}

	VO_CHAR* szRespose = NULL;
	iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
	if (iRet) {
		VOLOGE("HTTP getResposeInfo %d", iRet);
		return iRet;
	}

	VO_CHAR *p = strstr(szRespose, "Content-Type: ");
	if (p)
		p += strlen("Content-Type: ");
	else
		return -1;

	if (0 != strncmp( p, "application/x-mms-framed", strlen("application/x-mms-framed") ) ) {
		VOLOGE("!application/x-mms-framed");
		return -1;
	}

	m_eStateWMHTTP = WMHTTP_Running;

	return 0;
}

VO_S32 CWMHTTP::ArrangeData()
{
	VO_CHAR Buf[64] = {0};
	VO_S32 iRet = 0;

	do 
	{
		int iNeed = 2;
		do 
		{
			iRet = m_HTTPTransaction.ReadData(Buf + 2 - iNeed, iNeed);
			if (iRet < 0)
			{
				VOLOGE("!HTTP ReadData");
				return iRet;
			}
			else if (0 == iRet)
			{
				VO_CHAR* szRespose = NULL;
				iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
				if (iRet) {
					VOLOGE("HTTP getResposeInfo %d", iRet);
					m_pDelegate->some(NULL, 0);
					m_eStateWMHTTP = WMHTTP_UnInit;
				}

				return iRet;
			}

			iNeed-= iRet;
		} while (iNeed);

		if ( (*Buf & 0x7f) != 0x24 )
		{
			VOLOGE("!Framing Header");
			return -1;
		}

		switch ( *(Buf + 1) )
		{
		case 0x44: //$D
			{
				iRet = DataPacket(); //DataPacket_forVOReader()
				if (iRet) {
					VOLOGE("!DataPacket");
					return iRet;
				}

				break;
			}
		case 0x48: //$H
			{
				if (0xFFFFFFFFFFFFFFFFLL == m_llfromPos)
				{
					iRet = HeaderPacket();
					if (iRet) {
						VOLOGE("!HeaderPacket");
						return iRet;
					}

					break;
				}
			}
		case 0x4d: //$M
			{
				iRet = MetadataPacket();
				if (iRet) {
					VOLOGE("!MetadataPacket");
					return iRet;
				}

				break;
			}
		case 0x43: //C
			{
				iRet = MustSkip(6);
				if (iRet) {
					VOLOGE("!MustSkip");
					return iRet;
				}

				break;
			}
		case 0x45: //E
			{
				VOLOGR("$E");
			}
		default:
			{
				VOLOGR("%x", *(Buf + 1));

				m_pDelegate->some(NULL, 0);

				VO_S32 iRet = 0;
				do {
					iRet = m_HTTPTransaction.ReadData(Buf, 64);
				} while (iRet > 0);

				return iRet;
			}
		}
	} while (WMHTTP_Running == m_eStateWMHTTP);
	return 0;
}

VO_S32 CWMHTTP::MustRead(VO_CHAR *pBuf, VO_S32 iSize)
{
	VO_S32 iRead = 0;

	while(iSize)
	{
		VO_S32 iRet = m_HTTPTransaction.ReadData(pBuf + iRead, iSize);
		if (iRet <= 0)
			return iRet;

		iRead += iRet;
		iSize -= iRet;
	}

	return 0;
}

VO_S32 CWMHTTP::MustSave(VO_S32 iSize)
{
	VO_CHAR Buf[1024 * 4] = {0};

	do
	{
		VO_S32 iRet = m_HTTPTransaction.ReadData(Buf, 1024 * 4 < iSize ? 1024 *4 :iSize );
		if (iRet <= 0)
			return iRet;

		m_pDelegate->some(Buf, iRet);

		iSize -= iRet;
	} while (iSize);

	return 0;
}

VO_S32 CWMHTTP::MustSkip(VO_S32 iSize)
{
	VO_CHAR Buf[1024] = {0};

	do
	{
		VO_S32 iRet = m_HTTPTransaction.ReadData(Buf, 1024 < iSize ? 1024 :iSize );
		if (iRet <= 0)
			return iRet;

		iSize -= iRet;
	} while (iSize);

	return 0;
}

VO_S32 CWMHTTP::doLog()
{
	RequestInfo request;
	VO_CHAR szExtendHeaders[1024] = {0};

	memset( &request, 0, sizeof(RequestInfo) );
	request.method			= HTTP_POST;
	request.szURL			= m_szURL;
	request.szBody			= (char*)"\n<XML>\n<Summary>0.0.0.0 2011-04-25 01:57:50 - http://WIN-BE5N5O4QHH6/PublishingPoint2 0 4 1 200 {3300AD50-2C39-46c0-AE0A-F702E0EFCC30} 12.0.7601.17514 zh-CN WMFSDK/12.0.7601.17514_WMPlayer/12.0.7601.17514 - wmplayer.exe 12.0.7601.17514 Windows 6.1.0.7601 Pentium 35 1267756 313441 http TCP Windows_Media_Audio_9 Windows_Media_Video_9 - - 269832 - 177 0 0 0 0 0 0 1 5 100 - - - - http://WIN-BE5N5O4QHH6/PublishingPoint2 industrial.wmv - </Summary>\n<c-ip>0.0.0.0</c-ip>\n<date>2011-04-25</date>\n<time>01:57:50</time>\n<c-dns>-</c-dns>\n<cs-uri-stem>http://WIN-BE5N5O4QHH6/PublishingPoint2</cs-uri-stem>\n<c-starttime>0</c-starttime>\n<x-duration>4</x-duration>\n<c-rate>1</c-rate>\n<c-status>200</c-status>\n<c-playerid>{3300AD50-2C39-46c0-AE0A-F702E0EFCC30}</c-playerid>\n<c-playerversion>12.0.7601.17514</c-playerversion>\n<c-playerlanguage>zh-CN</c-playerlanguage>\n<cs-User-Agent>WMFSDK/12.0.7601.17514_WMPlayer/12.0.7601.17514</cs-User-Agent>\n<cs-Referer>-</cs-Referer>\n<c-hostexe>wmplayer.exe</c-hostexe>\n<c-hostexever>12.0.7601.17514</c-hostexever>\n<c-os>Windows</c-os>\n<c-osversion>6.1.0.7601</c-osversion>\n<c-cpu>Pentium</c-cpu>\n<filelength>35</filelength>\n<filesize>1267756</filesize>\n<avgbandwidth>313441</avgbandwidth>\n<protocol>http</protocol>\n<transport>TCP</transport>\n<audiocodec>Windows_Media_Audio_9</audiocodec>\n<videocodec>Windows_Media_Video_9</videocodec>\n<c-channelURL>-</c-channelURL>\n<sc-bytes>-</sc-bytes>\n<c-bytes>269832</c-bytes>\n<s-pkts-sent>-</s-pkts-sent>\n<c-pkts-received>177</c-pkts-received>\n<c-pkts-lost-client>0</c-pkts-lost-client>\n<c-pkts-lost-net>0</c-pkts-lost-net>\n<c-pkts-lost-cont-net>0</c-pkts-lost-cont-net>\n<c-resendreqs>0</c-resendreqs>\n<c-pkts-recovered-ECC>0</c-pkts-recovered-ECC>\n<c-pkts-recovered-resent>0</c-pkts-recovered-resent>\n<c-buffercount>1</c-buffercount>\n<c-totalbuffertime>5</c-totalbuffertime>\n<c-quality>100</c-quality>\n<s-ip>-</s-ip>\n<s-dns>-</s-dns>\n<s-totalclients>-</s-totalclients>\n<s-cpu-util>-</s-cpu-util>\n<cs-url>http://WIN-BE5N5O4QHH6/PublishingPoint2</cs-url>\n<ContentDescription>\n<WMS_CONTENT_DESCRIPTION_SERVER_BRANDING_INFO>WMServer/9.5</WMS_CONTENT_DESCRIPTION_SERVER_BRANDING_INFO>\n<copyright>Copyright_(C)_Microsoft_Corporation._All_rights_reserved.</copyright>\n<author>Microsoft_Corporation</author>\n<title>Industrial_WM_9_Series</title>\n<WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_START_OFFSET>3000</WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_START_OFFSET>\n<WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_DURATION>34874</WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_DURATION>\n<WMS_CONTENT_DESCRIPTION_COPIED_METADATA_FROM_PLAYLIST_FILE>1</WMS_CONTENT_DESCRIPTION_COPIED_METADATA_FROM_PLAYLIST_FILE>\n<WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_URL>industrial.wmv</WMS_CONTENT_DESCRIPTION_PLAYLIST_ENTRY_URL>\n</ContentDescription>\n</XML>";
	/*VO_S32 iReqSize = */sprintf(szExtendHeaders, "Accept: */*\r\nUser-Agent: NSPlayer/12.0.7601.17514\r\nPragma: xClientGUID={%s}\r\nX-Accept-Authentication: Negotiate, NTLM, Digest, Basic\r\nPragma: client-id=%s\r\nContent-Length: %d\r\nContent-Type: application/x-wms-LogStats;charset=UTF-8\r\n", m_szGUIDClient, m_clientID, strlen(request.szBody));
	request.szExtendHeaders = szExtendHeaders;

	VO_S32 iRet = m_HTTPTransaction.Start(&request);
	if (iRet)
	{
		VOLOGE("!HTTP Start %d", iRet);
		return iRet;
	}

	VO_CHAR* szRespose = NULL;
	iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
	if (iRet)
	{
		VOLOGE("!HTTP getResposeInfo %d", iRet);
	}

	m_HTTPTransaction.Reset();

	return iRet;
}

VO_S32 CWMHTTP::doKeepAlive()
{
	RequestInfo request;
	VO_CHAR szExtendHeaders[1024] = {0};

	memset( &request, 0, sizeof(RequestInfo) );
	sprintf(szExtendHeaders, "User-Agent: NSPlayer/12.0.7601.17514\r\nPragma: client-id=%s\r\n", m_clientID);
	request.method			= HTTP_OPTIONS;
	request.szURL			= m_szURL;
	request.szExtendHeaders = szExtendHeaders;

	VO_S32 iRet = m_HTTPTransaction.Start(&request);
	if (iRet)
	{
		VOLOGE("!HTTP Start %d", iRet);
		return iRet;
	}

	VO_CHAR* szRespose = NULL;
	iRet = m_HTTPTransaction.getResposeInfo(&szRespose);
	if (iRet)
	{
		VOLOGE("!HTTP getResposeInfo %d", iRet);
	}

	return iRet;
}

VO_VOID CWMHTTP::NotifyEvent(VO_U32 nID, VO_U32 nParam1, VO_U32 nParam2)
{
	VO_U32 uCBType = 0;
	if (E_TCP_Base == nID)
		uCBType = VO_EVENTID_SOURCE2_ERR_CONNECTFAIL;
	else
		uCBType = VO_EVENTID_SOURCE2_ERR_DOWNLOADFAIL;

	m_pDelegate->NotifyEvent(uCBType, nParam1, nParam2);
}