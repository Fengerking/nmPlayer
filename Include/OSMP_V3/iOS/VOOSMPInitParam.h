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

@interface VOOSMPInitParam : NSObject
{
    id          _context;
    NSString*   _libraryPath;
    long        _fileSize;
}


/**
 * Application context.
 */
@property (readwrite, retain, getter=getContext) id context;


/**
 * Library directory path name.
 */
@property (readwrite, retain, getter=getLibraryPath) NSString* libraryPath;


/**
 * Actual file size, only if {@link VO_OSMP_FLAG_INIT_ACTUAL_FILE_SIZE} is available
 *
 * @deprecated This function is replaced by {@link VOOSMPOpenParam}.
 */
@property (readwrite, assign, getter=getFileSize) long fileSize __attribute__((deprecated));


/**
 * Initialize filter with values.
 *
 * @return	object if successful; nil if unsuccessful
 */
- (id) init;

@end
