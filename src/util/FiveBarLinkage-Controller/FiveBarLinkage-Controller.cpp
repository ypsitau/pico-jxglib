#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Servo/SG90.h"

using namespace jxglib;

#if 0
Servo::SG90 servo(PWM::N(16));

ShellCmd_Named(servo, "servo", "controls servo motor")
{
	char* endp;
	float angle = ::strtof(argv[1], &endp);
	if (*endp != '\0') {
		terr.Printf("Invalid angle: %s\n", argv[1]);
		return Result::Error;
	}
	servo.SetAngleTarget(angle);
	return Result::Success;
}
#endif

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	//servo.Initialize();
#if 1
	//LABOPlatform::Instance.Initialize();
	::adc_init();
	Servo::SG90 servoL(PWM16);
	Servo::SG90 servoR(PWM17);
	Servo::SG90 servoPen(PWM18);
	servoL.Initialize();
	servoR.Initialize();
	servoPen.Initialize();
	GPIO26.adc_init();
	GPIO27.adc_init();
	GPIO28.adc_init();
	float center = 0.0725;
	float range = 0.0475;
	for (;;) {
		float angleL = (static_cast<float>(GPIO26.adc_read() >> 4) / 256) * 180.f - 90.f;
		float angleR = (static_cast<float>(GPIO27.adc_read() >> 4) / 256) * 180.f - 90.f;
		float anglePen = (static_cast<float>(GPIO28.adc_read() >> 4) / 256) * 180.f - 90.f;
		servoL.SetAngle(angleL);
		servoR.SetAngle(angleR);
		servoPen.SetAngle(anglePen);
		::printf("%f %f %f\n", angleL, angleR, anglePen);
		//Tickable::Sleep(500);
		Tickable::Tick();
	}
#endif
	for (;;) {
		Tickable::Tick();
	}
}
