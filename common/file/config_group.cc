#include "common/file/config_group.h"

#include <cstddef>

#include "common/dl_base.h"
#include "common/dl_log.h"
#include "common/dl_mem.h"
#include "common/dl_str.h"
#include "common/file/config_entry.h"
#include "common/file/gen_file.h"
/*===========================================================================
 *
 * Class CConfigGroup Method - void Destroy (void);
 *
 *=========================================================================*/
void CConfigGroup::Destroy() {
	CConfigEntry *pEntry;
	int Index;

	/* Unallocate all groups */
	for (Index = 0; Index < m_Entries.GetSize(); Index++) {
		pEntry = (CConfigEntry *)m_Entries.GetAt(Index);
		DestroyPointer(pEntry);
	}

	/* Clear the array */
	m_Entries.RemoveAll();
	m_Name.Empty();
}


/*===========================================================================
 *
 * Class CConfigGroup Method - CConfigEntry* AddEntry (pVar, pValue);
 *
 * Adds a new entry to the group.  Assumes that input strings are whitespace
 * trimmed already.  Does not check for duplicates.  Returns the new entry
 * pointer or NULL.
 *
 *=========================================================================*/
CConfigEntry *CConfigGroup::AddEntry(const TCHAR *pVar, const TCHAR *pValue) {
	DEFINE_FUNCTION("CConfigGroup::AddEntry()");
	CConfigEntry *pEntry;
	/* Allocate the new object */
	CreatePointer(pEntry, CConfigEntry);
	pEntry->SetVariable(pVar);

	if (pValue != NULL) {
		pEntry->SetValue(pValue);
	} else {
		pEntry->SetValue("");
	}

	m_Entries.Add(pEntry);
	return pEntry;
}


/*===========================================================================
 *
 * Class CConfigGroup Method - CConfigEntry* FindEntry (pVariable);
 *
 * Returns the entry with the given variable name, or NULL.
 *
 *=========================================================================*/
CConfigEntry *CConfigGroup::FindEntry(const TCHAR *pVariable) {
	DEFINE_FUNCTION("CConfigGroup::FindEntry()");
	CConfigEntry *pEntry;
	int Result;
	int Index;
	/* Ensure valid input */
	ASSERT(pVariable != NULL);

	/* Search all defined entries */
	for (Index = 0; Index < m_Entries.GetSize(); Index++) {
		pEntry = (CConfigEntry *)m_Entries.GetAt(Index);
		Result = _stricmp(pVariable, pEntry->GetVariable());

		if (Result == 0) {
			return pEntry;
		}
	}

	return NULL;
}


/*===========================================================================
 *
 * Class CConfigGroup Method - CConfigEntry* GetEntry (pVariable);
 *
 * Finds an existing entry or creates it if it does not exist.
 *
 *=========================================================================*/
CConfigEntry *CConfigGroup::GetEntry(const TCHAR *pVariable) {
	DEFINE_FUNCTION("CConfigGroup::GetEntry()");
	CConfigEntry *pEntry;
	/* Find an existing variable */
	pEntry = FindEntry(pVariable);

	if (pEntry != NULL) {
		return pEntry;
	}

	/* Create a new variable entry */
	pEntry = AddEntry(pVariable, NULL);
	ASSERT(pEntry != NULL);
	return pEntry;
}


/*===========================================================================
 *
 * Class TCHAR* CConfigGroup Method - const GetValue (pVariable);
 *
 * Returns the value of the first variable, or NULL if it does not exist.
 *
 *=========================================================================*/
const TCHAR *CConfigGroup::GetValue(const TCHAR *pVariable) {
	CConfigEntry *pEntry = FindEntry(pVariable);

	if (pEntry == NULL) {
		return NULL;
	}

	return pEntry->GetValue();
}


/*===========================================================================
 *
 * Class CConfigGroup Method - bool SetValue (pVariable, pValue);
 *
 * Modifies an existing variable value or creates it if it does not exist.
 *
 *=========================================================================*/
bool CConfigGroup::SetValue(const TCHAR *pVariable, const TCHAR *pValue) {
	CConfigEntry *pEntry;
	/* Find or create the entry */
	pEntry = GetEntry(pVariable);
	pEntry->SetValue(pValue);
	return true;
}


/*===========================================================================
 *
 * Class CConfigGroup Method - bool Write (File);
 *
 * Output the group to the current position in the file.  Returns false on
 * any error.
 *
 *=========================================================================*/
bool CConfigGroup::Write(CGenFile &File) {
	CConfigEntry *pEntry;
	int Index;
	bool Result;

	/* Output the group name if not empty */
	if (m_Name.GetLength() > 2) {
		Result = File.Printf("%s\n", m_Name);

		if (!Result) {
			return false;
		}
	}

	/* Output all entries in group */
	for (Index = 0; Index < m_Entries.GetSize(); Index++) {
		pEntry = (CConfigEntry *)m_Entries.GetAt(Index);
		Result = pEntry->Write(File);
	}

	return true;
}
