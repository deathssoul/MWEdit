/*===========================================================================
 *
 * File:    GenFile.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Thursday, May 17, 2001
 *
 * Implements the CGenFile class which handles simple file stream I/O
 * operations.
 *
 *=========================================================================*/
#include "common/file/gen_file.h"

#include <cerrno>
#include <climits>
#include <cstdarg>
#include <cstddef>
#include <cstdio>

#include "common/dl_base.h"
#include "common/dl_err.h"
#include "common/dl_file.h"

#if _DEBUG
#include "common/dl_log.h"
#include "common/dl_math.h"
#include "common/dl_mem.h"
#endif

DEFINE_FILE("GenFile.cpp");

/* Class constructors */
CGenFile::CGenFile() {
	m_pFileHandle = NULL;
	m_Attached = FALSE;
}

CGenFile::CGenFile(std::FILE *pSourceHandle) {
	m_pFileHandle = pSourceHandle;
	m_Attached = FALSE;
}

CGenFile::CGenFile(const char *pFilename, const char *pMode) {
	m_pFileHandle = NULL;
	m_Attached = FALSE;
	m_LineCount = 0;
	Open(pFilename, pMode);
}

/* Class destructor */
CGenFile::~CGenFile() {
	Destroy();
}

/* Class pseudo-destructor */
void CGenFile::Destroy () {
	Close();
}

/* Attach to an existing file stream */
void CGenFile::Attach(std::FILE *pFileHandle) {
	IASSERT(pFileHandle != NULL);
	Close();
	m_Attached = TRUE;
	m_pFileHandle = pFileHandle;
}

/* Unattached to an existing file stream */
void CGenFile::Detach() {
	if (m_Attached) {
		m_Attached = FALSE;
		m_pFileHandle = NULL;
	}
}

/* Clears the error flag of the file stream */
void CGenFile::ClearError() {
	IASSERT(m_pFileHandle != NULL);
	std::clearerr(m_pFileHandle);
}

/* Close the file stream, if it is open */
void CGenFile::Close() {
	if (IsOpen()) {
		if (!m_Attached) {
			std::fclose(m_pFileHandle);
		}

		m_pFileHandle = NULL;
	}

	m_Attached = FALSE;
}

/* Creates a new temporary file for output */
bool CGenFile::CreateTemp() {
	Close();
	m_pFileHandle = std::tmpfile();

	if (m_pFileHandle == NULL) {
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Failed to create temporary file!");
		return FALSE;
	}

	return TRUE;
}

bool CGenFile::Flush() {
	IASSERT(IsOpen());

	if (std::fflush(m_pFileHandle) != 0) {
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Failed to flush file stream!");
		return FALSE;
	}

	return TRUE;
}

/* Return the file handle for object */
std::FILE *CGenFile::GetHandle() {
	return m_pFileHandle;
}

/* Return the file size */
long CGenFile::GetFileSize() {
	IASSERT(IsOpen());
	return ::GetFileSize(m_pFileHandle);
}

/* Retrieve the file size with error status */
bool CGenFile::GetFileSize(long &FileSize) {
	IASSERT(IsOpen());
	return ::GetFileSize(FileSize, m_pFileHandle);
}

/* Returns TRUE if the EOF has been reached */
bool CGenFile::IsEOF() {
	if (!IsOpen() || std::feof(m_pFileHandle)) {
		return TRUE;
	}

	return FALSE;
}

/* Returns TRUE if the file stream has an error */
bool CGenFile::IsError() {
	if (!IsOpen() || std::ferror(m_pFileHandle)) {
		return TRUE;
	}

	return FALSE;
}

/* Returns TRUE if the file stream is currently open */
bool CGenFile::IsOpen() {
	return (bool)((m_pFileHandle == NULL) ? FALSE : TRUE);
}

/* Open a file as with the fopen() function */
bool CGenFile::Open(const char *pFilename, const char *pMode) {
	Close();
	m_LineCount = 0;
	return OpenFile(&m_pFileHandle, pFilename, pMode);
}

/* Output a formatted string to file */
bool CGenFile::Printf(const char *pString, ...) {
	std::va_list Args;
	bool Result;
	IASSERT(pString != NULL);
	std::va_start(Args, pString);
	Result = VPrintf(pString, Args);
	std::va_end(Args);
	return Result;
}

