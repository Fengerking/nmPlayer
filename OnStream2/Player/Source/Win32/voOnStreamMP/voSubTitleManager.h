#pragma once
#include "voSubTitleFormatSettingImpl.h"
#include "voOnStreamType.h"
#include "voOnStreamSourceType.h"
#include "voCMutex.h"
#include <list>
# include <map>
# include <string>
#include <algorithm>
#include "voSubtitleFunc.h"
#include "SubtitleFunc.h"
#include "voSubtitleType.h"

class voSubTitleRenderBase;

#define VOOSMP_PID_CLOSED_CAPTION_NOTIFY_EVENT  0x01500101  
#define WM_SUBTITLE_SHOW (WM_USER+765)

//typedef int (* VOOSMPListener) (void * pUserData, int nID, void * pParam1, void * pParam2);
//for VOOSMPListener, pParam1 is pointer of VOOSMP_SUBTITLE_CALLBACK_FLAG, value of pParam2 depend on pParam1
typedef enum
{
    VOOSMP_FLAG_SUBTITLE_GET_CURRENT_POS				= 0X00000001,       /*!< pParam2 is a pointer of 32-integer */
    VOOSMP_FLAG_SUBTITLE_GET_SUBTITLE_INFO				= 0X00000002,       /*!< pParam2 is a pointer of voSubtitleInfo */
    VOOSMP_FLAG_SUBTITLE_GET_RUN_STATUS					= 0X00000004,       /*!< pParam2 is a pointer of VOOSMP_STATUS*/
    VOOSMP_FLAG_SUBTITLE_VR_USERCALLBACK				= 0X00000008,       /*!< */
    VOOSMP_FLAG_SUBTITLE_MAX                  = 0X7FFFFFFF        /*!< Max value definition */
}VOOSMP_SUBTITLE_CALLBACK_FLAG;

template<>
struct std::greater<pvoSubtitleInfoEntry>
{
    bool operator()( pvoSubtitleInfoEntry _X, pvoSubtitleInfoEntry _Y) const
    {
        return (_X->stSubtitleRectInfo.nRectZOrder > _Y->stSubtitleRectInfo.nRectZOrder);
    }
};

template<>
struct std::less<pvoSubtitleInfoEntry>
{
    bool operator()( pvoSubtitleInfoEntry _X, pvoSubtitleInfoEntry _Y) const
    { 
        return (_X->stSubtitleRectInfo.nRectZOrder < _Y->stSubtitleRectInfo.nRectZOrder);
    }
};

class voSubTitleManager
{
public:
	voSubTitleManager(void);
	virtual ~voSubTitleManager(void);

	/**
	 * get custom Settings interface
	 * 
	 * @return CCSettings interface to set some settings
	 */
	voSubTitleFormatSettingImpl* GetSettings();

	/**
	 * set parent window for displaying subtitle
	 * 
	 * @param rl the layout contains closed caption
	 */
	void SetParentWnd(HWND hWnd);

	/**
	 * set Aspect ratio of display region for closed caption
	 * 
	 * @param rate the Aspect ratio, for example, width is 600, height is 480, then the Aspect ratio is 600/480.0
	 */
	void SetXYRate(float rate);

	/**
	 * show closed caption or not
	 * 
	 * @param bShow true is to show, false is to hide
	 */
    void Show(bool bShow);

	/**
	 * Set display region for windowless mode
	 * 
	 * @param rectDisplay the current time
	 */
	void SetDisplayRect(VOOSMP_RECT& rectDisplay);

	/**
	 * Draw for windowless mode
	 * 
	 * @param hdc, the DC for drawing
	 */
	void Draw(HDC hdc, void* pParam);

	/**
	 * set windowless mode
	 * 
	 * @param bWindowless, true is window less
	 */
	void SetWindowless(bool bWindowless){m_bWindowless = bWindowless;};

	int SetParam(int nParamID, void* pValue);
	int Start();
	int Stop();


	/**
	 * Set callback function
	 * 
	 * @param pCallback, the Callback pointer
	 */
	void SetListenerCallback(VOOSMP_LISTENERINFO* pCallback);

public:
	/**
	 * check whether there are some data to handle
	 * 
	 * @return void
	 */
	void HandleTimerEvent(){};

	/**
	 * check the closed caption which should show at current time
	 * 
	 * @param curTime the current time
	 */
    void CheckViewShowStatus(int curTime);

	HWND GetRenderWindow();
	//HWND GetMessageWindow();

	/**
	 * set raw data into closed caption manager
	 * 
	 * @param parc the raw data from sdk engine
	 * @param nFlag, if true, it is subtitle, else it is ID3
	 * @return true when success, false when fail
	 */
	bool SetData(voSubtitleInfo* subInfo, bool nFlag);

	VOOSMP_LISTENERINFO* GetListenerInfo(){return &mCallbackFunc;};
	HWND GetWindow(){return m_hWnd;};
	float GetXyRate(){return m_xyRate;};
	voSubtitleInfo* GetSubtitleInfo(){return m_subtitleInfo;};
	bool IsWindowShow(){return m_show;};
	bool IsID3Picture(){return m_bID3Picture;};
	//bool  CheckWindowSizeOrPosition();
	voCMutex* GetMutexObject(){return &m_mutex;};
	VOOSMP_RENDER_TYPE GetTypeRender(){return m_typeRender;};
	HDC GetWindowlessDC(){return m_hWindowlessDC;};
	void EnableDraw(bool bEnable);
	int GetEnableDrawValue(){return m_bEnableDraw;};
	void SetEnableDrawValue(int nValue){m_bEnableDraw = nValue;};
	 
protected:
	//void ResizeWindow();
	virtual bool Update(bool bForce = false);
	void DeleteSubtitleInfo();
	//static	DWORD		CreateRenderProc (LPVOID pParam);
	//void CreateRender();

private:
	voSubTitleFormatSettingImpl*	settings;
	HWND							m_hWnd;
	float							m_xyRate;
	voSubtitleInfo*					m_subtitleInfo;
	bool							m_show;
	bool							m_bWindowless;
	bool							m_bID3Picture;

	//typedef int (* VOOSMPListener) (void * pUserData, int nID, void * pParam1, void * pParam2);
	//for VOOSMPListener, pParam1 is pointer of VOOSMP_SUBTITLE_CALLBACK_FLAG, value of pParam2 depend on pParam1
	VOOSMP_LISTENERINFO				mCallbackFunc;
	VOOSMP_RENDER_TYPE				m_typeRender;


private:
	voSubTitleRenderBase*			m_render;
	int								m_widthOld;
	int								m_heightOld;
	voCMutex						m_mutex;

	HDC								m_hWindowlessDC;
	VOOSMP_RECT						mrcDraw;
	//HANDLE							m_hThread;
	//DWORD							m_dwThreadID;
	int								m_bEnableDraw;

};
