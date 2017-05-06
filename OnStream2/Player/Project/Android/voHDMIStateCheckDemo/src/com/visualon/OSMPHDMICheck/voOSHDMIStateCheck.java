/**
 */
package com.visualon.OSMPHDMICheck;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;
import java.util.List;

import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Handler;
import android.os.IInterface;
import android.os.Message;
import android.util.Log;

import com.visualon.OSMPHDMICheck.voOSHDMIBroadcastReceiver.ICheckHDMIState;

/**
 * Check HDMI state.
 * <p>
 * To use HDMI module, the simplest way is follow the below steps:
 *<p>
 *1.Put package voOSHDMICheck.jar into the reference path.<br/>
 *
 *2. import HDMI related classes.<br/>
 *<pre>
 *	import com.visualon.OSMPHDMICheck.voOSHDMIStateCheck;
 *	import com.visualon.OSMPHDMICheck.voOSHDMIStateCheck.onHDMIStateChangeListener;
 *</pre>
 *
 *3.Create an instance  of class voOSHDMIStateCheck voOSHDMIStateCheck.<br/>
 *<pre>
 *	m_HDMIStateCheck = new voOSHDMIStateCheck(this);
 *</pre>
 *
 *4.Set the listener for HDMI state change events. <br/>
 *<pre>
 *	m_HDMIStateCheck.setOnHDMIStateChangeListener(this);
 *</pre>
 *
 *5.Over ride HDMI event procedure {@link onHDMIStateChangeListener#onHDMIStateChangeEvent onHDMIStateChangeEvent()}, 
 *write your own event process code for events {@link #HDMISTATE_INITED HDMISTATE_INITED}, 
 *{@link #HDMISTATE_CONNECT HDMISTATE_CONNECT}, and {@link #HDMISTATE_DISCONNECT HDMISTATE_DISCONNECT}.<br/>
 *
 *6.Call {@link #Release() Release()} when the application exits.  Generally, it could be called in onDestroy().<br/>
 *<pre>
 *	m_HDMIStateCheck.Release();
 *</pre>
 *
 *Note: <br>
 *Some devices under some occasions will call onCreate() repeatedly. Call restart() in onCreate() to prevent this behavior cause chaos.<br/>
 *<pre>
 *      if(m_HDMIStateCheck != null){
 *               m_HDMIStateCheck.restart(this);
 *               return;
 *       }
 *</pre>
 *       <p>
 *Supported devices list:<p>
 *<pre>
 *	Motorola MZ606 (Android 3.1)
 *	Motorola Xoom (Android 4.0.3)
 *	Sony Ericsson LT18i-Android 2.3.4
 *	Samsung GT-P7310-Android 3.1
 *	HTC PC36100-Android 2.2
 *	Samsung GT-P7510-Android 3.0.1
 *	Motorola XT928-Android 2.3.6
 *	Motorola XT883-Android 2.3.4
 *	LG-P920-Android 2.2.2
 *	Sony LT26i-Android 2.3.7
 *	Motorola Xoom-Android 4.0.4
 *	Sharp SH8298U-Ansha 2.3.3 (Kernel:2.6.35.7)
 *	Asus Transformer Prime TF201-4.0.3 (Kernel:2.6.39.4-00003-g2108187)
 *	Acer A510-Android 4.0.3
 *</pre>
 *
 * @author wu_jing
 * 
 */
public class voOSHDMIStateCheck implements ICheckHDMIState{
	private static final String TAG = "HDMIStateCheck";
	
	/* event id */
	/**
	 * First check
	 * <p>
	 * could use {@link #isHDMIConnected() isHDMIConnected()} function to get HDMI state after receive this msg
	 */
	public static final int HDMISTATE_INITED = 0x800;
	/**
	 * HDMI state change to connected
	 */
	public static final int HDMISTATE_CONNECT = 1;
	/**
	 * HDMI state change to disconnected
	 */
	public static final int HDMISTATE_DISCONNECT = 0;
	
