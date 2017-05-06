package com.visualon.OSMPUtils;

import android.os.Parcel;

import com.visualon.OSMPUtils.voOSIOFailedDesc;


public class voOSIOFailedDescImpl implements voOSIOFailedDesc {
	
	private int reason;
	private String url;
	private String response;

	public voOSIOFailedDescImpl()
	{
		this.reason = -1;
		this.response = null;
		this.url = null;
	}
	
	public voOSIOFailedDescImpl(int reason, String response, String url)
	{
		this.reason = reason;
		this.response = response;
		this.url = url;
	}
	
	public boolean parse(Parcel parc)
	{
		if (parc == null)
			return false;
		
		parc.setDataPosition(0);
		reason = parc.readInt();
		response = parc.readString();
		url = parc.readString();
		parc.recycle();
		
		return true;
	}
	
	@Override
	public String getURL() {
		// TODO Auto-generated method stub
		return url;
	}

	@Override
	public VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON getReason() {
		// TODO Auto-generated method stub
		return VOOSMP_SRC_IO_DOWNLOAD_FAIL_REASON.valueOf(reason);
	}

	@Override
	public String getResponse() {
		// TODO Auto-generated method stub
		return response;
	}

}
