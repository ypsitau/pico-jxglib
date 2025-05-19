#include "pico/stdio.h"
#include "jxglib/Shell.h"

using namespace jxglib;

void test_case(const char* desc, int argc, const char* argv[])
{
	Printable& tout = Stdio::Instance;
	static const Shell::Cmd::Arg::Opt optTbl[] = {
		Shell::Cmd::Arg::OptBool("help", "h", "prints this help"),
		Shell::Cmd::Arg::OptBool("flag", "f", "a boolean flag"),
		Shell::Cmd::Arg::OptInt("num", "n", "specifies a number", "n"),
		Shell::Cmd::Arg::OptString("name", "", "specifies a name", "str"),
		Shell::Cmd::Arg::OptString("path", "p", "specifies a path", "str"),
	};
	Shell::Cmd::Arg arg(optTbl, count_of(optTbl));
	tout.Printf("---- %s ----\n", desc);
	tout.Printf("[before] ");
	for (int i = 0; i < argc; ++i) tout.Printf((i == 0)? "%s" : " %s", argv[i]);
	tout.Println();
	if (arg.Parse(tout, argc, argv)) {
		tout.Printf("[parsed] ");
		for (int i = 0; i < argc; ++i) tout.Printf((i == 0)? "%s" : " %s", argv[i]);
		tout.Println();
		tout.Printf("  help: %d\n", arg.GetBool("help"));
		tout.Printf("  flag: %d\n", arg.GetBool("flag"));
		int num = 0;
		tout.Printf("  num:  ");
		if (arg.GetInt("num", &num)) tout.Printf("%d\n", num); else tout.Printf("(none)\n");
		const char* name = 0;
		tout.Printf("  name: ");
		if (arg.GetString("name", &name)) tout.Printf("%s\n", name); else tout.Printf("(none)\n");
		const char* path = 0;
		tout.Printf("  path: ");
		if (arg.GetString("path", &path)) tout.Printf("%s\n", path); else tout.Printf("(none)\n");
	} else {
		tout.Printf("** Failed **\n");
	}
	tout.Printf("\n");
}

int main()
{
	::stdio_init_all();
	do {
		const char* argv[] = { "prog", "arg1", "arg2" };
		test_case("no options", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--help", "arg1", "arg2" };
		test_case("help long", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "-h", "arg1", "arg2" };
		test_case("help short", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--flag", "arg1", "arg2" };
		test_case("flag long", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "-f", "arg1", "arg2" };
		test_case("flag short", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--num=123", "arg1", "arg2" };
		test_case("num long", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "-n", "456", "arg1", "arg2" };
		test_case("num short", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--name=Alice", "arg1", "arg2" };
		test_case("name long", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--path=/tmp", "arg1", "arg2" };
		test_case("path long", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "-p", "/home", "arg1", "arg2" };
		test_case("path short", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--flag", "--num=789", "arg1", "arg2" };
		test_case("flag and num", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--flag", "--name=Bob", "arg1", "arg2" };
		test_case("flag and name", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--name=", "arg1", "arg2" };
		test_case("empty string", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--num=42", "--name=Carol", "arg1", "arg2" };
		test_case("num and name", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--flag", "--num=1", "--name=D", "arg1", "arg2" };
		test_case("flag, num, name", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--flag", "--num=2", "--name=Eve", "--path=/dev", "arg1", "arg2" };
		test_case("all options", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--flag", "--no-flag", "arg1", "arg2" };
		test_case("flag and no-flag", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--num=notanumber", "arg1", "arg2" };
		test_case("invalid int", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--unknown", "arg1", "arg2" };
		test_case("unknown option", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--name", "arg1", "arg2" };
		test_case("missing string value", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "--num", "arg1", "arg2" };
		test_case("missing int value", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "-p" };
		test_case("missing string value", count_of(argv), argv);
	} while (0);
	do {
		const char* argv[] = { "prog", "-n" };
		test_case("missing int value", count_of(argv), argv);
	} while (0);
	for (;;) ::tight_loop_contents();
}
