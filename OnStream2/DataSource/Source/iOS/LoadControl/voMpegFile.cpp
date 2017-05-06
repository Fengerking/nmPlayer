    /************************************************************************
    *                                                                      *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 -           *
    *                                                                      *
    ************************************************************************/
/*******************************************************************************
    File:        load.cpp
 
    Contains:    Sub Load Control cpp
 
    Written by:  Jeff
 
    Change History (most recent first):
    2012-08-09        Jeff            Create file
 
 *******************************************************************************/

#include "voLoadLibControl.h"

#import "voMpegFile.h"

void* voGetMPG2ReadAPIAdapter()
{
    return(void *)voGetMPG2ReadAPI;
}

void* voGetMPGReadAPIAdapter()
{
    return(void *)voGetMPGReadAPI;
}
