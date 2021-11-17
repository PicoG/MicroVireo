// Copyright (c) 2020 National Instruments
// SPDX-License-Identifier: MIT

/*! \file
 */

#ifndef Platform_h
#define Platform_h

#include "DataTypes.h"

#ifdef __rp2040__
#include "stdio.h"
#endif

#if kVireoOS_emscripten
#define rintf RINTF_UNDEFINED  // don't use rintf implementation on emscripten; it doesn't obey rounding modes correctly
#define EMSCRIPTEN_NOOPT __attribute__((optnone))  // allow disabling optimizations that expose bugs in
// emscripten libs, such as those that cause Double casts to be elided and a rint call replaced with rintf
#else
#define EMSCRIPTEN_NOOPT
#endif

namespace Vireo {

class SubString;
class String;
typedef String *StringRef;

//------------------------------------------------------------
//! Process level functions for memory allocation.
class PlatformMemory {
 private:
    size_t _totalAllocated = 0;
 public:
    void* Malloc(size_t countAQ);
    void* Realloc(void* pBuffer, size_t countAQ);
    void Free(void* pBuffer);
    size_t TotalAllocated() const { return _totalAllocated; }
};

// #define VIREO_TRACK_MALLOC

#if defined(VIREO_TRACK_MALLOC)
  #define LOG_PLATFORM_MEM(message)    gPlatform.IO.Printf(message " %d\n", (int)gPlatform.Mem.TotalAllocated());
#else
  #define LOG_PLATFORM_MEM(message)
#endif

#define CMD_UNKNOWN       0x00

//Device info commands
#define CMD_VERSION       0x01 // picoG Firmware version
#define CMD_PLATFORM      0x02 // Platform string
#define CMD_BOARD         0x03 // Platform variant
#define CMD_SERIAL        0x04 // Device unique identifier, variable length per platform
#define CMD_ALIAS         0x05 // User-defined name for the device

//Execution interaction commands
#define CMD_ISEXEC        0x06 // 'T' if currently executing, 'F' otherwise
#define CMD_RESET         0x07 // Reset type manager scope, will stop execution
#define CMD_ABORT         0x08 // Stops current execution
#define CMD_RUNMAIN       0x09 // Runs 'main' VI definition

#define CMD_SKIPSTARTUP   0x0A // Intended to be sent at connection before engine runs flash stored app

#if 1 //VIREO_VIA_PERSIST
struct PersistedViaInfo {
    uint8_t flags;  //PersistedViaFlags
    int len;        //Length of Via source
    char alias[64]; 
};

struct PersistedVia {
    PersistedViaInfo info;
    char *source;
};

//MSB is platform specific and used to know if configured
//IE Pi Pico must be 0 for flags to be valid since
//erasing a sector on Pico will read back all 1s
enum PersistedViaFlags {
    StoredVia       = 0x01,
    RunAtStartup    = 0x02,
    NA1             = 0x04,
    NA2             = 0x08,
    NA3             = 0x10,
    NA4             = 0x20,
    NA5             = 0x40,
    PersistCfg      = 0x80
};

class PlatformPersist {
public:
    PlatformPersist();

    bool LoadVia(PersistedVia *via);

    uint8_t ClearVia();
    
    uint8_t StartVia();
    uint8_t StoreViaChunk(char *start, int len);
    uint8_t CancelVia();
    uint8_t EndVia(bool runAtStartup);

    bool SetAlias(const Utf8Char *begin, const Utf8Char *end);
    void ClearAlias();
    const char * GetAlias();

    bool HasVia();
    bool HasStartup();

    char * CStr();

private:

    PersistedViaInfo _info; //Flags and meta for persisted Via source
};
#endif

//------------------------------------------------------------
//! Process level functions for stdio.
class PlatformIO {
 public:
    PlatformIO();
    void Print(Int32 len, ConstCStr str);
    void Print(ConstCStr str);
    void Print(char c);
    void Printf(ConstCStr format, ...) const;
    void ReadFile(SubString *name, StringRef buffer);
    void ReadStdin(StringRef buffer);
    void InitStatusLED();
    void StatusLED(bool val);
    uint8_t checkCommand();

private:
    char _cmd[50];
    int _cmdLen;
    int _cmdMatch;
    bool _readCmd;
    bool _unreadCmd;
    int _unreadI;

    //returns true if a command was intercepted and fgetc needs to be repeated
    char _fgetc(FILE *file);
    int _getchar_timeout_us(uint32_t timeout_us);
    void resetCmd();
};

//------------------------------------------------------------
//! Process level functions for low level time information.
#if defined (__ARDUINO__)
    typedef UInt32 PlatformTickType;
#elif kVireoOS_emscripten
    typedef Int64 PlatformTickType;
#else
    typedef Int64 PlatformTickType;
#endif

class PlatformTimer {
 public:
    static PlatformTickType TickCount();
    static PlatformTickType MicrosecondsToTickCount(Int64 microseconds);
    static Int64 TickCountToMilliseconds(PlatformTickType);
    static Int64 TickCountToMicroseconds(PlatformTickType);
    static PlatformTickType MillisecondsFromNowToTickCount(Int64 millisecondCount);
    static PlatformTickType MicrosecondsFromNowToTickCount(Int64 microsecondCount);
#if !kVireoOS_emscripten
    static void SleepMilliseconds(Int64 milliseconds);  // Cannot sleep in emscripten code without using interpreter, must sleep in caller on JS side
#endif
};

//------------------------------------------------------------
//! Single class to gather platform classes.
class Platform {
 public:
    static void Setup();
    static void Shutdown();
 public:
    PlatformMemory  Mem;
    PlatformIO      IO;
    PlatformTimer   Timer;

#if 1 //VIREO_VIA_PERSIST
    PlatformPersist Persist;
#endif
};

extern Platform gPlatform;

}  // namespace Vireo

#endif  // Platform_h
