package com.visualon.OSMPAdTracking;

import java.util.HashMap;
import java.util.Hashtable;

import android.content.Context;


import com.adobe.adms.measurement.*;
import com.visualon.OSMPPlayer.VOOSMPAdInfo;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPOmnitureTracking extends VOOSMPBaseTracking {

	private static final String TAG							= "@@@VOOSMPOmnitureTracking";
	private static final String OMNITURE_SITEID_VALUE		= "cbscom:";
	private static final String OMNITUE_EDITION				= "us";
	private static final String OMNITUE_SITE_TYPE			= "native app";
	private static final String OMNITUE_PRIMARY_REPORTID	= "cbsicbsapp";
	private static final String OMNITUE_PROP9_VALUE			= "D=User-Agent";
	
	
	
	private ADMS_Measurement mMeasurement = null;
	private String mPartnerID, mNetworkString;
	private String m_eVar32, m_eVar2, m_eVar3, m_eVar5, m_prop9;
	
	private long mElapsedTime = 0;
	private boolean mSeekEnd = false;
	
	/**
	 * 
	 */
	public VOOSMPOmnitureTracking(Context context, String RSID, String trackingServer, String partnerID, String  networkString,
			String edition, String siteType, String primaryReportID, String prop9) {
		super();
		// TODO Auto-generated constructor stub
		mTrackingRSID = RSID;
		mTrackingServer = trackingServer;
		mPartnerID = partnerID;
		mNetworkString = networkString;
		
		m_eVar32 = mPartnerID + "|" + mNetworkString;
		
		if (edition != null && edition.trim().length()>0)
			m_eVar2 = edition;
		else
			m_eVar2 = OMNITUE_EDITION;
		
		if (siteType != null && siteType.trim().length()>0)
			m_eVar3 = siteType;
		else
			m_eVar3 = OMNITUE_SITE_TYPE;
		
		if (primaryReportID != null && primaryReportID.trim().length()>0)
			m_eVar5 = primaryReportID;
		else
			m_eVar5 = OMNITUE_PRIMARY_REPORTID;
		
		if (prop9 != null && prop9.trim().length()>0)
			m_prop9 = prop9;
		else
			m_prop9 = OMNITUE_PROP9_VALUE;
		
		mMeasurement = ADMS_Measurement.sharedInstance(context);
		mMeasurement.configureMeasurement(RSID, trackingServer);
		mMeasurement.setSSL(false);
		mMeasurement.setReportSuiteIDs(mTrackingRSID);
		voLog.i(TAG, "OmnitureTrackingHelper configureMeasurement, ID is %s, server is %s", mTrackingRSID, mTrackingServer);
	}

	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#setADTrackingInfo(com.visualon.OSMPPlayer.VOOSMPADSInfo)
	 */
	@Override
	public VO_OSMP_RETURN_CODE setPlaybackInfo(VOOSMPAdInfo adsInfo) {
		// TODO Auto-generated method stub
		return super.setPlaybackInfo(adsInfo);
	}




	
	
	private String getADInfo(int periodID, long playingTime)
	{
		String curAdSlotType;
		int  curAdSlotPos = 0;
		int  curAdSequenceNum = 0;
		
		int prevADPeriod = -1;// -1 for init, 0 for content, 1 for AD
		boolean preroll = false;
		boolean postroll = false;
		boolean findPeriod = false;
		VOOSMPAdPeriod adsPeriod = null;
		VOOSMPAdPeriod curPeriod = null;
		
		curAdSequenceNum = 0;
		
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
			
			if (prevADPeriod == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT
					&&  adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS && !findPeriod)
			{
				curAdSlotPos++;
				curAdSequenceNum = 0;
			}
			
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS && !findPeriod)
			{
				curAdSequenceNum++;
			}
			
			prevADPeriod = adsPeriod.getPeriodType();
			
			if (adsPeriod.getID() == periodID)
			{
				findPeriod = true;
				curPeriod = adsPeriod;
				
				if (preroll)
					break;
				postroll = true;
			}
			
			if (findPeriod  && prevADPeriod == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				postroll = false;
		}
		
		String perdStr = getADPercentage(curPeriod, playingTime);
		
		voLog.i(TAG, "[TRACKING]  periodID = " +  periodID + ", playingTime = " +  playingTime
				+ ", preroll = " + preroll + ", postroll = "  +  postroll
				+ ", curAdSlotPos = " +  curAdSlotPos + ", curAdSequenceNum = " +  curAdSequenceNum);
		if (findPeriod && preroll)
			return perdStr + ":A:pre:0:" + curAdSequenceNum;
		
		if (findPeriod && postroll)
			return perdStr + ":A:post:0:" + curAdSequenceNum;
		
		if (findPeriod)
			return perdStr + ":A:mid:" + curAdSlotPos + ":" + + curAdSequenceNum;
		
		return "";
	}
	
	private String getADPercentage(VOOSMPAdPeriod period, long playingTime)
	{
		if (playingTime == 0)
			return "0";
		
		long pos = playingTime - period.getStartTime();
		long duration = period.getEndTime() - period.getStartTime();
		
		float perc = ((float)pos)/((float)duration);
		
		voLog.i(TAG, "[TRACKING] getADPercentage pos = " + pos + ", duration = " + duration + ", perc = " + perc);
		
		if (perc < 0.03)
			return "0";
		
		if (perc < 0.28)
			return "1";
		
		if (perc < 0.53)
			return "2";
		
		if (perc < 0.78)
			return "3";
		
		return "4";
	}
	
	private String getContentPercentage(long playingTime)
	{
		if (mContentTime == 0)
			return "-1";
		
		if (playingTime == 0)
			return "0:M:0";
		
		long time = 0;
		
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (adsPeriod.getPeriodType() ==  VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				if (playingTime > adsPeriod.getEndTime())
				{
					time = time + adsPeriod.getEndTime() - adsPeriod.getStartTime();
				}else
				{
					time = time + playingTime - adsPeriod.getStartTime();
					break;
				}
			}
			
		}
		
		float perc = ((float)time)/((float)mContentTime);
		
		voLog.v(TAG, "[TRACKING], getContentPercentage , current is  " + time + " , total content is " + mContentTime
				+ " , perc is " + perc);
		
		if (perc < 0.01)
			return "0:M:0";
		
		if (perc < 0.26)
			return "1:M:0-25";
		
		if (perc < 0.51)
			return "2:M:25-50";
		
		if (perc < 0.76)
			return "3:M:50-75";
		
		return "4:M:75-100";
	}

	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#sendTrackingEvent(com.visualon.OSMPPlayer.VOOSMPTrackingEvent)
	 */
	@Override
	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event) {
		super.sendTrackingEvent(event);
		// TODO Auto-generated method stub
		
		if ((event == null || mADSInfo == null || mADSInfo.getPeriodList() == null)
				&& event.getEventType() != VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION)
		{
			voLog.e(TAG, "[TRACKING] VOOSMPTrackingEvent or ADSInfo is null, don't sendTrackingEvent");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
		}
		
		VOOSMPAdPeriod period = getADSPeriod(event.getPeriodID());
		
		if (period == null )
		{
			voLog.e(TAG, "[TRACKING] Omniture, Don't find action.mPeriodID on ADSInfo. , don't sendTrackingEvent");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
		}
		
		mElapsedTime = mElapsedTime + event.getElapsedTime();
		String eventStr = "";
		String eVar  = "";
		String title  = "";
		int trackingType = 0;
		
		switch (event.getEventType())
		{
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART:
			{
				if (isFirstContent(event.getPeriodID()) && period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT
						&& !mContentStartPlaying) 
				{
					mContentStartPlaying = true;
					trackingType = 1;
				}
				else if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
				{
					trackingType = 1;
				}else
				{
					voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE:
			{
				if ( period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
					trackingType = 2;
				else if ( period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					if (isLastContent(event.getPeriodID()))
					{
						voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, event value is " + event.getEventValue()
								+ " , Period type = " + period.getPeriodType());
						mSeekEnd = false;
						return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
					}
					
					if (mSeekEnd)
					{
						voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, event value is " + event.getEventValue()
								+ " , Period type = " + period.getPeriodType());
						mSeekEnd = false;
						return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
					}else
					{
						trackingType = 4;
						
					}
				}
				break;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_FORCESTOP:
			{
				trackingType = 2;
				break;
			} 
			case VO_OSMP_ADS_TRACKING_EVENT_SEEKS:
			{
				trackingType = 4;
				mSeekEnd = true;
				break;
			} 
			case VO_OSMP_ADS_TRACKING_EVENT_PAUSE:
			{
				if (event.getEventValue() == 1)
				{
					trackingType = 4;
				}else
				{
					voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PAUSE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE:
			{
				if ((period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
						&&(event.getEventValue() == 25 || event.getEventValue() == 50 || event.getEventValue() == 75))
				{
					trackingType = 3;
				}else
				{
					voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_PERCENTAGE:
			{
				if ((period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
						&&(event.getEventValue() == 25 || event.getEventValue() == 50 || event.getEventValue() == 75))
				{
					trackingType = 3;
				}else
				{
					voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_PERCENTAGE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_END:
			{
				trackingType = 2;
				break;
			}
			default:
			{
				voLog.w(TAG, "[TRACKING] Omniture don't support this event, event type is " + event.getEventType().name() + " , event value is " + event.getEventValue());
				return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
			}
				
		}
		
		mMeasurement.setEvar(33, "");
		mMeasurement.setEvar(34, "");
		
		if (trackingType == 1)//open/start
		{	
			title = period.getPeriodTitle();
			
			if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				eventStr = "event52,event59,event57=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar =  getContentPercentage(event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				eVar = "eVar39:" + eVar;
			}else if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
			{
				eventStr = "event60,event65,event56=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar = getADInfo(event.getPeriodID(), event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				eVar = "eVar39:" + eVar;
				
				mMeasurement.setEvar(33, period.getPeriodID());
				eVar = eVar + ", eVar33:" + period.getPeriodID();
				
				mMeasurement.setEvar(34, period.getPeriodTitle());
				eVar = eVar + ", eVar34:" + period.getPeriodTitle();
				
				title = this.getADTitle(period.getID());
			}
		
			mMeasurement.setEvar(25, title);
			eVar = eVar + ", eVar25:" + title;
			
			mMeasurement.setEvar(38, getVideoType(period.isLive()));
			eVar = eVar + ", eVar38:" + getVideoType(period.isLive());
			
			mMeasurement.setEvar(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			mMeasurement.setProp(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			eVar = eVar + ", eVar31:" + OMNITURE_SITEID_VALUE + getADCID(period.getID());
			
			mMeasurement.setEvar(32, m_eVar32);
			mMeasurement.setProp(32, m_eVar32);
			eVar = eVar + ", eVar32:" + m_eVar32;
		}else if (trackingType == 2) //stop
		{
			if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				eventStr = "event58,event59,event57=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar = getContentPercentage(event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				
				title = period.getPeriodTitle();
			}else if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
			{
				eventStr = "event61,event65,event56=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar =  getADInfo(event.getPeriodID(), event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				
				title = getADTitle(event.getPeriodID());
			}
			
			eVar = "eVar39:" + eVar;
			
			mMeasurement.setEvar(25, title);
			eVar = eVar + ", eVar25:" + title;
			
			mMeasurement.setEvar(38, getVideoType(period.isLive()));
			eVar = eVar + ", eVar38:" + getVideoType(period.isLive());
			
			mMeasurement.setEvar(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			mMeasurement.setProp(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			eVar = eVar + ", eVar31:" + OMNITURE_SITEID_VALUE + getADCID(period.getID());
			
			mMeasurement.setEvar(32, m_eVar32);
			mMeasurement.setProp(32, m_eVar32);
			eVar = eVar + ", eVar32:" + m_eVar32;
		
			
		}else if (trackingType == 3) //period percentage
		{
			
			if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{	
				if (event.getEventValue() == 25)
				{
					eventStr = "event53,event59";
				}else if (event.getEventValue() == 50)
				{
					eventStr = "event54,event59";
				}else if (event.getEventValue() == 75)
				{
					eventStr = "event55,event59";
				}
				
				eventStr = eventStr + ",event57=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar =  getContentPercentage(event.getPlayingTime());
				mMeasurement.setEvar(39,eVar);
				
				title = period.getPeriodTitle();
			}else if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
			{
				
				if (event.getEventValue() == 25)
				{
					eventStr = "event62,event65";
				}else if (event.getEventValue() == 50)
				{
					eventStr = "event63,event65";
				}else if (event.getEventValue() == 75)
				{
					eventStr = "event64,event65";
				}
				
				eventStr = eventStr + ",event56=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar = getADInfo(event.getPeriodID(), event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				
				title = getADTitle(event.getPeriodID());
			}
			
			eVar = "eVar39:" + eVar;
			
			mMeasurement.setEvar(25, title);
			eVar = eVar + ", eVar25:" + title;
			
			mMeasurement.setEvar(38, getVideoType(period.isLive()));
			eVar = eVar + ", eVar38:" + getVideoType(period.isLive());
			
			mMeasurement.setEvar(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			mMeasurement.setProp(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			eVar = eVar + ", eVar31:" + OMNITURE_SITEID_VALUE + getADCID(period.getID());
			
			mMeasurement.setEvar(32, m_eVar32);
			mMeasurement.setProp(32, m_eVar32);
			eVar = eVar + ", eVar32:" + m_eVar32;
		}else if (trackingType == 4) //puase
		{
			if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				eventStr = "event57=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar = getContentPercentage(event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				
				title = period.getPeriodTitle();
			}else if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
			{
				eventStr = "event56=" + (int)(mElapsedTime/1000);
				mMeasurement.setEvents(eventStr);
				
				eVar =  getADInfo(event.getPeriodID(), event.getPlayingTime());
				mMeasurement.setEvar(39, eVar);
				
				title = getADTitle(event.getPeriodID());
			}
			
			eVar = "eVar39:" + eVar;
			
			mMeasurement.setEvar(25, title);
			eVar = eVar + ", eVar25:" + title;
			
			mMeasurement.setEvar(38, getVideoType(period.isLive()));
			eVar = eVar + ", eVar38:" + getVideoType(period.isLive());
			
			mMeasurement.setEvar(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			mMeasurement.setProp(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
			eVar = eVar + ", eVar31:" + OMNITURE_SITEID_VALUE + getADCID(period.getID());
			
			mMeasurement.setEvar(32, m_eVar32);
			mMeasurement.setProp(32, m_eVar32);
			eVar = eVar + ", eVar32:" + m_eVar32;
		
			
		}
		
		mMeasurement.setProp(25, title);
		mMeasurement.setProp(31, OMNITURE_SITEID_VALUE + getADCID(period.getID()));
		
		mMeasurement.setEvar(2, m_eVar2);
		mMeasurement.setProp(2, m_eVar2);
		eVar = eVar + ", eVar2:" + m_eVar2;
		
		mMeasurement.setEvar(3, m_eVar3);
		mMeasurement.setProp(3, m_eVar3);
		eVar = eVar + ", eVar3:" + m_eVar3;
		
		mMeasurement.setEvar(5, m_eVar5);
		mMeasurement.setProp(5, m_eVar5);
		eVar = eVar + ", eVar5:" + m_eVar5;
		
		mMeasurement.setProp(9, m_prop9);
		eVar = eVar + ", prop9:" + m_prop9;
		
		Hashtable<String, Object> table = new Hashtable<String, Object>();
		table.put("pe", "lnk_o");
		mMeasurement.setPersistentContextData(table);
		
		mMeasurement.track();
		mElapsedTime = 0;
		voLog.i(TAG, "[TRACKING], Omniture sendEvent " + eventStr + ", " + eVar);
		
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	
}
