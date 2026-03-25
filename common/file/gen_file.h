/*===========================================================================
 *
 * File:    GenFile.H
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  Thursday, May 17, 2001
 *
 * Contains the definition of the CGenFile class which contains the basic
 * file stream I/O handling.
 *
 *=========================================================================*/
#ifndef __GENFILE_H
#define __GENFILE_H

#include <cstdarg>
#include <cstddef>
#include <cstdio>

#if _DEBUG
#define TEST_GENFILE_SEEKFILESIZE 10000
#define TEST_GENFILE_RWNUMSIZE  1000
#endif  // _DEBUG

/* Used to indicate a position in a file stream */
typedef long filepos_t;
/*===========================================================================
 *
 * Begin Class CGenFile Definition
 *
 * Encapsulates basic file stream I/O operations, helping to reduce the
 * apparent complexity of source code.
 *
 *=========================================================================*/
class CGenFile {
  protected:
	std::FILE *m_pFileHandle; /* The file stream handle */
	bool m_Attached;     /* Indicates the stream should not be closed */
	long m_LineCount;    /* Used to track lines in file */


  public:
	/* Class Constructors */
	CGenFile();
	CGenFile(std::FILE *pSourceHandle);
	CGenFile(const char *pFilename, const char *pMode);

	/* Class destructors */
	virtual ~CGenFile();
	virtual void Destroy();

	/* Attach to an existing file stream */
	void Attach(std::FILE *pFileHandle);
	void Detach();

	/* Clears the error flag of the stream */
	void ClearError();

	/* Close an opened file stream */
	virtual void Close();

	/* Creates a new temporary file */
	bool CreateTemp();

	/* Attempt to flush a file stream buffer */
	bool Flush();

	/* Return the file handle for object */
	std::FILE *GetHandle();

	/* Return the file size for the current file */
	long GetFileSize();
	bool GetFileSize(long &FileSize);

	/* Class status methods */
	bool IsOpen();
	bool IsEOF();
	bool IsError();

	/* Line count methods */
	long DecLineCount() {
		return --m_LineCount;
	}

	long IncLineCount() {
		return ++m_LineCount;
	}

	void SetLineCount(const long Value) {
		m_LineCount = Value;
	}

	long GetLineCount() const {
		return m_LineCount;
	}

	/* Attempt to open the given filename */
	virtual bool Open(const char *pFilename, const char *pMode);

	/* Output formatted string to file stream */
	bool Printf(const char *pString, ...);
	bool VPrintf(const char *pString, std::va_list Args);

	/* Attempt to read a section from the file */
	bool Read(char *pBuffer, std::size_t &BytesRead, const std::size_t NumBytes);
	bool Read(char *pBuffer, const std::size_t NumBytes);
	bool ReadEx(char *pBuffer, const std::size_t Size, const std::size_t Count);

	/* Read a line */
	int ReadLine(char *pBuffer = NULL, const std::size_t MaxStringLength = 0);

	/* Input a character from the file stream */
	bool ReadChar(char &Char);

	/* Read binary integers */
	bool ReadShort(short &Value);
	bool ReadInt(int &Value);
	bool ReadLong(long &Value);
	bool ReadFloat(float &Value);

	/* Move the file position to the file beginning */
	void Rewind();

	/* Move the current file position */
	bool Seek(const filepos_t Position, const int SeekType);

	/* Get the current file position */
	filepos_t Tell();
	bool Tell(filepos_t &Position);

	/* Attempt to write a section to the file */
	bool Write(const char *pBuffer, std::size_t &BytesWritten, const std::size_t NumBytes);
	bool Write(const char *pBuffer, const std::size_t NumBytes);
	bool WriteEx(const char *pBuffer, const std::size_t Size, const std::size_t Count);

	/* Output a character to the file */
	bool WriteChar(const char Char);

	/* Write binary integers */
	bool WriteShort(const short Value);
	bool WriteInt(const int Value);
	bool WriteLong(const long Value);
	bool WriteFloat(const float Value);

	/* FILE* type conversion operator */
	operator std::FILE *();
};


#if _DEBUG
void Test_RWGenFile(const std::size_t NumTests = 100);
void Test_RWCharGenFile(const std::size_t NumTests = 20);
void Test_GenFileRWNumbers(const std::size_t NumTests = 1000);
void Test_GenFileSeekTell(const std::size_t NumTests = 1000);
void Test_GenFilePrint();
void Test_GenFile();
#endif  // _DEBUG


#endif
