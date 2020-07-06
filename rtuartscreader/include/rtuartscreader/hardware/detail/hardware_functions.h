// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

DEFINE_FUNCTION(hw_status_t, hw_initialize)
DEFINE_FUNCTION(hw_status_t, hw_start_clock, uint32_t)
DEFINE_FUNCTION(hw_status_t, hw_stop_clock)
DEFINE_FUNCTION(hw_status_t, hw_rst_initialize)
DEFINE_FUNCTION(hw_status_t, hw_rst_down)
DEFINE_FUNCTION(hw_status_t, hw_rst_down_up, uint32_t)
DEFINE_FUNCTION(hw_status_t, hw_rst_deinitialize)
DEFINE_FUNCTION(void, hw_deinitialize)
