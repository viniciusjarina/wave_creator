#ifndef __PANE_CSV_FILE_H__
#define __PANE_CSV_FILE_H__


// CpaneFixWave dialog

#include "WaveCreatorAbstract.h"

class CpaneCSVFile : public CRHGenericChildDialog, public CWaveCreatorAbstract

{
	array_double m_arrVal;
	DECLARE_DYNAMIC(CpaneCSVFile)

public:
	CpaneCSVFile();   // standard constructor
	virtual ~CpaneCSVFile();

	virtual int GetTotalSeconds(int nSampleRate);
	virtual int GetSample(int nSecond, double * & pBuffer, int nSampleRate, int nBps);

	
// Dialog Data
	//enum { IDD = IDD_PANE_FIX_WAVE };
	void OnChangeEdit();
	void OnBrowseCSV();

	void ParseCSV(const CString& path);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual int CRHGetDialogID();        // added

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

#endif // __PANE_CSV_FILE_H__
