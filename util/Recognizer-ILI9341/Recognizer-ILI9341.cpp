#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Image.h"
#include "jxglib/Canvas.h"
#include "jxglib/Display/ILI9341.h"
#include "jxglib/Font/shinonome14.h"
#include "jxglib/ML/TfLiteModelRunner.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

EmbedTfLiteModel("jxglib/ML/Model/Recognizer-EMNIST-mnist-binary.tflite", modelData_mnist, modelDataSize_mnist);
EmbedTfLiteModel("jxglib/ML/Model/Recognizer-EMNIST-letters-binary.tflite", modelData_letters, modelDataSize_letters);
EmbedTfLiteModel("jxglib/ML/Model/Recognizer-EMNIST-bymerge-binary.tflite", modelData_bymerge, modelDataSize_bymerge);

struct Config {
	const void* modelData;
	const char* strPrompt;
	const char* labelTbl[80];
};

static const Config configTbl[] = {
	{ modelData_bymerge, "Draw one of 0-9, A-Z, and a-z", {
		"0 (number)", "1", "2", "3", "4", "5", "6", "7", "8", "9",
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
		"K", "L", "M", "N", "O (alphabet)", "P", "Q", "R", "S", "T",
		"U", "V", "W", "X", "Y", "Z",
		"a", "b", "d", "e", "f", "g", "h", "n", "q", "r", "t",
	}, },
	{ modelData_mnist, "Draw one of 0-9", {
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	}, },
	{ modelData_letters, "Draw one of A-Z", {
		"A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
		"K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
		"U", "V", "W", "X", "Y", "Z",
	}, },
	{ modelData_mnist, "Draw one of 0-9", {
		"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	}, },
};

static ML::TfLiteModelRunner<16500, 8> modelRunner;

int main()
{
	::stdio_init_all();
	LABOPlatform::Instance.Initialize();
	::spi_init(spi0, 2 * 1000 * 1000);
	::spi_init(spi1, 125'000'000);
	GPIO0.set_function_SIO().set_dir_IN().pull_up();
	GPIO1.set_function_SIO().set_dir_IN().pull_up();
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
	Rect rcDrawArea(0, 0, 240, 240);
	Color bgDrawArea(0, 192, 0);
	Size szPixelDrawArea(static_cast<int>(rcDrawArea.width * 0.08f), static_cast<int>(rcDrawArea.height * 0.08f));
	terminal.Initialize()
		.AttachDisplay(display, {0, 240, 240, 80})
		.SetFont(Font::shinonome14)
		.SetColorBg(Color::black)
		.SetSpacingRatio(1., 1.2)
		.ClearScreen();
	touchScreen.Initialize(display);
	//touchScreen.Calibrate(display, true);
	//touchScreen.PrintCalibration();
	int iConfig = (GPIO0.get()? 0 : 1) + (GPIO1.get()? 0 : 2);
	terminal.Println(configTbl[iConfig].strPrompt);
	const uint32_t msecFlush = 1000;
	uint32_t msecLastTouch = Tickable::GetCurrentTime();
	display.DrawRectFill(rcDrawArea, bgDrawArea);
	bool isTouched = false;
	Canvas canvas;
	do {
		auto& opResolver = modelRunner.PrepareOpResolver();
		opResolver.AddConv2D();
		opResolver.AddMaxPool2D();
		opResolver.AddReshape();
		opResolver.AddFullyConnected();
		opResolver.AddSoftmax();
		if (modelRunner.Initialize(configTbl[iConfig].modelData) != kTfLiteOk) return 1;
		//modelRunner.PrintInfo();
		auto& input = modelRunner.GetInput(0);
		assert(input.type == kTfLiteInt8 && input.dims->size == 4);
		canvas.Allocate(Image::Format::Gray, input.dims->data[2], input.dims->data[1]);
	} while (0);
	Size szPixelCanvas(static_cast<int>(canvas.GetWidth() * 0.1f), static_cast<int>(canvas.GetHeight() * 0.1f));
	for (;;) {
		Point pt;
		if (touchScreen.ReadPoint(&pt) && rcDrawArea.Contains(pt)) {
			display.DrawRectFill(
				pt.x - szPixelDrawArea.width / 2, pt.y - szPixelDrawArea.height / 2,
				szPixelDrawArea.width, szPixelDrawArea.height);
			int x = (pt.x - (rcDrawArea.x)) * canvas.GetWidth() / rcDrawArea.width;
			int y = (pt.y - (rcDrawArea.y)) * canvas.GetHeight() / rcDrawArea.height;
			canvas.DrawRectFill(
				x - szPixelCanvas.width / 2, y - szPixelCanvas.height / 2,
				szPixelCanvas.width, szPixelCanvas.height, Color::white);
			isTouched = true;
			msecLastTouch = Tickable::GetCurrentTime();
		} else if (isTouched && Tickable::GetCurrentTime() - msecLastTouch > msecFlush) {
			canvas.GetImageOwn().PrintAscii();
			float confidence = 0.0f;
			int result = modelRunner.SetInputData(canvas.GetImageOwn()).Recognize(&confidence);
			if (result < 0) {
				// nothing to do
			} else if (confidence < 0.3f) {
				terminal.Printf("unrecognized (%.2f)\n", confidence);
			} else {
				terminal.Printf("recognized: %s (%.2f)\n", configTbl[iConfig].labelTbl[result], confidence);
			}
			isTouched = false;
			display.DrawRectFill(rcDrawArea, bgDrawArea);
			canvas.Fill(Color::black);
		}
		Tickable::Tick();
	}
}