/* Output a formatted string to file using a variable argument list */
bool CGenFile::VPrintf(const char *pString, std::va_list Args) {
	IASSERT(pString != NULL);

	/* Ensure file is open for output */
	if (!IsOpen()) {
		ErrorHandler.AddError(ERR_WRITEFILE, "File is not open!");
		return FALSE;
	}

	/* Output formatted string to file */
	if (std::vfprintf(m_pFileHandle, pString, Args) < 0) {
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Error outputting a formatted string to file!");
		return FALSE;
	}

	return Flush();
}

/* Read a section from the file */
bool CGenFile::Read(char *pBuffer, std::size_t &BytesRead, const std::size_t NumBytes) {
	IASSERT(pBuffer != NULL && IsOpen());
	/* Attempt to read buffer */
	BytesRead = std::fread(pBuffer, 1, NumBytes, m_pFileHandle);

	if (BytesRead != NumBytes) {
		ErrorHandler.AddError(ERR_SYSTEM,
		                      errno,
		                      "Failed to read section from file, only %u of %u bytes received!",
		                      BytesRead,
		                      NumBytes);
		return FALSE;
	}

	return TRUE;
}

/* Read a section from the file */
bool CGenFile::Read(char *pBuffer, const std::size_t NumBytes) {
	std::size_t BytesRead;
	return Read(pBuffer, BytesRead, NumBytes);
}

/* Read a section from the file */
bool CGenFile::ReadEx(char *pBuffer, const std::size_t Size, const std::size_t Count) {
	ulong NumBytes = (ulong)Size * (ulong)Count;
	std::size_t BytesRead;

	/* Ensure valid input size */
	if (NumBytes >= (ulong)UINT_MAX) {
		ErrorHandler.AddError(ERR_OVERFLOW,
		                      "Exceeded the maximum input size of %ud bytes!",
		                      UINT_MAX);
		return FALSE;
	}

	return Read(pBuffer, BytesRead, (std::size_t)NumBytes);
}

/* Read a character from the file */
bool CGenFile::ReadChar(char &InputChar) {
	IASSERT(IsOpen());
	int Input;
	/* Attempt to read character */
	Input = std::fgetc(m_pFileHandle);

	if (Input < 0) {
		InputChar = NULL_CHAR;
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Failed to read character from file!");
		return FALSE;
	}

	InputChar = (char)Input;
	return TRUE;
}

/* Read one line from the file */
int CGenFile::ReadLine(char *pBuffer, const std::size_t MaxStringLength) {
	IASSERT(IsOpen());
	return ::ReadLine(m_pFileHandle, pBuffer, MaxStringLength);
}

/* Read binary short integer (16 bit) */
bool CGenFile::ReadShort(short &Value) {
	return Read((char *)&Value, sizeof(short));
}

/* Read binary integer (size depends on system) */
bool CGenFile::ReadInt(int &Value) {
	return Read((char *)&Value, sizeof(int));
}

/* Read binary long integer (32 bit) */
bool CGenFile::ReadLong(long &Value) {
	return Read((char *)&Value, sizeof(long));
}

/* Read binary float (32 bit) */
bool CGenFile::ReadFloat(float &Value) {
	return Read((char *)&Value, sizeof(float));
}

/* Move to the file beginning and clear the stream errors */
void CGenFile::Rewind() {
	IASSERT(IsOpen());
	std::rewind(m_pFileHandle);
}

/* Move the current file position as per fseek() */
bool CGenFile::Seek(const filepos_t Position, const int SeekType) {
	IASSERT(IsOpen());

	if (std::fseek(m_pFileHandle, Position, SeekType) != 0) {
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Failed to change the current file position!");
		return FALSE;
	}

	return TRUE;
}

/* Retrieve the current file position as per ftell() */
bool CGenFile::Tell(filepos_t &Position) {
	IASSERT(IsOpen());
	Position = std::ftell(m_pFileHandle);

	if (Position < 0) {
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Failed to retrieve the current file position!");
		return FALSE;
	}

	return TRUE;
}

/* Retrieve the current file position as per ftell() */
filepos_t CGenFile::Tell() {
	filepos_t Position;
	Tell(Position);
	return Position;
}

