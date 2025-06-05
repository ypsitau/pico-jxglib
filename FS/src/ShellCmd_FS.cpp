#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/FS.h"

namespace jxglib::ShellCmd_FS {

ShellCmd(cat, "prints the contents of files")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options] [<filename>..]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("\n"
			"Reads the contents of files and prints them to standard output.\n"
			"When no filenames are given, the command reads from standard input.\n");
		return 1;
	}
	if (argc < 2) {
		int len;
		char buff[512];
		while ((len = tin.Read(buff, sizeof(buff) - 1)) >= 0) {
			buff[len] = '\0'; // null-terminate the string
			tout.Print(buff);
		}
	} else {
		for (int iArg = 1; iArg < argc; iArg++) {
			const char* pathName = argv[iArg];
			if (FS::DoesContainWildcard(pathName)) {
				std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pathName));
				if (pGlob) {
					for (;;) {
						const char* pathNameGlob;
						std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->Read(&pathNameGlob));
						if (!pFileInfo) break;
						if (pFileInfo->IsFile() && !FS::PrintFile(terr, tout, pathNameGlob)) return 1;
					}
				}
			} else {
				if (!FS::PrintFile(terr, tout, pathName)) break;
			}
		}
	}
	return 0;
}

ShellCmd(cd, "changes the current directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <directory>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = argv[1];
	return FS::ChangeCurDir(terr, dirName)? 0 : 1;
}

ShellCmd_Named(cd_dot_dot, "cd..", "changes the current directory to the parent directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = "..";
	return FS::ChangeCurDir(terr, dirName)? 0 : 1;
}

ShellCmd_Named(cd_slash, "cd/", "changes the current directory to the root directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = "/";
	return FS::ChangeCurDir(terr, dirName)? 0 : 1;
}

ShellCmd(copy, "copies files")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 3 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <src..> <dst>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* pathNameDst = argv[argc - 1];
	for (int iArg = 1; iArg < argc - 1; iArg++) {
		const char* pathNameSrc = argv[iArg];
		if (FS::DoesContainWildcard(pathNameSrc)) {
			std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pathNameSrc));
			if (pGlob) {
				for (;;) {
					const char* pathNameGlob;
					std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->Read(&pathNameGlob));
					if (!pFileInfo) break;
					if (pFileInfo->IsFile() && !FS::CopyFile(terr, pathNameGlob, pathNameDst)) return 1;
				}
			}
		} else {
			if (!FS::CopyFile(terr, pathNameSrc, pathNameDst)) return 1;
		}
	}
	return 0;
}

ShellCmdAlias(cp, copy)

ShellCmd(format, "formats the filesystem")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [drivename]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	for (int iArg = 1; iArg < argc; iArg++) {
		const char* driveName = argv[iArg];
		if (!FS::IsLegalDriveName(driveName)) {
			terr.Printf("invalid drive name: %s\n", driveName);
			return 1;
		}
		if (!FS::Format(terr, driveName)) return 1;
	}
	return 0;
}

ShellCmd(glob, "prints files matching a glob pattern")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <pattern>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* pattern = argv[1];
	std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pattern));
	if (pGlob) {
		for (;;) {
			const char* pathNameGlob;
			std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->Read(&pathNameGlob));
			if (!pFileInfo) break;
			tout.Printf("%s\n", pathNameGlob);
		}
	}
	return 0;
}

