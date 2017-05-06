package com.visualon.OSMPAdTracking;

import android.content.Context;
import android.net.Uri;

import com.comscore.analytics.comScore;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;


public class VOOSMPComScoreTracking extends VOOSMPBaseTracking {

	private static final String TAG                          = "@@@VOOSMPComScoreTracking";
	
	private static final String COMSCORE_C1_VALUE            = "1";
	private static final String COMSCORE_C2_VALUE            = "3002231";
	private static final String COMSCORE_C3_VALUE            = "3000008";
	private static final String COMSCORE_C4_VALUE            = "3002231";
	private static final String COMSCORE_C7_VALUE            = "cbsandroidapp";
	private static final String COMSCORE_C8_VALUE            = "cbsandroidapp";
	private static final String COMSCORE_C9_VALUE            = "cbsandroidapp";
	
	private static final String COMSCORE_PUBLISHER_SECRET    = "2cb08ca4d095dd734a374dff8422c2e5";
	
	private static final String COMSCORE_HTTP_HEADER         = "http://beacon.securestudies.com/scripts/beacon.dll?";
	//C1=[value]&C2=[value]&C3=[value]&cA1=[value]&CA2=[value]&CA3=[value]
	
	private Context mContext                                 = null;
	
	public VOOSMPComScoreTracking(Context context, String RSID, String trackingServer, String partnerID, String  networkString) {
		super();
		// TODO Auto-generated constructor stub
		mContext = context;
		voLog.i(TAG, "VOOSMPComScoreTracking construct");
		
		comScore.setCustomerC2(COMSCORE_C2_VALUE);
		comScore.setPublisherSecret(COMSCORE_PUBLISHER_SECRET);
		
		comScore.setAppContext(mContext.getApplicationContext());
		
		comScore.setCustomerC2(COMSCORE_C2_VALUE);
		comScore.setPublisherSecret(COMSCORE_PUBLISHER_SECRET);
		
	
	}
	
	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#sendTrackingEvent(com.visualon.OSMPPlayer.VOOSMPTrackingEvent)
	 */
	@Override
	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event) {
		// TODO Auto-generated method stub
		super.sendTrackingEvent(event);
		
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

		StringBuffer message = new StringBuffer();
		message.append(COMSCORE_HTTP_HEADER);
		switch(event.getEventType())
		{
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART:
			{
		
				message.append("C1="  + COMSCORE_C1_VALUE);
				message.append("&C2=" + COMSCORE_C2_VALUE);
				message.append("&C3=" + COMSCORE_C3_VALUE);
				message.append("&C4=" + COMSCORE_C4_VALUE);
				message.append("&C5=" + getVideoType(event.getPeriodID()));
				message.append("&C6=" + Uri.encode(getADTitle(event.getPeriodID())));
				message.append("&C7=" + COMSCORE_C7_VALUE);
				message.append("&C8=" + COMSCORE_C8_VALUE);
				message.append("&C9=" + COMSCORE_C9_VALUE);
				if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					message.append("&C10=" + getContentSequence(period.getID()));
				}
	
				break;
			}
			default:
			{
				return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
			}
		}
		
		VOOSMPHTTPAsyncTask httpTask = new VOOSMPHTTPAsyncTask();
		httpTask.execute(message.toString());
		
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	private String getVideoType(int periodID)
	{
		int prevADPeriod = -1;// -1 for init, 0 for content, 1 for AD
		boolean preroll = false;
		boolean postroll = false;
		boolean findPeriod = false;
		VOOSMPAdPeriod adsPeriod = null;

		for (int i = 0; i < mADSInfo.getPeriodList().size(); i ++)
		{
			adsPeriod =  mADSInfo.getPeriodList().get(i);
			if (i == 0)
			{
				if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
					preroll = false;
				else
					preroll = true;
			}
			
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				preroll = false;
			}
			
			prevADPeriod = adsPeriod.getPeriodType();
			
			if (adsPeriod.getID() == periodID)
			{
				if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					if (adsPeriod.isEpisode())
						return "03";
					else
						return "02";
				}
				
				findPeriod = true;
				
				if (preroll)
					break;
				postroll = true;
			}
			
			if (findPeriod  && prevADPeriod == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				postroll = false;
		}
		
		if (findPeriod && preroll)
			return "09";
		
		if (findPeriod && postroll)
			return "10";
		
		if (findPeriod)
			return "11";
		
		return "";
	}

}
