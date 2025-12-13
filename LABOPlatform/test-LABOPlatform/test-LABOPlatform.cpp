#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	//LABOPlatform::Instance.Initialize();
	::adc_init();
	::adc_set_temp_sensor_enabled(true);
	//USBDevice::VideoSimple& video = LABOPlatform::Instance.GetVideo();
	//int width = video.GetWidth(), height = video.GetHeight();
	//uint8_t* frameBuffer = reinterpret_cast<uint8_t*>(::malloc(width * height * 16 / 8));
	//int startPos = 0;
	for (;;) {
#if 0
		if (video.CanTransferFrame()) {
			static const uint8_t bar_color[8][4] = {
				//  Y,   U,   Y,   V
				{ 235, 128, 235, 128 }, // 100% White
				{ 219,  16, 219, 138 }, // Yellow
				{ 188, 154, 188,  16 }, // Cyan
				{ 173,  42, 173,  26 }, // Green
				{  78, 214,  78, 230 }, // Magenta
				{  63, 102,  63, 240 }, // Red
				{  32, 240,  32, 118 }, // Blue
				{  16, 128,  16, 128 }, // Black
			};
			uint8_t* end = &frameBuffer[width * 2];
			unsigned idx = (width / 2 - 1) - (startPos % (width / 2));
			uint8_t* p = &frameBuffer[idx * 4];
			for (unsigned i = 0; i < 8; ++i) {
				for (int j = 0; j < width / (2 * 8); ++j) {
					memcpy(p, &bar_color[i], 4);
					p += 4;
					if (end <= p) {
						p = frameBuffer;
					}
				}
			}
			p = &frameBuffer[width * 2];
			for (unsigned i = 1; i < height; ++i) {
				memcpy(p, frameBuffer, width * 2);
				p += width * 2;
			}
			startPos++;
			video.TransferFrame(frameBuffer);
		}
#endif
		Tickable::Tick();
	}
}
