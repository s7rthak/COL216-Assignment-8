#include <bits/stdc++.h>
#include "instruction_read.h"
#include "memory_read.h"
#include "util.h"
typedef long long ll;
using namespace std;

class PipeStage{
    public:
        bool RegDst, ALUOp1, ALUOp0, ALUSrc, Jump, Branch, MemRead, MemWrite, RegWrite, MemtoReg;
        int RegisterRd, RegisterRs, RegisterRt;
        int PC;
        string instruction;
        int WriteBack;
        int writeMem, readMem;
};

class Pipeline{
    public:   
        PipeStage instructionFetch, instructionDecode, executeInstruction, memoryAccess, writeBack;
        int clock = 0;
        int instructionRead = -4;
};

void handleIF(Pipeline& mips, vector<string>& inst_mem){
    mips.instructionRead += 4;
    PipeStage& IF = mips.instructionFetch;
    IF.PC = mips.instructionRead;
    IF.instruction = inst_mem[IF.PC] + inst_mem[IF.PC+1] + inst_mem[IF.PC+2] + inst_mem[IF.PC+3];
    mips.instructionDecode = mips.instructionFetch;
}

void handleID(Pipeline& mips, vector<int>& registers){
    PipeStage& ID = mips.instructionDecode;
    int op = stringToDecimal(ID.instruction.substr(0, 6));
    ID.RegisterRs = stringToDecimal(ID.instruction.substr(6, 5));
    ID.RegisterRt = stringToDecimal(ID.instruction.substr(11, 5));
    ID.RegisterRd = stringToDecimal(ID.instruction.substr(16, 5));
    if(op == 0){                                    // R-type instruction 
        int func = stringToDecimal(ID.instruction.substr(26, 6));
        if(func == 32 || func == 34){
            ID.RegisterRs = registers[stringToDecimal(ID.instruction.substr(6, 5))];
            ID.RegisterRt = registers[stringToDecimal(ID.instruction.substr(11, 5))];
        }else if(func == 0 || func == 2){
            ID.RegisterRt = registers[stringToDecimal(ID.instruction.substr(11, 5))];
        }
        ID.RegDst = 1, ID.ALUOp1 = 1, ID.RegWrite = 1;       
    }else if(op == 35 || op == 43){                 // I-type instruction (lw/sw)
        ID.RegisterRs = registers[stringToDecimal(ID.instruction.substr(6, 5))];
        if(op == 35){                                       // lw
            ID.ALUSrc = 1, ID.MemRead = 1, ID.RegWrite = 1, ID.MemtoReg = 1;
        }else{                                              // sw
            ID.ALUSrc = 1, ID.MemWrite = 1;
        }
    }
    mips.executeInstruction = mips.instructionDecode;
}

void handleEX(Pipeline& mips){
    PipeStage& EX = mips.executeInstruction;
    if(EX.ALUOp1 && !EX.ALUOp0){                                    // R-type
        int func = stringToDecimal(EX.instruction.substr(26, 6));
        if(func == 32){
            EX.RegisterRs = EX.RegisterRs + EX.RegisterRt;
        }else if(func == 34){
            EX.RegisterRs = EX.RegisterRs - EX.RegisterRt;
        }else if(func == 0){
            int shamt = stringToDecimal(EX.instruction.substr(21, 5));
            EX.RegisterRt = EX.RegisterRt << shamt;
        }else if(func == 2){
            int shamt = stringToDecimal(EX.instruction.substr(21, 5));
            EX.RegisterRt = EX.RegisterRt >> shamt;
        }
    }else if(!EX.ALUOp1 && !EX.ALUOp0){                             // lw/sw
        int offset = stringToDecimal(EX.instruction.substr(16, 16));
        EX.RegisterRs = EX.RegisterRs + offset;
    }
    mips.memoryAccess = mips.executeInstruction;
}

void handleMEM(Pipeline& mips, vector<int>& memory){
    PipeStage& MEM = mips.memoryAccess;
    if(MEM.MemRead){
        MEM.RegisterRs = memory[MEM.RegisterRs];
    }else if(MEM.MemWrite){
        memory[MEM.RegisterRs] = MEM.RegisterRt;
    }
    mips.writeBack = mips.memoryAccess;
}

void handleWB(Pipeline& mips, vector<int>& registers){
    PipeStage& WB = mips.writeBack;
    if(WB.MemtoReg){
        registers[WB.RegisterRt] = WB.RegisterRs;
    }
}

int main(int argc, char *argv[]){
    Pipeline MIPS;
    vector<string> instructionMemory = instructionsFileToVector(argv[1]);
    vector<int> registerFile(32);
    vector<int> Memory = memoryFileToVector(argv[2]);
    while(MIPS.instructionRead < instructionMemory.size()){
        if(MIPS.clock > 3){
            handleWB(MIPS, registerFile);
        }
        if(MIPS.clock > 2){
            handleMEM(MIPS, Memory);
        }
        if(MIPS.clock > 1){
            handleEX(MIPS);
        }
        if(MIPS.clock > 0){
            handleID(MIPS, registerFile);
        }
        handleIF(MIPS, instructionMemory);
        MIPS.clock++;
    }
}