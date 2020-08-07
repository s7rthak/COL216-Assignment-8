#include <fstream>
#include <bits/stdc++.h>
#include "util.h"

using namespace std;

vector<int> memoryFileToVector(string inst_file){
    ifstream in(inst_file);
    string str;
    vector<int>instruction_memory;
    while(getline(in, str)){
        int num = stringToDecimal(str);
        instruction_memory.push_back(num);
    }
    in.close();
    return instruction_memory;
}
