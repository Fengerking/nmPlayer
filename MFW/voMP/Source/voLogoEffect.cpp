#include "voLogoEffect.h"
#include "voLogoTextData.h"
#include "voLog.h"
#include "voCheck.h"
#include <stdlib.h>
#include <string.h>

#ifdef _VONAMESPACE
//using namespace _VONAMESPACE;
#endif

voLogoEffect::voLogoEffect()
: m_nVideoCount(0)
, m_nTimes(0)
, m_xPos(0)
, m_yPos(0)
{
	for (int n=0; n<MAX_VIDEO_COUNT; n++)
	{
		m_aResetBuffer[n] = NULL;
	}
}

voLogoEffect::~voLogoEffect()
{
	Flush();
}

VO_U32 voLogoEffect::Flush()
{
	m_nVideoWidth	= 0;
	m_nVideoHeight	= 0;
	m_nVideoCount	= 0;
	m_nTimes		= 0;
	
	for (int n=0; n<MAX_VIDEO_COUNT; n++)
	{
		if(m_aResetBuffer[n])
		{
			free(m_aResetBuffer[n]);
			m_aResetBuffer[n] = NULL;
		}
	}
	
	return 0;
}


VO_U32 voLogoEffect::CheckVideo (VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pFormat)
{	
	int					i = 0;
	int					nCurrent = 0;
	unsigned char *		pBackBuff = NULL;
	//VO_VIDEO_BUFFER *	pVideoBuff = NULL;
	
	m_nVideoWidth = pFormat->Width;
	m_nVideoHeight = pFormat->Height;

	int nLogoWidth = 0;//VOLOGO_DATA_WIDTH;
	int nLogoHeight = 0;//VOLOGO_DATA_HEIGHT;

	nLogoWidth = VOLOGOTEXT_DATA_WIDTH;
	nLogoHeight = VOLOGOTEXT_DATA_HEIGHT;

	//if (nLogoWidth > pFormat->Width &&  pFormat->Width > 0)
		//nLogoWidth = pFormat->Width;

	if (true/*bNew == 1*/)
	{
		nCurrent = m_nVideoCount;

		memcpy (&m_aVideoBuffer[m_nVideoCount], pOutBuffer, sizeof (VO_VIDEO_BUFFER));
		if (m_aResetBuffer[m_nVideoCount] == NULL)
			m_aResetBuffer[m_nVideoCount] = (unsigned char *)malloc (nLogoWidth * nLogoHeight * 2);
		m_nVideoCount++;
	}

	/*if (m_nLevel == 1 && m_nTimes >= 300)
	{
		if (m_yPos == 0)
		{
			m_xPos = (pFormat->Width - nLogoWidth) / 4 * 2;
			m_yPos = (pFormat->Height - nLogoHeight) / 4 * 2;
		}
	}*/

	if (m_aResetBuffer[nCurrent] == NULL)
	{
		VOLOGI ("m_aResetBuffer[nCurrent]; is NULL");
		m_aResetBuffer[nCurrent] = (unsigned char *)malloc (nLogoWidth * nLogoHeight * 2);
	}

	pBackBuff = m_aResetBuffer[nCurrent];
	if (pOutBuffer->ColorType == VO_COLOR_YUV_PLANAR420)
	{
		for (i = 0; i < nLogoHeight; i++)
			memcpy (pBackBuff + nLogoWidth * i, pOutBuffer->Buffer[0] + pOutBuffer->Stride[0] * i, nLogoWidth);
		pBackBuff = m_aResetBuffer[nCurrent] + nLogoWidth * nLogoHeight;
		for (i = 0; i < nLogoHeight / 2; i++)
			memcpy (pBackBuff + (nLogoWidth / 2) * i, pOutBuffer->Buffer[1] + pOutBuffer->Stride[1] * i, nLogoWidth / 2);
		pBackBuff = m_aResetBuffer[nCurrent] + nLogoWidth * nLogoHeight * 5 / 4;
		for (i = 0; i < nLogoHeight / 2; i++)
			memcpy (pBackBuff + (nLogoWidth / 2) * i, pOutBuffer->Buffer[2] + pOutBuffer->Stride[2] * i, nLogoWidth / 2);

		int w = 0;
		int * pLogo = (int *)voLogoTextBufferY;;
		for (i = 0; i < nLogoHeight; i++)
		{
			for (w = 0; w < nLogoWidth; w+=4)
			{
				if (*pLogo == 0)
				{
					pLogo++;
					continue;
				}
				else
				{
					if (*pLogo & 0XFF)
					{
						*(pOutBuffer->Buffer[0] + pOutBuffer->Stride[0] * (i + m_yPos) + m_xPos + w) = voLogoTextBufferY[nLogoWidth * i + w];
					}
					if (*pLogo & 0XFF00)
					{
						*(pOutBuffer->Buffer[0] + pOutBuffer->Stride[0] * (i + m_yPos) + m_xPos + w + 1) = voLogoTextBufferY[nLogoWidth * i + w + 1];
					}
					if (*pLogo & 0XFF0000)
					{
						*(pOutBuffer->Buffer[0] + pOutBuffer->Stride[0] * (i + m_yPos) + m_xPos + w + 2) = voLogoTextBufferY[nLogoWidth * i + w + 2];
					}
					if (*pLogo & 0XFF000000)
					{
						*(pOutBuffer->Buffer[0] + pOutBuffer->Stride[0] * (i + m_yPos) + m_xPos + w + 3) = voLogoTextBufferY[nLogoWidth * i + w + 3];
					}
				}
				pLogo++;
			}
		}

		for (i = 0; i < nLogoHeight / 2; i++)
		{
			pLogo = (int *)(voLogoTextBufferY + nLogoWidth * i * 2);
			for (w = 0; w < nLogoWidth / 2; w+=2)
			{
				if (*pLogo == 0)
				{
					pLogo++;
					continue;
				}

				if (voLogoTextBufferY[nLogoWidth * i * 2 + w * 2] > 16)
				{
					*(pOutBuffer->Buffer[1] + pOutBuffer->Stride[1] * (i + m_yPos) + (m_xPos + w)) = voLogoTextBufferU[(nLogoWidth / 2) * i + w];
					*(pOutBuffer->Buffer[2] + pOutBuffer->Stride[2] * (i + m_yPos) + (m_xPos + w)) = voLogoTextBufferV[(nLogoWidth / 2) * i + w];
				}
				if (voLogoTextBufferY[nLogoWidth * i * 2 + (w + 1) * 2] > 16)
				{
					*(pOutBuffer->Buffer[1] + pOutBuffer->Stride[1] * (i + m_yPos) + (m_xPos + w + 1)) = voLogoTextBufferU[(nLogoWidth / 2) * i + w + 1];
					*(pOutBuffer->Buffer[2] + pOutBuffer->Stride[2] * (i + m_yPos) + (m_xPos + w + 1)) = voLogoTextBufferV[(nLogoWidth / 2) * i + w + 1];
				}
				pLogo++;
			}
		}
	}

	return 0;
}


