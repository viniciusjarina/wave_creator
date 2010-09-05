#ifndef __CTRL_WAVE_H__
#define __CTRL_WAVE_H__


// CctrlWave



class CctrlWave : public CWnd
{
	DECLARE_DYNAMIC(CctrlWave)

	array_double m_arrBuffer;
	int  m_nBps;

public:
	CctrlWave();
	virtual ~CctrlWave();

	void SetBuffer(double *pBuffer, int nSize,int nBps);
	void Empty();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

#endif // __CTRL_WAVE_H__


