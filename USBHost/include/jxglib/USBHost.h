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
	class HIDDriver;
	class EventHandler {
	public:
		virtual void OnMount(uint8_t devAddr) {}
		virtual void OnUmount(uint8_t devAddr) {}
	};
	class HID : public Referable {
	public:
		struct Report {
			const uint8_t* report;
			uint16_t len;
		};
		struct ItemType {
			static const uint8_t None				= 0x00;
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
			uint32_t itemData;
		public:
			bool IsData() const				{ return  !(itemData & (1 << 0)); }
			bool IsConstant() const			{ return !!(itemData & (1 << 0)); }
			bool IsArray() const			{ return  !(itemData & (1 << 1)); }
			bool IsVariable() const			{ return !!(itemData & (1 << 1)); }
			bool IsAbsolute() const			{ return  !(itemData & (1 << 2)); }
			bool IsRelative() const			{ return !!(itemData & (1 << 2)); }
			bool IsNoWrap() const			{ return  !(itemData & (1 << 3)); }
			bool IsWrap() const				{ return !!(itemData & (1 << 3)); }
			bool IsLinear() const			{ return  !(itemData & (1 << 4)); }
			bool IsNonLinear() const		{ return !!(itemData & (1 << 4)); }
			bool IsPreferredState() const	{ return  !(itemData & (1 << 5)); }
			bool IsNoPreferred() const		{ return !!(itemData & (1 << 5)); }
			bool IsNoNullPosition() const	{ return  !(itemData & (1 << 6)); }
			bool IsNullState() const		{ return !!(itemData & (1 << 6)); }
			bool IsNonVolatile() const		{ return  !(itemData & (1 << 7)); }	// for Output and Feature
			bool IsVolatile() const			{ return !!(itemData & (1 << 7)); }	// for Output and Feature
			bool IsBitField() const			{ return  !(itemData & (1 << 8)); }
			bool IsBufferedBytes() const	{ return !!(itemData & (1 << 8)); }
		};
		enum class CollectionType : uint8_t {
			Physical		= 0x00,
			Application		= 0x01,
			Logical			= 0x02,
			Report			= 0x03,
			NamedArray		= 0x04,
			UsageSwitch		= 0x05,
			UsageModifier	= 0x06,
			None			= 0xff,
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
			MainItemData mainItemData;
			int32_t logicalMinimum;
			int32_t logicalMaximum;
			int32_t physicalMinimum;
			int32_t physicalMaximum;
			int32_t unitExponent;
			int32_t unit;
			uint32_t reportSize;
			uint32_t reportCount;
		public:
			static const GlobalItem None;
		public:
			bool IsValid() const { return itemType != ItemType::None; }
			void Clear() { ::memset(this, 0x00, sizeof(GlobalItem)); }
			void Print(Printable& printable = Stdio::Instance, int indentLevel = 0) const;
		};
		class GlobalItemList {
		public:
			GlobalItem globalItem;
		private:
			std::unique_ptr<GlobalItemList> pGlobalItemListNext_;
		public:
			GlobalItemList() { globalItem.Clear(); }
			GlobalItemList(const GlobalItem& globalItem) : globalItem{globalItem} {}
		public:
			void AppendList(GlobalItemList* pGlobalItemList) { GetListLast()->pGlobalItemListNext_.reset(pGlobalItemList); }
			GlobalItemList* GetListNext() { return pGlobalItemListNext_.get(); }
			const GlobalItemList* GetListNext() const { return pGlobalItemListNext_.get(); }
			void RemoveListNext() { pGlobalItemListNext_.reset(); }
			GlobalItemList* GetListLast();
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
		class UsageAccessor {
		private:
			uint32_t usage_;
			const GlobalItem* pGlobalItem_;
			uint32_t reportOffset_;
			std::unique_ptr<UsageAccessor> pUsageAccessorNext_;
		public:
			static const UsageAccessor None;
		public:
			UsageAccessor() : usage_{0}, pGlobalItem_{&GlobalItem::None}, reportOffset_{0} {}
			constexpr UsageAccessor(uint32_t usage, const GlobalItem* pGlobalItem, uint32_t reportOffset) :
				usage_{usage}, pGlobalItem_{pGlobalItem}, reportOffset_{reportOffset} {}
		public:
			void AppendList(UsageAccessor* pUsageAccessor) { GetListLast()->pUsageAccessorNext_.reset(pUsageAccessor); }
			UsageAccessor* GetListNext() { return pUsageAccessorNext_.get(); }
			const UsageAccessor* GetListNext() const { return pUsageAccessorNext_.get(); }
			UsageAccessor* GetListLast();
		public:
			bool IsValid() const { return pGlobalItem_->IsValid(); }
			uint32_t GetUsage() const { return usage_; }
			uint16_t GetUsagePage() const { return static_cast<uint16_t>(usage_ >> 16); }
			uint16_t GetUsageId() const { return static_cast<uint16_t>(usage_ & 0xffff); }
			uint8_t GetItemType() const { return pGlobalItem_->itemType; }
			const MainItemData& GetMainItemData() const { return pGlobalItem_->mainItemData; }
			int32_t GetLogicalMinimum() const { return pGlobalItem_->logicalMinimum; }
			int32_t GetLogicalMaximum() const { return pGlobalItem_->logicalMaximum; }
			int32_t GetPhysicalMinimum() const { return pGlobalItem_->physicalMinimum; }
			int32_t GetPhysicalMaximum() const { return pGlobalItem_->physicalMaximum; }
			int32_t GetUnitExponent() const { return pGlobalItem_->unitExponent; }
			int32_t GetUnit() const { return pGlobalItem_->unit; }
			uint32_t GetReportSize() const { return pGlobalItem_->reportSize; }
			uint32_t GetReportCount() const { return pGlobalItem_->reportCount; }
			uint32_t GetReportOffset() const { return reportOffset_; }
			bool IsAbsolute() const { return GetMainItemData().IsAbsolute(); }
			bool IsRelative() const { return GetMainItemData().IsRelative(); }
		public:
			int32_t GetVariable(const uint8_t* report, uint16_t len, int idx = 0) const;
			int32_t GetVariableWithDefault(const uint8_t* report, uint16_t len, int32_t valueDefault, int idx = 0) const {
				return IsValid()? GetVariable(report, len, idx) : valueDefault;
			}
			int32_t GetVariable(const Report& report, int idx = 0) const {
				return GetVariable(report.report, report.len, idx);
			}
			int32_t GetVariableWithDefault(const Report& report, int32_t valueDefault, int idx = 0) const {
				return GetVariableWithDefault(report.report, report.len, valueDefault, idx);
			}
		public:
			void Print(Printable& printable = Stdio::Instance, int indentLevel = 0) const;
		};
		class Collection {
		private:
			CollectionType collectionType_;
			uint32_t usage_;
			Collection* pCollectionParent_;
			std::unique_ptr<UsageAccessor> pUsageAccessorArray_;
			std::unique_ptr<UsageAccessor> pUsageAccessorTop_;
			std::unique_ptr<Collection> pCollectionChildTop_;
			std::unique_ptr<Collection> pCollectionNext_;
		public:
			static const Collection None;
		public:
			Collection(CollectionType collectionType, uint32_t usage, Collection* pCollectionParent) :
				collectionType_{collectionType}, usage_{usage}, pCollectionParent_{pCollectionParent} {}
		public:
			CollectionType GetCollectionType() const { return collectionType_; }
			uint32_t GetUsage() const { return usage_; }
			uint16_t GetUsagePage() const { return static_cast<uint16_t>(usage_ >> 16); }
			uint16_t GetUsageId() const { return static_cast<uint16_t>(usage_ & 0xffff); }
			Collection* GetCollectionParent() { return pCollectionParent_; }
			Collection* GetCollectionChildTop() { return pCollectionChildTop_.get(); }
			const Collection* GetCollectionChildTop() const { return pCollectionChildTop_.get(); }
			UsageAccessor* GetUsageAccessorTop() { return pUsageAccessorTop_.get(); }
		public:
			void AppendList(Collection* pCollection) { GetListLast()->pCollectionNext_.reset(pCollection); }
			Collection* GetListNext() { return pCollectionNext_.get(); }
			const Collection* GetListNext() const { return pCollectionNext_.get(); }
			Collection* GetListLast();
		public:
			int32_t GetArrayItem(const uint8_t* report, uint16_t len, int idx) const;
			uint32_t GetArraySize() const { return pUsageAccessorArray_? pUsageAccessorArray_->GetReportSize() : 0; }
		public:
			void AppendUsageAccessor(UsageAccessor* pUsageAccessor);
			void AppendCollectionChild(Collection* pCollection);
		public:
			void AddMainItem(GlobalItem* pGlobalItem, const LocalItem& localItem, uint32_t& reportOffset);
		public:
			const UsageAccessor& FindUsageAccessor(uint32_t usage) const;
			const Collection& FindCollection(uint32_t usage) const;
		public:
			void PrintUsage(Printable& printable, int indentLevel = 0) const;
		};
		class Application {
		private:
			uint8_t reportID_;
			std::unique_ptr<GlobalItemList> pGlobalItemListTop_;
			Collection collection_;
			std::unique_ptr<Application> pApplicationNext_;
		public:
			Application(uint32_t usage);
		public:
			void AppendList(Application* pApplication) { GetListLast()->pApplicationNext_.reset(pApplication); }
			Application* GetListNext() { return pApplicationNext_.get(); }
			const Application* GetListNext() const { return pApplicationNext_.get(); }
			Application* GetListLast();
		public:
			void SetReportID(uint8_t reportID) { reportID_ = reportID; }
			uint8_t GetReportID() const { return reportID_; }
			uint32_t GetUsage() const { return collection_.GetUsage(); }
		public:
			Collection& GetCollection() { return collection_; }
			const Collection& GetCollection() const { return collection_; }
		public:
			void AddMainItem(Collection& collection, const GlobalItem& globalItem, const LocalItem& localItem, uint32_t& reportOffset);
		public:
			int32_t GetVariable(const uint8_t* report, uint16_t len, uint32_t usage) const;
			int32_t GetVariable(const uint8_t* report, uint16_t len, uint32_t usageCollection, uint32_t usage) const;
			int32_t GetVariable(const uint8_t* report, uint16_t len, uint32_t usageCollection1, uint32_t usageCollection2, uint32_t usage) const;
			int32_t GetVariableWithDefault(const uint8_t* report, uint16_t len, uint32_t usage, int32_t valueDefault) const;
			int32_t GetVariableWithDefault(const uint8_t* report, uint16_t len, uint32_t usageCollection, uint32_t usage, int32_t valueDefault) const;
			int32_t GetVariableWithDefault(const uint8_t* report, uint16_t len, uint32_t usageCollection1, uint32_t usageCollection2, uint32_t usage, int32_t valueDefault) const;
			int32_t GetArrayItem(const uint8_t* report, uint16_t len, int idx) const;
			int32_t GetArrayItem(const uint8_t* report, uint16_t len, uint32_t usageCollection, int idx) const;
			int32_t GetArrayItem(const uint8_t* report, uint16_t len, uint32_t usageCollection1, uint32_t usageCollection2, int idx) const;
		public:
			void Print(Printable& printable = Stdio::Instance, int indentLevel = 0) const;
			void PrintAll(Printable& printable = Stdio::Instance, int indentLevel = 0) const;
		};
	private:
		uint8_t devAddr_;
		uint8_t iInstance_;
		std::unique_ptr<Application> pApplication_;
		uint8_t report_[CFG_TUH_HID_EPIN_BUFSIZE];
		uint16_t reportLen_;
		HIDDriver* pHIDDriver_;
	public:
		DeclareReferable(HID)
	public:
		HID(uint8_t devAddr, uint8_t iInstance, Application* pApplication);
	protected:
		~HID() {}
	public:
		virtual bool IsKeyboard() const { return false; }
	public:
		void AttachDriver(HIDDriver& hidDriver) { pHIDDriver_ = &hidDriver; }
		HIDDriver* GetHIDDriver() { return pHIDDriver_; }
	public:
		const uint8_t* GetReport() const { return report_; }
		uint16_t GetReportLen() const { return reportLen_; }
	public:
		bool IsSendReady() { return ::tuh_hid_send_ready(devAddr_, iInstance_); }
		void SendReport(uint8_t reportId, const uint8_t* report, uint16_t len) {
			::tuh_hid_send_report(devAddr_, iInstance_, reportId, report, len);
		}
	public:
		virtual void OnReport(const uint8_t* report, uint16_t len);
	public:
		static Application* ParseReportDescriptor(const uint8_t* descReport, uint16_t descLen);
		static const char* GetCollectionTypeName(CollectionType collectionType);
		static const char* GetItemTypeName(uint8_t itemType);
	};
	class HIDDriver {
	private:
		uint32_t usage_;
		RefPtr<HID> pHID_;
		HID::Application* pApplication_;
		std::unique_ptr<HIDDriver> pHIDDriverRegisteredNext_;
	public:
		static HIDDriver* pHIDDriverRegisteredTop;
	public:
		HIDDriver(uint32_t usage);
		virtual ~HIDDriver();
	public:
		void AppendRegisteredList(HIDDriver* pHIDDriver) { GetRegisteredListLast()->pHIDDriverRegisteredNext_.reset(pHIDDriver); }
		HIDDriver* GetRegisteredListNext() { return pHIDDriverRegisteredNext_.get(); }
		HIDDriver* ReleaseRegisteredListNext() { return pHIDDriverRegisteredNext_.release(); }
		void SetRegisteredListNext(HIDDriver* pHIDDriver) { pHIDDriverRegisteredNext_.reset(pHIDDriver); }
		const HIDDriver* GetRegisteredListNext() const { return pHIDDriverRegisteredNext_.get(); }
		HIDDriver* GetRegisteredListLast();
	public:
		uint32_t GetUsage() const { return usage_; }
		void AttachHID(HID* pHID, HID::Application* pApplication) { pHID_.reset(pHID); pApplication_ = pApplication; }
		void DetachHID() { pHID_.reset(), pApplication_ = nullptr; }
		bool IsMounted() const { return !!pHID_ && !!pApplication_; }
		HID& GetHID() { return *pHID_; }
		const HID& GetHID() const { return *pHID_; }
		const HID::Application& GetApplication() const { return *pApplication_; }
	public:
		virtual void OnMount() {};
		virtual void OnUmount() {}
		virtual void OnReport(const uint8_t* report, uint16_t len) {};
	public:
		uint8_t GetReportID() const { return pApplication_? pApplication_->GetReportID() : 0; }
	public:
		int32_t GetVariable(uint32_t usage) const {
			return pApplication_? pApplication_->GetVariable(pHID_->GetReport(), pHID_->GetReportLen(), usage) : 0;
		}
		int32_t GetVariable(uint32_t usageCollection, uint32_t usage) const {
			return pApplication_? pApplication_->GetVariable(
				pHID_->GetReport(), pHID_->GetReportLen(), usageCollection, usage) : 0;
		}
		int32_t GetVariable(uint32_t usageCollection1, uint32_t usageCollection2, uint32_t usage) const {
			return pApplication_? pApplication_->GetVariable(
				pHID_->GetReport(), pHID_->GetReportLen(), usageCollection1, usageCollection2, usage) : 0;
		}
		int32_t GetVariableWithDefault(uint32_t usage, int32_t valueDefault) const {
			return pApplication_? pApplication_->GetVariableWithDefault(
				pHID_->GetReport(), pHID_->GetReportLen(), usage, valueDefault) : valueDefault;
		}
		int32_t GetVariableWithDefault(uint32_t usageCollection, uint32_t usage, int32_t valueDefault) const {
			return pApplication_? pApplication_->GetVariableWithDefault(
				pHID_->GetReport(), pHID_->GetReportLen(), usageCollection, usage, valueDefault) : valueDefault;
		}
		int32_t GetVariableWithDefault(uint32_t usageCollection1, uint32_t usageCollection2, uint32_t usage, int32_t valueDefault) const {
			return pApplication_? pApplication_->GetVariableWithDefault(
				pHID_->GetReport(), pHID_->GetReportLen(), usageCollection1, usageCollection2, usage, valueDefault) : valueDefault;
		}
	public:
		int32_t GetArrayItem(int idx) const {
			return pApplication_? pApplication_->GetArrayItem(pHID_->GetReport(), pHID_->GetReportLen(), idx) : 0;
		}
		int32_t GetArrayItem(uint32_t usageCollection, int idx) const {
			return pApplication_? pApplication_->GetArrayItem(pHID_->GetReport(),
				pHID_->GetReportLen(), usageCollection, idx) : 0;
		}
		int32_t GetArrayItem(uint32_t usageCollection1, uint32_t usageCollection2, int idx) const {
			return pApplication_? pApplication_->GetArrayItem(
				pHID_->GetReport(), pHID_->GetReportLen(), usageCollection1, usageCollection2, idx) : 0;
		}
	};
	class Keyboard : public KeyboardRepeatable, public HIDDriver {
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
		static const UsageIdToKeyCode usageIdToKeyCodeTbl[256];
	public:
		Keyboard();
	public:
		// virtual function of HIDDriver
		virtual void OnReport(const uint8_t* report, uint16_t len) override;
	public:
		// virtual function of jxglib::Keyboard
		virtual jxglib::Keyboard& SetCapsLockAsCtrl(bool capsLockAsCtrlFlag = true) override;
		virtual uint8_t GetModifier() override { return reportCaptured_.modifier; }
		virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1, bool includeModifiers = false) override;
	};
	class Mouse : public jxglib::Mouse, public HIDDriver {
	private:
		float sensibility_;
		Rect rcStage_;
		Rect rcStageRaw_;
		int xRaw_;
		int yRaw_;
	public:
		Mouse();
	public:
		void UpdateStage();
		Point CalcPoint() const;
	public:
		// virtual function of HIDDriver
		virtual void OnReport(const uint8_t* report, uint16_t len) override;
	public:
		// virtual function of jxglib::Mouse
		virtual jxglib::Mouse& SetSensibility(float sensibility) override;
		virtual jxglib::Mouse& SetStage(const Rect& rcStage) override;
	};
	class GamePad : public HIDDriver {
	public:
		GamePad();
	public:
		uint32_t Get_Button1() const		{ return GetVariable(0x0009'0001); }
		uint32_t Get_Button2() const		{ return GetVariable(0x0009'0002); }
		uint32_t Get_Button3() const		{ return GetVariable(0x0009'0003); }
		uint32_t Get_Button4() const		{ return GetVariable(0x0009'0004); }
		uint32_t Get_Button5() const		{ return GetVariable(0x0009'0005); }
		uint32_t Get_Button6() const		{ return GetVariable(0x0009'0006); }
		uint32_t Get_Button7() const		{ return GetVariable(0x0009'0007); }
		uint32_t Get_Button8() const		{ return GetVariable(0x0009'0008); }
		uint32_t Get_Button9() const		{ return GetVariable(0x0009'0009); }
		uint32_t Get_Button10() const		{ return GetVariable(0x0009'000a); }
		uint32_t Get_Button11() const		{ return GetVariable(0x0009'000b); }
		uint32_t Get_Button12() const		{ return GetVariable(0x0009'000c); }
		uint32_t Get_Button13() const		{ return GetVariable(0x0009'000d); }
		uint32_t Get_HatSwitch() const		{ return GetVariableWithDefault(0x0001'0039, 0xf); }
		uint32_t GetRaw_LStickHorz() const	{ return GetVariable(0x0001'0030); }
		uint32_t GetRaw_LStickVert() const	{ return GetVariable(0x0001'0031); }
		uint32_t GetRaw_RStickHorz() const	{ return GetVariable(0x0001'0035); }
		uint32_t GetRaw_RStickVert() const	{ return GetVariable(0x0001'0032); }
		float Get_LStickHorz() const		{ return GetAxis(0x0001'0030); }
		float Get_LStickVert() const		{ return GetAxis(0x0001'0031); }
		float Get_RStickHorz() const		{ return GetAxis(0x0001'0035); }
		float Get_RStickVert() const		{ return GetAxis(0x0001'0032); }
	public:
		float GetAxis(uint32_t usage) const;
	};
public:
	static USBHost Instance;
private:
	HID* hidTbl_[CFG_TUH_HID];
	EventHandler* pEventHandler_;
public:
	USBHost();
public:
	static void Initialize(uint8_t rhport = BOARD_TUH_RHPORT, EventHandler* pEventHandler = nullptr);
public:
	void MountHID(uint8_t devAddr, uint8_t iInstance, const uint8_t* descReport, uint16_t descLen);
	void UmountHID(uint8_t iInstance);
	HID* LookupHID(uint8_t iInstance) { return hidTbl_[iInstance]; }
public:
	static EventHandler* GetEventHandler() { return Instance.pEventHandler_; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "USBHost"; }
	virtual void OnTick() override;
};

}

#endif
