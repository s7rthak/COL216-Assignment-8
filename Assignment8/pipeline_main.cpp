#include <bits/stdc++.h>
#include <execinfo.h>
#include <unistd.h>
#include "instruction_read.hpp"
#include "memory_read.hpp"
#include "pipeline.hpp"

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

int main(int argc, char *argv[]){
    // signal(SIGSEGV, handler);
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
        string pipe;
        if(i<4)
            handleWB(MIPS, registerFile);
        pipe = to_string(MIPS.writeBack.PC/4) + " " + pipe;
        if(i<3)
            handleMEM(MIPS, Memory);
        pipe = to_string(MIPS.memoryAccess.PC/4) + " " + pipe;
        if(i<2)
            handleEX(MIPS);
        pipe = to_string(MIPS.executeInstruction.PC/4) + " " + pipe;
        if(i < 1)
            handleID(MIPS, registerFile);
        pipe = to_string(MIPS.instructionDecode.PC/4) + " " + pipe;
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
}