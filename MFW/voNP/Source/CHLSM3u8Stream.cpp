//
//  CHLSM3u8Stream.cpp
//  
//
//  Created by Jim Lin on 9/11/12.
//
//

#include "CHLSM3u8Stream.h"
#include "voNPWrap.h"
#include "voLog.h"
#include "voOSFunc.h"

#include <ifaddrs.h>
#include <arpa/inet.h>


#define M3U8_FILE_START         "#EXTM3U\n"
#define M3U8_VERSION            "#EXT-X-VERSION:3\n"
#define M3U8_CACHE              "#EXT-X-ALLOW-CACHE:YES\n"
#define M3U8_TARGET_DURATION    "#EXT-X-TARGETDURATION:%d\n"
#define M3U8_SEQUENCE           "#EXT-X-MEDIA-SEQUENCE:%d\n"
#define M3U8_INFO               "#EXTINF:%d,\n"
#define M3U8_FILE_END           "#EXT-X-ENDLIST\n"
#define M3U8_DISCONTINUITY      "#EXT-X-DISCONTINUITY\n"


#define M3U8_CHUNK_MAX_COUNT    3
#define CHUNK_URI_PREFIX        "fileSequence_"

// in seconds, default chunk duration is 2 second,but it will be changed to 1 seconds after playback begins
// The reason why do this is to increase buffer depth in AVPlayer at the beginning of playback,decrease buffering time,especially seeking
#define CHUNK_DURATION          2

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

int CHLSM3u8Stream::m_nPortNumber = 3000;

CHLSM3u8Stream::CHLSM3u8Stream (void * hUserData)
:CHLSHttpStream(hUserData)
,m_nTargetDuration(CHUNK_DURATION)
,m_nCurrStreamIdx(0)
,m_bLastChunk(false)
,m_nNotUpdateM3u8Time(0)
{
    m_bM3u8Stream = true;
    strcpy(m_szPlaybackURL, "");
    
    memset(m_szIPAddr, 0, 128);
    strcpy(m_szIPAddr, "127.0.0.1");
}

CHLSM3u8Stream::~CHLSM3u8Stream (void)
{
}

bool CHLSM3u8Stream::Open (void * pSource, int nFlag)
{
    int nRet = VONP_ERR_None;
    
    nRet = CHLSHttpStream::Open(pSource, nFlag);
    
    CreateRandomURL();
    
    m_nNotUpdateM3u8Time = voOS_GetSysTime();
    
    return nRet;
}

bool CHLSM3u8Stream::Close (void)
{
    m_nCurrStreamIdx = 0;
    m_M3u8List.RemoveAll();
    m_AvailableStreamList.RemoveAll();

    return CHLSHttpStream::Close();
}

void CHLSM3u8Stream::CreateRandomURL()
{
    memset(m_szPlaybackURL, 0, 1024);
    //sprintf(m_szPlaybackURL, "http://127.0.0.1:%d/", m_nPortNumber);
    
    if(strlen(m_szIPAddr) > 0)
        sprintf(m_szPlaybackURL, "http://%s:%d/", m_szIPAddr, m_nPortNumber);
    else
        sprintf(m_szPlaybackURL, "http://%s:%d/", GetIPAddress(), m_nPortNumber);
    
#if 1
    for(int n=0; n<10; n++)
    {
        char szTmp[4];
        memset(szTmp, 0, 4);
        sprintf(szTmp, "%02d_", rand()%10);
        vostrcat(m_szPlaybackURL, szTmp);
    }
#endif
    
    vostrcat(m_szPlaybackURL, "playback.m3u8");
    
    memset(m_szSourceURL, 0, 1024);
    strcpy(m_szSourceURL, m_szPlaybackURL);
    
    return;
    
    
    //test code
    //const char* pszTmp = "http://devimages.apple.com/iphone/samples/bipbop/bipbopall.m3u8";
    //const char* pszTmp = "http://10.2.64.42:8082/hls/multibitrate/troy5677_clear_10min/hls.m3u8";
    //const char* pszTmp = "http://akamedia2.lsops.net/live/smil:bbcworld1_en.smil/playlist.m3u8";
    
    //close caption
    //const char* pszTmp = "http://10.2.64.42:8082/hls/closedcaption/cc_special/177C_640x480_612K_29f.m3u8";
    //const char* pszTmp = "http://10.2.64.42:8082/hls/closedcaption/closedcaption/43C_480x320_824K.m3u8";
    //const char* pszTmp = "http://10.2.64.42:8082/hls/videoonly/stream/videoonly.m3u8";
    const char* pszTmp = "http://127.0.0.1:8080/playback.m3u8";
    //const char* pszTmp = "http://192.168.0.115/playback.m3u8";
    
    memset(m_szPlaybackURL, 0, 1024);
    vostrcpy(m_szPlaybackURL, pszTmp);

    memset(m_szPlaybackURL, 0, 1024);
    vostrcpy(m_szPlaybackURL, pszTmp);
}

