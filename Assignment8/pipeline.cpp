#include "pipeline.hpp"
#include "util.hpp"


void handleIF(Pipeline& mips, vector<string>& inst_mem){
    PipeStage& IF = mips.instructionFetch;
    if(!mips.isStalled){
        
        IF.PC = mips.instructionRead;
        IF.instruction = inst_mem[IF.PC] + inst_mem[IF.PC+1] + inst_mem[IF.PC+2] + inst_mem[IF.PC+3];
        IF.toDo = true;
     
        mips.instructionRead += 4;
        mips.instructions++;
    }else{
        PipeStage p;
        IF = p;
        if(mips.executeInstruction.Jump){
            if(mips.executeInstruction.RegisterRs == -1){
                mips.instructionRead = mips.executeInstruction.offset * 4;
            }else{
                mips.instructionRead = mips.executeInstruction.RegisterRs;
            }
        }else if(mips.memoryAccess.Branch){
            mips.instructionRead = mips.memoryAccess.offset * 4;
        }
        if(mips.checkstallincr){
            mips.instructionRead += 4;
        }
        mips.checkstallincr = false;
    }
    mips.instructionDecode = mips.instructionFetch;
}

void handleID(Pipeline& mips, vector<int>& registers){
    // cout << "ID: \n";
    PipeStage& ID = mips.instructionDecode;
    if(ID.toDo){
        int op = stringToDecimal(ID.instruction.substr(0, 6));
        ID.Rs = stringToDecimal(ID.instruction.substr(6, 5));
        ID.Rt = stringToDecimal(ID.instruction.substr(11, 5));
        ID.Rd = stringToDecimal(ID.instruction.substr(16, 5));
        int checkforjr = stringToDecimal(ID.instruction.substr(26,6));
        if(op == 0 && checkforjr == 8){         //jr
            ID.Jump = 1;
            ID.RegisterRs = registers[31];
        }else if(op == 0){                                    // R-type instruction 
            int func = stringToDecimal(ID.instruction.substr(26, 6));
            if(func == 32 || func == 34){
                ID.RegisterRs = registers[ID.Rs];
                ID.RegisterRt = registers[ID.Rt];
                // cout << "Register values: " <<  ID.RegisterRs << " " << ID.RegisterRt << endl;
            }else if(func == 0 || func == 2){
                ID.RegisterRt = registers[ID.Rt];
            }
            ID.RegDst = 1, ID.ALUOp1 = 1, ID.RegWrite = 1;       
        }else if(op == 35 || op == 43){                 // I-type instruction (lw/sw)
            ID.RegisterRs = registers[ID.Rs]; 
            ID.Rd = ID.Rt;
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
        }else if(op == 3){              //jal
            ID.Jump = 1;
            ID.offset = stringToDecimal(ID.instruction.substr(6, 26));
            registers[31] = ID.PC + 4;
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
        }else if(EX.Branch){                                           // Branch
            int op = stringToDecimal(EX.instruction.substr(0, 6));
            if(op == 4){
                if(EX.RegisterRs == EX.RegisterRt){

                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }else{
                    mips.checkstallincr = true;
                }
            }else if(op == 5){
                if(EX.RegisterRs != EX.RegisterRt){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }else{
                     mips.checkstallincr = true;
                }
            }else if(op == 6){
                if(EX.RegisterRs <= 0){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }else{
                     mips.checkstallincr = true;
                }
            }else if(op == 7){
                if(EX.RegisterRs > 0){
                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
                }else{
                     mips.checkstallincr = true;
                }
            }
        }else if(!EX.ALUOp1 && !EX.ALUOp0){                             // lw/sw
            int offset = stringToDecimal(EX.instruction.substr(16, 16));
            EX.RegisterRs = EX.RegisterRs + offset;
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
    // cout << "WB: \n";
    PipeStage& WB = mips.writeBack;
    if(WB.toDo){
        if(WB.RegWrite){
            registers[WB.Rd] = WB.RegisterRs;
        }else if(WB.MemtoReg){
            registers[WB.Rd] = WB.RegisterRs;
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
    int IFfunc = stringToDecimal(IF.instruction.substr(26, 6));
    int IDfunc = stringToDecimal(ID.instruction.substr(26, 6));
    if((EX.Rd == ID.Rs || EX.Rd == ID.Rt) && EX.RegWrite){              // data hazard
        mips.isStalled = true;
    }else if((WB.Rd == EX.Rs || WB.Rd == EX.Rt) && WB.RegWrite){        // data hazard
        mips.isStalled = true;
    }else if(WB.Rt == EX.Rs && WB.MemtoReg){                            // data hazard
        mips.isStalled = true;
    }else if(IFop == 4 || IFop == 5 || IFop == 6 || IFop == 7){         // control hazard
        mips.isStalled = true;
    }else if(IDop == 4 || IDop == 5 || IDop == 6 || IDop == 7){         // control hazard
        mips.isStalled = true;
    }else if(EXop == 4 || EXop == 5 || EXop == 6 || EXop == 7){         // control hazard
        mips.isStalled = true;
    }else if(IFop == 2 || IFop == 3 || IFfunc == 8){                    // control hazard
        mips.isStalled = true;
    }else if(IDop == 2 || IDop == 3 || IDfunc == 8){                    // control hazard
        mips.isStalled = true;
    }else{
        mips.isStalled = false;
    }
}
