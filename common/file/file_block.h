#ifndef COMMON_FILE_FILE_BLOCK
#define COMMON_FILE_FILE_BLOCK

#include <ctime>

#if _WIN32
#include <io.h>

#include "common/dl_base.h"

/* Define the base file block type */
typedef struct _finddata_t fileblock_t;  // TODO: Windows specific so CFileBlock breaks due to the typedef. Replace with something cross-platform.
#endif
/*=========================================================================
 *
 * Class CFileBlock Definition
 *
 * A class to handle the different variations of the fileblock structure
 * under the various operating systems.
 *
 *=======================================================================*/
class CFileBlock {
  protected:
	fileblock_t BlockData;

  public:
	/* Class Constructor */
	CFileBlock() {
		Destroy();
	}

	/* Class Destructor */
	virtual void Destroy();

	/* Return various members of the file block data */
	fileblock_t &GetBlock() {
		return BlockData;
	}

	fileblock_t *GetBlockPtr() {
		return &BlockData;
	}

	/* Define the get methods depending on the platform */
#if _WIN32
	char *GetName() {
		return &BlockData.name[0];
	}

	int GetAttribute() const {
		return BlockData.attrib;
	}

	std::time_t GetCreationTime() const {
		return BlockData.time_create;
	}

	std::time_t GetAccessTime() const {
		return BlockData.time_access;
	}

	std::time_t GetWriteTime() const {
		return BlockData.time_write;
	}

	ulong GetSize() const {
		return BlockData.size;
	}
#endif
};

#endif  // COMMON_FILE_FILE_BLOCK
