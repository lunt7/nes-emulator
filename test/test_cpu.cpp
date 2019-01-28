#include "Nes.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

// class CpuTest : public testing::Test {
//     protected:
//         void SetUp(void) override {

//         }



//         //Nes nes2 = nes;
// };

// TEST_F(CpuTest, nestest) {

//     EXPECT_TRUE(true);
// }

TEST(CpuTest, nestest) {
    Nes nes;

    nes.PowerOn();
    // nes.Run("Donkey Kong (World) (Rev A).nes");
    nes.Run("nestest.nes", Nes::EMU_MODE_AUTOMATED);
    EXPECT_TRUE(true);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
