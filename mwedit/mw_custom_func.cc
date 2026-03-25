/*===========================================================================
 *
 * File:    Mwcustomfunc.CPP
 * Author:  Dave Humphrey (uesp@m0use.net)
 * Created On:  August 21, 2006
 *
 * Description
 *
 *=========================================================================*/
#include "mwedit/mw_custom_func.h"

//#include <string.h>  // Required for non-standard Windows extension _stricmp()

#include <winnt.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "common/dl_base.h"
#include "common/dl_err.h"
#include "common/dl_file.h"
#include "common/dl_mem.h"
#include "common/dl_str.h"
#include "common/file/gen_file.h"
#include "mwedit/script_defs.h"

DEFINE_FILE("MwCustomFunc.cpp");

/* Global custom function map */
CMwCustomFunctions g_CustomFunctions;
/*===========================================================================
 *
 * Class CMwCustomFunction Constructor
 *
 *=========================================================================*/
CMwCustomFunction::CMwCustomFunction () {
	//DEFINE_FUNCTION("CMwCustomFunction::CMwCustomFunction()");
	std::memset(&m_Data, 0, sizeof(m_Data));
}


/*===========================================================================
 *
 * Class CMwCustomFunction Method - bool Read (File, LineCount);
 *
 * Input function parameters from the current position in the given text
 * file. Returns false on any error.
 *
 *=========================================================================*/
bool CMwCustomFunction::Read(CGenFile &File, dword &LineCount) {
	TCHAR Buffer[512];
	TCHAR *pVar;
	TCHAR *pValue;
	int Result;

	while (!File.IsEOF()) {
		Result = File.ReadLine(Buffer, 500);

		if (Result == READLINE_ERROR) {
			return false;
		}

		if (Result == READLINE_MSL) {
			File.ReadLine(NULL);
		}

		++LineCount;
		m_LineCount = LineCount;
		Result = SeperateVarValueQ(&pVar, &pValue, Buffer);

		if (Result) {
			Result = SetParameter(pVar, pValue);
		} else if (_stricmp(pVar, "end") == 0) {  // TODO: Non-standard extension. Replace with a portable version
			break;
		}
	}

	return true;
}


/*===========================================================================
 *
 * Class CMwCustomFunction Method - dword ParseFuncOptions (Options, pValue);
 *
 * Parse and return function options from the given string. Returns false
 * on any error.
 *
 *=========================================================================*/
bool CMwCustomFunction::ParseFuncOptions(dword &Options, const char *pValue) {
	bool ReturnValue = true;
	TCHAR Buffer[512];
	TCHAR *pParse;
	Options = 0;
	strnncpy(Buffer, pValue, 500);
	pParse = std::strtok(Buffer, "|");

	while (pParse != NULL) {
		pParse = trim(pParse);

		if (_stricmp(pParse, "none") == 0) {
		} else if (_stricmp(pParse, "shortvar") == 0) {
			Options |= ESMSCR_FUNC_ALLOWGLOBAL;
		} else if (_stricmp(pParse, "allowglobal") == 0) {
			Options |= ESMSCR_FUNC_ALLOWGLOBAL;
		} else if (_stricmp(pParse, "extrashort") == 0) {
			Options |= ESMSCR_FUNC_EXTRASHORT;
		} else if (_stricmp(pParse, "nooptout") == 0) {
			Options |= ESMSCR_FUNC_NOOPTOUT;
		} else if (_stricmp(pParse, "bloodmoon") == 0) {
			Options |= ESMSCR_FUNC_BLOODMOON;
		} else if (_stricmp(pParse, "tribunal") == 0) {
			Options |= ESMSCR_FUNC_TRIBUNAL;
		} else if (_stricmp(pParse, "dialogue") == 0) {
			Options |= ESMSCR_FUNC_DIALOGUE;
		} else if (_stricmp(pParse, "bad") == 0) {
			Options |= ESMSCR_FUNC_BAD;
		} else if (_stricmp(pParse, "allowvar") == 0) {
			Options |= ESMSCR_FUNC_VAR;
		} else if (_stricmp(pParse, "mwse") == 0 || _stricmp(pParse, "extended") == 0) {
			Options |= ESMSCR_FUNC_MWSE;
		} else if (_stricmp(pParse, "mwe") == 0 || _stricmp(pParse, "mwenhanced") == 0) {
			Options |= ESMSCR_FUNC_MWE;
		} else {
			ErrorHandler.AddError(ERR_BADINPUT,
			                      "%05d: Unknown function option '%s'!",
			                      m_LineCount,
			                      pParse);
			ReturnValue = false;
		}

		pParse = std::strtok(NULL, "|");
	}

	return ReturnValue;
}


/*===========================================================================
 *
 * Class CMwCustomFunction Method - dword ParseFuncArgOptions (Options, pValue);
 *
 * Parse and return function argument options from the given string. Returns
 * false on any error.
 *
 *=========================================================================*/
