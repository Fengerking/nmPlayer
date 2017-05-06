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
    File:        VOOSMPHTTPDownloadFailureImpl

    Contains:    VOOSMPHTTPDownloadFailureImpl interface file

    Written by:    Jeff Huang


    Change History (most recent first):
    2012-10-24        Jeff Huang        Create file

 *******************************************************************************/

#import <Foundation/Foundation.h>
#import "VOOSMPHTTPDownloadFailure.h"

@interface VOOSMPHTTPDownloadFailureImpl : NSObject <VOOSMPHTTPDownloadFailure>
{
    NSString* _url;
    VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON _reason;
    NSString* _response;
}

@property (readwrite, retain, getter=getURL) NSString* URL;

/**
 * Get reason of the HTTP download failure
 *
 * @return  reason of HTTP download failure
 **/
@property (readwrite, assign, getter=getReason) VO_OSMP_HTTP_DOWNLOAD_FAILURE_REASON reason;

/**
 * Get response of the HTTP download failure when the download failure reason is VO_OSMP_HTTP_CLIENT_ERROR or VO_OSMP_HTTP_SERVER_ERROR.
 * It is null for other failure reasons
 *
 * @return  response of HTTP download failure
 **/
@property (readwrite, retain, getter=getResponse) NSString* response;

- (id) init;

@end
