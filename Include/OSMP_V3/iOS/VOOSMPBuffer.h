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

#import <Foundation/Foundation.h>

@protocol VOOSMPBuffer <NSObject>

/**
 * Get timestamp of buffer.
 *
 * @return  timestamp of buffer
 */
@property (readonly, assign, getter=getTimestamp) long long timestamp;


/**
 * Get size of buffer.
 *
 * @return  size of buffer
 */
@property (readonly, assign, getter=getBufferSize) int bufferSize;


/**
 * Get buffer data.
 *
 * @return  buffer data
 */
@property (readonly, retain, getter=getBuffer) NSData* buffer;

@end
