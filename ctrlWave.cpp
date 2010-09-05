// ctrlWave.cpp : implementation file
//

#include "stdafx.h"
#include "WaveCreator.h"
#include "ctrlWave.h"


// CctrlWave

IMPLEMENT_DYNAMIC(CctrlWave, CWnd)

CctrlWave::CctrlWave()
{
	m_nBps = 1;
}

CctrlWave::~CctrlWave()
{
}


BEGIN_MESSAGE_MAP(CctrlWave, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CctrlWave message handlers



void CctrlWave::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CRect rc;

	GetClientRect(rc);

	dc.SelectStockObject(BLACK_BRUSH);

	dc.Rectangle(rc);
	
	CPen penGrid;
	CPen penWave;
	penGrid.CreatePen(PS_SOLID, 1, RGB(0, 128, 64));
	penWave.CreatePen(PS_SOLID, 1, RGB(0, 255, 0));

	dc.SelectObject(penGrid);

	for(int i = 10; i < rc.Width(); i += 10)
	{
		dc.MoveTo(CPoint(i, 0));
		dc.LineTo(CPoint(i, rc.bottom));		
	}

	for(int j = 10; j < rc.Height(); j+= 10)
	{
		dc.MoveTo(CPoint(0, j));
		dc.LineTo(CPoint(rc.right, j));	
	}

	int nWidth = rc.Width();
	int nHeight = rc.Height();

	dc.SelectObject(penWave);

	if(m_arrBuffer.empty() || nWidth == 0)
		return;

	int nX = 0/nWidth;

	double & dVal = m_arrBuffer[0];

	int nY;
	if(m_nBps == 16)
	{
		nY	= (int)(((-(dVal/32769.0)) + 1)*nHeight)/2;
	}
	else
	{
		nY	= (int)(nHeight - ( (dVal/255) )*nHeight);
	}
	
	int nSize = m_arrBuffer.size();

	for(int i = 1; i < nSize; i++)
	{
		dc.MoveTo(nX, nY);

		nX = (i*nWidth)/nSize;

		double & dVal = m_arrBuffer[i];
		
		if(m_nBps == 16)
		{
			nY	= (int)(((-(dVal/32769.0)) + 1)*nHeight)/2;
		}
		else
		{
			nY	= (int)(nHeight - ( (dVal/255) )*nHeight);
		}

		dc.LineTo(nX, nY);
	}
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages
}

void CctrlWave::SetBuffer( double *pBuffer, int nSize,int nBps )
{
	m_arrBuffer.resize(nSize);
	std::copy(pBuffer, pBuffer + nSize, m_arrBuffer.begin());
	m_nBps = nBps;

	Invalidate();
}

void CctrlWave::Empty()
{
	m_arrBuffer.clear();

	Invalidate();
}