package com.visualon.OSMPUtils;

public class voOSHTTPProxy {
    
    /*!<Proxy server host name or ip address, must not be null */
    private String  mProxyHost;
    /*!<Proxy server port number,must not be null */
    private int             mProxyPort;
    /*!<Indicates some property of this structure, not used right now */
    private  int    mFlag;
    /*!<With some special Flag, this field may use */
    private Object      mFlagData;
    
    /**
     * Constructor voOSHTTPHeader class
     * 
     * @param Proxy server host name or ip address, must not be null
     * 
     * @param port Proxy server port number,must not be null
     * 
     * @param flag Indicates some property of this structure, not used right now, value is 0 now
     * 
     * @param flag With some special flag, this field may use, value is null now
     */
    public voOSHTTPProxy(String host, int port, int flag, Object flagData) {
        super();
        mProxyHost = host;
        mProxyPort = port;
        mFlag = flag;
        mFlagData = flagData;
    }
    public String getmProxyHost() {
        return mProxyHost;
    }
    
    public int getmProxyPort() {
        return mProxyPort;
    }
    
    public int getmFlag() {
        return mFlag;
    }
    
    public Object getmFlagData() {
        return mFlagData;
    }

}
