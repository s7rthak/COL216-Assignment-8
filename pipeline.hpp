#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <bits/stdc++.h>
using namespace std;

class PipeStage{
    public:
        bool RegDst = 0, ALUOp1 = 0, ALUOp0 = 0, ALUSrc = 0, Jump = 0, Branch = 0, MemRead = 0, MemWrite = 0, RegWrite = 0, MemtoReg = 0;
        int RegisterRd, RegisterRs, RegisterRt;
        int Rd = -2, Rs = -1, Rt = -1;
        int PC = -4;
        string instruction = "11111111111111111111111111111111";
        int WriteBack;
        int writeMem, readMem;
        bool toDo = 0;
        int offset = 0;
};

class Pipeline{
    public:   
        PipeStage instructionFetch, instructionDecode, executeInstruction, memoryAccess, writeBack;
        int clock = 0;
        int instructionRead = 0;
        bool isStalled = 0;
};

void handleIF(Pipeline& mips, vector<string>& inst_mem);
void handleID(Pipeline& mips, vector<int>& registers);
void handleEX(Pipeline& mips);
void handleMEM(Pipeline& mips, vector<int>& memory);
void handleWB(Pipeline& mips, vector<int>& registers);
void checkForStall(Pipeline& mips);

#endif
