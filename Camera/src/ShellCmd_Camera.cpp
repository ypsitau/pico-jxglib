#include "jxglib/Shell.h"
#include "jxglib/Camera.h"
#include "jxglib/Display.h"
#include "jxglib/ImageFile.h"

namespace jxglib::Camera::ShellCmd_Camera {

bool enablePreview_ = false;
int iCamera_ = 0;
int iDisplayPreview_ = 0;
Size sizePreviewPrev_;

void ListCameras(Printable& tout);

TickableEntry(CameraPreviewTickable, 33, Tickable::Priority::AboveNormal)
{
	if (!enablePreview_) return;
	Camera::Base& camera = Camera::GetInstance(0);
	if (!camera.IsValid()) return;
	Display::Base& display = Display::GetInstance(iDisplayPreview_);
	if (!display.IsValid()) return;
	const Image& image = camera.Capture();
	if (sizePreviewPrev_ != image.GetSize()) {
		sizePreviewPrev_ = image.GetSize();
		display.Clear();
	}
	display.DrawImage(
		(display.GetWidth() - image.GetWidth()) / 2,
		(display.GetHeight() - image.GetHeight()) / 2, image);
}

ShellCmd(camera, "controls cameras")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",	'h',	"prints this help"),
		Arg::OptString("index",	'i',	"specifies camera index (default: 0)"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	const char* value = nullptr;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... COMMAND...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Commands:\n");
		terr.Printf(" preview-start[:DISPLAY] starts preview on specified display\n");
		terr.Printf(" preview-stop            stops preview\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	if (arg.GetString("index", &value)) {
		char* endptr;
		int num = ::strtol(value, &endptr, 10);
		if (*endptr != '\0' || num < 0) {
			terr.Printf("invalid display index: %s\n", argv[1]);
			return Result::Error;
		}
		iCamera_ = num;
	}
	if (argc < 2) {
		ListCameras(tout);
		return Result::Success;
	}
	Camera::Base& camera = Camera::GetInstance(iCamera_);
	if (!camera.IsValid()) {
		terr.Printf("camera #%d is not available\n", iCamera_);
		return Result::Error;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return Result::Error;
	}
	while (const Arg::Subcmd* pSubcmd = each.NextSubcmd()) {
		const char* subcmd = pSubcmd->GetProc();
		if (Arg::GetAssigned(subcmd, "preview-start", &value)) {
			if (value) {
				char* endptr;
				int num = ::strtol(value, &endptr, 10);
				if (*endptr != '\0' || num < 0) {
					terr.Printf("invalid display index: %s\n", value);
					return Result::Error;
				}
				if (!Display::GetInstance(num).IsValid()) {
					terr.Printf("display #%d is not available\n", num);
					return Result::Error;
				}
				iDisplayPreview_ = num;
			}
			enablePreview_ = true;
		} else if (Arg::GetAssigned(subcmd, "preview-stop", &value)) {
			enablePreview_ = false;
			camera.FreeResource();
		} else if (Arg::GetAssigned(subcmd, "capture", &value)) {
			if (!value) {
				terr.Printf("output file not specified.\n");
				return Result::Error;
			}
			const Image& image = camera.Capture();
			std::unique_ptr<FS::File> pFile(FS::OpenFile(value, "w"));
			if (!pFile) {
				terr.Printf("failed to open file: %s\n", value);
				if (!enablePreview_) camera.FreeResource();
				return Result::Error;
			}
			if (!ImageFile::Write(const_cast<Image&>(image), *pFile, ImageFile::Format::BMP)) {
				terr.Printf("failed to write image to file: %s\n", value);
				if (!enablePreview_) camera.FreeResource();
				return Result::Error;
			}
			if (!enablePreview_) camera.FreeResource();
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

void ListCameras(Printable& tout)
{
	Base* pCamera = Base::GetHead();
	if (!pCamera) {
		tout.Printf("no cameras\n");
		return;
	}
	for (int iCamera = 0; pCamera; iCamera++, pCamera = pCamera->GetNext()) {
		char remarks[128];
		pCamera->GetRemarks(remarks, sizeof(remarks));
		bool remarksFlag = (remarks[0] != '\0');
		tout.Printf("camera#%d: %s%s%s\n", iCamera, pCamera->GetName(), remarksFlag? " " : "", remarks);
	}
}

}
