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
#include "jxglib/Image.h"

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
size_t CountElements(const TfLiteTensor& tensor);

template<typename T> static size_t ArgMax(const TfLiteTensor& tensor, T* pValueMax = nullptr) { return 0; }

template<> size_t ArgMax<int8_t>(const TfLiteTensor& tensor, int8_t* pValueMax) {
    return ArgMax<int8_t>(tensor.data.int8, CountElements(tensor), pValueMax);
}

//-----------------------------------------------------------------------------
// TfLiteModelRunnerBase
//-----------------------------------------------------------------------------
class TfLiteModelRunnerBase {
protected:
	uint8_t* arena_;
	size_t bytesArena_;
	std::unique_ptr<tflite::MicroOpResolver> opResolver_;
	std::unique_ptr<tflite::MicroInterpreter> interpreter_;
public:
	TfLiteModelRunnerBase(uint8_t* arena, size_t bytesArena);
	~TfLiteModelRunnerBase() {}
public:
	TfLiteStatus Initialize(const void* modelData);
public:
	void PrintInfo(Printable& tout = Stdio::Instance) const;
	size_t GetArenaUsedBytes() const { return interpreter_->arena_used_bytes(); }
	TfLiteStatus Invoke() { return interpreter_->Invoke(); }
public:
	tflite::MicroInterpreter& GetInterpreter() { return *interpreter_; }
	TfLiteTensor& GetInput(size_t index) { return *interpreter_->input(index); }
	TfLiteTensor& GetOutput(size_t index) { return *interpreter_->output(index); }
public:
	TfLiteModelRunnerBase& SetInputData(const Image& image);
	int Recognize(float* pConfidence = nullptr);
};

//-----------------------------------------------------------------------------
// TfLiteModelRunner
//-----------------------------------------------------------------------------
template<size_t ArenaBytes, size_t tOpCount> class TfLiteModelRunner : public TfLiteModelRunnerBase {
private:
	using OpResolver = tflite::MicroMutableOpResolver<tOpCount>;
private:
	alignas(16) uint8_t arena_[ArenaBytes];
public:
	TfLiteModelRunner() : TfLiteModelRunnerBase(arena_, ArenaBytes) {}
public:
	OpResolver& PrepareOpResolver() {
		OpResolver* opResolver = new OpResolver();
		opResolver_.reset(opResolver);
		return *opResolver;
	}
};

}

#endif
