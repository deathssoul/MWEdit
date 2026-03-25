/*===========================================================================
 *
 * File:    SString.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Tuesday, May 08, 2001
 *
 * Contains the implementation for the simple string class CSString.
 *
 *=========================================================================*/
#include "common/string/sstring.h"

#include <string.h>  // TODO: Required for non-standard extension _strlwr()

#include <climits>
#include <cstddef>
#include <cstring>

#include "common/dl_base.h"
#include "common/dl_mem.h"
#include "common/dl_str.h"
#include "common/string/sstring_data.h"

DEFINE_FILE("SString.cpp");

/* Defines an empty string */
static int s_SSData[] = {
	0,
	0,
	0
};

static CSStringData *s_pSDataNull = (CSStringData *)&s_SSData;
static const TCHAR *s_pSStringNull = (const TCHAR *)(((byte *)s_pSDataNull) + sizeof(CSStringData));
/*===========================================================================
 *
 * Class CSString Constructor - CSString ();
 *
 *=========================================================================*/
CSString::CSString() {
	//DEFINE_FUNCTION("CSString::CSString()");
	Init();
}


/*===========================================================================
 *
 * Class CSString Constructor - CSString (pSrcString);
 *
 * Uses the given character buffer to initialize the string.  Handles the
 * NULL input string case.
 *
 *=========================================================================*/
CSString::CSString(const TCHAR *pSrcString) {
	//DEFINE_FUNCTION("CSString::CSString(TCHAR*)");
	Init();

	if (pSrcString != NULL) {
		int Length = TSTRLEN(pSrcString);
		AllocString(Length);
		std::memcpy(m_pString, pSrcString, sizeof(TCHAR) * (Length + 1));
	}
}


/*===========================================================================
 *
 * Class CSString Constructor - CSString (pSrcString, SrcLength);
 *
 * Uses the given character buffer to initialize the string, up to the
 * given length.  Handles the NULL input string case.
 *
 *=========================================================================*/
CSString::CSString(const TCHAR *pSrcString, const int SrcLength) {
	//DEFINE_FUNCTION("CSString::CSString(TCHAR*, int)");
	Init();

	/* Check for the NULL case */
	if (pSrcString == NULL && SrcLength > 0) {
		AllocString(SrcLength);
		m_pString[0] = NULL_CHAR;
		GetData()->Length = 0;
	} else { /* Create a normally sized string */
		int InputLength = TSTRLEN(pSrcString);
		AllocString(SrcLength);

		if (InputLength < SrcLength) {
			GetData()->Length = InputLength;
		}

		std::memcpy(m_pString, pSrcString, GetLength() * sizeof(TCHAR));
	}
}


/*===========================================================================
 *
 * Class CSString Constructor - CSString (SrcString);
 *
 * Uses the given CSString object to initialize the string.
 *
 *=========================================================================*/
CSString::CSString(const CSString &SrcString) {
	//DEFINE_FUNCTION("CSString::CSString(CSString&)");
	Init();
	/* Copy the source string */
	AllocString(SrcString.GetLength());
	std::memcpy(m_pString, (const TCHAR *)SrcString, (SrcString.GetLength() + 1) * sizeof(TCHAR));
}


/*===========================================================================
 *
 * Class CSString Constructor - CSString (StringSize);
 *
 * Creates an empty string with the given number of bytes.  String is
 * initially composed of all space characters.
 *
 *=========================================================================*/
CSString::CSString(const int StringSize) {
	//DEFINE_FUNCTION("CSString::CSString(int)");
	Init();

	/* Copy the source string */
	if (StringSize > 0) {
		AllocString(StringSize);
		std::memset(m_pString, (TCHAR)' ', StringSize * sizeof(TCHAR));
	}
}


/*===========================================================================
 *
 * Class CSString Constructor - CSString (Char, Count);
 *
 * Creates a multiple character string.  ASSERTs on invalid input.
 *
 *=========================================================================*/
CSString::CSString(const TCHAR Char, const int Count) {
	//DEFINE_FUNCTION("CSString::CSString(TCHAR, int)");
	Init();

	/* Allocate and create the string */
	if (Count > 0) {
		AllocString((int)Count);
		std::memset(m_pString, (int)Char, Count * sizeof(TCHAR));
	}
}


/*===========================================================================
 *
 * Class CSString Destructor
 *
 *=========================================================================*/
