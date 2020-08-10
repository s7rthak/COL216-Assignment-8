#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "pipeline.hpp"

using namespace std;

TEST(MIPS, Stall_On_Data_Hazard_1){
    Pipeline P;
    P.instructionDecode.Rs = 1; P.executeInstruction.Rd = 1; P.executeInstruction.RegWrite = 1;
    checkForStall(P);
    ASSERT_EQ(P.isStalled, true);
}

TEST(MIPS, Stall_On_Data_Hazard_2){
    Pipeline P;
    P.writeBack.Rd = 1; P.executeInstruction.Rs = 1; P.writeBack.RegWrite = 1;
    checkForStall(P);
    ASSERT_EQ(P.isStalled, true);
}

TEST(MIPS, Stall_On_Data_Hazard_3){
    Pipeline P;
    P.writeBack.Rt = 1; P.executeInstruction.Rs = 1; P.writeBack.MemtoReg = 1;
    checkForStall(P);
    ASSERT_EQ(P.isStalled, true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
