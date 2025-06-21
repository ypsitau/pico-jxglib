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

#if 0
File* OpenFile(const char* fileName, const char* mode, Drive* pDrive)
{
	char filenameN[MaxPath];
	if (!pDrive) pDrive = FindDrive(fileName);
	return pDrive? pDrive->OpenFile(pDrive->NativePathName(filenameN, sizeof(filenameN), fileName), mode) : nullptr;
}
#endif

File* OpenFile(const char* fileName, const char* mode)
{
	char filenameN[MaxPath];
	Drive* pDrive = FindDrive(fileName);
	return pDrive? pDrive->OpenFile(pDrive->NativePathName(filenameN, sizeof(filenameN), fileName), mode) : nullptr;
}

Dir* OpenDir(const char* dirName, uint8_t attrExclude)
{
	char dirNameN[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) return nullptr;
	std::unique_ptr<Dir> pDir(pDrive->OpenDir(pDrive->NativePathName(dirNameN, sizeof(dirNameN), dirName), dirName, attrExclude));
	return pDir.release();
	
}

Glob* OpenGlob(const char* pathName, const FS::FileInfo::Cmp& cmp, bool patternAsDirFlag, uint8_t attrExclude)
{
	std::unique_ptr<Glob> pGlob(new Glob());
	return pGlob->Open(pathName, cmp, patternAsDirFlag)? pGlob.release() : nullptr;
}

bool Touch(Printable& terr, const char* pathName, const DateTime& dt)
{
	if (!FS::DoesExist(pathName)) {
		std::unique_ptr<FS::File> pFile(FS::OpenFile(pathName, "w"));
		if (!pFile) {
			terr.Printf("failed to create %s\n", pathName);
			return false;
		}
	}
	char pathNameN[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive || !pDrive->SetTimeStamp(pDrive->NativePathName(pathNameN, sizeof(pathNameN), pathName), dt)) {
		terr.Printf("failed to set time for %s\n", pathName);
		return false;
	}
	return true;
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

bool ListDrives(Printable& tout, const char* driveName, bool remarksFlag)
{
	const char* labelDriveName = "Drive";
	const char* labelFormatName = "Format";
	int lenMaxDriveName = ::strlen(labelDriveName) + 1, lenMaxFormatName = ::strlen(labelFormatName) + 1;
	for (FS::Drive* pDrive = FS::GetDriveHead(); pDrive; pDrive = pDrive->GetNext()) {
		if (driveName && !FS::DoesMatchDriveName(pDrive->GetDriveName(), driveName)) continue;
		lenMaxDriveName = ChooseMax(lenMaxDriveName, ::strlen(pDrive->GetDriveName()) + 1);
		lenMaxFormatName = ChooseMax(lenMaxFormatName, ::strlen(pDrive->GetFileSystemName()) + 1);
	}
	char remarks[80];
	tout.Printf(" %*s %*s %11s%s\n",
			-lenMaxDriveName, labelDriveName, -lenMaxFormatName, labelFormatName,
			"Total", remarksFlag? " Remarks" : "");
	for (FS::Drive* pDrive = FS::GetDriveHead(); pDrive; pDrive = pDrive->GetNext()) {
		if (driveName && !FS::DoesMatchDriveName(pDrive->GetDriveName(), driveName)) continue;
		char buff[32];
		::snprintf(buff, sizeof(buff), "%s:", pDrive->GetDriveName());
		tout.Printf("%s%*s %*s %11lld%s%s\n",
			pDrive->IsPrimary()? "*" : " ", -lenMaxDriveName, buff, -lenMaxFormatName, pDrive->GetFileSystemName(),
			pDrive->GetBytesTotal(), remarksFlag? " " : "", remarksFlag? pDrive->GetRemarks(remarks, sizeof(remarks)) : "");
	}
	return true;
}

bool ListFiles(Printable& terr, Printable& tout, const char* pathName, const FileInfo::Cmp& cmp, uint8_t attrExclude, bool slashForDirFlag)
{
	std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pathName, cmp, true, attrExclude));
	if (!pGlob) {
		terr.Printf("failed to open %s\n", pathName);
		return false;
	}
	std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->ReadAll());
	if (pFileInfo) pFileInfo->PrintList(tout, slashForDirFlag);
	return true;
}

