    /************************************************************************
    *                                                                      *
    *        VisualOn, Inc. Confidential and Proprietary, 2003 -           *
    *                                                                      *
    ************************************************************************/
/*******************************************************************************
    File:        load.cpp
 
    Contains:    sub load control cpp file
 
    Written by:  Jeff
 
    Change History (most recent first):
    2013-8-16        Jeff            Create file
 
 *******************************************************************************/

#include "voLoadLibControl.h"

#include "voH265.h"

void* voGetH265DecAPIAdapter()
{
    return (void *)voGetH265DecAPI;
}