/************************************************************************
 VisualOn Proprietary
 Copyright (c) 2012, VisualOn Incorporated. All Rights Reserved
 
VisualOn, Inc., 4675 Stevens Creek Blvd, Santa Clara, CA 95051, USA
 
All data and information contained in or disclosed by this document are
 confidential and proprietary information of VisualOn, and all rights
 therein are expressly reserved. By accepting this material, the
 recipient agrees that this material and the information contained
 therein are held in confidence and in trust. The material may only be
 used and/or disclosed as authorized in a license agreement controlling
 such use and disclosure.
 ************************************************************************/

/************************************************************************
 * @file voSubTitleManager.java
 * Manage subtitle functions.
 *
 * 
 *
 *
 * @author  Li Mingbo
 * @date    2012-2012 
 ************************************************************************/


#include "voSubTitleManager.h"
#include "voSubTitleRenderBase.h"
#include "voSubTitleWndRender.h"
#include "COverlayRender.h"
#include "voSubTitleWindowlessRender.h"
#include "vompType.h"
using namespace std;


voSubTitleManager::voSubTitleManager(void)
	//: m_hThread(NULL)
	//, m_dwThreadID(0)
{
	settings = NULL;
	m_hWnd = NULL;
	m_xyRate = 1.33f;
	m_subtitleInfo = NULL;
	m_show = true;
	m_render = NULL;
	m_widthOld = 0;
	m_heightOld = 0;
	m_bWindowless = false;
	m_hWindowlessDC = NULL;
	memset(&mCallbackFunc , 0 , sizeof(mCallbackFunc));
	memset (&mrcDraw, 0, sizeof (mrcDraw));
	m_typeRender = VOOSMP_RENDER_TYPE_DC;
	m_bID3Picture = false;
	m_bEnableDraw = 1;

}

voSubTitleManager::~voSubTitleManager(void)
{
	voCAutoLock lock(GetMutexObject());
	if(settings)
		delete settings;
	settings = NULL;
	DeleteSubtitleInfo();
	if(m_render)
		delete m_render;
	m_render = NULL;
}

voSubTitleFormatSettingImpl* voSubTitleManager::GetSettings()
{
	if(settings == NULL)
		settings = new voSubTitleFormatSettingImpl();
	return settings;
}

void voSubTitleManager::SetParentWnd(HWND hWnd)
{
	m_hWnd = hWnd;
	if(m_render)
	{
		m_render->Stop();
		delete m_render;
	}
	m_render = NULL;
	//if(m_hThread)
	//	return;

	//m_hThread = CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)CreateRenderProc, (LPVOID)this, 0, &m_dwThreadID);

	//voSubTitleWindowlessRender* render =new COverlayRender(this);//new voSubTitleWindowlessRender(this);// new voSubTitleWndRender(this);//new COverlayRender(this);//
	voSubTitleRenderBase* render = NULL;//new voSubTitleWindowlessRender(this);// new voSubTitleWndRender(this);//new COverlayRender(this);//
#ifndef _WIN32_WCE
	if(m_bWindowless)
		render =new voSubTitleWindowlessRender(this);
	else
		render =new voSubTitleWndRender(this);
#else
	render =new voSubTitleWndRender(this);
#endif
	if (render == NULL)
		return;

	(render)->CreateWnd(hWnd);
	m_render = render;
}

void voSubTitleManager::SetDisplayRect(VOOSMP_RECT& rectDisplay){
	mrcDraw = rectDisplay;
	if(m_render==NULL)
		return;
	m_render->SetDisplayRect(rectDisplay);
}

void voSubTitleManager::Draw(HDC hdc, void* pParam)
{
	if(m_render==NULL)
		return;
	voCAutoLock lock(GetMutexObject());

	m_render->Draw(hdc, pParam);
}
void voSubTitleManager::EnableDraw(bool bEnable)
{
	m_bEnableDraw = bEnable?(int)::GetTickCount():0;
}


//void voSubTitleManager::CreateRender()
//{
//	if(m_hWnd ==NULL)
//		return;
//	if(m_render)
//		delete m_render;
//	m_render = NULL;
//	voSubTitleWndRender* render = new voSubTitleWndRender(this);//new COverlayRender(this);//
//	((voSubTitleWndRender*)render)->CreateWnd(m_hWnd);
//	m_render = render;
//	m_render->Start();
//	if(m_hThread)
//		CloseHandle(m_hThread);
//	m_hThread = NULL;
//	m_dwThreadID = 0;
//}
//
//DWORD voSubTitleManager::CreateRenderProc (LPVOID pParam)
//{
//	voSubTitleManager * p = (voSubTitleManager *)pParam;
//	p->CreateRender();
//
//	return 0;
//}

HWND voSubTitleManager::GetRenderWindow()
{
	if(m_render)
		return m_render->GetRenderWindow();
	else
		return NULL;
}
//HWND voSubTitleManager::GetMessageWindow()
//{
//	if(m_render)
//		return m_render->GetMessageWindow();
//	else
//		return NULL;
//}