	/**
	 * set hdmi state change event listener
	 * <p>
	 * Event:
	 * <pre>
	 * {@link #HDMISTATE_INITED HDMISTATE_INITED} = 0x800;
	 * {@link #HDMISTATE_CONNECT HDMISTATE_CONNECT} = 1;
	 * {@link #HDMISTATE_DISCONNECT HDMISTATE_DISCONNECT} = 0;
	 * </pre>
	 * 
	 * @author wu_jing
	 * 
	 *
	 */
	public interface onHDMIStateChangeListener {
		/**
		 * HDMI state change event listener
		 * 
		 * @param nID Event type
		 * <pre>
		 * {@link #HDMISTATE_INITED HDMISTATE_INITED}
		 * 	Call {@link #isHDMIConnected() isHDMIConnected()} function to get HDMI state after receive this msg
		 * 	nObj is Integer type, value is:
		 * 		0-disconnect;
		 * 		1-connect;
		 *
		 * {@link #HDMISTATE_CONNECT HDMISTATE_CONNECT}
		 * express  HDMI connect
		 * {@link #HDMISTATE_DISCONNECT HDMISTATE_DISCONNECT}
		 * express  HDMI disconnect
		 * </pre>
		 * @param nObj Event value, 
		 * 
		 */
		public void onHDMIStateChangeEvent(int nID, Object nObj);
	}

	/* protected variable */
	protected Context m_Context;
	protected voOSHDMIBroadcastReceiver m_HDMIBroadcastReceiver = null;
	protected voOSHDMIDeviceType m_ManufacturerModel = voOSHDMIDeviceType.NoHDMI;
	protected onHDMIStateChangeListener m_OnHDMIStateChangeListener = null;
	protected IntentFilter m_ifReceiveIntent = null;

	protected int m_nHDMIState = -1;

	/* Message handler */
	protected Message m_Message;

	//hdmi state change event handle
	protected Handler m_Handler = new Handler(){

		public void handleMessage (Message msg) {
			if (m_OnHDMIStateChangeListener != null)
				m_OnHDMIStateChangeListener.onHDMIStateChangeEvent(msg.what, msg.obj);
			
			super.handleMessage(msg);
		}
	};
	
	/**create broadcast receiver
	 * 
	 */
	protected void excuteStart(){
		String strAction = "";

		//receive broadcast by manufacturer and model 
		switch (m_ManufacturerModel) {
		case StandardVersion3: {
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_HDMI_AUDIO_PLUG;
			m_ifReceiveIntent.addAction(strAction);

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
			
			if(isSupported()){
				InitCheck_v3();
			}
		}
			break;
		case Motorola_V2_3: {
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_MOTO_EX_DISPLAY_STATE;
			m_ifReceiveIntent.addAction(strAction);

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
		}
			break;
		case SE_LTs: {
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_SE_HDMI_EVENT;
			m_ifReceiveIntent.addAction(strAction);

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
		}
			break;
		case HTC_PC36100:
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_HTC_HEADSET_PLUG;
			m_ifReceiveIntent.addAction(strAction);

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
			break;
		case LG_P920:
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_LG_HDMI_EVENT;
			m_ifReceiveIntent.addAction(strAction);

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
			break;
		case SHARP_SHs:
			initSharpHDMIState();
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_SHARP_HDMI_CONNECTED_EVENT;
			m_ifReceiveIntent.addAction(strAction);
			m_ifReceiveIntent.addAction(voOSHDMIBroadcastAction.ACTION_SHARP_HDMI_DISCONNECTED_EVENT);
			m_ifReceiveIntent.addCategory("android.intent.category.DEFAULT");

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
			break;
		case StandardVersion4:
			m_ifReceiveIntent = new IntentFilter();

			strAction = voOSHDMIBroadcastAction.ACTION_HDMI_PLUGGED;
			m_ifReceiveIntent.addAction(strAction);

			m_Context.registerReceiver(m_HDMIBroadcastReceiver, m_ifReceiveIntent);
			break;
		default:
			;
		}
				
	}

