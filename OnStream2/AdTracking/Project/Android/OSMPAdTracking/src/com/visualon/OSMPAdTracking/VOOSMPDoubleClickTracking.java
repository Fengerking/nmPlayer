package com.visualon.OSMPAdTracking;

import java.util.HashMap;

import android.content.Context;

import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPDoubleClickTracking extends VOOSMPBaseTracking {
	
	private Context mContext;
	private static String TAG = "@@@VOOSMPDoubleClick";
	
	/**
	 * 
	 */
	public VOOSMPDoubleClickTracking(Context context, HashMap<String, String> map) {
		super();
		// TODO Auto-generated constructor stub
		
		mContext = context;
	}

	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#sendTrackingEvent(com.visualon.OSMPPlayer.VOOSMPTrackingEvent)
	 */
	@Override
	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event) {
		// TODO Auto-generated method stub
		VO_OSMP_RETURN_CODE ret = super.sendTrackingEvent(event);
		
		switch (event.getEventType())
		{
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_IMPRESSION:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_START:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_FIRST_QUARTILE:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_MID_POINT:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_THIRDQUARTILE:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_COMPLETE:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_CREATIVE_VIEW:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_MUTE:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_UNMUTE:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_PAUSE:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_RESUME:
		case VO_OSMP_VMAP_TRACKING_EVENT_AD_FULL_SCREEN:
		{
			if (event.getEventText() == null)
			{
				voLog.e(TAG, "[TRACKING], doubleclick, event.getEventText() is null, " + event.getEventType().name());
				return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
			}else
			{
				for (int i = 0; i < event.getEventText().length; i++)
				{
					voLog.i(TAG, "[TRACKING], ready send, doubleclick url is " + event.getEventText()[i]);
					VOOSMPHTTPAsyncTask httpTask = new VOOSMPHTTPAsyncTask();
					httpTask.execute(event.getEventText()[i]);

				}
			}
			break;
		}
		default:
//			voLog.e(TAG, "[TRACKING] Don't find action.mPeriodID on ADSInfo. , don't sendTrackingEvent");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;

		}
		
		
		
		return ret;
	}



}
