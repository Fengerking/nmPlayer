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
 * Enumeration of causes for HTTP failures.
 * VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON
 */
typedef enum {
    /** HTTP connect failed */
    VO_OSMP_HTTP_CONNECT_FAILED               = 0x00000000,
    
    /** HTTP failed to get response or response cannot be parsed or too large */
    VO_OSMP_HTTP_INVALID_RESPONSE             = 0x00000001,
    
    /** HTTP 4xx error */
    VO_OSMP_HTTP_CLIENT_ERROR                 = 0x00000002,
    
    /** HTTP 5xx error */
    VO_OSMP_HTTP_SERVER_ERROR                 = 0x00000003,
    
    /** Max value */
    VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON_MAX  = 0xFFFFFFFF
    
} VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON;


@protocol VOOSMPHTTPDownloadFailure <NSObject>

/**
 * Get URL of the HTTP download failure.
 *
 * @return  URL
 */
@property (readonly, retain, getter=getURL) NSString* URL;

/**
 * Get cause of the HTTP download failure.
 *
 * @return  reason of HTTP download failure
 */
@property (readonly, assign, getter=getReason) VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON reason;

/**
 * Get response of the HTTP download failure when the download failure cause is VO_OSMP_HTTP_CLIENT_ERROR or VO_OSMP_HTTP_SERVER_ERROR.
 * It is null for other failure causes.
 *
 * @return  response of HTTP download failure
 */
@property (readonly, retain, getter=getResponse) NSString* response;

@end

