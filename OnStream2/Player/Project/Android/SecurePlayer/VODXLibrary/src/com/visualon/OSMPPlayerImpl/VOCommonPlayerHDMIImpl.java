/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
 ************************************************************************/

package com.visualon.OSMPPlayerImpl;

import android.content.Context;

import com.visualon.OSMPHDMICheck.*;
import com.visualon.OSMPPlayer.VOCommonPlayerHDMI;
import com.visualon.OSMPPlayer.VOOSMPType;
import com.visualon.OSMPPlayer.VOOSMPType.VO_OSMP_RETURN_CODE;
import com.visualon.OSMPUtils.voLog;

public class VOCommonPlayerHDMIImpl implements VOCommonPlayerHDMI,
        voOSHDMIStateCheck.onHDMIStateChangeListener {

    private String TAG = "@@@VOCommonPlayerHDMIImpl";
    private static voOSHDMIStateCheck m_HDMIStateCheck = null;
    private Context mContext = null;
    private onHDMIConnectionChangeListener mListener = null;

    @Override
    public VO_OSMP_RETURN_CODE enableHDMIDetection(Context context,
            boolean value) {
        // TODO Auto-generated method stub
        mContext = context;

        if (value) {
            // create a instance of HdmiBroadcastReceiver
            m_HDMIStateCheck = new voOSHDMIStateCheck(mContext);
            m_HDMIStateCheck.setOnHDMIStateChangeListener(this);
        } else {
            m_HDMIStateCheck.Release();
            m_HDMIStateCheck = null;
        }

        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public boolean isHDMIDetectionSupported() {
        // TODO Auto-generated method stub
        if (m_HDMIStateCheck == null)
            return false;

        return m_HDMIStateCheck.isSupported();
    }

    @Override
    public VO_OSMP_HDMI_CONNECTION_STATUS getHDMIStatus() {
        // TODO Auto-generated method stub
        if (m_HDMIStateCheck == null)
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_UNKNOWN;

        if (m_HDMIStateCheck.isHDMIConnected()) {
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_CONNECT;
        } else {
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_DISCONNECT;
        }

    }

    @Override
    public VO_OSMP_RETURN_CODE setOnHDMIConnectionChangeListener(
            onHDMIConnectionChangeListener listener) {
        // TODO Auto-generated method stub
        if (m_HDMIStateCheck == null)
            return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_UNINITIALIZE;

        // set hdmi state change event listener
        mListener = listener;
        return VO_OSMP_RETURN_CODE.VO_OSMP_ERR_NONE;
    }

    @Override
    public void onHDMIStateChangeEvent(int arg0, Object arg1) {
        // TODO Auto-generated method stub
        if (mListener == null) {
            voLog.e(TAG,
                    "onHDMIConnectionChangeListener interface is Uninitialize. ");
        }

        mListener.onHDMIStateChangeEvent(getStatusEnumValue(arg0));
    }

    private VO_OSMP_HDMI_CONNECTION_STATUS getStatusEnumValue(int status) {
        switch (status) {
        case voOSHDMIStateCheck.HDMISTATE_INITED:
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_UNKNOWN;
        case voOSHDMIStateCheck.HDMISTATE_CONNECT:
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_CONNECT;
        case voOSHDMIStateCheck.HDMISTATE_DISCONNECT:
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_DISCONNECT;
        default:
            return VO_OSMP_HDMI_CONNECTION_STATUS.VO_OSMP_HDMISTATE_UNKNOWN;

        }
    }

}
