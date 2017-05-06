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

#import "voMP4.h"

void* voGetMP42ReadAPIAdapter()
{
    return(void *)voGetMP42ReadAPI;
}

void* voGetMP4ReadAPIAdapter()
{
    return(void *)voGetMP4ReadAPI;
}