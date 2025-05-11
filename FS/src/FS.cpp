//==============================================================================
// FS.cpp
//==============================================================================
#include "jxglib/FS.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FS
//------------------------------------------------------------------------------
FS::Manager* FS::pManagerTop = nullptr;

FS::FS()
{
}

FS::File* FS::OpenFile(const char* fileName, const char* mode)
{
	if (pManagerTop) {
		return pManagerTop->OpenFile(fileName, mode);
	}
	return nullptr;
}

FS::Dir* FS::OpenDir(const char* dirName)
{
	if (pManagerTop) {
		return pManagerTop->OpenDir(dirName);
	}
	return nullptr;
}

//------------------------------------------------------------------------------
// FS::Manager
//------------------------------------------------------------------------------
FS::Manager::Manager() : pManagerNext_{nullptr}
{
	if (pManagerTop) {
		pManagerNext_ = pManagerTop;
		FS::pManagerTop = this;
	} else {
		FS::pManagerTop = this;
	}
}

}
