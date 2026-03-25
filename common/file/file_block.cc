#include "common/file/file_block.h"

#if __WIN32
#include "common/dl_base.h"
#include "common/file/gen_find.h"
#endif

/*=========================================================================
 *
 * Class CFileBlock Destructor
 *
 *=======================================================================*/
void CFileBlock::Destroy() {
	//DEFINE_FUNCTION("CFileBlock::Destroy");
	/* Clear all elements to 0 initially depending on platform */
#if _WIN32
	BlockData.attrib = FA_NORMAL;
	BlockData.name[0] = NULL_CHAR;
	BlockData.time_create = -1;
	BlockData.time_access = -1;
	BlockData.time_write = -1;
	BlockData.size = 0;
#endif
}
