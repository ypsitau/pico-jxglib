#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/FS.h"

namespace jxglib::ShellCmd_FS {

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


}

