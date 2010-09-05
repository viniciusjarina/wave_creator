// WaveCreatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WaveCreator.h"
#include "WaveCreatorDlg.h"
#include "Path.h"
#include "dlgOptions.h"

#include "Wave.h"
#include <math.h>
#include "afxwin.h"

#include <Shlwapi.h>



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
public:
	CHyperLink m_link;
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_LINK, m_link);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWaveCreatorDlg dialog




CWaveCreatorDlg::CWaveCreatorDlg(CWnd* pParent /*=NULL*/)
: CDialog(CWaveCreatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nCurrentSecond = 0;

	m_nBps = AfxGetApp()->GetProfileInt(_T("Settings"), _T("Bps"), 16);
	m_nSampleRate = AfxGetApp()->GetProfileInt(_T("Settings"), _T("SampleRate"), 8000);
	m_pBuffer = new double[m_nSampleRate];

	m_arrCreators[0] = &m_paneFixWave;
	m_arrCreators[1] = &m_paneCSVFile;
}

void CWaveCreatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_KIND, m_combo);
	DDX_Control(pDX, IDC_CTRL_WAVE, m_ctrlWave);
}

BEGIN_MESSAGE_MAP(CWaveCreatorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CWaveCreatorDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CWaveCreatorDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_SAVE_AS, &CWaveCreatorDlg::OnBnClickedSaveAs)
	ON_CBN_SELCHANGE(IDC_CB_KIND, &CWaveCreatorDlg::OnCbnSelchangeCbKind)
	ON_BN_CLICKED(IDC_BUTTON3, &CWaveCreatorDlg::OnBnClickedButton3)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_SAVE_AS2, &CWaveCreatorDlg::OnBnClickedSaveAs2)
	ON_BN_CLICKED(IDC_BUTTON5, &CWaveCreatorDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON4, &CWaveCreatorDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CWaveCreatorDlg message handlers

BOOL CWaveCreatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	m_paneFixWave.CRHCreateGenericChildDialog(this, IDC_STATIC_PLACE, 0, NULL);
	m_paneCSVFile.CRHCreateGenericChildDialog(this, IDC_STATIC_PLACE, 0, NULL);


	int nLastPos = AfxGetApp()->GetProfileInt(_T("Settings"), _T("LastKind"), 0);

	m_combo.SetCurSel(nLastPos);

	UpdatePanes();
	UpdateWaveCtrl();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWaveCreatorDlg::UpdatePanes()
{
	int nSelected = m_combo.GetCurSel();

	m_paneCSVFile.ShowWindow(SW_HIDE);
	m_paneFixWave.ShowWindow(SW_HIDE);

	switch(nSelected)
	{
	case 0:
		m_paneFixWave.ShowWindow(SW_SHOW);
		break;
	case 1:
		m_paneCSVFile.ShowWindow(SW_SHOW);
		break;
	}
}

void CWaveCreatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWaveCreatorDlg::OnPaint()
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
HCURSOR CWaveCreatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWaveCreatorDlg::OnBnClickedOk()
{



	// TODO: Add your control notification handler code here
}

void CWaveCreatorDlg::OnBnClickedButton2()
{
	CdlgOptions dlg(this);

	dlg.m_nSampleRate = m_nSampleRate;
	dlg.m_nBps = m_nBps;

	if(dlg.DoModal() == IDOK)
	{
		m_nSampleRate = dlg.m_nSampleRate;
		m_nBps = dlg.m_nBps;

		UpdateWaveCtrl();
	}
}


CString CWaveCreatorDlg::BrowseSaveWaveFile(const CString & strInitialFile)
{
	CString strFile;
	CString strTempFileName;
	CPath strTempDir;
	CString folder;

	strFile.Empty();

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = AfxGetInstanceHandle();

	CString strFilter(MAKEINTRESOURCE(IDS_FILE_OPEN_WAV));


	ofn.Flags = OFN_ENABLESIZING |OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.hwndOwner = AfxGetMainWnd()->m_hWnd;

	LPTSTR pch = strFilter.GetBuffer(0); // modify the buffer in place
	// MFC delimits with '|' not '\0'
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 1;

	if(!strInitialFile.IsEmpty())
	{
		strTempDir = strInitialFile;
		folder = strTempDir.GetFolder();
		if(!folder.IsEmpty())
		{
			ofn.lpstrInitialDir = folder;
		}

		strTempFileName = strInitialFile;
	}

	ofn.nMaxFile  = MAX_PATH;
	ofn.lpstrFile = strTempFileName.GetBufferSetLength(MAX_PATH);

	if(::GetSaveFileName(&ofn))
	{
		strTempFileName.ReleaseBuffer();

		strFile = strTempFileName;
	}
	return strFile;
}

CString CWaveCreatorDlg::BrowseSaveCSVFile(const CString & strInitialFile)
{
	CString strFile;
	CString strTempFileName;
	CPath strTempDir;
	CString folder;

	strFile.Empty();

	OPENFILENAME ofn;
	ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hInstance = AfxGetInstanceHandle();

	CString strFilter(MAKEINTRESOURCE(IDS_FILE_OPEN_CSV));


	ofn.Flags = OFN_ENABLESIZING |OFN_EXPLORER | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.hwndOwner = AfxGetMainWnd()->m_hWnd;

	LPTSTR pch = strFilter.GetBuffer(0); // modify the buffer in place
	// MFC delimits with '|' not '\0'
	while ((pch = _tcschr(pch, '|')) != NULL)
		*pch++ = '\0';

	ofn.lpstrFilter = strFilter;
	ofn.nFilterIndex = 1;

	if(!strInitialFile.IsEmpty())
	{
		strTempDir = strInitialFile;
		folder = strTempDir.GetFolder();
		if(!folder.IsEmpty())
		{
			ofn.lpstrInitialDir = folder;
		}

		strTempFileName = strInitialFile;
	}

	ofn.nMaxFile  = MAX_PATH;
	ofn.lpstrFile = strTempFileName.GetBufferSetLength(MAX_PATH);

	if(::GetSaveFileName(&ofn))
	{
		strTempFileName.ReleaseBuffer();

		strFile = strTempFileName;
	}
	return strFile;
}


void CWaveCreatorDlg::OnBnClickedSaveAs()
{
	CString strNewPath;

	CString strLastName = AfxGetApp()->GetProfileString(_T("Settings"), _T("LastWaveFile"), _T(""));
	if(strLastName.IsEmpty())
	{
		strLastName.LoadString(IDS_DEFAULT_NAME);
	}

	CPath strPath = BrowseSaveWaveFile(strLastName);

	if(strPath.IsEmpty())
		return;

	if(strPath.GetExtension().IsEmpty())
	{
		strPath.ChangeExtension(_T("wav"));
	}

	CWave wav;

	WAVEFORMATEX wf;
	INT ErrorCode;

	ZeroMemory(&wf, sizeof(wf));

	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels  = 1;
	wf.wBitsPerSample = m_nBps;
	wf.nSamplesPerSec = m_nSampleRate;
	wf.nBlockAlign = wf.nChannels *(wf.wBitsPerSample/8);
	wf.nAvgBytesPerSec = wf.nSamplesPerSec *wf.nBlockAlign;
	wf.cbSize = 0;

	int nSel = m_combo.GetCurSel();
	if(nSel == LB_ERR)
		return;

	CWaveCreatorAbstract * pCreator = m_arrCreators[nSel];

	int nSeconds = pCreator->GetTotalSeconds(m_nSampleRate);

	ErrorCode = wav.OutOpen( &strPath,&wf, m_nSampleRate, m_nSampleRate*nSeconds);	

	for(int j = 0; j < nSeconds; j++)
	{	
		pCreator->GetSample(j ,m_pBuffer, m_nSampleRate, m_nBps);

		ErrorCode = wav.OutWrite( m_pBuffer, m_nSampleRate ) != m_nSampleRate;
	}
	wav.OutClose();
}


CWaveCreatorDlg::~CWaveCreatorDlg()
{
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("Bps"), m_nBps);
	AfxGetApp()->WriteProfileInt(_T("Settings"), _T("SampleRate"), m_nSampleRate);

	if(m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}


void CWaveCreatorDlg::OnCbnSelchangeCbKind()
{
	UpdatePanes();
	UpdateWaveCtrl();
}


void CWaveCreatorDlg::OnBnClickedButton3()
{	
	CPath tempPath(tempFileCreate);

	CWave wav;

	WAVEFORMATEX wf;
	INT ErrorCode;

	ZeroMemory(&wf, sizeof(wf));

	wf.wFormatTag = WAVE_FORMAT_PCM;
	wf.nChannels  = 1;
	wf.wBitsPerSample = m_nBps;
	wf.nSamplesPerSec = m_nSampleRate;
	wf.nBlockAlign = wf.nChannels *(wf.wBitsPerSample/8);
	wf.nAvgBytesPerSec = wf.nSamplesPerSec *wf.nBlockAlign;
	wf.cbSize = 0;

	int nSel = m_combo.GetCurSel();
	if(nSel == LB_ERR)
		return;

	CWaveCreatorAbstract * pCreator = m_arrCreators[nSel];

	int nSeconds = pCreator->GetTotalSeconds(m_nSampleRate);

	ErrorCode = wav.OutOpen( &tempPath,&wf, m_nSampleRate, m_nSampleRate*nSeconds);	

	for(int j = 0; j < nSeconds; j++)
	{	
		pCreator->GetSample(j ,m_pBuffer, m_nSampleRate, m_nBps);

		ErrorCode = wav.OutWrite( m_pBuffer, m_nSampleRate ) != m_nSampleRate;
	}
	wav.OutClose();

	::PlaySound(tempPath, NULL,SND_FILENAME| SND_SYNC);

	tempPath.DeleteFile();
}


void CWaveCreatorDlg::UpdateWaveCtrl()
{
	int nSel = m_combo.GetCurSel();
	if(nSel == LB_ERR)
		return;

	UpdateButtons();

	CWaveCreatorAbstract * pCreator = m_arrCreators[nSel];

	int nTotalSeconds = pCreator->GetTotalSeconds(m_nSampleRate);

	if(nTotalSeconds == 0)
	{
		m_ctrlWave.Empty();
		return;
	}

	pCreator->GetSample(0 ,m_pBuffer, m_nSampleRate, m_nBps);
	m_ctrlWave.SetBuffer(m_pBuffer, m_nSampleRate, m_nBps);
}
void CWaveCreatorDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
}

