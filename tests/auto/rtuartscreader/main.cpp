#include <cstdlib>
#include <ctime>

#include <gtest/gtest.h>

#include <faketransport/faketransport.h>

using namespace std;

int main(int argc, char **argv) {
    srand(time(nullptr));
    ::testing::InitGoogleTest(&argc, argv);

    rt::faketransport::initializeTransport();

    auto r = RUN_ALL_TESTS();

    rt::faketransport::deinitializeTransport();

    return r;
}