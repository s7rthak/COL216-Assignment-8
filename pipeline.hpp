#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include <bits/stdc++.h>
using namespace std;

class PipeStage{
    public:
        bool RegDst, ALUOp1, ALUOp0, ALUSrc, Jump, Branch, MemRead, MemWrite, RegWrite, MemtoReg;
        int RegisterRd, RegisterRs, RegisterRt;
        int Rd = -2, Rs = -1, Rt = -1;
        int PC;
        string instruction;
        int WriteBack;
        int writeMem, readMem;
        bool toDo;
        int offset = 0;
};

class Pipeline{
    public:   
        PipeStage instructionFetch, instructionDecode, executeInstruction, memoryAccess, writeBack;
        int clock = 0;
        int instructionRead = -4;
        bool isStalled;
};

void handleIF(Pipeline& mips, vector<string>& inst_mem);
void handleID(Pipeline& mips, vector<int>& registers);
void handleEX(Pipeline& mips);
void handleMEM(Pipeline& mips, vector<int>& memory);
void handleWB(Pipeline& mips, vector<int>& registers);
void checkForStall(Pipeline& mips);

#endif
