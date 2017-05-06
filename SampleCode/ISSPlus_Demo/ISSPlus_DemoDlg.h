
// ISSPlus_DemoDlg.h : header file
//

#pragma once

#include "CISSPlus.h"
#include "afxcmn.h"

// CISSPlus_DemoDlg dialog
class CISSPlus_DemoDlg : public CDialog
{
// Construction
public:
	CISSPlus_DemoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ISSPLUS_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtLoadlib();
	afx_msg void OnBnClickedBtHeaderparser();
public:
	CISSPlus m_ISSPlus;
	HTREEITEM   hItem;


	CTreeCtrl m_treectrl;
	HTREEITEM InsertItem(HTREEITEM item,  char *type, unsigned char* data);
	HTREEITEM InsertItem(HTREEITEM item, char *type, VO_U32 data);
	HTREEITEM InsertItem(HTREEITEM item, char *type, CString data);
	afx_msg void OnBnClickedBtChunkparser();
};
