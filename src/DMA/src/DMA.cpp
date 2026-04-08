//==============================================================================
// DMA.cpp
//==============================================================================
#include "jxglib/DMA.h"

namespace jxglib::DMA {

//------------------------------------------------------------------------------
// DMA::Channel
//------------------------------------------------------------------------------
Channel_T<0> Channel0;
Channel_T<1> Channel1;
Channel_T<2> Channel2;
Channel_T<3> Channel3;
Channel_T<4> Channel4;
Channel_T<5> Channel5;
Channel_T<6> Channel6;
Channel_T<7> Channel7;
Channel_T<8> Channel8;
Channel_T<9> Channel9;
Channel_T<10> Channel10;
Channel_T<11> Channel11;

Channel* ChannelTbl[] = {
	nullptr,
	&Channel0, &Channel1, &Channel2, &Channel3, &Channel4, &Channel5,
	&Channel6, &Channel7, &Channel8, &Channel9, &Channel10, &Channel11,
};

//------------------------------------------------------------------------------
// DMA::Timer
//------------------------------------------------------------------------------
const Timer Timer0(0);
const Timer Timer1(1);
const Timer Timer2(2);
const Timer Timer3(3);

const Timer* TimerTbl[] = { nullptr, &Timer0, &Timer1, &Timer2, &Timer3, };

}
