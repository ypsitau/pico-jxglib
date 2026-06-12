//==============================================================================
// SG90.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Servo/SG90.h"

namespace jxglib::Servo {

const float SG90::dutyMin = 0.025;
const float SG90::dutyMax = 0.12;

SG90::SG90(const PWM& pwm) : Tickable(0, Priority::Normal),
	pwm_(pwm), degreePerSec_{90}, dutyCur_{0.}, dutyStart_{0.}, dutyTarget_{0.}, msecStart_{0}, msecTarget_{0}
{}

void SG90::Initialize()
{
	pwm_.set_function().set_freq(50).set_chan_duty(dutyCur_).set_enabled();
}

void SG90::SetDuty(float duty)
{
	pwm_.set_chan_duty((duty < dutyMin)? dutyMin : (duty > dutyMax)? dutyMax : duty);
	dutyCur_ = duty;
}

void SG90::SetAngle(float angle, bool immediateFlag)
{
    if (immediateFlag) {
        dutyTarget_ = AngleToDuty(angle);
        SetDuty(dutyTarget_);
    } else if (!IsMoving()) {
        dutyStart_ = dutyCur_;
        dutyTarget_ = AngleToDuty(angle);
        msecStart_ = Tickable::GetCurrentTime();
        float degreeDiff = std::abs(angle - DutyToAngle(dutyStart_));
	    msecTarget_ = msecStart_ + static_cast<uint32_t>(degreeDiff / degreePerSec_ * 1000);
    }
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
