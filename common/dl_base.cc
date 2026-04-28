/*===========================================================================
 *
 * File:    DL_Base.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Sunday, April 01, 2001
 *
 * Implements common functions for Dave's Library of common code.
 *
 *=========================================================================*/
#include "common/dl_base.h"

#include <cstddef>
#include <cstdlib>

#include "common/dl_log.h"

#if _WIN32
#include "common/dl_err.h"
#else
#include <cstdio>
#endif  // _WIN32

DEFINE_FILE("DL_Base.cpp");

/* The current path seperator character (usually '\\' or '/') */
TCHAR LocalePathChar = '\\';

/* Local variables used by the new qsort() algorithim */
long l_QSortUserData = 0;
PQSORT_CMPFUNC l_QSortCmpFunc = NULL;

/* Global variable used to eliminate errors from not using the
   DEFINE_FUNCTION() macro (for DEBUG builds only) */
#if _DEBUG
TCHAR ThisFunction[] = _T("?");
#endif  // _DEBUG

/*===========================================================================
 *
 * Function - void qsort (pBase, NumElements, ElementWidth, pCmpFunc, lUserData);
 *
 * Same as the standard qsort() algorithm except it takes an extended
 * UserData parameter which is passed onto the user supplied compare
 * function.  This allows one compare function to sort a variety of
 * ways depending on the lUserData parameter.  Uses the standard qsort()
 * algorithim and local variables.
 *
 *=========================================================================*/
void qsort(void *pBase,
           std::size_t NumElements,
           std::size_t ElementWidth,
           PQSORT_CMPFUNC pCmpFunc,
           long lUserData) {
	DEFINE_FUNCTION("qsort()");
	/* Ensure valid input */
	ASSERT(pCmpFunc != NULL);
	/* Save the extended parameters to local variables */
	l_QSortCmpFunc = pCmpFunc;
	l_QSortUserData = lUserData;
	/* Call the standard qsort() algorithm with our inline compare function */
	std::qsort(pBase, NumElements, ElementWidth, l_QSortCompare);
}
