//==============================================================================
// jxglib/USBHost.h
//==============================================================================
#ifndef PICO_JXGLIB_USBHOST_H
#define PICO_JXGLIB_USBHOST_H
#include <memory>
#include "pico/stdlib.h"
#include "tusb.h"
#include "jxglib/FIFOBuff.h"
#include "jxglib/Tickable.h"
#include "jxglib/Keyboard.h"
#include "jxglib/Mouse.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
class USBHost : public Tickable {
public:
	class GenericHID;
	class ReportDescriptor {
	public:
		struct ItemType {
			// 6.2.2.4 Main Items
			static const uint8_t Input				= 0x80;
			static const uint8_t Output				= 0x90;
			static const uint8_t Feature			= 0xb0;
			static const uint8_t Collection			= 0xa0;
			static const uint8_t EndCollection		= 0xc0;
			// 6.2.2.7 Global Items
			static const uint8_t UsagePage			= 0x04;
			static const uint8_t LogicalMinimum		= 0x14;
			static const uint8_t LogicalMaximum		= 0x24;
			static const uint8_t PhysicalMinimum	= 0x34;
			static const uint8_t PhysicalMaximum	= 0x44;
			static const uint8_t UnitExponent		= 0x54;
			static const uint8_t Unit				= 0x64;
			static const uint8_t ReportSize			= 0x74;
			static const uint8_t ReportID			= 0x84;
			static const uint8_t ReportCount		= 0x94;
			static const uint8_t Push				= 0xa4;
			static const uint8_t Pop				= 0xb4;
			// 6.2.2.8 Local Items
			static const uint8_t Usage				= 0x08;
			static const uint8_t UsageMinimum		= 0x18;
			static const uint8_t UsageMaximum		= 0x28;
			static const uint8_t DesignatorIndex	= 0x38;
			static const uint8_t DesignatorMinimum	= 0x48;
			static const uint8_t DesignatorMaximum	= 0x58;
			static const uint8_t StringIndex		= 0x78;
			static const uint8_t StringMaximum		= 0x88;
			static const uint8_t StringMinimum		= 0x98;
			static const uint8_t Delimeter			= 0xa8;
		};
		struct MainItemData {
		public:
			static bool IsData(uint32_t mainItemData)			{ return  !(mainItemData & (1 << 0)); }
			static bool IsConstatnt(uint32_t mainItemData)		{ return !!(mainItemData & (1 << 0)); }
			static bool IsArray(uint32_t mainItemData)			{ return  !(mainItemData & (1 << 1)); }
			static bool IsVariable(uint32_t mainItemData)		{ return !!(mainItemData & (1 << 1)); }
			static bool IsAbsolute(uint32_t mainItemData)		{ return  !(mainItemData & (1 << 2)); }
			static bool IsRelative(uint32_t mainItemData)		{ return !!(mainItemData & (1 << 2)); }
			static bool IsNoWrap(uint32_t mainItemData)			{ return  !(mainItemData & (1 << 3)); }
			static bool IsWrap(uint32_t mainItemData)			{ return !!(mainItemData & (1 << 3)); }
			static bool IsLinear(uint32_t mainItemData)			{ return  !(mainItemData & (1 << 4)); }
			static bool IsNonLinear(uint32_t mainItemData)		{ return !!(mainItemData & (1 << 4)); }
			static bool IsPreferredState(uint32_t mainItemData)	{ return  !(mainItemData & (1 << 5)); }
			static bool IsNoPreferred(uint32_t mainItemData)	{ return !!(mainItemData & (1 << 5)); }
			static bool IsNoNullPosition(uint32_t mainItemData)	{ return  !(mainItemData & (1 << 6)); }
			static bool IsNullState(uint32_t mainItemData)		{ return !!(mainItemData & (1 << 6)); }
			static bool IsNonVolatile(uint32_t mainItemData)	{ return  !(mainItemData & (1 << 7)); }	// for Output and Feature
			static bool IsVolatile(uint32_t mainItemData)		{ return !!(mainItemData & (1 << 7)); }	// for Output and Feature
			static bool IsBitField(uint32_t mainItemData)		{ return  !(mainItemData & (1 << 8)); }
			static bool IsBufferedBytes(uint32_t mainItemData)	{ return !!(mainItemData & (1 << 8)); }
		};
		enum class CollectionType : uint8_t {
			Physical		= 0x00,
			Application		= 0x01,
			Logical			= 0x02,
			Report			= 0x03,
			NamedArray		= 0x04,
			UsageSwitch		= 0x05,
			UsageModifier	= 0x06,
		};