/* Write a section to the file */
bool CGenFile::Write(const char *pBuffer, std::size_t &BytesWritten, const std::size_t NumBytes) {
	IASSERT(pBuffer != NULL && IsOpen());
	/* Attempt to read buffer */
	BytesWritten = std::fwrite(pBuffer, 1, NumBytes, m_pFileHandle);

	if (BytesWritten != NumBytes) {
		ErrorHandler.AddError(ERR_SYSTEM,
		                      errno,
		                      "Failed to write section to file, only %u of %u bytes output!",
		                      BytesWritten,
		                      NumBytes);
		return FALSE;
	}

	return TRUE;
}

/* Write a section to the file */
bool CGenFile::Write(const char *pBuffer, const std::size_t NumBytes) {
	std::size_t BytesWritten;
	return Write(pBuffer, BytesWritten, NumBytes);
}

/* Write a section to the file */
bool CGenFile::WriteEx(const char *pBuffer, const std::size_t Size, const std::size_t Count) {
	ulong NumBytes = (ulong)Size * (ulong)Count;
	std::size_t BytesWritten;

	/* Ensure valid input size */
	if (NumBytes >= (ulong)UINT_MAX) {
		ErrorHandler.AddError(ERR_OVERFLOW,
		                      "Exceeded the maximum output size of %ud bytes!",
		                      UINT_MAX);
		return FALSE;
	}

	return Write(pBuffer, BytesWritten, (std::size_t)NumBytes);
}

/* Write a character to the file */
bool CGenFile::WriteChar(const char Char) {
	IASSERT(IsOpen());

	/* Attempt to write character */
	if (std::fputc((int)Char, m_pFileHandle) < 0 ) {
		ErrorHandler.AddError(ERR_SYSTEM, errno, "Failed to write character to file!");
		return FALSE;
	}

	return TRUE;
}

/* Write binary short integer (16 bit) */
bool CGenFile::WriteShort(const short Value) {
	short TempValue = Value;
	return Write((char *)&TempValue, sizeof(short));
}

/* Write binary integer (size depends on system) */
bool CGenFile::WriteInt(const int Value) {
	int TempValue = Value;
	return Write((char *)&TempValue, sizeof(int));
}

/* Write binary long integer (32 bit) */
bool CGenFile::WriteLong(const long Value) {
	long TempValue = Value;
	return Write((char *)&TempValue, sizeof(long));
}

/* Write binary float (32 bit) */
bool CGenFile::WriteFloat(const float Value) {
	float TempValue = Value;
	return Write((char *)&TempValue, sizeof(float));
}

/* Class FILE* type conversion operator */
CGenFile::operator std::FILE *(void) {
	return m_pFileHandle;
}

/*===========================================================================
 *
 * Begin Module Test Routines
 *
 *=========================================================================*/
#if _DEBUG
/*===========================================================================
 *
 * Function - void Test_GenFileOpen (void);
 *
 * Tests the Open() method.
 *  1. Test the constructor open version with valid/invalid inputs
 *  2. Test the Open() method with a variety of inputs
 *  3. Also indirectly tests the Close() method and destructor
 *
 *=========================================================================*/
void Test_GenFileOpen() {
	DEFINE_FUNCTION("Test_GenFileOpen()");
	SystemLog.Printf(stdout, "================= Testing Open Method of CGenFile ================");
	Test_CreateRandomFile("c:\\temp\\test1.dat", 10, FILE_BINARY);

	/* Test the constructor open version */
	CGenFile File1("c:\\temp\\test1.dat", "rb");
	CGenFile File2("c:\\tempxyt\\test1.dat", "rb");
	CGenFile File3("", "wb");
	CGenFile File4("c:\\temp\\test2.dat", "wb");
	CGenFile File5("c:\\temp\\t- =asasd&*!@#()$*.. . . .est2.dat", "wb");
	CGenFile File6("c:\\temp\\test1.dat", "");

	ASSERT(File1.IsOpen() == TRUE);
	ASSERT(File2.IsOpen() == FALSE);
	ASSERT(File3.IsOpen() == FALSE);
	ASSERT(File4.IsOpen() == TRUE);
	ASSERT(File5.IsOpen() == FALSE);
	ASSERT(File6.IsOpen() == FALSE);

	/* Test the open method with a variety of inputs */
	ASSERT(File1.Open("c:\\temp\\test1.dat", "rb") == TRUE);
	ASSERT(File2.Open("c:\\tempxyt\\test1.dat", "rb") == FALSE);
	ASSERT(File3.Open("", "wb") == FALSE);
	ASSERT(File4.Open("c:\\temp\\test2.dat", "wb") == TRUE);
	ASSERT(File5.Open("c:\\temp\\t- =asasd&*!@#()$*.. . . .est2.dat", "wb") == FALSE);
	ASSERT(File6.Open("c:\\temp\\test1.dat", "") == FALSE);
}


