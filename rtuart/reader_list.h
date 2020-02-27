#pragma once

#include <PCSC/wintypes.h>

#include "reader.h"

Reader* reader_list_alloc_reader(DWORD lun);
Reader* reader_list_get_reader(DWORD lun);
void reader_list_free_reader(DWORD lun);

enum { gReaderListSize = 32 };
