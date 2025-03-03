#include "ExecutionContext.h"
#include "TDCodecVia.h"
#include "DebuggingToggles.h"

#include <stdio.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>

#include <picog.h>
#include <pico/unique_id.h>

#define ALIAS_LEN_MAX 20

namespace Vireo {

static struct {
    TypeManagerRef _pRootShell;
    TypeManagerRef _pUserShell;
    Boolean _keepRunning;
} gShells;

static const char * runStr = "enqueue(main)\n";

bool RunExec();

bool SaveVia();

void ShowVia();

}  // namespace Vireo

int main()
{
    using namespace Vireo;  // NOLINT(build/namespaces)

    gPlatform.IO.InitStatusLED();

    stdio_init_all();

    //configure input for skipping startup
    gpio_init(22);
    gpio_set_dir(22, false);
    gpio_set_pulls(22, false, true);

    gPlatform.Setup();
    gShells._keepRunning = true;

    bool status = false;
    
    //need to give time for USB init and enumeration
    for (int i = 0; i < 8; ++i) {
        sleep_ms(200);
        status = !status;
        gPlatform.IO.StatusLED(status);
    }

    //TODO: Remove load() command. store() and boot will automatically
    // invoke equivalent load() functionality. load() functionality to invoke
    // parser.

    gShells._pRootShell = TypeManager::New(nullptr);
    gShells._pUserShell = TypeManager::New(gShells._pRootShell);
        
    gPlatform.IO.Print("\nChecking for startup Via...");

    PersistedVia via;

    gPlatform.Persist.LoadVia(&via);

    bool hasVia = gPlatform.Persist.HasStartup();
    bool loadStored = false;
    bool runStored = false;
    bool doRepl = false;

    if (hasVia) {
        gPlatform.IO.Print("Found.\n");
        gPlatform.IO.Print("Press any key to skip autorun...");
        gPlatform.IO.StatusLED(true);

        if (!gpio_get(22)) {
            int c = getchar_timeout_us(2000000);
            runStored = c == PICO_ERROR_TIMEOUT;

            if (runStored && gpio_get(22)) {
                runStored = false;
            }
        }

        gPlatform.IO.StatusLED(false);

        loadStored = runStored;

        if (runStored) {
            gPlatform.IO.Print("Running Startup Via.\n");
        } else {
            gPlatform.IO.Print("Skipped.\n");
        }
    } else {
        gPlatform.IO.Print("No Via stored, starting shell.\n");
    }

    gPlatform.IO.Print("\n");

    while (true) {
        doRepl = false;

        if (!runStored && !loadStored) {
            gPlatform.IO.Print("picoG> ");
        }

        {
            TypeManagerScope scope(gShells._pUserShell);
            STACK_VAR(String, buffer);

            SubString input;

            if (loadStored) {
                //loadStored is cleared after REPL so we can reply with OK
                //loadStored = false;
                gPlatform.IO.Print("Loading Stored Via...");
                input = SubString((uint8_t *)via.source, (uint8_t *)(via.source + via.info.len));
                doRepl = true;
            } else if (runStored) {
                //runStored = false;
                gPlatform.IO.Print("Running loaded Via...");
                input = SubString(runStr);
                doRepl = true;
            } else {

                gPlatform.IO.ReadStdin(buffer.Value);
                input = buffer.Value->MakeSubStringAlias();

                //gPlatform.IO.Printf("(%d)\n", input.Length());

                if (input.Length() == 0) {
                    continue;
                } else if (input.ComparePrefixCStr("dump()")) {
                    gShells._pRootShell->DumpTypeNameDictionary();
                    continue;
                } else if (input.ComparePrefixCStr("store()")) {
                    if (SaveVia()) {
                        gPlatform.IO.Print("\nOK\n");
                    } else {
                        gPlatform.IO.Print("\nStore Aborted!\n");
                    }
                } else if (input.ComparePrefixCStr("show()")) {
                    if (gPlatform.Persist.HasVia()) {
                        ShowVia();
                    } else {
                        gPlatform.IO.Print("NO VIA LOADED!\n");
                    }
                } else if (input.ComparePrefixCStr("erase()")) {
                    gPlatform.Persist.ClearVia();
                    gPlatform.IO.Print("OK\n");
                } else if (input.ComparePrefixCStr("load()")) {
                    loadStored = true;
                } else if (input.ComparePrefixCStr("run()")) {
                    if (gPlatform.Persist.HasVia()) {
                        runStored = true;
                    } else {
                        gPlatform.IO.Print("NO VIA LOADED!\n");
                    }
                } else if (input.ComparePrefixCStr("reset()")) {
                    gShells._pUserShell->DeleteTypes(false);
                    gPlatform.IO.Print("OK\n");
                } else if (input.ComparePrefixCStr("mem()")) {
                    fprintf(stdout, "Vireo Used Memory: %d\n", gPlatform.Mem.TotalAllocated());
                    fflush(stdout);
                } else if (input.ComparePrefixCStr("version()")) {
                    gPlatform.IO.Printf("%s\n%s\nOK\n", PICOG_VERSION, PICOG_VERSION_TS);
                } else if (input.ComparePrefixCStr("id()")) {
                    uint32_t len = PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1;
                    char id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES * 2 + 1];
                    
                    pico_get_unique_board_id_string(id, len);
                    gPlatform.IO.Printf("%s\nOK\n", id);
                } else if (input.ComparePrefixCStr("platform()")) {
                    gPlatform.IO.Printf("%s\nOK\n", PICOG_PLATFORM);
                } else if (input.ComparePrefixCStr("board()")) {
                    gPlatform.IO.Printf("%s\nOK\n", PICOG_BOARD);
                } else if (input.ComparePrefixCStr("alias(")) {
                    const Utf8Char *aliasBegin = 0, *aliasEnd = 0;

                    for(const Utf8Char *c = input.Begin(); c != input.End(); ++c) {
                        if (*c == '(') {
                            aliasBegin = c + 1;
                        } else if (*c == ')') {
                            aliasEnd = c - 1;
                        }
                    }

                    if (aliasBegin && aliasEnd) {
                        int len = aliasEnd - aliasBegin + 1;

                        if (len == 0) {
                            const char * alias = gPlatform.Persist.GetAlias();
                            if (alias) {
                                gPlatform.IO.Print(alias);
                            }

                            gPlatform.IO.Print("\nOK\n");
                        } else if (len <= ALIAS_LEN_MAX) {
                            gPlatform.Persist.SetAlias(aliasBegin, aliasEnd);
                            gPlatform.IO.Print("OK\n");
                        }
                    } else {
                        gPlatform.IO.Print("Error: Incomplete name command.\n");
                    }
                } else if (input.ComparePrefixCStr("clearalias()")) {
                    gPlatform.Persist.ClearAlias();
                    gPlatform.IO.Print("OK\n");
                } else{
                    doRepl = true;
                }
            }

            if (doRepl) {
                doRepl = false;

                NIError e = TDViaParser::StaticRepl(gShells._pUserShell, &input);

                if (loadStored) {
                    loadStored = false;
                    //Send OK if we made it successfully past load() action
                    if (e) {
                        gPlatform.IO.Print("FAILED\n");
                    } else {
                        gPlatform.IO.Print("Loaded OK\n");
                    }
                } else if (runStored) {
                    runStored = false;
                    if (e) {
                        gPlatform.IO.Print("FAILED\n");
                    } else {
                        gPlatform.IO.Print("Loaded OK\n");
                    }
                }
            }
        }

        bool exec = true;
        while (exec) {
            exec = RunExec();
        }
    }

    gPlatform.Shutdown();
    return 0;
}

