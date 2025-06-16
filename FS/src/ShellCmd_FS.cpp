#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/FS.h"

namespace jxglib::ShellCmd_FS {

ShellCmd(cat, "prints the contents of files")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [FILE]...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("\n"
			"Reads the contents of files and prints them to standard output.\n"
			"When no filenames are given, the command reads from standard input.\n");
		return 1;
	}
	if (argc < 2) {
		int len;
		char buff[128];
		Shell::BeginInteractive();
		while ((len = tin.Read(buff, sizeof(buff) - 1)) >= 0) {
			buff[len] = '\0'; // null-terminate the string
			tout.Print(buff);
		}
		Shell::EndInteractive();
	} else {
		for (Arg::Glob argIter(argv[1], argv[argc]); const char* pathName = argIter.Next(); ) {
			if (argIter.GetFileInfo().IsFile() && !FS::PrintFile(terr, tout, pathName)) return 1;
		}
	}
	return 0;
}

ShellCmd(cd, "changes the current directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DIRECTORY\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = argv[1];
	return FS::ChangeCurDir(terr, dirName)? 0 : 1;
}

ShellCmd_Named(cd_dot_dot, "cd..", "changes the current directory to the parent directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = "..";
	return FS::ChangeCurDir(terr, dirName)? 0 : 1;
}

ShellCmd_Named(cd_slash, "cd/", "changes the current directory to the root directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = "/";
	return FS::ChangeCurDir(terr, dirName)? 0 : 1;
}

ShellCmd(copy, "copies files")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("recursive",	'r',	"copies directories recursively"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 3 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... SOURCE... DEST\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	bool recursiveFlag = arg.GetBool("recursive");
	const char* pathNameDst = argv[argc - 1];
	int rtn = 0;
	for (Arg::Glob argIter(argv[1], argv[argc - 1]); const char* pathNameSrc = argIter.Next(); ) {
		if (!FS::Copy(terr, pathNameSrc, pathNameDst, recursiveFlag)) rtn = 1;
	}
	return rtn;
}

ShellCmdAlias(cp, copy)

ShellCmd(format, "formats drives")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s DRIVE...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	for (Arg::Each argIter(argv[1], argv[argc]); const char* driveName = argIter.Next(); ) {
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
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... PATTERN\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	for (Arg::Glob argIter(argv[1], argv[argc]); const char* pathName = argIter.Next(); ) {
		tout.Printf("%s\n", pathName);
	}
	return 0;
}

ShellCmd(ls, "lists files in the specified directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("all",			'a',	"lists all files, including hidden ones"),
		Arg::OptBool("mixed",		'm',	"lists files and directories in mixed order"),		
		Arg::OptString("sort",		0x0,	"sorts by WORD instead of name. WORD: name, size, time", "WORD"),
		Arg::OptBool("reverse",		'r',	"reverses the order of listing"),
		Arg::OptBool("elimslash",	'e',	"eliminates trailing slashes from directory names"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [DIRECTORY]...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	bool mixedFlag = arg.GetBool("mixed");
	bool reverseFlag = arg.GetBool("reverse");
	bool attrExclude = arg.GetBool("all")? 0 : (FS::FileInfo::Attr::Hidden | FS::FileInfo::Attr::System);
	bool slashForDirFlag = !arg.GetBool("elimslash");
	const FS::FileInfo::Cmp* pCmp1 = mixedFlag? &FS::FileInfo::Cmp::Zero : &FS::FileInfo::Cmp_Type::Ascent;
	const FS::FileInfo::Cmp* pCmp2 = reverseFlag? &FS::FileInfo::Cmp_Name::Descent : &FS::FileInfo::Cmp_Name::Ascent;
	const FS::FileInfo::Cmp* pCmp3 = &FS::FileInfo::Cmp::Zero;
	const char* value;
	if (!arg.GetString("sort", &value) || ::strcasecmp(value, "name") == 0) {
		// nothing to do, pCmp1 and pCmp2 are already set to name comparison
	} else if (::strcasecmp(value, "size") == 0) {
		pCmp2 = reverseFlag? &FS::FileInfo::Cmp_Size::Descent : &FS::FileInfo::Cmp_Size::Ascent;
		pCmp3 = &FS::FileInfo::Cmp_Name::Ascent;
	} else if (::strcasecmp(value, "time") == 0) {
		pCmp2 = reverseFlag? &FS::FileInfo::Cmp_DateTime::Descent : &FS::FileInfo::Cmp_DateTime::Ascent;
		pCmp3 = &FS::FileInfo::Cmp_Name::Ascent;
	} else {
		terr.Printf("Unknown sort option: %s\n", value);
		return 1;
	}
	FS::FileInfo::CmpDefault.Set(pCmp1, pCmp2, pCmp3);
	if (argc < 2) {
		return FS::ListFiles(terr, tout, "", FS::FileInfo::CmpDefault, attrExclude, slashForDirFlag)? 0 : 1;
	}
	for (Arg::Each argIter(argv[1], argv[argc]); const char* dirName = argIter.Next(); ) {
		if (argc > 2) tout.Printf("%s\n", dirName);
		if (!FS::ListFiles(terr, tout, dirName, FS::FileInfo::CmpDefault, attrExclude, slashForDirFlag)) return 1;
	}
	return 0;
}

ShellCmdAlias(ll, ls)
ShellCmdAlias(dir, ls)

ShellCmd_Named(ls_drive, "ls-drive", "lists availabld drives")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
		Arg::OptBool("verbose",	'v',	"prints remarks for each drive"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [DRIVE]\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* driveName = (argc < 2)? nullptr : argv[1];
	bool remarksFlag = arg.GetBool("verbose");
	return FS::ListDrives(tout, driveName, remarksFlag)? 0 : 1;
}

ShellCmdAlias_Named(dir_drive, "dir-drive", ls_drive)

ShellCmd(mkdir, "creates directories")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DIRECTORY...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	for (Arg::Each argIter(argv[1], argv[argc]); const char* dirName = argIter.Next(); ) {
		if (!FS::CreateDir(terr, dirName)) return 1;
	}
	return 0;
}

