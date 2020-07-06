// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

DEFINE_FUNCTION(transport_status_t, transport_initialize, transport_t*, const char*)
DEFINE_FUNCTION(transport_status_t, transport_reinitialize, transport_t*, const transmit_params_t*)
DEFINE_FUNCTION(transport_status_t, transport_deinitialize, const transport_t*)
