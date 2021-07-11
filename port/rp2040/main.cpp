#include "ExecutionContext.h"
#include "TDCodecVia.h"
#include "DebuggingToggles.h"

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

    gPlatform.Setup();
    gShells._keepRunning = true;
    LOG_PLATFORM_MEM("Mem after init")

    // Interactive mode is experimental.
    // the core loop should be processed by by a vireo program
    // once IO primitives are all there.
    gShells._pRootShell = TypeManager::New(nullptr);
    gShells._pUserShell = TypeManager::New(gShells._pRootShell);
    while (gShells._keepRunning) {
        gPlatform.IO.Print(">");
        {
            TypeManagerScope scope(gShells._pUserShell);
            STACK_VAR(String, buffer);
            gPlatform.IO.ReadStdin(buffer.Value);
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