/*===========================================================================
 *
 * File:    DL_Log.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Monday, April 02, 2001
 *
 * Implements the CLogFile class for handling output to a simple log file
 * for debugging purposes.  Note that this module should not use the
 * ErrorHandler for noting errors as the CErrorHandler class uses the
 * CLogFile class, causing possible infinite loops.
 *
 *
 * Version History
 * -------------------------------------------------------------------------
 * 2 December 2002 (Dave Humphrey)
 *  - Moved from regular char to TCHAR type to support wide characters
 *    under Windows. Successfully tested.
 *
 *=========================================================================*/
#include "common/dl_log.h"

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include "common/dl_base.h"
#include "common/dl_file.h"
#include "common/dl_mem.h"
#include "common/dl_time.h"

#if _DEBUG
#include "common/dl_block.h"
#endif  // _DEBUG

#if _WIN32
#include "common/dl_err.h"
#endif  // _WIN32

/* The main log file for debugging output */
CLogFile SystemLog;

DEFINE_FILE("DL_log.cpp");
/*===========================================================================
 *
 * Function - void CustomAssert (pString, pFile, pFunction, Line);
 *
 * A custom assertion type function used by the ASSERT macro defined in
 * DL_BASE.H.  Accepts a message string, file name, function name, and
 * line number where the assertion occurred. Outputs message to stderr.
 * Exits program with abort().
 *
 *=========================================================================*/
void CustomAssert(const TCHAR *pString,
                  const TCHAR *pFile,
                  const TCHAR *pFunction,
                  const long Line) {
	//DEFINE_FUNCTION("CustomAssert()");
	/* Output message to log file */
	if (SystemLog.IsOpen()) {
		SystemLog.Printf(_T("ASSERTION FAILED: '%s'"), pString);
		SystemLog.Printf(_T("     File: '%s'"), pFile);
		SystemLog.Printf(_T("     Func: '%s'"), pFunction);
		SystemLog.Printf(_T("     Line: %ld"), Line);
	}

	/* Display a message box under Borland */
#if _WIN32
	ErrorHandler.Printf(_T("Application Assert!"),
	                    _T("ASSERTION FAILED: '%s'\r\n\tFile: '%s'\r\n\tFunc: '%s'\r\n\tLine: %ld\r\nAborting Program!"),
	                    pString,
	                    pFile,
	                    pFunction,
	                    Line);
#else
	std::fprintf(stderr, _T("ASSERTION FAILED: '%s'\r\n"), pString);
	std::fprintf(stderr, _T("\tFile: '%s'\r\n"), pFile);
	std::fprintf(stderr, _T("\tFunc: '%s'\r\n"), pFunction);
	std::fprintf(stderr, _T("\tLine: %ld\r\n"), Line);
	std::fflush(stderr);
#endif
	/* Abort program */
	std::abort();
}

/*===========================================================================
 *
 * Class CLogFile Constructor (Default)
 *
 *=========================================================================*/
CLogFile::CLogFile() {
	TabLevel = 0;
	pLogFileHandle = NULL;
	pHookProc = NULL;
}


/*===========================================================================
 *
 * Class CLogFile Constructor - CLogFile (pFilename, AppendFile);
 *
 * Attempts to open the given filename as the current logfile.  Accepts
 * input as per the Open() class method.
 *
 *=========================================================================*/
CLogFile::CLogFile(const TCHAR *pFilename, const logmode_t AppendFile) {
	/* Initialize the class members */
	TabLevel = 0;
	pLogFileHandle = NULL;
	pHookProc = NULL;
	/* Attempt to open log file */
	Open(pFilename, AppendFile);
}


/*===========================================================================
 *
 * Class CLogFile Method - bool Close (void);
 *
 * Closes the log file, if currently open.  Returns TRUE on success or FALSE
 * if no log file is currently open.
 *
 *=========================================================================*/
