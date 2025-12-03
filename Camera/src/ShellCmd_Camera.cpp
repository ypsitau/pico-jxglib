#include "jxglib/Shell.h"
#include "jxglib/Camera.h"
#include "jxglib/ImageFile.h"

namespace jxglib::Camera::ShellCmd_Camera {

void PrintCameras(Printable& terr);

ShellCmd_Named(ls_display, "ls-camera", "lists all cameras")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]...\n", GetName());
		arg.PrintHelp(terr);
		return Result::Success;
	}
	PrintCameras(terr);
	return Result::Success;
}

void PrintCameras(Printable& terr)
{
	Base* pCamera = Base::GetHead();
	if (!pCamera) {
		terr.Printf("no cameras\n");
		return;
	}
	for (int iCamera = 0; pCamera; iCamera++, pCamera = pCamera->GetNext()) {
		char remarks[128];
		pCamera->GetRemarks(remarks, sizeof(remarks));
		bool remarksFlag = (remarks[0] != '\0');
		terr.Printf("camera %d: %s%s%s\n", iCamera, pCamera->GetName(), remarksFlag? " " : "", remarks);
	}
}

}
