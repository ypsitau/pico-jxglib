#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/FS.h"

namespace jxglib::ShellCmd_FS {

ShellCmd(cat, "prints the contents of files")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	for (int i = 1; i < argc; i++) {
		const char* fileName = argv[i];
		RefPtr<FS::File> pFile(FS::OpenFile(fileName, "r"));
		if (!pFile) {
			tout.Printf("failed to open %s\n", fileName);
			return 1;
		}
		int bytes;
		char buff[128];
		while ((bytes = pFile->Read(buff, sizeof(buff) - 1)) > 0) {
			buff[bytes] = '\0';
			tout.Print(buff);
		}
		pFile->Close();
	}
	return 0;
}

ShellCmd(cp, "copies a file")
{
	if (argc < 3) {
		tout.Printf("Usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	RefPtr<FS::File> pFileSrc(FS::OpenFile(fileNameSrc, "r"));
	if (!pFileSrc) {
		tout.Printf("failed to open %s\n", fileNameSrc);
		return 1;
	}
	std::unique_ptr<FS::FileInfo> pFileInfo(FS::GetFileInfo(fileNameDst));
	//if (pFileInfo) {
	//	if (pFileInfo->IsDirectory()) {
	//	}
	//}
	RefPtr<FS::File> pFileDst(FS::OpenFile(fileNameDst, "w"));
	if (!pFileDst) {
		tout.Printf("failed to open %s\n", fileNameDst);
		return 1;
	}
	int bytesRead;
	char buff[128];
	while ((bytesRead = pFileSrc->Read(buff, sizeof(buff))) > 0) {
		if (pFileDst->Write(buff, bytesRead) != bytesRead) {
			tout.Printf("failed to write %s\n", fileNameDst);
			return 1;
		}
	}
	pFileSrc->Close();
	pFileDst->Close();
	return 0;
}

ShellCmd(ls, "lists files in the specified directory")
{
	const char* dirName = (argc < 2)? "" : argv[1];
	RefPtr<FS::Dir> pDir(FS::OpenDir(dirName));
	if (!pDir) {
		tout.Printf("failed to open %s\n", dirName);
		return 1;
	}
	FS::FileInfo* pFileInfo;
	while (pDir->Read(&pFileInfo)) {
		if (pFileInfo->IsDirectory()) {
			tout.Printf("%-20s <DIR>\n", pFileInfo->GetName());
		} else if (pFileInfo->IsFile()) {
			tout.Printf("%-20s %d\n", pFileInfo->GetName(), pFileInfo->GetSize());
		}
	}
	pDir->Close();
	return 0;
}

ShellCmd_Named(ls_drive, "ls-drive", "lists availabld drives")
{
	const char* labelDriveName = "Drive";
	const char* labelFormatName = "Format";
	int lenMaxDriveName = ::strlen(labelDriveName) + 1, lenMaxFormatName = ::strlen(labelFormatName) + 1;
	for (FS::Drive* pDrive = FS::GetDriveHead(); pDrive; pDrive = pDrive->GetNext()) {
		lenMaxDriveName = ChooseMax(lenMaxDriveName, ::strlen(pDrive->GetDriveName()) + 1);
		lenMaxFormatName = ChooseMax(lenMaxFormatName, ::strlen(pDrive->GetFileSystemName()) + 1);
	}
#if 0
	tout.Printf("%*s  %*s %11s/%11s\n",
			-lenMaxDriveName, labelDriveName, -lenMaxFormatName, labelFormatName,
			"Used", "Total");
	for (FS::Drive* pDrive = FS::GetDriveHead(); pDrive; pDrive = pDrive->GetNext()) {
		char buff[32];
		::snprintf(buff, sizeof(buff), "%s:", pDrive->GetDriveName());
		tout.Printf("%*s  %*s %11lld/%11lld\n",
				-lenMaxDriveName, buff, -lenMaxFormatName, pDrive->GetFileSystemName(),
				pDrive->GetBytesUsed(), pDrive->GetBytesTotal());
	}
#endif
	char remarks[80];
	tout.Printf("%*s %*s %11s Remarks\n",
			-lenMaxDriveName, labelDriveName, -lenMaxFormatName, labelFormatName,
			"Total");
	for (FS::Drive* pDrive = FS::GetDriveHead(); pDrive; pDrive = pDrive->GetNext()) {
		char buff[32];
		::snprintf(buff, sizeof(buff), "%s:", pDrive->GetDriveName());
		tout.Printf("%*s %*s %11lld %s\n",
				-lenMaxDriveName, buff, -lenMaxFormatName, pDrive->GetFileSystemName(),
				pDrive->GetBytesTotal(), pDrive->GetRemarks(remarks, sizeof(remarks)));
	}
	return 0;
}

ShellCmd(mkdir, "creates a directory")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	if (!FS::CreateDir(dirName)) {
		tout.Printf("failed to create %s\n", dirName);
		return 1;
	}
	return 0;
}

ShellCmd(rmdir, "removes a directory")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	if (!FS::RemoveDir(dirName)) {
		tout.Printf("failed to remove %s\n", dirName);
		return 1;
	}
	return 0;
}

ShellCmd(cd, "changes the current directory")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	if (!FS::ChangeCurDir(dirName)) {
		tout.Printf("failed to change directory to %s\n", dirName);
		return 1;
	}
	return 0;
}

ShellCmd(pwd, "prints the current directory")
{
	FS::Drive* pDrive = FS::GetDriveCur();
	if (!pDrive) {
		tout.Printf("no current drive\n");
		return 1;
	}
	tout.Printf("%s:%s\n", pDrive->GetDriveName(), pDrive->GetDirNameCur());
	return 0;
}

ShellCmd(mv, "moves a file")
{
	if (argc < 3) {
		tout.Printf("Usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	if (!FS::RenameFile(fileNameSrc, fileNameDst)) {
		tout.Printf("failed to rename %s to %s\n", fileNameSrc, fileNameDst);
		return 1;
	}
	return 0;
}

ShellCmd(touch, "creates an empty file")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	RefPtr<FS::File> pFile(FS::OpenFile(fileName, "w"));
	if (!pFile) {
		tout.Printf("failed to create %s\n", fileName);
		return 1;
	}
	pFile->Close();
	return 0;
}

ShellCmd(rm, "removes a file")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	if (!FS::RemoveFile(fileName)) {
		tout.Printf("failed to remove %s\n", fileName);
		return 1;
	}
	return 0;
}

ShellCmd(format, "formats the filesystem")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <drivename>\n", argv[0]);
		return 1;
	}
	const char* driveName = argv[1];
	FS::Format(tout, driveName);
	return 0;
}

ShellCmd(mount, "mounts a specified drive")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <drivename>\n", argv[0]);
		return 1;
	}
	const char* driveName = argv[1];
	FS::Mount(tout, driveName);
	return 0;
}

ShellCmd(unmount, "unmounts a specified drive")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <drivename>\n", argv[0]);
		return 1;
	}
	const char* driveName = argv[1];
	FS::Unmount(tout, driveName);
	return 0;
}

}