bool CLogFile::Close() {
	//DEFINE_FUNCTION("CLogFile::Close()");
	int Result;

	/* Is the log file currently open? */
	if (!IsOpen()) {
		return false;
	}

	/* Output status messages to log file */
	Printf(_T("Closing Log File..."));
	OutputMemoryStatus();
	/* Close log file and reset parameters */
	Result = std::fclose(pLogFileHandle);
	pLogFileHandle = NULL;
	TabLevel = 0;

	if (Result < 0) {
		return false;
	}

	return true;
}


#if _DEBUG
/*===========================================================================
 *
 * Class CLogFile Method - void DebugPrintf (const TCHAR* pString, ...);
 *
 * Outputs a printf() formatted message to the log file but only in
 * debug builds.  Compiles to a NULL inline function for release builds.
 * Otherwise is identical to the Printf() method.
 *
 *=========================================================================*/
void CLogFile::DebugPrintf(const TCHAR *pString, ...) {
	DEFINE_FUNCTION("CLogFile::DebugPrintf()");
	std::va_list Args;
	/* Ensure valid input */
	ASSERT(pString != NULL);

	/* Ensure the log file is currently open */
	if (!IsOpen()) {
		return;
	}

	/* Print the line to the file */
	va_start(Args, pString);
	PrintLine(pString, Args);
	va_end(Args);
}
#endif  // _DEBUG


/*===========================================================================
 *
 * Class CLogFile Method - void DecrementTabs (void)
 *
 * Decreases the current tab level by one if possible.  The tab level
 * indicates how many tags preceed each line output to the log file,
 * allowing simple formatting of the output for easier viewing.
 *
 * See also:    CLogFile::IncrementTabs();
 *      CLogFile::SetTabLevel(NewTabLevel);
 *
 *=========================================================================*/
void CLogFile::DecrementTabs() {
	if (TabLevel > 0) {
		TabLevel--;
	}
}


/*===========================================================================
 *
 * Class CLogFile Method - void IncrementTabs (void)
 *
 * Increases the current tab level by one if possible.  The tab level
 * indicates how many tags preceed each line output to the log file,
 * allowing simple formatting of the output for easier viewing.
 *
 * See also:    CLogFile::DecrementTags();
 *      CLogFile::SetTabLevel(NewTabLevel);
 *
 *=========================================================================*/
void CLogFile::IncrementTabs() {
	if (TabLevel < LOGFILE_MAX_TABS) {
		TabLevel++;
	}
}


/*===========================================================================
 *
 * Class CLogFile Method - bool Open (pFilename, AppendFile);
 *
 * Attempts to create a log file using the given filename.  Any current
 * log file is closed.  If the AppendFile flag is LOG_APPEND (the default
 * is LOG_OPEN) and the log file currently exists, log entries will be
 * appended to the end of the file.  If AppendFile is FALSE, the log
 * file will be overwritten if it exists (the default).  Returns TRUE
 * if the log file was successfully opened.
 *
 *=========================================================================*/
bool CLogFile::Open(const TCHAR *pFilename, const logmode_t AppendFile) {
	DEFINE_FUNCTION("CLogFile::Open()");
	bool Result;
	/* Check for valid input */
	ASSERT(pFilename != NULL);
	ASSERT(*pFilename != NULL_CHAR);

	/* Ensure the current log file is closed */
	if (IsOpen()) {
		Result = Close();

		if (!Result) {
			return false;
		}
	}

	/* Attempt to open file for output, depending on the append flag */
	pLogFileHandle = TFOPEN(pFilename, AppendFile ? _T("at") : _T("wt"));

	if (pLogFileHandle == NULL) {
		return false;
	}

	/* Output the filename and full date to log file */
	Result = Printf(_T("==================== Opened Logfile %s ================"), pFilename);

	if (!Result) {
		return false;
	}

	return OutputDate();
}


/*===========================================================================
 *
 * Class CLogFile Method - bool OutputDate (void);
 *
 * Outputs the full date and time to the log file.  Returns FALSE on any
 * error.
 *
 *=========================================================================*/