char* CHLSM3u8Stream::GetM3u8URL()
{
    return m_szPlaybackURL;
}


void CHLSM3u8Stream::AddStream(CHLSHttpStream* pStream)
{
    voCAutoLock lock(&m_mtxStream);
    
    //change URI
    char szURI[128];
    memset(szURI, 0, 128);
    sprintf(szURI, "%s%d.ts", CHUNK_URI_PREFIX, m_nCurrStreamIdx);
    pStream->SetSourceURL(szURI);
    
    //add
    pStream->SetIdx(m_nCurrStreamIdx);
    pStream->SetAvailable(true);
    m_AvailableStreamList.AddTail(pStream);
        
    DumpChunk(pStream);
    
    //VOLOGI("[NPW]Add stream %s, id %d, size %lld, duration %d, total %d", pStream->GetSourceURL(), pStream->GetID(), pStream->Size(), pStream->GetDuration(), m_AvailableStreamList.GetCount());
    //VOLOGI("[NPW]Add stream duration %d", pStream->GetDuration());
    
    m_nCurrStreamIdx++;
}

void CHLSM3u8Stream::DumpChunk(CHLSHttpStream* pStream)
{
    static int nWrite = 0;
	
	// check file exist
	if(nWrite == 0)
	{
		VO_S32 nCheckBackDoor = voOS_EnableDebugMode(1);
		
		if(nCheckBackDoor > 0)
		{
#if defined(_IOS) || defined(_MAC_OS)
            char szDir[1024];
            voOS_GetAppFolder(szDir, 1024);
            strcat(szDir, "voDebugFolder/");
            strcat(szDir, "ts_chunk.dat");
            FILE* hCheck = fopen(szDir, "rb");
#else
			FILE* hCheck = fopen("/data/local/tmp/ts_chunk.dat", "rb");
#endif
            
			if(hCheck)
			{
				nWrite = 1;
				fclose(hCheck);
				hCheck = NULL;
			}
			else
				nWrite = -1;
		}
		else
		{
			nWrite = -1;
		}
	}
	
	if(nWrite == 1)
	{
        char szTmp[1024];
        memset(szTmp, 0, 1024);
        voOS_GetAppFolder(szTmp, 1024);
        strcat(szTmp, "voDebugFolder/chunk/");
        strcat(szTmp, pStream->GetSourceURL());
        
        FILE* hFile = fopen(szTmp, "wb");
        
        if(!hFile)
        {
            VOLOGI("[NPW]ts dump file failed. %s", szTmp);
            return;
        }
        
        if(pStream && pStream->GetBuffer() && pStream->Size()>0)
        {
            fwrite(pStream->GetBuffer(), 1, pStream->Size(), hFile);
        }		
        
        fclose(hFile);
	}
	// dump info end

    return;
}

void CHLSM3u8Stream::DumpStreamListInfo(bool bM3u8List)
{return;
    voCAutoLock lock(&m_mtxStream);
    
    CObjectList<CHLSHttpStream> * pList = bM3u8List?&m_M3u8List:&m_AvailableStreamList;
    
    VOLOGI("");
    
    if(bM3u8List)
    {
        VOLOGI("[NPW]------------------------------------- Dump m3u8 list ------------------------------------ START");
    }
    else
    {
        VOLOGI("[NPW]------------------------------------- Dump available stream list ------------------------ START");
    }
    
    POSITION pPos               = pList->GetHeadPosition();
    CHLSHttpStream* pStream     = (CHLSHttpStream*)pList->Get(pPos);
    
    char szTmp[1024];
    memset(szTmp, 0, 1024);

    while (pStream)
    {
        memset(szTmp, 0, 1024);
        
        VOLOGI("[NPW]idx %04d, id %02d, URI %s, size %lld", pStream->GetIdx(), pStream->GetID(), pStream->GetSourceURL(), pStream->Size());

        pPos        = pList->Next(pPos);
        pStream     = (CHLSHttpStream*)pList->Get(pPos);
    }
    
    if(bM3u8List)
    {
        VOLOGI("[NPW]------------------------------------- Dump m3u8 list ------------------------------------ END");
    }
    else
    {
        VOLOGI("[NPW]------------------------------------- Dump available stream list ------------------------ END");
    }
}

