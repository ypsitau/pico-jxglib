//==============================================================================
// FS.cpp
//==============================================================================
#include <ctype.h>
#include "jxglib/FS.h"

namespace jxglib::FS {

//------------------------------------------------------------------------------
// FS
//------------------------------------------------------------------------------
static Drive* pDriveTop = nullptr;
static Drive* pDriveCur = nullptr;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
Drive* FindDrive(const char* pathName)
{
	char driveName[32];
	ExtractDriveName(pathName, driveName, sizeof(driveName));
	if (driveName[0] == '\0') {
		if (!pDriveCur) pDriveCur = pDriveTop;
		return pDriveCur;
	}
	Drive* pDrive = pDriveTop;
	for ( ; pDrive; pDrive = pDrive->GetNext()) {
		if (::strcasecmp(pDrive->GetDriveName(), driveName) == 0) return pDrive;
	}
	return nullptr;
}

Drive* GetDriveCur()
{
	if (!pDriveCur) pDriveCur = pDriveTop;
	return pDriveCur;
}

const char* SkipDriveName(const char* pathName)
{
	for (const char* p = pathName; *p; p++) {
		if (*p == ':') return p + 1;
	}
	return pathName;
}

const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax)
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

bool SetDriveCur(const char* driveName)
{
	if (!IsLegalDriveName(driveName)) return false;
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) return false;
	pDriveCur = pDrive;
	return true;
}

Dir* OpenDirDrive()
{
	return new DirDrive(pDriveTop);
}

File* OpenFile(const char* fileName, const char* mode)
{
	char pathName[MaxLenPathName];
	Drive* pDrive = FindDrive(fileName);
	return pDrive? pDrive->OpenFile(pDrive->RegulatePathName(pathName, fileName), mode) : nullptr;
}

Dir* OpenDir(const char* dirName)
{
	char pathName[MaxLenPathName];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->OpenDir(pDrive->RegulatePathName(pathName, dirName)) : nullptr;
}

bool RemoveFile(const char* fileName)
{
	char pathName[MaxLenPathName];
	Drive* pDrive = FindDrive(fileName);
	return pDrive? pDrive->RemoveFile(pDrive->RegulatePathName(pathName, fileName)) : false;
}

bool RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	char pathNameOld[MaxLenPathName], pathNameNew[MaxLenPathName];
	Drive* pDrive = FindDrive(fileNameOld);
	return pDrive? pDrive->RenameFile(pDrive->RegulatePathName(pathNameOld, fileNameOld), pDrive->RegulatePathName(pathNameNew, fileNameNew)) : false;
}

bool CreateDir(const char* dirName)
{
	char pathName[MaxLenPathName];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->CreateDir(pDrive->RegulatePathName(pathName, dirName)) : false;
}

bool RemoveDir(const char* dirName)
{
	char pathName[MaxLenPathName];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->RemoveDir(pDrive->RegulatePathName(pathName, dirName)) : false;
}

bool RenameDir(const char* dirNameOld, const char* dirNameNew)
{
	char pathNameOld[MaxLenPathName], pathNameNew[MaxLenPathName];
	Drive* pDrive = FindDrive(dirNameOld);
	return pDrive? pDrive->RenameDir(pDrive->RegulatePathName(pathNameOld, dirNameOld), pDrive->RegulatePathName(pathNameNew, dirNameNew)) : false;
}

bool ChangeCurDir(const char* dirName)
{
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) return false;
	char pathName[MaxLenPathName];
	dirName = pDrive->RegulatePathName(pathName, dirName);
	RefPtr<Dir> pDir(pDrive->OpenDir(dirName));
	if (!pDir) return false;
	pDrive->SetDirNameCur(dirName);
	return true;
}

bool Format(const char* driveName, Printable& out)
{
	if (!IsLegalDriveName(driveName)) {
		out.Printf("illegal drive name %s\n", driveName);
		return false;
	}
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) {
		out.Printf("drive %s not found\n", driveName);
		return false;
	}
	if (pDrive->Format()) {
		out.Printf("drive %s formatted successfully\n", driveName);
		return true;
	} else {
		out.Printf("failed to format drive %s\n", driveName);
		return false;
	}
}

bool IsLegalDriveName(const char* driveName)
{
	for (const char*p = driveName; *p; p++) {
		if (*p == ':') {
			return *(p + 1) == '\0';
		} else if (!isalpha(*p)) {
			return false;
		}
	}
	return false;
}

const char* JoinPathName(char* pathName, const char* dirName, const char* fileName)
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
// FS::Drive
//------------------------------------------------------------------------------
Drive::Drive() : pDriveNext_{nullptr}
{
	::strcpy(dirNameCur_, "/");
	if (pDriveTop) {
		for (Drive* pDrive = pDriveTop; pDrive; pDrive = pDrive->pDriveNext_) {
			if (!pDrive->pDriveNext_) {
				pDrive->pDriveNext_ = this;
				break;
			}
		}
	} else {
		pDriveTop = this;
	}
}

const char* Drive::RegulatePathName(char* pathNameBuff, const char* pathName)
{
	pathName = SkipDriveName(pathName);
	if (pathName[0] == '/') {
		::strcpy(pathNameBuff, pathName);
	} else {
		::strcpy(pathNameBuff, dirNameCur_);
		int len = ::strlen(pathNameBuff);
		if (len == 0 || pathNameBuff[len - 1] != '/') {
			pathNameBuff[len++] = '/';
		}
		::strcpy(pathNameBuff + len, pathName);
	}
	return pathNameBuff;
}

//------------------------------------------------------------------------------
// FS::DirDrive
//------------------------------------------------------------------------------
bool DirDrive::Read(FS::FileInfo** ppFileInfo)
{
	*ppFileInfo = &fileInfo_;
	fileInfo_.SetDrive(pDrive_);
	bool rtn = !!pDrive_;
	pDrive_ = pDrive_->GetNext();
	return rtn;
}

}
