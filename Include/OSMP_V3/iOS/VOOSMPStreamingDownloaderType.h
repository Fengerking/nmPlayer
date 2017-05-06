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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
    
    /**
     * Enumeration of callback event IDs.
     * VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID
     * <p>
     * This event is sent from sub-thread, please do not update UI or call other OSMP+ APIs in callback (except API in comments).
     * </p>
     */
    typedef enum {
        /** Open complete */
        VO_OSMP_CB_STREAMING_DOWNLOADER_OPEN_COMPLETE            =  0X10000001,
        /** Manifest file download was successful */
        VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_OK              =  0X10000002,
        /** Entire content download completed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_END                      =  0X10000004,
        /** Program information has changed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_PROGRAM_INFO_CHANGE      =  0X10000005,
        /** Manifest file is updated, param1 is {@link VOOSMPStreamingDownloaderProgressInfo} */
        VO_OSMP_CB_STREAMING_DOWNLOADER_MANIFEST_UPDATE          =  0X10000006,
        
        /** Manifest file download has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DOWNLOAD_MANIFEST_FAIL   =  0X90000001,
        /** Manifest file write has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_WRITE_MANIFEST_FAIL      =  0X90000002,
        /** Chunk file download has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DOWNLOAD_CHUNK_FAIL      =  0X90000003,
        /** Chunk file write has failed */
        VO_OSMP_CB_STREAMING_DOWNLOADER_WRITE_CHUNK_FAIL         =  0X90000004,
        /** Disk is full */
        VO_OSMP_CB_STREAMING_DOWNLOADER_DISK_FULL                =  0X90000005,
        /** Live stream is not supported */
        VO_OSMP_CB_STREAMING_DOWNLOADER_LIVE_STREAM_NOT_SUPPORT  =  0X90000007,
        /** Stream on local disk not supported */
        VO_OSMP_CB_STREAMING_DOWNLOADER_LOCAL_STREAM_NOT_SUPPORT =  0X90000008,

        /** Max value definition */
        VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID_MAX             =  0xFFFFFFFF
    } VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID;
    
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */


@interface VOOSMPStreamingDownloaderInitParam : NSObject
{
    NSString*       _libraryPath;
}

/**
 * Working directory path name.
 */
@property (readwrite, retain, getter=getLibraryPath) NSString* libraryPath;


/**
 * Initialize filter with values.
 *
 * @return	object if successful; nil if unsuccessful
 */
- (id) init;

@end


@interface VOOSMPStreamingDownloaderProgressInfo : NSObject
{
    int _downloadedStreamDuration;
    int _totalStreamDuration;
}

/**
 * Duration of stream which is downloaded.The units should be in ms.
 */
@property (readwrite, assign, getter=getDownloadedStreamDuration) int downloadedStreamDuration;


/**
 * Total duration of stream.The units should be in ms.
 */
@property (readwrite, assign, getter=getTotalStreamDuration) int totalStreamDuration;


/**
 * Initialize progress information with values.
 *
 * @param downloadedStreamDuration   [in] Duration of stream which is downloaded.The units should be in ms.
 * @param totalStreamDuration   [in] Total duration of stream.The units should be in ms.
 *
 * @return	object if successful; nil if unsuccessful
 */
- (id) init:(int)downloadedStreamDuration totalStreamDuration:(int)totalStreamDuration;

@end


/**
 * Protocol for event process. The client who wants to receive event from Downloader shall implement the delegate.
 */
@protocol VOOSMPStreamingDownloaderDelegate <NSObject>

/**
 * Protocol to process events. The client can implement the listener to receive/manage events.
 *
 * @param nID    [out] Event type. Refer to {@link VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID}.
 * @param param1 [out] First  parameter, specific to the event if needed.
 * @param param2 [out] Second parameter, specific to the event if needed.
 * @param obj    [out] Third  parameter, specific to the event if needed.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful.
 */
- (VO_OSMP_RETURN_CODE) onVOStreamingDownloaderEvent:(VO_OSMP_CB_STREAMING_DOWNLOADER_EVENT_ID)nID param1:(int)param1 param2:(int)param2 pObj:(void *)pObj;


@end





