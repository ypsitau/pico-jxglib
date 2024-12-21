#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Drawable_TestCase.h"
#include "jxglib/ILI9341.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::spi_init(spi0, 1 * 1000 * 1000);
	::spi_init(spi1, 125 * 1000 * 1000);
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ILI9341 display(spi1, 240, 320, GPIO12, GPIO11, GPIO13, GPIO::None);
	ILI9341::TouchScreen touchScreen(spi0, GPIO6,GPIO7);
	display.Initialize(Display::Dir::Rotate0);
	touchScreen.Initialize();
	//Drawable_TestCase::DrawString(display);
	int msecDelay = 100;
	int nSamples = 10;
	Point ptTbl[2] = {{30, 30}, {200, 300}};
	Point adcTbl[2];
	for (int i = 0; i < count_of(ptTbl); i++) {
		display.Clear();
		display.DrawCross(ptTbl[i].x, ptTbl[i].y, 30, 30, 3, 3);
		int xSum = 0, ySum = 0;
		int x, y;
		while (touchScreen.IsPressed()) ::sleep_ms(msecDelay);
		for (int iSample = 0; iSample < nSamples; iSample++) {
			while (!touchScreen.ReadPosition(&x, &y)) ::sleep_ms(msecDelay);
			xSum += x, ySum += y;
			::sleep_ms(msecDelay);
		}
		adcTbl[i].x = xSum / nSamples, adcTbl[i].y = ySum / nSamples;
	}
	display.Clear();
	for (int i = 0; i < count_of(ptTbl); i++) {
		::printf("%d, %d -> %d, %d\n", adcTbl[i].x, adcTbl[i].y, ptTbl[i].x, ptTbl[i].y);
	}
}
