//
// Created by Jay on 4/25/21.
//

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include "circuit.h"

using namespace std;

void QM(int LUT_size, const string& temp);
string decToBin(int n);
string replace_complements(string a,string b);
bool isGreyCode(string a,string b);
bool VectorsEqual(vector<string> a,vector<string> b);
bool in_vector(const vector<string>& a,const string& b);
vector<string> reduce(vector<string> in_LUT);




/**Function*************************************************************
    convert decimal to binary
    Eg: 14 = 1110
***********************************************************************/

string decToBin(int n){
    if ( n == 0 )       return "";  //            return n+"";
    if ( n % 2 == 0 )   return decToBin(n / 2).append("0");
    else                return decToBin(n / 2).append("1");
}

/**Function*************************************************************
    function to pad zeros to the binary equivalent of digits.
    Eg: If there are 4 variables, 2, that is 10 in binary is represented as 0010
***********************************************************************/

string pad(const string& bin, int LUT_size){
    unsigned long max = LUT_size - bin.length();
    string s;
    s.append(max, '0');
    return s.append(bin);
}

/**Function*************************************************************
   function to check if two terms differ by just one bit
***********************************************************************/
bool isGreyCode(string a,string b){
    int flag=0;
    for(int i=0;i<a.length();i++){
        if(a[i]!=b[i])
            flag++;
    }
    return (flag==1);
}

/**Function*************************************************************
   function to check if two bits are XOR gate
***********************************************************************/

bool isXOR(string a, string b){
    int flag=0;
    for(int i=0;i<a.length();i++){
        if((a[i]=='0' && b[i]=='1')||(a[i]=='1' && b[i]=='0'))
            flag++;
    }
    return (flag==2);
}

/**Function*************************************************************
    replace complement terms with don't cares
    Eg: 0110 and 0111 becomes 011-
***********************************************************************/

string replace_complements(string a,string b){
    string temp;
    for(int i=0; i<a.length(); i++)
        if(a[i]!=b[i])  temp.append("-");
        else            temp.push_back(a[i]);
    return temp;
}

/**Function*************************************************************
    replace 01, 10 with XOR
    Eg: 0110 and 0101 becomes 01^^
***********************************************************************/

string replace_XOR(string a,string b){
    string temp;
    for(int i=0;i<a.length();i++)
        if(a[i]!=b[i])  temp.append("^");
        else            temp.push_back(a[i]);
    return temp;
}

/**Function*************************************************************
    check if string b exists in vector a
***********************************************************************/

bool in_vector(const vector<string>& a,const string& b){
    for(const auto & i : a)
        if(i==b)
            return true;
    return false;
}

/**Function*************************************************************
    check if 2 vectors are equal
***********************************************************************/
bool VectorsEqual(vector<string> a,vector<string> b){
    if(a.size()!=b.size())  return false;
    sort(a.begin(),a.end());
    sort(b.begin(),b.end());
    for(int i=0;i<a.size();i++){
        if(a[i]!=b[i])      return false;
    }
    return true;
}


/**Function*************************************************************
    reduce in_LUT
***********************************************************************/
vector<string> reduce(vector<string> in_LUT){
    vector<string> greyCombine;
    vector<string> xorCombine;
    int sizeLUT=in_LUT.size();
    int* checkLUT = new int[sizeLUT];
    for(int i=0; i < sizeLUT; i++){
        for(int j=i; j < sizeLUT; j++){     //If a grey code pair is found, replace the differing bits with don't cares.
            if(isGreyCode(in_LUT[i], in_LUT[j])){
                checkLUT[i]=1;
                checkLUT[j]=1;
                if(!in_vector(greyCombine, replace_complements(in_LUT[i], in_LUT[j])))
                    greyCombine.push_back(replace_complements(in_LUT[i], in_LUT[j]));
            }
        }
    }
    for(int i=0; i < sizeLUT; i++){
        if(checkLUT[i] != 1 && ! in_vector(greyCombine, in_LUT[i]))
            greyCombine.push_back(in_LUT[i]);
    }

    int sizeGrey=greyCombine.size();
    int* checkGrey = new int[sizeGrey];
    for(int i=0; i < sizeGrey; i++){
        for(int j=i; j < sizeGrey; j++){     //If a grey code pair is found, replace the differing bits with don't cares.
            if(isXOR(greyCombine[i], greyCombine[j])){
                checkGrey[i]=1;
                checkGrey[j]=1;
                if(!in_vector(xorCombine, replace_XOR(greyCombine[i], greyCombine[j])))
                    xorCombine.push_back(replace_XOR(greyCombine[i], greyCombine[j]));
            }
        }
    }
    for(int i=0; i < sizeGrey; i++){
        if(checkGrey[i] != 1 && ! in_vector(xorCombine, greyCombine[i]))
            xorCombine.push_back(greyCombine[i]);
    }

    //appending all reduced terms to a new vector
    delete[] checkLUT;
    delete[] checkGrey;
    return xorCombine;
}


/**Function*************************************************************
    Converting the boolean minterm into the variables
    Eg: 011- becomes a'bc
***********************************************************************/

string getValue(string a, const string& dontcares, vector<node*>& fin_node){
    string temp;
    if(a==dontcares)    return "1";
    for(int i=0;i<a.length();i++){
        if(a[i] != '-'){
            if(a[i] == '0'){
                temp.append(fin_node[i]->node_name);
                temp.append("\'");
            }
            else    temp.append(fin_node[i]->node_name);
            if(i != a.length()-1) temp.append("*");
        }
    }
    if(temp[temp.length()-1] == '*') temp = temp.substr(0,temp.length()-1);
    return temp;
}


/**Function*************************************************************
    Do the logic minimization
***********************************************************************/

vector<string> circuit::QM(vector<int>& temp, vector<node*>& fin_node){
    string dontcare;
    int LUT_size = fin_node.size();
    dontcare.append(LUT_size, '-');
    vector<string> input_LUT;
    vector<string> updated_eqn;
    for(auto item: temp){
        string x = pad(decToBin(item), LUT_size);
        input_LUT.push_back(pad(decToBin(item), LUT_size));
    }
    sort(input_LUT.begin(), input_LUT.end());
    do{
        input_LUT=reduce(input_LUT);
//        cout << "while input_LUT: " << endl;
//        for(const auto& i : input_LUT) cout << i << " ";  cout <<  endl;
        sort(input_LUT.begin(), input_LUT.end());
    }while(!VectorsEqual(input_LUT, reduce(input_LUT)));

    cout << "The reduced boolean expression in SOP form:" << endl;
    for (int i=0; i < input_LUT.size(); i++){
        updated_eqn.push_back(getValue(input_LUT[i], dontcare, fin_node));
        cout << getValue(input_LUT[i], dontcare, fin_node);
        if (i!=input_LUT.size()-1) cout << "+";
        else cout << endl;
    }
    return  input_LUT;
//    return  updated_eqn;
}




