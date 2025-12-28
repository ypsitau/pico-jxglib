#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

using namespace jxglib;

uint8_t tensorArena[32768];

int main(void)
{
	::stdio_init_all();
	tflite::InitializeTarget();
	const tflite::Model* model = nullptr; //tflite::GetModel(mnist_model);
	static tflite::MicroMutableOpResolver<5> resolver;
	resolver.AddConv2D();
    resolver.AddMaxPool2D();
    resolver.AddFullyConnected();
    resolver.AddReshape();
    resolver.AddSoftmax();
	static tflite::MicroInterpreter interpreter(model, resolver, tensorArena, sizeof(tensorArena));
	TfLiteTensor* input = interpreter.input(0);
	TfLiteTensor* output = interpreter.output(0);
	LABOPlatform::Instance.AttachStdio().Initialize();
	//LABOPlatform::Instance.Initialize();
	::adc_init();
	::adc_set_temp_sensor_enabled(true);
	for (;;) Tickable::Tick();
}
