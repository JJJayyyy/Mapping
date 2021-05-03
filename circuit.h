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

    unordered_map<string, int> gateCounter;
    unordered_map<string, float> errorRateMap = {{"PI",0},
                                                 {"SP", 0.03},
                                                 {"XOR", 0.07},
                                                 {"OR", 0.09},
                                                 {"AND",0.09},
                                                 {"NOT",0.05},
                                                 {"DFF",0.08},
                                                 {"SDFF",0.08}};
    unordered_map<string,node*> nameToNode;
    unordered_map<int, vector<string>> levelList;

    string circuit_name;
    int max_fout = 0, max_fin = 0;
    vector<node*> Node_list, Pinput, Poutput;
    int max_lvl = 0;
    ifstream circuit_infile;
    int level_flag = 0;
    int newNodeNameCounter = 0;
    int SDFFNameCounter = 0;
    int splitterNameCounter = 0;
    int totalGateNum = 0;

    circuit();
    circuit(const string& file_name, bool readFile);
//    circuit(circuit* sample);
    void Read_Bench_File(const string& file_name);

    void pc();

    void Levelization();
    void Depth_Balancing();
    int Get_Gate_Total_Num();
    void Splitter_Binary_Tree_Insertion();

    static vector<string> QM(vector<int>& temp, vector<node*> &fin_node);
    vector<vector<int>> LUT_generation();

    static void connectNodes(node* down_node, node* up_node);
    static void disconnectNodes(node *down_node, node *up_node);
    static int gateToInt(const string& gate_string);
    static string intToGate(int gate_name);
    vector<string>LUTtoQM();

    ~circuit();

    void fillCutToCircuit(circuit* LUTToCut);
    void deleteNode(node* cur_node);

    float errorRateCal();

    int Count_Gate_Num();

    void Levelization_By_PI();

    node* construct_PI(string& name);
    node* construct_Node(string &name, int int_gate_type);
    node* construct_Node(string &name);

    void cut_Depth_balancing();

    void DFF_optimization();

    void removeOldNode(vector<node *>& cutPI, node *root_node);
};


#endif //SFQ_PARSER_CIRCUIT_H
