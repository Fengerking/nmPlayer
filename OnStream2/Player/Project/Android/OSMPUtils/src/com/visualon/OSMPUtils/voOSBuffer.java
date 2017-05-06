/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

package com.visualon.OSMPUtils;

public interface voOSBuffer {

	/** return the timestamp of buffer */
    long getTimestamp();

    /** return the size of buffer in bytes */
    int  getBufferSize();

    /** return buffer array */
    byte[] getBuffer();
}
