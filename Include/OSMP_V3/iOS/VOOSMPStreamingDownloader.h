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
#import "VOOSMPStreamingDownloaderType.h"
#import "VOCommonPlayerAssetSelection.h"

@protocol VOOSMPStreamingDownloader <VOCommonPlayerAssetSelection>

/**
 * Initialize a player instance.
 * <pre>This function must be called first to start a session.</pre>
 *
 * @param   delegate  [in] Refer to {@link VOOSMPStreamingDownloaderDelegate}.
 * @param   initParam [in] Refer to {@link VOOSMPStreamingDownloaderInitParam}.
 * @return  player object if successful; nil if unsuccessful
 */
- (id) init:(id<VOOSMPStreamingDownloaderDelegate>)delegate initParam:(VOOSMPStreamingDownloaderInitParam*)initParam;


/**
 * Open media source.
 *
 * @param   url             [in] Source file description (e.g. an URL or a file descriptor, etc.)
 * @param   flag            [in] Flag for opening media source. 
 * @param   localDir        [in] Directory where to save content files.
 *
 * @return {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) open:(NSString *)url flag:(unsigned int)flag localDir:(NSString *)localDir;


/**
 * Close media source.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) close;


/**
 * Start content download.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) start;


/**
 * Stop content download.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) stop;


/**
 * Pause content download.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) pause;


/**
 * Resume content download.
 *
 * @return    {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) resume;

@end
