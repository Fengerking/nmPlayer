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
#import "VOOSMPHTTPProxy.h"

@protocol VOCommonPlayerHTTPConfiguration <NSObject>


/**
 * Set the verification information to start HTTP verification.
 *
 * @param   info   [in] Verification information. {@link VOOSMPVerificationInfo}
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
- (VO_OSMP_RETURN_CODE) setHTTPVerificationInfo:(VOOSMPVerificationInfo *)info;


/**
 * Set HTTP header.
 *
 * @param   headerName   [in] a HTTP header name
 * @param   headerValue  [in] value assigned to headerName
 *
 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setHTTPHeader:(NSString *)headerName headerValue:(NSString *)headerValue;


/**
 * Set HTTP proxy.
 *
 * @param   proxy   [in] proxy settings  {@link VOOSMPHTTPProxy}
 *
 * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setHTTPProxy:(VOOSMPHTTPProxy *)proxy;


/*
 * Set HTTP connection retry timeout.
 *
 * The setting is applicable only after data source has been opened successfully.
 *
 * @param time [in] interval, in seconds, in which the player will attempt to re-establish the HTTP connection. The default is 120 seconds. Setting the value to -1 disables the timeout so that the player will keep retrying, until the connection is established again
 *
 * @return {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setHTTPRetryTimeout:(int)time;


@end

