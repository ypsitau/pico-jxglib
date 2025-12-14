#include "jxglib/Shell.h"
#include "jxglib/Camera.h"
#include "jxglib/Display.h"
#include "jxglib/VideoTransmitter.h"
#include "jxglib/ImageFile.h"

namespace jxglib::Camera::ShellCmd_Camera {

enum class TickableMode {
	None,
	Display,
	VideoTransmitter,
};

TickableMode tickableMode_ = TickableMode::None;
int iCamera_ = 0;
int iDisplay_ = 0;
int iVideoTransmitter_ = 0;
Size sizeDisplayPrev_;

void ListCameras(Printable& tout);

TickableEntry(CameraDisplayTickable, 0, Tickable::Priority::AboveNormal)
{
	switch (tickableMode_) {
	case TickableMode::Display: {
		Camera::Base& camera = Camera::GetInstance(iCamera_);
		if (!camera.IsValid()) return;
		Display::Base& display = Display::GetInstance(iDisplay_);
		if (!display.IsValid()) return;
		const Image& image = camera.Capture();
		if (sizeDisplayPrev_ != image.GetSize()) {
			sizeDisplayPrev_ = image.GetSize();
			display.Clear();
		}
		display.DrawImage(
			(display.GetWidth() - image.GetWidth()) / 2,
			(display.GetHeight() - image.GetHeight()) / 2, image);
		break;
	}
	case TickableMode::VideoTransmitter: {
		Camera::Base& camera = Camera::GetInstance(iCamera_);
		if (!camera.IsValid()) return;
		VideoTransmitter& videoTransmitter = VideoTransmitter::GetInstance(iVideoTransmitter_);
		if (!videoTransmitter.IsValid()) return;
		const Image& image = camera.Capture();
		videoTransmitter.Transmit(image.GetPointer());
		break;
	}		
	default:
		break;
	}
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
		terr.Printf(" fps                     prints frames per second\n");
		terr.Printf(" display-start[:DISPLAY] starts display on specified display\n");
		terr.Printf(" display-stop            stops display\n");
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
		if (::strcasecmp(subcmd, "fps") == 0) {
			tout.Printf("%d fps\n", camera.GetFPS());
		} else if (Arg::GetAssigned(subcmd, "display-start", &value)) {
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
				iDisplay_ = num;
			}
			tickableMode_ = TickableMode::Display;
		} else if (Arg::GetAssigned(subcmd, "display-stop", &value)) {
			if (tickableMode_ == TickableMode::Display) {
				tickableMode_ = TickableMode::None;
				camera.FreeResource();
			}
		} else if (Arg::GetAssigned(subcmd, "capture", &value)) {
			if (!value) {
				terr.Printf("output file not specified.\n");
				return Result::Error;
			}
			const Image& image = camera.Capture();
			std::unique_ptr<FS::File> pFile(FS::OpenFile(value, "w"));
			if (!pFile) {
				terr.Printf("failed to open file: %s\n", value);
				if (tickableMode_ == TickableMode::None) camera.FreeResource();
				return Result::Error;
			}
			if (!ImageFile::Write(const_cast<Image&>(image), *pFile, ImageFile::Format::BMP)) {
				terr.Printf("failed to write image to file: %s\n", value);
				if (tickableMode_ == TickableMode::None) camera.FreeResource();
				return Result::Error;
			}
			if (tickableMode_ == TickableMode::None) camera.FreeResource();
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
