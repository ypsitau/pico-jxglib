#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/ImageFile.h"
#include "jxglib/Display.h"

namespace jxglib::ShellCmd_ImageFile {

int iDisplay = 0;
Point posDisp {0, 0};
Point posImage {0, 0};
Size sizeImage {0, 0};
Image image(Image::Format::RGB);

ShellCmd_Named(show_image, "show-image", "show image file")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
		Arg::OptString("display",	'D',	"specifies the number of display to use"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... [CMD]...\n", GetName());
		arg.PrintHelp(terr);
		tout.Printf("Sub Commands:\n");
		tout.Printf(" sleep:MSEC           sleep for specified milliseconds\n");
		tout.Printf(" repeat[:N] {CMD...}  repeat the commands N times (default: infinite)\n");
		tout.Printf(" image-file:FILE      specify image file to load\n");
		tout.Printf(" disp-pos:X,Y         specify display position (default: 0,0)\n");
		tout.Printf(" image-pos:X,Y        specify image position (default: 0,0)\n");
		tout.Printf(" image-slide:X,Y      slide image position by specified amount\n");
		tout.Printf(" image-size:W,H       specify image size to display (default: full size)\n");
		tout.Printf(" show                 show the image on the display\n");

		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	const char* value;
	if (arg.GetString("display", &value)) {
		char* endptr;
		int num = ::strtol(value, &endptr, 10);
		if (*endptr != '\0' || num < 0 || !Display::GetInstance(num).IsValid()) {
			terr.Printf("Invalid display number: %s\n", value);
			return Result::Error;
		}
		iDisplay = num;
	}
	Shell::Arg::EachSubcmd each(argv[1], argv[argc]);
	if (!each.Initialize()) {
		terr.Printf("%s\n", each.GetErrorMsg());
		return false;
	}
	while (const char* subcmd = each.Next()) {
		const char* value = nullptr;
		if (Shell::Arg::GetAssigned(subcmd, "image-file", &value)) {
			if (!value) {
				terr.Printf("file name is not specified\n");
				return Result::Error;
			}
			char fileName[FS::MaxPath];
			::snprintf(fileName, sizeof(fileName), "%s", value);
			Tokenizer::RemoveSurroundingQuotes(fileName);
			std::unique_ptr<FS::File> pFile(FS::OpenFile(fileName, "r"));
			if (!pFile) {
				terr.Printf("Failed to open file: %s\n", fileName);
				return Result::Error;
			}
			if (!ImageFile::Read(image, *pFile)) {
				terr.Printf("Failed to read image file: %s\n", fileName);
				return Result::Error;
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "disp-pos", &value)) {
			if (!value) {
				terr.Printf("disp-pos value is not specified\n");
				return Result::Error;
			} else if (!posDisp.Parse(value)) {
				terr.Printf("invalid disp-pos: %s\n", value? value : "(null)");
				return Result::Error;
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "image-pos", &value)) {
			if (!value) {
				terr.Printf("image-pos value is not specified\n");
				return Result::Error;
			} else if (!posImage.Parse(value)) {
				terr.Printf("invalid image-pos: %s\n", value);
				return Result::Error;
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "image-slide", &value)) {
			Point slideImage;
			if (!value) {
				terr.Printf("image-slide value is not specified\n");
				return Result::Error;
			} else if (!slideImage.Parse(value)) {
				terr.Printf("invalid image-slide: %s\n", value);
				return Result::Error;
			}
			posImage += slideImage;
		} else if (Shell::Arg::GetAssigned(subcmd, "image-size", &value)) {
			if (!value) {
				terr.Printf("image-size value is not specified\n");
				return Result::Error;
			} else if (::strcasecmp(value, "full") == 0) {
				sizeImage = Size(0, 0);
			} else if (!sizeImage.Parse(value) || sizeImage.width <= 0 || sizeImage.height <= 0) {
				terr.Printf("invalid image-size: %s\n", value);
				return Result::Error;
			}
		} else if (::strcasecmp(subcmd, "show") == 0) {
			Size sizeImageAdj = sizeImage.IsZero()? Size(image.GetWidth() - posImage.x, image.GetHeight() - posImage.y) : sizeImage;
			if (sizeImageAdj.width > 0 && sizeImageAdj.height > 0) {
				Display::GetInstance(iDisplay).DrawImage(posDisp.x, posDisp.y, image, Rect(posImage, sizeImageAdj)).Refresh();
			}
		} else if (Shell::Arg::GetAssigned(subcmd, "sleep", &value)) {
			if (!value) {
				terr.Printf("specify a sleep duration in milliseconds\n");
				return false;
			}
			int msec = ::strtol(value, nullptr, 0);
			if (msec <= 0) {
				terr.Printf("Invalid sleep duration: %s\n", value);
				return false;
			}
			Tickable::Sleep(msec);
		} else {
			terr.Printf("unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}
	return Result::Success;
}

}