bool CMwCustomFunction::ParseFuncArgOptions(dword &Options, const char *pValue) {
	bool ReturnValue = true;
	TCHAR Buffer[512];
	TCHAR *pParse;
	Options = 0;
	strnncpy(Buffer, pValue, 500);
	pParse = std::strtok(Buffer, "|");

	while (pParse != NULL) {
		pParse = trim(pParse);

		if (_stricmp(pParse, "none") == 0) {
		} else if (_stricmp(pParse, "byte") == 0) {
			Options |= ESMSCR_FUNC_BYTE;
		} else if (_stricmp(pParse, "short") == 0) {
			Options |= ESMSCR_FUNC_SHORT;
		} else if (_stricmp(pParse, "long") == 0 || _stricmp(pParse, "ref") == 0) {
			Options |= ESMSCR_FUNC_LONG;
		} else if (_stricmp(pParse, "float") == 0) {
			Options |= ESMSCR_FUNC_FLOAT;
		} else if (_stricmp(pParse, "number") == 0) {
			Options |= ESMSCR_FUNC_NUMBER;
		} else if (_stricmp(pParse, "string") == 0) {
			Options |= ESMSCR_FUNC_STRING;
		} else if (_stricmp(pParse, "id") == 0) {
			Options |= ESMSCR_FUNC_ID;
		} else if (_stricmp(pParse, "xyz") == 0) {
			Options |= ESMSCR_FUNC_XYZ;
		} else if (_stricmp(pParse, "effect") == 0) {
			Options |= ESMSCR_FUNC_EFFECT;
		} else if (_stricmp(pParse, "reset") == 0) {
			Options |= ESMSCR_FUNC_RESET;
		} else if (_stricmp(pParse, "animation") == 0) {
			Options |= ESMSCR_FUNC_ANIM;
		} else if (_stricmp(pParse, "optstart") == 0) {
			Options |= ESMSCR_FUNC_OPTSTART;
		} else if (_stricmp(pParse, "optional") == 0) {
			Options |= ESMSCR_FUNC_OPTIONAL;
		} else if (_stricmp(pParse, "notrequired") == 0) {
			Options |= ESMSCR_FUNC_NOTREQ;
		} else if (_stricmp(pParse, "cellstring") == 0) {
			Options |= ESMSCR_FUNC_CELLSTR;
		} else if (_stricmp(pParse, "shortstring") == 0) {
			Options |= ESMSCR_FUNC_SHORTSTR;
		} else if (_stricmp(pParse, "many") == 0) {
			Options |= ESMSCR_FUNC_MANY;
		} else if (_stricmp(pParse, "scriptid") == 0) {
			Options |= ESMSCR_FUNC_SCRIPTID;
		} else if (_stricmp(pParse, "soundid") == 0) {
			Options |= ESMSCR_FUNC_SOUNDID;
		} else if (_stricmp(pParse, "raceid") == 0) {
			Options |= ESMSCR_FUNC_RACEID;
		} else if (_stricmp(pParse, "journalid") == 0) {
			Options |= ESMSCR_FUNC_JOURNALID;
		} else if (_stricmp(pParse, "factionid") == 0) {
			Options |= ESMSCR_FUNC_FACTIONID;
		} else if (_stricmp(pParse, "itemid") == 0) {
			Options |= ESMSCR_FUNC_ITEMID;
		} else if (_stricmp(pParse, "regionid") == 0) {
			Options |= ESMSCR_FUNC_REGIONID;
		} else if (_stricmp(pParse, "topicid") == 0) {
			Options |= ESMSCR_FUNC_TOPICID;
		} else if (_stricmp(pParse, "cellid") == 0) {
			Options |= ESMSCR_FUNC_CELLID;
		} else if (_stricmp(pParse, "effectid") == 0) {
			Options |= ESMSCR_FUNC_EFFECTID;
		} else if (_stricmp(pParse, "levelcreatureid") == 0) {
			Options |= ESMSCR_FUNC_LEVELCID;
		} else if (_stricmp(pParse, "levelitemid") == 0) {
			Options |= ESMSCR_FUNC_LEVELIID;
		} else if (_stricmp(pParse, "soulgemid") == 0) {
			Options |= ESMSCR_FUNC_SOULGEMID;
		} else if (_stricmp(pParse, "creatureid") == 0) {
			Options |= ESMSCR_FUNC_CREATUREID;
		} else if (_stricmp(pParse, "spellid") == 0) {
			Options |= ESMSCR_FUNC_SPELLID;
		} else if (_stricmp(pParse, "npcid") == 0) {
			Options |= ESMSCR_FUNC_NPCID;
		} else {
			ErrorHandler.AddError(ERR_BADINPUT,
			                      "%05d: Unknown function argument option '%s'!",
			                      m_LineCount,
			                      pParse);
			ReturnValue = false;
		}

		pParse = std::strtok(NULL, "|");
	}

	return ReturnValue;
}


/*===========================================================================
 *
 * Class CMwCustomFunction Method - dword ParseReturnOptions (Options, pValue);
 *
 * Parse and return function return options from the given string. Returns
 * false on any error.
 *
 *=========================================================================*/
