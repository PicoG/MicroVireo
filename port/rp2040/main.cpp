#include "ExecutionContext.h"
#include "TDCodecVia.h"
#include "DebuggingToggles.h"

#include <stdio.h>
#include <pico/stdlib.h>

namespace Vireo {

static struct {
    TypeManagerRef _pRootShell;
    TypeManagerRef _pUserShell;
    Boolean _keepRunning;
} gShells;

bool RunExec();

}  // namespace Vireo

int main()
{
    using namespace Vireo;  // NOLINT(build/namespaces)

    stdio_init_all();

    gPlatform.Setup();
    gShells._keepRunning = true;
    //LOG_PLATFORM_MEM("Mem after init")

    //Delay for ~10 seconds to give a chance to get a terminal app
    //attached before trying to initialize the rest of Vireo
    for (int i = 0; i < 6; ++i) {
        gPlatform.IO.Print(".\n");
        sleep_ms(1000);
    }
    gPlatform.IO.Print("\n");

    // Interactive mode is experimental.
    // the core loop should be processed by by a vireo program
    // once IO primitives are all there.
    //gPlatform.IO.Print("Before RootShell\n");
    gShells._pRootShell = TypeManager::New(nullptr);

    //gPlatform.IO.Print("Before UserShell\n");
    gShells._pUserShell = TypeManager::New(gShells._pRootShell);

    //gPlatform.IO.Print("Before Loop\n");
    //while (gShells._keepRunning) {
    while (true) {
        gPlatform.IO.Print("VIREO> ");
        {
            TypeManagerScope scope(gShells._pUserShell);
            //gPlatform.IO.Print("After Scope\n");
            STACK_VAR(String, buffer);
            //gPlatform.IO.Print("After Buffer\n");
            //gPlatform.IO.Print(">");
            //sleep_ms(10);
            gPlatform.IO.ReadStdin(buffer.Value);
            //gPlatform.IO.Print("After Stdin");
            SubString input = buffer.Value->MakeSubStringAlias();

            if (input.ComparePrefixCStr("dump()")) {
                gShells._pRootShell->DumpTypeNameDictionary();
                continue;
            } else {
                TDViaParser::StaticRepl(gShells._pUserShell, &input);
            }
        }

        bool exec = true;
        while (exec) {
            //gPlatform.IO.Print("*\n");
            exec = RunExec();
        }
    }

    gPlatform.Shutdown();
    return 0;
}

//------------------------------------------------------------
//! Execution pump.
bool Vireo::RunExec() {
    TypeManagerRef tm = gShells._pUserShell;
    TypeManagerScope scope(tm);
    
    // These numbers may need further tuning (numSlices and millisecondsToRun).
    // They should match the values for VJS in io/module_eggShell.js
    Int32 state = tm->TheExecutionContext()->ExecuteSlices(10000, 4);
    Int32 delay = state > 0 ? state : 0;
    //gShells._keepRunning = (state != kExecSlices_ClumpsFinished);

    if (delay) {
        gPlatform.Timer.SleepMilliseconds(delay);
    }

    return state != kExecSlices_ClumpsFinished;
}