void CHLSM3u8Stream::RemoveStream(CHLSHttpStream* pStream)
{
    voCAutoLock lock(&m_mtxStream);
    
    //if(m_StreamList.GetCount() > M3U8_MAX_COUNT)
    {
        bool bRet = m_AvailableStreamList.Remove(pStream);
                
        if(bRet)
        {
            //VOLOGI("[NPW]Remove stream : id %d, %s", pStream->GetID(), pStream->GetSourceURL());
            pStream->SetFlag(STREAM_FLAG_UNKNOWN);
        }
        else
        {
            //VOLOGW("[NPW]Remove stream failed!!! id %d, %s", pStream->GetID(), pStream->GetSourceURL());
        }
    }
}


int CHLSM3u8Stream::GetChunkDuration()
{
    return m_nTargetDuration;
}

void CHLSM3u8Stream::SetChunkDuration(int nDuration)
{
    VOLOGI("[NPW]Update chunk duration %d", nDuration);
    m_nTargetDuration = nDuration;
}

void CHLSM3u8Stream::UpdateM3U8(int nUpdateCount)
{
    voCAutoLock lock(&m_mtxStream);
    
    //Close();
    
    //prepare the m3u8 list
    if (m_M3u8List.GetCount() > 0)
    {
        if(m_M3u8List.GetCount() < nUpdateCount)
        {
            VOLOGE("[NPW]Error update m3u8 count");
        }
        
        for(int n=0; n<nUpdateCount; n++)
        {
            CHLSHttpStream* pRemoved = m_M3u8List.RemoveHead();
            
            if(pRemoved)
            {
                // stream's buffer can be re-write
                pRemoved->SetAvailable(false);
                //VOLOGI("[NPW]Stream can be re-write: id %d, %s", pRemoved->GetID(), pRemoved->GetSourceURL());
            }
        }
    }
    
    int nSuccessCount           = 0;
    POSITION pPos               = m_AvailableStreamList.GetHeadPosition();
    CHLSHttpStream* pStream     = (CHLSHttpStream*)m_AvailableStreamList.Get(pPos);
    //for (int n=0; n<nUpdateCount; n++)
    while (pStream && nSuccessCount<nUpdateCount)
    {
        if(!m_M3u8List.Find(pStream))
        {
            nSuccessCount++;
            m_M3u8List.AddTail(pStream);
            //VOLOGI("[NPW]Add stream to m3u8 list: id %d, %s", pStream->GetID(), pStream->GetSourceURL());
        }
        
        pPos        = m_AvailableStreamList.Next(pPos);
        pStream     = (CHLSHttpStream*)m_AvailableStreamList.Get(pPos);
    }
    
    if(m_M3u8List.GetCount() != M3U8_CHUNK_MAX_COUNT)
    {
        VOLOGE("[NPW]+++++++++++++++++00 Prepare m3u8 list fatal error!!! %d+++++++++++++++", m_M3u8List.GetCount());
        DumpStreamListInfo(true);
        DumpStreamListInfo(false);
        return;
    }
    
    DumpStreamListInfo(true);
    DumpStreamListInfo(false);
    
    bool bLive          = true;
    int nMaxBufSize     = 1024*2;
    int nMaxCount       = M3U8_CHUNK_MAX_COUNT;
    
    int nWrite          = 0;
    char szTmp[1024];
    
    unsigned char* pBuffer = new unsigned char[nMaxBufSize];
    //unsigned char* pBuffer = AllocBuffer(nMaxBufSize);
    memset(pBuffer, 0, nMaxBufSize);
    
    char* pCurr = (char*)pBuffer;
    
    if(bLive)
    {
        //write head
        strcpy(pCurr, M3U8_FILE_START);
        nWrite += strlen(M3U8_FILE_START);
        
        strcpy(pCurr+nWrite, M3U8_VERSION);
        nWrite += strlen(M3U8_VERSION);
        
        //strcpy(pCurr+nWrite, M3U8_CACHE);
        //nWrite += strlen(M3U8_CACHE);
        
        memset(szTmp, 0, 1024);
        //sprintf(szTmp, M3U8_TARGET_DURATION, m_nTargetDuration);
        sprintf(szTmp, M3U8_TARGET_DURATION, GetMaxDuration());
        strcpy(pCurr+nWrite, szTmp);
        nWrite += strlen(szTmp);
        
        POSITION pPos               = m_M3u8List.GetHeadPosition();
        CHLSHttpStream* pStream     = (CHLSHttpStream*)m_M3u8List.Get(pPos);
        
        if(!pStream)
        {
            VOLOGE("[NPW]+++++++++++++++++01 Prepare m3u8 list fatal error!!! %d+++++++++++++++", m_M3u8List.GetCount());
            DumpStreamListInfo(true);
            DumpStreamListInfo(false);
            delete []pBuffer;
            return;
        }
        
        memset(szTmp, 0, 1024);
        sprintf(szTmp, M3U8_SEQUENCE, pStream->GetIdx());
        strcpy(pCurr+nWrite, szTmp);
        nWrite += strlen(szTmp);        
        
        // only write M3U8_MAX_COUNT stream items
        while (nMaxCount>0)
        {
            nMaxCount--;
            
            if(!pStream)
            {
                VOLOGE("[NPW]+++++++++++++++++02 Prepare m3u8 list fatal error!!! %d+++++++++++++++", m_M3u8List.GetCount());
                DumpStreamListInfo(true);
                DumpStreamListInfo(false);
                delete []pBuffer;
                return;
            }
            
            if(pStream->IsDiscontinuity())
            {
                strcpy(pCurr+nWrite, M3U8_DISCONTINUITY);
                nWrite += strlen(M3U8_DISCONTINUITY);
            }
            
            memset(szTmp, 0, 1024);
            //sprintf(szTmp, M3U8_INFO, m_nTargetDuration);
            sprintf(szTmp, M3U8_INFO, pStream->GetDuration()/1000);
            strcpy(pCurr+nWrite, szTmp);
            nWrite += strlen(szTmp);
            
            memset(szTmp, 0, 1024);
            
            sprintf(szTmp, "%s\n", pStream->GetSourceURL());
            strcpy(pCurr+nWrite, szTmp);
            nWrite += strlen(szTmp);
            
            pPos        = m_M3u8List.Next(pPos);
            pStream     = (CHLSHttpStream*)m_M3u8List.Get(pPos);
        }
        
        if(m_bLastChunk)
        {
            m_bLastChunk = false;
            
            sprintf(szTmp, "%s\n", M3U8_FILE_END);
            strcpy(pCurr+nWrite, szTmp);
            nWrite += strlen(szTmp);
            
            VOLOGI("[NPW]Append EOS tag in M3U8\n %s\n size %d", pBuffer, nWrite);
        }
    }
    else
    {
        //write head
        strcpy(pCurr, M3U8_FILE_START);
        nWrite += strlen(M3U8_FILE_START);
        
        strcpy(pCurr+nWrite, M3U8_VERSION);
        nWrite += strlen(M3U8_VERSION);
        
        //strcpy(pCurr+nWrite, M3U8_CACHE);
        //nWrite += strlen(M3U8_CACHE);
        
        memset(szTmp, 0, 1024);
        //sprintf(szTmp, M3U8_TARGET_DURATION, m_nTargetDuration);
        sprintf(szTmp, M3U8_TARGET_DURATION, GetMaxDuration());
        strcpy(pCurr+nWrite, szTmp);
        nWrite += strlen(szTmp);
        
        POSITION pPos               = m_M3u8List.GetHeadPosition();
        CHLSHttpStream* pStream     = (CHLSHttpStream*)m_M3u8List.Get(pPos);
        
        memset(szTmp, 0, 1024);
        sprintf(szTmp, M3U8_SEQUENCE, pStream->GetIdx());
        strcpy(pCurr+nWrite, szTmp);
        nWrite += strlen(szTmp);
        
        // only write M3U8_MAX_COUNT stream items
        while (nMaxCount>0)
        {
            nMaxCount--;
            
            if(pStream->IsDiscontinuity())
            {
                strcpy(pCurr+nWrite, M3U8_DISCONTINUITY);
                nWrite += strlen(M3U8_DISCONTINUITY);
            }
            
            memset(szTmp, 0, 1024);
            //sprintf(szTmp, M3U8_INFO, m_nTargetDuration);
            sprintf(szTmp, M3U8_INFO, pStream->GetDuration()/1000);
            strcpy(pCurr+nWrite, szTmp);
            nWrite += strlen(szTmp);
            
            memset(szTmp, 0, 1024);
            
            sprintf(szTmp, "%s\n", pStream->GetSourceURL());
            strcpy(pCurr+nWrite, szTmp);
            nWrite += strlen(szTmp);
            
            pPos        = m_M3u8List.Next(pPos);
            pStream     = (CHLSHttpStream*)m_M3u8List.Get(pPos);
        }
        
        sprintf(szTmp, "%s\n", M3U8_FILE_END);
        strcpy(pCurr+nWrite, szTmp);
        nWrite += strlen(szTmp);
    }
    
    CHLSHttpStream::Close();
    
    m_pBuffer = pBuffer;
    //VOLOGI("[NPW] %s\n size %d", (char*)m_pBuffer, nWrite);
    
    m_nSize     = nWrite;
    m_llEndPos  = nWrite;
    
    SetAvailable(true);
}