/*===========================================================================
 *
 * Function - void Test_GenFilePrint (void);
 *
 * Tests the Printf() and VPrintf() methods.
 *  1. Test with typical inputs
 *
 *=========================================================================*/
void Test_GenFilePrint() {
	DEFINE_FUNCTION("Test_GenFilePrint()");
	CGenFile File1("c:\\temp\\test1.dat", "wt");
	SystemLog.Printf(stdout,
	                 "================= Testing Printf/VPrintf Methods of CGenFile ================");
	ASSERT(File1.IsOpen());

	/* Test with typical inputs */
	ASSERT(File1.Printf("Testing with no vars") == TRUE);
	ASSERT(File1.Printf("Next string is blank") == TRUE);
	ASSERT(File1.Printf("") == TRUE);
	ASSERT(File1.Printf("Testing with one vars, 101 = %d", 101) == TRUE);
	ASSERT(File1.Printf("Testing with two vars, 123 = %s, 2.222e22=%f", "123", 2.222e22) == TRUE);
	ASSERT(File1.IsEOF() == FALSE);
	ASSERT(File1.IsError() == FALSE);
}


/*===========================================================================
 *
 * Function - void Test_GenFileSeekTell (NumTests);
 *
 * Repeatedly tests the Seek/Tell/Rewind methods.
 *  1. Randomly seeks to a valid file position
 *  2. Ensure the seek/tell positions match
 *  3. Attempt to seek to an invalid position
 *  4. Ensure the position has not changed
 *  5. Test the Rewind() method and ensure Tell returns 0
 *
 *=========================================================================*/
void Test_GenFileSeekTell(const std::size_t NumTests) {
	DEFINE_FUNCTION("Test_GenFileSeekTell()");
	std::size_t LoopCounter;
	filepos_t FilePos;
	filepos_t FilePos1;
	filepos_t TellPos;
	CGenFile TestFile;
	SystemLog.Printf(stdout,
	                 "================= Testing Seek/Tell/Rewind Method of CGenFile ================");
	Test_CreateRandomFile("c:\\temp\\test1.dat", TEST_GENFILE_SEEKFILESIZE, FILE_BINARY);
	ASSERT(TestFile.Open("c:\\temp\\test1.dat", "rb") == TRUE);
	RandomizeTimer();

	/* Repeat each test a number of times */
	for (LoopCounter = 0; LoopCounter < NumTests; LoopCounter++) {
		SystemLog.Printf(stdout, "\t%u of %u Tell/Seek/Rewind test", LoopCounter + 1, NumTests);
		FilePos = (filepos_t)Random(TEST_GENFILE_SEEKFILESIZE);
		FilePos1 = FilePos - TEST_GENFILE_SEEKFILESIZE / 2;

		/* Seek to valid file positions and test with Tell */
		ASSERT(TestFile.Seek(-FilePos, SEEK_END) == TRUE);
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TellPos == TEST_GENFILE_SEEKFILESIZE - FilePos);
		ASSERT(TestFile.Seek(TEST_GENFILE_SEEKFILESIZE / 2, SEEK_SET) == TRUE);
		ASSERT(TestFile.Seek(FilePos1, SEEK_CUR) == TRUE);
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TellPos == TEST_GENFILE_SEEKFILESIZE / 2 + FilePos1);
		ASSERT(TestFile.Seek(FilePos, SEEK_SET) == TRUE);
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TellPos == FilePos);
		ASSERT(TestFile.IsEOF() == FALSE);
		ASSERT(TestFile.IsError() == FALSE);

		/* Attempt seeking to invalid positions */
		ASSERT(TestFile.Seek(-1, SEEK_SET) == FALSE);
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TellPos == FilePos);
		ASSERT(TestFile.Seek(TEST_GENFILE_SEEKFILESIZE * 2, SEEK_CUR) == TRUE);
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TestFile.IsEOF() == FALSE);
		ASSERT(TestFile.IsError() == FALSE);
		ASSERT(TestFile.Seek(10000, SEEK_END) == TRUE);
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TestFile.IsEOF() == FALSE);
		ASSERT(TestFile.IsError() == FALSE);
		ASSERT(TellPos == TEST_GENFILE_SEEKFILESIZE + 10000);

		/* Test the rewind method */
		TestFile.Rewind();
		ASSERT(TestFile.Tell(TellPos) == TRUE);
		ASSERT(TellPos == 0);
		ASSERT(TestFile.IsError() == FALSE);
		ASSERT(TestFile.IsEOF() == FALSE);
	}
}


