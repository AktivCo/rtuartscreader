// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#include <PCSC/wintypes.h>

#include <rtuartscreader/reader.h>

Reader* reader_list_alloc_reader(DWORD lun);
Reader* reader_list_get_reader(DWORD lun);
void reader_list_free_reader(DWORD lun);

enum { gReaderListSize = 32 };
