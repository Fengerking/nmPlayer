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

/**
 * Protocol of User data unregistered SEI message ISO/IEC 14496-10:2005 (E) Annex D 2.6
 */
@protocol VOOSMPSEIUserDataUnregistered <NSObject>

/**
 * getFieldCount
 *
 * @return the number of data fields in the message
 **/
@property (readonly, assign, getter=getFieldCount) int fieldCount;


/**
 * getFieldLength
 *
 * @return the length of the i-th data field
 **/
- (int) getFieldLength:(int) i;


/**
 * getDataBuffer
 *
 * @return message data buffer
 **/
@property (readonly, retain, getter=getDataBuffer) NSData* dataBuffer;

@end

