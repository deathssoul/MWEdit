/*===========================================================================
 *
 * File:    DL_Chr.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Sunday, May 06, 2001
 *
 * Contains definitions for TCHARacter related functions of Dave's Library
 * of common code.
 *
 *=========================================================================*/
#ifndef __DL_CHR_H
#define __DL_CHR_H

#include <cstddef>

#include "common/dl_base.h"

#if _UNICODE
#include <cwctype>
#else
#include <cctype>
#endif  // _UNICODE

#if _UNICODE

#if _WIN32
#define TISSPACE(char1) _istspace(char1)
#define TISDIGIT(char1) _istdigit(char1)
#define TISALPHA(char1) _istalpha(char1)
#define TISLOWER(char1) _istlower(char1)
#define TISUPPER(char1) _istupper(char1)
#define TISPRINT(char1) _istprint(char1)
#define TISSPACE(char1) _istspace(char1)
#define TISXDIGIT(char1) _istxdigit(char1)
#define TTOUPPER(char1) std::towupper(char1)
#define TTOLOWER(char1) std::towlower(char1)
#endif  // _WIN32

#else
#define TISSPACE(char1) std::isspace(char1)
#define TISDIGIT(char1) std::isdigit(char1)
#define TISALPHA(char1) std::isalpha(char1)
#define TISLOWER(char1) std::islower(char1)
#define TISUPPER(char1) std::isupper(char1)
#define TISPRINT(char1) std::isprint(char1)
#define TISSPACE(char1) std::isspace(char1)
#define TISXDIGIT(char1) std::isxdigit(char1)
#define TTOUPPER(char1) std::toupper(char1)
#define TTOLOWER(char1) std::tolower(char1)
#endif  // _UNICODE

/* Insert a TCHARacter somewhere in a string */
void chradd(TCHAR *pString, const std::size_t CharIndex, const TCHAR NewChar);

/* Add a TCHARacter to the end of a string */
void chrcat(TCHAR *pString, const TCHAR NewChar);

/* Count the number of TCHARacters in the string */
int chrcount(const TCHAR *pString, const TCHAR Char);

/* Delete a TCHARacter from a string */
void chrdel(TCHAR *pString, const std::size_t CharIndex);

/* Delete the last TCHARacter from the string */
void chrdellast(TCHAR *pString);

/* Reverse search for last punctuation TCHARacter in string */
bool chrrpunc(std::size_t &CharIndex, const TCHAR *pString);

/* Divide string into substrings seperated by token TCHARacter */
TCHAR *chrtok(TCHAR *pString, const TCHAR TokenChar);

/* Truncate a string at first/last occurence of TCHARacter */
TCHAR *chrtrunc(TCHAR *pString, const TCHAR TruncateChar);
TCHAR *chrrtrunc(TCHAR *pString, const TCHAR TruncateChar);


/*===========================================================================
 *
 * Begin Testing Routine Prototypes
 *
 * Prototypes for test functions of module.  Only available in DEBUG builds.
 *
 *=========================================================================*/
#if _DEBUG
void Test_chradd();
void Test_chrcat();
void Test_chrdel();
void Test_chrdellast();
void Test_chrtok();
void Test_chrrpunc();
void Test_chrtrunc();
void Test_chrrtrunc();
void Test_DLChr();
#endif  // _DEBUG


#endif
