//=============================================================================
// TfLiteModelRunner.h
//=============================================================================
#ifndef JXGLIB_ML_TFLITEMODELRUNNER_H
#define JXGLIB_ML_TFLITEMODELRUNNER_H
#include <stdint.h>
#include "pico/stdlib.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"
#include "jxglib/ML.h"

//-----------------------------------------------------------------------------
// EmbedTfLiteModel
//-----------------------------------------------------------------------------
#define EmbedTfLiteModel(pathNameModel, varNameData, varNameSize) \
__asm__( \
	".pushsection .rodata\n" \
    ".balign 16\n" \
    #varNameData ":\n" \
    ".incbin " #pathNameModel "\n" \
    #varNameData "_end:\n" \
    ".popsection\n" \
    ".pushsection .rodata\n" \
    ".balign 4\n" \
    #varNameSize ":\n" \
    ".int " #varNameData "_end - " #varNameData "\n" \
    ".popsection\n" \
); \
extern "C" const uint8_t varNameData[]; \
extern "C" const int varNameSize

namespace jxglib::ML {

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
size_t CountElements(const TfLiteTensor* tensor);

template<typename T> static size_t ArgMax(const TfLiteTensor* tensor, T* pValueMax = nullptr) { return 0; }

template<> size_t ArgMax<int8_t>(const TfLiteTensor* tensor, int8_t* pValueMax) {
    return ArgMax<int8_t>(tensor->data.int8, CountElements(tensor), pValueMax);
}

//-----------------------------------------------------------------------------
// TfLiteModelRunner
//-----------------------------------------------------------------------------
template<size_t ArenaBytes_, size_t tOpCount_> class TfLiteModelRunner {
public:
	using OpResolver = tflite::MicroMutableOpResolver<tOpCount_>;
	using Interpreter = tflite::MicroInterpreter;
public:
	static constexpr size_t ArenaBytes = ArenaBytes_;
	static constexpr size_t tOpCount = tOpCount_;
protected:
	const uint8_t* modelData_;
	std::unique_ptr<OpResolver> opResolver_;
	std::unique_ptr<Interpreter> interpreter_;
	alignas(16) uint8_t arena_[ArenaBytes];
public:
	TfLiteModelRunner(const uint8_t* modelData) : modelData_{modelData} {};
	~TfLiteModelRunner() {}
public:
	void Initialize() {
		tflite::InitializeTarget();
		opResolver_.reset(new OpResolver());
	}
	bool Allocate() {
		const tflite::Model* model = tflite::GetModel(modelData_);
		if (model->version() != TFLITE_SCHEMA_VERSION) return false;
		interpreter_.reset(new Interpreter(model, *opResolver_, arena_, ArenaBytes));
		return interpreter_->AllocateTensors() == kTfLiteOk;
	}
public:
	size_t GetArenaUsedBytes() const { return interpreter_->arena_used_bytes(); }
	OpResolver& GetOpResolver() { return *opResolver_; }
	Interpreter& GetInterpreter() { return *interpreter_; }
	TfLiteTensor* GetInput(size_t index) { return interpreter_->input(index); }
	TfLiteTensor* GetOutput(size_t index) { return interpreter_->output(index); }
	bool Invoke() { return interpreter_->Invoke() == kTfLiteOk; }
public:
	int Recognize_GrayImage(const uint8_t* imageData, float* pConfidence = nullptr) {
		TfLiteTensor* input = GetInput(0);
		TfLiteTensor* output = GetOutput(0);
		int nElements = CountElements(input);
		for (int i = 0; i < nElements; i++) {
			input->data.int8[i] = static_cast<int8_t>(static_cast<int>(imageData[i]) - 128);
		}
		if (!Invoke()) return -1;
		int8_t valueMax;
		size_t indexMax = ArgMax<int8_t>(output, &valueMax);
		if (pConfidence) *pConfidence = static_cast<float>(valueMax + 128) / 255.0f;
		return indexMax;
	}
};

}

#endif