bool CLogFile::OutputDate() {
	//DEFINE_FUNCTION("CLogFile::OutputDate()");
	TCHAR DateString[33] = _T("");
	struct std::tm *pCurrentTime;
	std::time_t Today;

	/* Ensure the log file is currently open */
	if (!IsOpen()) {
		return false;
	}

	/* Get the current Date and convert it to local time */
	std::time(&Today);
	pCurrentTime = std::localtime(&Today);

	if (pCurrentTime == NULL) {
		return false;
	}

	/* Output the date line to the file */
	TSTRFTIME(DateString, 32, _T("%A, %d of %B, %Y"), pCurrentTime);
	return Printf(_T("The current date is %s."), DateString);
}


/*===========================================================================
 *
 * Class CLogFile Method - bool OutputCurrentTime (void)
 *
 * This protected class method outputs the current time to the log file
 * in the format:  "HH:MM:SS - "
 * Assumes that the log file is currently open and valid.
 *
 *=========================================================================*/
bool CLogFile::OutputCurrentTime() {
	DEFINE_FUNCTION("CLogFile::OutputCurrentTime()()");
	TCHAR TimeString[17] = _T("");
	int Result;
	std::time_t CurrentTime;
	struct std::tm *pToday;
	/* Ensure a valid log file handle */
	ASSERT(pLogFileHandle != NULL);
	/* Get the current local time and ensure its valid */
	std::time(&CurrentTime);
	pToday = std::localtime(&CurrentTime);

	if (pToday == NULL) {
		return false;
	}

	/* Output the formatted time to log file */
	TSTRFTIME(TimeString, 16, _T("%H:%M:%S"), pToday);
	Result = TFPRINTF(pLogFileHandle, _T("%s (%ld) - "), TimeString, clock());

	if (Result < 0) {
		return false;
	}

	return true;
}


/*===========================================================================
 *
 * Class CLogFile Method - void OutputMemoryStatus (void);
 *
 * Outputs the current memory status to the log file.
 *
 *=========================================================================*/
void CLogFile::OutputMemoryStatus() {
	//DEFINE_FUNCTION("CLogFile::OutputMemoryStatus()");
	bool MemResult;
	long UsedMemory = 0;
	long FreeMemory = 0;
	long TotalMemory = 0;
	/* Attempt to retrieve current memory usage */
	MemResult = GetUsedMemory(UsedMemory);

	if (MemResult) {
		MemResult = GetFreeMemory(FreeMemory);
	}

	if (MemResult) {
		MemResult = GetTotalMemory(TotalMemory);
	}

	if (MemResult) {
		Printf(_T("Used/Free/Total Memory: %lu/%lu/%lu bytes"),
		       UsedMemory,
		       FreeMemory,
		       TotalMemory);
	} else {
		Printf(_T("Error attempting to retrieve memory usage!"));
	}

	/* Output heap information */
#if _DEBUG
	DebugPrintf(_T("DebugHeapCheckMemory() returns %s"),
	            DebugHeapCheckMemory() ? _T("TRUE") : _T("FALSE"));
#endif
	Printf(_T("Heap Status: %s"), GetHeapStatusString());
	/* Output custom heap information */
#if _DEBUG
	OutputBlockInfo();
#endif
}


/*===========================================================================
 *
 * Class CLogFile Method - bool OutputTabs (void);
 *
 * This protected class method outputs all the tabs required by the
 * current TabLevel to the log file.  Assumes that the log file is currently
 * open and valid.  Returns FALSE on any error.
 *
 *=========================================================================*/
bool CLogFile::OutputTabs() {
	DEFINE_FUNCTION("CLogFile::OutputTabs()");
	int LoopCounter;
	int Result;
	/* Ensure a valid log file handle */
	ASSERT(pLogFileHandle != NULL);

	/* Output the tabs, if any */
	for (LoopCounter = 0; LoopCounter < TabLevel; LoopCounter++) {
		Result = TFPUTC((TCHAR)'\t', pLogFileHandle);

		if (Result == EOF) {
			return false;
		}
	}

	return true;
}


/*===========================================================================
 *
 * Class CLogFile Method - bool Printf (pString, ...);
 *
 * Outputs a log file entry in the usual printf() format.  Returns TRUE
 * on success, or FALSE on any error.
 *
 *=========================================================================*/
