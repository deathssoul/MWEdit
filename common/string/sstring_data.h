#ifndef COMMON_STRING_SSTRING_DATA_H_
#define COMMON_STRING_SSTRING_DATA_H_

#include "dl_base.h"

class CSStringData {
  public:
	int Length;      /* Size of string in characters */
	int AllocLength; /* Allocated size of string in characters */
	TCHAR *GetData() {
		return (TCHAR *)(this + 1);
	}
};

#endif  // COMMON_STRING_SSTRING_DATA_H_
