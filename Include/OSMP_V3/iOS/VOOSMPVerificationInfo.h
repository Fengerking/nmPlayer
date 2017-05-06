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
#import "VOOSMPType.h"

@interface VOOSMPVerificationInfo : NSObject
{
    VO_OSMP_SRC_VERIFICATION_FLAG _dataFlag;
    NSString* _verificationString;
    
    NSString* _responseString;
}

/**
 * Verification flag {@link VO_OSMP_SRC_VERIFICATION_FLAG} for HTTP. Not required if setting DRM verification.
 */
@property (readwrite, assign, getter=getDataFlag) VO_OSMP_SRC_VERIFICATION_FLAG dataFlag;

/**
 * Verification string.
 */
@property (readwrite, retain, getter=getVerificationString) NSString* verificationString;

/**
 * Get authentication response string.
 *
 * @return authentication response string (NSString)
 */
@property (readwrite, retain, getter=getResponseString) NSString* responseString;


/**
 * Initialize the verification info with values.
 *
 * @param dataFlag            [in] Set the verification flag {@link VO_OSMP_SRC_VERIFICATION_FLAG} for HTTP. Not required if setting DRM verification.
 * @param verificationString  [in] Set the verification string for the corresponding flag.
 *
 * @return  object if successful; nil if unsuccessful
 */
- (id) init:(VO_OSMP_SRC_VERIFICATION_FLAG)dataFlag verificationString:(NSString *)verificationString;

@end