		struct Range {
			uint32_t minimum;
			uint32_t maximum;
		public:
			Range() : minimum{0}, maximum{0} {}
			Range(uint32_t minimum, uint32_t maximum) : minimum{minimum}, maximum{maximum} {}
		};
		struct GlobalItem {
			uint8_t itemType;
			uint32_t mainItemData;
			uint32_t logicalMinimum;
			uint32_t logicalMaximum;
			uint32_t physicalMinimum;
			uint32_t physicalMaximum;
			uint32_t unitExponent;
			uint32_t unit;
			uint32_t reportSize;
			uint32_t reportID;
			uint32_t reportCount;
		public:
			static const GlobalItem None;
		public:
			void Clear() { ::memset(this, 0x00, sizeof(GlobalItem)); }
			void Print(int indentLevel = 0) const;
		};
		struct LocalItem {
			int nUsage;
			Range usageTbl[8];
			int nDesignatorIndex;
			Range designatorIndexTbl[4];
			int nStringIndex;
			Range stringIndexTbl[4];
			uint32_t delimeter;
		public:
			void Clear() { ::memset(this, 0x00, sizeof(LocalItem)); }
		};
		class UsageInfo {
		private:
			uint32_t usage_;
			const GlobalItem* pGlobalItem_;
			uint32_t reportOffset_;
			std::unique_ptr<UsageInfo> pUsageInfoNext_;
		public:
			static const UsageInfo None;
		public:
			UsageInfo() : usage_{0}, pGlobalItem_{nullptr}, reportOffset_{0} {}
			constexpr UsageInfo(uint32_t usage, const GlobalItem* pGlobalItem, uint32_t reportOffset) :
				usage_{usage}, pGlobalItem_{pGlobalItem}, reportOffset_{reportOffset} {}
		public:
			void AppendList(UsageInfo* pUsageInfo) { GetListLast()->pUsageInfoNext_.reset(pUsageInfo); }
			UsageInfo* GetListNext() { return pUsageInfoNext_.get(); }
			const UsageInfo* GetListNext() const { return pUsageInfoNext_.get(); }
			UsageInfo* GetListLast();
		public:
			bool IsValid() const { return usage_ != 0; }
			uint32_t GetUsage() const { return usage_; }
			uint32_t GetLogicalMinimum() const { return pGlobalItem_->logicalMinimum; }
			uint32_t GetLogicalMaximum() const { return pGlobalItem_->logicalMaximum; }
			uint32_t GetPhysicalMinimum() const { return pGlobalItem_->physicalMinimum; }
			uint32_t GetPhysicalMaximum() const { return pGlobalItem_->physicalMaximum; }
			uint32_t GetUnitExponent() const { return pGlobalItem_->unitExponent; }
			uint32_t GetUnit() const { return pGlobalItem_->unit; }
			uint32_t GetReportSize() const { return pGlobalItem_->reportSize; }
			uint32_t GetReportID() const { return pGlobalItem_->reportID; }
			uint32_t GetReportCount() const { return pGlobalItem_->reportCount; }
			uint32_t GetReportOffset() const { return reportOffset_; }
		public:
			uint32_t GetReportValue(const uint8_t* report, uint16_t len) const;
		public:
			void Print(int indentLevel = 0) const;
		};
	private:
		GlobalItem globalItem_;
		LocalItem localItem_;
	public:
		ReportDescriptor();
		GenericHID* Parse(const uint8_t* descReport, uint16_t descLen);
	public:
		static const char* GetItemTypeName(uint8_t itemType);
	};
	class EventHandler {
	public:
		virtual void OnMount(uint8_t devAddr) {}
		virtual void OnUmount(uint8_t devAddr) {}
	};
	class HID {
	private:
		bool deletableFlag_;
		HID* pHIDNext_;		
	public:
		HID(bool deletableFlag);
		virtual ~HID() {}
	public:
		bool IsDeletable() const { return deletableFlag_; }
	public:
		void AppendList(HID* pHID);
		static void DeleteList(HID* pHID);
		HID* GetListNext() { return pHIDNext_; }
		HID* GetListLast();
	public:
		virtual bool IsKeyboard() const { return false; }
		virtual bool IsMouse() const { return false; }
		virtual bool IsGenericHID(uint32_t usage) const { return false; }
		virtual void OnReport(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len) = 0;
	};
	class Keyboard : public HID, public KeyboardRepeatable {
	public:
		struct UsageIdToKeyCode {
			uint8_t keyCodeUS;
			uint8_t keyCodeNonUS;
		};
		struct ReportCaptured {
			uint8_t modifier;
			uint8_t keyCodeTbl[6 + 8];
		};
	private:
		bool capsLockAsCtrlFlag_;
		ReportCaptured reportCaptured_;
	public:
		static Keyboard None;
	public:
		static const UsageIdToKeyCode usageIdToKeyCodeTbl[256];
	public:
		Keyboard(bool deletableFlag);
	public:
		virtual bool IsKeyboard() const override { return true; }
		virtual void OnReport(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len) override;
	public:
		// virtual function of jxglib::Keyboard
		virtual jxglib::Keyboard& SetCapsLockAsCtrl(bool capsLockAsCtrlFlag = true) override;
		virtual uint8_t GetModifier() override { return reportCaptured_.modifier; }
		virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1, bool includeModifiers = false) override;
	};
	class Mouse : public HID, public jxglib::Mouse {
	private:
		float sensibility_;
		Rect rcStage_;
		Rect rcStageRaw_;
		int xRaw_;
		int yRaw_;
	public:
		static Mouse None;
	public:
		Mouse(bool deletableFlag);
	public:
		void UpdateStage();
		Point CalcPoint() const;
	public:
		virtual bool IsMouse() const override { return true; }
		virtual void OnReport(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len) override;
	public:
		// virtual function of jxglib::Mouse
		virtual jxglib::Mouse& SetSensibility(float sensibility) override;
		virtual jxglib::Mouse& SetStage(const Rect& rcStage) override;
	};
	class GenericHID : public HID {
	private:
		uint32_t usage_;
		uint8_t reportCaptured_[32];
		uint16_t lenCaptured_;
		int nGlobalItem_;
		ReportDescriptor::GlobalItem globalItemTbl_[32];
		std::unique_ptr<ReportDescriptor::UsageInfo> pUsageInfoTop_;
	public:
		static GenericHID None;
	public:
		GenericHID(bool deletableFlag, uint32_t usage);
	public:
		uint32_t GetReportValue(uint32_t usage) const;
		uint32_t GetReportValue(uint16_t usagePage, uint16_t usageId) const {
			return GetReportValue(Usage(usagePage, usageId));
		}
	public:
		const ReportDescriptor::UsageInfo& FindUsageInfo(uint32_t usage) const;
		const ReportDescriptor::UsageInfo& FindUsageInfo(uint16_t usagePage, uint16_t usageId) const {
			return FindUsageInfo(Usage(usagePage, usageId));
		}
	public:
		virtual bool IsGenericHID(uint32_t usage) const override { return usage == usage_; }
		virtual void OnReport(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len) override;
	public:
		void OnMainItem(const USBHost::ReportDescriptor::GlobalItem& globalItem, const USBHost::ReportDescriptor::LocalItem& localItem, uint32_t& reportOffset);
		void OnCollection(ReportDescriptor::CollectionType collectionType, uint32_t usage);
		void OnEndCollection();
	public:
		void PrintUsage(int indentLevel = 0) const;
	};
	class GamePad {
	private:
		GenericHID& genericHID_;
	public:
		GamePad(GenericHID& genericHID) : genericHID_{genericHID} {}
	public:
		const uint32_t Get_ButtonX() const		{ return GetReportValue(0x0009, 0x0001); }
		const uint32_t Get_ButtonY() const		{ return GetReportValue(0x0009, 0x0002); }
		const uint32_t Get_ButtonA() const		{ return GetReportValue(0x0009, 0x0003); }
		const uint32_t Get_ButtonB() const		{ return GetReportValue(0x0009, 0x0004); }
		const uint32_t Get_ButtonLB() const		{ return GetReportValue(0x0009, 0x0005); }
		const uint32_t Get_ButtonRB() const		{ return GetReportValue(0x0009, 0x0006); }
		const uint32_t Get_ButtonLT() const		{ return GetReportValue(0x0009, 0x0007); }
		const uint32_t Get_ButtonRT() const		{ return GetReportValue(0x0009, 0x0008); }
		const uint32_t Get_ButtonLStick() const	{ return GetReportValue(0x0009, 0x0009); }
		const uint32_t Get_ButtonRStick() const	{ return GetReportValue(0x0009, 0x000a); }
		const uint32_t Get_ButtonBACK() const	{ return GetReportValue(0x0009, 0x000b); }
		const uint32_t Get_ButtonSTART() const	{ return GetReportValue(0x0009, 0x000c); }
		const uint32_t Get_ButtonGUIDE() const	{ return GetReportValue(0x0009, 0x000d); }
		const uint32_t Get_HatSwitch() const	{ return GetReportValue(0x0001, 0x0039); }
		const uint32_t Get_LStickHorz() const	{ return GetReportValue(0x0001, 0x0030); }
		const uint32_t Get_LStickVert() const	{ return GetReportValue(0x0001, 0x0031); }
		const uint32_t Get_RStickHorz() const	{ return GetReportValue(0x0001, 0x0035); }
		const uint32_t Get_RStickVert() const	{ return GetReportValue(0x0001, 0x0032); }
	public:
		uint32_t GetReportValue(uint16_t usagePage, uint16_t usageId) const {
			return genericHID_.GetReportValue(usagePage, usageId);
		}
	};
