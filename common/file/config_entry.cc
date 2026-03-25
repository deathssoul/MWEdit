#include "common/file/config_entry.h"

#include "common/file/gen_file.h"
/*===========================================================================
 *
 * Class CConfigEntry Method - bool Write (File);
 *
 * Output the entry to the current position in the file.  Returns false on
 * any error.
 *
 *=========================================================================*/
bool CConfigEntry::Write(CGenFile &File) {
	bool Result;
	/* Output the entry name and value */
	Result = File.Printf("%s = %s\n", m_Variable, m_Value);
	return Result;
}