VO_U32 voLogoEffect::ResetVideo (VO_VIDEO_BUFFER * pOutBuffer)
{
	if(m_nVideoCount==0 || m_nVideoWidth==0 || m_nVideoHeight==0)
		return 0;
	
	int					i = 0;
	//int					nCurrent = 0;
	unsigned char *		pBackBuff = NULL;
	VO_VIDEO_BUFFER *	pVideoBuff = NULL;

	m_nTimes++;

	int nLogoWidth = 0;//VOLOGO_DATA_WIDTH;
	int nLogoHeight = 0;//VOLOGO_DATA_HEIGHT;

	nLogoWidth = VOLOGOTEXT_DATA_WIDTH;
	nLogoHeight = VOLOGOTEXT_DATA_HEIGHT;

	//if (nLogoWidth > m_nVideoWidth && m_nVideoWidth > 0)
		//nLogoWidth = m_nVideoWidth;
	
	if (pOutBuffer == NULL)
	{
		for (int nH = 0; nH < m_nVideoCount; nH++)
		{
			pVideoBuff = &m_aVideoBuffer[nH];
			if (pVideoBuff->Buffer[0] == NULL)
				continue;
			
			// 2011-11-07 Jeff: The buffer of logo need to reset too When the time is equal to 0
//			if (pVideoBuff->Time == 0)
//				continue;

			if (pVideoBuff->ColorType == VO_COLOR_YUV_PLANAR420)
			{
				pBackBuff = m_aResetBuffer[nH];
				for (i = 0; i < nLogoHeight; i++)
					memcpy (pVideoBuff->Buffer[0] + pVideoBuff->Stride[0] * (i + m_yPos) + m_xPos, pBackBuff + nLogoWidth * i, nLogoWidth);
				pBackBuff = m_aResetBuffer[nH] + nLogoWidth * nLogoHeight;
				for (i = 0; i < nLogoHeight / 2; i++)
					memcpy (pVideoBuff->Buffer[1] + pVideoBuff->Stride[1] * (i + m_yPos / 2) + m_xPos / 2, pBackBuff + nLogoWidth * i / 2, nLogoWidth / 2);
				pBackBuff = m_aResetBuffer[nH] + nLogoWidth * nLogoHeight * 5 / 4;
				for (i = 0; i < nLogoHeight / 2; i++)
					memcpy (pVideoBuff->Buffer[2] + pVideoBuff->Stride[2] * (i + m_yPos / 2) + m_xPos / 2, pBackBuff + nLogoWidth * i / 2, nLogoWidth / 2);

				pVideoBuff->Time = 0;
			}		
		}
		
		if(m_nVideoCount > 0)
			m_nVideoCount--;
	}

	return 0;
}

