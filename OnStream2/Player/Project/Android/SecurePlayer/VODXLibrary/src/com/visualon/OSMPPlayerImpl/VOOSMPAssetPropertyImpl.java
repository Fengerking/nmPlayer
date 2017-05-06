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

import java.util.ArrayList;

import com.visualon.OSMPDataSource.voOSDataSource;
import com.visualon.OSMPPlayer.VOCommonPlayerAssetSelection.VOOSMPAssetProperty;
import com.visualon.OSMPUtils.voLog;

public class VOOSMPAssetPropertyImpl implements VOOSMPAssetProperty {

	private final static String TAG = "@@@VOOSMPAssetPropertyImpl";
	
    class AssetProperty {
        String m_strKey;
        String m_strValue;

        public AssetProperty(String key, String value) {
            m_strKey = key;
            m_strValue = value;
        }
    }

    ArrayList<AssetProperty> m_lstProperty;

    public VOOSMPAssetPropertyImpl() {
        m_lstProperty = new ArrayList<AssetProperty>();
    }
    
    public VOOSMPAssetPropertyImpl(String [] strs) {
    	m_lstProperty = new ArrayList<AssetProperty>();
    	
    	if (strs == null || strs.length == 0 || (strs.length%2)!=0)
    	{
    		voLog.e(TAG, "VOOSMPAssetProperty info is invalid.");
    		return;
    	}
    	
    	for (int i = 0; i < strs.length/2; i++)
    	{
    		m_lstProperty.add(new AssetProperty(strs[2*i], strs[2*i+1]));
    	}
    }

    public void fillAssetsProperty(voOSDataSource source, int type, int index) {
        int nCount = source.GetPropertyCount(type, index);

        String key, value;

        for (int i = 0; i < nCount; i++) {
            key = source.GetPropertyKeyName(type, index, i);
            value = source.GetPropertyValue(type, index, i);

            m_lstProperty.add(new AssetProperty(key, value));

        }
    }

    @Override
    public int getPropertyCount() {
        return m_lstProperty.size();
    }

    @Override
    public String getKey(int index) {

        if (index >= getPropertyCount() || index < 0)
            return null;

        return m_lstProperty.get(index).m_strKey;

    }

    @Override
    public Object getValue(int index) {

        if (index >= getPropertyCount() || index < 0)
            return null;

        return m_lstProperty.get(index).m_strValue;
    }

}
