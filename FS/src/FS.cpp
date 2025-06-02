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
	if (driveName[0] == '\0') return pDriveCur;
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
	return pDriveCur;
}

const char* SkipDriveName(const char* pathName)
{
	for (const char* p = pathName; *p; p++) {
		if (*p == ':') return p + 1;
	}
	return pathName;
}

bool SetDriveCur(const char* driveName)
{
	if (!IsLegalDriveName(driveName)) return false;
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) return false;
	pDriveCur = pDrive;
	return true;
}

File* OpenFile(const char* fileName, const char* mode, Drive* pDrive)
{
	char pathName[MaxPath];
	if (!pDrive) pDrive = FindDrive(fileName);
	return pDrive? pDrive->OpenFile(pDrive->NativePathName(pathName, sizeof(pathName), fileName), mode) : nullptr;
}

File* OpenFileForCopy(const char* fileNameSrc, const char* fileNameDst)
{
	if (FS::IsDirectory(fileNameDst)) {
		char fileNameBuff[MaxPath];
		Drive* pDrive = FindDrive(fileNameDst);
		if (!pDrive) return nullptr;
		fileNameDst = SkipDriveName(fileNameDst);
		pDrive->RegulatePathName(fileNameBuff, sizeof(fileNameBuff), fileNameDst);
		AppendPathName(fileNameBuff, sizeof(fileNameBuff), ExtractFileName(fileNameSrc));
		return OpenFile(fileNameBuff, "w", pDrive);
	} else {
		return OpenFile(fileNameDst, "w");
	}
}

Dir* OpenDir(const char* dirName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	return pDrive? pDrive->OpenDir(pDrive->NativePathName(pathName, sizeof(pathName), dirName)) : nullptr;
}

Glob* OpenGlob(const char* pattern, bool patternAsDirFlag)
{
	std::unique_ptr<Glob> pGlob(new Glob());
	return pGlob->Open(pattern, patternAsDirFlag)? pGlob.release() : nullptr;
}

bool PrintFile(Printable& terr, Printable& tout, const char* fileName)
{
	std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
	if (!pFile) {
		terr.Printf("failed to open %s\n", fileName);
		return false;
	}
	pFile->PrintTo(tout);
	return true;
}

bool ListFiles(Printable& terr, Printable& tout, const char* pathName, const FileInfo::Cmp& cmp)
{
	std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pathName, true));
	if (!pGlob) {
		terr.Printf("failed to open %s\n", pathName);
		return false;
	}
	std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->ReadAll(cmp));
	if (pFileInfo) pFileInfo->PrintList(tout);
	return true;
}

bool CopyFile(Printable& terr, const char* fileNameSrc, const char* fileNameDst)
{
	std::unique_ptr<FS::File> pFileSrc(FS::OpenFile(fileNameSrc, "r"));
	if (!pFileSrc) {
		terr.Printf("failed to open %s\n", fileNameSrc);
		return false;
	}
	std::unique_ptr<FS::File> pFileDst(FS::OpenFileForCopy(fileNameSrc, fileNameDst));
	if (!pFileDst) {
		terr.Printf("failed to open %s\n", fileNameDst);
		return false;
	}
	int bytesRead;
	char buff[128];
	while ((bytesRead = pFileSrc->Read(buff, sizeof(buff))) > 0) {
		if (pFileDst->Write(buff, bytesRead) != bytesRead) {
			terr.Printf("failed to write %s\n", fileNameDst);
			return false;
		}
	}
	return true;
}

bool RemoveFile(const char* fileName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(fileName);
	return pDrive? pDrive->RemoveFile(pDrive->NativePathName(pathName, sizeof(pathName), fileName)) : false;
}

bool RemoveFile(Printable& terr, const char* fileName)
{
	if (RemoveFile(fileName)) return true;
	terr.Printf("failed to remove file %s\n", fileName);
	return false;
}

