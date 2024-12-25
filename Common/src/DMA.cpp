//==============================================================================
// DMA.cpp
//==============================================================================
#include "jxglib/DMA.h"

//------------------------------------------------------------------------------
// DMA
//------------------------------------------------------------------------------
namespace jxglib {

DMA::Channel_T<0> DMA::Channel0;
DMA::Channel_T<1> DMA::Channel1;
DMA::Channel_T<2> DMA::Channel2;
DMA::Channel_T<3> DMA::Channel3;
DMA::Channel_T<4> DMA::Channel4;
DMA::Channel_T<5> DMA::Channel5;
DMA::Channel_T<6> DMA::Channel6;
DMA::Channel_T<7> DMA::Channel7;
DMA::Channel_T<8> DMA::Channel8;
DMA::Channel_T<9> DMA::Channel9;
DMA::Channel_T<10> DMA::Channel10;
DMA::Channel_T<11> DMA::Channel11;

DMA::Channel* DMA::ChannelTbl[] = {
	nullptr,
	&Channel0, &Channel1, &Channel2, &Channel3, &Channel4, &Channel5,
	&Channel6, &Channel7, &Channel8, &Channel9, &Channel10, &Channel11,
};

const DMA::Timer DMA::Timer0(0);
const DMA::Timer DMA::Timer1(1);
const DMA::Timer DMA::Timer2(2);
const DMA::Timer DMA::Timer3(3);

const DMA::Timer* DMA::TimerTbl[] = { nullptr, &Timer0, &Timer1, &Timer2, &Timer3, };

}
