/************************************************************************
 *                                                                       *
 *        VisualOn, Inc. Confidential and Proprietary, 2003              *
 *                                                                       *
 ************************************************************************/
/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All rights Reserved
 
 VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
 All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/
/*******************************************************************************
 File:        voOSEvent
 
 Contains:    voOSEvent interface file
 
 Written by:    Jeff Huang
 
 
 Change History (most recent first):
 2013-08-30        Jeff Huang        Create file
 
 *******************************************************************************/

#import <Foundation/Foundation.h>

@interface voOSEvent : NSObject
{
    int _eventID;
    int _param1;
    int _param2;
    id _obj;
}

- (id) init:(int)eventID param1:(int)param1 param2:(int)param2 obj:(id)obj;

@property (readwrite, assign) int eventID;
@property (readwrite, assign) int param1;
@property (readwrite, assign) int param2;
@property (readwrite, retain) id obj;

@end
