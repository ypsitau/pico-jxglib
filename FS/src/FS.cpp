//==============================================================================
// FS.cpp
//==============================================================================
#include <string.h>
#include <ctype.h>
#include <memory>
#include "jxglib/FS.h"

namespace jxglib::FS {

//------------------------------------------------------------------------------
// Global Variables
//------------------------------------------------------------------------------
static Drive* pDriveHead = nullptr;
static Drive* pDriveCur = nullptr;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
Drive* FindDrive(const char* pathName)
{
	char driveName[32];
	ExtractDriveName(pathName, driveName, sizeof(driveName));
	if (driveName[0] == '\0') {
		if (!pDriveCur) pDriveCur = pDriveHead;
		return pDriveCur;
	}
	Drive* pDrive = pDriveHead;
	for ( ; pDrive; pDrive = pDrive->GetNext()) {
		if (::strcasecmp(pDrive->GetDriveName(), driveName) == 0) return pDrive;
	}
	return nullptr;
}

Drive* GetDriveHead()
{
	return pDriveHead;
}

Drive* GetDriveCur()
{
	if (!pDriveCur) pDriveCur = pDriveHead;
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

File* OpenFile(const char* fileName, const char* mode)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(fileName);
	return pDrive? pDrive->OpenFile(pDrive->NativePathName(pathName, sizeof(pathName), fileName), mode) : nullptr;
}

Dir* OpenDir(const char* dirName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->OpenDir(pDrive->NativePathName(pathName, sizeof(pathName), dirName)) : nullptr;
}

bool RemoveFile(const char* fileName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(fileName);
	return pDrive? pDrive->RemoveFile(pDrive->NativePathName(pathName, sizeof(pathName), fileName)) : false;
}

bool RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	char pathNameOld[MaxPath], pathNameNew[MaxPath];
	Drive* pDrive = FindDrive(fileNameOld);
	return pDrive? pDrive->RenameFile(pDrive->NativePathName(pathNameOld, sizeof(pathNameOld), fileNameOld),
				pDrive->NativePathName(pathNameNew, sizeof(pathNameNew), fileNameNew)) : false;
}

bool CreateDir(const char* dirName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->CreateDir(pDrive->NativePathName(pathName, sizeof(pathName), dirName)) : false;
}

bool RemoveDir(const char* dirName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->RemoveDir(pDrive->NativePathName(pathName, sizeof(pathName), dirName)) : false;
}

bool RenameDir(const char* dirNameOld, const char* dirNameNew)
{
	char pathNameOld[MaxPath], pathNameNew[MaxPath];
	Drive* pDrive = FindDrive(dirNameOld);
	return pDrive? pDrive->RenameDir(pDrive->NativePathName(pathNameOld, sizeof(pathNameOld), dirNameOld),
			pDrive->NativePathName(pathNameNew, sizeof(pathNameNew), dirNameNew)) : false;
}

bool ChangeCurDir(const char* dirName)
{
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) return false;
	char pathName[MaxPath];
	char pathNameNative[MaxPath];
	dirName = pDrive->RegulatePathName(pathName, sizeof(pathName), dirName);
	if (pDrive->IsDirectory(pDrive->NativePathName(pathNameNative, sizeof(pathNameNative), dirName))) {
		pDrive->SetDirNameCur(dirName);
		return true;
	}
	return false;
}

bool Format(Printable& out, const char* driveName)
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
	if (pDrive->Unmount() && pDrive->Format() && pDrive->Mount()) {
		out.Printf("drive %s formatted in %s\n", driveName, pDrive->GetFileSystemName());
		return true;
	} else {
		out.Printf("failed to format drive %s\n", driveName);
		return false;
	}
}

bool Mount(Printable& out, const char* driveName)
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
	pDrive->Mount();
	return true;
}

bool Unmount(Printable& out, const char* driveName)
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
	pDrive->Unmount();
	return true;
}

FileInfo* GetFileInfo(const char* pathName)
{
	char pathNameBuff[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive) return nullptr;
	return pDrive->GetFileInfo(pDrive->NativePathName(pathNameBuff, sizeof(pathNameBuff), pathName));
}

bool IsLegalDriveName(const char* driveName)
{
	for (const char*p = driveName; *p; p++) {
		if (*p == ':') {
			return *(p + 1) == '\0';
		} else if (!isalnum(*p)) {
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

bool DoesMatchWildcard(const char* pattern, const char* str)
{
	const char* p = pattern;
	const char* s = str;
	for (;;) {
		if (*p == '?') {
			p++;
			s++;
		} else if (*p == '*') {
			p++;
			if (*p == '\0') return true;
			if (*s == '.' && s == str) return false;
			while (*s && *s != *p) s++;
		} else if (::toupper(*p) == ::toupper(*s)) {
			if (*p == '\0') break;
			p++;
			s++;
		} else {
			return false;
		}
	}
	return true;
}

bool DoesMatchElemName(const char* str1, const char* str2)
{
	for (;;) {
		char ch1 = (*str1 == '/')? '\0' : *str1;
		char ch2 = (*str2 == '/')? '\0' : *str2;
		if (::toupper(ch1) != ::toupper(ch2)) return false;
		if (ch1 == '\0') break;
		str1++;
		str2++;
	}
	return true;
}

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
Drive::Drive(const char* formatName, const char* driveName) :
		formatName_{formatName}, driveName_{driveName}, pDriveNext_{nullptr}
{
	::strcpy(dirNameCur_, "/");
	if (pDriveHead) {
		for (Drive* pDrive = pDriveHead; pDrive; pDrive = pDrive->pDriveNext_) {
			if (!pDrive->pDriveNext_) {
				pDrive->pDriveNext_ = this;
				break;
			}
		}
	} else {
		pDriveHead = this;
	}
}

bool Drive::IsDirectory(const char* pathName)
{
	RefPtr<Dir> pDir(OpenDir(pathName));
	return !!pDir;
}

void Drive::SetDirNameCur(const char* dirName)
{
	int len = ::strlen(dirName);
	if (len == 0) {
		::strcpy(dirNameCur_, "/");
	} else {
		if (::snprintf(dirNameCur_, sizeof(dirNameCur_), dirName) >= sizeof(dirNameCur_)) ::panic("Drive::SetDirNameCur");
		if (dirNameCur_[len - 1] != '/') {
			if (len >= sizeof(dirNameCur_) - 2) ::panic("Drive::SetDirNameCur");
			dirNameCur_[len++] = '/';
			dirNameCur_[len] = '\0';
		}
	}	
}

const char* Drive::RegulatePathName(char* pathNameBuff, int lenBuff, const char* pathName)
{
	pathName = SkipDriveName(pathName);
	if (pathName[0] == '/') {
		if (::snprintf(pathNameBuff, lenBuff, "%s", pathName) >= lenBuff) ::panic("Drive::RegulatePathName");
	} else {
		if (::snprintf(pathNameBuff, lenBuff, "%s%s", dirNameCur_, pathName) >= lenBuff) ::panic("Drive::RegulatePathName");
	}
	return pathNameBuff;
}

}
