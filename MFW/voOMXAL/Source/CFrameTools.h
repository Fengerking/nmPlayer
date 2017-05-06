//
//  CFrameTools.h
//  voOMXALWrap
//
//  Created by Lin Jun on 8/30/12.
//  Copyright (c) 2012 VisualOn. All rights reserved.
//

#ifndef _CFRAME_TOOLS_H_
#define _CFRAME_TOOLS_H_

#include "voNPWrap.h"
#include "voVideoParser.h"

class CFrameTools
{
public:
    CFrameTools();
    ~CFrameTools();
    
public:
    int Init();
    int Uninit();

    int GetAspectRatio(VONP_BUFFERTYPE* pBuf, int* pAspectRatio);
    int GetProfileLevel(VONP_BUFFERTYPE* pBuf, int* pProfileLevel);
    
private:
    VO_HANDLE           m_hVideoParser;
    VO_VIDEO_PARSERAPI  m_funcVideoParser;
};


#endif // end of _CFRAME_TOOLS_H_
