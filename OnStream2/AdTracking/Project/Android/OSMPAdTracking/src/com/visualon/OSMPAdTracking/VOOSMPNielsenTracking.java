package com.visualon.OSMPAdTracking;

import java.util.HashMap;
import java.util.Map;
import android.content.Context;



import com.nielsen.collection.NielsenVideoBeaconActivity;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPNielsenTracking extends VOOSMPBaseTracking {
	
	private static final String TAG = "@@@VOOSMPNielsenTracking";
	
	private Context mContext                        = null;
//	private CombinedBeaconApplication mParentApp    = null;
	private NielsenVideoBeaconActivity mBeacon      = null;
	
	private String txtClientId                      = "us-700144";
	private String txtVcId                          = "c01";
	private String txtSfCode 						= "us";
	private String txtProd 							= "vc";
	private String txtCiSuffix 						= "";
	private String pid;
	
	public VOOSMPNielsenTracking(Context context, String RSID, String trackingServer, String partnerID, String  networkString) {
		super();
		// TODO Auto-generated constructor stub
		mContext = context;
		pid = RSID;
		voLog.i(TAG, "VOOSMPNielsenTracking construct");
		
		mBeacon = new NielsenVideoBeaconActivity();

		Map<String, String> tempData = new HashMap<String, String>();
		tempData.put("clientid", txtClientId);
		tempData.put("vcid", txtVcId);
		tempData.put("cisuffix", txtCiSuffix);
		tempData.put("sfcode", txtSfCode);
		tempData.put("szprod", txtProd);
		tempData.put("10,25,75", "msgint");
		
		mBeacon.initialize(tempData, mContext.getSharedPreferences("com.nielsenbeacon.app", Context.MODE_PRIVATE));
	}


	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#sendTrackingEvent(com.visualon.OSMPPlayer.VOOSMPTrackingEvent)
	 */
	@Override
	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event) {
		// TODO Auto-generated method stub
		super.sendTrackingEvent(event);
		
		String type = "";
		String param1 = "";
		String param2 = "";
		String param3 = "";
		String param4 = "";
		
		if ((event == null || mADSInfo == null || mADSInfo.getPeriodList() == null)
				&& event.getEventType() != VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION)
			{
				voLog.e(TAG, "[TRACKING] VOOSMPTrackingEvent or ADSInfo is null, don't sendTrackingEvent");
				return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
			}
		
		VOOSMPAdPeriod period = getADSPeriod(event.getPeriodID());
		
		if (period == null )
		{
			voLog.e(TAG, "[TRACKING] Don't find action.mPeriodID on ADSInfo. , don't sendTrackingEvent");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
		}
		
		switch(event.getEventType())
		{
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART:
			{
				type = "15";
				param1 = period.getPeriodID();
				param2 = getADPos(period.getID(), event.getPlayingTime());
//				param3 = "<title>" + period.getPeriodTitle() + "</title><length>" 
//						+ (int)((period.getEndTime() - period.getStartTime())/1000) + "</length>";
				param3 = period.getPeriodTitle() + ","	+ (int)((period.getEndTime() - period.getStartTime())/1000);
				param4 = "1";
				
				break;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE:
			case VO_OSMP_ADS_TRACKING_EVENT_FORCESTOP:
			{
				type = "7";
				param1 = Integer.toString((int)(event.getPlayingTime()/1000));
				break;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_PAUSE:
			{
				if (event.getEventValue() == 1)
				{
					type = "6";
					param1 = Integer.toString((int)(event.getPlayingTime()/1000));
				}else if (event.getEventValue() == 0)
				{
					type = "5";
					param1 = Integer.toString((int)(event.getPlayingTime()/1000));
				}else
				{
				//	voLog.w(TAG, "[TRACKING] Neilsen, don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PAUSE, event value is " + event.getEventValue());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_SEEKS:
			{
				type = "8";
				param1 = Integer.toString((int)(event.getPlayingTime()/1000));
				param2 = Integer.toString((int)(event.getEventValue()/1000));
				break;
			}
			default:
			//	voLog.w(TAG, "[TRACKING] Nielsen , don't support this event, event type is " + event.getEventType().name() + " , event value is " + event.getEventValue());
				return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
		}
		
		sendBeaconMsg(type, param1, param2, param3, param4);
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	private void sendBeaconMsg(String event, String param1, String param2, String param3, String param4){
		try{
			if(event == null || event.trim().length() == 0){
				voLog.e(TAG, "error: "+"event param is empty in sendBeaconMsg");
				return;
			}
			
			//make our beacon call
			mBeacon.ggPM(event,
					((param1 != null) ? param1 : ""),
					((param2 != null) ? param2 : ""),
					((param3 != null) ? param3 : ""),
					((param4 != null) ? param4 : ""));
			printToLog(event, param1, param2, param3, param4);
		}catch(Exception ex){
			voLog.e(TAG, "error: don't send event.");
		}
	}
	
	private void printToLog(String event, String param1, String param2, String param3, String param4){
		try{
		String msg = String.format("PM(%s, %s, %s, %s, %s);\n", event,
				((param1 != "") ? param1 : "null"),
				((param2 != "") ? param2 : "null"),
				((param3 != "") ? param3 : "null"),
				((param4 != "") ? param4 : "null"));

		voLog.i(TAG, "[TRACKING], Nielsen is %s", msg);
		} catch(Exception ex){
			voLog.e(TAG, "error: do not send event.");
		}
	}
	
	@Override
	protected String getADPos(int periodID, long playingTime)
	{
		String value = super.getADPos(periodID, playingTime);
		if (!value.contains("content"))
		{
			value = value + "roll";
		}
		
		return value;
	}
	
	/*
	public class CombinedBeaconApplication extends Application{
		public String beaconMsgLog = "";
		
		private NielsenVideoBeaconActivity NVCBeacon;
		public NielsenVideoBeaconActivity getBeacon(){return NVCBeacon;}
		
		public Map<String, String> clipData;
		
	    @Override
	    public void onCreate() {
	    	super.onCreate();
	    	NVCBeacon = new NielsenVideoBeaconActivity();
	    	clipData = new HashMap<String, String>();
	    }
	    
		public void hideSoftKeyboard(View view) { 
	        InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE); 
	        imm.hideSoftInputFromWindow(view.getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS); 
	    }
	}
*/
}