bool MoveFile(Printable& terr, const char* fileNameOld, const char* fileNameNew)
{
	char pathNameOld[MaxPath], pathNameNew[MaxPath];
	Drive* pDriveOld = FindDrive(fileNameOld);
	if (!pDriveOld) {
		terr.Printf("drive for old file %s not found\n", fileNameOld);
		return false; // Drive not found
	}	
	Drive* pDriveNew = FindDrive(fileNameNew);
	if (!pDriveNew) {
		terr.Printf("drive for new file %s not found\n", fileNameNew);
		return false; // Drive not found
	}
	if (pDriveOld == pDriveNew) {
		// Same drive, just rename
		if (pDriveOld->RenameFile(pDriveOld->NativePathName(pathNameOld, sizeof(pathNameOld), fileNameOld),
				pDriveOld->NativePathName(pathNameNew, sizeof(pathNameNew), fileNameNew))) return true;
		terr.Printf("failed to rename file %s to %s\n", fileNameOld, fileNameNew);
		return false; // Rename failed
	}
	return CopyFile(terr, fileNameOld, fileNameNew) && RemoveFile(terr, fileNameOld); // Copy and then remove old file
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

bool IsDirectory(const char* pathName)
{
	char pathNameBuff[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive) return false;
	return pDrive->IsDirectory(pDrive->NativePathName(pathNameBuff, sizeof(pathNameBuff), pathName));
}

bool Format(Printable& terr, const char* driveName)
{
	if (!IsLegalDriveName(driveName)) {
		terr.Printf("illegal drive name %s\n", driveName);
		return false;
	}
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) {
		terr.Printf("drive %s not found\n", driveName);
		return false;
	}
	if (pDrive->Unmount() && pDrive->Format() && pDrive->Mount()) {
		terr.Printf("drive %s formatted in %s\n", driveName, pDrive->GetFileSystemName());
		return true;
	} else {
		terr.Printf("failed to format drive %s\n", driveName);
		return false;
	}
}

bool Mount(Printable& terr, const char* driveName)
{
	if (!IsLegalDriveName(driveName)) {
		terr.Printf("illegal drive name %s\n", driveName);
		return false;
	}
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) {
		terr.Printf("drive %s not found\n", driveName);
		return false;
	}
	pDrive->Mount();
	return true;
}

