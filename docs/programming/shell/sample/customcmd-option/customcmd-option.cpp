#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

using namespace jxglib;

ShellCmd(customcmd, "tests command line arguments")
{
    static const Arg::Opt optTbl[] = {
        Arg::OptBool("help",   'h', "display help"),
        Arg::OptBool("flag",   'b', "bool value"),
        Arg::OptString("str",  's', "string value", "STR"), 
        Arg::OptInt("num",     'i', "int value",    "NUM"),
        Arg::OptFloat("float", 'f', "float value",  "FLOAT"),
    };
    Arg arg(optTbl, count_of(optTbl));
    if (!arg.Parse(terr, argc, argv)) return Result::Error;
    if (arg.GetBool("help")) {
        arg.PrintHelp(terr);
        return Result::Success;
    }
    for (int i = 0; i < argc; i++) {
        tout.Printf("argv[%d] \"%s\"\n", i, argv[i]);
    }
    tout.Printf("flag  %s\n", arg.GetBool("flag")? "true" : "false");
    const char* strValue;
    if (arg.GetString("str", &strValue)) {
        tout.Printf("str   \"%s\"\n", strValue);
    } else {
        tout.Printf("str   (none)\n");
    }
    int intValue;
    if (arg.GetInt("num", &intValue)) {
        tout.Printf("num   %d\n", intValue);
    } else {
        tout.Printf("num   (none)\n");
    }
    float floatValue;
    if (arg.GetFloat("float", &floatValue)) {
        tout.Printf("float %f\n", floatValue);
    } else {
        tout.Printf("float (none)\n");
    }
    return Result::Success;
}

int main(void)
{
    ::stdio_init_all();
    Serial::Terminal terminal;
    terminal.Initialize();
    Shell::AttachTerminal(terminal);
    for (;;) {
        Tickable::Tick();
    }
}