void CHLSM3u8Stream::CreateM3u8()
{
    int nCount = IsNeedUpdateM3U8();
    
    if(nCount > 0)
    {
        //VOLOGI("[NPW]It need to update M3U8, %d", nCount);
        UpdateM3U8(nCount);
    }
    else
    {
        //VOLOGI("[NPW]No need to update M3U8");
        if(m_bLastChunk)
        {
            UpdateM3U8(0);
            
            m_bLastChunk    = false;
        }
    }
}


bool CHLSM3u8Stream::IsReady()
{
    int nCount                  = 0;
    POSITION pPos               = m_AvailableStreamList.GetHeadPosition();
    CHttpBaseStream* pStream    = m_AvailableStreamList.Get(pPos);
    
    while (pStream)
    {
        //if(pStream->IsAvailable())
            nCount++;
        
        if(nCount >= M3U8_CHUNK_MAX_COUNT)
            return true;
            
        pPos        = m_AvailableStreamList.Next(pPos);
        pStream     = m_AvailableStreamList.Get(pPos);
    }
    
    return false;
}

int CHLSM3u8Stream::CheckSameStreamCount(CObjectList<CHLSHttpStream>* pSrcList, CObjectList<CHLSHttpStream>* pDstList)
{
    int nSameCount          = 0;
    POSITION pPos           = pSrcList->GetHeadPosition();
    CHLSHttpStream* pSrc    = pSrcList->Get(pPos);
    
    while (pSrc)
    {
        // can't find stream, the stream was sent completely
        if(pDstList->Find(pSrc))
        {
            nSameCount++;
        }
        
        pPos     = m_M3u8List.Next(pPos);
        pSrc     = m_M3u8List.Get(pPos);
    }

    return nSameCount;
}