	/**init HDMI state for sharp
	 * will send event with init state
	 * 
	 */
	protected void initSharpHDMIState(){
		try {
            Context ctxHDMISharp = m_Context.createPackageContext("jp.co.sharp.android.hdmi",
                Context.CONTEXT_INCLUDE_CODE | Context.CONTEXT_IGNORE_SECURITY);

            Class<?> clHDMIState = Class.forName(
                "jp.co.sharp.android.hdmi.service.external.HDMIState",
                true, ctxHDMISharp.getClassLoader());
            
            Method getHDMIState = clHDMIState.getMethod("getHDMIState", null);
            
            Constructor consHDMIState = clHDMIState.getConstructor(Context.class);
            Object objHDMIState = consHDMIState.newInstance(m_Context);   
            
            int nState = (Integer)getHDMIState.invoke(objHDMIState,null);
            
            checkHDMIState(nState);

        } catch(Exception ex) {
//        	ex.printStackTrace();
        }
	}
	
	/**recreate handle and broadcast receiver
	 * for samsung
	 * @param c
	 * 
	 */
	public void restart(Context c){
		m_Context = c;

    	Release();
		
		m_HDMIBroadcastReceiver = new voOSHDMIBroadcastReceiver(m_ManufacturerModel);
		m_HDMIBroadcastReceiver.setCheckHDMIStateInterface(this);
		
		new Thread(new Runnable(){

			@Override
			public void run() {
				excuteStart();
			}
			
		}).start();
	}

	/**Constructor
	 * @param c
	 * 
	 */
	public voOSHDMIStateCheck(Context c) {
		//check manufacturer and model
		checkManufacturerAndModel();
		
		restart(c);
	}
	
	/**release broadcast bind
	 * 
	 */
	public void Release() {
		if (m_HDMIBroadcastReceiver != null ){
			if((m_ManufacturerModel != voOSHDMIDeviceType.NoHDMI)
					&&(m_ManufacturerModel != voOSHDMIDeviceType.SHARP_SHs))
				m_Context.unregisterReceiver(m_HDMIBroadcastReceiver);
			
			m_HDMIBroadcastReceiver = null;
		}
	}
	
	/**check device support state of multi-graphics and 1080p
	 * 
	 * @return boolean:true-support;false-do not support;
	 */
	protected boolean isDeviceSupportMultiGraphicsAnd1080p(){
		boolean bIsSupported = false;

		File fDev = new File("/sys/class/graphics/");
		if(!fDev.exists()||!fDev.isDirectory()){
			return bIsSupported;
		}
		
		String[] strGraphicsFiles = fDev.list();
		
		if(strGraphicsFiles.length < 2) return false;
		
		for(String strGraphicsFile:strGraphicsFiles){
			String strFilePath = fDev.getPath() + "/" + strGraphicsFile + "/modes";
			File fGraphicsFile = new File(strFilePath);
			
			if(!fGraphicsFile.exists()) continue;
			
			InputStream isModes = null;
			try {
				isModes = new FileInputStream(fGraphicsFile);
				
				DataInputStream disModes = new DataInputStream(isModes);
				
				String strModes = null;
				while((strModes = disModes.readLine()) != null){
					if(strModes.contains("1280")){
						bIsSupported = true;
						break;
					}
				}
				
				isModes.close();
			} catch (FileNotFoundException e) {
			} catch (IOException e) {
			}
		}
		
		return bIsSupported;
	}
	