///////////////////////////////////////////////////////////////////
//   class voLogoEffectExt implementation
///////////////////////////////////////////////////////////////////

voLogoEffectExt::voLogoEffectExt()
:m_hLicense(NULL)
{
    memset(m_szLicensePath, 0, 1024);
    
    //voCheckLibInit(&m_hLicense, VO_INDEX_DEC_H264, 0, 0, NULL);
    voCheckLibInit(&m_hLicense, 0xFFFFFFFF, 0, 0, NULL);
}

voLogoEffectExt::~voLogoEffectExt()
{
    if(m_hLicense)
    {
        voCheckLibUninit(m_hLicense);
        m_hLicense = NULL;
    }
}

VO_U32 voLogoEffectExt::CheckVideo (VO_VIDEO_BUFFER * pOutBuffer, VO_VIDEO_FORMAT * pFormat)
{
    if(m_hLicense)
    {
        return voCheckLibCheckVideo(m_hLicense, pOutBuffer, pFormat);
    }
    
    return -1;
}

VO_U32 voLogoEffectExt::ResetVideo (VO_VIDEO_BUFFER * pOutBuffer)
{
    if(m_hLicense)
        return voCheckLibResetVideo(m_hLicense, pOutBuffer);
    
    return -1;
}

VO_U32 voLogoEffectExt::Flush()
{
    return -1;
}

VO_U32 voLogoEffectExt::SetKey(VO_TCHAR* pszKey)
{
    if (NULL == pszKey) {
        return -1;
    }
    //strcpy(m_szKey, "VISUALON/DATA/DATA/COM.VISUALON.OSMPDEMOPLAYER");
    
    if(m_hLicense)
    {
        VO_VIDEO_BUFFER buf;
        memset(&buf, 0, sizeof(VO_VIDEO_BUFFER));
        buf.Buffer[0] = (VO_PBYTE)pszKey;
        buf.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_TEXT;
        
        int nRet = voCheckLibCheckImage(m_hLicense, &buf, NULL);
        VOLOGI("voCheckLibCheckImage:%d", nRet);
    }
    
	return 0;
}

VO_U32 voLogoEffectExt::SetLicenseFilePath(VO_TCHAR* pszFile)
{
    if (NULL == pszFile) {
        return -1;
    }
    
    memset(m_szLicensePath, 0, 1024);
    
    //remove /voVidDec.dat
    VO_TCHAR * pPath = vostrstr (pszFile, _T("/voVidDec"));
    if (pPath != NULL)
    {
        //strncpy(m_szLicensePath, pszFile, pPath-pszFile);
        
        //if (pPath - pszFile + vostrlen (_T("/voVidDec")) >= vostrlen (pszFile))
        {
            *(pPath + 1) = 0;
        }
    }

    strcpy (m_szLicensePath, pszFile);
    
    if(m_hLicense)
    {
        VO_VIDEO_BUFFER buf;
        memset(&buf, 0, sizeof(VO_VIDEO_BUFFER));
        buf.Buffer[0] = (VO_PBYTE)m_szLicensePath;
        buf.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_PATH;

        int nRet = voCheckLibCheckImage(m_hLicense, &buf, NULL);
        VOLOGI("voCheckLibCheckImage:%d", nRet);
    }
    
    return 0;
}

VO_U32 voLogoEffectExt::SetLicenseContent(VO_TCHAR* pData)
{
    if (NULL == pData) {
        return -1;
    }
    
    if (m_hLicense)
    {
        VO_VIDEO_BUFFER buf;
        memset(&buf, 0, sizeof(VO_VIDEO_BUFFER));
        buf.Buffer[0] = (VO_PBYTE)pData;
        buf.ColorType = (VO_IV_COLORTYPE)VOLICENSE_SET_DATA;
        
        int nRet = voCheckLibCheckImage(m_hLicense, &buf, NULL);
        VOLOGI("voCheckLibCheckImage:%d", nRet);
    }
    else {
        return -1;
    }
    
    return 0;
}


