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

const char* CreateFileNameForMove(char* fileName, int lenMax, const char* pathNameSrc, const char* pathNameDst)
{
	if (FS::IsDirectory(pathNameDst)) {
		::snprintf(fileName, lenMax, "%s", pathNameDst);
		AppendPathName(fileName, lenMax, ExtractFileName(pathNameSrc));
	} else {
		::snprintf(fileName, lenMax, "%s", pathNameDst);
	}
	//::snprintf(fileName, lenMax, "%s", pathNameDst);
	return fileName;
}

File* OpenFileForCopy(const char* pathNameSrc, const char* pathNameDst)
{
	if (FS::IsDirectory(pathNameDst)) {
		char fileNameBuff[MaxPath];
		Drive* pDrive = FindDrive(pathNameDst);
		if (!pDrive) return nullptr;
		pathNameDst = SkipDriveName(pathNameDst);
		pDrive->RegulatePathName(fileNameBuff, sizeof(fileNameBuff), pathNameDst);
		AppendPathName(fileNameBuff, sizeof(fileNameBuff), ExtractFileName(pathNameSrc));
		return OpenFile(fileNameBuff, "w", pDrive);
	} else {
		return OpenFile(pathNameDst, "w");
	}
}

Dir* OpenDir(const char* dirName, uint8_t attrExclude)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) return nullptr;
	std::unique_ptr<Dir> pDir(pDrive->OpenDir(
			pDrive->NativePathName(pathName, sizeof(pathName), dirName),
			attrExclude));
	if (pDir) pDir->SetDirName(dirName);	
	return pDir.release();
	
}

Glob* OpenGlob(const char* pattern, bool patternAsDirFlag, uint8_t attrExclude)
{
	std::unique_ptr<Glob> pGlob(new Glob());
	return pGlob->Open(pattern, patternAsDirFlag)? pGlob.release() : nullptr;
}

bool SetTimeStamp(const char* pathName, const DateTime& dt)
{
	char pathNameBuff[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	return pDrive? pDrive->SetTimeStamp(pDrive->NativePathName(pathNameBuff, sizeof(pathNameBuff), pathName), dt) : false;
}

bool Touch(Printable& terr, const char* pathName)
{
	if (FS::DoesExist(pathName)) {
		DateTime dt;
		RTC::Get(&dt);
		if (!FS::SetTimeStamp(pathName, dt)) {
			terr.Printf("failed to set time for %s\n", pathName);
			return false;
		}
	} else {
		std::unique_ptr<FS::File> pFile(FS::OpenFile(pathName, "w"));
		if (!pFile) {
			terr.Printf("failed to create %s\n", pathName);
			return false;
		}
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
	std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pathName, true, attrExclude));
	if (!pGlob) {
		terr.Printf("failed to open %s\n", pathName);
		return false;
	}
	std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->ReadAll(cmp));
	if (pFileInfo) pFileInfo->PrintList(tout, slashForDirFlag);
	return true;
}

bool CopyFile(Printable& terr, const char* pathNameSrc, const char* pathNameDst)
{
	std::unique_ptr<FS::File> pFileSrc(FS::OpenFile(pathNameSrc, "r"));
	if (!pFileSrc) {
		terr.Printf("failed to open %s\n", pathNameSrc);
		return false;
	}
	std::unique_ptr<FS::File> pFileDst(OpenFileForCopy(pathNameSrc, pathNameDst));
	if (!pFileDst) {
		terr.Printf("failed to open %s\n", pathNameDst);
		return false;
	}
	int bytesRead;
	char buff[128];
	while ((bytesRead = pFileSrc->Read(buff, sizeof(buff))) > 0) {
		if (pFileDst->Write(buff, bytesRead) != bytesRead) {
			terr.Printf("failed to write %s\n", pathNameDst);
			return false;
		}
	}
	return true;
}

bool RemoveFile(Printable& terr, const char* fileName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(fileName);
	if (!pDrive) {
		terr.Printf("drive for file %s not found\n", fileName);
		return false; // Drive not found
	}
	if (pDrive->RemoveFile(pDrive->NativePathName(pathName, sizeof(pathName), fileName))) return true;
	terr.Printf("failed to remove %s\n", fileName);
	return false;
}

