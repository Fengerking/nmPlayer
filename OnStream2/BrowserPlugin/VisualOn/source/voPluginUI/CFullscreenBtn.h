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

#pragma once
#include "windows.h"
#include "atlconv.h"
#include "atlimage.h"

#define FULLBTN_NORMAL_WIDTH 31
#define FULLBTN_NORMAL_OFFSET 50
#define WM_USERMOVE			 WM_USER+104

class CPlugInUIWnd;

class CFullscreenBtn
{
public:
  CFullscreenBtn(HWND hParent);
  ~CFullscreenBtn();

  void Show(bool bShow);
  void showButton(const int index, const bool bShow = true); // 1: full-button   2:  normal-button
  void SetFullScreen(bool bFullscreen);
  void SetParentObject(CPlugInUIWnd*	pParent){m_pParent = pParent;};
  HWND GetWindow();

  void UpdatePosition();

  static void InitCImage(CImage* pImage, int nID, Gdiplus::Bitmap** pBitmap = NULL, bool bComputeData = true);

public:
  static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  virtual LRESULT	OnReceiveMessage (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
  bool m_bFullscreen;
  HWND m_hWndParent;

  HWND m_hWndNormalBtn;
  HWND m_hWndFullBtn;

  CImage m_imageExit;
  CImage* m_imageEnter;

  CPlugInUIWnd *	m_pParent;

  bool CreateNormalBtnWnd();
  bool CreateFullBtnWnd();
};

