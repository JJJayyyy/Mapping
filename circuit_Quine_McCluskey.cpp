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
string decToBin(int n);
string pad(const string& bin, int LUT_size);
bool isGreyCode(string a,string b);
int isXOR(string& a, string& b);
unordered_map<int, int> isMAJ(string& a, string& b, string& c);
string replace_complements(string a,string b);
string replace_XOR(string& a, string& b);
string replace_XNOR(string& a, string& b);
string replace_MAJ(string& a, string& b, string& c, unordered_map<int, int> MAJIdxMap);
bool VectorsEqual(vector<string> a,vector<string> b);
bool in_vector(const vector<string>& a,const string& b);
vector<string> reduce(vector<string>& implicants);
vector<string> MAJ_Reduce(vector<string>& greyCombine);
vector<string> XOR_Reduce(vector<string>& majCombine);
void BooleanExpressionCheck(vector<string> input_LUT, vector<string> updated_eqn, const string& dontcare, vector<node*>& fin_node);
void Debugging(vector<string>& input_LUT, vector<string>& input_LUT_X, vector<string>& input_LUT_M, vector<int>& temp);



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
int isXOR(string& a, string& b){
    int flag10 = 0, flag01 = 0;
    bool breakFlag = false;
    for(int i=0; i<a.length(); i++){
        if(a[i]=='0' && b[i]=='1')
            flag01++;
        else if(a[i]=='1' && b[i]=='0')
            flag10++;
        else if (a[i] != b[i])
            breakFlag = true;
    }
    if((flag10 == 1 && flag01 == 1) && !breakFlag)
        return 1;
    else if(((flag10 == 2 && flag01 == 0)|| (flag10 == 0 && flag01 == 2))&& !breakFlag)
        return 2;
    else
        return 0;
}


/**Function*************************************************************
   function to check if three terms have majority function
***********************************************************************/
unordered_map<int, int> isMAJ(string& a, string& b, string& c){
    unordered_map<int, int> result;
    bool jump_flag = false;
    int flag11_ = 0, flag1_1 = 0, flag_11 = 0;
    int flag00_ = 0, flag0_0 = 0, flag_00 = 0;
    for(int i = 0; i < a.length(); i++){
        if(a[i] == '1' && b[i] == '1' && c[i] == '-')   {
            flag11_++;
            result[i] = 1;
        }
        else if(a[i] == '1' && b[i] == '-' && c[i] == '1')   {
            flag1_1++;
            result[i] = 1;
        }
        else if(a[i] == '-' && b[i] == '1' && c[i] == '1')   {
            flag_11++;
            result[i] = 1;
        }
        else if(a[i] == '0' && b[i] == '0' && c[i] == '-')   {
            flag00_++;
            result[i] = 0;
        }
        else if(a[i] == '0' && b[i] == '-' && c[i] == '0')   {
            flag0_0++;
            result[i] = 0;
        }
        else if(a[i] == '-' && b[i] == '0' && c[i] == '0')   {
            flag_00++;
            result[i] = 0;
        } else if(a[i] == b[i] && a[i] == c[i] && b[i]==c[i]){
            continue;
        }else{
            jump_flag = true;
        }
    }

    // only three 1 is MAJ
    if(flag11_ + flag1_1 + flag_11 + flag00_ + flag0_0 + flag_00>3){
        jump_flag = true;
    }
//    if(flag11_>1 || flag1_1 >1 || flag_11 > 1 || flag00_>1 || flag0_0>1 || flag_00>1){
//        jump_flag = true;
//    }



    if(!jump_flag){
        cout << "maj exist: " << a << " " << b << " " << c << "\t|\t" <<flag11_<<flag1_1<<flag_11<<flag00_<<flag0_0<<flag_00<< endl;
        if(flag11_ == 1 && flag1_1 == 1 && flag_11 ==1)         return result;  // 0 inv
        else if(flag00_ == 1 && flag_11 == 1 && flag1_1 ==1)    return result;  // 1 inv
        else if(flag0_0 == 1 && flag_11 == 1 && flag11_ ==1)    return result;  // 1 inv
        else if(flag00_ == 1 && flag1_1 == 1 && flag_11 ==1)    return result;  // 1 inv
        else if(flag0_0 == 1 && flag11_ == 1 && flag_11 ==1)    return result;  // 1 inv
        else if(flag_00 == 1 && flag1_1 == 1 && flag11_ ==1)    return result;  // 1 inv
        else if(flag_00 == 1 && flag11_ == 1 && flag1_1 ==1)    return result;  // 1 inv
        else if(flag00_ == 1 && flag_11 == 1 && flag0_0 ==1)    return result;  // 2 inv
        else if(flag0_0 == 1 && flag_11 == 1 && flag00_ ==1)    return result;  // 2 inv
        else if(flag00_ == 1 && flag1_1 == 1 && flag_00 ==1)    return result;  // 2 inv
        else if(flag0_0 == 1 && flag11_ == 1 && flag_00 ==1)    return result;  // 2 inv
        else if(flag_00 == 1 && flag1_1 == 1 && flag00_ ==1)    return result;  // 2 inv
        else if(flag_00 == 1 && flag11_ == 1 && flag0_0 ==1)    return result;  // 2 inv
        else if(flag00_ == 1 && flag_00 == 1 && flag0_0 ==1)    return result;  // 3 inv
    }

    result.clear();
    return result;
}

