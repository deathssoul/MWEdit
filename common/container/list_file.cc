/*===========================================================================
 *
 * File:    ListFile.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Thursday, March 29, 2001
 *
 * Implements the CListFile class for handling files containing a list of
 * items, one per line.  The file is kept open and the lines input one by
 * one, as required, which reduces the overhead of having to store and
 * manage the whole list.
 *
 *=========================================================================*/
#include "common/container/list_file.h"

#include <cstddef>
#include <cstdio>

#include "common/dl_base.h"
#include "common/dl_file.h"
#include "common/dl_log.h"
#include "common/dl_mem.h"

#if _DEBUG
#include <cstring>
#endif  // _DEBUG
/*===========================================================================
 *
 * Class CListFile Constructor
 *
 * Class constructor which sets the maximum line length input from the file.
 * If an invalid line length is given (or no value), the default value
 * LISTFILE_LINE_LENGTH is used.
 *
 *=========================================================================*/
CListFile::CListFile(const std::size_t LineLength) {
	//DEFINE_FUNCTION("CListFile::CListFile()");
	/* Initialize the class members */
	MaxLineLength = LineLength;
	pCurrentLine = NULL;
	pFileHandle = NULL;
	BufferValid = false;
	IgnoreBlankLines = false;

	/* Ensure a valid line length */
	if (MaxLineLength <= 1) {
		MaxLineLength = LISTFILE_LINE_LENGTH;
	}

	/* Allocate the line buffer */
	pCurrentLine = CreateString(MaxLineLength);
	*pCurrentLine = NULL_CHAR;
}


/*===========================================================================
 *
 * Class CListFile Destructor
 *
 *=========================================================================*/
CListFile::~CListFile() {
	//DEFINE_FUNCTION("CListFile::~CListFile()");
	/* Close the file if required */
	Close();
	/* Unallocate the line buffer */
	DestroyPointer(pCurrentLine);
}


/*===========================================================================
 *
 * Class CListFile Method - void Destroy (void);
 *
 * Clears the contents of the object.
 *
 *=========================================================================*/
void CListFile::Destroy() {
	//DEFINE_FUNCTION("CListFile::Destroy()");
	/* Clear the line buffer */
	*pCurrentLine = NULL_CHAR;
	BufferValid = false;
	IgnoreBlankLines = false;
	/* Close the file if required */
	Close();
}


/*===========================================================================
 *
 * Class CListFile Method - void Close (void);
 *
 * Closes the current list file if it is open.
 *
 *=========================================================================*/
void CListFile::Close() {
	//DEFINE_FUNCTION("CListFile::Close()");

	/* Don't close the file unless it is valid */
	if (pFileHandle != NULL) {
		std::fclose(pFileHandle);
		pFileHandle = NULL;
		*pCurrentLine = NULL_CHAR;
		BufferValid = false;
	}
}


/*===========================================================================
 *
 * Class CListFile Method - boolean Open (const char* pFilename)
 *
 * Opens and starts parsing a list file.  Returns FALSE on any error.
 * ASSERTs if passed an invalid filename. If a file is currently open,
 * it is closed.
 *
 *=========================================================================*/
bool CListFile::Open(const char *pFilename) {
	DEFINE_FUNCTION("CListFile::Open()");
	/* Ensure valid input */
	ASSERT(pFilename != NULL);

	/* Ensure the current file is closed */
	if (IsOpen()) {
		Close();
	}

	/* Attempt to open file for input */
	pFileHandle = OpenFile(pFilename, "rb");

	if (pFileHandle == NULL) {
		return false;
	}

	/* Attempt to read the first line from file */
	return ReadNextLine();
}


/*===========================================================================
 *
 * Class CListFile Method - boolean ReadNextLine (void)
 *
 * Attempts to read the next line from the file, returning FALSE on any
 * error.  If the line is longer than the maximum line buffer size,
 * the extra characters on the line are ignored.
 *
 *=========================================================================*/
