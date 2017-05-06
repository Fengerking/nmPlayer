
// ISSPlus_DemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ISSPlus_Demo.h"
#include "ISSPlus_DemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CISSPlus_DemoDlg dialog




CISSPlus_DemoDlg::CISSPlus_DemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CISSPlus_DemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CISSPlus_DemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_STREAMINFO, m_treectrl);
}

BEGIN_MESSAGE_MAP(CISSPlus_DemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_BUTTON1, &CISSPlus_DemoDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_BT_LOADLIB, &CISSPlus_DemoDlg::OnBnClickedBtLoadlib)
ON_BN_CLICKED(IDC_BT_HEADERPARSER, &CISSPlus_DemoDlg::OnBnClickedBtHeaderparser)
ON_BN_CLICKED(IDC_BT_CHUNKPARSER, &CISSPlus_DemoDlg::OnBnClickedBtChunkparser)
END_MESSAGE_MAP()


// CISSPlus_DemoDlg message handlers

BOOL CISSPlus_DemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CISSPlus_DemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CISSPlus_DemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CISSPlus_DemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//void CISSPlus_DemoDlg::OnBnClickedButton1()
//{
//	// TODO: Add your control notification handler code here
//}

void CISSPlus_DemoDlg::OnBnClickedBtLoadlib()
{
	// TODO: Add your control notification handler code here
	m_ISSPlus.Open();
}
HTREEITEM CISSPlus_DemoDlg::InsertItem(HTREEITEM item, char *type, VO_U32 data)
{
	CString ss;	
	TCHAR ttype[256];
	MultiByteToWideChar (CP_UTF8, 0,  (char*)type, -1, ttype, sizeof(ttype));
	ss.Format(_T("%s: %d"), ttype,data);
	return m_treectrl.InsertItem(ss, item);
}
HTREEITEM CISSPlus_DemoDlg::InsertItem(HTREEITEM item, char *type, unsigned char* data)
{
	CString ss;	
	TCHAR tdata[256];
	TCHAR ttype[256];
	MultiByteToWideChar (CP_UTF8, 0,  (char*)type, -1, ttype, sizeof(ttype));
	MultiByteToWideChar (CP_UTF8, 0,  (char*)data, -1, tdata, sizeof(tdata));
	ss.Format(_T("%s: %s"), ttype,tdata);
	return m_treectrl.InsertItem(ss, item);
}
HTREEITEM CISSPlus_DemoDlg::InsertItem(HTREEITEM item, char *type, CString data)
{
	CString ss;	
	TCHAR ttype[256];
	MultiByteToWideChar (CP_UTF8, 0,  (char*)type, -1, ttype, sizeof(ttype));
	ss.Format(_T("%s: %s"), ttype,data);
	return m_treectrl.InsertItem(ss, item);
}
void CISSPlus_DemoDlg::OnBnClickedBtHeaderparser()
{
	// TODO: Add your control notification handler code here
	CFile ff;
	int len = 1024*1024*1;
	unsigned char *buf =new unsigned char[len];
	int size  =0;

	ff.Open(L"./testFile/Manifest",CFile::modeRead);
	size = ff.Read(buf, len);
	m_ISSPlus.HeaderParser(buf, size);
	delete []buf;
	ff.Close();
	VO_U32 nCount = 0;
	m_ISSPlus.GetStreamCount(&nCount);
	VO_LIVESRC_PLUS_STREAM_INFO *pSampleInfo = NULL;
	CString ss;	
	TCHAR tt[256];
	for(VO_U32 n =0 ; n < nCount;n++)
	{
		m_ISSPlus.GetStreamInfo(n, &pSampleInfo);
		MultiByteToWideChar (CP_UTF8, 0,  (char*)pSampleInfo->strStreamName, -1, tt, sizeof(tt));
		ss.Format(_T("Stream_%d: %s"),n, tt);
		hItem =	m_treectrl.InsertItem( ss,0,0,TVI_ROOT); 
		InsertItem(hItem, "SubType", pSampleInfo->strSubType);
		InsertItem(hItem, "BaseUrl", pSampleInfo->strBaseUrl);
		InsertItem(hItem, "Duration", pSampleInfo->nDuration);
		InsertItem(hItem, "ChunkCounts", pSampleInfo->nChunkCounts);
		InsertItem(hItem, "StreamType", pSampleInfo->nStreamType);
		InsertItem(hItem, "BandWidth", pSampleInfo->nBandWidth);
		HTREEITEM hh = InsertItem(hItem, "TrackCounts", pSampleInfo->nTrackCounts);
		for(VO_U32 x =0 ; x<pSampleInfo->nTrackCounts; x++)
		{
			VO_LIVESRC_PLUS_TRACK_INFO *info = pSampleInfo->ppTrackInfo[x];
			ss.Format(_T("QualityLevel__%d"),info->nBitRate);
			HTREEITEM hh2 =	m_treectrl.InsertItem( ss,hh); 

			InsertItem(hh2, "TrackType", info->nTrackType);
			InsertItem(hh2, "FourCC", info->strFourCC);
			InsertItem(hh2, "Codec", info->nCodec);
			char c[128];
			if(info->nTrackType == VO_SOURCE_TT_AUDIO)
			{
				m_ISSPlus.SetCodecType(VO_SOURCE_TT_AUDIO, info->nCodec);
				sprintf (c ,("Channel:%d, SampleRate:%d, SampleBits:%d"),info->stAudio_info.nChannels, info->stAudio_info.nSampleRate, info->stAudio_info.nSampleBits);
			}
			else 	if(info->nTrackType == VO_SOURCE_TT_VIDEO)
			{
				if(!m_ISSPlus.video_ff)
				{
					m_ISSPlus.video_ff = fopen("video.264","wb");
					fwrite(info->HeadData, 1, info->nHeadSize, m_ISSPlus.video_ff );
				}
				m_ISSPlus.SetCodecType(VO_SOURCE_TT_VIDEO, info->nCodec);
				sprintf (c,("Width:%d, Height:%d"),info->stVideo_info.nWidth, info->stVideo_info.nHeight);
			}
			InsertItem(hh2, "Format", (unsigned char*)c);
			InsertItem(hh2, "HeadSize", info->nHeadSize);
			unsigned char* pp = info->HeadData;
			ss ="";
			for (VO_U32 i=0 ;i< info->nHeadSize;i++)
			{
				CString sp;
				sp.Format(_T("%x"), *pp); pp++;
				ss += sp;
			}
			InsertItem(hh2, "HeadData",ss);
		}
	}

	VO_LIVESRC_PLUS_DRM_INFO  *pDRMInfo = NULL;
	m_ISSPlus.GetDrmInfo(&pDRMInfo);
	ss.Format(_T("Protection"));
	hItem =	m_treectrl.InsertItem( ss,0,0,TVI_ROOT); 
	if(pDRMInfo)
	{
		ss = "";
		for (VO_U32 i=0 ;i< 4;i++)
		{
			CString sp;
			sp.Format(_T("%x"), pDRMInfo->strSystemID[i]);
			ss += sp;
		}
		InsertItem(hItem, "SystemID", ss);
		ss = "";
		unsigned char* pp = pDRMInfo->strData;
		for (VO_U32 i=0 ;i< pDRMInfo->nDataSize;i++)
		{
			CString sp;
			sp.Format(_T("%c"), *pp); pp++;
			ss += sp;
		}
		InsertItem(hItem, "value", ss);
	}
}

