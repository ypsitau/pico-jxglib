//=============================================================================
// DigitRecognizer.cpp
//=============================================================================
#include "jxglib/ML/DigitRecognizer.h"

EmbedTFLiteModel("jxglib/ML/DigitRecognizer.tflite", modelData, modelDataSize);

namespace jxglib::ML {

DigitRecognizer::DigitRecognizer() : TFLiteRunner(modelData)
{
}

void DigitRecognizer::AddOpResolver(OpResolver& opResolver)
{
	opResolver.AddConv2D();
	opResolver.AddMaxPool2D();
	opResolver.AddReshape();
	opResolver.AddFullyConnected();
	opResolver.AddSoftmax();
}

int DigitRecognizer::Recognize(const uint8_t* imageData, float* pConfidence)
{
	Interpreter& interpreter = GetInterpreter();
	TfLiteTensor* input = interpreter.input(0);
	TfLiteTensor* output = interpreter.output(0);
	for(int i = 0; i < 784; i++) {
		input->data.int8[i] = static_cast<int8_t>(static_cast<int>(imageData[i]) - 128);
	}
	if (interpreter.Invoke() != kTfLiteOk) return -1;
	int8_t maxValue = output->data.int8[0];
	int maxIndex = 0;
	for (int i = 1; i < 10; i++) {
		if (output->data.int8[i] > maxValue) {
			maxValue = output->data.int8[i];
			maxIndex = i;
		}
	}
	if (pConfidence) *pConfidence = static_cast<float>(maxValue + 128) / 255.0f;
	return maxIndex;
}

}