bool CLogFile::Printf(const TCHAR *pString, ...) {
	DEFINE_FUNCTION("CLogFile::Printf(TCHAR*)");
	std::va_list Args;
	bool Result;
	/* Ensure valid input */
	ASSERT(pString != NULL);

	/* Ensure the log file is currently open */
	if (!IsOpen()) {
		return false;
	}

	/* Print the line to the file */
	va_start(Args, pString);
	Result = PrintLine(pString, Args);
	va_end(Args);
	return Result;
}


/*===========================================================================
 *
 * Class CLogFile Method - bool Printf (pFileHandle, pString, ...);
 *
 * Outputs a log file entry in the usual printf() format to the log file as well
 * as the given file stream.  Returns TRUE on success, or FALSE on any error.
 * Ignores the file stream if its NULL.  Outputs to the input stream
 * even if the log file is closed.
 *
 *=========================================================================*/
bool CLogFile::Printf(std::FILE *pFileHandle, const TCHAR *pString, ...) {
	DEFINE_FUNCTION("CLogFile::Printf(FILE*, TCHAR*)");
	int Result;
	int LoopCounter;
	std::va_list Args;
	/* Ensure valid input */
	ASSERT(pString != NULL);
	va_start(Args, pString);

	/* Output line header to log file if open */
	if (IsOpen()) {
		Result = PrintLine(pString, Args);

		if (!Result) {
			va_end(Args);
			return false;
		}
	}

	/* Output result to file stream if required */
	if (pFileHandle != NULL) {
		/* Output the tabs, if any */
		for (LoopCounter = 0; LoopCounter < TabLevel; LoopCounter++) {
			Result = TFPUTC((TCHAR)'\t', pFileHandle);

			if (Result == EOF) {
				return false;
			}
		}

		Result = TVFPRINTF(pFileHandle, pString, Args);
		va_end(Args);

		if (Result < 0) {
			return false;
		}

		/* Terminate line with a line feed character */
		Result = TFPRINTF(pFileHandle, _T("\n"));

		if (Result < 0) {
			return false;
		}

		/* Flush output stream */
		Result = std::fflush(pFileHandle);

		if (Result == EOF) {
			return false;
		}
	}

	return true;
}


/*===========================================================================
 *
 * Class CLogFile Method - bool PrintLine (pString, Args);
 *
 * Outputs a line to the log file using the format string and variable
 * arguments.  Returns FALSE on any error.  Assumes that the log file
 * is currently open.  Protected class method.
 *
 *=========================================================================*/
bool CLogFile::PrintLine(const TCHAR *pString, std::va_list Args) {
	DEFINE_FUNCTION("CLogFile::PrintLine()");
	int Result;
	ASSERT(pString != NULL);
	ASSERT(pLogFileHandle != NULL);
	/* Get the current time and write to file as the line header */
	Result = OutputCurrentTime();

	if (!Result) {
		return false;
	}

	/* Output the tabs, if any */
	Result = OutputTabs();

	if (!Result) {
		return false;
	}

	/* Print the variable argument list to the file */
	Result = TVFPRINTF(pLogFileHandle, pString, Args);

	if (Result < 0) {
		return false;
	}

	/* Output to the optional hook procedure */
	if (pHookProc != NULL) {
		pHookProc(pString, Args);
	}

	/* A linefeed to terminate the current line */
	Result = TFPRINTF(pLogFileHandle, _T("\n"));

	if (Result < 0) {
		return false;
	}

	/* Flush the file stream to make sure the written characters are written
	 * to the file. This is in case the program crashes and isn't closed. If
	 * this did happen without the following line, any buffered log file
	 * data might not be output. */
	Result = std::fflush(pLogFileHandle);

	if (Result == EOF) {
		return false;
	}

	return true;
}


/*===========================================================================
 *
 * Class CLogFile Method - void SetTabLevel (NewTabLevel);
 *
 * Sets the current tab level to the given value, if valid.  The tab level
 * indicates how many tags preceed each line output to the log file,
 * allowing simple formatting of the output for easier viewing.  Valid
 * input is from 0 (no tabs) to LOGFILE_MAX_TABS.
 *
 * See also:    CLogFile::IncrementTags();
 *      CLogFile::DecrementTags();
 *
 *=========================================================================*/
