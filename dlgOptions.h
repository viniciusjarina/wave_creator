#pragma once
#include "afxwin.h"


// CdlgOptions dialog

class CdlgOptions : public CDialog
{
	DECLARE_DYNAMIC(CdlgOptions)

public:
	CdlgOptions(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgOptions();

// Dialog Data
	enum { IDD = IDD_OPTIONS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CComboBox m_cbBps;

	DECLARE_MESSAGE_MAP()
public:
	int m_nSampleRate;
	int m_nBps;
	
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
