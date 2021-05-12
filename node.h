//
// Created by Jay on 4/22/21.
//

#ifndef SFQ_PARSER_NODE_H
#define SFQ_PARSER_NODE_H

#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

//case 0: return("PI");
//case 1: return("SP");
//case 2: return("XOR");
//case 3: return("XNOR");
//case 4: return("OR");
//case 5: return("NOR");
//case 6: return("AND");
//case 7: return("NAND");
//case 8: return("NOT");
//case 9: return("BUFF");
//case 10 : return("MAJ");
//case 11: return("DFF");
//case 12: return("SDFF");
//default: return("NONE");

class node {

public:
    vector <node*> unodes, dnodes;

    string node_name;

    int level = 0;
    int gtype;
    int val_sim = INT_MIN;
    bool initial_node = false;                   // initial node gtype: PI and DFF

    node(string &name, int gate_type);
    node(string &name);
    ~node();

};


#endif //SFQ_PARSER_NODE_H