ShellCmdAlias(md, mkdir)

ShellCmd(mount, "mounts specified drives")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DRIVE\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	for (Arg::Each argIter(argv[1], argv[argc]); const char* driveName = argIter.Next(); ) {
		if (!FS::Mount(terr, driveName)) return 1;
	}
	return 0;
}

ShellCmd(move, "moves a file")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 3 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... SOURCE... DEST\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* pathNameDst = argv[argc - 1];
	for (Arg::Glob argIter(argv[1], argv[argc - 1]); const char* pathNameSrc = argIter.Next(); ) {
		if (!FS::Move(terr, pathNameSrc, pathNameDst)) return 1;
	}
	return 0;
}

ShellCmdAlias(mv, move)
ShellCmdAlias(ren, move)

ShellCmd(pwd, "prints the current directory")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
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
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("recursive",	'r',	"removes directories recursively"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... FILE...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	bool recursiveFlag = arg.GetBool("recursive");
	int rtn = 0;
	for (Arg::Glob argIter(argv[1], argv[argc]); const char* pathName = argIter.Next(); ) {
		if (!FS::Remove(terr, pathName, recursiveFlag)) rtn = 1;
	}
	return rtn;
}

ShellCmdAlias(del, rm)

ShellCmd(rmdir, "removes directories")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DIRECTORY...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	int rtn = 0;
	for (Arg::Glob argIter(argv[1], argv[argc]); const char* pathName = argIter.Next(); ) {
		if (!FS::RemoveDir(terr, pathName)) rtn = 1;
	}
	return rtn;
}

ShellCmd(touch, "updates time stamps or creates empty files")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("datetime",	'd',	"specifies the date and time to set (format: YYYY-MM-DD HH:MM:SS)", "DATETIME"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... FILE...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	int rtn = 0;
	DateTime dt;
	const char* value;
	if (arg.GetString("datetime", &value)) {
		if (DateTime::HasTimeFormat(value)) {
			dt.ParseTime(value); // DateTime::HasTimeFormat() already ensures the validity
		} else if (!dt.Parse(value)) {
			terr.Printf("invalid date/time format. Use YYYY-MM-DD HH:MM:SS\n");
			return 1;
		}
	} else {
		RTC::Get(&dt);
	}
	for (Arg::Glob argIter(argv[1], argv[argc]); const char* pathName = argIter.Next(); ) {
		if (!FS::Touch(terr, pathName, dt)) rtn = 1;
	}
	return rtn;
}

ShellCmd(umount, "unmounts specified drives")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DRIVE...\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	int rtn = 0;
	for (Arg::Each argIter(argv[1], argv[argc]); const char* driveName = argIter.Next(); ) {
		if (!FS::Unmount(tout, driveName)) rtn = 1;
	}
	return rtn;
}

ShellCmd(walk, "walks through directories")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptBool("filefirst",	'f',	"walks files before directories"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [DIRECTORY]\n", GetName());
		arg.PrintHelp(terr);
		return 1;
	}
	const char* dirName = (argc < 2)? "." : argv[1];
	uint8_t attrExclude = 0;
	bool fileFirstFlag = arg.GetBool("filefirst");
	jxglib::FS::Walker walker(fileFirstFlag);;
	if (!walker.Open(dirName, attrExclude)) {
		terr.Printf("cannot open directory: %s\n", dirName);
		return 1;
	}
	std::unique_ptr<jxglib::FS::FileInfo> pFileInfo;
	for (;;) {
		const char* pathName = nullptr;
		pFileInfo.reset(walker.Read(&pathName));
		if (!pFileInfo) break; // no more files
		tout.Printf("%-40s %s\n", pathName, walker.GetPathNameSub());
	}
	return 0;
}

}
