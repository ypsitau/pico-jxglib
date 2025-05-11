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

bool FS::RemoveFile(const char* fileName)
{
	if (pManagerTop) {
		return pManagerTop->RemoveFile(fileName);
	}
	return false;
}

bool FS::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	if (pManagerTop) {
		return pManagerTop->RenameFile(fileNameOld, fileNameNew);
	}
	return false;
}

bool FS::CreateDir(const char* dirName)
{
	if (pManagerTop) {
		return pManagerTop->CreateDir(dirName);
	}
	return false;
}

bool FS::RemoveDir(const char* dirName)
{
	if (pManagerTop) {
		return pManagerTop->RemoveDir(dirName);
	}
	return false;
}

bool FS::RenameDir(const char* fileNameOld, const char* fileNameNew)
{
	if (pManagerTop) {
		return pManagerTop->RenameDir(fileNameOld, fileNameNew);
	}
	return false;
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
