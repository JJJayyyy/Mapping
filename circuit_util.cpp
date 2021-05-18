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
    for(int i=0; i<25;i++)
        cout << "--";
    cout << endl;
    printf(" Node   Type \t    Out    \t\t      In       \n");
    printf("------ ------\t-----------\t\t---------------\n");
    for(const auto& cur_node : Node_list) {
        printf(" %-4s   %-4s\t", cur_node->node_name.c_str(), intToGate(cur_node->gtype).c_str());
        for(int j = 0; j < max_fout; j++){
            if(cur_node->dnodes.size() > j)
                printf("%-4s ", cur_node->dnodes[j]->node_name.c_str());
            else
                printf("%-4s ", " ");
        }
        cout <<"\t\t";
        for(int j = 0; j < max_fin; j++) {
            if(cur_node->unodes.size() > j)
                printf("%-4s ", cur_node->unodes[j]->node_name.c_str());
            else
                printf("%-4s ", " ");
        }
        printf("\n");
    }
    for(int i=0; i<25;i++)
        cout << "--";
    printf("\nPrimary inputs:  ");
    for(const auto& i : Pinput)
        printf("%s ",i->node_name.c_str());
    printf("\nPrimary outputs: ");
    for(const auto& i : Poutput)
        printf("%s ",i->node_name.c_str());
    printf("\n\nNumber of nodes = %lu\n", Node_list.size());
    printf("Number of Gates = %d\n", totalGateNum);
    for(const auto& i : gateCounter) if(i.second>0)
        printf("#%-4s: %-4d\n", i.first.c_str(), i.second);
    printf("\nMaximum level = %d\n", max_lvl);
    for(int lvl = 1; lvl<=max_lvl; lvl++){
        vector<string> lvl_list = levelList[lvl];
        printf("Level %-2d:", lvl);
        for(const auto& nodes_name : lvl_list)
            printf("%-4s ", nodes_name.c_str());
        cout << endl;
    }
    for(int i=0; i<25;i++)
        cout << "--";
    cout << endl;
}

void circuit:: pcGateOnly(){
    if(level_flag != 1) {
        cout << "circuit needs to be leveled first before printing circuit info" << endl;
        return;
    }
    cout << endl;
    cout << "Circuit name: " << circuit_name << endl;
    for(int i=0; i<25;i++)
        cout << "--";
    printf("\nNumber of nodes = %lu\n", Node_list.size());
    printf("Number of Gates = %d\n", totalGateNum);
    for(const auto& i : gateCounter) if(i.second>0)
            printf("#%-4s: %-4d\n", i.first.c_str(), i.second);

    int JJ_num = JJ_Calculation();
    printf("\nMaximum level = %d\n", max_lvl);
    printf("Total JJs: %d\n", JJ_num);
    printf("PND value: %d\n", JJ_num * max_lvl);
    for(int i=0; i<25;i++)
        cout << "--";
    cout << endl;
}

/**Function*************************************************************
    Connect two nodes
***********************************************************************/
void circuit::Connect_Nodes(node *down_node, node *up_node) {
    if(down_node->node_name != up_node->node_name){
        up_node->dnodes.push_back(down_node);           // connect read_node with internode
        down_node->unodes.push_back(up_node);
//        cout << "connect: " << up_node->node_name << " -> " << down_node->node_name << endl;
    }
}



/**Function*************************************************************
    disconnect two nodes
***********************************************************************/
void circuit::Disconnect_Nodes(node *down_node, node *up_node){
    if(down_node->node_name != up_node->node_name) {
        up_node->dnodes.erase(remove(up_node->dnodes.begin(), up_node->dnodes.end(), down_node),
                              up_node->dnodes.end()); // remove old connection
        down_node->unodes.erase(remove(down_node->unodes.begin(), down_node->unodes.end(), up_node),
                                down_node->unodes.end());
//        cout << "disconnect: " << up_node->node_name << " -> " << down_node->node_name << endl;
    }
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
    else if(gate_string=="BUFF")    num = 9;
    else if (gate_string=="MAJ")    num = 10;
    else if(gate_string=="DFF")     num = 11;
    else if(gate_string=="SDFF")    num = 12;
    else num = 13;
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
        case 9: return("BUFF");
        case 10 : return("MAJ");
        case 11: return("DFF");
        case 12: return("SDFF");
        default: return("Not exist type");
    }
}



