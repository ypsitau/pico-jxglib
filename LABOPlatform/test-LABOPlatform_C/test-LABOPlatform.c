#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

int shellcmd_test(int argc, char* argv[])
{
	FILE* fp;
	fp = fopen("test.txt", "w");
	if (fp) {
		//fwrite("Test", 4, 1, fp);
		fprintf(fp, "Test command executed with %d arguments.\n", argc);
		fclose(fp);
	}
	//printf("Test command executed with %d arguments.\n", argc);
	//for (int i = 0; i < argc; ++i) {
	//	printf("Arg[%d]: %s\n", i, argv[i]);
	//}
	return 0;
}

int main(void)
{
	stdio_init_all();
	jxglib_labo_init(true);
	jxglib_shellcmd("test", "Test command", shellcmd_test);
	for (;;) {
		//printf("Hello, pico-jxgLABO!\n");
		jxglib_sleep(300);
	}
}