CSString::~CSString() {
	FreeData();
}


/*===========================================================================
 *
 * Class CSString Method - void Destroy (void);
 *
 *=========================================================================*/
void CSString::Destroy() {
	Empty();
}


/*===========================================================================
 *
 * Class CSString Method - void AllocString (StringSize);
 *
 * Protected class method which allocates a new string buffer for the
 * given size.
 *
 *=========================================================================*/
void CSString::AllocString(const int StringSize) {
	DEFINE_FUNCTION("CSString::AllocString()");
	CSStringData *pData;
	int nAllocLength;
	/* Check for integer wrap arounds */
	ASSERT(StringSize < INT_MAX - SSTRING_ALLOC_EXTRA - 1);

	/* Use a NULL string? */
	if (StringSize == 0) {
		Init();
	} else { /* Do we need to allocate a new buffer */
		nAllocLength = StringSize + SSTRING_ALLOC_EXTRA;
		pData = (CSStringData *)new byte[sizeof(CSStringData) + (nAllocLength + 1) * sizeof(TCHAR)];
		ValidateNewPointer(pData);
		pData->Length = StringSize;
		pData->AllocLength = nAllocLength;
		pData->GetData()[nAllocLength] = '\0';
		pData->GetData()[StringSize] = '\0';
		m_pString = pData->GetData();
	}

	/* Ensure valid output */
	ASSERT(m_pString != NULL);
	ASSERT(GetLength() <= GetData()->AllocLength);
}


/*===========================================================================
 *
 * Class CSString Method - void AllocCopy (StringSize);
 *
 * Protected class method which allocates a copy of the the current
 * string buffer with the given size, if the new size doesn't fit in the
 * currently allocated buffer.  Thus, all that possibly changes is
 * the allocated string length.
 *
 *=========================================================================*/
void CSString::AllocCopy(const int StringSize) {
	DEFINE_FUNCTION("CSString::AllocCopy()");
	CSStringData *pData;
	int nAllocLength;
	/* Check for integer wrap arounds */
	ASSERT(StringSize < (int)(INT_MAX - SSTRING_ALLOC_EXTRA - 1));

	/* Do we need to allocate a new buffer */
	if (StringSize > GetData()->AllocLength) {
		nAllocLength = StringSize + SSTRING_ALLOC_EXTRA;
		pData = (CSStringData *)new byte[sizeof(CSStringData) + (nAllocLength + 1) * sizeof(TCHAR)];
		ValidateNewPointer(pData);
		pData->Length = StringSize;
		pData->AllocLength = nAllocLength;
		/* Copy the old buffer to the new then destroy the old */
		std::memcpy(pData->GetData(), m_pString, (GetData()->Length + 1) * sizeof(TCHAR));
		pData->GetData()[nAllocLength] = '\0';
		FreeData();
		m_pString = pData->GetData();
	}

	/* Ensure valid output */
	GetData()->Length = StringSize;
	m_pString[StringSize] = '\0';
	ASSERT(m_pString != NULL);
	ASSERT(GetLength() <= GetData()->AllocLength);
}


/*===========================================================================
 *
 * Class CSString Method - void ConcatStrings (String1Size, pString1,
 *                         String2Size, pString2);
 *
 * Protected class method used by the friend operator+() functions.
 * Creates a concatenation of pString1+pString2.  Handles NULL and
 * zero length strings.
 *
 *=========================================================================*/
void CSString::ConcatStrings(const int String1Size,
                             const TCHAR *pString1,
                             const int String2Size,
                             const TCHAR *pString2) {
	DEFINE_FUNCTION("CSString::ConcatStrings()");
	int NewSize;
	/* Ensure the strings can combine to produce a valid size */
	ASSERT(String1Size < (int)(INT_MAX - String2Size));
	NewSize = String1Size + String2Size;
	/* Create a buffer larger enough to hold the new string */
	AllocString(NewSize);

	/* Create the new string */
	if (pString1 != NULL && String1Size != 0) {
		std::memmove(m_pString, pString1, String1Size * sizeof(TCHAR));
	}

	if (pString2 != NULL && String2Size != 0) {
		std::memmove(m_pString + String1Size, pString2, String2Size * sizeof(TCHAR));
	}

	m_pString[NewSize] = NULL_CHAR;
}


