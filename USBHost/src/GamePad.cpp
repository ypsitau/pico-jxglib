//==============================================================================
// GamePad.cpp
//==============================================================================
#include "jxglib/USBHost.h"

#if CFG_TUH_HID > 0
namespace jxglib {

//------------------------------------------------------------------------------
// USBHost::GamePad
//------------------------------------------------------------------------------
USBHost::GamePad::GamePad()
{
	ClearUsageAccessor();
}

void USBHost::GamePad::ClearUsageAccessor()
{
	pUsage_Button0		= &HID::UsageAccessor::None;
	pUsage_Button1		= &HID::UsageAccessor::None;
	pUsage_Button2		= &HID::UsageAccessor::None;
	pUsage_Button3		= &HID::UsageAccessor::None;
	pUsage_Button4		= &HID::UsageAccessor::None;
	pUsage_Button5		= &HID::UsageAccessor::None;
	pUsage_Button6		= &HID::UsageAccessor::None;
	pUsage_Button7		= &HID::UsageAccessor::None;
	pUsage_Button8		= &HID::UsageAccessor::None;
	pUsage_Button9		= &HID::UsageAccessor::None;
	pUsage_Button10		= &HID::UsageAccessor::None;
	pUsage_Button11		= &HID::UsageAccessor::None;
	pUsage_Button12		= &HID::UsageAccessor::None;
	pUsage_Axis0		= &HID::UsageAccessor::None;
	pUsage_Axis1		= &HID::UsageAccessor::None;
	pUsage_Axis2		= &HID::UsageAccessor::None;
	pUsage_Axis3		= &HID::UsageAccessor::None;
	pUsage_Axis4		= &HID::UsageAccessor::None;
	pUsage_Axis5		= &HID::UsageAccessor::None;
	pUsage_Axis6		= &HID::UsageAccessor::None;
	pUsage_Axis7		= &HID::UsageAccessor::None;
	pUsage_Axis8		= &HID::UsageAccessor::None;
	pUsage_HatSwitch	= &HID::UsageAccessor::None;
	pUsage_ButtonA		= &HID::UsageAccessor::None;
	pUsage_ButtonB		= &HID::UsageAccessor::None;
	pUsage_ButtonX		= &HID::UsageAccessor::None;
	pUsage_ButtonY		= &HID::UsageAccessor::None;
	pUsage_ButtonLB		= &HID::UsageAccessor::None;
	pUsage_ButtonRB		= &HID::UsageAccessor::None;
	pUsage_ButtonLT		= &HID::UsageAccessor::None;
	pUsage_ButtonRT		= &HID::UsageAccessor::None;
	pUsage_ButtonLeft	= &HID::UsageAccessor::None;
	pUsage_ButtonRight	= &HID::UsageAccessor::None;
	pUsage_ButtonBack	= &HID::UsageAccessor::None;
	pUsage_ButtonStart	= &HID::UsageAccessor::None;
	pUsage_ButtonHome	= &HID::UsageAccessor::None;
	pUsage_LeftX		= &HID::UsageAccessor::None;
	pUsage_LeftY		= &HID::UsageAccessor::None;
	pUsage_RightX		= &HID::UsageAccessor::None;
	pUsage_RightY		= &HID::UsageAccessor::None;
}

void USBHost::GamePad::OnMount()
{
	pUsage_Button0		= &GetApplication().FindUsageAccessorRecursive(0x0009'0001);
	pUsage_Button1		= &GetApplication().FindUsageAccessorRecursive(0x0009'0002);
	pUsage_Button2		= &GetApplication().FindUsageAccessorRecursive(0x0009'0003);
	pUsage_Button3		= &GetApplication().FindUsageAccessorRecursive(0x0009'0004);
	pUsage_Button4		= &GetApplication().FindUsageAccessorRecursive(0x0009'0005);
	pUsage_Button5		= &GetApplication().FindUsageAccessorRecursive(0x0009'0006);
	pUsage_Button6		= &GetApplication().FindUsageAccessorRecursive(0x0009'0007);
	pUsage_Button7		= &GetApplication().FindUsageAccessorRecursive(0x0009'0008);
	pUsage_Button8		= &GetApplication().FindUsageAccessorRecursive(0x0009'0009);
	pUsage_Button9		= &GetApplication().FindUsageAccessorRecursive(0x0009'000a);
	pUsage_Button10		= &GetApplication().FindUsageAccessorRecursive(0x0009'000b);
	pUsage_Button11		= &GetApplication().FindUsageAccessorRecursive(0x0009'000c);
	pUsage_Button12		= &GetApplication().FindUsageAccessorRecursive(0x0009'000d);
	pUsage_Axis0		= &GetApplication().FindUsageAccessorRecursive(0x0001'0030);
	pUsage_Axis1		= &GetApplication().FindUsageAccessorRecursive(0x0001'0031);
	pUsage_Axis2		= &GetApplication().FindUsageAccessorRecursive(0x0001'0032);
	pUsage_Axis3		= &GetApplication().FindUsageAccessorRecursive(0x0001'0033);
	pUsage_Axis4		= &GetApplication().FindUsageAccessorRecursive(0x0001'0034);
	pUsage_Axis5		= &GetApplication().FindUsageAccessorRecursive(0x0001'0035);
	pUsage_Axis6		= &GetApplication().FindUsageAccessorRecursive(0x0001'0036);
	pUsage_Axis7		= &GetApplication().FindUsageAccessorRecursive(0x0001'0037);
	pUsage_Axis8		= &GetApplication().FindUsageAccessorRecursive(0x0001'0038);
	pUsage_HatSwitch	= &GetApplication().FindUsageAccessorRecursive(0x0001'0039);
	uint16_t vid = GetHID().GetVID();
	uint16_t pid = GetHID().GetPID();
	pUsage_ButtonA		= pUsage_Button0;
	pUsage_ButtonB		= pUsage_Button1;
	pUsage_ButtonX		= pUsage_Button2;
	pUsage_ButtonY		= pUsage_Button3;
	pUsage_ButtonLB		= pUsage_Button4;
	pUsage_ButtonRB		= pUsage_Button5;
	pUsage_ButtonLT		= pUsage_Button6;
	pUsage_ButtonRT		= pUsage_Button7;
	pUsage_ButtonLeft	= pUsage_Button8;
	pUsage_ButtonRight	= pUsage_Button9;
	pUsage_ButtonBack	= pUsage_Button10;
	pUsage_ButtonStart	= pUsage_Button11;
	pUsage_ButtonHome	= pUsage_Button12;
	pUsage_LeftX		= pUsage_Axis0;
	pUsage_LeftY		= pUsage_Axis1;
	pUsage_RightX		= pUsage_Axis2;
	pUsage_RightY		= pUsage_Axis3;
	switch (vid) {
	case 0x056e:		// Elecom Co., Ltd.
		switch (pid) {
		case 0x2010:	// JC-U4113S DirectInput Mode
			pUsage_ButtonA		= pUsage_Button2;
			pUsage_ButtonB		= pUsage_Button3;
			pUsage_ButtonX		= pUsage_Button0;
			pUsage_ButtonY		= pUsage_Button1;
			pUsage_ButtonLB		= pUsage_Button4;
			pUsage_ButtonRB		= pUsage_Button5;
			pUsage_ButtonLT		= pUsage_Button6;
			pUsage_ButtonRT		= pUsage_Button7;
			pUsage_ButtonLeft	= pUsage_Button8;
			pUsage_ButtonRight	= pUsage_Button9;
			pUsage_ButtonBack	= pUsage_Button10;
			pUsage_ButtonStart	= pUsage_Button11;
			pUsage_ButtonHome	= pUsage_Button12;
			pUsage_LeftX		= pUsage_Axis0;
			pUsage_LeftY		= pUsage_Axis1;
			pUsage_RightX		= pUsage_Axis5;
			pUsage_RightY		= pUsage_Axis2;
			break;
		default:		// others
			pUsage_ButtonA		= pUsage_Button2;
			pUsage_ButtonB		= pUsage_Button3;
			pUsage_ButtonX		= pUsage_Button0;
			pUsage_ButtonY		= pUsage_Button1;
			pUsage_ButtonLB		= pUsage_Button4;
			pUsage_ButtonRB		= pUsage_Button5;
			pUsage_ButtonLT		= pUsage_Button6;
			pUsage_ButtonRT		= pUsage_Button7;
			pUsage_ButtonLeft	= pUsage_Button8;
			pUsage_ButtonRight	= pUsage_Button9;
			pUsage_ButtonBack	= pUsage_Button10;
			pUsage_ButtonStart	= pUsage_Button11;
			pUsage_ButtonHome	= pUsage_Button12;
			pUsage_LeftX		= pUsage_Axis0;
			pUsage_LeftY		= pUsage_Axis1;
			pUsage_RightX		= pUsage_Axis5;
			pUsage_RightY		= pUsage_Axis2;
			break;
		}
		break;
	default:
		break;
	}
}

void USBHost::GamePad::OnUmount()
{
	ClearUsageAccessor();
}

float USBHost::GamePad::GetCookedAxis(const HID::UsageAccessor& usageAccessor) const
{
	if (!usageAccessor.IsValid()) return 0.;
	int32_t valueMin = usageAccessor.GetLogicalMinimum();
	int32_t valueMax = usageAccessor.GetLogicalMaximum();
	int32_t value = usageAccessor.GetVariable(GetHID().GetReport());
	if (valueMin <= 0) {
		if (valueMin == 0) {
			int32_t valueMid = (valueMax + 1) / 2;
			valueMin -= valueMid;
			valueMax -= valueMid;
			value -= valueMid;
			if (valueMin == 0) return 0.;
		}
		return (value < 0)? -static_cast<float>(value) / valueMin :
			(valueMax > 0)? static_cast<float>(value) / valueMax : 0;
	} else if (valueMax > 0) {
		return static_cast<float>(value) / valueMax;
	} else {
		return 0.;
	}
}

}

#endif