/*===========================================================================
 *
 * Function - void Test_RWGenFile (NumTests);
 *
 * Repeatedly tests the Read/Write routines on randomly sized files.
 *
 *=========================================================================*/
void Test_RWGenFile(const std::size_t NumTests) {
	DEFINE_FUNCTION("Test_RWGenFile()");
	CGenFile InputFile;
	CGenFile OutputFile;
	std::size_t FileSize;
	std::size_t LoopCounter;
	std::size_t BytesIO;
	char *pBuffer;
	char IOChar;
	SystemLog.Printf(stdout,
	                 "================= Testing Read/Write Methods of CGenFile ================");
	RandomizeTimer();
	pBuffer = CreateString(TEST_RWFILE_MAXFILESIZE);

	/* Perform each test a number of times */
	for (LoopCounter = 0; LoopCounter < NumTests; LoopCounter++) {
		/* Create randomly sized file */
		FileSize = Random((int)TEST_RWFILE_MAXFILESIZE);
		SystemLog.Printf(stdout, "\t%u) Testing file with size %u...", LoopCounter + 1, FileSize);
		Test_CreateRandomFile ("c:\\temp\\test1.dat", FileSize, FILE_BINARY);

		/* Attempt to open and read file */
		ASSERT(InputFile.Open("c:\\temp\\test1.dat", "rb") == TRUE);
		ASSERT(InputFile.Read(pBuffer, BytesIO, FileSize) == TRUE);
		ASSERT(BytesIO == FileSize);
		ASSERT(InputFile.IsError() == FALSE);
		ASSERT(InputFile.IsEOF() == FALSE);
		ASSERT(InputFile.ReadChar(IOChar) == FALSE);
		ASSERT(InputFile.IsEOF() == TRUE);
		ASSERT(InputFile.IsError() == FALSE);
		InputFile.Close();

		/* Attempt to open and write file */
		ASSERT(OutputFile.Open("c:\\temp\\test2.dat", "wb") == TRUE);
		ASSERT(OutputFile.Write(pBuffer, BytesIO, FileSize) == TRUE);
		ASSERT(BytesIO == FileSize);
		ASSERT(OutputFile.IsError() == FALSE);
		OutputFile.Close();

		/* Attempt to open and read file */
		ASSERT(InputFile.Open("c:\\temp\\test2.dat", "rb") == TRUE);
		ASSERT(InputFile.Read(pBuffer, FileSize) == TRUE);
		ASSERT(InputFile.IsError() == FALSE);
		ASSERT(InputFile.IsEOF() == FALSE);
		ASSERT(InputFile.ReadChar(IOChar) == FALSE);
		ASSERT(InputFile.IsEOF() == TRUE);
		ASSERT(InputFile.IsError() == FALSE);
		InputFile.Close();

		/* Attempt to open and write file */
		ASSERT(OutputFile.Open("c:\\temp\\test3.dat", "wb") == TRUE);
		ASSERT(OutputFile.Write(pBuffer, FileSize) == TRUE);
		ASSERT(OutputFile.IsError() == FALSE);
		OutputFile.Close();

		/* Ensure the files were successfully read and output */
		ASSERT(Test_CompareFiles("c:\\temp\\test1.dat", "c:\\temp\\test2.dat") == TRUE);
		ASSERT(Test_CompareFiles("c:\\temp\\test1.dat", "c:\\temp\\test3.dat") == TRUE);
	}

	DestroyArrayPointer(pBuffer);
}


