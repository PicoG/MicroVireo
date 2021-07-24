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

void RunExec();

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
    for (int i = 0; i < 10; ++i) {
        gPlatform.IO.Print("Hello, world!\n");
        sleep_ms(1000);
    }

    // Interactive mode is experimental.
    // the core loop should be processed by by a vireo program
    // once IO primitives are all there.
    gPlatform.IO.Print("Before RootShell\n");
    gShells._pRootShell = TypeManager::New(nullptr);
    gPlatform.IO.Print("Before UserShell\n");
    sleep_ms(100);
    gShells._pUserShell = TypeManager::New(gShells._pRootShell);

    gPlatform.IO.Print("Before Loop\n");
    sleep_ms(100);
    while (gShells._keepRunning) {
        gPlatform.IO.Print(">");
        {
            TypeManagerScope scope(gShells._pUserShell);
            gPlatform.IO.Print("After Scope\n");
            STACK_VAR(String, buffer);
            gPlatform.IO.Print("After Buffer\n");
            gPlatform.IO.Print(">");
            gPlatform.IO.ReadStdin(buffer.Value);
            gPlatform.IO.Print("After Stdin");
            SubString input = buffer.Value->MakeSubStringAlias();
            TDViaParser::StaticRepl(gShells._pUserShell, &input);
        }

        while (gShells._keepRunning) {
            RunExec();
        }
    }

    gPlatform.Shutdown();
    return 0;
}

//------------------------------------------------------------
//! Execution pump.
void Vireo::RunExec() {
    TypeManagerRef tm = gShells._pUserShell;
    TypeManagerScope scope(tm);
    
    // These numbers may need further tuning (numSlices and millisecondsToRun).
    // They should match the values for VJS in io/module_eggShell.js
    Int32 state = tm->TheExecutionContext()->ExecuteSlices(10000, 4);
    Int32 delay = state > 0 ? state : 0;
    gShells._keepRunning = (state != kExecSlices_ClumpsFinished);

    if (delay) {
        gPlatform.Timer.SleepMilliseconds(delay);
    }
}