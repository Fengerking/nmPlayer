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
    File:        VOOSMPImageDataImpl

    Contains:    VOOSMPImageDataImpl interface file

    Written by:    Jeff Huang


    Change History (most recent first):
    2012-10-24        Jeff Huang        Create file

 *******************************************************************************/

#import <Foundation/Foundation.h>

#import "voOnStreamType.h"
#import "VOOSMPImageData.h"

@interface VOOSMPImageDataImpl : NSObject <VOOSMPImageData>
{
    VO_OSMP_IMAGE_TYPE _imageType;
    int _imageWidth;
    int _imageHeight;
    int _imageSize;
    NSData* _imageData;
}

@property (readwrite, retain, getter=getBuffer) NSData* imageData;

- (id) init:(VOOSMP_IMAGE_DATA *)pValue;

@end