/*===========================================================================
 *
 * Class CSString Method - void Empty (void);
 *
 * Deletes the current string contents, reallocating an empty string.
 *
 *=========================================================================*/
void CSString::Empty() {
	DEFINE_FUNCTION("CSString::Empty()");
	FreeData();
	/* Ensure success */
	ASSERT(GetLength() == 0);
	ASSERT(m_pString[0] == NULL_CHAR);
}


/*===========================================================================
 *
 * Class CSString Method - void FreeData (void);
 *
 *=========================================================================*/
void CSString::FreeData() {
	if (GetData() != s_pSDataNull) {
		TCHAR *pStringData = (TCHAR *)GetData();
		DestroyArrayPointer(pStringData);
		Init();
	}
}


/*===========================================================================
 *
 * Class CSString Method - void FreeExtra (void);
 *
 * Ensures that the allocated and string lengths are equal, removing any
 * extra allocated buffer from the end of the string.
 *
 *=========================================================================*/
void CSString::FreeExtra() {
	DEFINE_FUNCTION("CSString::FreeExtra()");
	CSStringData *pData;
	int nAllocLength;

	/* Create an allocated string the exact size of the current buffer */
	if (GetLength() != GetData()->AllocLength) {
		nAllocLength = GetLength();
		pData = (CSStringData *)new byte[sizeof(CSStringData) + (nAllocLength + 1) * sizeof(TCHAR)];
		ValidateNewPointer(pData);
		pData->Length = GetLength();
		pData->AllocLength = nAllocLength;
		/* Copy the old buffer to the new then destroy the old */
		std::memcpy(pData->GetData(), m_pString, (GetData()->Length + 1) * sizeof(TCHAR));
		pData->GetData()[nAllocLength] = '\0';
		FreeData();
		m_pString = pData->GetData();
	}
}


/*===========================================================================
 *
 * Class CSString Method - void Init (void);
 *
 * Protected class method to intialize the string at creation.
 *
 *=========================================================================*/
void CSString::Init() {
	//DEFINE_FUNCTION("CSString::Init()");
	m_pString = (TCHAR *)s_pSStringNull;
}


/*===========================================================================
 *
 * Class CSString Method - CSString& TrimLeft (void);
 *
 * Trims whitespace from the left side of the string (according to the
 * isspace() function).
 *
 *=========================================================================*/
CSString &CSString::TrimLeft() {
	DEFINE_FUNCTION("CSString::TrimLeft()");
	int StringIndex = 0;

	/* Find the first non-whitespace character from left side of string */
	while (m_pString[StringIndex] != NULL_CHAR) {
		if (!TISSPACE(m_pString[StringIndex])) {
			break;
		}

		StringIndex++;
	}

	/* Shift the string, if required, to remove whitespace */
	if (StringIndex != 0) {
		std::memmove(m_pString,
		             m_pString + StringIndex,
		             (GetLength() - StringIndex + 1) * sizeof(TCHAR));
		GetData()->Length -= StringIndex;
		ASSERT(GetLength() >= 0);
		m_pString[GetLength()] = NULL_CHAR;
	}

	ASSERT(m_pString[GetLength()] == NULL_CHAR);
	return *this;
}


/*===========================================================================
 *
 * Class CSString Method - CSString& TrimRight (void);
 *
 * Trims whitespace from the right side of the string (according to the
 * isspace() function).
 *
 *=========================================================================*/
CSString &CSString::TrimRight() {
	DEFINE_FUNCTION("CSString::TrimRight()");
	int StringIndex = GetLength();

	/* Find the first non-whitespace character from right side of string */
	while (StringIndex != 0) {
		StringIndex--;

		if (!TISSPACE(m_pString[StringIndex])) {
			break;
		}
	}

	/* Terminate the string at the last non-whitespace TCHAR */
	if (StringIndex != GetLength()) {
		m_pString[StringIndex + 1] = NULL_CHAR;
		GetData()->Length = StringIndex + 1;
	}

	ASSERT(m_pString[GetLength()] == NULL_CHAR);
	return *this;
}


/*===========================================================================
 *
 * Class CSString& CSString Method - operator= (SrcString);
 *
 * Copies the given source string to the current object.
 *
 *=========================================================================*/