//return the stream count that can be updated
int CHLSM3u8Stream::IsNeedUpdateM3U8()
{
    voCAutoLock lock(&m_mtxStream);
    
    //We need check if stream is changed in M3u8 list, stream added or removed
    if(m_M3u8List.GetCount()<=0 && m_AvailableStreamList.GetCount()>=M3U8_CHUNK_MAX_COUNT)
    {
        VOLOGI("[NPW] 00 return update m3u8 count %d", M3U8_CHUNK_MAX_COUNT);
        return M3U8_CHUNK_MAX_COUNT;
    }
    
    //if(m_AvailableStreamList.GetCount() < M3U8_CHUNK_MAX_COUNT)
        //return 0;
    
    // first check if stream was sent completely
    int nAvailableCount             = m_AvailableStreamList.GetCount();
    int nRemovedCount               = 0;
    POSITION pM3u8Pos               = m_M3u8List.GetHeadPosition();
    CHLSHttpStream* pM3u8Stream     = m_M3u8List.Get(pM3u8Pos);
    
    while (pM3u8Stream)
    {
        // can't find stream, the stream was sent completely
        if(!m_AvailableStreamList.Find(pM3u8Stream))
        {
            // we still need confirm if more than M3U8_MAX_COUNT streams are available
            //if(m_StreamList.GetCount())
                //return true;
            nRemovedCount++;
        }
        
        pM3u8Pos        = m_M3u8List.Next(pM3u8Pos);
        pM3u8Stream     = m_M3u8List.Get(pM3u8Pos);
    }
    
    // second check if stream is avalable
    //if (nRemovedCount >= M3U8_CHUNK_MAX_COUNT)
    if (nRemovedCount > 0 || IsEOS())
    {
        if(nAvailableCount > 0)
        {
            // it need check the real available count,maybe stream is already in m3u8 list,but is not sent out
            int nSameStreamCount    = CheckSameStreamCount(&m_AvailableStreamList, &m_M3u8List);
            int nRealAvailableCount = nAvailableCount - nSameStreamCount;
        
            /*
            if(nRealAvailableCount > 0)
            {
                if(nRealAvailableCount >= nRemovedCount)
                {
                    VOLOGI("[NPW] 01 return update m3u8 count %d", nRemovedCount);
                    return nRemovedCount;
                }
                else
                {
                    VOLOGI("[NPW] 02 return update m3u8 count %d", nRealAvailableCount);
                    return nRealAvailableCount;
                }
            }
            else
            {
                DumpStreamListInfo(true);
                DumpStreamListInfo(false);
                
                VOLOGI("[NPW]No need update m3u8, m3u8 remove %d, real available %d", nRemovedCount, nRealAvailableCount);
            }*/
             
    
            if(nRealAvailableCount >= nRemovedCount)
            {
                //VOLOGI("[NPW] 01 return update m3u8 count %d", nRemovedCount);
                DumpStreamListInfo(true);
                DumpStreamListInfo(false);
                
                return nRemovedCount;
            }
            else if(nRealAvailableCount > 0 && IsEOS())
            {
                //VOLOGI("[NPW] 02 return update m3u8 count %d", nRealAvailableCount);
                m_bLastChunk    = true;
                m_nFlag         =  STREAM_FLAG_UNKNOWN;
                return nRealAvailableCount;
            }
            else
            {
                DumpStreamListInfo(true);
                DumpStreamListInfo(false);

                //VOLOGI("[NPW]--No need update m3u8, m3u8 remove %d, real available %d", nRemovedCount, nRealAvailableCount);
                return 0;
            }
        }
        else if(IsEOS())
        {
            m_bLastChunk    = true;
            m_nFlag         =  STREAM_FLAG_UNKNOWN;
            return 0;
        }
    }
    
    VOLOGI("[NPW]No need update m3u8, remove count is %d, available count %d, interval %ld", nRemovedCount, nAvailableCount, voOS_GetSysTime()-m_nNotUpdateM3u8Time);
    m_nNotUpdateM3u8Time = voOS_GetSysTime();
    
    return 0;
}

