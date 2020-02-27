#include "reader_list.h"

#include <stdbool.h>
#include <stddef.h>

#include "reader_detail.h"

#define arraysize(array) (sizeof(array) / sizeof(array[0]))

typedef struct {
    bool initialized;
    DWORD lun;
    Reader reader;
} ReaderEntry;

static const ReaderEntry kEmptyReaderEntry;

static ReaderEntry gReaderList[gReaderListSize];

Reader* reader_list_alloc_reader(DWORD lun) {
    size_t i;

    for (i = 0; i < arraysize(gReaderList); ++i) {
        if (!gReaderList[i].initialized) {
            gReaderList[i].lun = lun;
            gReaderList[i].initialized = true;
            return &(gReaderList[i].reader);
        }
    }

    return NULL;
}

Reader* reader_list_get_reader(DWORD lun) {
    size_t i;

    for (i = 0; i < arraysize(gReaderList); ++i) {
        if (gReaderList[i].initialized && gReaderList[i].lun == lun) {
            return &(gReaderList[i].reader);
        }
    }

    return NULL;
}

void reader_list_free_reader(DWORD lun) {
    size_t i;

    for (i = 0; i < arraysize(gReaderList); ++i) {
        if (gReaderList[i].initialized && gReaderList[i].lun == lun) {
            gReaderList[i] = kEmptyReaderEntry;
            return;
        }
    }
}