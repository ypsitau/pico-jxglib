#include "pico/stdio.h"
#include "jxglib/Shell.h"

using namespace jxglib;

void test_Parse()
{
	struct ParseTestCase {
		const char* desc;
		int argc;
		const char* argv[8];
	};
	Printable& tout = Stdio::Instance;
	static const Shell::Cmd::Arg::Opt optTbl[] = {
		Shell::Cmd::Arg::OptBool("help",	'h', "prints this help"),
		Shell::Cmd::Arg::OptBool("flag",	'f', "a boolean flag"),
		Shell::Cmd::Arg::OptInt("num",		'n', "specifies a number", "n"),
		Shell::Cmd::Arg::OptString("name",	0x0, "specifies a name", "str"),
		Shell::Cmd::Arg::OptString("path",	'p', "specifies a path", "str"),
	};
	const ParseTestCase cases[] = {
		{ "no options", 3, { "prog", "arg1", "arg2" } },
		{ "help long", 4, { "prog", "--help", "arg1", "arg2" } },
		{ "help short", 4, { "prog", "-h", "arg1", "arg2" } },
		{ "help and flag short", 5, { "prog", "-h", "-f", "arg1", "arg2" } },
		{ "help and flag short in a series", 4, { "prog", "-hf", "arg1", "arg2" } },
		{ "help, flag and path short in a series", 5, { "prog", "-hfp", "/tmp", "arg1", "arg2" } },
		{ "help, flag and path short in a series (wrong order)", 5, { "prog", "-hpf", "/tmp", "arg1", "arg2" } },
		{ "flag long", 4, { "prog", "--flag", "arg1", "arg2" } },
		{ "flag short", 4, { "prog", "-f", "arg1", "arg2" } },
		{ "num long", 4, { "prog", "--num=123", "arg1", "arg2" } },
		{ "num short", 4, { "prog", "-n", "456", "arg1", "arg2" } },
		{ "name long", 4, { "prog", "--name=Alice", "arg1", "arg2" } },
		{ "path long", 4, { "prog", "--path=/tmp", "arg1", "arg2" } },
		{ "path short", 4, { "prog", "-p", "/home", "arg1", "arg2" } },
		{ "flag and num", 5, { "prog", "--flag", "--num=789", "arg1", "arg2" } },
		{ "flag and name", 5, { "prog", "--flag", "--name=Bob", "arg1", "arg2" } },
		{ "empty string", 4, { "prog", "--name=", "arg1", "arg2" } },
		{ "num and name", 5, { "prog", "--num=42", "--name=Carol", "arg1", "arg2" } },
		{ "flag, num, name", 6, { "prog", "--flag", "--num=1", "--name=D", "arg1", "arg2" } },
		{ "all options", 7, { "prog", "--flag", "--num=2", "--name=Eve", "--path=/dev", "arg1", "arg2" } },
		{ "flag and no-flag", 5, { "prog", "--flag", "--no-flag", "arg1", "arg2" } },
		{ "invalid int", 4, { "prog", "--num=notanumber", "arg1", "arg2" } },
		{ "unknown option", 4, { "prog", "--unknown", "arg1", "arg2" } },
		{ "missing string value", 4, { "prog", "--name", "arg1", "arg2" } },
		{ "missing int value", 4, { "prog", "--num", "arg1", "arg2" } },
		{ "missing string value", 2, { "prog", "-p" } },
		{ "missing int value", 2, { "prog", "-n" } },
	};

	for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
		const ParseTestCase& tc = cases[i];
		Shell::Cmd::Arg arg(optTbl, count_of(optTbl));
		tout.Printf("---- %s ----\n", tc.desc);
		tout.Printf("[argv] ");
		for (int j = 0; j < tc.argc; ++j)
			tout.Printf((j == 0) ? "%s" : " %s", tc.argv[j]);
		tout.Println();
		int argc = tc.argc;
		const char* argv[8];
		for (int j = 0; j < tc.argc; ++j) argv[j] = tc.argv[j];
		if (arg.Parse(tout, argc, argv)) {
			tout.Printf("[parsed] ");
			for (int j = 0; j < tc.argc; ++j)
				tout.Printf((j == 0) ? "%s" : " %s", tc.argv[j]);
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
}

void test_EachNum()
{
	Printable& tout = Stdio::Instance;

	struct EachNumTestCase {
		const char* desc;
		int argc;
		const char* argv[8];
		const char* expected;
	};

	const EachNumTestCase cases[] = {
		{ "single number", 1, { "5" }, "5" },
		{ "range", 1, { "1-3" }, "1, 2, 3" },
		{ "range with step", 1, { "2-8:3" }, "2, 5, 8" },
		{ "multiple arguments", 2, { "1", "4-5" }, "1, 4, 5" },
		{ "invalid input", 1, { "abc" }, "(none)" },
		{ "reverse range", 1, { "5-3" }, "5, 4, 3" },
		{ "negative numbers", 1, { "-2-2" }, "-2, -1, 0, 1, 2" },
		{ "negative step", 1, { "5-1:2" }, "5, 3, 1" },
		{ "zero", 1, { "0" }, "0" },
		{ "large range", 1, { "10-15" }, "10, 11, 12, 13, 14, 15" },
		{ "large reverse range", 1, { "15-10" }, "15, 14, 13, 12, 11, 10" },
		{ "range with negative step", 1, { "10-2:2" }, "10, 8, 6, 4, 2" },
		{ "range with positive step", 1, { "2-10:2" }, "2, 4, 6, 8, 10" },
		{ "single negative", 1, { "-7" }, "-7" },
#if 1
		{ "range with zero step (invalid)", 1, { "1-5:0" }, "(none)" },
		{ "step larger than range", 1, { "1-3:5" }, "1" },
		{ "step larger than reverse range", 1, { "5-1:10" }, "5" },
		{ "range with negative start", 1, { "-5--2" }, "-5, -4, -3, -2" },
		{ "range with negative end", 1, { "2--2" }, "2, 1, 0, -1, -2" },
		{ "range with negative step and negatives", 1, { "-2--5:-1" }, "-2, -3, -4, -5" },
		{ "range with positive step and negatives", 1, { "-5--2:2" }, "-5, -3" },
		{ "multiple single numbers", 3, { "1", "2", "3" }, "1, 2, 3" },
		{ "multiple ranges", 2, { "1-2", "4-5" }, "1, 2, 4, 5" },
		{ "mixed single and range", 3, { "7", "10-12", "15" }, "7, 10, 11, 12, 15" },
		{ "empty string", 1, { "" }, "(none)" },
		{ "range with missing end", 1, { "3-" }, "(none)" },
		{ "range with missing start", 1, { "-3" }, "-3" },
		{ "range with missing step", 1, { "1-5:" }, "1, 2, 3, 4, 5" },
		{ "range with step 1 explicitly", 1, { "1-3:1" }, "1, 2, 3" },
		{ "range with negative step explicitly", 1, { "3-1:-1" }, "3, 2, 1" },
		{ "multiple mixed", 4, { "1", "3-5", "7-5:-1", "9" }, "1, 3, 4, 5, 7, 6, 5, 9" },
#endif
	};
	for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
		const EachNumTestCase& tc = cases[i];
		tout.Printf("---- %s ----\n", tc.desc);
		tout.Printf("[argv] ");
		for (int j = 0; j < tc.argc; ++j)
			tout.Printf((j == 0) ? "%s" : " %s", tc.argv[j]);
		tout.Println();
		const char** argv = const_cast<const char**>(tc.argv);
		Shell::Arg::EachNum each(argv[0], argv[tc.argc]);
		int n;
		bool first = true;
		tout.Printf("[nums] ");
		while (each.Next(&n)) {
			if (!first) tout.Printf(", ");
			tout.Printf("%d", n);
			first = false;
		}
		if (first) {
			tout.Printf("(none)");
		}
		tout.Println();
		tout.Printf("\n");
	}
}

int main()
{
	::stdio_init_all();
	//test_Parse();
	test_EachNum();
	for (;;) ::tight_loop_contents();
}
