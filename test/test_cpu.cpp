#include "Nes.h"
#include "Logging.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

TEST(CpuTest, nestest) {
    string ref, uut, word;
    ifstream ref_log("../../test/nestest-bus-cycles.log");
    ifstream uut_log("test_nestest.log");

    LOG_INIT("test_nestest.log");

    Nes nes;
    nes.PowerOn();
    nes.Run("../../roms/nestest.nes", Nes::EMU_MODE_AUTOMATED);

    while (getline(ref_log, ref)) {
        istringstream iss(ref);
        iss >> word;
        if (word != "READ" && word != "WRITE" && getline(uut_log, uut)) {
            int ref_CPUC = stoi(ref.substr(79), nullptr);

            string ref_PC = ref.substr(0, 4);
            string uut_PC = uut.substr(0, 4);
            ASSERT_EQ(uut_PC, ref_PC) << "@ CPUC: " << ref_CPUC;

            string ref_opcode = ref.substr(16, 3);
            string uut_opcode = uut.substr(16, 3);
            ASSERT_EQ(uut_opcode, ref_opcode) << "@ CPUC: " << ref_CPUC;

            int ref_A = stoi(ref.substr(50, 2), nullptr, 16);
            int uut_A = stoi(uut.substr(25, 2), nullptr, 16);
            ASSERT_EQ(uut_A, ref_A) << "@ CPUC: " << ref_CPUC;

            int ref_X = stoi(ref.substr(55, 2), nullptr, 16);
            int uut_X = stoi(uut.substr(30, 2), nullptr, 16);
            ASSERT_EQ(uut_X, ref_X) << "@ CPUC: " << ref_CPUC;

            int ref_Y = stoi(ref.substr(60, 2), nullptr, 16);
            int uut_Y = stoi(uut.substr(35, 2), nullptr, 16);
            ASSERT_EQ(uut_Y, ref_Y) << "@ CPUC: " << ref_CPUC;

            int ref_P = stoi(ref.substr(65, 2), nullptr, 16);
            int uut_P = stoi(uut.substr(40, 2), nullptr, 16);
            ASSERT_EQ(uut_P, ref_P) << "@ CPUC: " << ref_CPUC;

            int ref_SP = stoi(ref.substr(71, 2), nullptr, 16);
            int uut_SP = stoi(uut.substr(46, 2), nullptr, 16);
            ASSERT_EQ(uut_SP, ref_SP) << "@ CPUC: " << ref_CPUC;

            int uut_CPUC = stoi(uut.substr(54), nullptr);
            ASSERT_EQ(uut_CPUC, ref_CPUC) << "@ CPUC: " << ref_CPUC;
        }
    }
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
