package com.visualon.OSMPBasePlayer;

import java.io.FileNotFoundException;

import com.visualon.OSMPUtils.voLog;
import com.visualon.OSMPUtils.voOSType;

import android.content.Context;

public class voOSDRM {
    private String TAG = getClass().getSimpleName();
    public static int VO_SOURCEDRM_FLAG_DRMINFO = 0x1;
	private static boolean mJNILoaded = false;
    protected Context mContext;

    /* Pointer of VO_DRM2_API */
    protected int mDRMAPIHandle = 0;

    /* pHandle inside VO_DRM2_API */
    private int mDRM2Handle = 0;
    
    private voOSBasePlayer mBasePlayer = null;
    
	public final static int VOSDK_PARAM_DRMFUNCOP = 0x14000000 | 0x16;
	public final static int VO_PID_DRM2_DataSource =  	2;
	public final static int VO_PID_DRM2_THIRDLIBOP = 	3;
	public final static int VO_PID_DRM2_THIRDFILEOP = 	4;
	
	voOSDRM(voOSBasePlayer basePlayer, String packagePath, String libName) throws FileNotFoundException
	{
		mBasePlayer = basePlayer;
		Integer contextHandle = new Integer(0);
		long ret = getDRMAPI(contextHandle, packagePath, libName, 0);
		int r = new Long(ret).intValue();
		voLog.i(TAG, "getDRMAPI, return is %d, integer is %d", ret, r);
		if (r != voOSType.VOOSMP_ERR_None)
			 throw new FileNotFoundException();
	}

    /* Get DRM API, the function must be earliest call 
     * pHandle [out] is DRMAPI Handle
     * nflag[in] is reserved.
     * return 0 if OK
     */
    private long getDRMAPI(Integer pHandle, String packagePath, String libName, long nflag)
	{
	    long nRet;
	    if(!mJNILoaded)
		{
			loadJNI(packagePath);
			if(!mJNILoaded)
			{
				voLog.e(TAG, "Fail to load JNI library from Init()");
                return voOSType.VOOSMP_ERR_JNI;
			}
		}
	    nRet = nativeGetDRMAPI(pHandle, packagePath, libName, nflag);
	    mDRMAPIHandle = pHandle.intValue();
	    return nRet;
	}

    /*
     * Initial DRM Egine, call this function after getDRMAPI function
     * nflag[in] is reserved.
     * return 0 if OK
     */
    public long init(long nflag)
	{
	    Integer pDRMHandle = new Integer(mDRMAPIHandle);
	    Integer pHandle = new Integer(0);
	    long nRet;
	    
	    nRet = nativeInit(pDRMHandle, pHandle, nflag);
	    mDRM2Handle = pHandle;
	    return nRet;
	}
    
    /*
     * Destroy DRM engine, must be call when you quit the program. 
     * return 0 if OK
     * 
     * 
     */
    public long uninit()
	{
	    long ret = nativeUninit(mDRMAPIHandle, mDRM2Handle);

        if (voOSType.VOOSMP_ERR_None == ret) {
            mDRM2Handle = 0;
        }
        
	    return ret;
	}

    public long setParameter(long uID, Object pParam)
	{
	    return nativeSetParameter(mDRMAPIHandle, mDRM2Handle, uID, pParam);
	}
    
    public long getParameter(long uID, Object pParam)
    {
    	return nativeGetParameter(mDRMAPIHandle, mDRM2Handle, uID, pParam);
    }
    
    public long getInternalAPI(Object pParam)
    {
    	return nativeGetInternalAPI(mDRMAPIHandle, mDRM2Handle, pParam);
    }

    public int voDRMCallback(int id, int param1, int param2, Object obj)
    {
    	if (mBasePlayer == null || mBasePlayer.mRequestListener == null)
    		return voOSType.VOOSMP_ERR_Uninitialize;
    	
    	voLog.i(TAG, "voDRMCallback , id is %s", Integer.toHexString(id));
    	return mBasePlayer.mRequestListener.onRequest(id, param1, param2, obj);
    }
    
    
    static
	{
		try {
			
			System.loadLibrary("voDRMJNI_OSMP");
			mJNILoaded = true;
		} catch (UnsatisfiedLinkError e) {
			// TODO Auto-generated catch block
			mJNILoaded = false;
			e.printStackTrace();
		}
	}
    private static void loadJNI(String libPath)
	{
		
		try {
			
			System.load(libPath +"voDRMJNI_OSMP");
			mJNILoaded  = true; 
			
		} catch (Exception  e) {
			
			mJNILoaded = false;
			e.printStackTrace();
			
			// TODO: handle exception
		}
	}

    public native long nativeGetDRMAPI(Integer pHandle, String packagePath, String libName, long nflag);
    public native long nativeInit(Integer pDRMHandle, Integer pHandle, long nflag);
    public native long nativeUninit(int pDRMHandle, int pHandle);
    public native long nativeSetThirdpartyAPI(int pDRMHandle, int pHandle, Object pParam);
    public native long nativeGetInternalAPI(int pDRMHandle, int pHandle, Object pParam);
    public native long nativeSetParameter(int pDRMHandle, int pHandle, long uID, Object pParam);
    public native long nativeGetParameter(int pDRMHandle, int pHandle, long uID, Object pParam);
    

}
