#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Image.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Font/shinonome14.h"
#include "jxglib/ML/TfLiteModelRunner.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

#if 0
EmbedTfLiteModel("jxglib/ML/Model/Recognizer-EMNIST-mnist-binary.tflite", modelData, modelDataSize);
static ML::TfLiteModelRunner<16000, 8> modelRunner(modelData);
const char* strPrompt = "Draw one of 0-9";
const char* labelTbl[] = {
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
};
#elif 0
EmbedTfLiteModel("jxglib/ML/Model/Recognizer-EMNIST-letters-binary.tflite", modelData, modelDataSize);
static ML::TfLiteModelRunner<16000, 8> modelRunner(modelData);
const char* strPrompt = "Draw one of A-Z";
const char* labelTbl[] = {
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
	"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
	"U", "V", "W", "X", "Y", "Z",
};
#else
EmbedTfLiteModel("jxglib/ML/Model/Recognizer-EMNIST-bymerge-binary.tflite", modelData, modelDataSize);
static ML::TfLiteModelRunner<16500, 8> modelRunner(modelData);
const char* strPrompt = "Draw one of 0-9, A-Z, and a-z";
const char* labelTbl[] = {
	"0 (number)", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
	"K", "L", "M", "N", "O (alphabet)", "P", "Q", "R", "S", "T",
	"U", "V", "W", "X", "Y", "Z",
	"a", "b", "d", "e", "f", "g", "h", "n", "q", "r", "t",
};
#endif

int main()
{
	::stdio_init_all();
	LABOPlatform::Instance.Initialize();
	//LABOPlatform::Instance.AttachStdio().Initialize();
	do {
		auto& opResolver = modelRunner.PrepareOpResolver();
		opResolver.AddConv2D();
		opResolver.AddMaxPool2D();
		opResolver.AddReshape();
		opResolver.AddFullyConnected();
		opResolver.AddSoftmax();
	} while (0);
	if (modelRunner.Initialize() != kTfLiteOk) return 1;
	modelRunner.PrintInfo();
	Image imageGray;
	do {
		auto input = modelRunner.GetInput(0);
		assert(input.type == kTfLiteInt8 && input.dims->size == 4);
		imageGray.Allocate(Image::Format::Gray, input.dims->data[2], input.dims->data[1]);
	} while (0);
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
	//touchScreen.PrintCalibration();
	terminal.Println(strPrompt);
	Size szPixel(10, 10);
	const uint32_t msecFlush = 1000;
	uint32_t msecLastTouch = Tickable::GetCurrentTime();
	display.DrawRectFill(rcCanvas, bgCanvas);
	bool isTouched = false;
	imageGray.FillZero();
	for (;;) {
		Point pt;
		if (touchScreen.ReadPoint(&pt) && rcCanvas.Contains(pt)) {
			display.DrawRectFill(pt.x - szPixel.width / 2, pt.y - szPixel.height / 2, szPixel.width, szPixel.height);
			msecLastTouch = Tickable::GetCurrentTime();
			int x = (pt.x - (rcCanvas.x)) * imageGray.GetWidth() / rcCanvas.width;
			int y = (pt.y - (rcCanvas.y)) * imageGray.GetHeight() / rcCanvas.height;
			imageGray.Put(x, y, 255);
			if (y + 1 < imageGray.GetHeight()) imageGray.Put(x, y + 1, 255);
			if (y - 1 >= 0) imageGray.Put(x, y - 1, 255);
			if (x + 1 < imageGray.GetWidth()) imageGray.Put(x + 1, y, 255);
			if (x - 1 >= 0) imageGray.Put(x - 1, y, 255);
			isTouched = true;
		} else if (isTouched && Tickable::GetCurrentTime() - msecLastTouch > msecFlush) {
			imageGray.PrintAscii();
			float confidence = 0.0f;
			int result = modelRunner.Recognize_GrayImage(imageGray.GetPointer(), &confidence);
			if (result < 0) {
				// nothing to do
			} else if (confidence < 0.3f) {
				terminal.Printf("unrecognized (%.2f)\n", confidence);
			} else {
				terminal.Printf("recognized: %s (%.2f)\n", labelTbl[result], confidence);
			}
			isTouched = false;
			display.DrawRectFill(rcCanvas, bgCanvas);
			imageGray.FillZero();
		}
		Tickable::Tick();
	}
}