const CSString &CSString::operator=(const CSString &SrcString) {
	//DEFINE_FUNCTION("CSString::operator=(CSString&)");
	if (GetLength() < SrcString.GetLength()) {
		FreeData();
		AllocString(SrcString.GetLength());
	}

	std::memcpy(m_pString, (const TCHAR *)SrcString, (SrcString.GetLength() + 1) * sizeof(TCHAR));
	GetData()->Length = SrcString.GetLength();
	return *this;
}


/*===========================================================================
 *
 * Class CSString& CSString Method - operator= (Char);
 *
 * Creates a one character string.
 *
 *=========================================================================*/
const CSString &CSString::operator=(const TCHAR Char) {
	//DEFINE_FUNCTION("CSString::operator=(Char)");
	if (GetLength() < 1) {
		FreeData();
		AllocString(1);
	}

	m_pString[0] = Char;
	m_pString[1] = NULL_CHAR;
	GetData()->Length = 1;
	return *this;
}


/*===========================================================================
 *
 * Class CSString& CSString Method - operator= (pSrcString);
 *
 * Copies the given string buffer to the current object.  Handles the
 * NULL string case.
 *
 *=========================================================================*/
const CSString &CSString::operator=(const TCHAR *pSrcString) {
	//DEFINE_FUNCTION("CSString::operator=(TCHAR*)");

	/* Check for the NULL string case */
	if (pSrcString == NULL) {
		FreeData();
	} else { /* Create a normally sized string */
		int Length = TSTRLEN(pSrcString);

		if (GetLength() < Length) {
			FreeData();
			AllocString(Length);
		}

		std::memcpy(m_pString, pSrcString, sizeof(TCHAR) * (Length + 1));
		GetData()->Length = Length;
	}

	return *this;
}


/*===========================================================================
 *
 * Class CSString Method - CSString& operator+= (pString);
 *
 * Concatenates the given string to the end of the string buffer.
 * Accepts NULL string input.
 *
 *=========================================================================*/
const CSString &CSString::operator+=(const TCHAR *pString) {
	DEFINE_FUNCTION("CSString::operator+=(TCHAR*)");
	int NewLength;
	int OldLength;

	/* Ignore NULL inputs */
	if (pString == NULL) {
		return *this;
	}

	/* Ensure an allocated buffer of the correct length */
	NewLength = TSTRLEN(pString);
	OldLength = GetLength();
	AllocCopy(NewLength + OldLength);
	/* Add the source string to the end of the buffer */
	std::memmove(m_pString + OldLength, pString, (NewLength + 1) * sizeof(TCHAR));
	/* Ensure valid output */
	ASSERT(m_pString[GetLength()] == NULL_CHAR);
	return *this;
}


const CSString &CSString::Append(const TCHAR *pString, const int Length) {
	DEFINE_FUNCTION("CSString::Append(TCHAR*, int)");
	int OldLength;

	/* Ignore NULL inputs */
	if (pString == NULL) {
		return *this;
	}

	/* Ensure an allocated buffer of the correct length */
	OldLength = GetLength();
	AllocCopy(Length + OldLength);
	/* Add the source string to the end of the buffer */
	std::memmove(m_pString + OldLength, pString, (Length + 1) * sizeof(TCHAR));
	/* Ensure valid output */
	ASSERT(m_pString[GetLength()] == NULL_CHAR);
	return *this;
}

/* String comparison, case sensitive, returns as per strcmp() */
int CSString::Compare(const TCHAR *pString) const {
	IASSERT(pString != NULL);
	return std::strcmp(m_pString, pString);
}

/* String comparison, case insensitive, returns as per stricmp() */
int CSString::CompareNoCase(const TCHAR *pString) const {
	IASSERT(pString != NULL);
	return _stricmp(m_pString, pString);
}

/* Creates a new string at most Count bytes in size */
void CSString::Copy(const TCHAR *pString, const int Count) {
	//int InputSize = SafeStrLen(pString);

	/* Check for special cases */
	if (Count <= 0) {
		Empty();
		return;
	}

	if (GetLength() < Count) {
		FreeData();
		AllocString(Count);
	}

	if (pString != NULL) {
		std::memcpy(m_pString, pString, Count * sizeof(TCHAR));
	}

	m_pString[Count] = NULL_CHAR;
	GetData()->Length = Count;
	//if (InputSize < Count) GetData()->Length = InputSize;
}

