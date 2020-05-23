// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <rtuartscreader/log/init.h>

#include <stdbool.h>
#include <stdlib.h>

#include <dlfcn.h>

#include <PCSC/debuglog.h>
#include <PCSC/ifdhandler.h>

#include <log/log.h>

int log_convert_to_priority(log_level_t logLevel) {
    switch (logLevel) {
    case LOG_LEVEL_CRITICAL: return PCSC_LOG_CRITICAL;
    case LOG_LEVEL_ERROR: return PCSC_LOG_ERROR;
    case LOG_LEVEL_INFO: return PCSC_LOG_INFO;
    case LOG_LEVEL_DEBUG: return PCSC_LOG_DEBUG;
    default: return PCSC_LOG_DEBUG;
    }
}

static const log_level_t kLogLevelAll = (LOG_LEVEL_CRITICAL | LOG_LEVEL_ERROR | LOG_LEVEL_INFO | LOG_LEVEL_DEBUG);

static bool gLogIsInitialized = false;

void init_log() {
    if (gLogIsInitialized) return;
    gLogIsInitialized = true;

    log_level_t logLevel = LOG_LEVEL_CRITICAL | LOG_LEVEL_ERROR;

    void* logFunction = dlsym((void*)0, "log_msg");
    if (!logFunction) {
        return; // sorry, no log
    }

    char* logLevelString = getenv("LIBRTUARTSCREADER_ifdLogLevel");
    if (logLevelString) {
        char* endPtr;
        unsigned long logLevelLong = strtoul(logLevelString, &endPtr, 0);
        if (logLevelString != endPtr) {
            logLevel = logLevelLong & kLogLevelAll;
        }
    }

    log_init(logLevel, logFunction, log_convert_to_priority);
}