bool CMwCustomFunction::ParseReturnOptions(dword &Options, const char *pValue) {
	bool ReturnValue = true;
	TCHAR Buffer[512];
	TCHAR *pParse;
	Options = 0;
	strnncpy(Buffer, pValue, 500);
	pParse = std::strtok(Buffer, "|");

	while (pParse != NULL) {
		pParse = trim(pParse);

		if (_stricmp(pParse, "none") == 0) {
		} else if (_stricmp(pParse, "byte") == 0) {
			Options |= ESMSCR_FUNC_BYTE;
		} else if (_stricmp(pParse, "short") == 0) {
			Options |= ESMSCR_FUNC_SHORT;
		} else if (_stricmp(pParse, "long") == 0 || _stricmp(pParse, "ref") == 0) {
			Options |= ESMSCR_FUNC_LONG;
		} else if (_stricmp(pParse, "float") == 0) {
			Options |= ESMSCR_FUNC_FLOAT;
		} else if (_stricmp(pParse, "string") == 0) {
			Options |= ESMSCR_FUNC_STRING;
		} else if (_stricmp(pParse, "many") == 0) {
			Options |= ESMSCR_FUNC_MANY;
		} else {
			ErrorHandler.AddError(ERR_BADINPUT,
			                      "%05d: Unknown function return option '%s'!",
			                      m_LineCount,
			                      pParse);
			ReturnValue = false;
		}

		pParse = std::strtok(NULL, "|");
	}

	return ReturnValue;
}


/*===========================================================================
 *
 * Class CMwCustomFunction Method - bool SetParameter (pVariable, pValue);
 *
 * Attempts to set a function parameter using the given inputs. Returns
 * false on any error.
 *
 *=========================================================================*/
bool CMwCustomFunction::SetParameter(const char *pVariable, const char *pValue) {
	dword Options;
	bool Result;

	if (_stricmp(pVariable, "name") == 0) {
		SetName(pValue);
		return true;
	} else if (_stricmp(pVariable, "opcode") == 0) {
		dword OpCode = std::strtoul(pValue, NULL, 0);
		SetOpCode((short)OpCode);
		return OpCode != 0;
	} else if (_stricmp(pVariable, "options") == 0) {
		Result = ParseFuncOptions(Options, pValue);
		SetOptions(Options);
		return Result;
	} else if (_stricmp(pVariable, "return") == 0) {
		Result = ParseReturnOptions(Options, pValue);
		SetReturnOptions(Options);
		return Result;
	} else if (_stricmp(pVariable, "param1") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(0, Options);
		return Result;
	} else if (_stricmp(pVariable, "param2") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(1, Options);
		return Result;
	} else if (_stricmp(pVariable, "param3") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(2, Options);
		return Result;
	} else if (_stricmp(pVariable, "param4") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(3, Options);
		return Result;
	} else if (_stricmp(pVariable, "param5") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(4, Options);
		return Result;
	} else if (_stricmp(pVariable, "param6") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(5, Options);
		return Result;
	} else if (_stricmp(pVariable, "param7") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(6, Options);
		return Result;
	} else if (_stricmp(pVariable, "param8") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(7, Options);
		return Result;
	} else if (_stricmp(pVariable, "param9") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(8, Options);
		return Result;
	} else if (_stricmp(pVariable, "param10") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(9, Options);
		return Result;
	} else if (_stricmp(pVariable, "param11") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(10, Options);
		return Result;
	} else if (_stricmp(pVariable, "param12") == 0) {
		Result = ParseFuncArgOptions(Options, pValue);
		SetArgOptions(11, Options);
		return Result;
	}

	ErrorHandler.AddError(ERR_BADINPUT, "Unknown function parameter '%s'!", pVariable);
	return false;
}


/*===========================================================================
 *
 * Function - bool ReadMwCustomFunctions (Functions, pFilename);
 *
 * Input custom functions from the given file. Returns false on any error.
 *
 *=========================================================================*/
bool ReadMwCustomFunctions(CMwCustomFunctions &Functions, const char *pFilename) {
	DEFINE_FUNCTION("ReadMwCustomFunctions()");
	CMwCustomFunction *pFunction;
	CGenFile File;
	dword LineCount = 0;

	char Buffer[512];
	char *pVar;
	char *pValue;
	int Result;

	/* Clear the current content */
	Functions.RemoveAll();
	Result = File.Open(pFilename, "rb");

	if (!Result) {
		return false;
	}

	while (!File.IsEOF()) {
		Result = File.ReadLine(Buffer, 500);

		if (Result == READLINE_ERROR) {
			return false;
		}

		if (Result == READLINE_MSL) {
			File.ReadLine(NULL);
		}

		++LineCount;
		SeperateVarValueQ(&pVar, &pValue, Buffer);

		if (stricmp(pVar, "function") == 0) {
			CreatePointer(pFunction, CMwCustomFunction);
			Result = pFunction->Read(File, LineCount);
			Functions.SetAt(pFunction->GetName(), pFunction);

			if (!Result) {
				return false;
			}
		} else if (*pVar != NULL_CHAR) {
			ErrorHandler.AddError(ERR_BADINPUT, "Unknown function parameter '%s'!", pVar);
		}
	}

	return true;
}