int CSString::Find(const TCHAR *pString) {
	if (pString == NULL || *pString == NULL_CHAR) {
		return -1;
	}

	TCHAR *pFind = TSTRSTR(m_pString, pString);

	if (pFind == NULL) {
		return -1;
	}

	return pFind - m_pString;
}

int CSString::FindI(const TCHAR *pString) {
	if (pString == NULL || *pString == NULL_CHAR) {
		return -1;
	}

	TCHAR *pFind = stristr(m_pString, pString);

	if (pFind == NULL) {
		return -1;
	}

	return pFind - m_pString;
}

/* Return the current string size */
int CSString::GetLength() const {
	return GetData()->Length;
}

/* Get the current allocated size of string */
int CSString::GetAllocLength() const {
	return GetData()->AllocLength;
}

/* Returns TRUE if the string is empty, "" */
bool CSString::IsEmpty() {
	return (GetLength() == 0) ? TRUE : FALSE;
}

/* Returns a string containing the first Count characters from the
 * left of the string object */
CSString CSString::Left(const int Count) const {
	int NewCount = Count;

	if (NewCount > GetLength()) {
		NewCount = GetLength();
	}

	if (NewCount < 0) {
		NewCount = 0;
	}

	CSString NewString(m_pString, NewCount);
	return NewString;
}

/* Makes the string all lower/upper case */
void CSString::MakeLower() {
	_strlwr(m_pString);
}

void CSString::MakeUpper() {
	_strupr(m_pString);
}

/* Returns a new string starting at the 0-based index of the current string */
CSString CSString::Mid(const int Index) const {
	int NewIndex = Index;

	if (NewIndex > GetLength()) {
		NewIndex = GetLength();
	}

	if (NewIndex < 0) {
		NewIndex = 0;
	}

	CSString NewString(m_pString + NewIndex);
	return NewString;
}

/* Returns a new string starting at the 0-based index of the current string
 * with the given length. */
CSString CSString::Mid(const int Index, const int Length) const {
	int NewIndex = Index;
	int NewLength;

	if (NewIndex > GetLength()) {
		NewIndex = GetLength();
	}

	if (NewIndex < 0) {
		NewIndex = 0;
	}

	if (NewLength < 0) {
		NewLength = 0;
	}

	if (NewLength + NewIndex > GetLength()) {
		NewLength = GetLength() - NewIndex;
	}

	CSString NewString(m_pString + NewIndex, NewLength);
	return NewString;
}

/* Returns a string containing the first Count characters from the
 * right side of the string object */
CSString CSString::Right(const int Count) const {
	int NewCount = Count;

	if (NewCount > GetLength()) {
		NewCount = GetLength();
	}

	if (NewCount < 0) {
		NewCount = 0;
	}

	CSString NewString(m_pString + GetLength() - NewCount, NewCount);
	return NewString;
}

void CSString::SetSize(const int Size) {
	AllocCopy(Size);
}

/* Trim whitespace from right and left sides of string */
CSString &CSString::Trim() {
	TrimLeft();
	TrimRight();
	return *this;
}

/* Access the (const TCHAR*) string */
CSString::operator const TCHAR*() const {
	return m_pString;
}

/* Get the specified character from the string.  ASSERTs if given an
   invalid index (0 returns the first character in string). */
TCHAR CSString::operator[](const int Index) const {
	return GetAt(Index);
}

/* Same as operator[], return a specific character in string */
TCHAR CSString::GetAt(const int Index) const {
	IASSERT(Index < GetLength() && Index >= 0);
	return m_pString[Index];
}

/* Sets a specific character in the string.  ASSERTs if an invalid
 * index is given.  0 is the first character in string. */
void CSString::SetAt(const int Index, const TCHAR Char) {
	IASSERT(Index < GetLength() && Index >= 0);
	m_pString[Index] = Char;
}

/* Truncate the string at the given index */
void CSString::Truncate(const int Index) {
	if (Index < 0 || Index >= GetLength()) {
		return;
	}

	m_pString[Index] = NULL_CHAR;
	GetData()->Length = Index;
}

void CSString::UpdateLength() {
	int Index = GetLength() - 1;

	while (Index > 0) {
		if (m_pString[Index] != NULL_CHAR) {
			GetData()->GetData()[Index + 1] = NULL_CHAR;
			GetData()->Length = Index + 1;
			return;
		}

		Index--;
	}
}


