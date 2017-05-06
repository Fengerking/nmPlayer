/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved
 
 VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
 All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/
/************************************************************************
 * @file voCheckUseTime.cpp
 * voCheckUseTime cpp file
 *
 * A tool to print log when over time
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#import "voCheckUseTime.h"

#include "voLog.h"
#include "voOSFunc.h"

#ifdef _VONAMESPACE
using namespace _VONAMESPACE;
#endif

voCheckUseTime::voCheckUseTime(int timeShouldPrint, const char *printString)
:m_nOverTime(timeShouldPrint)
,m_pInfo(NULL)
{
    if (NULL != printString) {
        m_pInfo = new char[strlen(printString) + 1];
        memset(m_pInfo, 0, sizeof(strlen(printString) + 1));
        strcpy(m_pInfo, printString);
    }
    
    m_nInitTime = voOS_GetSysTime();
}

voCheckUseTime::~voCheckUseTime()
{
    long long nTime = voOS_GetSysTime() - m_nInitTime;
    if (m_nOverTime <= nTime) {
        if (NULL != m_pInfo) {
            VOLOGI("!!!! %s Use time:%lld", m_pInfo, nTime);
        }
        else {
            VOLOGI("!!!! Use time:%lld", nTime);
        }
    }
    
    if (NULL != m_pInfo) {
        delete []m_pInfo;
    }
}


