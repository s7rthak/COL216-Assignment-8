#include <fstream>
#include <bits/stdc++.h>
typedef long long ll;

using namespace std;



ll stringToDecimal(string s)
{
    ll p = 0;
    for(int i=s.length() - 1; i>= 0; i--)
    {
        if(s[i]=='1')
        {
            p = p + pow(2,s.length() - 1 - i);
        }
    } 

    return p;

}