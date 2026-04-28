/*===========================================================================
 *
 * File:    DL_Base.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Sunday, April 01, 2001
 *
 * Contains common definitions, types, etc... for Dave's Library of common
 * code.
 *
 * 18 September 2003
 *  - Added the DL_RGB() macro.
 *  - Added the TSTRCAT() macro.
 *
 *=========================================================================*/
#ifndef __DL_BASE_H
#define __DL_BASE_H

/*===========================================================================
 *
 * Begin Windows Pragmas
 *
 * Some windows specific pragma directives to eliminate some annoying
 * level 4 warnings.
 *  4514 = Unused inline function has been removed
 *  4512 = Assignment operator could not be generated
 *
 *=========================================================================*/
#if _WIN32
#pragma warning(disable : 4514)
#pragma warning(disable : 4512)
#endif  // _WIN32

#include <windef.h>

#include <cstddef>

/*===========================================================================
 *
 * Begin Enable Debug Memory Handler for Windows
 *
 * By including "crtdgb.h"
 *
 *=========================================================================*/
#if _DEBUG && _WIN32
//#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif  // _DEBUG && _WIN32

/*===========================================================================
 *
 * Begin File Identification
 *
 * Definitions for identifying the file.
 *
 *=========================================================================*/
#define DL_BASE_NAME    "DL_Base.cpp"
#define DL_BASE_VERSION "0.01d"
#define DL_BASE_AUTHOR  "Dave Humphrey"
#define DL_BASE_EMAIL   "uesp@m0use.net"
#define DL_BASE_DATE    "1 April 2001"


/*===========================================================================
 *
 * Begin System Specific Creation Macros
 *
 * Macros to help create system specific commands.
 *
 *=========================================================================*/
#if _WIN32
#define SYS_MSDOS(Cmd)
#define SYS_WIN32(Cmd) Cmd
#define SYS_UNIX(Cmd)
#define SYS_NONE(Cmd)

#else
#define SYS_MSDOS(Cmd)
#define SYS_WIN32(Cmd)
#define SYS_UNIX(Cmd)
#define SYS_NONE(Cmd) Cmd
#endif  // _WIN32


/*===========================================================================
 *
 * Begin Time Stamp Macro
 *
 * Various compilers have different definitions for the time stamp.
 *
 *=========================================================================*/
#if _WIN32
#define __DL_TIMESTAMP__ __TIMESTAMP__
#endif  // _WIN32


#if _WIN32
#define ASM __asm
#define interrupt
#endif  // _WIN32

#define DL_RGB(r, g ,b) ((DWORD)(((BYTE)(r) | \
                                 ((WORD)(g) << 8)) | \
                                 (((DWORD)(BYTE)(b)) << 16)))
#define DL_RED(color) ((BYTE)((color)&0xFF))
#define DL_GREEN(color) ((BYTE)(((color)>>8)&0xFF))
#define DL_BLUE(color) ((BYTE)(((color)>>16)&0xFF))


typedef unsigned char byte;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned int word;
typedef unsigned long ulong;
typedef unsigned long dword;

// TODO redefinition
//#define WORD_MAX  UINT_MAX
//#define DWORD_MAX ULONG_MAX

/* Define the TCHAR type if required */
#if _WIN32
#include <tchar.h>
#else
#ifndef _TCHAR_DEFINED
#if _UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif  // _UNICODE
#define _TCHAR_DEFINED 1
#endif  // _TCHAR_DEFINED

/* Define the custom _T() macro for wide/single byte strings */
#ifndef _T
#if _UNICODE
#define _T(text) L##text
#else
#define _T(text) text
#endif  // _UNICODE
#endif  // _T
#endif  // _WIN32

/*===========================================================================
 *
 * Begin Character Definitions
 *
 * Define commonly used character macros.
 *
 *=========================================================================*/
#define NULL_CHAR ((TCHAR)'\0')
#define DEL_CHAR ((TCHAR)'\b')
#define ESC_CHAR ((TCHAR)'\x1B')
#define LF_CHAR ((TCHAR)'\n')
#define CR_CHAR ((TCHAR)'\r')
#define TAB_CHAR ((TCHAR)'\t')
#define GARBAGE_CHAR ((TCHAR)0xCC)
#define BLOCKEND_CHAR ((TCHAR)0xFC)
#define DEGREE_CHAR ((TCHAR)248)
#define MU_CHAR ((TCHAR)230)

