package com.visualon.OSMPAdTracking;


import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;

public interface VOOSMPAdTrackingServer {
	
	public enum VO_OSMP_AD_TRACKING_SERVER
	{
		VO_OSMP_AD_TRACKING_OMNITURE                  (0x00),
		VO_OSMP_AD_TRACKING_NIELSEN                   (0x01),
		VO_OSMP_AD_TRACKING_COMSCORE                  (0x02),
		VO_OSMP_AD_TRACKING_DATAWARE                  (0x03),
		VO_OSMP_AD_TRACKING_DOUBLECLICK               (0x04);
		
		private int value;
		VO_OSMP_AD_TRACKING_SERVER(int value)
        {
            this.value = value;
        }
       
        public int getValue()
        {
            return value;
        }
	}
	
	public final static String VO_OSMP_AD_PRODUCTION_SERVER 	= "Production";
	public final static String VO_OSMP_AD_DEVELOPMENT_SERVER 	= "Development";
	
	
	public VO_OSMP_RETURN_CODE addTrackingServer(VO_OSMP_AD_TRACKING_SERVER customerId, String rsid,
			String server, String partnerID, String  networkString,
			String edition, String siteType, String primaryReportID, String prop9);
	
	public VO_OSMP_RETURN_CODE notifyPlayNewVideo();

}
