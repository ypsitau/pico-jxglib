//==============================================================================
// FS.cpp
//==============================================================================
#include "jxglib/FS.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FS
//------------------------------------------------------------------------------
FS::Manager* FS::pManagerTop = nullptr;
FS::Manager* FS::pManagerCur = nullptr;

FS::FS()
{
}

FS::Manager* FS::FindManager(const char* pathName)
{
	char driveName[32];
	ExtractDriveName(pathName, driveName, sizeof(driveName));
	if (driveName[0] == '\0') {
		if (!pManagerCur) pManagerCur = pManagerTop;
		return pManagerCur;
	}
	Manager* pManager = pManagerTop;
	for ( ; pManager; pManager = pManager->GetNext()) {
		if (::strcasecmp(pManager->GetDriveName(), driveName) == 0) return pManager;
	}
	return nullptr;
}

const char* FS::SkipDriveName(const char* pathName)
{
	for (const char* p = pathName; *p; p++) {
		if (*p == ':') return p + 1;
	}
	return pathName;
}

const char* FS::ExtractDriveName(const char* pathName, char* driveName, int lenMax)
{
	driveName[0] = '\0';
	for (const char* p = pathName; *p; p++) {
		if (*p == ':') {
			int len = ChooseMin(p - pathName, lenMax - 1);
			::memcpy(driveName, pathName, len);
			driveName[len] = '\0';
		}
	}
	return driveName;
}

bool FS::SetDriveCur(const char* driveName)
{
	if (!IsLegalDriveName(driveName)) return false;
	Manager* pManager = FindManager(driveName);
	if (!pManager) return false;
	pManagerCur = pManager;
	return true;
}

FS::Dir* FS::OpenDirDrive()
{
	return new DirDrive(pManagerTop);
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

bool FS::Format(const char* driveName, Printable& out)
{
	if (!IsLegalDriveName(driveName)) {
		out.Printf("illegal drive name %s\n", driveName);
		return false;
	}
	Manager* pManager = FindManager(driveName);
	if (!pManager) {
		out.Printf("drive %s not found\n", driveName);
		return false;
	}
	if (pManager->Format()) {
		out.Printf("drive %s formatted successfully\n", driveName);
		return true;
	} else {
		out.Printf("failed to format drive %s\n", driveName);
		return false;
	}
}

bool FS::IsLegalDriveName(const char* driveName)
{
	int len = ::strlen(driveName);
	return len > 0 && driveName[len - 1] == ':';
}

const char* FS::JoinPathName(char* pathName, const char* dirName, const char* fileName)
{
	::strcpy(pathName, dirName);
	int len = ::strlen(pathName);
	if (len > 0 && pathName[len - 1] != '/') {
		pathName[len++] = '/';
	}
	::strcpy(pathName + len, fileName);
	return pathName;
}

//------------------------------------------------------------------------------
// FS::Manager
//------------------------------------------------------------------------------
FS::Manager::Manager() : pManagerNext_{nullptr}
{
	if (pManagerTop) {
		for (Manager* pManager = FS::pManagerTop; pManager; pManager = pManager->pManagerNext_) {
			if (!pManager->pManagerNext_) {
				pManager->pManagerNext_ = this;
				break;
			}
		}
	} else {
		FS::pManagerTop = this;
	}
}

//------------------------------------------------------------------------------
// FS::DirDrive
//------------------------------------------------------------------------------
bool FS::DirDrive::Read(FS::FileInfo** ppFileInfo)
{
	*ppFileInfo = &fileInfo_;
	fileInfo_.SetManager(pManager_);
	bool rtn = !!pManager_;
	pManager_ = pManager_->GetNext();
	return rtn;
}

}