void CWaveCreatorDlg::OnBnClickedSaveAs2()
{
	CString strNewPath;

	CString strLastName = AfxGetApp()->GetProfileString(_T("Settings"), _T("LastCSVFile"), _T(""));
	if(strLastName.IsEmpty())
	{
		strLastName.LoadString(IDS_DEFAULT_CSV);
	}

	CPath strPath = BrowseSaveCSVFile(strLastName);

	if(strPath.IsEmpty())
		return;

	if(strPath.GetExtension().IsEmpty())
	{
		strPath.ChangeExtension(_T("csv"));
	}

	CStdioFile fileOut;

	if(!fileOut.Open(strPath, CFile::modeCreate | CFile::modeWrite))
		return;

	int nSel = m_combo.GetCurSel();
	if(nSel == LB_ERR)
		return;

	CWaveCreatorAbstract * pCreator = m_arrCreators[nSel];

	int nSeconds = pCreator->GetTotalSeconds(m_nSampleRate);


	for(int j = 0; j < nSeconds; j++)
	{	
		for(int k = 0; k < m_nSampleRate; k++)
		{
			double & dVal = m_pBuffer[k];
			CString strVal;
			strVal.Format(_T("%g,"), dVal);
			fileOut.WriteString(strVal);
		}
	}
}



