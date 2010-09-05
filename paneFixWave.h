#ifndef __PANE_FIX_WAVE_H__
#define __PANE_FIX_WAVE_H__


// CpaneFixWave dialog

#include "WaveCreatorAbstract.h"

class CpaneFixWave : public CRHGenericChildDialog  , public CWaveCreatorAbstract

{
	DECLARE_DYNAMIC(CpaneFixWave)

public:
	CpaneFixWave();   // standard constructor
	virtual ~CpaneFixWave();

	virtual int GetTotalSeconds(int nSampleRate);
	virtual int GetSample(int nSecond, double * & pBuffer, int nSampleRate, int nBps);


// Dialog Data
	//enum { IDD = IDD_PANE_FIX_WAVE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual int CRHGetDialogID();        // added

	virtual BOOL OnInitDialog();

	void OnValueChange();

	int m_nWaveType;
	int m_nTime;
	int m_nFrequency;

	CSpinButtonCtrl m_spin1;
	CSpinButtonCtrl m_spin2;


	DECLARE_MESSAGE_MAP()
};

#endif // __PANE_FIX_WAVE_H__
