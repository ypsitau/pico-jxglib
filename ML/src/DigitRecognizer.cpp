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
	TfLiteTensor* input = GetInput(0);
	TfLiteTensor* output = GetOutput(0);
	int nElements = CountElements(input);
	for(int i = 0; i < nElements; i++) {
		input->data.int8[i] = static_cast<int8_t>(static_cast<int>(imageData[i]) - 128);
	}
	if (!Invoke()) return -1;
	int8_t valueMax;
	size_t indexMax = ArgMax<int8_t>(output, &valueMax);
	if (pConfidence) *pConfidence = static_cast<float>(valueMax + 128) / 255.0f;
	return indexMax;
}

}