bool Copy(Printable& terr, const char* pathNameSrc, const char* pathNameDst, bool recursiveFlag, bool verboseFlag, bool forceFlag)
{
	bool rtn = true;
	if (IsDirectory(pathNameSrc)) {
		if (!recursiveFlag) {
			terr.Printf("cannot copy directory %s\n", pathNameSrc);
			return false;
		}
		char pathNameDstBase[MaxPath];
		std::unique_ptr<FileInfo> pFileInfoDst(GetFileInfo(pathNameDst));
		if (!pFileInfoDst) {
			::snprintf(pathNameDstBase, sizeof(pathNameDstBase), "%s", pathNameDst);
		} else if (pFileInfoDst->IsDirectory()) {
			JoinPathName(pathNameDstBase, sizeof(pathNameDstBase), pathNameDst, ExtractBottomName(pathNameSrc));
			std::unique_ptr<FileInfo> pFileInfoDstBase(GetFileInfo(pathNameDstBase));
			if (!pFileInfoDstBase) {
				// nothing to do
			} else if (forceFlag) {
				Remove(terr, pathNameDstBase, true, verboseFlag);
			} else {
				terr.Printf("already exists: %s\n", pathNameDstBase);
				return false;
			}
		} else if (forceFlag) {
			Remove(terr, pathNameDst, true, verboseFlag);
			::snprintf(pathNameDstBase, sizeof(pathNameDstBase), "%s", pathNameDst);
		} else {
			terr.Printf("already exists: %s\n", pathNameDst);
			return false;
		}
		if (!CreateDir(terr, pathNameDstBase)) return false;
		if (verboseFlag) terr.Printf("created dir: %s\n", pathNameDstBase);
		Walker walker(false); // fileFirstFlag = false: read directories first
		if (!walker.Open(pathNameSrc, 0)) {
			terr.Printf("failed to open directory %s\n", pathNameSrc);
			return false; // Open directory failed
		}
		for (;;) {
			const char* pathNameSrcEach;
			std::unique_ptr<FileInfo> pFileInfo(walker.Read(&pathNameSrcEach));
			if (!pFileInfo) break; // No more files
			char pathNameDstEach[MaxPath];
			JoinPathName(pathNameDstEach, sizeof(pathNameDstEach), pathNameDstBase, walker.GetPathNameSub());
			if (pFileInfo->IsDirectory()) {
				if (!CreateDir(terr, pathNameDstEach)) {
					rtn = false;
					break;
				} else if (verboseFlag) {
					terr.Printf("created dir: %s\n", pathNameDstEach);
				}
			} else {
				if (!CopyFile(terr, pathNameSrcEach, pathNameDstEach, forceFlag)) {
					rtn = false;
				} else if (verboseFlag) {
					terr.Printf("copied file: %s -> %s\n", pathNameSrcEach, pathNameDstEach);
				}
			}
		}
	} else {
		if (!CopyFile(terr, pathNameSrc, pathNameDst, forceFlag)) {
			rtn = false;
		} else if (verboseFlag) {
			terr.Printf("%s\n", pathNameSrc);
		}
	}
	return rtn;
}

