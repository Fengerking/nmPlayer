package com.visualon.OSMPAdTracking;


import com.visualon.OSMPPlayer.VOOSMPAdInfo;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.*;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPBaseTracking {
	
	private static final String TAG = "@@@VOOSMPBaseTracking";
	
	protected String mTrackingRSID;
	protected String mTrackingServer;
	protected VOOSMPAdInfo mADSInfo;
	protected long mPlaybackTime;
	protected long mContentTime;
	protected long mADTime;
	protected int mPlayNo;
	protected long mTime;
	protected boolean mContentStartPlaying;
	

	/**
	 * 
	 */
	public VOOSMPBaseTracking() {
		super();
		// TODO Auto-generated constructor stub
		mADSInfo = null;
		mPlaybackTime = 0;
		mContentTime = 0;
		mADTime = 0;
		mPlayNo = 0;
		mTime = 0;
		mContentStartPlaying = false;
	}

	public VO_OSMP_RETURN_CODE setPlaybackInfo(VOOSMPAdInfo adsInfo)
	{
		voLog.i(TAG, "PlayBackInfo Period start");
		mADSInfo = adsInfo;
		if (mADSInfo == null || mADSInfo.getPeriodList() == null || mADSInfo.getPeriodList().size() == 0)
		{
			voLog.e(TAG, "VOOSMPADSInfo is not available.");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_PARAMID;
		}
		
		mPlaybackTime = mADSInfo.getPeriodList().get(mADSInfo.getPeriodList().size() - 1).getEndTime();
		
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod =  mADSInfo.getPeriodList().get(i);
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				mContentTime = mContentTime + adsPeriod.getEndTime() - adsPeriod.getStartTime();
			}else if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
			{
				mADTime = mADTime + adsPeriod.getEndTime() - adsPeriod.getStartTime();
			}
			
			voLog.i(TAG, "PlayBackInfo Period, ID is " + adsPeriod.getID()
					+ ", Period EndTime is " + adsPeriod.getEndTime() + ", Period StartTime is " + adsPeriod.getStartTime()
					+ ", Period time " +  (adsPeriod.getEndTime() - adsPeriod.getStartTime())
					+ ", Period isLive is " + adsPeriod.isLive()
					+ ", PeriodID is " + adsPeriod.getPeriodID() + ", CustomerID is " + adsPeriod.getContentID()
					+ ", Period Title is " + adsPeriod.getPeriodTitle() + ", subtitle is " + adsPeriod.getCaptionURL());
		}
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	public VO_OSMP_RETURN_CODE sendTrackingEvent(VOOSMPTrackingEvent event) {
		// TODO Auto-generated method stub
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	public VO_OSMP_RETURN_CODE notifyPlayNewVideo() {
		// TODO Auto-generated method stub
		mPlayNo = mPlayNo + 1;
		mContentStartPlaying = false;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}
	
	protected VOOSMPAdPeriod getADSPeriod(int periodID)
	{
		if (mADSInfo == null || mADSInfo.getPeriodList() == null)
		{
			voLog.e(TAG, "VOOSMPADSInfo or PeriodList is null, don't getADSPeriod periodID = " + periodID);
			return null;
		}
		
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod =  mADSInfo.getPeriodList().get(i);
			if (adsPeriod.getID() == periodID)
			{
				return adsPeriod;
			}
		}
		return null;
	}
	
	protected String getVideoType(boolean isLive)
	{
		if (isLive)
			return "live video";
		else
			return "video";
	}
	
	protected String getADCID(int periodID)
	{
		String CID = null;
		boolean find = false;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (periodID == adsPeriod.getID())
				find = true;
			
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				CID = adsPeriod.getContentID();
			
			if (CID != null && find)
				return CID;
		}
		
		if (CID == null)
			return "";
		else
			return CID;
	}
	
	protected long getCurrentContentPosition(long playingTime)
	{
		long  curContentTime = 0;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (playingTime <= adsPeriod.getStartTime())
			{
				return curContentTime;
			}else if (playingTime > adsPeriod.getStartTime() && playingTime <= adsPeriod.getEndTime())
			{
				if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					curContentTime = curContentTime + playingTime - adsPeriod.getStartTime() ;
				}else
				{
					return curContentTime;
				}
			}else if (playingTime > adsPeriod.getEndTime())
			{
				if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					curContentTime = curContentTime + adsPeriod.getEndTime() - adsPeriod.getStartTime() ;
				}
			}
			
		}
		voLog.i(TAG, "[TRACKING] getCurrentContentPosition, playingTime is " + playingTime 
				+ " , curContentTime is "  + curContentTime);
		return curContentTime;
	}

	protected int getADNum(int periodID)
	{
		int  num = 0;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS )
				num++;
			
			if (periodID == adsPeriod.getID())
				return num;
		}
		return num;
	}
	
	protected String getADPos(int periodID, long playingTime)
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
				if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
					return "content";
				
				findPeriod = true;
				curPeriod = adsPeriod;
				
				if (preroll)
					break;
				postroll = true;
			}
			
			if (findPeriod  && prevADPeriod == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				postroll = false;
		}
		
		if (findPeriod && preroll)
			return "pre";
		
		if (findPeriod && postroll)
			return "post";
		
		if (findPeriod)
			return "mid";
		
		return "";
	}
	
	protected boolean isFirstAD(int periodID)
	{
		boolean b = true;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (periodID == adsPeriod.getID())
				return b;
		
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS
					&& periodID != adsPeriod.getID())
				return false;
			
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
				b = false;
			
		}
		return b;
	}
	
	protected boolean isFirstContent(int periodID)
	{	
		boolean b = true;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (periodID == adsPeriod.getID())
				return b;
		
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT
					&& periodID != adsPeriod.getID())
				return false;
			
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				b = false;
			
		}
		return b;
	}
	
	protected boolean isLastContent(int periodID)
	{
		boolean b = false;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (periodID == adsPeriod.getID() && adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				b = true;
		
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT
					&& periodID != adsPeriod.getID() && b)
				return false;
		}
		return b;
	}
	
	protected String getADTitle(int periodID)
	{
		String title = null;
		boolean find = false;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (periodID == adsPeriod.getID())
				find = true;
			
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				title = adsPeriod.getPeriodTitle();
			
			if (title != null && find)
				return title;
		}
		
		if (title == null)
			return "";
		else
			return title;
	}
	
	protected long getADTime(int periodID, long playingTime)
	{
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (periodID == adsPeriod.getID())
				return playingTime - adsPeriod.getStartTime();
		}
		return 0;
	}
	
	protected long getPlaylistTime(long contentTime)
	{
		long time = 0;
		long ctime = 0;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				if (ctime + adsPeriod.getEndTime() - adsPeriod.getStartTime() > contentTime)
					return time + contentTime - adsPeriod.getStartTime();
				else
				{
					time = time + adsPeriod.getEndTime() - adsPeriod.getStartTime();
					ctime = ctime  + adsPeriod.getEndTime() - adsPeriod.getStartTime();
				}
			}else
			{
				time = time + adsPeriod.getEndTime() - adsPeriod.getStartTime();
			}
				
		}
		return time;
	}
	
	protected String getContentSequence(int periodID)
	{
		int sequence = -1;
		int total = 0;
		for (int i = 0; i < mADSInfo.getPeriodList().size(); i++)
		{
			VOOSMPAdPeriod adsPeriod = mADSInfo.getPeriodList().get(i);
			if (adsPeriod.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
			{
				total++;
			}
			if (adsPeriod.getID() == periodID)
			{
				sequence = total;
			}
				
		}
		String str1 = "", str2 = "";
		
		if (Integer.toString(sequence).length() == 1)
			str1 = "0" + sequence;
		else
			str1 = Integer.toString(sequence);
		
		if (Integer.toString(total).length() == 1)
			str2 = "0" + total;
		else
			str2 = Integer.toString(total);
		
		

		return str1 + "-" + str2;
	}
}
