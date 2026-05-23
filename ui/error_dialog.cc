// ErrorDialog.cpp : implementation file
#include "ui/error_dialog.h"

#include <afx.h>
#include <afxdd_.h>
#include <afxwin.h>
#include <windef.h>

#include <cstddef>
#include <cstdio>

#include "common/dl_base.h"
#include "common/dl_err.h"
#include "ui/Resource.h"

#if _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif  // _DEBUG
/////////////////////////////////////////////////////////////////////////////
// CErrorDialog dialog

CErrorDialog::CErrorDialog(CWnd *pParent) : CDialog(CErrorDialog::IDD, pParent) {
	//{{AFX_DATA_INIT(CErrorDialog)
	//}}AFX_DATA_INIT
	m_TitleText = "Error Dialog";
	m_MessageText = "";
}

void CErrorDialog::DoDataExchange(CDataExchange *pDX) {
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CErrorDialog)
	DDX_Control(pDX, IDC_EDIT1, m_Text);
	DDX_Control(pDX, IDC_LABEL, m_Label);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CErrorDialog, CDialog)
	//{{AFX_MSG_MAP(CErrorDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*===========================================================================
 *
 * Class CErrorDialog Event - BOOL OnInitDialog ();
 *
 *=========================================================================*/
BOOL CErrorDialog::OnInitDialog() {
	CErrorIncident *pError;
	CErrorRecord *pErrorRecord;
	TCHAR ErrorBuffer[MAX_ERROR_MESSAGESIZE + 1];
	int OutputErrors;
	int Length;

	CDialog::OnInitDialog();
	SetWindowText(m_TitleText);
	m_Label.SetWindowText(m_MessageText);
	pError = ErrorHandler.PeekError();
	OutputErrors = 0;

	/* Output all required errors */
	while (pError != NULL && OutputErrors < ErrorHandler.GetErrorCount()) {
		pErrorRecord = ErrorDatabase.Find(pError->GetCode());

		if (pErrorRecord == NULL) {
			std::snprintf(ErrorBuffer,
			              MAX_ERROR_MESSAGESIZE,
			              _T("%d) %s\r\n"),
			              OutputErrors + 1,
			              pError->GetMsg());
		} else {
			std::snprintf(ErrorBuffer,
			              MAX_ERROR_MESSAGESIZE,
			              _T("%d) %s\r\n\t%s\r\n"),
			              OutputErrors + 1,
			              pError->GetMsg(),
			              pErrorRecord->GetMsg(pError->GetSubCode()));
		}

		Length = m_Text.GetWindowTextLength();
		m_Text.SetSel(Length, Length, true);
		m_Text.ReplaceSel(ErrorBuffer, false);
		pError = pError->GetNext();
		OutputErrors++;
	}

	m_Text.SetSel(0, 0, false);
	return TRUE;
}
