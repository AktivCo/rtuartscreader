#include "log.h"

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

    char* logLevelString = getenv("LIBRTUART_ifdLogLevel");
    if (logLevelString) {
        char* endPtr;
        unsigned long logLevelLong = strtoul(logLevelString, &endPtr, 0);
        if (logLevelString != endPtr) {
            logLevel = logLevelLong & kLogLevelAll;
        }
    }

    log_init(logLevel, logFunction, log_convert_to_priority);
}

const char* ifd_error_to_string(int error) {
    switch (error) {
    case IFD_SUCCESS: return "IFD_SUCCESS";
    case IFD_ERROR_TAG: return "IFD_ERROR_TAG";
    case IFD_ERROR_SET_FAILURE: return "IFD_ERROR_SET_FAILURE";
    case IFD_ERROR_VALUE_READ_ONLY: return "IFD_ERROR_VALUE_READ_ONLY";
    case IFD_ERROR_PTS_FAILURE: return "IFD_ERROR_PTS_FAILURE";
    case IFD_ERROR_NOT_SUPPORTED: return "IFD_ERROR_NOT_SUPPORTED";
    case IFD_PROTOCOL_NOT_SUPPORTED: return "IFD_PROTOCOL_NOT_SUPPORTED";
    case IFD_ERROR_POWER_ACTION: return "IFD_ERROR_POWER_ACTION";
    case IFD_ERROR_SWALLOW: return "IFD_ERROR_SWALLOW";
    case IFD_ERROR_EJECT: return "IFD_ERROR_EJECT";
    case IFD_ERROR_CONFISCATE: return "IFD_ERROR_CONFISCATE";
    case IFD_COMMUNICATION_ERROR: return "IFD_COMMUNICATION_ERROR";
    case IFD_RESPONSE_TIMEOUT: return "IFD_RESPONSE_TIMEOUT";
    case IFD_NOT_SUPPORTED: return "IFD_NOT_SUPPORTED";
    case IFD_ICC_PRESENT: return "IFD_ICC_PRESENT";
    case IFD_ICC_NOT_PRESENT: return "IFD_ICC_NOT_PRESENT";
    case IFD_NO_SUCH_DEVICE: return "IFD_NO_SUCH_DEVICE";
    case IFD_ERROR_INSUFFICIENT_BUFFER: return "IFD_ERROR_INSUFFICIENT_BUFFER";
    default: return "unknown";
    }
}
