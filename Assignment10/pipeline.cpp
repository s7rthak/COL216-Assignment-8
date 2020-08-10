#include <bits/stdc++.h>
#include <execinfo.h>
#include <unistd.h>
#include "instruction_read.hpp"
#include "memory_read.hpp"
#include "util.hpp"
#include "pipeline.hpp"
typedef long long ll;
using namespace std;

void handler(int sig) {
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

void handleIF(Pipeline& mips, vector<string>& inst_mem){
    PipeStage& IF = mips.instructionFetch;
    cout << "Offset: \n";
    cout << mips.instructionRead << " " << mips.memoryAccess.offset << " " << mips.isStalled << " "  <<  mips.executeInstruction.Jump << " " << mips.memoryAccess.Branch <<  endl;
    if(!mips.isStalled){
        
        IF.PC = mips.instructionRead;
        IF.instruction = inst_mem[IF.PC] + inst_mem[IF.PC+1] + inst_mem[IF.PC+2] + inst_mem[IF.PC+3];
        IF.toDo = true;
     
        mips.instructionRead += 4;
    
        
        
    }else{
        PipeStage p;
        IF = p;
        if(mips.executeInstruction.Jump){
            mips.instructionRead = mips.executeInstruction.offset * 4;
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
    PipeStage& ID = mips.instructionDecode;
    if(ID.toDo){
        int op = stringToDecimal(ID.instruction.substr(0, 6));
        ID.Rs = stringToDecimal(ID.instruction.substr(6, 5));
        ID.Rt = stringToDecimal(ID.instruction.substr(11, 5));
        ID.Rd = stringToDecimal(ID.instruction.substr(16, 5));
        int checkforjr = stringToDecimal(ID.instruction.substr(26,6));
        if(op == 0 && checkforjr == 8){         //jr
            ID.Jump = 1;
            ID.offset = registers[ID.Rs];
        }else if(op == 0){                                    // R-type instruction 
            int func = stringToDecimal(ID.instruction.substr(26, 6));
            // cout << "func: " << func << endl;
            if(func == 32 || func == 34){
                ID.RegisterRs = registers[ID.Rs];
                ID.RegisterRt = registers[ID.Rt];
            }else if(func == 0 || func == 2){
                ID.RegisterRt = registers[ID.Rt];
            }
            ID.RegDst = 1, ID.ALUOp1 = 1, ID.RegWrite = 1;       
        }else if(op == 35 || op == 43){                 // I-type instruction (lw/sw)
            ID.RegisterRs = registers[ID.Rs]; 
            ID.Rd = ID.Rt;
            if(op == 35){                                       // lw
                ID.ALUSrc = 1, ID.MemRead = 1, ID.RegWrite = 1, ID.MemtoReg = 1; mips.checkforvariabledelays = true;
            }else{                                              // sw
                ID.ALUSrc = 1, ID.MemWrite = 1; mips.checkforvariabledelays = true;
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
            ID.RegisterRs = ID.PC;
            ID.RegWrite = 1;
            ID.Rd = 31;
        }
    }
    mips.executeInstruction = mips.instructionDecode;
}

void handleEX(Pipeline& mips){
    PipeStage& EX = mips.executeInstruction;
    PipeStage& MEM = mips.memoryAccess;
    PipeStage& WB = mips.writeBack;
    if(EX.toDo){
        if(mips.ForwardA == 10){
            EX.RegisterRs = MEM.RegisterRs;
        }else if(mips.ForwardA == 01){
            EX.RegisterRs = WB.RegisterRs;
        }
        if(mips.ForwardB == 10){
            EX.RegisterRt = MEM.RegisterRs;
        }else if(mips.ForwardB == 01){
            EX.RegisterRs = WB.RegisterRs;
        }
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
                int shamt = stringToDecimal(EX.instruction.substr(21, 5));;
                EX.RegisterRs = EX.RegisterRt >> shamt;
            }
        }else if(EX.Branch){                                           // Branch
            int op = stringToDecimal(EX.instruction.substr(0, 6));
            if(op == 4){
                if(EX.RegisterRs == EX.RegisterRt){

                    EX.offset = stringToDecimal(EX.instruction.substr(16, 16));
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
    if(MEM.RegWrite && MEM.Rd == EX.Rs && MEM.Rd != 0)                   //handling data hazard
    {
        mips.ForwardA = 10;
    }
    if(MEM.RegWrite && MEM.Rd == EX.Rt && MEM.Rd != 0)
    {
        mips.ForwardB = 10;
    }
    if(WB.RegWrite && WB.Rd == EX.Rs && WB.Rd != 0 && !(MEM.RegWrite && MEM.Rd != 0 && (MEM.Rd != EX.Rs)))
    {
        mips.ForwardA = 01;
    }
    if(WB.RegWrite && WB.Rd == EX.Rt && WB.Rd != 0 && !(MEM.RegWrite && MEM.Rd != 0 && (MEM.Rd != EX.Rt)))
    {
        mips.ForwardB = 01;
    }
    if((EX.Rd == ID.Rs || EX.Rd == ID.Rt) && EX.RegWrite){              // data hazard
        mips.isStalled = true;
    }if(EX.MemRead && (EX.Rd == ID.Rs || EX.Rd == ID.Rt)){              // taking Rs = Rs1 and Rt = Rs2 according to book
        mips.isStalled = true;                                          // data hazard after load needs stalling as it can't be solved by forwarding
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
    }else{
        mips.isStalled = false;
    }

    if(mips.checkforvariabledelays && !mips.variabledelay){
        int n = rand()%10 + 1;
        cout << "Stalling Cycles for Memory Acess: " <<  n << endl;
        double x = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        cout << "Probaility to found in cache(if x>0.5 MISS else HIT): " << x << endl;
        if(x > 0.5){                //miss
            mips.variabledelay = true; mips.lengthforvariabledelay = n-1;
        }
    }
}

int main(int argc, char *argv[]){
    Pipeline MIPS;
    vector<string> instructionMemory = instructionsFileToVector(argv[1]);
    vector<int> registerFile(32);
    vector<int> Memory = memoryFileToVector(argv[2]);
    registerFile[0] = 1;
    registerFile[1] = 2;
    int i=0;
    while(MIPS.instructionRead < (signed)instructionMemory.size()){
        string pipe;
        checkForStall(MIPS);
        if(MIPS.variabledelay && MIPS.lengthforvariabledelay){
            MIPS.isStalled = true;
            MIPS.lengthforvariabledelay--;
        }else{
            MIPS.checkforvariabledelays = false;
            MIPS.variabledelay = false;
        }
        handleWB(MIPS, registerFile);
        pipe = to_string(MIPS.writeBack.PC/4) + " " + pipe;
        handleMEM(MIPS, Memory);
        pipe = to_string(MIPS.memoryAccess.PC/4) + " " + pipe;
        handleEX(MIPS);
        pipe = to_string(MIPS.executeInstruction.PC/4) + " " + pipe;
        handleID(MIPS, registerFile);
        pipe = to_string(MIPS.instructionDecode.PC/4) + " " + pipe;
        handleIF(MIPS, instructionMemory);
        pipe = to_string(MIPS.instructionFetch.PC/4) + " " + pipe;

        for(int i=0; i<registerFile.size(); i++)
        {
            cout << "$" << i << " " << registerFile[i] << endl;
        }
        cout << endl << endl;

        for(int i=0;i<Memory.size(); i++)
        {
            cout << "Memory[" << i << "]: " << Memory[i] << endl;
        }
        cout << "Pipeline:"  << "\n";
        cout << pipe << "\n\n";
        MIPS.clock++;
    }

    // Ending Statements
    for(int i=0; i<4; i++)
    {   
        checkForStall(MIPS);
        if(MIPS.variabledelay && MIPS.lengthforvariabledelay){
            MIPS.lengthforvariabledelay--;
            MIPS.isStalled = true;
        }else{
            MIPS.checkforvariabledelays  =false;
            MIPS.variabledelay = false;
        }
        if(i<4)
            handleWB(MIPS, registerFile);
        if(i<3)
            handleMEM(MIPS, Memory);
        if(i<2)
            handleEX(MIPS);
        if(i < 1)
            handleID(MIPS, registerFile);

        

        for(int i=0; i<registerFile.size(); i++)
        {
            cout << "$" << i << " " << registerFile[i] << endl;
        }
        cout << endl << endl;

        for(int i=0;i<Memory.size(); i++)
        {
            cout << "Memory[" << i << "]: " << Memory[i] << endl;
        }
        cout << "Pipeline:"  << "\n";
        cout << MIPS.instructionFetch.PC/4 << " " << MIPS.instructionDecode.PC/4 << " " << MIPS.executeInstruction.PC/4 << " " << MIPS.memoryAccess.PC/4
         << " " << MIPS.writeBack.PC/4 << endl;
        MIPS.clock++;




    }
}