bool Unmount(Printable& terr, const char* driveName)
{
	if (!IsLegalDriveName(driveName)) {
		terr.Printf("illegal drive name %s\n", driveName);
		return false;
	}
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) {
		terr.Printf("drive %s not found\n", driveName);
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

bool GetDirNameCur(const char** pDriveName, const char** pDirName)
{
	if (pDriveName) *pDriveName = "";
	if (pDirName) *pDirName = "";
	FS::Drive* pDrive = FS::GetDriveCur();
	if (!pDrive) return false;
	if (pDriveName) *pDriveName = pDrive->GetDriveName();
	if (pDirName) *pDirName = pDrive->GetDirNameCur();
	return true;
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

const char* ExtractFileName(const char* pathName)
{
	const char* p = pathName;
	const char* fileName = nullptr;
	for ( ; *p; p++) {
		if (*p == '/') fileName = p + 1; // remember the start of the file name
	}
	return fileName? fileName : pathName; // return the last part or the whole path if no slashes
}

void SplitDirName(const char* pathName, char* dirName, int lenMax, const char** pFileName)
{
	const char* pSlash = nullptr;
	for (const char* p = pathName; *p; p++) {
		if (*p == '/' || *p == ':') pSlash = p;
	}
	dirName[0] = '\0';
	int lenDirName = 0;
	int lenFileName = 0;
	if (pSlash) {
		lenDirName = pSlash - pathName + 1;
		if (lenDirName + 1 > lenMax) ::panic("FS::SplitDirName");
		::memcpy(dirName, pathName, lenDirName);
		dirName[lenDirName] = '\0';
	}
	if (pFileName) {
		const char* fileNameSrc = pathName + lenDirName;
		char* fileNameDst = dirName + lenDirName + 1;
		int lenFileName = ::strlen(fileNameSrc);
		if (lenDirName + 1 + lenFileName + 1 > lenMax) ::panic("FS::SplitDirName");
		::memcpy(fileNameDst, fileNameSrc, lenFileName + 1);
		*pFileName = fileNameDst;
	}
}

const char* AppendPathName(char* pathName, int lenMax, const char* pathNameSub)
{
	int len = ::strlen(pathName);
	const char* p = pathNameSub;
	if (len > 0) {
		if (pathName[len - 1] != '/' && pathName[len - 1] != ':' && len < lenMax) pathName[len++] = '/';
		while (*p == '/') p++;
	}
	while (*p) {
		if (DoesMatchElemName(p, ".")) {
			p += 1;
			while (*p == '/') p++;
		} else if (DoesMatchElemName(p, "..")) {
			p += 2;
			if (len == 1 && pathName[0] == '/') {
				// nothing to do
			} else if (len > 0) {
				if (pathName[len - 1] == '/') len--;
				while (len > 0 && pathName[len - 1] != '/') len--;
			}
			while (*p == '/') p++;
		} else {
			for ( ; len < lenMax && *p && *p != '/'; len++, p++) pathName[len] = *p;
			if (*p == '/') {
				while (*p == '/') p++;
				if (len < lenMax) pathName[len++] = '/';
			}
		}
	}
	if (len >= lenMax) ::panic("FS::AppendPathName");
	pathName[len] = '\0';
	return pathName;
}

const char* JoinPathName(char* pathName, int lenMax, const char* dirName, const char* fileName)
{
	pathName[0] = '\0';
	AppendPathName(pathName, lenMax, dirName);
	AppendPathName(pathName, lenMax, fileName);
	return pathName;
}

bool DoesContainWildcard(const char* str)
{
	for (const char* p = str; *p; p++) {
		if (*p == '*' || *p == '?') return true;
	}
	return false;
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

bool DoesMatchElemName(const char* elemName1, const char* elemName2)
{
	for (;;) {
		char ch1 = (*elemName1 == '/')? '\0' : *elemName1;
		char ch2 = (*elemName2 == '/')? '\0' : *elemName2;
		if (::toupper(ch1) != ::toupper(ch2)) return false;
		if (ch1 == '\0') break;
		elemName1++;
		elemName2++;
	}
	return true;
}

bool DoesMatchDriveName(const char* driveName1, const char* driveName2)
{
	for (;;) {
		char ch1 = (*driveName1 == ':')? '\0' : *driveName1;
		char ch2 = (*driveName2 == ':')? '\0' : *driveName2;
		if (::toupper(ch1) != ::toupper(ch2)) return false;
		if (ch1 == '\0') break;
		driveName1++;
		driveName2++;
	}
	return true;
}

//------------------------------------------------------------------------------
// FS::File
//------------------------------------------------------------------------------
File::File(const Drive& drive) : drive_(drive)
{
}

//------------------------------------------------------------------------------
// FS::FileInfo
//------------------------------------------------------------------------------
FileInfo::Cmp_Combine FileInfo::CmpDefault(FileInfo::Cmp_Type::Ascent, FileInfo::Cmp_Name::Ascent, FileInfo::Cmp::Zero);

FileInfo::FileInfo() : name_{nullptr}, type_{Type::None}, size_{0} {}

FileInfo::FileInfo(const char* name, Type type, uint32_t size) : name_{new char[::strlen(name) + 1]}, type_{type}, size_{size}
{
	::strcpy(name_.get(), name);
}

void FS::FileInfo::PrintList(Printable& tout) const
{
	int lenMax = 16;
	for (const FileInfo* pFileInfo = this; pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		lenMax = ChooseMax(lenMax, ::strlen(pFileInfo->GetName()));
	}
	for (const FileInfo* pFileInfo = this; pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		if (pFileInfo->IsDirectory()) {
			tout.Printf("%*s <DIR>\n", -lenMax, pFileInfo->GetName());
		} else if (pFileInfo->IsFile()) {
			tout.Printf("%*s %d\n", -lenMax, pFileInfo->GetName(), pFileInfo->GetSize());
		}
	}
}

//------------------------------------------------------------------------------
// FS::FileInfo::Cmp
//------------------------------------------------------------------------------
const FileInfo::Cmp FileInfo::Cmp::Zero(0);

int FileInfo::Cmp::Compare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	return DoCompare(fileInfo1, fileInfo2) * multiplier_;
}

int FileInfo::Cmp::DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	return 0;
}

const FileInfo::Cmp_Type FileInfo::Cmp_Type::Ascent(+1);
const FileInfo::Cmp_Type FileInfo::Cmp_Type::Descent(-1);

int FileInfo::Cmp_Type::DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	return static_cast<int>(fileInfo1.GetType()) - static_cast<int>(fileInfo2.GetType());
}

const FileInfo::Cmp_Name FileInfo::Cmp_Name::Ascent(+1);
const FileInfo::Cmp_Name FileInfo::Cmp_Name::Descent(-1);

int FileInfo::Cmp_Name::DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	return ::strcasecmp(fileInfo1.GetName(), fileInfo2.GetName());
}

const FileInfo::Cmp_Size FileInfo::Cmp_Size::Ascent(+1);
const FileInfo::Cmp_Size FileInfo::Cmp_Size::Descent(-1);

int FileInfo::Cmp_Size::DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	return static_cast<int>(fileInfo1.GetSize()) - static_cast<int>(fileInfo2.GetSize());
}

