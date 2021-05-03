//
// Created by Jay on 5/1/21.
//
#include "circuit.h"

/**Function*************************************************************
    Print the circuit information
***********************************************************************/
void circuit:: pc(){
    if(level_flag != 1) {
        cout << "circuit needs to be leveled first before printing circuit info" << endl;
        return;
    }
    printf(" Node   Type \t    Out    \t\t      In       \n");
    printf("------ ------\t-----------\t\t---------------\n");
    for(const auto& cur_node : Node_list) {
        printf(" %-4s   %-4s\t", cur_node->node_name.c_str(), intToGate(cur_node->gtype).c_str());
        for(int j = 0; j < max_fout; j++){
            if(cur_node->dnodes.size() > j) printf("%-4s ", cur_node->dnodes[j]->node_name.c_str());
            else printf("%-4s ", " ");
        }
        cout <<"\t\t";
        for(int j = 0; j < max_fin; j++) {
            if(cur_node->unodes.size() > j) printf("%-4s ", cur_node->unodes[j]->node_name.c_str());
            else printf("%-4s ", " ");
        }
        printf("\n");
    }
    for(int i=0; i<25;i++) cout << "--";
    printf("\nPrimary inputs:  ");
    for(const auto& i : Pinput) printf("%s ",i->node_name.c_str());
    printf("\nPrimary outputs: ");
    for(const auto& i : Poutput) printf("%s ",i->node_name.c_str());
    printf("\n\nNumber of nodes = %lu\n", Node_list.size());
    printf("Number of Gates = %d\n", totalGateNum);
    for(const auto& i : gateCounter) if(i.second>0) printf("#%-4s: %-4d\n", i.first.c_str(), i.second);
    printf("\nMaximum level = %d\n", max_lvl);
    for(int lvl = 1; lvl<levelList.size()+1; lvl++){
        vector<string> lvl_list = levelList[lvl];
        printf("Level %-2d:", lvl);
        for(const auto& nodes_name : lvl_list)
            printf("%-4s ", nodes_name.c_str());
        cout << endl;
    }
    cout << endl;
}


/**Function*************************************************************
    Connect two nodes
***********************************************************************/
void circuit::connectNodes(node *down_node, node *up_node) {
    up_node->dnodes.push_back(down_node);           // connect read_node with internode
    down_node->unodes.push_back(up_node);
}


/**Function*************************************************************
    disconnect two nodes
***********************************************************************/
void circuit::disconnectNodes(node *down_node, node *up_node){
    up_node->dnodes.erase(remove(up_node->dnodes.begin(), up_node->dnodes.end(), down_node),
                          up_node->dnodes.end()); // remove old connection
    down_node->unodes.erase(remove(down_node->unodes.begin(), down_node->unodes.end(), up_node),
                            down_node->unodes.end());
}


/**Function*************************************************************
    method to return node gtype value by gate name
***********************************************************************/
int circuit::gateToInt(const string &gate_string) {
    int num;
    if(gate_string=="PI")           num = 0;
    else if(gate_string=="SP")      num = 1;
    else if(gate_string=="XOR")     num = 2;
    else if(gate_string=="XNOR")    num = 3;
    else if(gate_string=="OR")      num = 4;
    else if(gate_string=="NOR")     num = 5;
    else if(gate_string=="AND")     num = 6;
    else if(gate_string=="NAND")    num = 7;
    else if(gate_string=="NOT")     num = 8;
    else if(gate_string=="DFF")     num = 9;
    else if(gate_string=="SDFF")    num = 10;
    else num = 11;
    return num;
}


/**Function*************************************************************
    method to return gate name by node gtype value
***********************************************************************/
string circuit:: intToGate(int gate_type){
    switch(gate_type) {
        case 0: return("PI");
        case 1: return("SP");
        case 2: return("XOR");
        case 3: return("XNOR");
        case 4: return("OR");
        case 5: return("NOR");
        case 6: return("AND");
        case 7: return("NAND");
        case 8: return("NOT");
        case 9: return("DFF");
        case 10: return("SDFF");
        default: return("NONE");
    }
}


/**Function*************************************************************
    Construct a node in PI gtype by node name
***********************************************************************/
node* circuit :: construct_PI(string& name){
    node* new_node = new node(name, circuit::gateToInt("PI"));
    Node_list.push_back(new_node);
    Pinput.push_back(new_node);
    nameToNode[new_node->node_name] = new_node;
    new_node->initial_node = true;
    return new_node;
}


/**Function*************************************************************
    Construct a node by node name
***********************************************************************/
node* circuit :: construct_Node(string& name){
    node* new_node = new node(name);
    Node_list.push_back(new_node);
    nameToNode[new_node->node_name] = new_node;
    return new_node;
}


/**Function*************************************************************
    Construct a node by node name and gtype
***********************************************************************/
node* circuit :: construct_Node(string& name, int int_gate_type){
    node* new_node = new node(name, int_gate_type);
    Node_list.push_back(new_node);
    nameToNode[new_node->node_name] = new_node;
    return new_node;
}



int circuit :: Get_Gate_Total_Num (){
    int num = 0;
    for(const auto& i : this->gateCounter) num+=i.second;
    return num;
}

