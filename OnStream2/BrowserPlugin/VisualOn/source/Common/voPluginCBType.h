/************************************************************************
VisualOn Proprietary
Copyright (c) 2013, VisualOn Incorporated. All Rights Reserved

VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA

All data and information contained in or disclosed by this document are
confidential and proprietary information of VisualOn, and all rights
therein are expressly reserved. By accepting this material, the
recipient agrees that this material and the information contained
therein are held in confidence and in trust. The material may only be
used and/or disclosed as authorized in a license agreement controlling
such use and disclosure.
*********************************************************************** */


#ifndef __voPluginCBType_h__
#define __voPluginCBType_h__

#define   RGB_DEFAULT_BKG   RGB(0,0,0)

#define   VOOSMP_PLUGIN_UNIT_OSMP_PLAYER        0X2000001
#define   VOOSMP_PLUGIN_CB_EVENT_INFO           0x2000003
#define   VOOSMP_PLUGIN_ENALBE_THIS_INSTANCE    0x2000002   //notify this instance can work or not
#define   VOOSMP_PLUGIN_PLAYER_STATUS           0x2000004   //player status changed: paramters is VOOSMP_STATUS

#define		VO_OSMP_CB_FULLSCREEN_INDICATOR          0x00001022

#define 	VO_OSMP_CB_WINDOWED_UI_COMMAND           0x01000001
#define		VOUI_CMD_PLAY			                 0X01	// the pValue1, pValue2 is NULL
#define		VOUI_CMD_PAUSE			                 0X02	// the pValue1, pValue2 is NULL
#define		VOUI_CMD_STOP			                 0X03	// the pValue1, pValue2 is NULL
#define		VOUI_CMD_FULLSCREEN		                 0X04	// the pValue1, pValue2 is NULL
#define		VOUI_CMD_SETPOS			                 0X05	// the pValue1 is int * (ms) pValue2 is NULL
#define		VOUI_CMD_SETVOLUME		                 0X06	// the pValue1 is int * (0 - 100) pValue2 is NULL

#define   PLUGIN_SETTING_INI_FILE                  _T("pluginSetting.ini")

#endif