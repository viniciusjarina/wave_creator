// WaveCreatorDlg.h : header file
//

#ifndef __WAVECREATORDLG__
#define __WAVECREATORDLG__


#include "afxwin.h"

#include "paneFixWave.h"
#include "paneCSVFile.h"

#include "ctrlwave.h"

class CWaveCreatorAbstract;

// CWaveCreatorDlg dialog
class CWaveCreatorDlg : public CDialog
{
// Construction
public:
	CWaveCreatorDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CWaveCreatorDlg();

// Dialog Data
	enum { IDD = IDD_WAVECREATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	double* m_pBuffer;
	int		m_nBps;
	int     m_nSampleRate;
	int		m_nCurrentSecond;
// Implementation
protected:
	HICON m_hIcon;

	CString BrowseSaveWaveFile(const CString & strInitialFile);
	CString BrowseSaveCSVFile(const CString & strInitialFile);
	void UpdatePanes();
	void UpdateButtons();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	void UpdateWaveCtrl();

	CComboBox m_combo;
	CpaneFixWave m_paneFixWave;
	CpaneCSVFile m_paneCSVFile;

	CWaveCreatorAbstract * m_arrCreators[2];

	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedSaveAs();

	CctrlWave m_ctrlWave;
	afx_msg void OnCbnSelchangeCbKind();

	afx_msg void OnBnClickedButton3();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedSaveAs2();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
};


#endif //__WAVECREATORDLG__