/* The type for the new qsort() user compare function */
typedef int (_cdecl *PQSORT_CMPFUNC)(const void *pElem1,
                                     const void *pElem2,
                                     const long lUserData );
typedef int (_cdecl *PQSORT_CMPFUNC_ORIG)(const void *pElem1, const void *pElem2);


/*===========================================================================
 *
 * Begin Function Definition Macro
 *
 * Use this macro at the beginning of a function to store the function
 * name as a static string allowing the output of the function name
 * in debugging messages.  The string only exists in debug builds and is
 * an empty string in release builds.
 *
 *=========================================================================*/
#if _DEBUG
#define DEFINE_FUNCTION(FuncString) static TCHAR ThisFunction[] = _T(FuncString);
extern TCHAR ThisFunction[10];
#else
//#define DEFINE_FUNCTION(FuncString) static char ThisFunction[] = "";
#define DEFINE_FUNCTION(FuncString)
#define ThisFunction _T("")
#endif  // _DEBUG


/*===========================================================================
 *
 * Begin File Definition Macro
 *
 * Similar to the function macro above but for individual source files
 * instead.  Include the macro at the top of a file to create a static
 * string ThisFile for use in debug messages.  Only defined in debug builds.
 * Use the ThisFile string instead of __FILE__ to reduce the amount of
 * global strings the program allocates.
 *
 *=========================================================================*/
#define DEFINE_FILE(string) static TCHAR ThisFile[] = _T(string);

/* Eliminates errors from not using the DEFINE_FILE() macro */
extern TCHAR ThisFile[];

/*===========================================================================
 *
 * Begin BIT Field Manipulation Macros
 *
 * Definitions to help manipulate bit flags.
 *
 *=========================================================================*/
#define CHECK_BITFLAG(Variable, Flag) ( ((Variable) & (Flag)) != 0)
#define STORE_BITFLAG(Variable, Flag) ((Variable) |=  (Flag))
#define CLEAR_BITFLAG(Variable, Flag) ((Variable) &= ~(Flag))
#define SET_BITFLAG(Variable, Flag, Bool) ((Bool) ? STORE_BITFLAG(Variable, Flag) : CLEAR_BITFLAG(Variable, Flag))


/*===========================================================================
 *
 * Begin Mathematical Constants
 *
 * Commonly used math constants accurate to 21 decimal places.
 *
 *=========================================================================*/
#ifndef M_PI
#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401
#define M_RAD2DEG   57.29577951
#define M_DEG2RAD   0.017453292
#endif  // M_PI


/*===========================================================================
 *
 * Begin Limit Fix Macros
 *
 *=========================================================================*/
#define FIXLIMIT(Value, Min, Max) if ((Value) < (Min)) (Value) = (Min); else if ((Value) > (Max)) (Value) = (Max);

/* Standard qsort() replacement */
void qsort(void *pBase,
           std::size_t NumElements,
           std::size_t ElementWidth,
           PQSORT_CMPFUNC pCmpFunc,
           long lUserData);


/*===========================================================================
 *
 * Inline Function - int l_QSortCompare (pElem1, pElem2);
 *
 * Inline function used by the improved qsort() algorithm.  This function
 * is the one passed onto, and called by, the standard qsort() function.
 * It then passes the call onto the extended compare function specified
 * by the used, along with the custom user data.
 *
 * Note: This function is included in the header only for the purpose
 * of defining it as inline, hopefully improving sort speeds.  Should
 * only be used by the qsort() routine in DL_BASE.CPP.  Similarily
 * with the defined local parameters.
 *
 *=========================================================================*/
/* Local variables needed, do not use! */
extern long l_QSortUserData;
extern PQSORT_CMPFUNC l_QSortCmpFunc;

inline int l_QSortCompare(const void *pElem1, const void *pElem2) {
	//DEFINE_FUNCTION("l_QSortCompare()");
	/* Pass the call onto the extended function */
	return l_QSortCmpFunc(pElem1, pElem2, l_QSortUserData);
}


extern TCHAR LocalePathChar;


#endif
