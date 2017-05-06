package com.visualon.OSMPAdTracking;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.content.Context;
import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;

import com.adobe.adms.measurement.ADMS_Measurement;
import com.visualon.OSMPPlayer.VOOSMPAdInfo;
import com.visualon.OSMPPlayer.VOOSMPAdPeriod;
import com.visualon.OSMPPlayer.VOOSMPTrackingEvent;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_ADS_TRACKING_EVENT_TYPE;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPDWTracking extends VOOSMPBaseTracking {
	
	private static final String TAG                = "@@@VOOSMPDWTracking";
	
	private static final String DW_HTTPHeader      = "http://dw.cbsi.com/levt/video/e.gif?";
	private static final String DW_ADASTID         = "&adastid=";
	private static final String DW_ADBREAK         = "&adbreak=";
	private static final String DW_ADID            = "&adid=";
	private static final String DW_ADLENGTH        = "&adlength=";
	private static final String DW_ADNUM           = "&adnum=";
	private static final String DW_ADPOD           = "&adpod=";
	private static final String DW_ADPODPOS        = "&adpodpos=";
	private static final String DW_ADPOS           = "&adpos=";
	private static final String DW_ADTIME          = "&adtime=";
	private static final String DW_ADTITLE         = "&adtitle=";
	private static final String DW_ADTYPE          = "&adtype=";
	private static final String DW_BITRATE         = "&bitrate=";
	private static final String DW_BLOCKCNTRY      = "&blockcntry=";
	private static final String DW_CODEC           = "&codec=";
	private static final String DW_COMPONENTID     = "&componentid=";
	private static final String DW_DEVICE          = "&device=";
	private static final String DW_DISTNTWRK       = "&distntwrk=";
	private static final String DW_ENCODEPRFL      = "&encodeprfl=";
	private static final String DW_EVENT           = "&event=";
	private static final String DW_EVENTDUR        = "&eventdur=";
	private static final String DW_GESTVAL         = "&gestval=";
	private static final String DW_MAPP            = "&mapp=";
	private static final String DW_MEDASTID        = "&medastid=";
	private static final String DW_MEDID           = "&medid=";
	private static final String DW_MEDLENGTH       = "&medlength=";
	private static final String DW_MEDNUM          = "&mednum=";
	private static final String DW_MEDRLS          = "&medrls=";
	private static final String DW_MEDTIME         = "&medtime=";
	private static final String DW_MEDTITLE        = "&medtitle=";
	private static final String DW_MEDTYPE         = "&medtype=";
	private static final String DW_MSO             = "&mso=";
	private static final String DW_PAGEURL         = "&pageurl=";
	private static final String DW_PART            = "&part=";
	private static final String DW_PLAYEREMBED     = "&playerembed=";
	private static final String DW_PLAYERLOC       = "&playerloc=";
	private static final String DW_PLAYERSZ        = "&playersz=";
	private static final String DW_PLAYERTIME      = "&playertime=";
	private static final String DW_QOSNUM          = "&qosnum=";
	private static final String DW_RECOMMEND       = "&recommend=";
	private static final String DW_RELSESSID       = "&relsessid=";
	private static final String DW_SDLVRYTYPE      = "&sdlvrytype=";
	private static final String DW_SITEID          = "&siteid=";
	private static final String DW_SPONSORED       = "&sponsored=";
	private static final String DW_SRCHOST         = "&srchost=";
	private static final String DW_SUBJ            = "&subj=";
	private static final String DW_TS              = "ts=";
	private static final String DW_USRBNDWDTH      = "&usrbndwdth=";
	private static final String DW_VIDEOSZ         = "&videosz=";
	private static final String DW_VOLOME          = "&volume=";
	
	private static final String DW_SITEID_VALUE    = "244";
	private static final String DW_ADASTID_VALUE   = "43";
	
	private Context mContext                       = null;
	private long mPauseTime                        = 0;
	private long mFullScreenTime                   = 0;
	private long mClosedCaptionTime                = 0;
	private boolean mADSStart                      = false;
	private boolean mContentStart                  = false;
	private String mPartner                        = "cbs";
	private String DW_COMPONENTID_VALUE;
	
	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#notifyPlayNewVideo()
	 */
	@Override
	public VO_OSMP_RETURN_CODE notifyPlayNewVideo() {
		// TODO Auto-generated method stub
		super.notifyPlayNewVideo();
		mADSStart = false;
		mContentStart = false;
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	public VOOSMPDWTracking(Context context, String RSID, String trackingServer, String partnerID, String  networkString) {
		super();
		// TODO Auto-generated constructor stub
		mContext = context;
		DW_COMPONENTID_VALUE = java.util.UUID.randomUUID().toString();
		mTrackingServer = trackingServer;
		
		if ((partnerID != null) && (partnerID.trim().length() > 0))
		{
			mPartner = partnerID;
		}
		voLog.i(TAG, "VOOSMPDWTracking construct mTrackingServer is" + mTrackingServer );
	}

	/* (non-Javadoc)
	 * @see com.visualon.omsptracking.VOOSMPBaseTracking#setPlaybackInfo(com.visualon.OSMPPlayer.VOOSMPADSInfo)
	 */
	@Override
	public VO_OSMP_RETURN_CODE setPlaybackInfo(VOOSMPAdInfo adsInfo) {
		// TODO Auto-generated method stub
		return super.setPlaybackInfo(adsInfo);
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
		
		if (period == null && event.getEventType() != VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION)
		{
			voLog.e(TAG, "[TRACKING] Don't find action.mPeriodID on ADSInfo. , don't sendSegmentEvent");
			return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNKNOWN;
		}
		
		StringBuffer buffer = new StringBuffer();
		buffer.append(this.DW_HTTPHeader);
		
		SimpleDateFormat sdf = new SimpleDateFormat("yyyy.MM.dd.HH.mm.ss");
		buffer.append(DW_TS + sdf.format(new Date(System.currentTimeMillis())));
		buffer.append(DW_SITEID + DW_SITEID_VALUE);
		buffer.append(DW_SRCHOST + "");	//APP input the value
		buffer.append(DW_COMPONENTID + DW_COMPONENTID_VALUE);
		buffer.append(DW_PLAYERSZ + "640*360");
		
		if (event.getEventType() == VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION)
		{
			buffer.append(DW_PLAYERTIME + 0);
			mTime = System.currentTimeMillis();
		}else
		{
			buffer.append(DW_PLAYERTIME + (int)((System.currentTimeMillis() - mTime)/1000));
		}
		
		switch (event.getEventType())
		{
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYER_INITIALIZATION:
			{
				buffer.append(DW_EVENT + "init");
				buffer.append(DW_MAPP + "OSMP;3,7,9,B53722");
			//	buffer.append(DW_USRBNDWDTH + "");
				buffer.append(DW_PART + mPartner);
				sendHTTPRequest(buffer.toString());
				return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
			}
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART:		
				if (period.getPeriodType() ==  VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT
						&& !isFirstContent(period.getID()) )
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART or VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}else
				{
					if (isFirstAD(period.getID()) && !mADSStart)
					{
						buffer.append(DW_EVENT + "start");
						mADSStart = true;
					}else if (isFirstContent(period.getID()) && !mContentStart)
					{
						buffer.append(DW_EVENT + "start");
						mContentStart = true;
					}else
					{
						buffer.append(DW_EVENT + "play");
					}
					
					if (period.getPeriodType() ==  VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
					{
						String ccURL = period.getCaptionURL();
						if (ccURL != null && ccURL.trim().length() > 0)
						{
							buffer.append(DW_GESTVAL + "caption_avaliable:1");
						}
					}
				}
				break;
			case VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE:
				if (period.getPeriodType() ==  VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT
						&& !isLastContent(period.getID()))
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKSTART or VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}else
				{
					buffer.append(DW_EVENT + "end");
					if (period.getPeriodType() ==  VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
					{
						String ccURL = period.getCaptionURL();
						if (ccURL != null && ccURL.trim().length() > 0)
						{
							buffer.append(DW_GESTVAL + "caption_avaliable:1");
						}
					}
				}
				break;
			case VO_OSMP_ADS_TRACKING_EVENT_FORCESTOP:
			{
				if (event.getEventType() == VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_PLAYBACKCOMPLETE
					||	event.getEventType() == VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_FORCESTOP)
				{
					buffer.append(DW_EVENT + "end");
				}else if (isFirstAD(period.getID()) && !mADSStart)
				{
					buffer.append(DW_EVENT + "start");
					mADSStart = true;
				}else if (isFirstContent(period.getID()) && !mContentStart)
				{
					buffer.append(DW_EVENT + "start");
					mContentStart = true;
				}else
				{
					buffer.append(DW_EVENT + "play");
				}
				
				if (period.getPeriodType() ==  VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					String ccURL = period.getCaptionURL();
					if (ccURL != null && ccURL.trim().length() > 0)
					{
						buffer.append(DW_GESTVAL + "caption_avaliable:1");
					}
				}
				
				break;
			} 
			case VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE:
			{
				if ((period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
						&&(event.getEventValue() == 25 || event.getEventValue() == 50 || event.getEventValue() == 75))
				{
					buffer.append(DW_EVENT + "play");
					buffer.append(DW_GESTVAL + "pct:" + event.getEventValue());
				}else
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_WHOLECONTENT_PERCENTAGE:
			{
				if ((period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
						&&(event.getEventValue() == 25 || event.getEventValue() == 50 || event.getEventValue() == 75))
				{
					buffer.append(DW_EVENT + "play");
					buffer.append(DW_GESTVAL + "pct:" + event.getEventValue());
				}else if ((period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
						&& (period.isEpisode()) && (event.getEventValue() == 10 || event.getEventValue() == 20 
						|| event.getEventValue() == 30|| event.getEventValue() == 40|| event.getEventValue() == 60
						|| event.getEventValue() == 70|| event.getEventValue() == 80|| event.getEventValue() == 90))
				{
					buffer.append(DW_EVENT + "play");
					buffer.append(DW_GESTVAL + "pct:" + event.getEventValue());
				}else
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PERCENTAGE, event value is " + event.getEventValue()
							+ " , Period type = " + period.getPeriodType());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_TIMEPASSED:
			{
				if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
				{
					int time = (int) (event.getEventValue()/1000);
					if (time < 61 && !period.isLive()&& (time%15==0))
					{
						buffer.append(DW_EVENT + "play");
					}else if (time < 61 && period.isLive()&& (time%5==0))
					{
						buffer.append(DW_EVENT + "play");
					}else if (time >= 61 && (time%60==0))
					{
						buffer.append(DW_EVENT + "play");
					}else
					{
						voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_TIMEPASSED, event value is " + event.getEventValue());
						return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
					}
				}else
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_TIMEPASSED, event value is " + event.getEventValue());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
					
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_PAUSE:
			{
				if (event.getEventValue() == 1)
				{
					buffer.append(DW_EVENT + "pause");
					mPauseTime = System.currentTimeMillis();
				}else if (event.getEventValue() == 0)
				{
					buffer.append(DW_EVENT + "unpause");
					long cur = System.currentTimeMillis();
					int dur = (int)((cur- mPauseTime)/1000);
					voLog.i(TAG, "Current time is " + cur + ", pause time is " + mPauseTime);
					buffer.append(DW_EVENTDUR + dur);
					mPauseTime = 0;
				}else
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PAUSE, event value is " + event.getEventValue());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_SEEKS:
			{
				long seekPos = getPlaylistTime(event.getEventValue());
				long curContent = getCurrentContentPosition(event.getPlayingTime());
				if (seekPos < event.getPlayingTime())
				{
					buffer.append(DW_EVENT + "rewind");
					buffer.append(DW_EVENTDUR + (curContent - event.getEventValue())/1000);
				}else
				{
					buffer.append(DW_EVENT + "forward");
					buffer.append(DW_EVENTDUR + (event.getEventValue() - curContent)/1000);
				}
				voLog.i(TAG, "VO_OSMP_ADS_TRACKING_EVENT_SEEKS, id is " + event.getPeriodID() + " , event value is " + event.getEventValue()
						+ " , playingTime is " + event.getPlayingTime() + "seek pos is " + seekPos
						+ " , current Content pos is " + curContent);
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN:
			{

				if (event.getEventValue() == 1)
				{
					buffer.append(DW_EVENT + "fullscreen");
					mFullScreenTime = System.currentTimeMillis();
				}else if (event.getEventValue() == 0)
				{
					buffer.append(DW_EVENT + "normalscreen");
					int dur = 0;
					if (mFullScreenTime !=0)
					{
						dur = (int)((System.currentTimeMillis() - mFullScreenTime)/1000);
					}
					buffer.append(DW_EVENTDUR + dur);
					mFullScreenTime = 0;
				}else
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYER_FULLSCREEN, event value is " + event.getEventValue());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}case VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION:
			{

				if (event.getEventValue() == 1)
				{
					buffer.append(DW_EVENT + "caption");
					buffer.append(DW_GESTVAL + "caption:on");
					mClosedCaptionTime = System.currentTimeMillis();
				}else if (event.getEventValue() == 0)
				{
					buffer.append(DW_EVENT + "caption");
					buffer.append(DW_GESTVAL + "caption:off");
					int dur = 0;
					if (mClosedCaptionTime !=0)
					{
						dur = (int)((System.currentTimeMillis() - mClosedCaptionTime)/1000);
					}
					buffer.append(DW_EVENTDUR + dur);
					mClosedCaptionTime = 0;
				}else
				{
					voLog.w(TAG, "[TRACKING] DW don't support this event, event type is VO_OSMP_ADS_TRACKING_EVENT_PLAYER_CLOSEDCAPTION, event value is " + event.getEventValue());
					return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
				}
				break;
			}
			default:
			{
				voLog.w(TAG, "[TRACKING] DW don't support this event, event type is " + event.getEventType().name() + " , event value is " + event.getEventValue());
				return  VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
			}
				
		}
		
		buffer.append(DW_MEDASTID + "595");
		buffer.append(DW_MEDID + getADCID(period.getID()));
		buffer.append(DW_MEDLENGTH + mContentTime/1000);
	
		if (event.getEventType() == VO_OSMP_ADS_TRACKING_EVENT_TYPE.VO_OSMP_ADS_TRACKING_EVENT_SEEKS)
		{
			buffer.append(DW_MEDTIME + event.getEventValue()/1000);
		}else
		{
			buffer.append(DW_MEDTIME + getCurrentContentPosition(event.getPlayingTime())/1000);
		}
		
		buffer.append(DW_MEDNUM + mPlayNo);
		buffer.append(DW_MEDTITLE + Uri.encode(getADTitle(period.getID())));
		buffer.append(DW_SDLVRYTYPE + Integer.toString(period.isLive()?2:1));
		
		buffer.append(DW_ADNUM + getADNum(period.getID()));
		buffer.append(DW_DISTNTWRK + "can");
		
		if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_NORMALCONTENT)
		{
			buffer.append("&eventcat=" + "video");
		}else if (period.getPeriodType() == VOOSMPAdPeriod.VO_ADSMANAGER_PERIODTYPE_ADS)
		{
			if (mTrackingServer.compareTo(VOOSMPAdTrackingServer.VO_OSMP_AD_DEVELOPMENT_SERVER) == 0)
			{
				buffer.append(DW_ADID + Uri.encode(period.getPeriodID()));
				buffer.append(DW_ADTITLE +  Uri.encode(period.getPeriodTitle()));
				Log.i(TAG, "[TRACKING], VO_OSMP_AD_DEVELOPMENT_SERVER is " + mTrackingServer);
			}else
			{
				buffer.append(DW_ADID + Uri.encode(period.getPeriodTitle()));
				buffer.append(DW_ADTITLE +  Uri.encode(period.getPeriodID()));
				Log.i(TAG, "[TRACKING], VO_OSMP_AD_PRODUCTION_SERVER is " + mTrackingServer);
			}
			
			buffer.append(DW_ADASTID + DW_ADASTID_VALUE);
			buffer.append(DW_ADTIME + getADTime(period.getID(), event.getPlayingTime())/1000);
			
			buffer.append(DW_ADTYPE + "1");
			buffer.append(DW_ADPOS + getADPos(period.getID(), event.getPlayingTime()));
		}
		
		sendHTTPRequest(buffer.toString());
		
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

	private VO_OSMP_RETURN_CODE sendHTTPRequest(String str)
	{
		VOOSMPHTTPAsyncTask httpTask = new VOOSMPHTTPAsyncTask();
		httpTask.execute(str);
		return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
	}

}