bool voSubTitleManager::Update(bool bForce)
{
	if(m_render)
	{
		return m_render->Update(bForce);
	}
	else
		return false;

}
int voSubTitleManager::SetParam(int nParamID, void* pValue)
{
	if(nParamID == VOOSMP_PID_LISTENER)
	{
		VOOSMP_LISTENERINFO *pFunc = (VOOSMP_LISTENERINFO *)pValue;
		mCallbackFunc.pListener = pFunc->pListener;
		mCallbackFunc.pUserData = pFunc->pUserData;

	}
	if(nParamID == VOOSMP_PID_VIDEO_DC)
	{
		//if(!m_bWindowless)
		//{
		//	m_bWindowless = true;
		//	if(m_hWnd)
		//	{
		//		this->SetParentWnd(m_hWnd);
		//		Start();
		//	}
		//}
		m_hWindowlessDC = (HDC)pValue;
	}
	if(nParamID ==  VOOSMP_PID_VIDEO_REDRAW)
	{
		if( m_typeRender != VOOSMP_RENDER_TYPE_DDRAW || IsID3Picture())
			this->Draw(m_hWindowlessDC,NULL);
	}
	if(nParamID == VOOSMP_PID_VIDEO_RENDER_TYPE)
	{
		VOOSMP_RENDER_TYPE* typeRender = (VOOSMP_RENDER_TYPE*)pValue;
		m_typeRender = *typeRender;
		if(*typeRender == VOOSMP_RENDER_TYPE_DC || m_typeRender == VOOSMP_RENDER_TYPE_DDRAW )//
		{
			m_bWindowless = true;
		}
		else
		{
			//m_bWindowless = false;
		}
		if(m_hWnd)
		{
			bool bRun = false;
			if(m_render)
			{
				if(m_render->IsRunning())
					bRun = true;
			}
			
			this->SetParentWnd(m_hWnd);
			if(bRun)
				this->Start();
		}
	}
	return VOOSMP_ERR_None;
}
void voSubTitleManager::SetListenerCallback(VOOSMP_LISTENERINFO* pCallback)
{
	SetParam(VOOSMP_PID_LISTENER, pCallback);
}
int voSubTitleManager::Start()
{
	if(m_render)
	{
		m_render->SetDisplayRect(mrcDraw);
		return m_render->Start();
	}
	return VOOSMP_ERR_None;
}
int voSubTitleManager::Stop()
{
	if(m_render)
		return m_render->Stop();
	return VOOSMP_ERR_None;
}

bool voSubTitleManager::SetData(voSubtitleInfo* subInfo, bool nFlag) 
{
	if(subInfo == NULL)
		return false;
	if(m_subtitleInfo!=NULL)
	{
		if(isequal_subtitleinfo((voSubtitleInfo*)m_subtitleInfo, subInfo))
			return false;
		DeleteSubtitleInfo();

	}
	vo_allocator< VO_BYTE > m_alloc;

	m_subtitleInfo = create_subtitleinfo(subInfo, m_alloc);

	//to sort subtitle by z-order
	list<pvoSubtitleInfoEntry> lst;
	pvoSubtitleInfoEntry pEntry = m_subtitleInfo->pSubtitleEntry;
	while(pEntry){
		lst.push_back(pEntry);
		pEntry = pEntry->pNext;
	}
	if(lst.size()>1)
	{
		lst.sort(less<pvoSubtitleInfoEntry>());
		list<pvoSubtitleInfoEntry>::iterator b = lst.begin();
		while(b!=lst.end()){
			list<pvoSubtitleInfoEntry>::iterator b2 = b;
			b++;
			if(b == lst.end())
			{
				(*b2)->pNext = NULL;
			}
			else
			{
				(*b2)->pNext = (*b);
			}
		}
		m_subtitleInfo->pSubtitleEntry = (*(lst.begin()));
		if(lst.size()>1)
		{
			int k = 0;
			k++;
		}
	}

	if(!nFlag)//we use window mode
	{
		m_bID3Picture = true;
		//if(m_bWindowless){
		//this->m_bWindowless = false;
		//SetParentWnd(this->m_hWnd);
		//if(m_render)
		//	m_render->Show(true);
		//}
	}
	else 
		m_bID3Picture = false;

	if(m_show)
		Update(true);
	return true;
}

void voSubTitleManager::Show(bool bShow)
{
	//WM_USER
	m_show = bShow;
	if(m_render)
		m_render->Show(bShow);
	if(!bShow)
	{
		DeleteSubtitleInfo();
		this->Update(true);
		//this->SetData(NULL,true);
	}


	//if(this->GetRenderWindow())
	//{
	//	::ShowWindow(GetRenderWindow(), m_show?SW_SHOW:SW_HIDE);
	//}
}
void voSubTitleManager::DeleteSubtitleInfo()
{
		if(m_subtitleInfo){
			vo_allocator< VO_BYTE > m_alloc;
			destroy_subtitleinfo(m_subtitleInfo,m_alloc);
			m_subtitleInfo = NULL;
		}
}
void voSubTitleManager::CheckViewShowStatus(int curTime)
{
	if(!m_show)
		return;

	//if(Update())
	//	return;

	if(m_subtitleInfo!=NULL)
	{
		bool bFind = false;
		if(m_subtitleInfo->nTimeStamp<=curTime)
			bFind = true;
		if(!bFind && m_subtitleInfo->nTimeStamp>=0)
		{
			DeleteSubtitleInfo();
			Update(true);
		}
		if(bFind)
		{
			if(curTime - m_subtitleInfo->nTimeStamp>15000)
			{
				DeleteSubtitleInfo();
				Update(true);
			}
			
		}
	}
}
void voSubTitleManager::SetXYRate(float rate)
{
	bool bChanged = true;
	if((m_xyRate - rate)<0.001&&(m_xyRate - rate)>-0.001)
		bChanged =false;
	m_xyRate = rate;
	if(m_render)
		m_render->ComputeValidRectOfXYRatio(bChanged);
}