/**Function*************************************************************
    replace complement terms with don't cares
    Eg: 0110 and 0111 becomes 011-
***********************************************************************/
string replace_complements(string a,string b){
    string temp;
    for(int i=0; i<a.length(); i++)
        if(a[i]!=b[i])
            temp.append("-");
        else
            temp.push_back(a[i]);
    return temp;
}



/**Function*************************************************************
    replace 01, 10 with XOR
    Eg: 0110 and 0101 becomes 01XX
***********************************************************************/
string replace_XOR(string& a, string& b){
    string temp;
    for(int i=0;i<a.length();i++){
        if(a[i]!=b[i])
            temp.append("X");
        else
            temp.push_back(a[i]);
    }
    return temp;
}


/**Function*************************************************************
    replace 00, 11 with XNOR
    Eg: 0100 and 0111 becomes 01xx
***********************************************************************/
string replace_XNOR(string& a, string& b){
    string temp;
    for(int i=0;i<a.length();i++){
        if(a[i]!=b[i])
            temp.append("x");
        else
            temp.push_back(a[i]);
    }
    return temp;
}



/**Function*************************************************************
    replace 11-, 1-1, -11 with MAJ
    Eg: 11-, 1-1,and -11 becomes MMM
***********************************************************************/
string replace_MAJ(string& a, string& b, string& c, unordered_map<int, int> MAJIdxMap){
    string temp;
    for(int i=0; i<a.length(); i++){
        if(a[i]==b[i] && a[i]==c[i] && b[i]==c[i]){
            temp.push_back(a[i]);
        }else{
            if(MAJIdxMap[i] == 1)
                temp.append("M");
            else if (MAJIdxMap[i] == 0)
                temp.append("m");
            else
                cout << "!!!check replace MAJ!!!" << endl;
        }
    }
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
    if(a.size()!=b.size())
        return false;
    sort(a.begin(),a.end());
    sort(b.begin(),b.end());
    for(int i=0;i<a.size();i++){
        if(a[i]!=b[i])
            return false;
    }
    return true;
}



/**Function*************************************************************
    reduce implicants with greycode check
***********************************************************************/
vector<string> reduce(vector<string>& implicants){
    vector<string> greyCombine;
    string tempImplicant;

    //{1 0} pair -> '-'
    int sizeLUT = implicants.size();
    int* checkGrey = new int[sizeLUT]();
    for(int i = 0; i < sizeLUT; i++){
        for(int j = i+1; j < sizeLUT; j++){
            if(isGreyCode(implicants[i], implicants[j])){
                checkGrey[i] = 1, checkGrey[j] = 1;
                tempImplicant = replace_complements(implicants[i], implicants[j]);
                if(!in_vector(greyCombine, tempImplicant)) greyCombine.push_back(tempImplicant);
            }
        }
    }

    for(int i = 0; i < sizeLUT; i++){
        if(checkGrey[i] != 1 && ! in_vector(greyCombine, implicants[i]))
            greyCombine.push_back(implicants[i]);
    }
//    cout << "grey Combine: ";
//    for(const auto& x : greyCombine) cout <<  x << " "; cout << endl;

    //delete the dynamic arrays
    delete[] checkGrey;
    return greyCombine;
}


/**Function*************************************************************
    reduce greyCombine with MAJ check
***********************************************************************/
vector<string> MAJ_Reduce(vector<string>& greyCombine){
    vector<string> majCombine;
    string tempImplicant;

    // Majority check M: not inv; m: inv
    int sizeGrey = greyCombine.size();
    int* checkMAJ = new int[sizeGrey]();
    if(sizeGrey >= 3){
        for(int i = 0; i < sizeGrey; i++){
            if(greyCombine[i].find('X')!= string::npos || greyCombine[i].find('x') != string::npos)
                continue;
            for(int j = i+1; j < sizeGrey; j++){
                for(int k = j+1; k < sizeGrey; k++){
                    unordered_map<int, int> MAJIdxMap = isMAJ(greyCombine[i], greyCombine[j], greyCombine[k]);
                    if(!MAJIdxMap.empty()){
                        checkMAJ[i] = 1, checkMAJ[j] = 1, checkMAJ[k] = 1;
                        tempImplicant = replace_MAJ(greyCombine[i], greyCombine[j], greyCombine[k], MAJIdxMap);
                        if(!in_vector(majCombine, tempImplicant)) majCombine.push_back(tempImplicant);
                    }
                }
            }
        }
    }
    for(int i = 0; i < sizeGrey; i++){
        if(checkMAJ[i] != 1 && ! in_vector(majCombine, greyCombine[i]))
            majCombine.push_back(greyCombine[i]);
    }

//    cout << "MAJ Combine: ";
//    for(const auto& x : majCombine) cout <<  x << " "; cout << endl;

    //delete the dynamic arrays
    delete[] checkMAJ;
    return majCombine;
}




