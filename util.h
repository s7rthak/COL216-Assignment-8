#include <fstream>
#include <bits/stdc++.h>

using namespace std;

int stringToDecimal(string s){
    int n = stoi(s);
    int num = n; 
    int dec_value = 0; 
    int base = 1; 
    int temp = num; 
    while (temp) { 
        int last_digit = temp % 10; 
        temp = temp / 10; 
        dec_value += last_digit * base; 
        base = base * 2; 
    } 
    return dec_value;
}