void Vireo::ShowVia() {
    //gPlatform.IO.Print(gPlatform.Persist.CStr());
    //gPlatform.IO.Print("\n");

    char * c = gPlatform.Persist.CStr();

    int line = 0;
    while (*c) {
        if (*c == '\n') {
            gPlatform.IO.Printf("\n%04d ", ++line);
            fflush(stdout);
        } else {
            gPlatform.IO.Print(*c);
        }

        c++;
    }

    gPlatform.IO.Print("\n\n");
}

bool Vireo::SaveVia() {
    gPlatform.IO.Print("Existing Via invalidated.\n");
    gPlatform.IO.Print("Saving Via to EOF (EOF = Ctrl+D, Ctrl+C to cancel)\n");
    gPlatform.IO.Print("OK\n");

    PlatformPersist *p = &gPlatform.Persist;

    p->StartVia();

    char c = 0, last = 0;
    bool inString = false;
    bool complete = false;

    char etx = 0x03; //Ctrl+C, abort without saving
    char eof = 0x04; //Ctrl+D, finished

    while (true) {
        c = getchar();

        if (c == etx) {
            p->CancelVia();
            return false;
        } else if (c == eof) {
            //
            p->EndVia(RunAtStartup);
            break;
        } else {
            if (c == 0x0D) c = 0x0A; //convert line ending

            //StoreViaChunk can take several bytes at once but it buffers
            //based on flash page size so it's fine to do single bytes
            //which is convenient since that's how we're getting them.
            p->StoreViaChunk(&c, 1);
            //gPlatform.IO.Printf("%02x", c);
            putchar(c);
        }
    }

    return true;
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