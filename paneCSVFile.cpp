// paneFixWave.cpp : implementation file
//

#include "stdafx.h"
#include "WaveCreator.h"
#include "paneCSVFile.h"

#include "Path.h"
#include "WaveCreatorDlg.h"


// CpaneFixWave dialog

IMPLEMENT_DYNAMIC(CpaneCSVFile, CRHGenericChildDialog)

CpaneCSVFile::CpaneCSVFile()
{
	
}

CpaneCSVFile::~CpaneCSVFile()
{
	
}

void CpaneCSVFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}


BEGIN_MESSAGE_MAP(CpaneCSVFile, CRHGenericChildDialog)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit)
	ON_BN_CLICKED(IDC_BUTTON1, OnBrowseCSV)
END_MESSAGE_MAP()

int CpaneCSVFile::CRHGetDialogID()
{
	return IDD_PANE_CSV_FILE;
}

BOOL CpaneCSVFile::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	return TRUE;
}

int CpaneCSVFile::GetTotalSeconds( int nSampleRate )
{
	return m_arrVal.size()/nSampleRate;
}


int CpaneCSVFile::GetSample( int nSecond, double * & pBuffer, int nSampleRate, int nBps )
{
	//m_arrVal.
	int nPos = nSecond*nSampleRate;
	array_double::const_iterator it = m_arrVal.begin() + nPos;
	array_double::const_iterator it2 = it + nSampleRate;
	std::copy(it, it2, pBuffer); 
	return 0;
}


void CpaneCSVFile::ParseCSV(const CString& path)
{
	CStdioFile fileInput;

	if(!fileInput.Open(path, CFile::modeRead))
		return;

	CString strLine;

	while(fileInput.ReadString(strLine))
	{
		TCHAR * pTok = (TCHAR *)(LPCTSTR)strLine;

		for(;*pTok; pTok++)
		{
			if(!_istdigit(*pTok) && *pTok != _T('-'))
				continue;

			double dVal = _tcstod(pTok, &pTok);

			m_arrVal.push_back(dVal);
		}
	}
}


void CpaneCSVFile::OnChangeEdit()
{
	CPath path;
	GetDlgItemText(IDC_EDIT1, path);

	CWaveCreatorDlg * pDlg = (CWaveCreatorDlg *)GetParent();


	if(path.Exists())
	{
		CWaitCursor cur;

		ParseCSV(path);

		pDlg->UpdateWaveCtrl();
	}
	else if(!m_arrVal.empty())
	{
		m_arrVal.clear();

		pDlg->UpdateWaveCtrl();
	}
}


void CpaneCSVFile::OnBrowseCSV()
{
	CPath path;
	CString strFilter;
	strFilter.LoadString(IDS_FILE_OPEN_CSV);

	if(path.SelFile(strFilter))
	{
		SetDlgItemText(IDC_EDIT1, path);
	}
}
// CpaneFixWave message handlers

