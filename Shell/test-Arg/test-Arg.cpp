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
		Shell::Cmd::Arg::OptFloat("float",	0x0, "specifies a floating point number", "n"),
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
		{ "float long", 4, { "prog", "--float=3.14", "arg1", "arg2" } },
		{ "float negative", 4, { "prog", "--float=-2.5", "arg1", "arg2" } },
		{ "float zero", 4, { "prog", "--float=0.0", "arg1", "arg2" } },
		{ "float scientific", 4, { "prog", "--float=1.23e-4", "arg1", "arg2" } },
		{ "invalid float", 4, { "prog", "--float=notafloat", "arg1", "arg2" } },
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
			float floatVal = 0.0f;
			tout.Printf("  float: ");
			if (arg.GetFloat("float", &floatVal)) tout.Printf("%.2f\n", floatVal); else tout.Printf("(none)\n");
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
		bool limitFlag;
		const char* expected;
	};
	const EachNumTestCase cases[] = {
		{ "single number", 1, { "5" }, false, "5" },
		{ "range", 1, { "1-3" }, false, "1, 2, 3" },
		{ "range with step", 1, { "2-8:3" }, false, "2, 5, 8" },
		{ "multiple arguments", 2, { "1", "4-5" }, false, "1, 4, 5" },
		{ "invalid input", 1, { "abc" }, false, "(none)" },
		{ "reverse range", 1, { "5-3" }, false, "5, 4, 3" },
		{ "negative numbers", 1, { "-2-2" }, false, "-2, -1, 0, 1, 2" },
		{ "negative step", 1, { "5-1:2" }, false, "5, 3, 1" },
		{ "zero", 1, { "0" }, false, "0" },
		{ "large range", 1, { "10-15" }, false, "10, 11, 12, 13, 14, 15" },
		{ "large reverse range", 1, { "15-10" }, false, "15, 14, 13, 12, 11, 10" },
		{ "range with negative step", 1, { "10-2:2" }, false, "10, 8, 6, 4, 2" },
		{ "range with positive step", 1, { "2-10:2" }, false, "2, 4, 6, 8, 10" },
		{ "single negative", 1, { "-7" }, false, "-7" },
		{ "range with zero step (invalid)", 1, { "1-5:0" }, false, "(error)" },
		{ "step larger than range", 1, { "1-3:5" }, false, "1" },
		{ "step larger than reverse range", 1, { "5-1:10" }, false, "5" },
		{ "range with negative start", 1, { "-5--2" }, false, "-5, -4, -3, -2" },
		{ "range with negative end", 1, { "2--2" }, false, "2, 1, 0, -1, -2" },
		{ "range with negative step and negatives", 1, { "-2--5:-1" }, false, "-2, -3, -4, -5" },
		{ "range with positive step and negatives", 1, { "-5--2:2" }, false, "-5, -3" },
		{ "multiple single numbers", 3, { "1", "2", "3" }, false, "1, 2, 3" },
		{ "multiple ranges", 2, { "1-2", "4-5" }, false, "1, 2, 4, 5" },
		{ "mixed single and range", 3, { "7", "10-12", "15" }, false, "7, 10, 11, 12, 15" },
		{ "empty string", 1, { "" }, false, "(none)" },
		{ "range with missing end", 1, { "3-" }, false, "(none)" },
		{ "range with missing start", 1, { "-3" }, false, "-3" },
		{ "range with missing step", 1, { "1-5:" }, false, "1, 2, 3, 4, 5" },
		{ "range with step 1 explicitly", 1, { "1-3:1" }, false, "1, 2, 3" },
		{ "range with negative step explicitly", 1, { "3-1:-1" }, false, "3, 2, 1" },
		{ "multiple mixed", 4, { "1", "3-5", "7-5:-1", "9" }, false, "1, 3, 4, 5, 7, 6, 5, 9" },
		{ "comma numbers", 1, { "1,2,3" }, false, "1, 2, 3" },
		{ "comma ranges", 1, { "1-3,5-7" }, false, "1, 2, 3, 5, 6, 7" },
		{ "comma mixed", 1, { "1,3-5,7" }, false, "1, 3, 4, 5, 7" },
		{ "comma with step", 1, { "1-5:2,10-12" }, false, "1, 3, 5, 10, 11, 12" },
		{ "comma negative numbers", 1, { "-2,-1,0,1" }, false, "-2, -1, 0, 1" },
		{ "comma negative ranges", 1, { "-3--1,1-2" }, false, "-3, -2, -1, 1, 2" },
		{ "comma reverse ranges", 1, { "5-3,2-1" }, false, "5, 4, 3, 2, 1" },
		{ "comma with invalid", 1, { "1,abc,3-4" }, false, "1(error)" },
		{ "comma with empty", 1, { "1,,2-3" }, false, "1(error)" },
		{ "comma with step and negatives", 1, { "-5--1:2,3" }, false, "-5, -3, -1, 3" },
		{ "range with missing end", 1, { "3-" }, true, "3, 4, 5, 6, 7, 8, 9, 10" },
		{ "range with missing end", 1, { "13-" }, true, "13, 12, 11, 10" },
		{ "quoted string A", 1, { "\"A\"" }, false, "65" },
		{ "quoted string AB", 1, { "\"AB\"" }, false, "65, 66" },
		{ "quoted string ABC", 1, { "\"ABC\"" }, false, "65, 66, 67" },
		{ "quoted string ABCD", 1, { "\"ABCD\"" }, false, "65, 66, 67, 68" },
		{ "quoted string hello", 1, { "\"hello\"" }, false, "104, 101, 108, 108, 111" },
		{ "quoted string with space", 1, { "\"A B\"" }, false, "65, 32, 66" },
		{ "quoted string with numbers", 1, { "\"123\"" }, false, "49, 50, 51" },
		{ "quoted string mixed with numbers", 2, { "\"AB\"", "1-2" }, false, "65, 66, 1, 2" },
		{ "comma with quoted string", 1, { "1,\"X\",3" }, false, "1, 88, 3" },
		{ "empty quoted string", 1, { "\"\"" }, false, "(none)" },
		{ "comma with multiple strings", 1, { "\"A\",\"B\",\"C\"" }, false, "65, 66, 67" },
		{ "comma string and numbers", 1, { "\"AB\",1,2" }, false, "65, 66, 1, 2" },
		{ "comma numbers and string", 1, { "1,2,\"XY\"" }, false, "1, 2, 88, 89" },
		{ "comma string and range", 1, { "\"Hi\",5-6" }, false, "72, 105, 5, 6" },
		{ "comma range and string", 1, { "1-2,\"OK\"" }, false, "1, 2, 79, 75" },
		{ "comma mixed strings ranges numbers", 1, { "\"A\",3-4,\"B\",7" }, false, "65, 3, 4, 66, 7" },
		{ "comma string with special chars", 1, { "\"!\",\"@\",\"#\"" }, false, "33, 64, 35" },
		{ "comma empty and non-empty strings", 1, { "\"\",\"A\",\"\"" }, false, "65" },
		{ "comma long string with numbers", 1, { "\"TEST\",100-101" }, false, "84, 69, 83, 84, 100, 101" },
		{ "escape newline", 1, { "\"A\\nB\"" }, false, "65, 10, 66" },
		{ "escape carriage return", 1, { "\"A\\rB\"" }, false, "65, 13, 66" },
		{ "escape tab", 1, { "\"A\\tB\"" }, false, "65, 9, 66" },
		{ "escape backslash", 1, { "\"A\\\\B\"" }, false, "65, 92, 66" },
		{ "escape quote", 1, { "\"A\\\"B\"" }, false, "65, 34, 66" },
		{ "escape null", 1, { "\"A\\0B\"" }, false, "65, 0, 66" },
		{ "escape hex lowercase", 1, { "\"\\x41\\x42\"" }, false, "65, 66" },
		{ "escape hex uppercase", 1, { "\"\\x4A\\x4B\"" }, false, "74, 75" },
		{ "escape hex mixed case", 1, { "\"\\x4a\\x4B\"" }, false, "74, 75" },
		{ "multiple escapes", 1, { "\"\\n\\t\\r\"" }, false, "10, 9, 13" },
		{ "escape octal single digit", 1, { "\"A\\7B\"" }, false, "65, 7, 66" },
		{ "escape octal two digits", 1, { "\"A\\41B\"" }, false, "65, 33, 66" },
		{ "escape octal three digits", 1, { "\"A\\101B\"" }, false, "65, 65, 66" },
		{ "escape octal zero", 1, { "\"A\\000B\"" }, false, "65, 0, 66" },
		{ "escape octal max", 1, { "\"A\\377B\"" }, false, "65, 255, 66" },
		{ "escape hex single digit", 1, { "\"A\\x7B\"" }, false, "65, 123" },
		{ "escape hex zero", 1, { "\"A\\x00B\"" }, false, "65, 0, 66" },
		{ "escape hex max", 1, { "\"A\\xFFB\"" }, false, "65, 255, 66" },
		{ "escape hex mixed with octal", 1, { "\"\\x41\\101\"" }, false, "65, 65" },
		{ "escape hex special chars", 1, { "\"\\x21\\x40\\x23\"" }, false, "33, 64, 35" },
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
		if (tc.limitFlag) each.SetLimit(10);
		int n;
		bool first = true;
		tout.Printf("[nums] ");
		while (each.Next(&n)) {
			if (!first) tout.Printf(", ");
			tout.Printf("%d", n);
			first = false;
		}
		if (!each.IsSuccess()) {
			tout.Printf("(%s)", each.GetErrorMsg());
		} else if (first) {
			tout.Printf("(none)");
		}
		tout.Println();
		tout.Printf("[expt] %s\n", tc.expected);
		tout.Printf("\n");
	}
}

void test_EachCmd()
{
	char str[100];
	Tokenizer tokenizer = Shell::CreateTokenizer();
	char* argv[32];
	int argc = count_of(argv);
	const char* errorMsg = nullptr;
	::strcpy(str, "{cmd1 cmd2 cmd3 {cmd4 cmd5} cmd6} cmd7 cmd8");
	tokenizer.Tokenize(str, sizeof(str), argv, &argc, &errorMsg);
	Shell::Arg::EachCmd each(argv[0], argv[argc]);
	each.Initialize();
	each.Print();
	while (const char* proc = each.Next()) {
		::printf("%s\n", proc);
	}
	::printf("----\n");
}

int main()
{
	::stdio_init_all();
	//test_Parse();
	//test_EachNum();
	test_EachCmd();
	for (;;) ::tight_loop_contents();
}
