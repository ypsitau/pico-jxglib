//==============================================================================
// ProtocolDecoder.cpp
//==============================================================================
#include "jxglib/LogicAnalyzer.h"
#include "jxglib/BinaryInfo.h"

namespace jxglib { 

//------------------------------------------------------------------------------
// ProtocolDecoder
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ProtocolDecoder::Factory
//------------------------------------------------------------------------------
ProtocolDecoder::Factory* ProtocolDecoder::Factory::pFactoryHead_ = nullptr;

ProtocolDecoder::Factory::Factory(const char* name) : name_{name}
{
	Factory* pFactoryPrev = nullptr;
	for (Factory* pFactory = pFactoryHead_; pFactory; pFactory = pFactory->GetNext()) {
		if (::strcmp(name, pFactory->GetName()) <= 0) break;
		pFactoryPrev = pFactory;
	}
	if (pFactoryPrev) {
		SetNext(pFactoryPrev->GetNext());
		pFactoryPrev->SetNext(this);
	} else {
		SetNext(pFactoryHead_);
		pFactoryHead_ = this;
	}
}

ProtocolDecoder::Factory* ProtocolDecoder::Factory::Find(const char* name)
{
	for (Factory* pFactory = pFactoryHead_; pFactory; pFactory = pFactory->GetNext()) {
		if (::strcasecmp(pFactory->GetName(), name) == 0) return pFactory;
	}
	return nullptr;
}

}
