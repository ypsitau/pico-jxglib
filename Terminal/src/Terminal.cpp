//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
void Terminal::AttachOutput(Drawable& drawable)
{
	if (drawable.CanScrollVert()) {
		pDrawable_ = &drawable;
	} else {
		canvas_.AttachOutput(drawable);
		pDrawable_ = &canvas_;
	}
}

Terminal& Terminal::puts(const char* str)
{
	Drawable& drawable = *pDrawable_;
	uint32_t code;
	UTF8Decoder decoder;
	const char* p = str;
	const FontSet& fontSet = drawable.GetFont();
	int yAdvance = drawable.CalcAdvanceY();
	for ( ; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = drawable.CalcAdvanceX(fontEntry);
		if (code == '\n' || pt_.x + xAdvance > drawable.GetWidth()) {
			pt_.x = 0;
			pt_.y += yAdvance;
		}
		if (pt_.y + yAdvance > drawable.GetHeight()) {
			pt_.y -= yAdvance;
			drawable.ScrollVert(DirVert::Up, yAdvance);
			drawable.DrawRectFill(0, pt_.y, drawable.GetWidth(), drawable.GetHeight() - pt_.y, drawable.GetColorBg());
		}
		if (code != '\n') {
			drawable.DrawChar(pt_, fontEntry);
			pt_.x += xAdvance;
		}
	}
	drawable.Refresh();
	return *this;
}

Terminal& Terminal::vprintf(const char* format, va_list args)
{
	char buff[256];
	::vsnprintf(buff, sizeof(buff), format, args);
	return puts(buff);
}

Terminal& Terminal::printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	return *this;
}

}
