/*===========================================================================
 *
 * File:    Custrichedit.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Friday, February 21, 2003
 *
 * Description
 *
 *=========================================================================*/
#ifndef __CUSTRICHEDIT_H
#define __CUSTRICHEDIT_H

#include <afx.h>
#include <afxcmn.h>
#include <atlstr.h>
#include <atltypes.h>
#include <windef.h>
#include <winuser.h>
#include <afxwin.h>  // TODO: Needs to go after winuser.h for UpdateLayeredWindow()

#include "common/dl_base.h"

/* Custom events */
#define CRE_UPDATEPOS       0x8001
#define CRE_UPDATESCROLL    0x8002
/*===========================================================================
 *
 * Begin CCustRichEdit Class Definition
 *
 *=========================================================================*/
class CCustRichEdit : public CRichEditCtrl {
  public:
	/* Construction */
	CCustRichEdit();
	virtual ~CCustRichEdit();

	/* Get the current line of text */
	const TCHAR *GetCurLineText(CString &Buffer);
	int GetCurLineCharPos();

	/* ClassWizard generated virtual function overrides */
	//{{AFX_VIRTUAL(CCustRichEdit)
  public:
	virtual bool Create(DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, UINT nID);
	//}}AFX_VIRTUAL

	/* Generated message map functions */
  protected:
	//{{AFX_MSG(CCustRichEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg bool OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();
};


//{{AFX_INSERT_LOCATION}}
//}}AFX_INSERT_LOCATION


#endif
