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
 * Protocol of Picture timing as defined in ISO/IEC 14496-10:2005 (E) Annex D 2.2.
 */
@protocol VOOSMPSEIClockTimestamp <NSObject>

/**
 * Get clockTimestampFlag.
 *
 * @return clockTimestampFlag
 */
@property (readonly, assign, getter=getClockTimestampFlag) int clockTimestampFlag;

/**
 * Get ctType.
 *
 * @return ctType
 */
@property (readonly, assign, getter=getCtType) int ctType;

/**
 * Get nuitFieldBasedFlag.
 *
 * @return nuitFieldBasedFlag
 */
@property (readonly, assign, getter=getNuitFieldBasedFlag) int nuitFieldBasedFlag;

/**
 * Get countingType.
 *
 * @return countingType
 */
@property (readonly, assign, getter=getCountingType) int countingType;

/**
 * Get fullTimestampFlag.
 *
 * @return fullTimestampFlag
 */
@property (readonly, assign, getter=getFullTimestampFlag) int fullTimestampFlag;

/**
 * Get discontinuityFlag.
 *
 * @return discontinuityFlag
 */
@property (readonly, assign, getter=getDiscontinuityFlag) int discontinuityFlag;

/**
 * Get countDroppedFlag.
 *
 * @return countDroppedFlag
 */
@property (readonly, assign, getter=getCountDroppedFlag) int countDroppedFlag;

/**
 * Get frames.
 *
 * @return frames
 */
@property (readonly, assign, getter=getFrames) int frames;

/**
 * Get secondsValue.
 *
 * @return secondsValue
 */
@property (readonly, assign, getter=getSecondsValue) int secondsValue;

/**
 * Get minutesValue.
 *
 * @return minutesValue
 */
@property (readonly, assign, getter=getMinutesValue) int minutesValue;

/**
 * Get hoursValue.
 *
 * @return hoursValue
 */
@property (readonly, assign, getter=getHoursValue) int hoursValue;

/**
 * Get secondsFlag.
 *
 * @return secondsFlag
 */
@property (readonly, assign, getter=getSecondsFlag) int secondsFlag;

/**
 * Get minutesFlag.
 *
 * @return minutesFlag
 */
@property (readonly, assign, getter=getMinutesFlag) int minutesFlag;

/**
 * Get hoursFlag.
 *
 * @return hoursFlag
 */
@property (readonly, assign, getter=getHoursFlag) int hoursFlag;

/**
 * Get timeOffset.
 *
 * @return timeOffset
 */
@property (readonly, assign, getter=getTimeOffset) int timeOffset;


@end