	/**check hdmi support state of LG devices
	 * 
	 * @return boolean:true-support hdmi;false-do not support hdmi;
	 */
	protected boolean isLGHDMISupported(){
		try {
//			PackageManager pm = m_Context.getPackageManager();
//			List<PackageInfo> lsPiServices = pm.getInstalledPackages(PackageManager.GET_SERVICES);
//			Log.i(TAG,"packname of services:");
//			
//			for(PackageInfo pi:lsPiServices){
//				String strName = pi.packageName;
//				String version = pi.versionName;
//				Log.i(TAG,strName);
//			}			
//
//			List<PackageInfo> lsPiu = pm.getInstalledPackages(PackageManager.GET_UNINSTALLED_PACKAGES);
//			
//			Log.i(TAG,"packname of uninstalled packages:");
//			for(PackageInfo pi:lsPiu){
//				String strName = pi.packageName;
//				String version = pi.versionName;
//				Log.i(TAG,strName);
//			}			

            Context ctxHDMINative = m_Context.createPackageContext("android",
                Context.CONTEXT_INCLUDE_CODE | Context.CONTEXT_IGNORE_SECURITY);

            Class<?> cl = Class.forName(
                "android.media.HDMINative", true, ctxHDMINative.getClassLoader());

            Method nativeIsHdmiEnabled = cl.getMethod("nativeIsHdmiEnabled", null);
            int nIsHdmiEnabled = (Integer)nativeIsHdmiEnabled.invoke(null);
            
            return nIsHdmiEnabled == 1;

        } catch(Exception ex) {
        	return false;
        }
	}
	
	/**check hdmi support state of Sharp devices
	 * 
	 * @return boolean:true-support hdmi;false-do not support hdmi;
	 */
	protected boolean isSharpHDMISupported(){
        try {
			Context ctxHDMISharp = m_Context.createPackageContext("jp.co.sharp.android.hdmi",
			    Context.CONTEXT_INCLUDE_CODE | Context.CONTEXT_IGNORE_SECURITY);
			
			return true;
		} catch (NameNotFoundException e) {
			return false;
		}
	}
	
	/**check hdmi support state
	 * 
	 * @return boolean:true-support hdmi;false-do not support hdmi;
	 */
	public boolean isSupported(){
		if(m_ManufacturerModel == voOSHDMIDeviceType.NoHDMI)
			return false;
		
		File fDev = new File("/sys/class/switch/hdmi/");//v3.0
		
		boolean bIsSupported = fDev.exists();
		
		if(bIsSupported) {
			return bIsSupported;
		}
		
		if(m_ManufacturerModel == voOSHDMIDeviceType.SE_LTs){			
			return isDeviceSupportMultiGraphicsAnd1080p();
		}
		else if(m_ManufacturerModel == voOSHDMIDeviceType.LG_P920){
			return isLGHDMISupported();
		}
		else if(m_ManufacturerModel == voOSHDMIDeviceType.SHARP_SHs){
			return isSharpHDMISupported();
		}
		
		fDev = new File("/sys/class/drm/");//moto 2.n
		
		if(!fDev.exists()||!fDev.isDirectory()){
			return bIsSupported;
		}
			
		String[] strFiles = fDev.list();
		
		for(String strFileName:strFiles){
			if(strFileName.contains("HDMI")){
				bIsSupported = true;
				break;
			}
		}
		
		return bIsSupported;
	}
	
	/**query hdmi connect state
	 * 
	 * @return boolean:true-hdmi connected;false-hdmi disconnnected or before initial finished;
	 * 
	 * attention: This function can only be used after initial finish
	 * 				initial finish:nID in onHDMIStateChangeEvent is HDMISTATE_INITED
	 */
	public boolean isHDMIConnected() {
		return (m_nHDMIState == 1);
	}

	/**set hdmi state change event listener
	 * <br/>
	 * @param listener
	 * 
	 */
	public void setOnHDMIStateChangeListener(onHDMIStateChangeListener listener) {
		m_OnHDMIStateChangeListener = listener;
	}
	