/*===========================================================================
 *
 * Function - void Test_RWCharGenFile (NumTests);
 *
 * Repeatedly tests the ReadChar() and WriteChar() methods
 *
 *=========================================================================*/
void Test_RWCharGenFile(const std::size_t NumTests) {
	DEFINE_FUNCTION("Test_RWCharGenFile()");
	CGenFile InputFile;
	CGenFile OutputFile;
	std::size_t FileSize;
	std::size_t LoopCounter;
	std::size_t IOCounter;
	char IOChar;
	SystemLog.Printf(stdout,
	                 "================= Testing ReadChar/WriteChar Methods of CGenFile ================");
	RandomizeTimer();

	for (LoopCounter = 0; LoopCounter < NumTests; LoopCounter++) {
		/* Create randomly sized file */
		FileSize = Random((int)TEST_RWFILE_MAXFILESIZE);
		SystemLog.Printf(stdout, "\t%u) Testing file with size %u...", LoopCounter + 1, FileSize);
		Test_CreateRandomFile ("c:\\temp\\test1.dat", FileSize, FILE_BINARY);

		/* Open input/output files */
		ASSERT(InputFile.Open("c:\\temp\\test1.dat", "rb") == TRUE);
		ASSERT(OutputFile.Open("c:\\temp\\test2.dat", "wb") == TRUE);

		/* Copy the file, byte by byte */
		for (IOCounter = 0; IOCounter < FileSize; IOCounter++) {
			ASSERT(InputFile.ReadChar(IOChar) == TRUE);
			ASSERT(OutputFile.WriteChar(IOChar) == TRUE);
		}

		/* Check file status */
		ASSERT(InputFile.IsError() == FALSE);
		ASSERT(InputFile.IsEOF() == FALSE);
		ASSERT(InputFile.ReadChar(IOChar) == FALSE);
		ASSERT(InputFile.IsError() == FALSE);
		ASSERT(InputFile.IsEOF() == TRUE);
		ASSERT(OutputFile.IsError() == FALSE);
		ASSERT(OutputFile.IsEOF() == FALSE);
		InputFile.Close();
		OutputFile.Close();
		/* Ensure the files were successfully read and output */
		ASSERT(Test_CompareFiles("c:\\temp\\test1.dat", "c:\\temp\\test2.dat") == TRUE);
	}
}


/*===========================================================================
 *
 * Function - void Test_GenFileRWNumbers (NumTests);
 *
 * Repeatedly tests the Read/Write Int/Short/Long/Float methods.
 *
 *=========================================================================*/

#define OUTPUTSET(Array, Function) NumCounter = 0; while (TRUE) { \
		ASSERT(OutputFile.Function(Array[NumCounter])); if (Array[NumCounter] == 0) break; NumCounter++; }

