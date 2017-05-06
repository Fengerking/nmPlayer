package com.visualon.OSMPAdTracking;

import java.util.ArrayList;
import android.content.Context;

import com.visualon.OSMPPlayer.VOOSMPAdInfo;
import com.visualon.OSMPPlayer.VOOSMPAdTracking;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.*;
import com.visualon.OSMPUtils.*;




public class VOOSMPAdTrackingImpl implements VOOSMPAdTracking, VOOSMPAdTrackingServer{
	
	private static final String TAG = "@@@VOOSMPTrackingImpl";
	
	private ArrayList<VOOSMPBaseTracking> mTrackingServer;
	private Context mContext = null;
	private String mPartnerID, mNetworkString;

	/**
	 * 
	 */
	public VOOSMPAdTrackingImpl(Context context)
 {
		super();
		// TODO Auto-generated constructor stub
		voLog.i(TAG, "VOOSMPTrackingImpl construct");
		
		mContext = context;
		
		mTrackingServer = null;
		mTrackingServer = new ArrayList<VOOSMPBaseTracking>();
		
		
		if (mContext == null)
		{
			voLog.e(TAG, "context is null! App need an availabe context parameter.");
		}
	}

	@Override
	public VO_OSMP_RETURN_CODE initTracking() {
		voLog.i(TAG, "VOOSMPTrackingImpl initTracking");
		// TODO Auto-generated method stub
		if (mTrackingServer == null)
			mTrackingServer = new ArrayList<VOOSMPBaseTracking>();
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	@Override
	public VO_OSMP_RETURN_CODE uninitTracking() {
		// TODO Auto-generated method stub
		if (mTrackingServer != null)
		{
			mTrackingServer = null;
		}
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	@Override
	public VO_OSMP_RETURN_CODE setPlaybackInfo(VOOSMPAdInfo adsInfo) {
		// TODO Auto-generated method stub
		
		if (mTrackingServer == null || mTrackingServer.size() == 0)
		{
			voLog.e(TAG, "[TRACKING] Tracker Server is null or empty! Don't send event");
		}
		
		for (int i = 0; i < mTrackingServer.size(); i ++ )
		{
			VOOSMPBaseTracking tracking = mTrackingServer.get(i);
			tracking.setPlaybackInfo(adsInfo);
		}
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	@Override
	public VO_OSMP_RETURN_CODE addTrackingServer(VO_OSMP_AD_TRACKING_SERVER customerId, String rsid,
			String server, String partnerID, String  networkString,
			String edition, String siteType, String primaryReportID, String prop9) {
		// TODO Auto-generated method stub
		voLog.i(TAG, "[TRACKING] Server is %s, rsid is %s, server is %s, %s|%s", 
				customerId.name(), rsid, server, partnerID, networkString);
		
		mPartnerID = partnerID;
		mNetworkString = networkString;
		
		switch (customerId)
		{
			case VO_OSMP_AD_TRACKING_OMNITURE:
			{
				VOOSMPOmnitureTracking omniture = new VOOSMPOmnitureTracking(mContext, rsid, server, mPartnerID, mNetworkString,
						edition, siteType, primaryReportID, prop9);
				mTrackingServer.add(omniture);
				break;
			}
			case VO_OSMP_AD_TRACKING_DATAWARE:
			{
				VOOSMPDWTracking dw = new VOOSMPDWTracking(mContext, rsid, server, mPartnerID, mNetworkString);
				mTrackingServer.add(dw);
				break;
			}
			case VO_OSMP_AD_TRACKING_NIELSEN:
			{
				VOOSMPNielsenTracking nielsen = new VOOSMPNielsenTracking(mContext, rsid, server, mPartnerID, mNetworkString);
				mTrackingServer.add(nielsen);
				break;
			}
			case VO_OSMP_AD_TRACKING_COMSCORE:
			{
				VOOSMPComScoreTracking comScore = new VOOSMPComScoreTracking(mContext, rsid, server, mPartnerID, mNetworkString);
				mTrackingServer.add(comScore);
				break;
			}
			case VO_OSMP_AD_TRACKING_DOUBLECLICK:
				VOOSMPDoubleClickTracking  doubleClick = new VOOSMPDoubleClickTracking(mContext, null);
				mTrackingServer.add(doubleClick);
				break;
				
		}
		
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	@Override
	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event) {
		// TODO Auto-generated method stub
		
//		if (event.getEventType() == VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_CLICK_THROUGH)
//		{
//			VOOSMPHTTPAsyncTask httpTask = new VOOSMPHTTPAsyncTask();
//			httpTask.execute(event.getEventText());
//		}
		
		if (mTrackingServer == null || mTrackingServer.size() == 0)
		{
			voLog.e(TAG, "[TRACKING] Tracker Server is null or empty! Don't send event");
		}
		
		voLog.i(TAG,  "[TRACKING], sendTrackingEvent, event type is " + event.getEventType().name()
				+ " , event value is " + event.getEventValue() + " , event periodID is " + event.getPeriodID()
				+ " , playingTime is " + event.getPlayingTime() + ", getElapsedTime is " + event.getElapsedTime()
				+ ". mTrackingServer size is " + mTrackingServer.size());
		
		for (int i = 0; i < mTrackingServer.size(); i ++ )
		{
			VOOSMPBaseTracking tracking = mTrackingServer.get(i);
	//		voLog.i(TAG, "Tracking server is " + tracking.getClass().getSimpleName() + ", index " + i);
			tracking.sendTrackingEvent(event);
		}
		
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	@Override
	public VO_OSMP_RETURN_CODE notifyPlayNewVideo() {
		// TODO Auto-generated method stub
		if (mTrackingServer == null || mTrackingServer.size() == 0)
		{
			voLog.e(TAG, "[TRACKING] Tracker Server is null or empty! Don't send event");
		}
		
		for (int i = 0; i < mTrackingServer.size(); i ++ )
		{
			VOOSMPBaseTracking tracking = mTrackingServer.get(i);
			tracking.notifyPlayNewVideo();
		}
		
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}



	
}