/**Function*************************************************************
    reduce majCombine with XOR/XNOR check
***********************************************************************/
vector<string> XOR_Reduce(vector<string>& majCombine){
    vector<string> xorCombine;
    string tempImplicant;

    //{10 01} pair -> '^^'; {00 11} pair -> '@@'
    int sizeMAJ = majCombine.size();
    int* checkXOR_XNOR = new int[sizeMAJ]();        //initialize all elements to 0;
    if(sizeMAJ>1){
        for(int i = 0; i < sizeMAJ; i++){
            if(majCombine[i].find('M')!= string::npos || majCombine[i].find('m') != string::npos)
                continue;
            for(int j = i+1; j < sizeMAJ; j++){
                if(majCombine[j].find('M') != string::npos || majCombine[j].find('m') != string::npos)
                    continue;
                if(isXOR(majCombine[i], majCombine[j]) == 1){
                    checkXOR_XNOR[i] += 1, checkXOR_XNOR[j] += 1;     // remove this two implicants
                    tempImplicant = replace_XOR(majCombine[i], majCombine[j]);
                    if(!in_vector(xorCombine, tempImplicant))
                        xorCombine.push_back(tempImplicant);
                } else if (isXOR(majCombine[i], majCombine[j]) == 2){
                    checkXOR_XNOR[i] += 1, checkXOR_XNOR[j] += 1;     // remove this two implicants
                    tempImplicant = replace_XNOR(majCombine[i], majCombine[j]);
                    if(!in_vector(xorCombine, tempImplicant))
                        xorCombine.push_back(tempImplicant);
                }
            }
        }

        vector<string> simp;
        vector<int> sim_check;
        vector<size_t> index_simp;
        bool multicombination = false;
        for(int item = 0; item < majCombine.size(); item++){
            if(checkXOR_XNOR[item] > 0){
                simp.push_back(majCombine[item]);
                sim_check.push_back(checkXOR_XNOR[item]);
                if(checkXOR_XNOR[item] > 1){
                    multicombination = true;
                }
            }
        }
        for (size_t i = 0; i != simp.size(); ++i) { index_simp.push_back(i); }
        assert(simp.size() == sim_check.size());

        if(!multicombination){
            simp.clear();
            sim_check.clear();
        } else{
            xorCombine.clear();
//            cout << "simp : "; for(const auto& x : simp) cout << x << " ";   cout << endl;
//            cout << "simp_c : "; for(const auto& x : sim_check) cout << x << " ";   cout << endl;
//            cout << "idx : "; for(const auto& x : index_simp) cout << x << " ";   cout << endl;
            sort(index_simp.begin(), index_simp.end(), [&](size_t a, size_t b) { return sim_check[a] < sim_check[b];});
//            cout << "simp : "; for(int i=0; i<simp.size(); i++) cout << simp[index_simp[i]] << " ";   cout << endl;
//            cout << "simp_c : "; for(const auto& x : sim_check) cout << x << " ";   cout << endl;
//            cout << "idx : "; for(const auto& x : index_simp) cout << x << " ";   cout << endl;

            for(int i = 0; i < simp.size(); i++){
                if(sim_check[index_simp[i]] == 0)
                    continue;
                for(int j = i+1; j < simp.size(); j++){
                    if(sim_check[index_simp[i]] == 0 || sim_check[index_simp[j]] == 0)
                        continue;
                    if(isXOR(simp[index_simp[i]], simp[index_simp[j]]) == 1){
                        tempImplicant = replace_XOR(simp[index_simp[i]], simp[index_simp[j]]);
                        xorCombine.push_back(tempImplicant);
                        sim_check[index_simp[i]] = 0;
                        sim_check[index_simp[j]] = 0;
                    }
                    else if(isXOR(simp[index_simp[i]], simp[index_simp[j]]) == 2){
                        tempImplicant = replace_XNOR(simp[index_simp[i]], simp[index_simp[j]]);
                        xorCombine.push_back(tempImplicant);
                        sim_check[index_simp[i]] = 0;
                        sim_check[index_simp[j]] = 0;
                    }
                }
            }
            for(int i = 0; i < sim_check.size(); i++){
                if(sim_check[i] != 0)
                    xorCombine.push_back(simp[i]);
            }

            cout << "simp : "; for(const auto& x : simp) cout << x << " ";   cout << endl;
            cout << "simpxorCombine : "; for(const auto& x : xorCombine) cout << x << " ";   cout << endl;
        }

    }
    for(int i=0; i < sizeMAJ; i++){
        if(checkXOR_XNOR[i] == 0 && ! in_vector(xorCombine, majCombine[i]))
            xorCombine.push_back(majCombine[i]);
    }

//    cout << "XOR/XNOR Combine: ";
//    for(const auto& x : xorCombine) cout <<  x << " "; cout << endl;

    //delete the dynamic arrays
    delete[] checkXOR_XNOR;
    return xorCombine;
}



