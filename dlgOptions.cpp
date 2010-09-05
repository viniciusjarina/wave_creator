// dlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "WaveCreator.h"
#include "dlgOptions.h"


// CdlgOptions dialog

IMPLEMENT_DYNAMIC(CdlgOptions, CDialog)

CdlgOptions::CdlgOptions(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgOptions::IDD, pParent)
	, m_nSampleRate(0)
{
	m_nBps = 16;
	m_nSampleRate = 8000;
}

CdlgOptions::~CdlgOptions()
{
}

void CdlgOptions::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cbBps);
	DDX_Text(pDX, IDC_EDIT1, m_nSampleRate);
}


BEGIN_MESSAGE_MAP(CdlgOptions, CDialog)
END_MESSAGE_MAP()


// CdlgOptions message handlers

BOOL CdlgOptions::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(m_nBps == 8)
	{
		m_cbBps.SetCurSel(0);
	}
	else
	{
		m_cbBps.SetCurSel(1);
	}

	
	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CdlgOptions::OnOK()
{
	if(m_cbBps.GetCurSel() == 0)
		m_nBps = 8;
	else
		m_nBps = 16;

	CDialog::OnOK();
}