bool CopyFile(Printable& terr, const char* fileNameSrc, const char* pathNameDst, bool forceFlag)
{
	std::unique_ptr<FS::File> pFileSrc(FS::OpenFile(fileNameSrc, "r"));
	if (!pFileSrc) {
		terr.Printf("failed to open %s\n", fileNameSrc);
		return false;
	}
	std::unique_ptr<FS::File> pFileDst;
	if (FS::IsDirectory(pathNameDst)) {
		Drive* pDriveDst = FindDrive(pathNameDst);
		if (pDriveDst) {
			char fileNameDst[MaxPath], fileNameDstN[MaxPath];
			::snprintf(fileNameDst, sizeof(fileNameDst), "%s", pathNameDst);
			AppendPathName(fileNameDst, sizeof(fileNameDst), ExtractFileName(fileNameSrc));
			pDriveDst->NativePathName(fileNameDstN, sizeof(fileNameDstN), fileNameDst);
			std::unique_ptr<FileInfo> pFileInfoDst(pDriveDst->GetFileInfo(fileNameDstN));
			if (!pFileInfoDst) {
				// nothing to do
			} else if (forceFlag) {
				if (pFileInfoDst->IsDirectory()) Remove(fileNameDst, true, false);
			} else {
				terr.Printf("already exists: %s\n", fileNameDst);
				return false;
			}
			pFileDst.reset(pDriveDst->OpenFile(fileNameDstN, "w"));
		}
	} else {
		std::unique_ptr<FileInfo> pFileInfoDst(GetFileInfo(pathNameDst));
		if (!pFileInfoDst) {
			// nothing to do
		} else if (forceFlag) {
			if (pFileInfoDst->IsDirectory()) Remove(pathNameDst, true, false);
		} else {
			terr.Printf("already exists: %s\n", pathNameDst);
			return false;
		}
		pFileDst.reset(OpenFile(pathNameDst, "w"));
	}
	if (!pFileDst) {
		terr.Printf("failed to open %s\n", pathNameDst);
		return false;
	}
	int bytesRead;
	char buff[512];
	while ((bytesRead = pFileSrc->Read(buff, sizeof(buff))) > 0) {
		if (pFileDst->Write(buff, bytesRead) != bytesRead) {
			terr.Printf("failed to write %s\n", pathNameDst);
			return false;
		}
	}
	return true;
}

bool Remove(Printable& terr, const char* pathName, bool recursiveFlag, bool verboseFlag)
{
	if (IsDirectory(pathName)) {
		if (!recursiveFlag) {
			terr.Printf("cannot remove directory %s as file\n", pathName);
			return false;
		}
		Walker walker(true); // fileFirstFlag = true: read files first
		if (!walker.Open(pathName, 0)) {
			terr.Printf("failed to open directory %s\n", pathName);
			return false; // Open directory failed
		}
		for (;;) {
			const char* pathName;
			std::unique_ptr<FileInfo> pFileInfo(walker.Read(&pathName));
			if (!pFileInfo) break; // No more files
			if (pFileInfo->IsDirectory()) {
				if (!RemoveDir(pathName)) {
					return false;
				} else if (verboseFlag) {
					terr.Printf("removed dir:  %s\n", pathName);
				}
			} else {
				if (!RemoveFile(pathName)) {
					return false;
				} else if (verboseFlag) {
					terr.Printf("removed file: %s\n", pathName);
				}
			}
		}
		return RemoveDir(terr, pathName);
	} else {
		return RemoveFile(terr, pathName);
	}
}

bool RemoveFile(Printable& terr, const char* fileName)
{
	char pathNameN[MaxPath];
	Drive* pDrive = FindDrive(fileName);
	if (!pDrive) {
		terr.Printf("drive for file %s not found\n", fileName);
		return false; // Drive not found
	}
	pDrive->NativePathName(pathNameN, sizeof(pathNameN), fileName);
	if (pDrive->IsDirectory(pathNameN)) {
		terr.Printf("cannot remove directory %s as file\n", fileName);
	} else {
		if (pDrive->RemoveFile(pathNameN)) return true;
		terr.Printf("failed to remove %s\n", fileName);
	}
	return false;
}

bool RemoveDir(Printable& terr, const char* dirName)
{
	char pathNameN[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) {
		terr.Printf("drive for direcory %s not found\n", dirName);
		return false; // Drive not found
	}
	pDrive->NativePathName(pathNameN, sizeof(pathNameN), dirName);
	if (pDrive->IsDirectory(pathNameN)) {
		if (pDrive->RemoveDir(pathNameN)) return true;
		terr.Printf("failed to remove %s\n", dirName);
	} else {
		terr.Printf("cannot remove %s as directory\n", dirName);
	}
	return false;
}