void CLogFile::SetTabLevel(const int NewTabLevel) {
	/* Ensure a valid input */
	if (NewTabLevel < 0 || NewTabLevel > LOGFILE_MAX_TABS) {
		return;
	}

	TabLevel = NewTabLevel;
}


/*===========================================================================
 *
 * Begin Module Test Routines
 *
 *=========================================================================*/
#if _DEBUG
/*===========================================================================
 *
 * Function - void Test_LogHook (pString, Args);
 *
 * Tests the callback functionality of the CLogFile class.
 *
 *=========================================================================*/
void Test_LogHook(const TCHAR *pString, std::va_list Args) {
	//DEFINE_FUNCTION("Test_LogHook()");
	TPRINTF(_T("Test_LogHook called...\n"));
	TVPRINTF(pString, Args);
	TPRINTF(_T("\n"));
}


/*===========================================================================
 *
 * Function - void Test_LogFile (void);
 *
 * Tests the CLogFile class.  Only defined in debug builds.
 *  - Checks constructors
 *  - Standard open method
 *  - Appending log files
 *  - Open an invalid file (empty string and NULL)
 *  - Open a log file without closing the current one.
 *  - Tab level increments and decrements
 *  - SetTabLevel() method with random values (-100 to 100)
 *  - Checks the hook procedure
 *  - Test split output to file stream
 *  - Tests DebugPrintf() method.
 *
 *=========================================================================*/
void Test_LogFile() {
	DEFINE_FUNCTION("Test_LogFile()");
	CLogFile TestLog1(_T("test1.log")); /* Check constructors */
	CLogFile TestLog2;
	CLogFile TestLog3;
	int LoopCounter;
	int TabLevel;

	/* Check standard open method */
	TestLog2.Open(_T("test2.log"));

	/* Test appending logs */
	TestLog3.Open(_T("test3.log"), LOG_OPEN);
	TestLog3.Printf(_T("Test1"));
	TestLog3.Close();
	TestLog3.Open(_T("test3.log"), LOG_APPEND);
	TestLog3.Printf(_T("Append Test1"));

	/* Attempt to open an invalid file (both should ASSERT) */
	//TestLog2.Open("");
	//TestLog2.Open(NULL);
	/* Open file without closing the current log */
	TestLog1.Open(_T("test1a.log"));

	/* Check tab level incrementation */
	for (LoopCounter = 0; LoopCounter < 100; LoopCounter++) {
		TestLog1.IncrementTabs();
		TestLog1.Printf(_T("TabLevel = %d"), LoopCounter);
	}

	/* Check tab level decrementation */
	for (LoopCounter = 0; LoopCounter < 110; LoopCounter++) {
		TestLog1.DecrementTabs();
		TestLog1.Printf(_T("TabLevel = %d"), LoopCounter);
	}

	/* Check random tab levels with SetTabLevel() method */
	for (LoopCounter = 0; LoopCounter < 100; LoopCounter++) {
		TabLevel = (int)((float)std::rand() * 201 / RAND_MAX) - 100;
		TestLog1.SetTabLevel(TabLevel);
		TestLog1.Printf(_T("SetTabLevel = %d"), TabLevel);
	}

	/* Reset the tab level for the log */
	TestLog1.SetTabLevel();

	/* Check the hook procedures */
	TestLog1.SetHookProc(Test_LogHook);
	TestLog1.Printf(_T("Testing hook proc..."));
	TestLog1.SetHookProc();
	TestLog1.Printf(_T("Removed hook proc..."));

	/* Test splitting output to file stream */
	TestLog1.Printf(TestLog2.GetFileHandle(), _T("Testing split output to file stream..."));
	TestLog1.Printf(stdout, _T("Testing split output to stdout..."));

	/* Test debug output */
	TestLog1.DebugPrintf(_T("Testing debug print...%d, %s"), 1001, _T("adedr"));
	ASSERT(DebugHeapCheckMemory());
}


#endif  // _DEBUG
