package com.visualon.demo;



import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.os.Build.VERSION;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.SurfaceView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.visualon.OSMPHDMICheck.voOSHDMIStateCheck;
import com.visualon.OSMPHDMICheck.voOSHDMIStateCheck.onHDMIStateChangeListener;

public class VoHDMIStateCheckDemoActivity extends Activity implements
onHDMIStateChangeListener {
	private static final String TAG = "@@@HDMIStateCheckDemo";
	
	private static voOSHDMIStateCheck m_HDMIStateCheck = null;
	
	private TextView m_tvHDMIISSuppprt;
	private TextView m_tvHDMIStatus;
	private TextView m_tvHDCPISSupport;
	private TextView m_tvHDCPStatus;
	private ListView m_lstHDMIInfo;
	private SurfaceView m_svView;
	
	private static final String EXTDISP_PUBLIC_STATE = "com.motorola.intent.action.externaldisplaystate";
	private HdmiBroadcastReceiver mExtConnectionReceiver = null;
	
	class HdmiBroadcastReceiver extends BroadcastReceiver
	{

		@Override
		public void onReceive(Context context, Intent intent)
		{
	
			String action = intent.getAction();
			Log.i(TAG, "Moto receive is " + action);
			Bundle extras = (intent != null) ? intent.getExtras() : null;
	
			if (action.equals(EXTDISP_PUBLIC_STATE))
			{
				if (extras != null)
				{
					m_tvHDMIISSuppprt.setText("Moto Yes");
					m_tvHDCPISSupport.setText("Moto Yes");
					int hdmi = extras.getInt("hdmi");
					int hdcp = extras.getInt("hdcp");				
					

					
					
					if (hdmi == 1)
					{
						m_tvHDMIStatus.setText("Moto Enable");
					}else
					{
						m_tvHDMIStatus.setText("Moto Disable");
					}
					
					if (hdcp == 1)
					{
						m_tvHDCPStatus.setText("Moto Enable");
					}else
					{
						m_tvHDCPStatus.setText("Moto Disable");
					}
				}
			}
		}
	}
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        this.setTitle("HDMI and HDCP information, " 
        + Build.MANUFACTURER + ", " + Build.MODEL + ", " + Build.BOARD);
        
    	m_tvHDMIISSuppprt = (TextView)findViewById(R.id.txtHDMIIsSupport);
    	m_tvHDMIStatus = (TextView)findViewById(R.id.txtHDMIStatus);
    	m_tvHDCPISSupport = (TextView)findViewById(R.id.txtHDCPIsSupport);
    	m_tvHDCPStatus = (TextView)findViewById(R.id.txtHDCPStatus);
    	m_lstHDMIInfo = (ListView)findViewById(R.id.lstChangedInfo);
    	m_svView = (SurfaceView)findViewById(R.id.svMain);
        

        if(m_HDMIStateCheck != null){
        	m_HDMIStateCheck.restart(this);
        	return;
        }
        
		//create a instance of HdmiBroadcastReceiver
		m_HDMIStateCheck = new voOSHDMIStateCheck(this);

		//set hdmi state change event listener
		m_HDMIStateCheck.setOnHDMIStateChangeListener(this);
		
		String strIsSupported = 
			m_HDMIStateCheck.isSupported()?"supported":"unsupported";
		Toast.makeText(this, "HDMI " + strIsSupported, Toast.LENGTH_SHORT).show();

		mExtConnectionReceiver = new HdmiBroadcastReceiver();
		IntentFilter intentFilter = new IntentFilter();
		intentFilter.addAction(EXTDISP_PUBLIC_STATE);
		registerReceiver(mExtConnectionReceiver,intentFilter);
		
//		updateHDMIStatus();
    }
    
    @Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();
//		updateHDMIStatus();
	}

	@Override
    public void onDestroy(){
    	m_HDMIStateCheck.Release();
    	
    	unregisterReceiver(mExtConnectionReceiver);
    	super.onDestroy();
    }

	//override hdmi state change event interface
	@Override
	public void onHDMIStateChangeEvent(int nID, Object obj) {
		switch (nID) {
		case voOSHDMIStateCheck.HDMISTATE_INITED:
			// could use isHDMIConnected() function to get HDMI state after
			// receive this msg
			// value of obj is int value:0-disconnect;1-connect;
			boolean bIsConnected = ((Integer)obj == 1);
			Toast.makeText(this, "HDMI Inited: "+(bIsConnected?"connected":"broke"), Toast.LENGTH_SHORT).show();
			break;
		case voOSHDMIStateCheck.HDMISTATE_CONNECT:
			Toast.makeText(this, "HDMI Connected", Toast.LENGTH_SHORT).show();
			break;
		case voOSHDMIStateCheck.HDMISTATE_DISCONNECT:
			Toast.makeText(this, "HDMI broke", Toast.LENGTH_SHORT).show();
			break;
		}
		
//		updateHDMIStatus();

	}
	
//	@SuppressLint("NewApi")
	private void updateHDMIStatus()
	{
		m_tvHDMIISSuppprt.setText(m_HDMIStateCheck.isSupported()?"Yes":"No");
		
		File file = new File("/sys/class/switch/hdmi/state");
		if (file.exists() && file.isFile())
		{
			try {
				InputStream is = new FileInputStream(file);
				byte[] buffer = new byte[1];
				int count = is.read(buffer);
				String str = new String(buffer);
				if (str.compareTo("0") == 0)
				{
					m_tvHDMIStatus.setText("Disconnected");
				}else if (str.compareTo("1") == 0)
				{
					m_tvHDMIStatus.setText("Connected");
				}else
				{
					m_tvHDMIStatus.setText("N/A");
				}
				is.close();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				m_tvHDMIStatus.setText("N/A");
			}
		}else
		{
			m_tvHDMIStatus.setText("N/A");
		}
		
//		if (VERSION.SDK_INT >= 17)
//		{
//			Display display = getWindowManager().getDefaultDisplay();
//			int flag = display.getFlags();
//			Log.i(TAG, "Display getFalgs is " + flag);
//			
//			if (((flag & Display.FLAG_SUPPORTS_PROTECTED_BUFFERS) > 0) ||
//				((flag & Display.FLAG_SECURE) > 0))
//			{
//				m_tvHDCPISSupport.setText("Yes");
//				m_tvHDCPStatus.setText("Enable");
//			}else
//			{
//				m_tvHDCPISSupport.setText("No");
//				m_tvHDCPStatus.setText("Disable");
//			}
//		}else
		{
			m_tvHDCPISSupport.setText("N/A");
			m_tvHDCPStatus.setText("N/A");
		}
	}
	
	
}