void CWaveCreatorDlg::OnBnClickedButton5()
{
	int nSel = m_combo.GetCurSel();
	if(nSel == LB_ERR)
		return;

	CWaveCreatorAbstract * pCreator = m_arrCreators[nSel];

	int nSeconds = pCreator->GetTotalSeconds(m_nSampleRate);

	if(nSeconds == m_nCurrentSecond + 1)
		return;

	m_nCurrentSecond++;

	UpdateButtons();
}


void CWaveCreatorDlg::UpdateButtons()
{	
	int nSel = m_combo.GetCurSel();
	if(nSel == LB_ERR)
		return;

	CWaveCreatorAbstract * pCreator = m_arrCreators[nSel];

	int nSeconds = pCreator->GetTotalSeconds(m_nSampleRate);

	if(nSeconds == 0)
	{
		m_nCurrentSecond = 0;
	}

	SetDlgItemInt(IDC_STATIC_START, m_nCurrentSecond);
	SetDlgItemInt(IDC_STATIC_END, m_nCurrentSecond + 1);

	GetDlgItem(IDC_BUTTON5)->EnableWindow(nSeconds && nSeconds != m_nCurrentSecond + 1);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(nSeconds && m_nCurrentSecond);
}


void CWaveCreatorDlg::OnBnClickedButton4()
{	
	if(!m_nCurrentSecond)
		return;

	m_nCurrentSecond--;

	UpdateWaveCtrl();
}
