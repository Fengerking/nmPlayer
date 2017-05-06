/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
************************************************************************/

package com.visualon.OSMPPlayer;

import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public class VOOSMPHTTPProxy
{
    /*!<Proxy server host name or ip address, must not be null */
    private String  mProxyHost;
    /*!<Proxy server port number,must not be null */
    private int     mProxyPort;
   

    public VOOSMPHTTPProxy( )
    {
        super();
    }

    /**
     *
     * @param host host name or ip address of the proxy server
     * @param port proxy server port number
     *
     */
    public VOOSMPHTTPProxy(String host, int port) {
        super();
        mProxyHost = host;
        mProxyPort = port;
    }
 
    /**
     * Get host name or IP address of the proxy server 
     *
     * @return  host name or IP address of the proxy server
     */
    public String getProxyHost() {
        return mProxyHost;
    }
   
    /**
     * Get proxy server port number 
     *
     * @return  proxy server port number
     */
    public int getProxyPort() {
        return mProxyPort;
    }
   
    /**
     * Set proxy server port number 
     *
     * @param host host name or IP address of the proxy server
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setProxyHost(String host) {
        this.mProxyHost = host;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    /**
     * Set proxy server port number 
     *
     * @param port proxy server port number
     *
     * @return  {@link VO_OSMP_RETURN_CODE#VO_OSMP_ERR_NONE} if successful.
     */
    public VO_OSMP_RETURN_CODE setProxyPort(int port) {
        this.mProxyPort = mProxyPort;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }
}
