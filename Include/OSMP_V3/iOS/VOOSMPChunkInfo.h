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

@protocol VOOSMPChunkInfo <NSObject>

/**
 * The type of this chunk.
 *
 * @return chunk type
 */
@property (readonly, assign, getter=getType) int type;

/**
 * The root URL of manifest. It must be filled by parser.
 *
 * @return root URL
 */
@property (readonly, retain, getter=getRootURL) NSString* rootURL;

/**
 * The URL of this chunk. It can be a relative URL.
 *
 * @return chunk URL
 */
@property (readonly, retain, getter=getURL) NSString* URL;

/**
 * The start offset time of this chunk. The units of ( startTime / timeScale * 1000 ) should be in ms
 *
 * @return start offset time of this chunk
 */
@property (readonly, assign, getter=getStartTime) long startTime;

/**
 * The duration of this chunk. The units of ( duration / timeScale * 1000 ) should be inms.
 *
 * @return duration of this chunk
 */
@property (readonly, assign, getter=getDuration) long duration;

/**
 * The time scale of this chunk.
 *
 * @return time scale of this chunk
 */
@property (readonly, assign, getter=getTimeScale) long timeScale;

@end