#define INPUTSET(Array, Input, Function) NumCounter = 0; while (TRUE) { \
		ASSERT(InputFile.Read##Function(Input)); ASSERT(Input == Array[NumCounter]); ASSERT(OutputFile.Write##Function(Input)); if (Array[NumCounter] == 0) break; NumCounter++; }


void Test_GenFileRWNumbers(const std::size_t NumTests) {
	DEFINE_FUNCTION("Test_GenFileRWNumbers()");
	static short ShortNumbers[] = {
		1,
		-1,
		SHRT_MIN,
		SHRT_MAX,
		(short)USHRT_MAX,
		0,
		0
	};
	static int IntNumbers[] = {
		1,
		-1,
		INT_MIN,
		INT_MAX,
		(int)UINT_MAX,
		0,
		0
	};
	static long LongNumbers[] = {
		1,
		-1,
		LONG_MIN,
		LONG_MAX,
		(long)ULONG_MAX,
		0,
		0
	};
	static float FloatNumbers[] = {
		1,
		-1,
		SHRT_MIN,
		SHRT_MAX,
		USHRT_MAX,
		0,
		0
	};

	CGenFile InputFile;
	CGenFile OutputFile;
	std::size_t NumCounter;
	std::size_t TestCounter;
	ulong RandomSeed;
	std::size_t NumRandomNumbers;
	short InputShort, OutputShort;
	int InputInt, OutputInt;
	long InputLong, OutputLong;
	float InputFloat, OutputFloat;
	SystemLog.Printf(stdout,
	                 "================= Testing Read/Write Number Methods of CGenFile ================");
	RandomizeTimer();

	/* Main test loop, create random file and duplicate */
	for (TestCounter = 0; TestCounter < NumTests; TestCounter++) {
		ASSERT(OutputFile.Open ("c:\\temp\\gfnum1.dat", "wb"));
		SystemLog.Printf(stdout, "\t%4d) Testing...", TestCounter + 1);
		RandomSeed = Random((ulong)0x7FFFFFFFul);
		NumRandomNumbers = Random((int)TEST_GENFILE_RWNUMSIZE);
		SeedRandom(RandomSeed);

		/* Output set numbers */
		OUTPUTSET(ShortNumbers, WriteShort);
		OUTPUTSET(IntNumbers, WriteInt);
		OUTPUTSET(LongNumbers, WriteLong);
		OUTPUTSET(FloatNumbers, WriteFloat);

		/* Output random numbers */
		for (NumCounter = 0; NumCounter < NumRandomNumbers; NumCounter++) {
			OutputShort = (short)Random((ulong)USHRT_MAX);
			OutputInt = (int)Random((ulong)UINT_MAX);
			OutputLong = (long)Random();
			OutputFloat = (float)(Random() / (Random() * 0.8 + 1));
			ASSERT(OutputFile.WriteShort(OutputShort));
			ASSERT(OutputFile.WriteInt(OutputInt));
			ASSERT(OutputFile.WriteLong(OutputLong));
			ASSERT(OutputFile.WriteFloat(OutputFloat));
		}

		/* Reopen file for input */
		OutputFile.Close();
		ASSERT(InputFile.Open("c:\\temp\\gfnum1.dat", "rb"));
		ASSERT(OutputFile.Open("c:\\temp\\gfnum2.dat", "wb"));
		SeedRandom(RandomSeed);

		/* Input and verify set numbers */
		INPUTSET(ShortNumbers, InputShort, Short);
		INPUTSET(IntNumbers, InputInt, Int);
		INPUTSET(LongNumbers, InputLong, Long);
		INPUTSET(FloatNumbers, InputFloat, Float);

		/* Input and verify random numbers */
		for (NumCounter = 0; NumCounter < NumRandomNumbers; NumCounter++) {
			OutputShort = (short)Random((ulong)USHRT_MAX);
			OutputInt = (int)Random((ulong)UINT_MAX);
			OutputLong = (long)Random();
			OutputFloat = (float)(Random() / (Random() * 0.8 + 1));
			ASSERT(InputFile.ReadShort(InputShort));
			ASSERT(InputFile.ReadInt(InputInt));
			ASSERT(InputFile.ReadLong(InputLong));
			ASSERT(InputFile.ReadFloat(InputFloat));
			ASSERT(InputShort == OutputShort);
			ASSERT(InputInt == OutputInt);
			ASSERT(InputLong == OutputLong);
			ASSERT(InputFloat == OutputFloat);
			ASSERT(OutputFile.WriteShort(InputShort));
			ASSERT(OutputFile.WriteInt(InputInt));
			ASSERT(OutputFile.WriteLong(InputLong));
			ASSERT(OutputFile.WriteFloat(InputFloat));
		}

		InputFile.Close();
		OutputFile.Close();
		/* Ensure the files were successfully read and output */
		ASSERT(Test_CompareFiles("c:\\temp\\gfnum1.dat", "c:\\temp\\gfnum2.dat") == TRUE);
	}
}


/*===========================================================================
 *
 * Function - void Test_GenFile (void);
 *
 * Tests the CGenFile class.
 *  1. Repeatedly tests the Read/Write methods
 *  2. Repeatedly tests the ReadChar/WriteChar methods
 *  3. Tests the Open() Method
 *  4. Repeatedly tests the Read/Write number methods
 *
 *=========================================================================*/
void Test_GenFile() {
	//DEFINE_FUNCTION("Test_GenFile()");
	Test_GenFileOpen();
	Test_GenFilePrint();
	Test_GenFileSeekTell();
	Test_RWGenFile();
	Test_RWCharGenFile();
	Test_GenFileRWNumbers();
}

#endif
