//==============================================================================
// DMA.cpp
//==============================================================================
#include "jxglib/DMA.h"

//------------------------------------------------------------------------------
// DMA
//------------------------------------------------------------------------------
namespace jxglib {

DMA::Channel DMA::Channel0(0);
DMA::Channel DMA::Channel1(1);
DMA::Channel DMA::Channel2(2);
DMA::Channel DMA::Channel3(3);
DMA::Channel DMA::Channel4(4);
DMA::Channel DMA::Channel5(5);
DMA::Channel DMA::Channel6(6);
DMA::Channel DMA::Channel7(7);
DMA::Channel DMA::Channel8(8);
DMA::Channel DMA::Channel9(9);
DMA::Channel DMA::Channel10(10);
DMA::Channel DMA::Channel11(11);

const DMA::Timer DMA::Timer0(0);
const DMA::Timer DMA::Timer1(1);
const DMA::Timer DMA::Timer2(2);
const DMA::Timer DMA::Timer3(3);

const DMA::IRQ_n DMA::IRQ_0(0);
const DMA::IRQ_n DMA::IRQ_1(1);

DMA::Channel* DMA::channelTbl_[] = {
	&Channel0, &Channel1, &Channel2, &Channel3, &Channel4, &Channel5,
	&Channel6, &Channel7, &Channel8, &Channel9, &Channel10, &Channel11,
};

DMA::Channel& DMA::claim_unused_channel(bool required)
{
	return *channelTbl_[::dma_claim_unused_channel(required)];
}

}
