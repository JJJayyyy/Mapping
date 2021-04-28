//
// Created by Jay on 4/22/21.
//

#ifndef SFQ_PARSER_CIRCUIT_H
#define SFQ_PARSER_CIRCUIT_H


#include "node.h"
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <fstream>
#include <queue>
#include <vector>
#include <string>

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
    int read_flag = 0;
    int level_flag = 0;

    circuit();
    circuit(const string& file_name, bool readFile);
//    circuit(circuit* sample);
    void read(const string& file_name);

    /*******   Status check functions   *******/
    int read_done();

    void pc();
    static int gateToInt(const string& gate_string);
    static string intToGate(int gate_name);
    void levelization();
    void depth_balancing();
    int getGateTotalNum();
    void spliterTreeAdder();

    vector<string> QM(vector<int>& temp, vector<node*> &fin_node);
    vector<vector<int>> LUT_generation();
    void connectNodes(node* down_node, node* up_node);
    void disconnectNodes(node *down_node, node *up_node);
    vector<string>LUTtoQM();

    ~circuit();

    void replaceCutwithQM(vector<string> boolEqn, node* root_node);
    void replaceCutwithQM(vector<node*> boolEqn, node* root_node);
    void deleteOldCut(node* cur_node, vector<string> node_name);

    float errorRateCal();

    int countGate();
};


#endif //SFQ_PARSER_CIRCUIT_H