bool Move(Printable& terr, const char* pathNameOld, const char* pathNameNew, bool verboseFlag, bool forceFlag)
{
	char pathNameOldN[MaxPath], pathNameNewN[MaxPath];
	Drive* pDriveOld = FindDrive(pathNameOld);
	if (!pDriveOld) {
		terr.Printf("drive for old file %s not found\n", pathNameOld);
		return false; // Drive not found
	}	
	Drive* pDriveNew = FindDrive(pathNameNew);
	if (!pDriveNew) {
		terr.Printf("drive for new file %s not found\n", pathNameNew);
		return false; // Drive not found
	}
	if (pDriveOld == pDriveNew) {
		pDriveOld->NativePathName(pathNameNewN, sizeof(pathNameNewN), pathNameNew);
		if (FS::IsDirectory(pathNameNew)) {
			AppendPathName(pathNameNewN, sizeof(pathNameNewN), ExtractBottomName(pathNameOld));
		}
		if (pDriveOld->Rename(pDriveOld->NativePathName(pathNameOldN, sizeof(pathNameOldN), pathNameOld), pathNameNewN)) return true;
		terr.Printf("failed to rename %s to %s\n", pathNameOld, pathNameNew);
		return false; // Rename failed
	}
	bool recursiveFlag = true;
	return Copy(terr, pathNameOld, pathNameNew, recursiveFlag, verboseFlag, forceFlag) && Remove(terr, pathNameOld, recursiveFlag, verboseFlag);
}

bool CreateDir(Printable& terr, const char* dirName)
{
	char dirNameN[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) {
		terr.Printf("drive for direcory %s not found\n", dirName);
		return false; // Drive not found
	}
	if (pDrive->CreateDir(pDrive->NativePathName(dirNameN, sizeof(dirNameN), dirName))) return true;
	terr.Printf("failed to create directory %s\n", dirName);
	return false;
}

bool ChangeCurDir(Printable& terr, const char* dirName)
{
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) {
		terr.Printf("drive for direcory %s not found\n", dirName);
		return false; // Drive not found
	}
	char pathName[MaxPath];
	char dirNameN[MaxPath];
	dirName = pDrive->RegulatePathName(pathName, sizeof(pathName), dirName);
	if (pDrive->IsDirectory(pDrive->NativePathName(dirNameN, sizeof(dirNameN), dirName))) {
		pDrive->SetDirNameCur(dirName);
		return true;
	}
	terr.Printf("failed to change current directory to %s\n", dirName);
	return false;
}

bool DoesExist(const char* pathName)
{
	char pathNameN[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive) return false;
	return pDrive->DoesExist(pDrive->NativePathName(pathNameN, sizeof(pathNameN), pathName));
}

