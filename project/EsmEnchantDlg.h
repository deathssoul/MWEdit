/*===========================================================================
 *
 * File:    Esmenchantdlg.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  February 15, 2003
 *
 * Description
 *
 *=========================================================================*/

#ifndef __ESMENCHANTDLG_H
#define __ESMENCHANTDLG_H


/*===========================================================================
 *
 * Begin Required Includes
 *
 *=========================================================================*/

#include "EsmRecDialog.h"
#include "Resource.h"

/*===========================================================================
 *      End of Required Includes
 *=========================================================================*/


/*===========================================================================
 *
 * Begin Class CEsmEnchantDlg Definition
 *
 *=========================================================================*/

class CEsmEnchantDlg : public CEsmRecDialog {

	DECLARE_DYNCREATE(CEsmEnchantDlg);

	/*---------- Begin Protected Class Members ------------------------*/

  protected:

	CEsmEnchant *m_pEnchant;
	esmrecinfo_t *m_pEffectInfo[MWESM_ENCHANT_NUMENCHANTS];
	bool m_IsConstantEffect;


	/*---------- Begin Protected Class Methods ------------------------*/

  protected:

	/* Helper set/get methods */
	void GetEffectData();
	void GetEffectData(const int EffectIndex);
	void SetEffectData();
	void SetEffectData(const int EffectIndex, CEsmSubENAM *pEffectRecord);

	void OnSelChangeEffectList(const int EffectIndex);
	void OnSelChangeSkillList(const int EffectIndex);
	void OnSelChangeRangeList(const int EffectIndex);

	/* Update item data */
	virtual int OnUpdateItem(esmrecinfo_t *pRecInfo);
	void UpdateSpellCost(const int Index);
	void UpdateTotalSpellCost();


	/*---------- Begin Public Class Methods ---------------------------*/

  public:

	/* Construction */
	CEsmEnchantDlg();

	/* Get class members */
	virtual bool IsModified();

	/* Set or update the record data */
	virtual void GetControlData();
	virtual void SetControlData();

	/* Dialog Data */

	//{{AFX_DATA(CEsmEnchantDlg)
	enum {
		IDD = IDD_ENCHANT_DLG
	};

	CButton m_AutoCalcCheck;

	CEdit m_CostText;
	CEdit m_ChargeText;

	CComboBox m_TypeList;
	//}}AFX_DATA

	CComboBox m_EffectList[MWESM_ENCHANT_NUMENCHANTS];
	CComboBox m_SkillList[MWESM_ENCHANT_NUMENCHANTS];
	CComboBox m_RangeList[MWESM_ENCHANT_NUMENCHANTS];

	CEdit m_CostLabel[MWESM_ENCHANT_NUMENCHANTS];
	CEdit m_AreaText[MWESM_ENCHANT_NUMENCHANTS];
	CEdit m_DurationText[MWESM_ENCHANT_NUMENCHANTS];
	CEdit m_Magnitude1Text[MWESM_ENCHANT_NUMENCHANTS];
	CEdit m_Magnitude2Text[MWESM_ENCHANT_NUMENCHANTS];
	CEdit m_TotalCostText[MWESM_ENCHANT_NUMENCHANTS];

	/* ClassWizard generated virtual function overrides */

	//{{AFX_VIRTUAL(CEsmEnchantDlg)

  protected:

	virtual void OnInitialUpdate();
	virtual void DoDataExchange(CDataExchange *pDX);

	//}}AFX_VIRTUAL

  protected:

	/* Generated message map functions */

	//{{AFX_MSG(CEsmEnchantDlg)
	afx_msg void OnAutocalccheck();
	afx_msg void OnSelchangeTypelist();
	//}}AFX_MSG

	afx_msg void OnChangeDurationtext1() {
		UpdateSpellCost(0);
	}

	afx_msg void OnChangeDurationtext2() {
		UpdateSpellCost(1);
	}

	afx_msg void OnChangeDurationtext3() {
		UpdateSpellCost(2);
	}

	afx_msg void OnChangeDurationtext4() {
		UpdateSpellCost(3);
	}

	afx_msg void OnChangeDurationtext5() {
		UpdateSpellCost(4);
	}

	afx_msg void OnChangeDurationtext6() {
		UpdateSpellCost(5);
	}

	afx_msg void OnChangeDurationtext7() {
		UpdateSpellCost(6);
	}

	afx_msg void OnChangeDurationtext8() {
		UpdateSpellCost(7);
	}

	afx_msg void OnChangeAreatext1() {
		UpdateSpellCost(0);
	}

	afx_msg void OnChangeAreatext2() {
		UpdateSpellCost(1);
	}

	afx_msg void OnChangeAreatext3() {
		UpdateSpellCost(2);
	}

	afx_msg void OnChangeAreatext4() {
		UpdateSpellCost(3);
	}

	afx_msg void OnChangeAreatext5() {
		UpdateSpellCost(4);
	}

	afx_msg void OnChangeAreatext6() {
		UpdateSpellCost(5);
	}

	afx_msg void OnChangeAreatext7() {
		UpdateSpellCost(6);
	}

	afx_msg void OnChangeAreatext8() {
		UpdateSpellCost(7);
	}

	afx_msg void OnSelchangeEffectlist1() {
		OnSelChangeEffectList(0);
	}

	afx_msg void OnSelchangeEffectlist2() {
		OnSelChangeEffectList(1);
	}

	afx_msg void OnSelchangeEffectlist3() {
		OnSelChangeEffectList(2);
	}

	afx_msg void OnSelchangeEffectlist4() {
		OnSelChangeEffectList(3);
	}

	afx_msg void OnSelchangeEffectlist5() {
		OnSelChangeEffectList(4);
	}

	afx_msg void OnSelchangeEffectlist6() {
		OnSelChangeEffectList(5);
	}

	afx_msg void OnSelchangeEffectlist7() {
		OnSelChangeEffectList(6);
	}

	afx_msg void OnSelchangeEffectlist8() {
		OnSelChangeEffectList(7);
	}

	afx_msg void OnSelchangeSkilllist1() {
		OnSelChangeSkillList(0);
	}

	afx_msg void OnSelchangeSkilllist2() {
		OnSelChangeSkillList(1);
	}

	afx_msg void OnSelchangeSkilllist3() {
		OnSelChangeSkillList(2);
	}

	afx_msg void OnSelchangeSkilllist4() {
		OnSelChangeSkillList(3);
	}

	afx_msg void OnSelchangeSkilllist5() {
		OnSelChangeSkillList(4);
	}

	afx_msg void OnSelchangeSkilllist6() {
		OnSelChangeSkillList(5);
	}

	afx_msg void OnSelchangeSkilllist7() {
		OnSelChangeSkillList(6);
	}

	afx_msg void OnSelchangeSkilllist8() {
		OnSelChangeSkillList(7);
	}

	DECLARE_MESSAGE_MAP();
};

/*===========================================================================
 *      End of Class CEsmEnchantDlg Definition
 *=========================================================================*/


#endif

/*===========================================================================
 *      End of File Esmenchantdlg.H
 *=========================================================================*/
