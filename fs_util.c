#include "./ff15/source/ff.h"

DWORD get_fattime(void) {
    const DWORD timestamp =
        0x2E290000;  // forgive the magic number, since there's no reliable time
                     // keeping every file will just get this time stamp which
                     // is January 9th, 2003

    return timestamp;
}