bool IsDirectory(const char* pathName)
{
	char pathNameN[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive) return false;
	return pDrive->IsDirectory(pDrive->NativePathName(pathNameN, sizeof(pathNameN), pathName));
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

bool CheckMounted(const char* driveName)
{
	if (!IsLegalDriveName(driveName)) return false;
	Drive* pDrive = FindDrive(driveName);
	if (!pDrive) return false;
	return pDrive->CheckMounted();
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
	return pDrive->Mount();
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
	return pDrive->Unmount();
}

FileInfo* GetFileInfo(const char* pathName)
{
	char pathNameN[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive) return nullptr;
	return pDrive->GetFileInfo(pDrive->NativePathName(pathNameN, sizeof(pathNameN), pathName));
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
	for ( ; *p != '\0'; p++) {
		if (*p == '/' || *p == ':') fileName = p + 1; // remember the start of the file name
	}
	return fileName? fileName : pathName; // return the last part or the whole path if no slashes
}

const char* ExtractBottomName(const char* pathName)
{
	const char* p = pathName;
	const char* bottohName = nullptr;
	for ( ; *p != '\0'; p++) {
		if ((*p == '/' || *p == ':') && *(p + 1) != '\0') bottohName = p + 1; // remember the start of the bottom name
	}
	return bottohName? bottohName : pathName; // return the last part or the whole path if no slashes
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

FileInfo::FileInfo() : name_{nullptr}, attr_{0}, size_{0} {}

FileInfo::FileInfo(const char* name, uint8_t attr, uint32_t size, const DateTime& dateTime) :
			name_{new char[::strlen(name) + 1]}, attr_{attr}, size_{size}, dateTime_{dateTime}
{
	::strcpy(name_.get(), name);
}

void FileInfo::PrintList(Printable& tout, bool slashForDirFlag) const
{
	int lenMax = 6;
	char buff1[32], buff2[64];;
	for (const FileInfo* pFileInfo = this; pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		lenMax = ChooseMax(lenMax, ::snprintf(buff1, sizeof(buff1), "%d", pFileInfo->GetSize()));
	}
	for (const FileInfo* pFileInfo = this; pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		if (pFileInfo->IsDirectory()) {
			tout.Printf("%s %s %*s %s%s\n",
				pFileInfo->MakeAttrString(buff1, sizeof(buff1)),
				pFileInfo->MakeDateTimeString(buff2, sizeof(buff2)), lenMax, "", pFileInfo->GetName(),
				slashForDirFlag? "/" : "");
		} else if (pFileInfo->IsFile()) {
			tout.Printf("%s %s %*d %s\n",
				pFileInfo->MakeAttrString(buff1, sizeof(buff1)),
				pFileInfo->MakeDateTimeString(buff2, sizeof(buff2)), lenMax, pFileInfo->GetSize(), pFileInfo->GetName());
		}
	}
}

void FileInfo::PrintTree(Printable& tout, bool dirOnlyFlag, bool slashForDirFlag, char* strBranch, int lenBranchMax) const
{
	for (const FileInfo* pFileInfo = this; pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		if (pFileInfo->IsDirectory()) {
			bool contFlag = false;
			if (dirOnlyFlag) {
				for (const FileInfo* pFileInfoTmp = pFileInfo->GetNext(); pFileInfoTmp; pFileInfoTmp = pFileInfoTmp->GetNext()) {
					if (pFileInfoTmp->IsDirectory()) {
						contFlag = true; // there is a next directory
						break;
					}
				}
			} else {
				contFlag = !!pFileInfo->GetNext();
			}
			tout.Printf("%s%s%s%s\n", strBranch, contFlag? "├── " : "└── ", pFileInfo->GetName(), slashForDirFlag? "/" : "");
			const char* strBranchAdded = contFlag? "│   " : "    ";
			int lenBranchAdded = ::strlen(strBranchAdded);
			if (::strlen(strBranch) + lenBranchAdded + 1 < lenBranchMax) {
				::strcat(strBranch, strBranchAdded);
			} else {
				lenBranchAdded = 0;
			}
			if (pFileInfo->GetChild()) pFileInfo->GetChild()->PrintTree(tout, dirOnlyFlag, slashForDirFlag, strBranch, lenBranchMax);
			int lenBranch = ::strlen(strBranch);
			strBranch[lenBranch - lenBranchAdded] = '\0';
		} else if (!dirOnlyFlag) {
			bool contFlag = !!pFileInfo->GetNext();
			tout.Printf("%s%s%s\n", strBranch, contFlag? "├── " : "└── ", pFileInfo->GetName());
		}
	}
}

const char* FileInfo::JoinPathName(char* pathName, int lenBuff) const
{
	pathName[0] = '\0';
	for (const FileInfo* pFileInfo = this; pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		AppendPathName(pathName, lenBuff, pFileInfo->GetName());
	}
	return pathName;
}

const char* FileInfo::MakeAttrString(char* buff, int lenBuff) const
{
	::snprintf(buff, lenBuff, "%c%c%c%c%c",
		IsDirectory()? 'd' : '-',
		IsArchive()? 'a' : '-',
		IsReadOnly()? 'r' : '-',
		IsHidden()? 'h' : '-',
		IsSystem()? 's' : '-');
	return buff;
}

const char* FileInfo::MakeDateTimeString(char* buff, int lenBuff) const
{
	::snprintf(buff, lenBuff, "%04d-%02d-%02d %02d:%02d:%02d",
			dateTime_.year, dateTime_.month, dateTime_.day, dateTime_.hour, dateTime_.min, dateTime_.sec);
	return buff;
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
	return static_cast<int>(fileInfo1.IsFile()) - static_cast<int>(fileInfo2.IsFile());
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

const FileInfo::Cmp_DateTime FileInfo::Cmp_DateTime::Ascent(+1);
const FileInfo::Cmp_DateTime FileInfo::Cmp_DateTime::Descent(-1);

int FileInfo::Cmp_DateTime::DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const
{
	return DateTime::Compare(fileInfo1.GetDateTime(), fileInfo2.GetDateTime());
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
FileInfo* FileInfoReader::ReadAll(const FileInfo::Cmp& cmp, const char* pattern)
{
	if (*pattern == '\0') pattern = nullptr;
	std::unique_ptr<FileInfo> pFileInfoHead;
	for (;;) {
		std::unique_ptr<FileInfo> pFileInfoToAdd(Read());
		if (!pFileInfoToAdd) break; // No more files to read
		if (pattern && !DoesMatchWildcard(pattern, pFileInfoToAdd->GetName())) continue; // Skip files that do not match the pattern
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
Dir::Dir(Drive& drive, const char* dirName) : drive_(drive), rewindFlag_{false}
{
	::snprintf(dirName_, sizeof(dirName_), "%s", dirName);
}

Dir* Dir::RemoveBottomChild()
{
	Dir* pDirPrev = nullptr;
	for (Dir* pDir = this; pDir->GetChild(); pDir = pDir->GetChild()) pDirPrev = pDir;
	if (pDirPrev) {
		pDirPrev->SetChild(nullptr);
		return pDirPrev;
	}
	return this;
}

FileInfo* Dir::ReadAllRecursive(const FileInfo::Cmp& cmp, const char* pattern, bool* pSuccessFlag)
{
	uint8_t attrExclude = 0;
	if (pSuccessFlag) *pSuccessFlag = true;
	std::unique_ptr<FileInfo> pFileInfoTop(ReadAll(cmp, pattern));
	for (FileInfo* pFileInfo = pFileInfoTop.get(); pFileInfo; pFileInfo = pFileInfo->GetNext()) {
		if (pFileInfo->IsDirectory()) {
			char dirNameChild[MaxPath], dirNameChildN[MaxPath];
			JoinPathName(dirNameChild, sizeof(dirNameChild), GetDirName(), pFileInfo->GetName());
			std::unique_ptr<Dir> pDirChild(drive_.OpenDir(
				drive_.NativePathName(dirNameChildN, sizeof(dirNameChildN), dirNameChild), dirNameChild, attrExclude));
			if (!pDirChild) {
				if (pSuccessFlag) *pSuccessFlag = false;
				return nullptr;
			}
			pFileInfo->SetChild(pDirChild->ReadAllRecursive(cmp, pattern, pSuccessFlag));
		}
		if (Tickable::TickSub()) return nullptr; // Interrupted
	}
	return pFileInfoTop.release();
}

//------------------------------------------------------------------------------
// FS::Glob
//------------------------------------------------------------------------------
Glob::Glob() : pattern_{""}
{
}

bool Glob::Open(const char* pathName, const FileInfo::Cmp& cmp, bool patternAsDirFlag, uint8_t attrExclude)
{
	std::unique_ptr<Dir> pDir;
	if (patternAsDirFlag) pDir.reset(OpenDir(pathName));
	if (pDir) {
		::snprintf(dirName_, sizeof(dirName_), "%s", pathName);
		pattern_ = "";
	} else {
		SplitDirName(pathName, dirName_, sizeof(dirName_), &pattern_);
		pDir.reset(OpenDir(dirName_, attrExclude));
		if (!pDir) return false;
	}
	pFileInfoTop_.reset(pDir->ReadAll(cmp, pattern_));
	return true;
}

FileInfo* Glob::Read(const char** pPathName)
{
	std::unique_ptr<FileInfo> pFileInfo(pFileInfoTop_.release());
	if (pFileInfo) {
		pFileInfoTop_.reset(pFileInfo->ReleaseNext());
		if (pPathName) {
			JoinPathName(pathName_, sizeof(pathName_), dirName_, pFileInfo->GetName());
			*pPathName = pathName_;
		}
	}
	return pFileInfo.release();
}

//------------------------------------------------------------------------------
// FS::Walker
//------------------------------------------------------------------------------
Walker::Walker(bool fileFirstFlag) : fileFirstFlag_{fileFirstFlag}, attrExclude_{0}, pDirCur_{nullptr}, pathNameSub_{""}
{
	pathName_[0] = '\0';
}

bool Walker::Open(const char* dirName, uint8_t attrExclude)
{
	attrExclude_ = attrExclude;
	pDirTop_.reset(OpenDir(dirName, attrExclude_));
	if (!pDirTop_) return false;
	pDirCur_ = pDirTop_.get();
	pathName_[0] = '\0';
	AppendPathName(pathName_, sizeof(pathName_) - 1, pDirCur_->GetDirName());
	int len = ::strlen(pathName_);
	pathName_[len + 1] = '\0';
	pathNameSub_ = (len == 0)? pathName_ : (pathName_[len - 1] == '/')? pathName_ + len : pathName_ + len + 1;
	return true;
}

FileInfo* Walker::Read(const char** pPathName)
{
	if (pPathName) *pPathName = pathName_;
	std::unique_ptr<FileInfo> pFileInfo;
	for (;;) {
		pFileInfo.reset(pDirCur_->Read());
		if (pFileInfo) {
			JoinPathName(pathName_, sizeof(pathName_), pDirCur_->GetDirName(), pFileInfo->GetName());
			if (!pFileInfo->IsDirectory()) break;
			Dir* pDir = OpenDir(pathName_, attrExclude_);
			if (!pDir) return nullptr;
			pDirCur_->SetChild(pDir);
			pDirCur_ = pDir;
			if (fileFirstFlag_) {
				pDir->SetFileInfo(pFileInfo.release());
			} else {
				break;
			}
		} else {
			if (pDirCur_ == pDirTop_.get()) return nullptr;
			if (fileFirstFlag_) {
				pFileInfo.reset(pDirCur_->ReleaseFileInfo());
				Dir* pDir = pDirTop_->RemoveBottomChild();
				JoinPathName(pathName_, sizeof(pathName_), pDir->GetDirName(), pFileInfo->GetName());
				pDirCur_ = pDir;
				break;
			} else {
				pDirCur_ = pDirTop_->RemoveBottomChild();
			}
		}
	}
	return pFileInfo.release();
}

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
Drive::Drive(const char* formatName, const char* driveName) : mountedFlag_{false}, formatName_{formatName}, pDriveNext_{nullptr}
{
	do {
		const char* driveNameEnd = ::strchr(driveName, ':');
		int len = ChooseMin(driveNameEnd? driveNameEnd - driveName : ::strlen(driveName), MaxDriveName);
		::memcpy(driveNameRaw_, driveName, len);
		driveNameRaw_[len] = '\0';
		driveName_ = ::isalpha(driveNameRaw_[0])? driveNameRaw_ : driveNameRaw_ + 1;
	} while (0);
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

bool Drive::DoesExist(const char* pathNameN)
{
	std::unique_ptr<FileInfo> pFileInfo(GetFileInfo(pathNameN));
	return !!pFileInfo;
}

bool Drive::IsDirectory(const char* pathNameN)
{
	std::unique_ptr<FileInfo> pFileInfo(GetFileInfo(pathNameN));
	return pFileInfo && pFileInfo->IsDirectory();
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