int FileInfo::Cmp_Combine::DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	int rtn;
	rtn = pCmp1_->Compare(fileInfo1, fileInfo2);
	if (rtn != 0) return rtn;
	rtn = pCmp2_->Compare(fileInfo1, fileInfo2);
	if (rtn != 0) return rtn;
	rtn = pCmp3_->Compare(fileInfo1, fileInfo2);
	return rtn;
}

//------------------------------------------------------------------------------
// FS::FileInfoReader
//------------------------------------------------------------------------------
FileInfo* FileInfoReader::ReadAll(const FileInfo::Cmp& cmp)
{
	std::unique_ptr<FileInfo> pFileInfoHead;
	for (;;) {
		std::unique_ptr<FileInfo> pFileInfoToAdd(Read());
		if (!pFileInfoToAdd) break; // No more files to read
		FileInfo* pFileInfoPrev = nullptr;
		for (FileInfo* pFileInfo = pFileInfoHead.get(); pFileInfo; pFileInfo = pFileInfo->GetNext()) {
			if (cmp.Compare(*pFileInfoToAdd, *pFileInfo) < 0) break;
			pFileInfoPrev = pFileInfo;
		}
		if (pFileInfoPrev) {
			pFileInfoToAdd->SetNext(pFileInfoPrev->ReleaseNext());
			pFileInfoPrev->SetNext(pFileInfoToAdd.release());
		} else {
			pFileInfoToAdd->SetNext(pFileInfoHead.release());
			pFileInfoHead.reset(pFileInfoToAdd.release());
		}
	}
	return pFileInfoHead.release();
}

//------------------------------------------------------------------------------
// FS::Dir
//------------------------------------------------------------------------------
Dir::Dir(const Drive& drive) : drive_(drive), rewindFlag_{false}
{
}

//------------------------------------------------------------------------------
// FS::Glob
//------------------------------------------------------------------------------
Glob::Glob(): pattern_{""}
{
}

bool Glob::Open(const char* pattern, bool patternAsDirFlag)
{
	if (patternAsDirFlag) {
		// If the pattern is a directory, we can use it directly
		pDir_.reset(OpenDir(pattern));
		if (pDir_) {
			::snprintf(dirName_, sizeof(dirName_), "%s", pattern);
			pattern_ = "";
			return true;
		}
	}
	SplitDirName(pattern, dirName_, sizeof(dirName_), &pattern_);
	pDir_.reset(OpenDir(dirName_));
	return !!pDir_;
}

FileInfo* Glob::Read(const char** pPathName)
{
	if (!pDir_) return nullptr;
	for (;;) {
		std::unique_ptr<FileInfo> pFileInfo(pDir_->Read());
		if (!pFileInfo) break;
		if (!*pattern_ || DoesMatchWildcard(pattern_, pFileInfo->GetName())) {
			if (pPathName) {
				JoinPathName(pathName_, sizeof(pathName_), dirName_, pFileInfo->GetName());
				*pPathName = pathName_;
			}
			return pFileInfo.release();
		}
	}
	return nullptr;
}

void Glob::Close()
{
	pDir_.reset();
}

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
Drive::Drive(const char* formatName, const char* driveName) : mountedFlag_{false}, formatName_{formatName},
	driveName_{::isalpha(*driveName)? driveName : driveName + 1}, driveNameRaw_{driveName}, pDriveNext_{nullptr}
{
	::strcpy(dirNameCur_, "/");
	if (pDriveHead) {
		Drive* pDrivePrev = nullptr;
		for (Drive* pDrive = pDriveHead; pDrive; pDrive = pDrive->pDriveNext_) {
			if (::strcasecmp(GetDriveName(), pDrive->GetDriveName()) < 0) break;
			pDrivePrev = pDrive;
		}
		if (pDrivePrev) {
			pDriveNext_ = pDrivePrev->pDriveNext_;
			pDrivePrev->pDriveNext_ = this;
		} else {
			pDriveNext_ = pDriveHead;
			pDriveHead = this;
		}
	} else {
		pDriveHead = this;
	}
	if (IsPrimary() || !pDriveCur || (!pDriveCur->IsPrimary() && pDriveHead == this)) pDriveCur = this;
}

bool Drive::IsDirectory(const char* pathName)
{
	std::unique_ptr<Dir> pDir(OpenDir(pathName));
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
		pathNameBuff[0] = '\0';
	} else {
		if (::snprintf(pathNameBuff, lenBuff, "%s", dirNameCur_) >= lenBuff) ::panic("Drive::RegulatePathName");
	}
	AppendPathName(pathNameBuff, lenBuff, pathName);
	return pathNameBuff;
}

}
