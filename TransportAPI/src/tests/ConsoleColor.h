// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#pragma once

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define COLOR_NORMAL KNRM
#define COLOR_OK KGRN
#define COLOR_ERROR KRED
#define COLOR_WARNING KYEL
#define COLOR_INFO KNRM

#define LOG_OK 0
#define LOG_ERROR 1
#define LOG_WARNING 2
#define LOG_INFO 3

void log_log(const char* color, const char* fmt, ...);

void log_xxd(const char* color, const char* msg, const unsigned char* buffer, int len);

#define log_success(...) log_log(COLOR_OK, __VA_ARGS__)
#define log_error(...) log_log(COLOR_ERROR, __VA_ARGS__)
#define log_warning(...) log_log(COLOR_WARNING, __VA_ARGS__)
#define log_info(...) log_log(COLOR_INFO, __VA_ARGS__)
#define log_buf(msg, buffer, size) log_xxd(COLOR_INFO, msg, buffer, size)
