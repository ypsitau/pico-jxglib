#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/FS.h"

namespace jxglib::ShellCmd_FS {

ShellCmd(cat, "prints the contents of a file")
{
	if (argc < 2) {
		out.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	RefPtr<FS::File> pFile(FS::OpenFile(fileName, "r"));
	if (!pFile) {
		err.Printf("failed to open %s\n", fileName);
		return 1;
	}
	int bytes;
	char buff[128];
	while ((bytes = pFile->Read(buff, sizeof(buff) - 1)) > 0) {
		buff[bytes] = '\0';
		out.Print(buff);
	}
	pFile->Close();
	return 0;
}

ShellCmd(cp, "copies a file")
{
	if (argc < 3) {
		out.Printf("Usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	RefPtr<FS::File> pFileSrc(FS::OpenFile(fileNameSrc, "r"));
	if (!pFileSrc) {
		out.Printf("failed to open %s\n", fileNameSrc);
		return 1;
	}
	RefPtr<FS::File> pFileDst(FS::OpenFile(fileNameDst, "w"));
	if (!pFileDst) {
		out.Printf("failed to open %s\n", fileNameDst);
		return 1;
	}
	int bytesRead;
	char buff[128];
	while ((bytesRead = pFileSrc->Read(buff, sizeof(buff))) > 0) {
		if (pFileDst->Write(buff, bytesRead) != bytesRead) {
			out.Printf("failed to write %s\n", fileNameDst);
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
		out.Printf("failed to open %s\n", dirName);
		return 1;
	}
	FS::FileInfo* pFileInfo;
	while (pDir->Read(&pFileInfo)) {
		if (pFileInfo->IsDirectory()) {
			out.Printf("%-20s <DIR>\n", pFileInfo->GetName());
		} else if (pFileInfo->IsFile()) {
			out.Printf("%-20s %d\n", pFileInfo->GetName(), pFileInfo->GetSize());
		}
	}
	pDir->Close();
	return 0;
}

ShellCmd_Alias(ls_drive, "ls-drive", "lists availabld drives")
{
	RefPtr<FS::Dir> pDir(FS::OpenDirDrive());
	FS::FileInfo* pFileInfo;
	while (pDir->Read(&pFileInfo)) {
		out.Printf("%s:\n", pFileInfo->GetName());
	}
	pDir->Close();
	return 0;
}

ShellCmd(mkdir, "creates a directory")
{
	if (argc < 2) {
		out.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	if (!FS::CreateDir(dirName)) {
		out.Printf("failed to create %s\n", dirName);
		return 1;
	}
	return 0;
}

ShellCmd(rmdir, "removes a directory")
{
	if (argc < 2) {
		out.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	if (!FS::RemoveDir(dirName)) {
		out.Printf("failed to remove %s\n", dirName);
		return 1;
	}
	return 0;
}

ShellCmd(cd, "changes the current directory")
{
	if (argc < 2) {
		out.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	if (!FS::ChangeCurDir(dirName)) {
		out.Printf("failed to change directory to %s\n", dirName);
		return 1;
	}
	return 0;
}

ShellCmd(pwd, "prints the current directory")
{
	FS::Manager* pManager = FS::GetManagerCur();
	if (!pManager) {
		out.Printf("no current drive\n");
		return 1;
	}
	out.Printf("%s:%s\n", pManager->GetDriveName(), pManager->GetDirNameCur());
	return 0;
}

ShellCmd(mv, "moves a file")
{
	if (argc < 3) {
		out.Printf("Usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	if (!FS::RenameFile(fileNameSrc, fileNameDst)) {
		out.Printf("failed to rename %s to %s\n", fileNameSrc, fileNameDst);
		return 1;
	}
	return 0;
}

ShellCmd(touch, "creates an empty file")
{
	if (argc < 2) {
		out.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	RefPtr<FS::File> pFile(FS::OpenFile(fileName, "w"));
	if (!pFile) {
		out.Printf("failed to create %s\n", fileName);
		return 1;
	}
	pFile->Close();
	return 0;
}

ShellCmd(rm, "removes a file")
{
	if (argc < 2) {
		out.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	if (!FS::RemoveFile(fileName)) {
		out.Printf("failed to remove %s\n", fileName);
		return 1;
	}
	return 0;
}

ShellCmd(format, "formats the filesystem")
{
	if (argc < 2) {
		out.Printf("Usage: %s <drivename>\n", argv[0]);
		return 1;
	}
	const char* driveName = argv[1];
	FS::Format(driveName, out);
	return 0;
}

}
