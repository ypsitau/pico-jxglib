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
			uint32_t reportID;
			uint32_t reportCount;
		public:
			static const GlobalItem None;
		public:
			bool IsValid() const { return itemType != ItemType::None; }
			void Clear() { ::memset(this, 0x00, sizeof(GlobalItem)); }
			void Print(Printable& printable, int indentLevel = 0) const;
		};
		class GlobalItemList {
		public:
			GlobalItem globalItem;
		private:
			std::unique_ptr<GlobalItemList> pGlobalItemListNext_;
		public:
			GlobalItemList(const GlobalItem& globalItem) : globalItem{globalItem} {}
		public:
			void AppendList(GlobalItemList* pGlobalItemList) { GetListLast()->pGlobalItemListNext_.reset(pGlobalItemList); }
			GlobalItemList* GetListNext() { return pGlobalItemListNext_.get(); }
			const GlobalItemList* GetListNext() const { return pGlobalItemListNext_.get(); }
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
		class UsageInfo {
		private:
			uint32_t usage_;
			const GlobalItem* pGlobalItem_;
			uint32_t reportOffset_;
			std::unique_ptr<UsageInfo> pUsageInfoNext_;
		public:
			static const UsageInfo None;
		public:
			UsageInfo() : usage_{0}, pGlobalItem_{&GlobalItem::None}, reportOffset_{0} {}
			constexpr UsageInfo(uint32_t usage, const GlobalItem* pGlobalItem, uint32_t reportOffset) :
				usage_{usage}, pGlobalItem_{pGlobalItem}, reportOffset_{reportOffset} {}
		public:
			void AppendList(UsageInfo* pUsageInfo) { GetListLast()->pUsageInfoNext_.reset(pUsageInfo); }
			UsageInfo* GetListNext() { return pUsageInfoNext_.get(); }
			const UsageInfo* GetListNext() const { return pUsageInfoNext_.get(); }
			UsageInfo* GetListLast();
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
			uint32_t GetReportID() const { return pGlobalItem_->reportID; }
			uint32_t GetReportCount() const { return pGlobalItem_->reportCount; }
			uint32_t GetReportOffset() const { return reportOffset_; }
			bool IsAbsolute() const { return GetMainItemData().IsAbsolute(); }
			bool IsRelative() const { return GetMainItemData().IsRelative(); }
		public:
			int32_t GetVariable(const uint8_t* report, uint16_t len, int idx = 0) const;
		public:
			void Print(Printable& printable, int indentLevel = 0) const;
		};
		class Collection {
		private:
			CollectionType collectionType_;
			uint32_t usage_;
			Collection* pCollectionParent_;
			std::unique_ptr<UsageInfo> pUsageInfoArray_;
			std::unique_ptr<UsageInfo> pUsageInfoTop_;
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
			UsageInfo* GetUsageInfoTop() { return pUsageInfoTop_.get(); }
		public:
			void AppendList(Collection* pCollection) { GetListLast()->pCollectionNext_.reset(pCollection); }
			Collection* GetListNext() { return pCollectionNext_.get(); }
			const Collection* GetListNext() const { return pCollectionNext_.get(); }
			Collection* GetListLast();
		public:
			int32_t GetArrayItem(const uint8_t* report, uint16_t len, int idx) const;
			uint32_t GetArraySize() const { return pUsageInfoArray_? pUsageInfoArray_->GetReportSize() : 0; }
		public:
			void AppendUsageInfo(UsageInfo* pUsageInfo);
			void AppendCollectionChild(Collection* pCollection);
		public:
			void AddMainItem(GlobalItem* pGlobalItem, const LocalItem& localItem, uint32_t& reportOffset);
		public:
			const UsageInfo& FindUsageInfo(uint32_t usage) const;
			const Collection& FindCollection(uint32_t usage) const;
		public:
			void PrintUsage(Printable& printable, int indentLevel = 0) const;
		};
		class Application : public Referable {
		private:
			std::unique_ptr<GlobalItemList> pGlobalItemListTop_;
			Collection collection_;
			RefPtr<Application> pApplicationNext_;
		public:
			DeclareReferable(Application)
		public:
			Application(uint32_t usage);
		protected:
			~Application() {}
		public:
			void AppendList(Application* pApplication) { GetListLast()->pApplicationNext_.reset(pApplication); }
			Application* GetListNext() { return pApplicationNext_.get(); }
			const Application* GetListNext() const { return pApplicationNext_.get(); }
			Application* GetListLast();
		public:
			uint32_t GetUsage() const { return collection_.GetUsage(); }
		public:
			Collection& GetCollection() { return collection_; }
			const Collection& GetCollection() const { return collection_; }
		public:
			void AddMainItem(Collection& collection, const GlobalItem& globalItem, const LocalItem& localItem, uint32_t& reportOffset);
		public:
			void Print(Printable& printable, int indentLevel = 0) const;
			void PrintAll(Printable& printable, int indentLevel = 0) const;
		};
	private:
		GlobalItem globalItem_;
		LocalItem localItem_;
	public:
		ReportDescriptor();
		Application* Parse(const uint8_t* descReport, uint16_t descLen);
	public:
		static const char* GetCollectionTypeName(CollectionType collectionType);
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
		RefPtr<ReportDescriptor::Application> pApplication_;
		uint8_t reportCaptured_[CFG_TUH_HID_EPIN_BUFSIZE];
		uint16_t lenCaptured_;
	public:
		static GenericHID None;
	public:
		GenericHID(ReportDescriptor::Application* pApplication, bool deletableFlag);
	public:
		void AttachApplication(ReportDescriptor::Application* pApplication) { pApplication_.reset(pApplication); }
	public:
		int32_t GetVariable(uint32_t usage) const;
		int32_t GetVariable(uint32_t usage1, uint32_t usage2) const;
		int32_t GetVariable(uint32_t usage1, uint32_t usage2, uint32_t usage3) const;
	public:
		int32_t GetArrayItem(int idx) const;
		int32_t GetArrayItem(uint32_t usage, int idx) const;
		int32_t GetArrayItem(uint32_t usage1, uint32_t usage2, int idx) const;
	public:
		virtual bool IsGenericHID(uint32_t usage) const override {
			return pApplication_? pApplication_->GetUsage() == usage : false;
		}
		virtual void OnReport(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len) override;
	};
	class GamePad {
	private:
		GenericHID& genericHID_;
	public:
		GamePad(GenericHID& genericHID) : genericHID_{genericHID} {}
	public:
		const uint32_t Get_ButtonX() const		{ return GetVariable(0x0009'0001); }
		const uint32_t Get_ButtonY() const		{ return GetVariable(0x0009'0002); }
		const uint32_t Get_ButtonA() const		{ return GetVariable(0x0009'0003); }
		const uint32_t Get_ButtonB() const		{ return GetVariable(0x0009'0004); }
		const uint32_t Get_ButtonLB() const		{ return GetVariable(0x0009'0005); }
		const uint32_t Get_ButtonRB() const		{ return GetVariable(0x0009'0006); }
		const uint32_t Get_ButtonLT() const		{ return GetVariable(0x0009'0007); }
		const uint32_t Get_ButtonRT() const		{ return GetVariable(0x0009'0008); }
		const uint32_t Get_ButtonLStick() const	{ return GetVariable(0x0009'0009); }
		const uint32_t Get_ButtonRStick() const	{ return GetVariable(0x0009'000a); }
		const uint32_t Get_ButtonBACK() const	{ return GetVariable(0x0009'000b); }
		const uint32_t Get_ButtonSTART() const	{ return GetVariable(0x0009'000c); }
		const uint32_t Get_ButtonGUIDE() const	{ return GetVariable(0x0009'000d); }
		const uint32_t Get_HatSwitch() const	{ return GetVariable(0x0001'0039); }
		const uint32_t Get_LStickHorz() const	{ return GetVariable(0x0001'0030); }
		const uint32_t Get_LStickVert() const	{ return GetVariable(0x0001'0031); }
		const uint32_t Get_RStickHorz() const	{ return GetVariable(0x0001'0035); }
		const uint32_t Get_RStickVert() const	{ return GetVariable(0x0001'0032); }
	public:
		uint32_t GetVariable(uint32_t usage) const {
			return genericHID_.GetVariable(usage);
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
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "USBHost"; }
	virtual void OnTick() override;
};

}

#endif
