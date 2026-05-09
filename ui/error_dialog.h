/*===========================================================================
 *
 * File:    Errordialog.H
 * Author:  Dave Humphrey (uesp@sympatico.ca)
 * Created On:  November 21, 2006
 *
 * Description
 *
 *=========================================================================*/
#ifndef __ERRORDIALOG_H
#define __ERRORDIALOG_H

#include <afx.h>
#include <atlstr.h>
#include <windef.h>
#include <winuser.h>
#include <afxwin.h>  // TODO: Needs to go after winuser.h for UpdateLayeredWindow()

#include <cstddef>

#include "ui/Resource.h"

class CErrorDialog : public CDialog {
  public:
	CErrorDialog(CWnd *pParent = NULL);

	CString m_MessageText;
	CString m_TitleText;


	//{{AFX_DATA(CErrorDialog)
	enum {
		IDD = IDD_ERROR_DLG
	};

	CEdit m_Text;
	CStatic m_Label;
	//}}AFX_DATA


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CErrorDialog)

  protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	//}}AFX_VIRTUAL


  protected:
	/* Generated message map functions */

	//{{AFX_MSG(CErrorDialog)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();
};


//{{AFX_INSERT_LOCATION}}
//}}AFX_INSERT_LOCATION


#endif
