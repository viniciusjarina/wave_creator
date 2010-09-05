// GenericChildDialog.cpp : implementation file
//

// From: www.codeproject.com
// Date: 19/02/2008
// Author: Chris Hills (Gold. Member No. 533206)

#include "stdafx.h"
#include "CRHGenericChildDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

// CRect( int l, int t, int r, int b );
CRect CRHGenericChildDialog::CRHRectForGroupBox(4,15,4,6);

/////////////////////////////////////////////////////////////////////////////
// CRHGenericChildDialog dialog

#if 0
                CRHGenericChildDialog::CRHGenericChildDialog(CWnd* pParent /*=NULL*/)
	                : CDialog(CRHGenericChildDialog::IDD, pParent)
                {
	                //{{AFX_DATA_INIT(CRHGenericChildDialog)
		                // NOTE: the ClassWizard will add member initialization here
					m_nPlaceMarkerCtrlID = 0;
	                //}}AFX_DATA_INIT
                }
#endif

#if 0
                CRHGenericChildDialog::CRHGenericChildDialog(UINT nIDTemplate, CWnd* pParent /*= NULL*/)
                    : CDialog(nIDTemplate, pParent)
                {
					m_nPlaceMarkerCtrlID = 0;
                }
#endif

CRHGenericChildDialog::CRHGenericChildDialog()
{
	m_nPlaceMarkerCtrlID = 0;
}

void CRHGenericChildDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRHGenericChildDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRHGenericChildDialog, CDialog)
	//{{AFX_MSG_MAP(CRHGenericChildDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRHGenericChildDialog message handlers


void CRHGenericChildDialog::CRHCreateGenericChildDialog(CWnd *pParent, 
                                                        int PlaceMarkerCtrlID, 
                                                        int Id, 
                                                        CRect *ABorderRect /*= NULL*/)
{
    CRHpParent = pParent;
    CRHId = Id;

    // Create() the child dialog ...
    VERIFY(Create( CRHGetDialogID(), pParent ));
    // ModifyStyle() here to make the child dialog a Child just didn't work, the window wouldn't appear
    //SetParent(pParent); - not necessary, Create() above did it

    // use no borders if caller hasn't given any ...
    if (ABorderRect == NULL)
    {
        static CRect NoBordersRect(0,0,0,0);
        ABorderRect = &NoBordersRect;
    }

	m_nPlaceMarkerCtrlID = PlaceMarkerCtrlID;
	m_ABorderRect = *ABorderRect;

	RecalcSize();
    //ShowWindow( TRUE ); - not needed if child dialog is marked More Styles: Visible
}

void CRHGenericChildDialog::CRHCreateGenericChildDialog(CWnd *pParent, 
                                                        CRect *pPlaceMarkerRect, 
                                                        int Id, 
                                                        CRect *ABorderRect /*= NULL*/)
{
    CRHpParent = pParent;
    CRHId = Id;

    // Create() the child dialog ...
    VERIFY(Create( CRHGetDialogID(), pParent ));
    // ModifyStyle() here to make the child dialog a Child just didn't work, the window wouldn't appear
    //SetParent(pParent); - not necessary, Create() above did it

    // use no borders if caller hasn't given any ...
    if (ABorderRect == NULL)
    {
        static CRect NoBordersRect(0,0,0,0);
        ABorderRect = &NoBordersRect;
    }
   
    // get size of child dialog ...
    CRect ChildDBRect;
    GetClientRect(&ChildDBRect);

    // move child dialog to place indicated by the place marker control ...
    MoveWindow(pPlaceMarkerRect->left + ABorderRect->left, pPlaceMarkerRect->top + ABorderRect->top, 
               ChildDBRect.right, ChildDBRect.bottom);


    //ShowWindow( TRUE ); - not needed if child dialog is marked More Styles: Visible
}

void CRHGenericChildDialog::CRHPostMessageToParent(UINT message, LPARAM lParam)
{
    CRHpParent->PostMessage(message, CRHId, lParam);
}

void CRHGenericChildDialog::RecalcSize()
{
	CWnd *pParent = CRHpParent;

	if(m_nPlaceMarkerCtrlID == 0)
		return;

		// get position of place marker control ...
	CRect PlaceMarkerRect;
	CWnd *pPlaceMarkerWnd = pParent->GetDlgItem( m_nPlaceMarkerCtrlID );
	pPlaceMarkerWnd->GetWindowRect(&PlaceMarkerRect);
	pParent->ScreenToClient(&PlaceMarkerRect);

	// get size of child dialog ...
	CRect ChildDBRect;
	GetClientRect(&ChildDBRect);

	// move child dialog to place indicated by the place marker control ...
	MoveWindow(PlaceMarkerRect.left + m_ABorderRect.left, PlaceMarkerRect.top + m_ABorderRect.top, 
		PlaceMarkerRect.Width(), PlaceMarkerRect.Height());

	// adjust size of place marker control to match child dialog ...
	int cx = PlaceMarkerRect.right  + m_ABorderRect.left + m_ABorderRect.right;
	int cy = PlaceMarkerRect.bottom + m_ABorderRect.top  + m_ABorderRect.bottom;
	VERIFY(pPlaceMarkerWnd->SetWindowPos(NULL, 
		0,0, 
		cx,cy, 
		SWP_NOMOVE | SWP_NOZORDER));

	// put child dialog on top of the place marker control ...
	VERIFY(SetWindowPos(pPlaceMarkerWnd, 
		0,0, 
		0,0, 
		SWP_NOMOVE | SWP_NOSIZE));
}