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
 * @file voCheckUseTime.h
 * voCheckUseTime header file
 *
 * A tool to print log when over time
 *
 * @author  Jeff Huang
 *
 * @date    2013-2013 (yyyy: year of creation, zzzz: year of last update)
 ************************************************************************/

#ifndef __VO_CHECK_USE_TIME_LOG_H__
#define __VO_CHECK_USE_TIME_LOG_H__

#ifdef _VONAMESPACE
namespace _VONAMESPACE {
#endif

class voCheckUseTime
{
public:
    // timeShouldPrint: ms
    voCheckUseTime(int timeShouldPrint, const char *printString);
    virtual ~voCheckUseTime();
protected:
    int m_nOverTime;
    long long m_nInitTime;
    char *m_pInfo;
};

#ifdef _VONAMESPACE
}
#endif

#endif
