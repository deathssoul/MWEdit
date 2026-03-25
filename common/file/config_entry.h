#ifndef COMMON_FILE_CONFIG_ENTRY
#define COMMON_FILE_CONFIG_ENTRY

#include <winnt.h>

#include "common/container/ptr_array.h"
#include "common/file/gen_file.h"
#include "common/string/sstring.h"
/*===========================================================================
 *
 * Begin Class CConfigEntry Definition
 *
 * One variable/value pair in the config file.
 *
 *=========================================================================*/
class CConfigEntry {
  private:
	CSString m_Variable;
	CSString m_Value;


  public:
	/* Class Constructors/Destructors */
	CConfigEntry() { ; }

	virtual ~CConfigEntry() {
		Destroy();
	}

	virtual void Destroy() {
		m_Variable.Empty();
		m_Value.Empty();
	}

	/* Get class members */
	const TCHAR *GetVariable() const {
		return m_Variable;
	}

	const TCHAR *GetValue() const {
		return m_Value;
	}

	/* Set class members */
	void SetVariable(const TCHAR *pString) {
		m_Variable = pString;
	}

	void SetValue(const TCHAR *pString) {
		m_Value = pString;
	}

	/* Output entry to file */
	bool Write(CGenFile &File);
};

typedef TPtrArray<CConfigEntry> CCfgEntryArray;
#endif  // COMMON_FILE_CONFIG_ENTRY