	/**check device manufacture and model
	 * set m_ManufacturerModel value
	 * 
	 */
	private void checkManufacturerAndModel() {
		String strManufacturer = android.os.Build.MANUFACTURER;
		String strModel = android.os.Build.MODEL;
//		String strSystemVersion = android.os.Build.BOARD;
//		String strVersion = android.os.Build.BRAND;
//		String strVersion1 = android.os.Build.DEVICE;
//		String strVersion2 = android.os.Build.DISPLAY;
//		String strVersion3 = android.os.Build.FINGERPRINT;
//		String strVersion4 = android.os.Build.ID;
//		String strVersion5 = android.os.Build.TAGS;
//		String strVersion6 = android.os.Build.USER;
		String strSDK = android.os.Build.VERSION.SDK;
		int nSDK = Integer.valueOf(strSDK);
//		Log.i(TAG,"strManufacturer:"+strManufacturer);
//		Log.i(TAG,"strModel:"+strModel);
//		Log.i(TAG,"nSDK:"+nSDK);

		if (nSDK > 14) {
			m_ManufacturerModel = voOSHDMIDeviceType.StandardVersion4;
		}
		else if(nSDK > 10){
			m_ManufacturerModel = voOSHDMIDeviceType.StandardVersion3;
		}
		else if (strManufacturer.compareToIgnoreCase("Motorola") == 0) {
			if(nSDK > 8){
				m_ManufacturerModel = voOSHDMIDeviceType.Motorola_V2_3;
			}
			/*if (strModel.compareToIgnoreCase("MZ606") == 0) {//3.1
				m_ManufacturerModel = ManufacturerModel.StandardVersion3_1;
			}
			else if(strModel.compareToIgnoreCase("MB860") == 0){//2.3
				m_ManufacturerModel = ManufacturerModel.Motorola_MB860;
			}else if(strModel.compareToIgnoreCase("XT928") == 0){//2.3
				m_ManufacturerModel = ManufacturerModel.Motorola_MB860;
			}*/
		}
		else if (strManufacturer.contains("Sony")){
//			if (strModel.compareToIgnoreCase("LT18i") == 0) {
				m_ManufacturerModel = voOSHDMIDeviceType.SE_LTs;
//			}
		}
		else if (strManufacturer.compareToIgnoreCase("htc") == 0){
			if (strModel.compareToIgnoreCase("PC36100") == 0) {
				m_ManufacturerModel = voOSHDMIDeviceType.HTC_PC36100;
			}
		}	
		else if (strManufacturer.compareToIgnoreCase("LGE") == 0){
//			if(strModel.compareToIgnoreCase("LG-P920") == 0){
				m_ManufacturerModel = voOSHDMIDeviceType.LG_P920;
//			}
		}
		else if (strManufacturer.compareToIgnoreCase("Sharp") == 0){
			m_ManufacturerModel = voOSHDMIDeviceType.SHARP_SHs;
		}	
//		else if (strManufacturer.compareToIgnoreCase("samsung") == 0){
//			if(strModel.compareToIgnoreCase("GT-P7310") == 0){
//				m_ManufacturerModel = HDMIDeviceType;
//			}
//		}
		else
			m_ManufacturerModel = voOSHDMIDeviceType.NoHDMI;
	}
	
	/**check hdmi state and send init event message-for samsung
	 * 
	 */
	protected void InitCheck_v3(){
		int nState = 0;
		File fDev = new File("/sys/class/switch/hdmi/state");
		
		if(fDev.exists()){
			InputStream isState = null;
			try {
				isState = new FileInputStream(fDev);
				
				DataInputStream disState = new DataInputStream(isState);
				String strState = disState.readLine();
				
				nState = Integer.valueOf(strState);
				
				isState.close();
			} catch (FileNotFoundException e) {
				nState = 0;
			} catch (IOException e) {
				nState = 0;
			}
		}
			
		checkHDMIState(nState);
	}

	/**check hdmi state and send event message
	 * 
	 * @param nState
	 * 
	 */
	@Override
	public void checkHDMIState(int nState) {
		int nMsgId = -1;
		
		if (m_nHDMIState == -1) {
			m_nHDMIState = nState;
			nMsgId = HDMISTATE_INITED;
		}
		else{
			if (nState == m_nHDMIState)
				return;
	
			m_nHDMIState = nState;
	
			if (nState == 1) {
				nMsgId = HDMISTATE_CONNECT;
			} else {
				nMsgId = HDMISTATE_DISCONNECT;
			}
		}
			
		m_Message = m_Handler.obtainMessage(nMsgId,nState);
		
		while(m_OnHDMIStateChangeListener == null){
			try {
				Thread.sleep(10);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		m_Message.sendToTarget();
	}

}
