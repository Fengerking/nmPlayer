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
#import "VOOSMPSEIClockTimestamp.h"

/** 
 * Protocol of Picture timing SEI message as defined by ISO/IEC 14496-10:2005 (E) Annex D 2.2.
 */
@protocol VOOSMPSEIPicTiming <NSObject>

/**
 * Get the cpbDpbDelaysPresentFlag.
 *
 * @return cpbDpbDelaysPresentFlag
 */
@property (readonly, assign, getter=getCpbDpbDelaysPresentFlag) int cpbDpbDelaysPresentFlag;

/**
 * Get the cpbRemovalDelay.
 *
 * @return cpbRemovalDelay
 */
@property (readonly, assign, getter=getCpbRemovalDelay) int cpbRemovalDelay;

/**
 * Get the dpbOutputDelay.
 *
 * @return dpbOutputDelay
 */
@property (readonly, assign, getter=getDpbOutputDelay) int dpbOutputDelay;

/**
 * Get the pictureStructurePresentFlag.
 *
 * @return pictureStructurePresentFlag
 */
@property (readonly, assign, getter=getPictureStructurePresentFlag) int pictureStructurePresentFlag;

/**
 * Get the pictureStructure.
 *
 * @return pictureStructure
 */
@property (readonly, assign, getter=getPictureStructure) int pictureStructure;

/**
 * Get the numClockTs.
 *
 * @return numClockTs
 */
@property (readonly, assign, getter=getNumClockTs) int numClockTs;

/**
 * Return an array of SEI clock timestamps {link @VOOSMPSEIClockTimestamp}.
 *
 * @return clock
 */
@property (readonly, retain, getter=getClock) NSArray * clock;


@end

