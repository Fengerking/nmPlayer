//
//  CFrameTools.cpp
//  voOMXALWrap
//
//  Created by Lin Jun on 8/30/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#include "voLog.h"
#include "CFrameTools.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif	// _VONAMESPACE

CFrameTools::CFrameTools()
:m_hVideoParser(NULL)
{
}

CFrameTools::~CFrameTools()
{
    Uninit();
}

int CFrameTools::Init()
{
    voGetVideoParserAPI(&m_funcVideoParser,VO_VIDEO_CodingH264);
    
    if(!m_funcVideoParser.Init)
    {
        VOLOGW("[OMXAL]Get vdeo parser API failed!!!");
        return VONP_ERR_OutMemory;
    }
    
    m_funcVideoParser.Init(&m_hVideoParser);
    
    if(!m_hVideoParser)
        return VONP_ERR_OutMemory;
    
    VOLOGI("[OMXAL]Video parser init ok!!!");

    return VONP_ERR_None;
}

int CFrameTools::Uninit()
{
    if(m_hVideoParser)
    {
        m_funcVideoParser.Uninit(m_hVideoParser);
        m_hVideoParser = NULL;
    }
    
    return VONP_ERR_None;
}


int CFrameTools::GetAspectRatio(VONP_BUFFERTYPE* pBuf, int* pAspectRatio)
{
    if(m_hVideoParser && pBuf->pBuffer && pBuf->nSize>0)
    {
        int nRet = VONP_ERR_Unknown;
        
        VO_CODECBUFFER buf;
        memset(&buf, 0, sizeof(VO_CODECBUFFER));
        
        buf.Buffer	= pBuf->pBuffer;
        buf.Length	= pBuf->nSize;
                
        nRet = m_funcVideoParser.Process(m_hVideoParser, &buf);
        
        if(VO_ERR_NONE == nRet)
        {
            VO_PARSER_ASPECTRATIO sRatio;
            memset(&sRatio, 0, sizeof(VO_PARSER_ASPECTRATIO));
            sRatio.mode = -1;
                        
            nRet = m_funcVideoParser.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_RATIO, &sRatio);
        
            if(VO_ERR_NONE == nRet)
            {                
                if(sRatio.mode == VO_RATIO_00)
                    *pAspectRatio = VONP_RATIO_00;
                else if(sRatio.mode == VO_RATIO_11)
                    *pAspectRatio = VONP_RATIO_11;
                else if(sRatio.mode == VO_RATIO_43)
                    *pAspectRatio = VONP_RATIO_43;
                else if(sRatio.mode == VO_RATIO_169)
                    *pAspectRatio = VONP_RATIO_169;
                else if(sRatio.mode == VO_RATIO_21)
                    *pAspectRatio = VONP_RATIO_21;
                else if(sRatio.mode == VO_RATIO_MAX)
                {
					VOLOGW("[OMXAL]Handle undefined aspect ratio, %dx%d", sRatio.width, sRatio.height);

					int nWidth = sRatio.width;
					int nHeight = sRatio.height;

					if(nWidth > 0x7fff || nHeight > 0x7fff)
					{
						nWidth = (nWidth + 512) >> 10;
						nHeight = (nHeight + 512) >> 10;
					}
					*pAspectRatio = (nWidth << 16) | nHeight;
                }
                else
                {
                    VOLOGE("[OMXAL]Video parser output error!!!");
                }
                
                VOLOGI("[OMXAL]Video parser output respect ratio: %d", *pAspectRatio);

                return VONP_ERR_None;
            }
        }
    }
    
    return VONP_ERR_Unknown;
}

int CFrameTools::GetProfileLevel(VONP_BUFFERTYPE* pBuf, int* pProfileLevel)
{
    if(m_hVideoParser && pBuf->pBuffer && pBuf->nSize>0)
    {
        int nRet = VONP_ERR_Unknown;
        
        VO_CODECBUFFER buf;
        memset(&buf, 0, sizeof(VO_CODECBUFFER));
        
        buf.Buffer	= pBuf->pBuffer;
        buf.Length	= pBuf->nSize;
        
        nRet = m_funcVideoParser.Process(m_hVideoParser, &buf);
        
        if(VO_ERR_NONE == nRet)
        {
            //refer to enum VO_VIDEO_LEVELTYPE
            int nProfileLevel = -1;
            nRet = m_funcVideoParser.GetParam(m_hVideoParser, VO_PID_VIDEOPARSER_PROFILE, &nProfileLevel);
            
            if(VO_ERR_NONE == nRet)
            {                
                VOLOGI("[OMXAL]Profile level is 0X%x", nProfileLevel);
                return VONP_ERR_None;
            }
            else
            {
                VOLOGW("[OMXAL]Profile level is 0X%x", nProfileLevel);
                return VONP_ERR_Unknown;
            }
        }
    }
    
    return VONP_ERR_Unknown;
}
