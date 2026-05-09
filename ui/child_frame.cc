/*===========================================================================
 *
 * File:    ChildFrm.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Monday, 03 February, 2003
 *
 * Implementation of the CChildFrame class
 *
 *=========================================================================*/
#include "ui/child_frame.h"

#include <afx.h>
#include <atltypes.h>
#include <windef.h>
#include <winuser.h>
#include <afxwin.h>  // TODO: Needs to go after winuser.h for UpdateLayeredWindow()

#include <cstddef>

#include "common/dl_base.h"

/* Debug defines */
#if _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _DEBUG

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd);
DEFINE_FILE("ChildFrm.cpp");
/*===========================================================================
 *
 * Class CChildFrame Message Map
 *
 *=========================================================================*/
BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SIZE()
	ON_WM_MDIACTIVATE()
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*===========================================================================
 *
 * Class CChildFrame Constructor
 *
 *=========================================================================*/
CChildFrame::CChildFrame() {
	m_IsFakeMaximized = false;
}


/*===========================================================================
 *
 * Class CChildFrame Destructor
 *
 *=========================================================================*/
CChildFrame::~CChildFrame() {
}


/*===========================================================================
 *
 * Class CChildFrame Method - BOOL PreCreateWindow (cs);
 *
 *=========================================================================*/
bool CChildFrame::PreCreateWindow(CREATESTRUCT &cs) {
	cs.style &= ~WS_MAXIMIZE;

	if (!CMDIChildWnd::PreCreateWindow(cs)) {
		return false;
	}

	return true;
}


#if _DEBUG
/*===========================================================================
 *
 * CChildFrame Diagnostics
 *
 *=========================================================================*/
void CChildFrame::AssertValid() const {
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext &dc) const {
	CMDIChildWnd::Dump(dc);
}
#endif


/*===========================================================================
 *
 * Class CChildFrame Event - void OnSize (nType, cx, cy);
 *
 *=========================================================================*/
void CChildFrame::OnSize(UINT nType, int cx, int cy) {
	CMDIChildWnd::OnSize(nType, cx, cy);
	m_IsFakeMaximized = false;
}


/*===========================================================================
 *
 * Class CChildFrame Event - void OnMDIActivate (bActivate, pActivateWnd, pDeactivateWnd);
 *
 *=========================================================================*/
void CChildFrame::OnMDIActivate(bool bActivate, CWnd *pActivateWnd, CWnd *pDeactivateWnd) {
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
}


/*===========================================================================
 *
 * Class CChildFrame Method - void FakeMaximize (void);
 *
 *=========================================================================*/
void CChildFrame::FakeMaximize() {
	CRect ClientRect;
	CRect RestoreRect;

	if (m_IsFakeMaximized) {
		SetWindowPos(NULL,
		             m_RestoreRect.left,
		             m_RestoreRect.top,
		             m_RestoreRect.Width(),
		             m_RestoreRect.Height(),
		             SWP_NOZORDER);
		m_IsFakeMaximized = false;
	} else {
		if (IsIconic()) {
			ShowWindow(SW_RESTORE);
		}

		GetWindowRect(&RestoreRect);
		GetParent()->ScreenToClient(&RestoreRect);
		GetParent()->GetClientRect(&ClientRect);
		SetWindowPos(NULL, 0, 0, ClientRect.Width(), ClientRect.Height(), SWP_NOZORDER);
		m_IsFakeMaximized = true;
		m_RestoreRect = RestoreRect;
	}
}


/*===========================================================================
 *
 * Class CChildFrame Event - void OnSysCommand (nID, Param);
 *
 *=========================================================================*/
void CChildFrame::OnSysCommand(UINT nID, LPARAM Param) {
	if ((nID & 0xFFF0) == SC_MAXIMIZE) {
		FakeMaximize();
		return;
	} else if ((nID & 0xFFF0) == SC_MINIMIZE) {
		m_IsFakeMaximized = false;
	}

	CMDIChildWnd::OnSysCommand(nID, Param);
}
