package com.visualon.OSMPAdTracking;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.SocketTimeoutException;
import java.net.URL;

import com.visualon.OSMPUtils.voLog;

import android.os.AsyncTask;

public class VOOSMPHTTPAsyncTask extends AsyncTask<String, Integer, String> {

	private static final String TAG = "@@@VOOSMPHTTPAsyncTask";
	
	@Override
	protected String doInBackground(String... arg0) {
		// TODO Auto-generated method stub
		String result = null;
		URL url = null;
		HttpURLConnection connection = null;
		InputStreamReader in = null;
		try {
		    url = new URL(arg0[0]);
            connection = (HttpURLConnection) url.openConnection();
            connection.setConnectTimeout(500);
            connection.setReadTimeout(500);
            in = new InputStreamReader(connection.getInputStream());
//            BufferedReader bufferedReader = new BufferedReader(in);
//            StringBuffer strBuffer = new StringBuffer();
//            String line = null;
//            while ((line = bufferedReader.readLine()) != null) {
//                strBuffer.append(line);
//        }
//            result = strBuffer.toString();
        }catch (SocketTimeoutException e)
		{
        	voLog.e(TAG, "[TRACKING], SocketTimeoutException, url is %s .", arg0[0]);
		}
		catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (connection != null) {
                connection.disconnect();
            }
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
 
        }
        
        voLog.i(TAG, "[TRACKING], url is %s, result is %s", arg0[0], result);
		return result;
	}
}
