#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Font/shinonome14.h"
#include "jxglib/ML/DigitRecognizer.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

const int wdImage = 28, htImage = 28;

ML::DigitRecognizer digitRecognizer;

uint8_t imageData[wdImage * htImage];
uint8_t imageDataBlurred[wdImage * htImage];

void PrintImageData(const uint8_t* imageData);

int main()
{
	::stdio_init_all();
	LABOPlatform::Instance.Initialize();
	//LABOPlatform::Instance.AttachStdio().Initialize();
	digitRecognizer.Initialize();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125'000'000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	Display::ILI9341 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	Display::ILI9341::TouchScreen touchScreen(spi0, {CS: GPIO8, IRQ: GPIO9});
	Display::Terminal terminal;
	display.Initialize(Display::Dir::Rotate0);
	display.SetFont(Font::shinonome14);
	Rect rcCanvas(0, 0, 240, 240);
	Color bgCanvas(0, 192, 0);
	terminal.Initialize()
		.AttachDisplay(display, {0, 240, 240, 80})
		.SetFont(Font::shinonome14)
		.SetColorBg(Color::black)
		.SetSpacingRatio(1., 1.2)
		.ClearScreen();
	touchScreen.Initialize(display);
	//touchScreen.Calibrate(display, true);
	touchScreen.PrintCalibration();
	terminal.Println("Touch the screen to draw.");
	Size szPixel(10, 10);
	const uint32_t msecFlush = 1000;
	uint32_t msecLastTouch = Tickable::GetCurrentTime();
	display.DrawRectFill(rcCanvas, bgCanvas);
	bool isTouched = false;
	::memset(imageData, 0, sizeof(imageData));
	for (;;) {
		Point pt;
		if (touchScreen.ReadPoint(&pt) && rcCanvas.Contains(pt)) {
			display.DrawRectFill(pt.x - szPixel.width / 2, pt.y - szPixel.height / 2, szPixel.width, szPixel.height);
			msecLastTouch = Tickable::GetCurrentTime();
			int x = (pt.x - (rcCanvas.x)) * wdImage / rcCanvas.width;
			int y = (pt.y - (rcCanvas.y)) * htImage / rcCanvas.height;
			imageData[y * wdImage + x] = 255;
			if (y + 1 < htImage) imageData[(y + 1) * wdImage + x] = 255;
			if (y - 1 >= 0) imageData[(y - 1) * wdImage + x] = 255;
			if (x + 1 < wdImage) imageData[y * wdImage + (x + 1)] = 255;
			if (x - 1 >= 0) imageData[y * wdImage + (x - 1)] = 255;
			isTouched = true;
		} else if (isTouched && Tickable::GetCurrentTime() - msecLastTouch > msecFlush) {
			PrintImageData(imageData);
			float confidence = 0.0f;
			int result = digitRecognizer.Recognize(imageData, &confidence);
			if (result >= 0) terminal.Printf("recognized: %d (%.2f)\n", result, confidence);
			isTouched = false;
			display.DrawRectFill(rcCanvas, bgCanvas);
			::memset(imageData, 0, sizeof(imageData));
		}
		Tickable::Tick();
	}
}

void PrintImageData(const uint8_t* imageData)
{
	static const char patTbl[] = { ' ', '.', ':', '+', '#' };
	int iCol = 0;
	for (int i = 0; i < wdImage * htImage; i++) {
		::printf("%c", patTbl[imageData[i] * 5 / 256]);
		iCol++;
		if (iCol >= wdImage) {
			::printf("\n");
			iCol = 0;
		}
	}
	::printf("\n");
}
