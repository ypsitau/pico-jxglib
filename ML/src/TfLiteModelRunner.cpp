//=============================================================================
// TfLiteModelRunner.cpp
//=============================================================================
#include "jxglib/ML/TfLiteModelRunner.h"

namespace jxglib::ML {

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
const char* GetTensorTypeName(TfLiteType type)
{
	return
		(type == kTfLiteNoType)? "NoType" :
		(type == kTfLiteFloat32)? "Float32" :
		(type == kTfLiteInt32)? "Int32" :
		(type == kTfLiteUInt8)? "UInt8" :
		(type == kTfLiteInt64)? "Int64" :
		(type == kTfLiteString)? "String" :
		(type == kTfLiteBool)? "Bool" :
		(type == kTfLiteInt16)? "Int16" :
		(type == kTfLiteComplex64)? "Complex64" :
		(type == kTfLiteInt8)? "Int8" :
		(type == kTfLiteFloat16)? "Float16" :
		(type == kTfLiteFloat64)? "Float64" :
		(type == kTfLiteComplex128)? "Complex128" :
		(type == kTfLiteUInt64)? "UInt64" :
		(type == kTfLiteResource)? "Resource" :
		(type == kTfLiteVariant)? "Variant" :
		(type == kTfLiteUInt32)? "UInt32" :
		(type == kTfLiteUInt16)? "UInt16" :
		(type == kTfLiteInt4)? "Int4" :
		(type == kTfLiteBFloat16)? "BFloat16" :
		"Unknown";
}

void ToString(char* str, size_t len, const TfLiteTensor& tensor)
{
	int pos = 0;
	pos += ::snprintf(str + pos, len - pos, "type=%s, dims=[", GetTensorTypeName(tensor.type));
	for (int d = 0; d < tensor.dims->size; d++) {
		if (d > 0) pos += ::snprintf(str + pos, len - pos, ", ");
		pos += ::snprintf(str + pos, len - pos, "%d", tensor.dims->data[d]);
	}
	pos += ::snprintf(str + pos, len - pos, "], size=%zu", CountElements(tensor));
}

size_t CountElements(const TfLiteTensor& tensor)
{
	size_t count = 1;
	for (int i = 0; i < tensor.dims->size; i++) count *= tensor.dims->data[i];
	return count;
}

//-----------------------------------------------------------------------------
// TfLiteModelRunnerBase
//-----------------------------------------------------------------------------
TfLiteModelRunnerBase::TfLiteModelRunnerBase(const uint8_t* modelData, uint8_t* arena, size_t bytesArena) :
		modelData_{modelData}, arena_{arena}, bytesArena_{bytesArena}
{}

TfLiteStatus TfLiteModelRunnerBase::Initialize()
{
	const tflite::Model* model = tflite::GetModel(modelData_);
	if (model->version() != TFLITE_SCHEMA_VERSION) return kTfLiteError;
	interpreter_.reset(new tflite::MicroInterpreter(model, *opResolver_, arena_, bytesArena_));
	return interpreter_->AllocateTensors();
}

void TfLiteModelRunnerBase::PrintInfo(Printable& tout) const
{
	char buff[256];
	tout.Printf("arena %zu/%zu bytes used\n", GetArenaUsedBytes(), bytesArena_);
	for (size_t i = 0; i < interpreter_->inputs_size(); i++) {
		ToString(buff, sizeof(buff), *interpreter_->input(i));
		tout.Printf("input(%zu)  %s\n", i, buff);
	}
	for (size_t i = 0; i < interpreter_->outputs_size(); i++) {
		ToString(buff, sizeof(buff), *interpreter_->output(i));
		tout.Printf("output(%zu) %s\n", i, buff);
	}
}

int TfLiteModelRunnerBase::Recognize_GrayImage(const uint8_t* imageData, float* pConfidence)
{
	TfLiteTensor& input = GetInput(0);
	TfLiteTensor& output = GetOutput(0);
	int nElements = CountElements(input);
	for (int i = 0; i < nElements; i++) {
		input.data.int8[i] = static_cast<int8_t>(static_cast<int>(imageData[i]) - 128);
	}
	if (Invoke() != kTfLiteOk) return -1;
	int8_t valueMax;
	size_t indexMax = ArgMax<int8_t>(output, &valueMax);
	if (pConfidence) *pConfidence = static_cast<float>(valueMax + 128) / 255.0f;
	return indexMax;
}

}
