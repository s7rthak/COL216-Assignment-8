#include <fstream>
#include <bits/stdc++.h>
typedef long long ll;

using namespace std;


void break_string(string &s, string &x, string &y){
    ll a,b;
    b = s.size()-1;
    for(ll i=0;i<s.size();i++){
        if(s[i] == '('){
            a = i;
            break;
        }
    }
    x = s.substr(0,a);
    y = s.substr(a+1,b-a-1);
}
 
string lltobinary(string s)
{
    stringstream geek(s); 
    ll n = 0; 
    geek >> n;
    string answer;

     for (ll i = 4; i >= 0; i--) { 
        ll k = n >> i; 
        if (k & 1) 
            answer += "1";
        else
            answer += "0";
    }
    if(answer.size()<5)
    {
        for(ll i=0; i<5-answer.size() ; i++)
        {
            answer = "0" + answer;
        }
    }

    return answer;
    


}


string lltooffset(string s)
{
stringstream geek(s); 
    ll n = 0; 
    geek >> n;
    string answer;
     for (ll i = 15; i >= 0; i--) { 
        ll k = n >> i; 
        if (k & 1) 
            answer += "1";
        else
            answer += "0";
    }
    if(answer.size()<16)
    {
        for(ll i=0; i<16-answer.size() ; i++)
        {
            answer = "0" + answer;
        }
    }

    return answer;
    


}


string lltotarget(string s)
{
    stringstream geek(s); 
    ll n = 0; 
    geek >> n;
    string answer;

     for (ll i = 25; i >= 0; i--) { 
        ll k = n >> i; 
        if (k & 1) 
            answer += "1";
        else
            answer += "0";
    }
    if(answer.size()<26)
    {
        for(ll i=0; i<26-answer.size() ; i++)
        {
            answer = "0" + answer;
        }
    }

    return answer;
    


}


vector<string> instructionsFileToVector(string inst_file){
	ll count =0;
    ifstream in(inst_file);
    string str;
    vector<string>instruction_memory;
       vector<vector<string>>opcodes;
    map<string,ll> pcc;
    while (in) {

        string s;
        bool b = true;
        if(!getline(in,s)) break;

        stringstream ss(s);
        vector<string>separated;
        string sep;
        while(ss>>sep){
            if (sep.find(':') != std::string::npos)
            {
                string func = sep.substr(0, sep.size()-1);
                pcc[func] = count;
                b = false;

            }
            else
            {    
                sep.erase(remove(sep.begin(),sep.end(),','),sep.end());
                separated.push_back(sep);
                if(ss.peek() == ' ' || ss.peek() == ',')
                    ss.ignore();
            }
        }
        if(!ss.str().empty() && b)
            count++;

        for(ll i=0;i<separated.size();i++)
            cout<<separated[i]<<" "  ;

        opcodes.push_back(separated);
        cout<<"\n";
    }
 
 vector<string> vec;
 for(ll i=0;i<opcodes.size();i++){
        if(opcodes[i].size() > 0)
        {
            string s;
        if(opcodes[i][0] == "add"){
            s = "000000";
            s = s+ lltobinary(opcodes[i][2].substr(1));
            s = s+ lltobinary(opcodes[i][3].substr(1));
            s = s+ lltobinary(opcodes[i][1].substr(1));
            s += "00000";
            s += "100000";


        }

        else if(opcodes[i][0] == "sub"){
            s = "000000";
            s = s+ lltobinary(opcodes[i][2].substr(1));
            s = s+ lltobinary(opcodes[i][3].substr(1));
            s = s+ lltobinary(opcodes[i][1].substr(1));
            s += "00000";
            s += "100010";

        
        }
        else if (opcodes[i][0] == "sll")
            {
                s = "000000";
                s = s + "00000";
                s = s + lltobinary(opcodes[i][2].substr(1));
                s = s + lltobinary(opcodes[i][1].substr(1));
                s += lltobinary(opcodes[i][3]);
                s += "000000";
                //vop.push_back("sll");
            }
        else if(opcodes[i][0] == "srl"){
        
            s = "000000";
            s = s+ "00000";
            s = s+ lltobinary(opcodes[i][2].substr(1));
            s = s+ lltobinary(opcodes[i][1].substr(1));
            s += lltobinary(opcodes[i][3]);
            s += "000010";

        }
        else if(opcodes[i][0] == "sw"){
            string x,y;
            break_string(opcodes[i][2],x,y);
            s = "101011";
            s+= lltobinary(y.substr(1));
            s += lltobinary(opcodes[i][1].substr(1));
            s +=  lltooffset(x);
        }
        else if(opcodes[i][0] == "lw"){
            string x,y;
            break_string(opcodes[i][2],x,y);
            s = "100011";
            s+= lltobinary(y.substr(1));
            s += lltobinary(opcodes[i][1].substr(1));
            s +=  lltooffset(x);
        }
        else if(opcodes[i][0] == "bne")
        {
            
            s = "000101";
            s += lltobinary(opcodes[i][1].substr(1));
            s += lltobinary(opcodes[i][2].substr(1));
            s += lltooffset(to_string(pcc[opcodes[i][3]]));
            


        }
        else if(opcodes[i][0] == "beq")
        {


            s = "000100";
            s += lltobinary(opcodes[i][1].substr(1));
            s += lltobinary(opcodes[i][2].substr(1));
            s += lltooffset(to_string(pcc[opcodes[i][3]]));
            
        }
        else if(opcodes[i][0] == "blez")
        {

            
            s = "000110";
            s += lltobinary(opcodes[i][1].substr(1));
            s += "00000";
            s += lltooffset(to_string(pcc[opcodes[i][2]]));
            

        }
        else if(opcodes[i][0] == "bgtz")
        {
            

            s = "000111";
            s += lltobinary(opcodes[i][1].substr(1));
            s += "00000";
            s += lltooffset(to_string(pcc[opcodes[i][2]]));
            

        }
        else if(opcodes[i][0] == "j")
        {

            s = "000010";
            s +=lltotarget(to_string(pcc[opcodes[i][1]]));
            
        }
        else if(opcodes[i][0] == "jal")
        {
            s = "000011";
            s +=lltotarget(to_string(pcc[opcodes[i][1]]));
           

        }
        else if(opcodes[i][0] == "jr")
        {

            s = "000000";
            s += lltobinary(opcodes[i][1].substr(1));
            s += "000000000000000001000";
            
        }

        vec.push_back(s);
    }
    }


	for(int i=0; i<vec.size() ; i++)
	{
			string str1 = vec[i].substr(0,8);
			string str2 = vec[i].substr(8,8);
			string str3 = vec[i].substr(16,8);
			string str4 = vec[i].substr(24,8);
			cout << vec[i] << endl;
			cout << vec[i].substr(11,5) << endl;
    		instruction_memory.push_back(str1);
    		instruction_memory.push_back(str2);
    		instruction_memory.push_back(str3);
    		instruction_memory.push_back(str4);
		

	}
    
    in.close();
    return instruction_memory;
}
