//==============================================================================
// jxglib/Mouse.h
//==============================================================================
#ifndef PICO_JXGLIB_MOUSE_H
#define PICO_JXGLIB_MOUSE_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Mouse
//------------------------------------------------------------------------------
class Mouse {
public:
	struct Button {
		static const uint8_t Left		= (1 << 0);
		static const uint8_t Right		= (1 << 1);
		static const uint8_t Middle		= (1 << 2);
		static const uint8_t Backward	= (1 << 3);
		static const uint8_t Forward	= (1 << 4);
	};
public:
	class Status {
	private:
		Point pt_;
		int deltaX_;
		int deltaY_;
		int deltaWheel_;
		int deltaPan_;
		uint8_t buttons_;
		uint8_t buttonsPrev_;
	public:
		Status() : deltaX_{0}, deltaY_{0}, deltaWheel_{0}, deltaPan_{0}, buttons_{0}, buttonsPrev_{0} {}
		Status(const Status& status) : pt_{status.pt_}, deltaX_{status.deltaX_}, deltaY_{status.deltaY_},
			deltaWheel_{status.deltaWheel_}, deltaPan_{status.deltaPan_},
			buttons_{status.buttons_}, buttonsPrev_{status.buttonsPrev_} {}
	public:
		void SetPoint(const Point& pt) { pt_ = pt; }
		void Update(const Point& pt, int deltaX, int deltaY, int deltaWheel, int deltaPan, uint8_t buttons);
		Status Capture();
		uint8_t GetButtons() const { return buttons_; }
		const Point& GetPoint() const { return pt_; }
		int GetDeltaX() const { return deltaX_; }
		int GetDeltaY() const { return deltaY_; }
		int GetDeltaWheel() const { return deltaWheel_; }
		uint8_t GetPan() const { return deltaPan_; }
		bool GetButtonLeft() const { return !!(buttons_ & Button::Left); }
		bool GetButtonRight() const { return !!(buttons_ & Button::Right); }
		bool GetButtonMiddle() const { return !!(buttons_ & Button::Middle); }
		bool GetButtonBackward() const { return !!(buttons_ & Button::Backward); }
		bool GetButtonForward() const { return !!(buttons_ & Button::Forward); }
		bool IsButtonLeftChanged() const { return !!((buttons_ ^ buttonsPrev_) & Button::Left); }
		bool IsButtonRightChanged() const { return !!((buttons_ ^ buttonsPrev_) & Button::Right); }
		bool IsButtonMiddleChanged() const { return !!((buttons_ ^ buttonsPrev_) & Button::Middle); }
		bool IsButtonBackwardChanged() const { return !!((buttons_ ^ buttonsPrev_) & Button::Backward); }
		bool IsButtonForwardChanged() const { return !!((buttons_ ^ buttonsPrev_) & Button::Forward); }
		bool IsButtonLeftPressed() const { return IsButtonLeftChanged() && GetButtonLeft(); }
		bool IsButtonRightPressed() const { return IsButtonRightChanged() && GetButtonRight(); }
		bool IsButtonMiddlePressed() const { return IsButtonMiddleChanged() && GetButtonMiddle(); }
		bool IsButtonBackwardPressed() const { return IsButtonBackwardChanged() && GetButtonBackward(); }
		bool IsButtonForwardPressed() const { return IsButtonForwardChanged() && GetButtonForward(); }
		bool IsButtonLeftReleased() const { return IsButtonLeftChanged() && !GetButtonLeft(); }
		bool IsButtonRightReleased() const { return IsButtonRightChanged() && !GetButtonRight(); }
		bool IsButtonMiddleReleased() const { return IsButtonMiddleChanged() && !GetButtonMiddle(); }
		bool IsButtonBackwardReleased() const { return IsButtonBackwardChanged() && !GetButtonBackward(); }
		bool IsButtonForwardReleased() const { return IsButtonForwardChanged() && !GetButtonForward(); }
	};
protected:
	Status status_;
public:
	Mouse() {}
public:
	Status CaptureStatus() { return status_.Capture(); }
	virtual Mouse& SetSensibility(float sensibility) { return *this; }
	virtual Mouse& SetStage(const Rect& rcStage) { return *this; }
};

}

#endif
