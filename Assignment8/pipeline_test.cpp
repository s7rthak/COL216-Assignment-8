#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "pipeline.hpp"

using namespace std;

TEST(MIPS, Stall_On_Data_Hazard_1){
    Pipeline P;
    vector<int>registers(32);
    P.instructionDecode.Rs = 1; P.executeInstruction.Rd = 1; P.executeInstruction.RegWrite = 1;
    ASSERT_EQ(stallAtEX(P,registers), true);
}

TEST(MIPS, Stall_On_Data_Hazard_2){
    Pipeline P;
    vector<int>registers(32);
    P.writeBack.Rd = 1; P.instructionDecode.Rs = 1; P.writeBack.RegWrite = 1;
    ASSERT_EQ(stallAtEX(P,registers), true);
}

TEST(MIPS, Stall_On_Data_Hazard_3){
    Pipeline P;
    vector<int>registers(32);
    P.memoryAccess.Rt = 1; P.instructionDecode.Rs = 1; P.memoryAccess.MemtoReg = 1;
    ASSERT_EQ(stallAtEX(P,registers), true);
}

TEST(MIPS, Stall_On_Data_Hazard_4){
    Pipeline P;
    vector<int>registers(32);
    P.instructionDecode.Rt = 1; P.executeInstruction.Rs = 1; P.executeInstruction.MemtoReg = 1;
    ASSERT_EQ(stallAtEX(P,registers), true);
}

TEST(MIPS, Stall_On_Control_Hazard_1){
    Pipeline P;
    P.instructionFetch.instruction = "00010000000000000000000000000000";
    stallAtIF(P);
    ASSERT_EQ(P.isStalled, true);
}

TEST(MIPS, Stall_On_Control_Hazard_2){
    Pipeline P;
    P.instructionDecode.instruction = "00010100000000000000000000000000";
    stallAtIF(P);
    ASSERT_EQ(P.isStalled, true);
}

TEST(MIPS, Stall_On_Control_Hazard_3){
    Pipeline P;
    P.executeInstruction.instruction = "00011000000000000000000000000000";
    stallAtIF(P);
    ASSERT_EQ(P.isStalled, true);
}

TEST(MIPS, Stall_On_Control_Hazard_4){
    Pipeline P;
    P.instructionFetch.instruction = "00001000000000000000000000000000";
    stallAtIF(P);
    ASSERT_EQ(P.isStalled, true);
}

TEST(MIPS, Stall_On_Control_Hazard_5){
    Pipeline P;
    P.instructionDecode.instruction = "00001000000000000000000000000000";
    stallAtIF(P);
    ASSERT_EQ(P.isStalled, true);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
