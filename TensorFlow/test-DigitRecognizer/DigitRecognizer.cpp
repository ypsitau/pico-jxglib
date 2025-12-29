#include "DigitRecognizer.h"
#include "mnist_model.h"

DigitRecognizer::DigitRecognizer()
{
}

bool DigitRecognizer::Initialize()
{
	tflite::InitializeTarget();
	const tflite::Model* model = tflite::GetModel(conv_mnist_quant_tflite);
	if (model->version() != TFLITE_SCHEMA_VERSION) return false;
	resolver_.reset(new tflite::MicroMutableOpResolver<tOpCount>());
	interpreter_.reset(new tflite::MicroInterpreter(model, *resolver_, arena_, ArenaSize));
	resolver_->AddConv2D();
	resolver_->AddMaxPool2D();
	resolver_->AddFullyConnected();
	resolver_->AddReshape();
	resolver_->AddSoftmax();
	if (interpreter_->AllocateTensors() != kTfLiteOk) return false;
	return true;
}

bool DigitRecognizer::Invoke()
{
	return interpreter_->Invoke() == kTfLiteOk;
}
