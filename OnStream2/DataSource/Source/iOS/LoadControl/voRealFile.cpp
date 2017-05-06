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

#import "voRealFile.h"

void* voGetReal2ReadAPIAdapter()
{
    return(void *)voGetReal2ReadAPI;
}

void* voGetRealReadAPIAdapter()
{
    return(void *)voGetRealReadAPI;
}