ShellCmd(ls, "lists files in the specified directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
		Arg::OptBool("all",			"a",	"lists all files, including hidden ones"),
		Arg::OptBool("mixed",		"m",	"lists files and directories in mixed order"),		
		Arg::OptBool("name",		"n",	"sorts by name"),
		Arg::OptBool("size",		"s",	"sorts by size"),
		Arg::OptBool("datetime",	"d",	"sorts by date/time"),
		Arg::OptBool("reverse",		"r",	"reverses the order of listing"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options] [<pathname>]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	bool mixedFlag = arg.GetBool("mixed");
	bool reverseFlag = arg.GetBool("reverse");
	bool attrExclude = arg.GetBool("all")? 0 : (FS::FileInfo::Attr::Hidden | FS::FileInfo::Attr::System);
	const char* dirName = (argc < 2)? "" : argv[1];
	const FS::FileInfo::Cmp* pCmp1 = mixedFlag? &FS::FileInfo::Cmp::Zero : &FS::FileInfo::Cmp_Type::Ascent;
	const FS::FileInfo::Cmp* pCmp2 = reverseFlag? &FS::FileInfo::Cmp_Name::Descent : &FS::FileInfo::Cmp_Name::Ascent;
	const FS::FileInfo::Cmp* pCmp3 = &FS::FileInfo::Cmp::Zero;
	if (arg.GetBool("name")) {
		// nothing to do, pCmp1 and pCmp2 are already set to name comparison
	} else if (arg.GetBool("size")) {
		pCmp2 = reverseFlag? &FS::FileInfo::Cmp_Size::Descent : &FS::FileInfo::Cmp_Size::Ascent;
		pCmp3 = &FS::FileInfo::Cmp_Name::Ascent;
	} else if (arg.GetBool("datetime")) {
		pCmp2 = reverseFlag? &FS::FileInfo::Cmp_DateTime::Descent : &FS::FileInfo::Cmp_DateTime::Ascent;
		pCmp3 = &FS::FileInfo::Cmp_Name::Ascent;
	}
	FS::FileInfo::CmpDefault.Set(pCmp1, pCmp2, pCmp3);
	return FS::ListFiles(terr, tout, dirName, FS::FileInfo::CmpDefault, attrExclude)? 0 : 1;
}

ShellCmdAlias(ll, ls)
ShellCmdAlias(dir, ls)

ShellCmd_Named(ls_drive, "ls-drive", "lists availabld drives")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
		Arg::OptBool("remarks",		"r",	"prints remarks for each drive"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options] [<drivename>]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* driveName = (argc < 2)? nullptr : argv[1];
	bool remarksFlag = arg.GetBool("remarks");
	return FS::ListDrives(tout, driveName, remarksFlag)? 0 : 1;
}

ShellCmdAlias_Named(dir_drive, "dir-drive", ls_drive)

ShellCmd(mkdir, "creates a directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <directory>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = argv[1];
	return FS::CreateDir(terr, dirName)? 0 : 1;
}

ShellCmd(mount, "mounts a specified drive")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <drivename>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* driveName = argv[1];
	return FS::Mount(terr, driveName)? 0 : 1;
}

ShellCmd(move, "moves a file")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 3 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <src> <dst>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	return FS::Move(terr, fileNameSrc, fileNameDst)? 0 : 1;
}

ShellCmdAlias(mv, move)
ShellCmdAlias(ren, move)

ShellCmd(pwd, "prints the current directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("usage: %s [options]\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	FS::Drive* pDrive = FS::GetDriveCur();
	if (!pDrive) {
		tout.Printf("no current drive\n");
		return 1;
	}
	tout.Printf("%s:%s\n", pDrive->GetDriveName(), pDrive->GetDirNameCur());
	return 0;
}

ShellCmd(rm, "removes files")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <filename..>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	for (int iArg = 1; iArg < argc; iArg++) {
		const char* pathName = argv[iArg];
		if (FS::DoesContainWildcard(pathName)) {
			std::unique_ptr<FS::Glob> pGlob(FS::OpenGlob(pathName));
			if (pGlob) {
				for (;;) {
					const char* pathNameGlob;
					std::unique_ptr<FS::FileInfo> pFileInfo(pGlob->Read(&pathNameGlob));
					if (!pFileInfo) break;
					if (pFileInfo->IsFile() && !FS::RemoveFile(terr, pathNameGlob)) return 1;
				}
			}
		} else {
			if (!FS::RemoveFile(terr, pathName)) return 1;
		}
	}
	return 0;
}

ShellCmdAlias(del, rm)

ShellCmd(rmdir, "removes a directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <directory>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = argv[1];
	return FS::RemoveDir(terr, dirName)? 0 : 1;
}

ShellCmd(touch, "creates an empty file")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <filename>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* fileName = argv[1];
	std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "w"));
	if (!pFile) {
		tout.Printf("failed to create %s\n", fileName);
		return 1;
	}
	pFile->Close();
	return 0;
}

ShellCmd(umount, "unmounts a specified drive")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		"h",	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("usage: %s [options] <drivename>\noptions:\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* driveName = argv[1];
	return FS::Unmount(tout, driveName)? 0 : 1;
}

}