int CHLSM3u8Stream::GetMaxDuration()
{
    if(m_M3u8List.GetCount() <= 0)
        return m_nTargetDuration;
    
    int nMax = 0;
    
    POSITION pPos               = m_M3u8List.GetHeadPosition();
    CHttpBaseStream* pStream    = m_M3u8List.Get(pPos);
    
    while (pStream)
    {
        if (pStream->GetDuration() > nMax)
        {
            nMax = pStream->GetDuration();
        }

        pPos        = m_M3u8List.Next(pPos);
        pStream     = m_M3u8List.Get(pPos);
    }
    
    return nMax/1000;
}

int CHLSM3u8Stream::GetAvailableStreamCount()
{
    //tag:need check available flag
    voCAutoLock lock(&m_mtxStream);
    return m_AvailableStreamList.GetCount();
}

int CHLSM3u8Stream::GetPortNumber()
{
    m_nPortNumber++;
    
    CreateRandomURL();
    
    return m_nPortNumber;
}

int CHLSM3u8Stream::Flush()
{
    voCAutoLock lock(&m_mtxStream);
    
    m_nCurrStreamIdx = 0;
    m_M3u8List.RemoveAll();
    m_AvailableStreamList.RemoveAll();
    
    return VONP_ERR_None;
}


int CHLSM3u8Stream::GetMaxChunkCount()
{
    return M3U8_CHUNK_MAX_COUNT;
}

char* CHLSM3u8Stream::GetIPAddress()
{
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    int success = 0;
    
    //retrieve the current interfaces - returns 0 on success
    success = getifaddrs(&interfaces);
    
    if (success == 0)
    {
        //Loop through linked list of interfaces
        temp_addr = interfaces;
        while (temp_addr != NULL)
        {
            if (temp_addr->ifa_addr->sa_family == AF_INET)
            {
                //Check if interface is en0 which is the wifi connection on the iPhone
                //if ([[NSString stringWithUTF8String: temp_addr->ifa_name] isEqualToString:@"en0"])
                if(!strcmp(temp_addr->ifa_name, "en0"))
                {
                    VOLOGI("IP addr is %s", inet_ntoa(((struct sockaddr_in *) temp_addr->ifa_addr)->sin_addr));
                    strcpy(m_szIPAddr, inet_ntoa(((struct sockaddr_in *) temp_addr->ifa_addr)->sin_addr));
                    break;
                }
            }
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    freeifaddrs(interfaces);
    
    return m_szIPAddr;
}
