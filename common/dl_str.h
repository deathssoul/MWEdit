/*===========================================================================
 *
 * File:    DL_Str.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Saturday, May 05, 2001
 *
 * Contains string related definitions for Dave's Library of common code.
 *
 *=========================================================================*/
#ifndef __DL_STR_H
#define __DL_STR_H

#include <tchar.h>

#include <cstddef>

#include "common/dl_base.h"

#if !_UNICODE
#include <string.h>  // TODO: Required for non-standard extension _stricmp()

#include <cstdlib>
#include <cstring>
#endif

/* Basic string-tchar definitions as required */
#if _UNICODE

#if _WIN32
#define TSTRLEN(string) _tcslen(string)
#define TSTRCMP(string1, string2) _tcscmp(string1, string2)
#define _stricmp(string1, string2) _tcsicmp(string1, string2)
#define TSTRCPY(string1, string2) _tcscpy(string1, string2)
#define TSTRCAT(string1, string2) _tcscat(string1, string2)
#define TSTRCHR(string, char1) _tcschr(string, (TCHAR)char1)
#define TSTRRCHR(string, char1) _tcsrchr(string, (TCHAR)char1)
#define TSTRTOK(string1, string2) _tcstok(string1, string2)
#define TSTRNCMP(string1, string2, length) _tcsncmp(string1, string2, length)
#define TSTRNICMP(string1, string2, length) _tcsnicmp(string1, string2, length)
#define TSTRSTR(string1, string2) _tcsstr(string1, string2)
#define TSTRNCAT(string1, string2, length) _tcsncat(string1, string2, length)
#define TSTRNCPY(string1, string2, length) strnncpy(string1, string2, length)
#define TSTRTOL(string, ptr, base) _tcstol(string, ptr, base)
#define TSTRTOD(string, ptr) _tcstod(string, ptr)
#endif  // _WIN32

#else
#define TSTRCPY(string1, string2) std::strcpy(string1, string2)
#define TSTRCAT(string1, string2) std::strcat(string1, string2)
#define TSTRCMP(string1, string2) std::strcmp(string1, string2)
#define _stricmp(string1, string2) stricmp(string1, string2)
#define TSTRLEN(string) std::strlen(string)
#define TSTRCHR(string, char1) std::strchr(string, char1)
#define TSTRRCHR(string, char1) std::strrchr(string, char1)
#define TSTRTOK(string1, string2) std::strtok(string1, string2)
#define TSTRNCMP(string1, string2, length) std::strncmp(string1, string2, length)
#define TSTRNICMP(string1, string2, length) strnicmp(string1, string2, length)
#define TSTRNCPY(string1, string2, length) strnncpy(string1, string2, length)
#define TSTRSTR(string1, string2) std::strstr(string1, string2)
#define TSTRNCAT(string1, string2, length) std::strncat(string1, string2, length)
#define TSTRTOL(string, ptr, base) std::strtol(string, ptr, base)
#define TSTRTOD(string, ptr) std::strtod(string, ptr)
#endif  // _UNICODE

/* Convert a bool value to a string */
#define BooleanToString(Flag) ((Flag) ? _T("True") : _T("False"))
#define BOOLTOYESNO(Flag)     ((Flag) ? _T("Yes") : _T("No"))

/* Shortcut to trimming whitespace from a string */
#define trim(String) ltrim(rtrim(String))

/* Returns the number of substrings in the given string */
std::size_t CountSubstrings(const TCHAR *pSourceString, const TCHAR *pSearchString);

/* Is... type functions for strings */
bool IsStringNumber(const TCHAR *pString);
bool IsStringDigit(const TCHAR *pString);
bool IsStringPrint(const TCHAR *pString);
bool IsStringFloat(const TCHAR *pString);

/* Strip whitespace from left/right side of string */
TCHAR *ltrim(TCHAR *pString);
TCHAR *rtrim(TCHAR *pString);

