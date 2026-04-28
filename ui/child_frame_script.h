/*===========================================================================
 *
 * File:    Childfrmscript.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  September 3, 2003
 *
 * Defines the child frame class for the script window.
 *
 *=========================================================================*/
#ifndef __CHILDFRMSCRIPT_H
#define __CHILDFRMSCRIPT_H

#include <afx.h>
#include <afxwin.h>
#include <atltypes.h>
#include <windef.h>
#include <winuser.h>

#include "ui/script_dlg.h"
#include "ui/script_error_view.h"
#include "ui/splitter_wnd.h"
/*===========================================================================
 *
 * Begin Class CChildFrmScript Definition
 *
 *=========================================================================*/
class CChildFrmScript : public CMDIChildWnd {
	DECLARE_DYNCREATE(CChildFrmScript);

  protected:
	CMwSplitterWnd m_wndSplitter;
	CScriptErrorView *m_pErrorView;
	CEsmScriptDlg *m_pScriptView;
	bool m_IsFakeMaximized;
	CRect m_RestoreRect;
	bool m_Created;


	/* Protected constructor used by dynamic creation */
	CChildFrmScript();

	virtual bool PreCreateWindow(CREATESTRUCT &cs);


  public:
	virtual ~CChildFrmScript();

	virtual bool OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext);
	void FakeMaximize();


	/* Generated message map functions */

	//{{AFX_MSG(CChildFrmScript)
	afx_msg LRESULT OnClearError(LPARAM lParam, WPARAM wParam);
	afx_msg LRESULT OnUpdateError(LPARAM lParam, WPARAM wParam);
	afx_msg LRESULT OnGotoError(LPARAM lParam, WPARAM wParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClose();
	afx_msg void OnMove(int cx, int cy);
	afx_msg void OnKillFocus(CWnd *pWnd);
	afx_msg void OnMDIActivate(bool bActivate, CWnd *pActivateWnd, CWnd *pDeactivateWnd);
	afx_msg void OnSysCommand(UINT nID, LPARAM Param);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();
};


#endif
