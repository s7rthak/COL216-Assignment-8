#include <fstream>
#include <bits/stdc++.h>

using namespace std;

vector<string> instructionsFileToVector(string inst_file){
    ifstream in(inst_file);
    string str;
    vector<string>instruction_memory;
    while(getline(in, str)){
        instruction_memory.push_back(str);
    }
    in.close();
    return instruction_memory;
}
