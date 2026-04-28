/*===========================================================================
 *
 * File:    SString.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Tuesday, May 08, 2001
 *
 * Contains the definition for the CSString class for handling simple
 * strings.
 *
 *=========================================================================*/
#ifndef __SSTRING_H
#define __SSTRING_H

#include <cstddef>

#include "common/dl_base.h"
#include "common/dl_log.h"
#include "common/string/sstring_data.h"

/* Number of extra bytes to allocate when creating strings */
#define SSTRING_ALLOC_EXTRA 32
/*===========================================================================
 *
 * Begin Class CSString Definition
 *
 * A simple class for handling strings.
 *
 *=========================================================================*/
class CSString {
  private:
	TCHAR *m_pString; /* The string data */


  protected:
	/* Allocate string buffers of a given size */
	void AllocString(const int StringSize);
	void AllocCopy(const int StringSize);

	/* Create a string buffer from two source strings */
	void ConcatStrings(const int String1Size,
	                   const TCHAR *pString1,
	                   const int String2Size,
	                   const TCHAR *pString2);

	/* Free the current string data */
	void FreeData();

	/* Access the string data */
	CSStringData *GetData() const {
		IASSERT(m_pString != NULL);
		return ((CSStringData *)m_pString) - 1;
	}

	/* Initializes the string data */
	void Init();


  public:
	/* Class Constructors */
	CSString();
	CSString(const TCHAR *pString);
	CSString(const TCHAR *pString, const int Length);
	CSString(const CSString &SrcString);
	CSString(const int StringSize);
	CSString(const TCHAR Char, const int Count);

	/* Class Destructors */
	~CSString();
	void Destroy();

	/* String comparison */
	int Compare(const TCHAR *pString) const;
	int CompareNoCase(const TCHAR *pString) const;

	/* Copy a number of characters from source strings */
	void Copy(const TCHAR *pString, const int Count);
	void Copy(const CSString &String, const int Count);

	/* Empties the string contents */
	void Empty();

	/* Search for sub-strings */
	int Find(const TCHAR *pSearch);
	int FindI(const TCHAR *pSearch);

	/* Frees any extra memory allocated for the string */
	void FreeExtra();

	/* Get the length, in bytes, of the string */
	int GetLength() const;
	int GetAllocLength() const;

	/* Checks for an empty (zero sized) string */
	bool IsEmpty();

	/* Extract sub-strings from string */
	CSString Left(const int Count) const;
	CSString Mid(const int Index, const int Count) const;
	CSString Mid(const int Index) const;
	CSString Right(const int Count) const;

	/* Makes the string lower/upper case */
	void MakeLower();
	void MakeUpper();

	/* Set a specific character in the string */
	void SetAt(const int Index, const TCHAR Char);

	/* Set the maximum string size */
	void SetSize(const int Size);

	/* Trims whitespace from left/right sides of string */
	CSString &TrimLeft();
	CSString &TrimRight();
	CSString &Trim();

	/* Truncate the string at the given index */
	void Truncate(const int Index);

	void UpdateLength();

	/* Overloaded copy operators */
	const CSString &operator=(const CSString &SrcString);
	const CSString &operator=(const TCHAR Char);
	const CSString &operator=(const TCHAR *pSrcString);

	/* String concatenation */
	const CSString &operator+=(const CSString &String);
	const CSString &operator+=(const TCHAR Char);
	const CSString &operator+=(const TCHAR *pString);
	const CSString &Append(const TCHAR *pString, const int Length);

	/* Addition operators, friend functions */
	friend CSString operator+(const CSString &String1, const CSString &String2);
	friend CSString operator+(const TCHAR Char, const CSString &String);
	friend CSString operator+(const CSString &String, const TCHAR Char);
	friend CSString operator+(const TCHAR *pString1, const CSString &String2);
	friend CSString operator+(const CSString &String1, const TCHAR *pString2);

	/* Get a specific character from the string */
	TCHAR operator[](const int Index) const;
	TCHAR GetAt(const int Index) const;

	/* Return a const pointer to the string buffer */
	operator const TCHAR *() const;
};


inline CSString &TerminatePathString(CSString &PathBuffer) {
	if (PathBuffer.GetLength() == 0) {
		return PathBuffer;
	}

	if (PathBuffer.GetAt(PathBuffer.GetLength() - 1) != '\\') {
		PathBuffer += '\\';
	}

	return PathBuffer;
}

/* String comparisons */
inline bool operator==(const CSString &String1, const CSString &String2) {
	return String1.Compare(String2) == 0;
}

inline bool operator==(const TCHAR *String1, const CSString &String2) {
	return String2.Compare(String1) == 0;
}

inline bool operator==(const CSString &String1, const TCHAR *String2) {
	return String1.Compare(String2) == 0;
}

inline bool operator!=(const CSString &String1, const CSString &String2) {
	return String1.Compare(String2) != 0;
}

inline bool operator!=(const TCHAR *String1, const CSString &String2) {
	return String2.Compare(String1) != 0;
}

inline bool operator!=(const CSString &String1, const TCHAR *String2) {
	return String1.Compare(String2) != 0;
}

inline bool operator<=(const CSString &String1, const CSString &String2) {
	return String1.Compare(String2) <= 0;
}

inline bool operator<=(const TCHAR *String1, const CSString &String2) {
	return String2.Compare(String1) >= 0;
}

inline bool operator<=(const CSString &String1, const TCHAR *String2) {
	return String1.Compare(String2) <= 0;
}

inline bool operator>=(const CSString &String1, const CSString &String2) {
	return String1.Compare(String2) >= 0;
}

inline bool operator>=(const TCHAR *String1, const CSString &String2) {
	return String2.Compare(String1) <= 0;
}

inline bool operator>=(const CSString &String1, const TCHAR *String2) {
	return String1.Compare(String2) >= 0;
}

inline bool operator<(const CSString &String1, const CSString &String2) {
	return String1.Compare(String2) < 0;
}

inline bool operator<(const TCHAR *String1, const CSString &String2) {
	return String2.Compare(String1) > 0;
}

inline bool operator<(const CSString &String1, const TCHAR *String2) {
	return String1.Compare(String2) < 0;
}

inline bool operator>(const CSString &String1, const CSString &String2) {
	return String1.Compare(String2) > 0;
}

inline bool operator>(const TCHAR *String1, const CSString &String2) {
	return String2.Compare(String1) < 0;
}

inline bool operator>(const CSString &String1, const TCHAR *String2) {
	return String1.Compare(String2) > 0;
}


#endif