bool Move(Printable& terr, const char* pathNameOld, const char* pathNameNew)
{
	char pathNameBuffOld[MaxPath], pathNameBuffNew[MaxPath];
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
		// Same drive, just rename
		char pathNameBuff[MaxPath];
		CreateFileNameForMove(pathNameBuff, sizeof(pathNameBuff), pathNameOld, pathNameNew);
		if (pDriveOld->Rename(pDriveOld->NativePathName(pathNameBuffOld, sizeof(pathNameBuffOld), pathNameOld),
				pDriveOld->NativePathName(pathNameBuffNew, sizeof(pathNameBuffNew), pathNameBuff))) return true;
		terr.Printf("failed to rename %s to %s\n", pathNameOld, pathNameNew);
		return false; // Rename failed
	}
	if (IsDirectory(pathNameOld)) {
		terr.Printf("cannot move directory %s to %s\n", pathNameOld, pathNameNew);
		return false; // Cannot move directory
	}
	return CopyFile(terr, pathNameOld, pathNameNew) && RemoveFile(terr, pathNameOld); // Copy and then remove old file
}

bool CreateDir(Printable& terr, const char* dirName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) {
		terr.Printf("drive for direcory %s not found\n", dirName);
		return false; // Drive not found
	}
	if (pDrive->CreateDir(pDrive->NativePathName(pathName, sizeof(pathName), dirName))) return true;
	terr.Printf("failed to create directory %s\n", dirName);
	return false;
}

bool RemoveDir(Printable& terr, const char* dirName)
{
	char pathName[MaxPath];
	Drive* pDrive = FindDrive(dirName);
	if (!pDrive) {
		terr.Printf("drive for direcory %s not found\n", dirName);
		return false; // Drive not found
	}
	if (pDrive->RemoveDir(pDrive->NativePathName(pathName, sizeof(pathName), dirName))) return true;
	terr.Printf("failed to remove %s\n", dirName);
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
	char pathNameNative[MaxPath];
	dirName = pDrive->RegulatePathName(pathName, sizeof(pathName), dirName);
	if (pDrive->IsDirectory(pDrive->NativePathName(pathNameNative, sizeof(pathNameNative), dirName))) {
		pDrive->SetDirNameCur(dirName);
		return true;
	}
	terr.Printf("failed to change current directory to %s\n", dirName);
	return false;
}

bool DoesExist(const char* pathName)
{
	char pathNameBuff[MaxPath];
	Drive* pDrive = FindDrive(pathName);
	if (!pDrive) return false;
	return pDrive->DoesExist(pDrive->NativePathName(pathNameBuff, sizeof(pathNameBuff), pathName));
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
Dir::Dir(const Drive& drive, const char* dirName) : drive_(drive), rewindFlag_{false}
{
	::snprintf(dirName_, sizeof(dirName_), "%s", dirName);
}

Dir* Dir::RemoveLast()
{
	Dir* pDirPrev = nullptr;
	for (Dir* pDir = this; pDir->GetNext(); pDir = pDir->GetNext()) {
		pDirPrev = pDir;
	}
	if (pDirPrev) {
		pDirPrev->SetNext(nullptr);
		return pDirPrev;
	}
	return this;
}

//------------------------------------------------------------------------------
// FS::Glob
//------------------------------------------------------------------------------
Glob::Glob(): pattern_{""}
{
}

bool Glob::Open(const char* pattern, bool patternAsDirFlag, uint8_t attrExclude)
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
	pDir_.reset(OpenDir(dirName_, attrExclude));
	return !!pDir_;
}

void Glob::Close()
{
	pDir_.reset();
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

//------------------------------------------------------------------------------
// FS::Walker
//------------------------------------------------------------------------------
Walker::Walker() : attrExclude_{0}, pDirCur_{nullptr}
{
	pathName_[0] = '\0';
}

bool Walker::Open(const char* dirName, uint8_t attrExclude)
{
	attrExclude_ = attrExclude;
	pDirTop_.reset(OpenDir(dirName, attrExclude_));
	if (!pDirTop_) return false;
	pDirCur_ = pDirTop_.get();
	return true;
}

FileInfo* Walker::Read(const char** pPathName)
{
	std::unique_ptr<FileInfo> pFileInfo;
	for (;;) {
		pFileInfo.reset(pDirCur_->Read());
		if (pFileInfo) break;
		if (pDirCur_ == pDirTop_.get()) return nullptr;
		pDirCur_ = pDirTop_->RemoveLast();
	}
	if (pPathName) *pPathName = pathName_;
	JoinPathName(pathName_, sizeof(pathName_), pDirCur_->GetDirName(), pFileInfo->GetName());
	if (pFileInfo->IsDirectory()) {
		Dir* pDir = OpenDir(pathName_, attrExclude_);
		if (!pDir) return nullptr;
		pDirCur_->SetNext(pDir);
		pDirCur_ = pDir;
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

bool Drive::DoesExist(const char* pathName)
{
	std::unique_ptr<FileInfo> pFileInfo(GetFileInfo(pathName));
	return !!pFileInfo;
}

bool Drive::IsDirectory(const char* pathName)
{
	std::unique_ptr<Dir> pDir(OpenDir(pathName, 0));
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
