//
// Created by Jay on 4/22/21.
//

#ifndef SFQ_PARSER_CIRCUIT_H
#define SFQ_PARSER_CIRCUIT_H


#include "node.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>
#include <climits>

using namespace std;

class circuit {

public:
    ifstream circuit_infile;

    unordered_map<string, int> gateCounter;
    unordered_map<string, float> errorRateMap = {{"PI",0},
                                                 {"SP", 0.03},
                                                 {"XOR", 0.07},
                                                 {"OR", 0.09},
                                                 {"AND",0.09},
                                                 {"MAJ",0.09},
                                                 {"NOT",0.05},
                                                 {"DFF",0.08},
                                                 {"SDFF",0.08}};
    unordered_map<string,node*> nameToNode;
    unordered_map<int, vector<string>> levelList;
    vector<node*> Node_list, Pinput, Poutput;

    string circuit_name;
    int max_fout = 0, max_fin = 0;
    int max_lvl = 0;
    int level_flag = 0;
    int SDFFNameCounter = 0;
    int splitterNameCounter = 0;
    int totalGateNum = 0;
    int newNodeNameCounter = 0;

    /**** Circuit ****/
    circuit();
    circuit(const string& file_name, bool readFile);
    bool Read_Bench_File(const string& file_name);
    void Levelization();
    void Levelization_By_PI();
    void Depth_Balancing();
    void cut_Depth_balancing();
    void Splitter_Binary_Tree_Insertion();
    void Splitter_Insertion();
    void DFF_optimization();
    ~circuit();


    /**** Circuit util ****/
    void pc();
    static void Connect_Nodes(node* down_node, node* up_node);
    static void Disconnect_Nodes(node *down_node, node *up_node);
    static int gateToInt(const string& gate_string);
    static string intToGate(int gate_name);
    node* Construct_PI(string& name);
    node* Construct_Node(string &name, int int_gate_type);
    node* Construct_Node(string &name);
    int Get_Gate_Total_Num();
    float Error_Rate_Calculation();


    /**** Circuit Simulation ****/
    vector<string>LUT_TO_QM();
    static vector<string> QM(vector<int>& temp, vector<node*> &fin_node);
    vector<vector<int>> LUT_Generation();


    /**** Circuit Replacement ****/
    void Fill_Cut_To_Circuit(circuit* LUTToCut);
    void Delete_Node(node* cur_node);
    void Remove_Old_Node(vector<node *>& cutPI, node *root_node);


};


#endif //SFQ_PARSER_CIRCUIT_H
