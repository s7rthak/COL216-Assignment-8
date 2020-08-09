#include <fstream>
#include <bits/stdc++.h>

using namespace std;

vector<int> memoryFileToVector(string inst_file){
    ifstream in(inst_file);
    string str;
    vector<int>memory;
    while(getline(in, str)){
    	//cout << str << endl;
        int num = stoi(str);
        memory.push_back(num);
    }
    in.close();
    return memory;
}