void CISSPlus_DemoDlg::OnBnClickedBtChunkparser()
{
	// TODO: Add your control notification handler code here
	CFile ff;
	int len = 1024*1024*1;
	unsigned char *buf =new unsigned char[len];
	int size  =0;

	ff.Open(L"./testFile/video/Fragments(video=0).mp4",CFile::modeRead);
	size = ff.Read(buf, len);
	m_ISSPlus.ChunkParser(VO_SOURCE_TT_VIDEO, buf, size);
	ff.Close();

	memset(buf, 0x00,len);
	ff.Open(L"./testFile/video/Fragments(video=20020000).mp4",CFile::modeRead);
	size = ff.Read(buf, len);
	m_ISSPlus.ChunkParser(VO_SOURCE_TT_VIDEO, buf, size);
	ff.Close();

	ff.Open(L"./testFile/audio/Fragments(audio=0).mp4",CFile::modeRead);
	size = ff.Read(buf, len);
	m_ISSPlus.ChunkParser(VO_SOURCE_TT_AUDIO, buf, size);
	ff.Close();
	ff.Open(L"./testFile/audio/Fragments(audio=20201360).mp4",CFile::modeRead);
	size = ff.Read(buf, len);
	m_ISSPlus.ChunkParser(VO_SOURCE_TT_AUDIO, buf, size);
	ff.Close();
	delete []buf;
	
	if(m_ISSPlus.video_ff) fclose(m_ISSPlus.video_ff) ;m_ISSPlus.video_ff = NULL;
	if(m_ISSPlus.audio_ff) fclose(m_ISSPlus.audio_ff) ;m_ISSPlus.audio_ff = NULL;
}
