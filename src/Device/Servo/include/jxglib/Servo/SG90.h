//==============================================================================
// jxglib/Servo/SG90.h
//==============================================================================
#ifndef PICO_JXGLIB_SERVO_SG90_H
#define PICO_JXGLIB_SERVO_SG90_H
#include "jxglib/PWM.h"

namespace jxglib::Servo {

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
	void SetAngle(float angle, bool immediateFlag = false);
	void SetDuty(float duty);
    float GetDuty() const { return dutyCur_; }
    float GetAngle() const { return DutyToAngle(dutyCur_); }
    bool IsMoving() const { return dutyCur_ != dutyTarget_; }
	void SetSpeed(float degreePerSec) { degreePerSec_ = degreePerSec; }
    float GetSpeed() const { return degreePerSec_; }
public:
	static float AngleToDuty(float angle);
	static float DutyToAngle(float duty);
public:
	// virtual functions of Tickable
	virtual void OnTick() override;
};

}

#endif
