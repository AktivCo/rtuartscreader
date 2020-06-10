// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/reader_list.h>

#include <stdbool.h>
#include <stddef.h>

#include <rtuartscreader/reader_detail.h>

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
