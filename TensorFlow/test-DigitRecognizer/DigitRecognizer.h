#ifndef DIGIT_RECOGNIZER_H
#define DIGIT_RECOGNIZER_H
#include <stdint.h>
#include "pico/stdlib.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

class DigitRecognizer {
public:
    static constexpr size_t ArenaSize = 1024 * 10;
    static constexpr size_t tOpCount = 8;
private:
	std::unique_ptr<tflite::MicroMutableOpResolver<tOpCount>> resolver_;
	std::unique_ptr<tflite::MicroInterpreter> interpreter_;
	alignas(16) uint8_t arena_[ArenaSize];
public:
	DigitRecognizer();
	~DigitRecognizer() {}
public:
	bool Initialize();
	bool Invoke();
public:
	size_t GetArenaUsedBytes() const { return interpreter_->arena_used_bytes(); }
	TfLiteTensor* GetInput(int index) { return interpreter_->input(index); }
	TfLiteTensor* GetOutput(int index) { return interpreter_->output(index); }
};

#endif
