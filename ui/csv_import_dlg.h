/*===========================================================================
 *
 * File:    Esmcsvimportdlg.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  October 13, 2003
 *
 * Description
 *
 *=========================================================================*/
#ifndef __ESMCSVIMPORTDLG_H
#define __ESMCSVIMPORTDLG_H

#include <afx.h>
#include <afxcmn.h>
#include <windef.h>
#include <winuser.h>
#include <afxwin.h>  // TODO: Needs to go after winuser.h for UpdateLayeredWindow()

#include "common/dl_base.h"
#include "file/csv_file.h"
#include "mwedit/csv_defs.h"
#include "ui/mwedit_doc.h"
#include "ui/Resource.h"

/* List definitions */
#define ESMCSVIMP_LIST_COLWIDTH 80

/* Csv file definitions */
#define ESMCSVIMP_CSV_EXT     _T("csv")
#define ESMCSVIMP_CSV_FILTER  _T("CSV Files (*.csv)|*.csv|Text Files (*.txt)|*.txt|All Files (*.*)|*.*||")
/*===========================================================================
 *
 * Begin Class CEsmCsvImportDlg Definition
 *
 *=========================================================================*/
class CEsmCsvImportDlg : public CDialog {
  protected:
	CCsvFile m_CsvFile;      /* Csv file object */
	CMWEditDoc *m_pDocument; /* Reference to parent document */
	int m_CreatedRecords;
	int m_IDColIndex;        /* Special column indices */
	int m_TypeColIndex;


  public:
	/* Protected constructor used by dynamic creation */
	CEsmCsvImportDlg(CWnd *pParent);
	virtual ~CEsmCsvImportDlg();

	/* Checks the validity of the current data */
	bool CheckData();
	bool CheckRow(const int RowIndex);

	/* Helper methods to create new records */
	bool CreateRecords();
	bool CreateRecord(CCsvRow *pRow);

	/* Update the list control */
	void ClearCsvList();
	void FillCsvList();

	/* Look for special column names */
	bool FindColumns();

	/* Prompts user and loads CSV file if required */
	bool LoadCsvFile();

	/* Set control data */
	void SetControlData();
	void SetDocument(CMWEditDoc *pDoc) {
		m_pDocument = pDoc;
	}


	//{{AFX_DATA(CEsmCsvImportDlg)
	enum {
		IDD = IDD_IMPORTCSV_DLG
	};

	CListCtrl m_CsvList;
	CEdit m_CsvFilename;
	//}}AFX_DATA

  public:

	/* ClassWizard generated virtual function overrides */

	//{{AFX_VIRTUAL(CEsmCsvImportDlg)

  protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	//}}AFX_VIRTUAL

  protected:
	/* Generated message map functions */

	//{{AFX_MSG(CEsmCsvImportDlg)
	afx_msg void OnLoadcsvbutton();
	afx_msg void OnCreatebutton();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP();
};


//{{AFX_INSERT_LOCATION}}
//}}AFX_INSERT_LOCATION


#endif
