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
        int Rd, Rs, Rt;
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

void handleIF(Pipeline& mips, vector<string>& inst_mem){
    PipeStage& IF = mips.instructionFetch;
    if(!mips.isStalled){
        if(mips.executeInstruction.Jump){
            mips.instructionRead = mips.executeInstruction.offset * 4;
        }else if(mips.memoryAccess.Branch){
            mips.instructionRead = mips.memoryAccess.offset * 4 + 4;
        }else{
            mips.instructionRead += 4;
        }
        IF.PC = mips.instructionRead;
        IF.instruction = inst_mem[IF.PC] + inst_mem[IF.PC+1] + inst_mem[IF.PC+2] + inst_mem[IF.PC+3];
        IF.toDo = true;
    }else{
        PipeStage p;
        IF = p;
    }
    mips.instructionDecode = mips.instructionFetch;
}

void handleID(Pipeline& mips, vector<int>& registers){
    PipeStage& ID = mips.instructionDecode;
    if(ID.toDo){
        int op = stringToDecimal(ID.instruction.substr(0, 6));
        ID.Rs = stringToDecimal(ID.instruction.substr(6, 5));
        ID.Rt = stringToDecimal(ID.instruction.substr(11, 5));
        ID.Rd = stringToDecimal(ID.instruction.substr(16, 5));
        if(op == 0){                                    // R-type instruction 
            int func = stringToDecimal(ID.instruction.substr(26, 6));
            if(func == 32 || func == 34){
                ID.RegisterRs = registers[ID.Rs];
                ID.RegisterRt = registers[ID.Rt];
            }else if(func == 0 || func == 2){
                ID.RegisterRt = registers[ID.Rt];
            }
            ID.RegDst = 1, ID.ALUOp1 = 1, ID.RegWrite = 1;       
        }else if(op == 35 || op == 43){                 // I-type instruction (lw/sw)
            ID.RegisterRs = registers[ID.Rs];
            if(op == 35){                                       // lw
                ID.ALUSrc = 1, ID.MemRead = 1, ID.RegWrite = 1, ID.MemtoReg = 1;
            }else{                                              // sw
                ID.ALUSrc = 1, ID.MemWrite = 1;
            }
        }else if(op == 4 || op == 5){
            ID.RegisterRs = registers[ID.Rs];
            ID.RegisterRt = registers[ID.Rt];
            ID.Branch = 1;
        }else if(op == 6 || op == 7){
            ID.RegisterRs = registers[ID.Rs];
            ID.Branch = 1;
        }else if(op == 2){
            ID.Jump = 1;
            ID.offset = stringToDecimal(ID.instruction.substr(6, 26));
        }
    }
    mips.executeInstruction = mips.instructionDecode;
}

void handleEX(Pipeline& mips){
    PipeStage& EX = mips.executeInstruction;
    if(EX.toDo){
        if(EX.ALUOp1 && !EX.ALUOp0){                                    // R-type
            int func = stringToDecimal(EX.instruction.substr(26, 6));
            if(func == 32){
                EX.RegisterRs = EX.RegisterRs + EX.RegisterRt;
            }else if(func == 34){
                EX.RegisterRs = EX.RegisterRs - EX.RegisterRt;
            }else if(func == 0){
                int shamt = stringToDecimal(EX.instruction.substr(21, 5));
                EX.RegisterRs = EX.RegisterRt << shamt;
            }else if(func == 2){
                int shamt = stringToDecimal(EX.instruction.substr(21, 5));
                EX.RegisterRs = EX.RegisterRt >> shamt;
            }
        }else if(!EX.ALUOp1 && !EX.ALUOp0){                             // lw/sw
            int offset = stringToDecimal(EX.instruction.substr(16, 16));
            EX.RegisterRs = EX.RegisterRs + offset;
        }else if(EX.Branch){                                            // Branch
            int op = stringToDecimal(EX.instruction.substr(0, 6));
            if(op == 4){
                if(EX.RegisterRs == EX.RegisterRt){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }
            }else if(op == 5){
                if(EX.RegisterRs != EX.RegisterRt){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }
            }else if(op == 6){
                if(EX.RegisterRs <= 0){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }
            }else if(op == 7){
                if(EX.RegisterRs > 0){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }
            }
        }
    }
    mips.memoryAccess = mips.executeInstruction;
}

void handleMEM(Pipeline& mips, vector<int>& memory){
    PipeStage& MEM = mips.memoryAccess;
    if(MEM.toDo){
        if(MEM.MemRead){
            MEM.RegisterRs = memory[MEM.RegisterRs];
        }else if(MEM.MemWrite){
            memory[MEM.RegisterRs] = MEM.RegisterRt;
        }
    }
    mips.writeBack = mips.memoryAccess;
}

void handleWB(Pipeline& mips, vector<int>& registers){
    PipeStage& WB = mips.writeBack;
    if(WB.toDo){
        if(WB.RegWrite){
            registers[WB.RegisterRd] = WB.RegisterRs;
        }else if(WB.MemtoReg){
            registers[WB.RegisterRt] = WB.RegisterRs;
        }
    }
}

void checkForStall(Pipeline& mips){
    PipeStage& IF = mips.instructionFetch;
    PipeStage& ID = mips.instructionDecode;
    PipeStage& EX = mips.executeInstruction;
    PipeStage& MEM = mips.memoryAccess;
    PipeStage& WB = mips.writeBack;
    int IFop = stringToDecimal(IF.instruction.substr(0, 6));
    int IDop = stringToDecimal(ID.instruction.substr(0, 6));
    int EXop = stringToDecimal(EX.instruction.substr(0, 6));
    if(MEM.Rd == EX.Rs || MEM.Rd == EX.Rt){                             // data hazard
        mips.isStalled = true;
    }else if(WB.Rd == EX.Rs || WB.Rd == EX.Rt){                         // data hazard
        mips.isStalled = true;
    }else if(IFop == 4 || IFop == 5 || IFop == 6 || IFop == 7){         // control hazard
        mips.isStalled = true;
    }else if(IDop == 4 || IDop == 5 || IDop == 6 || IDop == 7){         // control hazard
        mips.isStalled = true;
    }else if(EXop == 4 || EXop == 5 || EXop == 6 || EXop == 7){         // control hazard
        mips.isStalled = true;
    }else if(IFop == 2){                                                // control hazard
        mips.isStalled = true;
    }else if(IDop == 2){                                                // control hazard
        mips.isStalled = true;
    }
}

int main(int argc, char *argv[]){
    Pipeline MIPS;
    vector<string> instructionMemory = instructionsFileToVector(argv[1]);
    vector<int> registerFile(32);
    vector<int> Memory = memoryFileToVector(argv[2]);
    while(MIPS.instructionRead < instructionMemory.size()){
        checkForStall(MIPS);
        handleWB(MIPS, registerFile);
        handleMEM(MIPS, Memory);
        handleEX(MIPS);
        handleID(MIPS, registerFile);
        handleIF(MIPS, instructionMemory);
        MIPS.isStalled = false;
        MIPS.clock++;
    }
}