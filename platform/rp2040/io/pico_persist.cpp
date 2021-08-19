/*  Contains methods for persisting VIA source to be loaded after boot.
    Function definitions exist in Platform.h
*/
#include <hardware/flash.h>
#include <hardware/sync.h>

//Get Platform.h from DataTypes.h 
#include "DataTypes.h"

//Info starts at 1MB into flash memory
#define PICOG_VIA_INFO_OFFSET() ((uint32_t)0x100000)

//Start the source one sector after the info
#define PICOG_VIA_SRC_OFFSET() (PICOG_VIA_INFO_OFFSET() + FLASH_SECTOR_SIZE)

//These macros retrieve the actual data accessible to program space
#define PICOG_VIA_INFO() ((PersistedViaInfo *)(PICOG_VIA_INFO_OFFSET() + XIP_BASE))
#define PICOG_VIA_SRC() ((char *)(PICOG_VIA_SRC_OFFSET() + XIP_BASE))

#define PAGES_PER_SECTOR() (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE)

struct FlashInfo {
    Vireo::PersistedViaInfo info;
    char *viaStart;

    bool started;

    uint8_t pageBuf[FLASH_PAGE_SIZE];
    int pageLen;

    uint32_t curOffset; //Where to write to or offset of sector to erase

    int8_t sectorPage; //# of pages left in current sector. Used to know when to erase sectors

};

static FlashInfo flash;

void writePage() {
    //save interrupt config and disable to not interfere with flash ops
    uint32_t ints = save_and_disable_interrupts();

    if (flash.sectorPage <= 0) {
        flash_range_erase(flash.curOffset, FLASH_SECTOR_SIZE);
        flash.sectorPage = PAGES_PER_SECTOR();
    }

    flash_range_program(flash.curOffset, flash.pageBuf, FLASH_PAGE_SIZE);
    restore_interrupts(ints);

    flash.pageLen = 0;
    flash.curOffset += FLASH_PAGE_SIZE;
    flash.sectorPage--;
}

void writeInfo() {
    //Copy info into buffer before writing to flash
    memcpy(flash.pageBuf, &(flash.info), sizeof(Vireo::PersistedViaInfo));

    //save interrupt config and disable to not interfere with flash ops
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(PICOG_VIA_INFO_OFFSET(), flash.pageBuf, FLASH_PAGE_SIZE);
    restore_interrupts(ints);
}

namespace Vireo {

PlatformPersist::PlatformPersist() {
    flash.info = *PICOG_VIA_INFO();
    flash.viaStart = (char *)PICOG_VIA_SRC();

    flash.started = false;
}

bool PlatformPersist::LoadVia(PersistedVia *via) {
    via->source = PICOG_VIA_SRC();
    via->info = flash.info;

    return HasVia();
}

bool PlatformPersist::HasVia() {
    return ((flash.info.flags & PersistCfg) == 0) &&
           ((flash.info.flags & StoredVia) > 0);
}

bool PlatformPersist::HasStartup() {
    return ((flash.info.flags & PersistCfg) == 0) &&
           ((flash.info.flags & StoredVia) > 0) &&
           ((flash.info.flags & RunAtStartup) > 0);
}

uint8_t PlatformPersist::ClearVia() {
    flash.started = false;

    //only need to clear flags if set
    //flags & PersistCfg will be nonzero if flash sector is already erased
    if (((flash.info.flags & PersistCfg) == 0) && ((flash.info.flags & StoredVia) > 0)) {
        //save interrupt config and disable to not interfere with flash ops
        uint32_t ints = save_and_disable_interrupts();

        //clear flash data configured with info
        flash_range_erase(PICOG_VIA_INFO_OFFSET(), FLASH_SECTOR_SIZE);

        restore_interrupts(ints);
    }

    return 0;
}

uint8_t PlatformPersist::StartVia() {
    //Make sure flags/info cleared until EndVia is called
    ClearVia();

    flash.sectorPage = 0;
    flash.pageLen = 0;
    flash.curOffset = PICOG_VIA_SRC_OFFSET();

    flash.started = true;

    return 0;
}

uint8_t PlatformPersist::CancelVia() {
    flash.started = false;

    return 0;
}

uint8_t PlatformPersist::StoreViaChunk(char *buf, int len) {
    if (!flash.started) return 1;

    for (int i = 0; i < len; ++i) {
        flash.pageBuf[flash.pageLen] = buf[i];
        flash.pageLen++;
        
        if (flash.pageLen == FLASH_PAGE_SIZE) {
            writePage();
        }
    }

    return 0;
}

char * PlatformPersist::CStr() {
    return PICOG_VIA_SRC();
}

uint8_t PlatformPersist::EndVia(bool runAtStartup) {
    if (!flash.started) return 1;

    //Always set the StoredVia flag which denotes successful save
    uint8_t flags = StoredVia;

    //Add additional flags as configured
    if (runAtStartup) flags |= RunAtStartup;

    //terminate with nullchar
    char nc = 0;
    StoreViaChunk(&nc, 1);

    //determine how long the via source is.
    //Writing any remaining buffer will muck with pageLen and curOffset
    //in a way that won't reflect the via length so we calc the len first
    int viaLen = (flash.curOffset + flash.pageLen) - PICOG_VIA_SRC_OFFSET();

    //if pageLen > 0, finish writing buffer
    if (flash.pageLen > 0) {
        writePage();
    }

    flash.info.flags = flags;
    flash.info.len = viaLen;

    writeInfo();

    return 0;
}

} //namespace Vireo
