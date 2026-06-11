#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

namespace Servo {

class SG90 : public Tickable {
public:
	static const float dutyMin;
	static const float dutyMax;
private:
	const PWM& pwm_;
	float degreePerSec_;
	float dutyCur_;
	float dutyStart_;
	float dutyTarget_;
	uint32_t msecStart_;
	uint32_t msecTarget_;
public:
	SG90(const PWM& pwm);
public:
	void Initialize();
	void SetAngleQuickly(float angle);
	void SetAngleTarget(float angle);
	void SetDuty(float duty);
	void SetSpeed(float degreePerSec) { degreePerSec_ = degreePerSec; }
public:
	static float AngleToDuty(float angle);
	static float DutyToAngle(float duty);
public:
	// virtual functions of Tickable
	virtual void OnTick() override;
};

}

namespace Servo {

const float SG90::dutyMin = 0.025;
const float SG90::dutyMax = 0.12;

SG90::SG90(const PWM& pwm) : Tickable(0, Priority::Normal),
	pwm_(pwm), degreePerSec_{90}, dutyCur_{0.}, dutyStart_{0.}, dutyTarget_{0.}, msecStart_{0}, msecTarget_{0}
{}

void SG90::Initialize()
{
	pwm_.set_function().set_freq(50).set_chan_duty(0.).set_enabled();
}

void SG90::SetDuty(float duty)
{
	pwm_.set_chan_duty((duty < dutyMin)? dutyMin : (duty > dutyMax)? dutyMax : duty);
	dutyCur_ = duty;
}

void SG90::SetAngleQuickly(float angle)
{
	dutyTarget_ = AngleToDuty(angle);
	SetDuty(dutyTarget_);
}

void SG90::SetAngleTarget(float angle)
{
	dutyStart_ = dutyCur_;
	dutyTarget_ = AngleToDuty(angle);
	msecStart_ = Tickable::GetCurrentTime();
	msecTarget_ = msecStart_ + static_cast<uint32_t>(std::abs(angle - DutyToAngle(dutyStart_)) / degreePerSec_ * 1000);
}

float SG90::AngleToDuty(float angle)
{
	angle = (angle < -90.)? -90. : (angle > 90.)? 90. : angle;
	return dutyMin + (dutyMax - dutyMin) * (angle + 90.) / 180.;
}

float SG90::DutyToAngle(float duty)
{
	duty = (duty < dutyMin)? dutyMin : (duty > dutyMax)? dutyMax : duty;
	return (duty - dutyMin) / (dutyMax - dutyMin) * 180. - 90.;
}

void SG90::OnTick()
{
	if (dutyCur_ == dutyTarget_) return;
	uint32_t msecCur = Tickable::GetCurrentTime();
	if (msecCur < msecTarget_) {
		float duty = dutyStart_ + (dutyTarget_ - dutyStart_) * (msecCur - msecStart_) / (msecTarget_ - msecStart_);
		SetDuty(duty);
	} else {
		SetDuty(dutyTarget_);
	}
}

}

Servo::SG90 servo(PWM16);

ShellCmd_Named(servo_sg90, "servo-sg90", "controls SG90 servo")
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

int main(void)
{
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	servo.Initialize();
#if 0
	//LABOPlatform::Instance.Initialize();
	::adc_init();
	PWM16.set_function().set_freq(50).set_chan_duty(0.).set_enabled();
	PWM17.set_function().set_freq(50).set_chan_duty(0.).set_enabled();
	PWM18.set_function().set_freq(50).set_chan_duty(0.).set_enabled();
	GPIO26.adc_init();
	GPIO27.adc_init();
	GPIO28.adc_init();
	float center = 0.0725;
	float range = 0.0475;
	for (;;) {
		float valueL = GPIO26.adc_read() >> 4;
		float valueR = GPIO27.adc_read() >> 4;
		float valuePen = GPIO28.adc_read() >> 4;
		PWM16.set_chan_duty(((valueL - 128) / 128.) * range + center);
		PWM17.set_chan_duty(((valueR - 128) / 128.) * range + center);
		PWM18.set_chan_duty(((valuePen - 128) / 128.) * range + center);
		//::printf("%d %d %d\n", valueL, valueR, valuePen);
		Tickable::Tick();
	}
#endif
	for (;;) {
		Tickable::Tick();
	}
}
