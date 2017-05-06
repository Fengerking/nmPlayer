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
extern "C"{
#endif /* __cplusplus */
    
    /**
     * Enumeration of HDMI connection statuses
     */
    typedef enum {
        /** HDMI state change to disconnected  */
        VO_OSMP_HDMISTATE_DISCONNECT = 0,
        /** HDMI state change to connected     */
        VO_OSMP_HDMISTATE_CONNECT    = 1,
        /** Could not detect HDMI connection state */
        VO_OSMP_HDMISTATE_UNKNOWN    = 2,
        
        VO_OSMP_HDMISTATE_MAX        = 0X7FFFFFFF
    } VO_OSMP_HDMI_CONNECTION_STATUS;
    
#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

@protocol onHDMIConnectionChangeDelegate;


@protocol VOCommonPlayerHDMI <NSObject>

/**
 * Enable/disable HDMI connection detection.
 *
 * @param   value [in] Enable/disable; true to enable HDMI detection, false to disable.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful.
 */
- (VO_OSMP_RETURN_CODE) enableHDMIDetection:(bool)value;

/**
 * Check if HDMI connection detection is supported.
 *
 * @return  true if HDMI connection detection is supported, false if not supported
 */
- (bool) isHDMIDetectionSupported;

/**
 * Check if HDMI is connected.
 *
 * @return  HDMI connection state, refer to {@link VO_OSMP_HDMI_CONNECTION_STATUS }
 */
- (VO_OSMP_HDMI_CONNECTION_STATUS) getHDMIStatus;

/**
 * Set a delegate for HDMI Connection change callback.
 *
 * @param   listener [in] Listener, an onHDMIConnectionChangeListener instance.
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 */
- (VO_OSMP_RETURN_CODE) setOnHDMIConnectionChangeDelegate:(id <onHDMIConnectionChangeDelegate>)delegate;

@end

@protocol onHDMIConnectionChangeDelegate<NSObject>
/**
 * HDMI state change event listener.
 *
 * @param   nID [in] Event ID type, refer to {@link VO_OSMP_HDMI_CONNECTION_STATUS }
 *
 * @return  {@link VO_OSMP_ERR_NONE} if successful
 *
 */
- (VO_OSMP_RETURN_CODE) onHDMIStateChangeEvent:(VO_OSMP_HDMI_CONNECTION_STATUS)nID;

@end

