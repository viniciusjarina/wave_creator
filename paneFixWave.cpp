// paneFixWave.cpp : implementation file
//

#include "stdafx.h"
#include "WaveCreator.h"
#include "paneFixWave.h"

#include "WaveCreatorDlg.h"

// CpaneFixWave dialog

/*
*/


#define M_PI       3.14159265358979323846

#include <math.h>

static double senoide(double theta, int nBps)
{
	if(nBps == 16)
	{
		return sin(theta)*32767.0;
	}
	else
	{
		return (sin(theta) + 1)*127;
	}
}

static double triangular(double theta, int nBps)
{
	if(nBps == 16)
	{
		return (asin(sin(theta))*2.0/M_PI)*32767.0;
	}
	else
	{
		return ((asin(sin(theta))*2.0/M_PI) + 1)*127;
	}
}

static double quadrada(double theta, int nBps)
{
	if(nBps == 16)
	{
		return sin(theta) < 0? -32767.0 : 32767.0;
	}
	else
	{
		return sin(theta) < 0? 0 : 255;
	}
}

static double dente_serra(double theta, int nBps)
{
	if(nBps == 16)
	{
		return ((-M_PI/6 + theta/3 - floor(theta/3))*6.0/M_PI)*32767.0;
	}
	else
	{
		return (((-M_PI/6 + theta/3 - floor(theta/3))*6.0/M_PI) + 1)*127;
	}
}



IMPLEMENT_DYNAMIC(CpaneFixWave, CRHGenericChildDialog)

CpaneFixWave::CpaneFixWave()
{
	m_nFrequency = AfxGetApp()->GetProfileInt(_T("FixWave"), _T("Freq"), 440);
	m_nWaveType = AfxGetApp()->GetProfileInt(_T("FixWave"), _T("WaveType"), 0);
	m_nTime = AfxGetApp()->GetProfileInt(_T("FixWave"), _T("Time"), 1);
}

CpaneFixWave::~CpaneFixWave()
{
	AfxGetApp()->WriteProfileInt(_T("FixWave"), _T("Freq"), m_nFrequency);
	AfxGetApp()->WriteProfileInt(_T("FixWave"), _T("WaveType"), m_nWaveType);
	AfxGetApp()->WriteProfileInt(_T("FixWave"), _T("Time"), m_nTime);
}

void CpaneFixWave::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_nWaveType);
	DDX_Text(pDX, IDC_ED_FREQ, m_nFrequency);
	DDX_Text(pDX, IDC_ED_TIME, m_nTime);
	DDX_Control(pDX, IDC_SPIN1, m_spin1);
	DDX_Control(pDX, IDC_SPIN2, m_spin2);
}


BEGIN_MESSAGE_MAP(CpaneFixWave, CDialog)
	ON_EN_CHANGE(IDC_ED_FREQ, OnValueChange)
	ON_EN_CHANGE(IDC_ED_TIME, OnValueChange)
	ON_BN_CLICKED(IDC_RADIO1, OnValueChange)
	ON_BN_CLICKED(IDC_RADIO2, OnValueChange)
	ON_BN_CLICKED(IDC_RADIO3, OnValueChange)
	ON_BN_CLICKED(IDC_RADIO4, OnValueChange)
END_MESSAGE_MAP()

int CpaneFixWave::CRHGetDialogID()
{
	return IDD_PANE_FIX_WAVE;
}

BOOL CpaneFixWave::OnInitDialog()
{
	CRHGenericChildDialog::OnInitDialog();

	SetDlgItemInt(IDC_ED_FREQ, m_nFrequency);
	SetDlgItemInt(IDC_ED_TIME, m_nTime);

	m_spin1.SetRange(1,20000);
	m_spin2.SetRange(1,60);

	return TRUE;
}

int CpaneFixWave::GetTotalSeconds( int nSampleRate )
{
	int nSecond = GetDlgItemInt(IDC_ED_TIME);
	return nSecond;
}

int CpaneFixWave::GetSample( int nSecond, double * & pBuffer, int nSampleRate, int nBps )
{
	typedef double (*pWaveProc)(double theta, int bps);

	pWaveProc pProc;

	static pWaveProc arrProc[4] = 
	{
		senoide,
		quadrada,
		triangular,
		dente_serra
	};

	double nFreq = (double)GetDlgItemInt(IDC_ED_FREQ);

	for(int i = 0; i < nSampleRate; i++)
	{
		double tetha = (i*2.0*M_PI*nFreq)/(double)nSampleRate;

		double & sample = pBuffer[i];

		pProc = arrProc[m_nWaveType];

		sample = pProc(tetha, nBps);

	}
	return 0;
}

void CpaneFixWave::OnValueChange()
{
	CWaveCreatorDlg * pDlg = (CWaveCreatorDlg *)GetParent();

	if(IsDlgButtonChecked(IDC_RADIO1))
		m_nWaveType = 0;
	else if(IsDlgButtonChecked(IDC_RADIO2))
		m_nWaveType = 1;
	else if(IsDlgButtonChecked(IDC_RADIO3))
		m_nWaveType = 2;
	else if(IsDlgButtonChecked(IDC_RADIO4))
		m_nWaveType = 3;

	int nVal;
	nVal = GetDlgItemInt(IDC_ED_FREQ);

	if(nVal != 0)
		m_nFrequency = nVal;

	nVal = GetDlgItemInt(IDC_ED_TIME);
	if(nVal != 0)
		m_nTime = nVal;
	
	pDlg->UpdateWaveCtrl();
}
// CpaneFixWave message handlers