bool CListFile::ReadNextLine() {
	DEFINE_FUNCTION("CListFile::ReadNextLine()");
	int Result;
	/* Ensure the list file is currently open */
	BufferValid = false;

	if (!IsOpen()) {
		return false;
	}

	/* Attempt to input one line from file */
	Result = ReadLine(pFileHandle, pCurrentLine, MaxLineLength);

	/* Check for error conditions */
	switch (Result) {
		case READLINE_MSL:
			ReadLine(pFileHandle);
			break;

		case READLINE_ERROR:
			return false;

		case READLINE_EOF:
			if (IgnoreBlankLines && *pCurrentLine == NULL_CHAR) {
				BufferValid = false;
			} else {
				BufferValid = true;
			}

			return true;

		default:
			if (IgnoreBlankLines && *pCurrentLine == NULL_CHAR) {
				return ReadNextLine();
			}

			break;
	}

	BufferValid = true;
	return true;
}


#if _DEBUG
/*===========================================================================
 *
 * Function - void Test_ListFile (void);
 *
 * Tests all the required methods in this module.
 *  1. Tests construction of the CListFile class
 *  2. Tests the Open() method with valid/invalid filenames
 *  3. Test the IsValidLine() method
 *  4. Test the GetCurrentLine() method (ensuring correct input)
 *  5. Test the ReadNextLine() method
 *  6. Test reopening an open file
 *  7. Test the Close() method
 *  8. Test the IsOpen() method
 *  9. Test the autodestruction of the class objects
 *
 *=========================================================================*/
void Test_ListFile() {
	DEFINE_FUNCTION("Test_ListFile()");
	SystemLog.Printf(/*std::*/stdout, "================= Testing CListFile =========================");

	/* Test the CListFile construction */
	CListFile TestFile1;
	CListFile TestFile2(10);
	CListFile TestFile3(0);
	ASSERT(TestFile1.GetMaxLineLength() == LISTFILE_LINE_LENGTH);
	ASSERT(TestFile2.GetMaxLineLength() == 10);
	ASSERT(TestFile3.GetMaxLineLength() == LISTFILE_LINE_LENGTH);

	/* Test the IsValidLine() method */
	ASSERT(TestFile1.IsValidLine() == false);
	ASSERT(TestFile2.IsValidLine() == false);
	ASSERT(TestFile3.IsValidLine() == false);

	/* Test the Open() method */
	ASSERT(TestFile1.Open("test1.lst") == true);
	ASSERT(TestFile2.Open("test2.lst") == true);
	ASSERT(TestFile3.Open("nofile.lst") == false);

	/* Test the IsValidLine() method again */
	ASSERT(TestFile1.IsValidLine() == true);
	ASSERT(TestFile2.IsValidLine() == true);
	ASSERT(TestFile3.IsValidLine() == false);

	/* Test the GetCurrentLine() method */
	ASSERT(std::strcmp(TestFile1.GetCurrentLine(), "Test1, Line1") == 0);
	ASSERT(std::strcmp(TestFile2.GetCurrentLine(), "Test2, Lin") == 0);
	ASSERT(std::strcmp(TestFile3.GetCurrentLine(), "") == 0);

	/* Output the rest of the TestFile1 file to the SystemLog */
	do {
		ASSERT(TestFile1.IsValidLine() == true);
		SystemLog.Printf("TestFile1:%s", TestFile1.GetCurrentLine());
	} while (TestFile1.ReadNextLine());

	ASSERT(TestFile1.IsValidLine() == false);

	/* Test reopening an open file */
	ASSERT(TestFile1.Open("test1.lst") == true);
	ASSERT(TestFile1.IsValidLine() == true);
	ASSERT(std::strcmp(TestFile1.GetCurrentLine(), "Test1, Line1") == 0);

	/* Test the Close() and IsOpen() methods */
	ASSERT(TestFile1.IsOpen() == true);
	TestFile1.Close();
	ASSERT(TestFile1.IsOpen() == false);
	ASSERT(TestFile1.IsValidLine() == false);
}
#endif  // _DEBUG