/**Function*************************************************************
    Converting the boolean minterm into the variables
    Eg: 011- becomes a'bc
***********************************************************************/
string getValue(string a, const string& dontcares, vector<node*>& fin_node){
    string temp;
    if(a==dontcares)
        return "1";
    for(int i=0;i<a.length();i++){
        if(a[i] != '-'){
            if(a[i] == '0'){
                temp.append(fin_node[i]->node_name);
                temp.append("\'");
            }
            else
                temp.append(fin_node[i]->node_name);
            if(i != a.length()-1)
                temp.append("*");
        }
    }
    if(temp[temp.length()-1] == '*')
        temp = temp.substr(0,temp.length()-1);
    return temp;
}



void BooleanExpressionCheck(vector<string> input_LUT, const string& dontcare, vector<node*>& fin_node){
    cout << "The reduced boolean expression in SOP form:" << endl;
    for (int i=0; i < input_LUT.size(); i++){
        cout << getValue(input_LUT[i], dontcare, fin_node);
        if (i!=input_LUT.size()-1)
            cout << "+";
        else
            cout << endl;
    }
}


void Debugging(vector<string>& input_LUT, vector<string>& input_LUT_X, vector<string>& input_LUT_M, vector<int>& temp){
    bool check_flag = false;
    if(input_LUT_X.size() >= temp.size() && temp.size() != 2){
        cout << "size not reduce: " << input_LUT_X.size() << " >= " << temp.size() << endl;
        check_flag = true;
    }

    for(const auto& item : input_LUT_X){
        int m_count = 0;
        int x_count = 0;
        for(auto c : item){
            if(c == 'M' || c == 'm') m_count++;
            if(c == 'X' || c == 'x') x_count++;
        }
        if(m_count > 3 || x_count > 2){
            cout << "M > 3 || X > 2" << endl;
            check_flag = true;
        }
        assert(m_count == 3 || m_count == 0);
        assert(x_count == 2 || x_count == 0);
    }

    if(check_flag){
        cout << "LUT : "; for(const auto& x : temp) cout <<  x << " ";   cout << endl;
        cout << "Grey: "; for(const auto& x : input_LUT) cout <<  x << " ";   cout << endl;
        cout << "MAJ : "; for(const auto& x : input_LUT_M) cout <<  x << " ";   cout << endl;
        cout << "XOR : "; for(const auto& x : input_LUT_X) cout <<  x << " ";   cout << endl << endl;
    }
}



/**Function*************************************************************
    Do the logic minimization
***********************************************************************/
vector<string> circuit::QM(vector<int>& temp, vector<node*>& fin_node){
    string dontcare;
    vector<string> input_LUT, input_LUT_M, input_LUT_X;
    vector<string> updated_eqn;
    int LUT_size = fin_node.size();
    dontcare.append(LUT_size, '-');
    input_LUT.reserve(temp.size());



    for(auto item: temp){
        input_LUT.push_back(pad(decToBin(item), LUT_size));
    }
    sort(input_LUT.begin(), input_LUT.end());


    if(input_LUT.size() > 1){
        do{
            input_LUT=reduce(input_LUT);
            sort(input_LUT.begin(), input_LUT.end());
            if(input_LUT.size() == 1) break;
        }while(!VectorsEqual(input_LUT, reduce(input_LUT)));
    }
    input_LUT_M = MAJ_Reduce(input_LUT);
    input_LUT_X = XOR_Reduce(input_LUT_M);
    Debugging(input_LUT, input_LUT_X, input_LUT_M, temp);
    for(const auto& x : input_LUT_X) cout <<  x << " ";   cout << endl << endl;
//    BooleanExpressionCheck(input_LUT_X, dontcare, fin_node);
    return  input_LUT_X;
}