/**Function*************************************************************
    Construct a node in PI gtype by node name
***********************************************************************/
node* circuit :: Construct_PI(string& name){
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
node* circuit :: Construct_Node(string& name){
    if(nameToNode.find(name) == nameToNode.end()){
        node* new_node = new node(name);
        Node_list.push_back(new_node);
        nameToNode[new_node->node_name] = new_node;
        return new_node;
    } else{
        string errorMessage = circuit_name + " create an exist node: " + name ;  // Exception
        throw runtime_error(errorMessage);
    }
}



/**Function*************************************************************
    Construct a node by node name and gtype
***********************************************************************/
node* circuit :: Construct_Node(string& name, int int_gate_type){
    if(nameToNode.find(name) == nameToNode.end()){
        node* new_node = new node(name, int_gate_type);
        Node_list.push_back(new_node);
        nameToNode[new_node->node_name] = new_node;
        return new_node;
    } else{
        string errorMessage = circuit_name + " create an exist node: " + name ;  // Exception
        throw runtime_error(errorMessage);
    }
}



/**Function*************************************************************
    get the total number of the gates
***********************************************************************/
int circuit :: Get_Gate_Total_Num (){
    int num = 0;
    for(const auto& i : this->gateCounter) num+=i.second;
    return num;
}



/**Function*************************************************************
    Calculate the total circuit Error Rate
***********************************************************************/
//float circuit::Error_Rate_Calculation(){
//    float result = 1;
//    for(auto cur_node : Node_list){
//        if(cur_node->gtype > 0) {
//            result = result*(1-errorRateMap[intToGate(cur_node->gtype)]);
//            /** ER calculation check
//            cout << "current gate: " << intToGate(cur_node->gtype) \
//            << "\tER: " << errorRateMap[intToGate(cur_node->gtype)] << endl;
//            cout <<  "Correct rate: " << result << endl;
//             **/
//        }
//    }
////    cout << "Total error rate of the circuit is: " << 1-result << endl;
//    return 1-result;
//}

double circuit::Error_Rate_Calculation(){
    double result = 1;
    double temp = 0;
    for(auto cur_node : Node_list){
        if(cur_node->gtype > 0) {
            if(intToGate(cur_node->gtype) == "AND"){
                temp = errorRateMap[intToGate(cur_node->gtype)] + 0.0003 * double (cur_node->unodes.size()-2);
                result = result*(1-temp);
            }
            else if(intToGate(cur_node->gtype) == "OR"){
                temp = errorRateMap[intToGate(cur_node->gtype)] + 0.002 * double (cur_node->unodes.size()-2);
                result = result*(1-temp);
            }
            else
                result = result*(1-errorRateMap[intToGate(cur_node->gtype)]);
        }
    }
//    cout << "Total error rate of the circuit is: " << 1-result << endl;
    return 1-result;
}


int circuit::JJ_Calculation(){
    int result = 0;
    int temp = 0;
    for(auto cur_node : Node_list){
        if(cur_node->gtype > 0) {
            if(intToGate(cur_node->gtype) == "AND"){
                temp = JJNumMap[intToGate(cur_node->gtype)] + 3 * int (cur_node->unodes.size()-2);
                result += temp;
            }
            else if(intToGate(cur_node->gtype) == "OR"){
                temp = JJNumMap[intToGate(cur_node->gtype)] + 2 * int (cur_node->unodes.size()-2);
                result += temp;
            }
            else
                result += JJNumMap[intToGate(cur_node->gtype)];
        }
    }
//    cout << "Total error rate of the circuit is: " << result << endl;
    return result;
}