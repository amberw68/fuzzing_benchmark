/*
 * PinTrace
 *
 * API call trace tool built with intel pin (https://software.intel.com/en-us/articles/pin-a-binary-instrumentation-tool-downloads).
 *
 * CC by mirar@chaosmail.org
 *
 * This module can either be run in audit mode (-a flag) or provided with a config file (-c path/to/config).
 *
 * The config format is as follows:
 *    symbol;ARGTYPE:argname;ARGTYPE:argname...\n
 *
 * Example:
 *    strcmp;CSTR:s1;CSTR:s2
 *
 */

#include "pin.H"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <list>

using std::endl;
using std::getline;
using std::list;
using std::string;
using std::stringstream;

typedef enum {
    CSTR,
    WSTR,
    PTR,
    VAL
} argtype_t;

typedef struct {
    argtype_t argtype;
    unsigned int index;
    string name;
} argconf_t;

typedef struct {
    string symbol;
    list<argconf_t> argconf;
} symconf_t;

ostream *out = &cerr;
list<string> trace;
list<symconf_t> config;
bool auditMode = false;

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "", "specify an output file");
KNOB<string> KnobConfigFile(KNOB_MODE_WRITEONCE, "pintool", "c", "", "specify trace config file");
KNOB<bool> KnobAuditMode(KNOB_MODE_WRITEONCE, "pintool", "a", "", "enable audit mode");

argconf_t ParseArgToken(string token) {
    size_t dpos;
    argconf_t arg;
    if ((dpos = token.find(":")) != string::npos) {
        auto typestring = token.substr(0, dpos);
        auto namestring = token.substr(dpos+1, token.length());
        arg.name = namestring;
        token = typestring;
    }
    if (token == "CSTR") {
        arg.argtype = CSTR;
    } else if (token == "WSTR") {
        arg.argtype = WSTR;
    } else if (token == "PTR") {
        arg.argtype = PTR;
    } else if (token == "VAL") {
        arg.argtype = VAL;
    }
    return arg;
}

/*
 * config format
 * symbol;TYPE:name;TYPE:name ... \n
 */
list<symconf_t> ReadConfig(string configFile)  {
    ifstream f(configFile.c_str());
    string line;
    list<symconf_t> config;
    while(getline(f, line)) {
        if (line.length() == 0 ||
                line[0] == '#')
            continue;

        // parse line
        symconf_t s;

        size_t last { 0 };
        size_t next { 0 };
        bool readSym { false };
        while ((next = line.find(";", last)) != string::npos) {
            auto token = line.substr(last, next-last);
            if (!readSym) {
                s.symbol = token;
                last = next + 1;
                readSym = true;
                continue;
            }
            auto arg = ParseArgToken(token);
            s.argconf.push_back(arg);
            last = next + 1;
        }

        auto token = line.substr(last, line.length());
        auto arg = ParseArgToken(token);
        s.argconf.push_back(arg);
        config.push_back(s);
    }

    f.close();
    return config;
}

INT32 Usage() {
    cerr << "This pintool traces calls to the specified functions in dynamic libraries" << endl;
    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

VOID Fini(INT32 code, VOID *v) {
    for (auto &t : trace)
        *out << t << endl;
}

VOID RoutineCall(string *library,
        string *symbol,
        void *arg0,
        void *arg1,
        void *arg2,
        void *arg3,
        void *arg4,
        void *arg5,
        void *arg6,
        void *arg7) {
    stringstream ss;
    if (auditMode) {
        ss << *library << " " << *symbol;
    }
    for (auto &sym : config) {
        if(sym.symbol == *symbol) {
            ss << *library << " " << *symbol;
            unsigned int i {0};
            for (auto &arg : sym.argconf) {
                void *argval;
                switch (i) {
                    case 0:
                        argval = arg0;
                        break;
                    case 1:
                        argval = arg1;
                        break;
                    case 2:
                        argval = arg2;
                        break;
                    case 3:
                        argval = arg3;
                        break;
                    case 4:
                        argval = arg4;
                        break;
                    case 5:
                        argval = arg5;
                        break;
                    case 6:
                        argval = arg6;
                        break;
                    case 7:
                        argval = arg7;
                        break;
                }
                switch (arg.argtype) {
                    case CSTR:
                        if (argval != 0) {
                            ss << " " << arg.name << " " << reinterpret_cast<char*>(argval);
                        }
                        break;
                    case WSTR:
                        if (argval != 0) {
                            auto warg =  wstring(reinterpret_cast<wchar_t*>(argval));
                            auto sarg = string(warg.begin(), warg.end());
                            ss << " " << arg.name << " " << sarg;
                        }
                        break;
                    case VAL:
                        ss << " " << arg.name << " " << reinterpret_cast<unsigned long long>(argval);
                        break;
                    case PTR:
                        ss << " " << arg.name << " " << argval;
                        break;
                }
                i++;
            }
        }
    }

    auto s = ss.str();
    if (s.length() > 0) {
        PIN_LockClient();
        trace.push_back(ss.str());
        PIN_UnlockClient();
    }
}

VOID ImageLoad(IMG img, VOID *v) {
    stringstream ss;
    SYM sym = IMG_RegsymHead(img);
    auto library = new string(IMG_Name(img));
    while (SYM_Valid(sym)) {
        auto symbol = new string(SYM_Name(sym));
        auto rtn = RTN_FindByName(img, symbol->c_str());
        RTN_Open(rtn);
        RTN_InsertCall(rtn, IPOINT_BEFORE, (AFUNPTR)RoutineCall,
                IARG_PTR, library, IARG_PTR, symbol,
                IARG_FUNCARG_CALLSITE_VALUE, 0,
                IARG_FUNCARG_CALLSITE_VALUE, 1,
                IARG_FUNCARG_CALLSITE_VALUE, 2,
                IARG_FUNCARG_CALLSITE_VALUE, 3,
                IARG_FUNCARG_CALLSITE_VALUE, 4,
                IARG_FUNCARG_CALLSITE_VALUE, 5,
                IARG_FUNCARG_CALLSITE_VALUE, 6,
                IARG_FUNCARG_CALLSITE_VALUE, 7,
                IARG_END);
        RTN_Close(rtn);
        sym = SYM_Next(sym);
    }
}



int main(int argc, char* argv[]) {
    if (PIN_Init(argc, argv)) return Usage();

    string fileName { KnobOutputFile.Value() };
    string configFileName { KnobConfigFile.Value() };

    if (!fileName.empty()) {
        out = new ofstream(fileName.c_str());
    }

    if (KnobAuditMode) {
        auditMode = true;
    } else {
        config = ReadConfig(configFileName);
    }

    IMG_AddInstrumentFunction(ImageLoad, nullptr);

    PIN_AddFiniFunction(Fini, nullptr);

    PIN_InitSymbols();

    PIN_StartProgram();

    return 0;
}
