// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

DEFINE_FUNCTION(transport_status_t, transport_recv_byte, const transport_t*, uint8_t*)
DEFINE_FUNCTION(transport_status_t, transport_send_byte, const transport_t*, uint8_t)
DEFINE_FUNCTION(transport_status_t, transport_recv_bytes, const transport_t*, uint8_t*, size_t)
DEFINE_FUNCTION(transport_status_t, transport_send_bytes, const transport_t*, const uint8_t*, size_t)
