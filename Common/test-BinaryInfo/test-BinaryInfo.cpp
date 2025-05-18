#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/BinaryInfo.h"

using namespace jxglib;

int main()
{
	bi_decl(bi_2pins_with_func(GPIO3, GPIO4, GPIO_FUNC_PIO0));
	bi_decl(bi_4pins_with_func(GPIO8, GPIO9, GPIO10, GPIO11, GPIO_FUNC_PWM));
	bi_decl(bi_1pin_with_func(GPIO2, GPIO_FUNC_I2C));
	bi_decl(bi_5pins_with_func(GPIO12, GPIO13, GPIO14, GPIO15, GPIO16, GPIO_FUNC_USB));
	bi_decl(bi_1pin_with_name(GPIO17, "name1"));
	bi_decl(bi_2pins_with_func(GPIO0, GPIO1, GPIO_FUNC_SPI));
	bi_decl(bi_3pins_with_func(GPIO5, GPIO6, GPIO7, GPIO_FUNC_PIO1));
	bi_decl(bi_2pins_with_names(GPIO18, "name1", GPIO19, "name2"));
	bi_decl(bi_4pins_with_names(GPIO23, "name1", GPIO24, "name2", GPIO25, "name3", GPIO26, "name4"));
	bi_decl(bi_3pins_with_names(GPIO20, "name1", GPIO21, "name2", GPIO22, "name3"));
	::stdio_init_all();
	auto& tout = Stdio::Instance;
	BinaryInfo::PrintProgramInformation(tout);    
	tout.Println();
	BinaryInfo::PrintFixedPinInformation(tout);    
	tout.Println();
	BinaryInfo::PrintBuildInformation(tout);    
	tout.Println();
	BinaryInfo::PrintMemoryMap(tout);    
	//BinaryInfo::Print(tout);    
	for (;;) ::tight_loop_contents();
}
