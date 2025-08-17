//==============================================================================
// ProtocolAnalyzer.cpp
//==============================================================================
#include "jxglib/LogicAnalyzer.h"
#include "jxglib/BinaryInfo.h"

namespace jxglib { 

//------------------------------------------------------------------------------
// ProtocolAnalyzer
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// ProtocolAnalyzer::Factory
//------------------------------------------------------------------------------
ProtocolAnalyzer::Factory* ProtocolAnalyzer::Factory::pFactoryTop_ = nullptr;

ProtocolAnalyzer::Factory::Factory(const char* name) : name_{name}
{
	pFactoryNext_ = pFactoryTop_;
	pFactoryTop_ = this;
}

ProtocolAnalyzer::Factory* ProtocolAnalyzer::Factory::Find(const char* name)
{
	for (Factory* pFactory = pFactoryTop_; pFactory; pFactory = pFactory->GetNext()) {
		if (::strcasecmp(pFactory->GetName(), name) == 0) return pFactory;
	}
	return nullptr;
}

}
