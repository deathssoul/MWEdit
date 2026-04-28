#ifndef COMMON_FILE_CONFIG_GROUP
#define COMMON_FILE_CONFIG_GROUP

#include "common/dl_base.h"
#include "common/container/ptr_array.h"
#include "common/file/config_entry.h"
#include "common/file/gen_file.h"
#include "common/string/sstring.h"
/*===========================================================================
 *
 * Begin Class CConfigGroup Definition
 *
 * A single group of config variables in a config file.
 *
 *=========================================================================*/
class CConfigGroup {
  private:
	CSString m_Name;          /* Group name */
	CCfgEntryArray m_Entries; /* Array of config entries */


  public:
	/* Class Constructors/Destructors */
	CConfigGroup() : m_Entries(2) {;}

	virtual ~CConfigGroup() {
		Destroy();
	}

	virtual void Destroy();

	/* Adds a new entry to the group */
	CConfigEntry *AddEntry(const TCHAR *pVar, const TCHAR *pValue);

	/* Find the first matching entry */
	CConfigEntry *FindEntry(const TCHAR *pVariable);

	/* Find an existing entry or create it */
	CConfigEntry *GetEntry(const TCHAR *pVariable);

	/* Get class members */
	const TCHAR *GetName() const {
		return m_Name;
	}

	/* Find a variable value */
	const TCHAR *GetValue(const TCHAR *pVariable);

	/* Set class members */
	void SetName(const TCHAR *pName) {
		m_Name = pName;
	}

	/* Set a variable value */
	bool SetValue(const TCHAR *pVariable, const TCHAR *pValue);

	/* Output the group to the given file */
	bool Write(CGenFile &File);
};

typedef TPtrArray<CConfigGroup> CCfgGroupArray;
#endif  // COMMON_FILE_CONFIG_GROUP
