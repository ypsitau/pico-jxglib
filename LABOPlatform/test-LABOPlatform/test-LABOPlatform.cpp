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
	USBDevice::VideoTransmitter& videoTransmitter = LABOPlatform::Instance.GetVideoTransmitter();
	const Size& size = videoTransmitter.GetSize();
	uint8_t* frameBuffer = reinterpret_cast<uint8_t*>(::malloc(size.width * size.height * 16 / 8));
	int startPos = 0;
	for (;;) {
#if 1
		if (videoTransmitter.CanTransmitFrame()) {
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
			uint8_t* end = &frameBuffer[size.width * 2];
			unsigned idx = (size.width / 2 - 1) - (startPos % (size.width / 2));
			uint8_t* p = &frameBuffer[idx * 4];
			for (unsigned i = 0; i < 8; ++i) {
				for (int j = 0; j < size.width / (2 * 8); ++j) {
					memcpy(p, &bar_color[i], 4);
					p += 4;
					if (end <= p) {
						p = frameBuffer;
					}
				}
			}
			p = &frameBuffer[size.width * 2];
			for (unsigned i = 1; i < size.height; ++i) {
				memcpy(p, frameBuffer, size.width * 2);
				p += size.width * 2;
			}
			startPos++;
			videoTransmitter.TransmitFrame(frameBuffer);
		}
#endif
		Tickable::Tick();
	}
}
