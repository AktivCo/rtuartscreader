// Copyright (C) 2020, Aktiv-Soft JSC. All rights reserved.
// This file is part of rtuart project licensed under the terms of the 2-clause
// BSD license. See the LICENSE file found in the top-level directory of this
// distribution.

#include <cstdlib>
#include <ctime>

#include <gtest/gtest.h>

#include <fakehardware/fakehardware.h>
#include <faketransport/faketransport.h>

using namespace std;

int main(int argc, char **argv) {
    srand(time(nullptr));
    ::testing::InitGoogleTest(&argc, argv);

    rt::fakehardware::initialize();
    rt::faketransport::initializeTransport();

    auto r = RUN_ALL_TESTS();

    rt::faketransport::deinitializeTransport();
    rt::fakehardware::deinitialize();

    return r;
}