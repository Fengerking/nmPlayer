    /************************************************************************
    *                                                                      *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 -           *
    *                                                                      *
    ************************************************************************/
/*******************************************************************************
    File:        load.cpp
 
    Contains:    sub load control cpp file
 
    Written by:  Chris
 
    Change History (most recent first):
    2013-12-23        Chris            Create file
 
 *******************************************************************************/

#include "voLoadLibControl.h"

#include "voG711.h"

void* voGetG711DecAPIAdapter()
{
    return (void *)voGetG711DecAPI;
}