/*===========================================================================
 *
 * Class CSString Method - CSString& operator+= (SourceString);
 *
 * Concatenates the given string to the end of the string buffer.
 * Accepts NULL string input.
 *
 *=========================================================================*/
const CSString &CSString::operator+=(const CSString &SourceString) {
	DEFINE_FUNCTION("CSString::operator+=(CSString)");
	int OldLength;
	/* Ensure an allocated buffer of the correct length */
	OldLength = GetLength();
	AllocCopy(SourceString.GetLength() + OldLength);
	/* Add the source string to the end of the buffer */
	std::memmove(m_pString + OldLength,
	             (const TCHAR *)SourceString,
	             sizeof(TCHAR) * (SourceString.GetLength() + 1));
	/* Ensure valid output */
	ASSERT(m_pString[GetLength()] == NULL_CHAR);
	return *this;
}


/*===========================================================================
 *
 * Class CSString Method - CSString& operator+= (SourceChar);
 *
 * Concatenates the given character to the end of the string buffer.
 *
 *=========================================================================*/
const CSString &CSString::operator+=(const TCHAR SourceChar) {
	DEFINE_FUNCTION("CSString::operator+=(TCHAR)");
	int OldLength;
	/* Ensure an allocated buffer of the correct length */
	OldLength = GetLength();
	AllocCopy(OldLength + 1);
	/* Add the source character to the end of the buffer */
	m_pString[OldLength] = SourceChar;
	m_pString[OldLength + 1] = NULL_CHAR;
	/* Ensure valid output */
	ASSERT(m_pString[GetLength()] == NULL_CHAR);
	return *this;
}


/*===========================================================================
 *
 * Function - CSString operator+ (String1, String2);
 *
 * Friend function to the CSString class to add two CSString objects.
 *
 *=========================================================================*/
CSString operator+(const CSString &String1, const CSString &String2) {
	DEFINE_FUNCTION("operator+(CSString, CSString)");
	CSString NewString;
	NewString.ConcatStrings(String1.GetLength(),
	                        (const TCHAR *)String1,
	                        String2.GetLength(),
	                        (const TCHAR *)String2);
	return NewString;
}


/*===========================================================================
 *
 * Function - CSString operator+ (pString1, String2);
 *
 * Friend function to the CSString class to add a string pointer with
 * a CSString object.
 *
 *=========================================================================*/
CSString operator+(const TCHAR *pString1, const CSString &String2) {
	DEFINE_FUNCTION("operator+(TCHAR*, CSString)");
	CSString NewString;
	NewString.ConcatStrings(SafeStrLen(pString1),
	                        pString1,
	                        String2.GetLength(),
	                        (const TCHAR *)String2);
	return NewString;
}


/*===========================================================================
 *
 * Function - CSString operator+ (String1, pString2);
 *
 * Friend function to the CSString class to add a string pointer with
 * a CSString object.
 *
 *=========================================================================*/
CSString operator+(const CSString &String1, const TCHAR *pString2) {
	DEFINE_FUNCTION("operator+(CSString, TCHAR*)");
	CSString NewString;
	NewString.ConcatStrings(String1.GetLength(),
	                        (const TCHAR *)String1,
	                        SafeStrLen(pString2),
	                        pString2);
	return NewString;
}


/*===========================================================================
 *
 * Function - CSString operator+ (String1, TCHAR);
 *
 * Friend function to the CSString class to add a character pointer with
 * a CSString object.
 *
 *=========================================================================*/
CSString operator+(const CSString &String1, const TCHAR Char2) {
	DEFINE_FUNCTION("operator+(CSString, TCHAR)");
	CSString NewString;
	NewString.ConcatStrings(String1.GetLength(),
	                        (const TCHAR *)String1,
	                        1,
	                        &Char2);
	return NewString;
}


/*===========================================================================
 *
 * Function - CSString operator+ (TCHAR, String2);
 *
 * Friend function to the CSString class to add a character pointer with
 * a CSString object.
 *
 *=========================================================================*/
CSString operator+(const TCHAR Char1, const CSString &String2) {
	DEFINE_FUNCTION("operator+(TCHAR, CSString)");
	CSString NewString;
	NewString.ConcatStrings(1,
	                        &Char1,
	                        String2.GetLength(),
	                        (const TCHAR *)String2);
	return NewString;
}
