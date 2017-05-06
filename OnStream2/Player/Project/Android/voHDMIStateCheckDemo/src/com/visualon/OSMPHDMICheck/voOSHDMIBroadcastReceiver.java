/**
 * 
 */
package com.visualon.OSMPHDMICheck;

import java.util.Set;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

/**
 * @author wu_jing
 *
 */
public class voOSHDMIBroadcastReceiver extends BroadcastReceiver {
	private static final String TAG = "HDMIBroadcastReceiver";
	
	protected voOSHDMIDeviceType m_ManufacturerModel = voOSHDMIDeviceType.NoHDMI;
	protected ICheckHDMIState m_implCheckHDMIState = null;
	
	public interface ICheckHDMIState{
		public void checkHDMIState(int nState);
	}
	
	public voOSHDMIBroadcastReceiver(voOSHDMIDeviceType model){
		m_ManufacturerModel = model;
	}
	
	public void setCheckHDMIStateInterface(ICheckHDMIState impl){
		m_implCheckHDMIState = impl;
	}

	/* (non-Javadoc)
	 * @see android.content.BroadcastReceiver#onReceive(android.content.Context, android.content.Intent)
	 */
	@Override
	public void onReceive(Context context, Intent intent) {

		String action = intent.getAction();
		Bundle extras = (intent != null) ? intent.getExtras() : null;

		switch (m_ManufacturerModel) {
		case Motorola_V2_3:
			receiveMotorola_V2_3Broadcast(action, extras);
			break;
		case SE_LTs: 
			receiveSELTsBroadcast(action, extras);
			break;
		case HTC_PC36100: 
			receiveHTCPC36100Broadcast(action, extras);
			break;
		case LG_P920: 
			receiveLGP920Broadcast(action, extras);
			break;
		case SHARP_SHs: 
			receiveSharpSHsBroadcast(action, extras);
			break;
		case StandardVersion3:
			receiveVersion3Broadcast(action, extras);
			break;
		case StandardVersion4:
			receiveVersion4Broadcast(action, extras);
			break;
		case NoHDMI:
		default:
			;
		}
	}

	//deal with broadcast of motorola devices with android 2.3.x
	//[in]-action
	//		extras
	//[out]-void
	private void receiveMotorola_V2_3Broadcast(String action, Bundle extras) {
		if (action.equals(voOSHDMIBroadcastAction.ACTION_MOTO_EX_DISPLAY_STATE)) {
			if (extras == null)
				return;
				 
			if (m_implCheckHDMIState == null)
				return;
			
			int nState = extras.getInt("hdmi");

			m_implCheckHDMIState.checkHDMIState(nState);
		}
	}

	//deal with broadcast of sony ericsson LT serial
	//[in]-action
	//		extras
	//[out]-void
	private void receiveSELTsBroadcast(String action, Bundle extras) {
		if (action.equals(voOSHDMIBroadcastAction.ACTION_SE_HDMI_EVENT)) {	        
			if (extras == null)
				return;
			
			String strHDMIState = extras.getString("com.sonyericsson.intent.extra.HDMI_STATE");

			if (m_implCheckHDMIState == null)
				return;

			int nState = (strHDMIState.compareToIgnoreCase("HDMI_IN_USE") == 0)?1:0;

			m_implCheckHDMIState.checkHDMIState(nState);
		}
	}
	
	//deal with broadcast of htc PC36100
	//[in]-action
	//		extras
	//[out]-void
	private void receiveHTCPC36100Broadcast(String action, Bundle extras) {
		if (action.equals(voOSHDMIBroadcastAction.ACTION_HTC_HEADSET_PLUG)) {	       
			if (extras == null)
				return;
			
			 String strName = extras.getString("name");
			 
 			if (strName.compareToIgnoreCase("Headset") != 0)
 				return;
 
 			if (m_implCheckHDMIState == null)
 				return;

 			int nState = (extras.getInt("state") == 2048)?1:0;
 
 			m_implCheckHDMIState.checkHDMIState(nState);
		}
	}
	
	//deal with broadcast of LG P920
	//[in]-action
	//		extras
	//[out]-void
	private void receiveLGP920Broadcast(String action, Bundle extras) {
		if (action.equals(voOSHDMIBroadcastAction.ACTION_LG_HDMI_EVENT)) {	       
			if (extras == null)
				return;
			
			String strName = extras.getString("name");
			 
 			if (strName.compareToIgnoreCase("tv") != 0)
 				return;
 
 			if (m_implCheckHDMIState == null)
 				return;

 			int nState = extras.getInt("state");
 
 			m_implCheckHDMIState.checkHDMIState(nState);
		}
	}
	
	/**deal with broadcast of Sharp SH serial
	 * 
	 * @param [in]-action
	 * @param [in]-extras
	 * @return void
	 */
	private void receiveSharpSHsBroadcast(String action, Bundle extras) {
 			if (m_implCheckHDMIState == null)
 				return;
 			
		if (action.equals(voOSHDMIBroadcastAction.ACTION_SHARP_HDMI_CONNECTED_EVENT)) {
 			m_implCheckHDMIState.checkHDMIState(1);
		}
		else if(action.equals(voOSHDMIBroadcastAction.ACTION_SHARP_HDMI_DISCONNECTED_EVENT)) {	
 			m_implCheckHDMIState.checkHDMIState(0);   
		}
	}
	
	//deal with broadcast of android version 3.1
	//[in]-action
	//		extras
	//[out]-void
	private void receiveVersion3Broadcast(String action, Bundle extras) {
		if (action.equals(voOSHDMIBroadcastAction.ACTION_HDMI_AUDIO_PLUG)) {
			if (extras == null) return;

			String strName = extras.getString("name");

			if (strName.compareToIgnoreCase("hdmi") != 0)
				return;

			if (m_implCheckHDMIState == null)
				return;

			int nState = extras.getInt("state");

			Log.i(TAG,"nState = "+nState);
			m_implCheckHDMIState.checkHDMIState(nState);
		}
	}
	
	//deal with broadcast of android version larger than 4.0
	//[in]-action
	//		extras
	//[out]-void
	private void receiveVersion4Broadcast(String action, Bundle extras) {
		if (action.equals(voOSHDMIBroadcastAction.ACTION_HDMI_PLUGGED)) {
			// motorola xoom
			if (extras != null) {
				// Set<String> keySet = extras.keySet();
				// for(String strKey:keySet){
				// Object obj = extras.get(strKey);
				// Log.i("HDMI","0");
				// }

				if (m_implCheckHDMIState == null)
					return;

				boolean bState = extras.getBoolean("state");
				int nState = bState ? 1 : 0;

				m_implCheckHDMIState.checkHDMIState(nState);
			}
		}
	}
}