/* A strlen() function which can handle NULL strings */
std::size_t SafeStrLen(const TCHAR *pString);

/* Seperate a string in a variable/value pair */
bool SeperateVarValue(TCHAR **ppVariable,
                      TCHAR **ppValue,
                      TCHAR *pString,
                      const TCHAR SeperatorTCHAR = _T('='),
                      const TCHAR CommentTCHAR = _T('#'));
bool SeperateVarValueQ(TCHAR **ppVariable,
                       TCHAR **ppValue,
                       TCHAR *pString,
                       const TCHAR SeperatorTCHAR = _T('='),
                       const TCHAR CommentTCHAR = _T('#'));

/* A smart string compare function which supports NULL cases and case sensitivity */
bool StringChanged(const TCHAR *pString1, const TCHAR *pString2, const bool CaseSensitive = false);
int StringCompare(const TCHAR *pString1, const TCHAR *pString2, const bool CaseSensitive = false);

/* Convert a string to a bool value */
bool StringToBoolean(bool &Flag, const TCHAR *pString);
bool StringToBoolean(const TCHAR *pString);

/* Counts the number of lines in string, seperated by a CR */
std::size_t strhgt(const TCHAR *pString);

/* Find a substring in a string with case insensitivity */
TCHAR *stristr(const TCHAR *pString, const TCHAR *pSubString);

/* Compares two strings up to the length of the shortest, case insensitive */
int strlicmp(const TCHAR *pString1, const TCHAR *pString2);

/* Return number of TCHARacters to first CR or end of string */
std::size_t strlinelen(const TCHAR *pString);

/* Returns the maximum line length of lines seperated by CR */
std::size_t strmaxlinelen(const TCHAR *pString);

/* Copies a maximum number of TCHARacter ensuring string is NULL terminated */
TCHAR *strnncpy(TCHAR *pDestString, const TCHAR *pSourceString, const std::size_t MaxStringLength);

/* Output printf() formatted message to a string buffer */
//int snprintf(TCHAR *pBuffer, const std::size_t MaxLength, const TCHAR *pFormat, ...);

/* Removes quotes from the string */
TCHAR *UnquoteString(TCHAR *pString);

/* String argument formatter with length checking */
//int vsnprintf(TCHAR *pBuffer, const std::size_t MaxLength, const TCHAR *pFormat, std::va_list Args);


/* A strlen() function which can handle NULL strings */
inline std::size_t SafeStrLen(const TCHAR *pString) {
	return (pString == NULL) ? 0 : TSTRLEN(pString);
}


/*===========================================================================
 *
 * Begin Standard Replacement Function Prototypes
 *
 * These functions are only used in systems where they are not
 * otherwise available.
 *
 *=========================================================================*/
#ifndef _WIN32
/* Standard uppercase/lowercase conversion functions */
TCHAR *strlwr(TCHAR *pString);
TCHAR *strupr(TCHAR *pString);

	/* Compare a portion of a string with case insensitivity */
int strnicmp(const TCHAR *pString1, const TCHAR *pString2, const std::size_t MaxStringLength);
#endif  // _WIN32

/*===========================================================================
 *
 * Begin Testing Routine Prototypes
 *
 * Prototypes for test functions of module.  Only available in DEBUG builds.
 *
 *=========================================================================*/
#if _DEBUG
void Test_vsnprintf();
void Test_CountSubstrings();
void Test_ltrim();
void Test_rtrim();
void Test_strlinelen();
void Test_SeperateVarValue();
void Test_StringToBoolean();
void Test_StringChanged();
void Test_IsStringNumber();
void Test_stristr();
void Test_strlicmp();
void Test_strnncpy();
void Test_strproper();
void Test_strhgt();
void Test_strmaxlinelen();
void Test_strupr();
void Test_strnicmp();
void Test_DLStr();
#endif  // _DEBUG


#endif