public:
	static USBHost Instance;
public:
	ReportDescriptor reportDescriptor;
private:
	Keyboard keyboard_;
	Mouse mouse_;
	HID* hidTbl_[CFG_TUH_HID];
	EventHandler* pEventHandler_;
public:
	USBHost();
public:
	static void Initialize(uint8_t rhport = BOARD_TUH_RHPORT, EventHandler* pEventHandler = nullptr);
public:
	void SetKeyboard(uint8_t iInstance);
	void SetMouse(uint8_t iInstance);
	void SetHID(uint8_t iInstance, HID* pHID) { hidTbl_[iInstance] = pHID; }
	HID* GetHID(uint8_t iInstance) { return hidTbl_[iInstance]; }
	void DeleteHID(uint8_t iInstance);
public:
	static Keyboard& GetKeyboard() { return Instance.keyboard_; }
	static Mouse& GetMouse() { return Instance.mouse_; }
	static Keyboard& FindKeyboard(int idx = 0);
	static Mouse& FindMouse(int idx = 0);
	static GenericHID& FindGenericHID(uint32_t usage, int idx = 0);
	static EventHandler* GetEventHandler() { return Instance.pEventHandler_; }
	constexpr static uint32_t Usage(uint16_t usagePage, uint16_t usageID) { return (static_cast<uint32_t>(usagePage) << 16) + usageID; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "USBHost"; }
	virtual void OnTick() override;
};

}

#endif
