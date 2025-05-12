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

FS::Manager* FS::FindManager(const char* pathName)
{
	char driveName[32];
	ExtractDriveName(pathName, driveName, sizeof(driveName));
	Manager* pManager = pManagerTop;
	for ( ; pManager; pManager = pManager->GetNext()) {
		if (::strcasecmp(pManager->GetDriveName(), driveName) == 0) return pManager;
	}
	return nullptr;
}

const char* FS::SkipDriveName(const char* pathName)
{
	const char* p = pathName;
	if (*p == '/') p++;
	for ( ; *p && *p != '/'; p++) ;
	return p;
}

const char* FS::ExtractDriveName(const char* pathName, char* driveName, int lenMax)
{
	const char* p = pathName;
	if (*p == '/') p++;
	const char* pMark = p;
	for ( ; *p && *p != '/'; p++) ;
	int len = ChooseMin(p - pMark, lenMax - 1);
	::memcpy(driveName, pMark, len);
	driveName[len] = '\0';
	return driveName;
}

FS::File* FS::OpenFile(const char* fileName, const char* mode)
{
	Manager* pManager = FindManager(fileName);
	return pManager? pManager->OpenFile(SkipDriveName(fileName), mode) : nullptr;
}

FS::Dir* FS::OpenDir(const char* dirName)
{
	Manager* pManager = FindManager(dirName);
	return pManager? pManager->OpenDir(SkipDriveName(dirName)) : nullptr;
}

bool FS::RemoveFile(const char* fileName)
{
	Manager* pManager = FindManager(fileName);
	return pManager? pManager->RemoveFile(SkipDriveName(fileName)) : false;
}

bool FS::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	Manager* pManager = FindManager(fileNameOld);
	return pManager? pManager->RenameFile(SkipDriveName(fileNameOld), SkipDriveName(fileNameNew)) : false;
}

bool FS::CreateDir(const char* dirName)
{
	Manager* pManager = FindManager(dirName);
	return pManager? pManager->CreateDir(SkipDriveName(dirName)) : false;
}

bool FS::RemoveDir(const char* dirName)
{
	Manager* pManager = FindManager(dirName);
	return pManager? pManager->RemoveDir(SkipDriveName(dirName)) : false;
}

bool FS::RenameDir(const char* dirNameOld, const char* dirNameNew)
{
	Manager* pManager = FindManager(dirNameOld);
	return pManager? pManager->RenameDir(SkipDriveName(dirNameOld), SkipDriveName(dirNameNew)) : false;
}

bool FS::Format(const char* driveName)
{
	Manager* pManager = FindManager(driveName);
	return pManager? pManager->Format() : false;
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
