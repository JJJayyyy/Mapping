//
// Created by Jay on 4/22/21.
//
#include "circuit.h"

/**Function*************************************************************
    circuit constructor
***********************************************************************/
circuit::circuit(const string& file_name, bool readFile){
    if(readFile){
        Read_Bench_File(file_name);
        Levelization();
//        Depth_Balancing();
        Splitter_Binary_Tree_Insertion();
        Levelization();
        cout << endl;
    }else{
        circuit_name = file_name;
    }
}


/**Function*************************************************************
    circuit deconstructor
***********************************************************************/
circuit::~circuit() {
    for(auto& i: Node_list) delete i;
    vector<node*>().swap(Node_list);
    vector<node*>().swap(Poutput);
    vector<node*>().swap(Pinput);
    nameToNode.clear();
    gateCounter.clear();
}


/**Function*************************************************************
    slides the line to info
***********************************************************************/
vector<string> Read_Line_Helper(string& line){
    istringstream lineStream (line);
    vector<string> info;
    string buff;
    while(lineStream){
        lineStream >> buff;
        info.push_back(buff);
    }
    info.pop_back();
    for(int i=0; i<info.size(); i++){
        if (i==2) {
            info[i-1] = info[i].substr(0, info[i].find('('));
            transform(info[i-1].begin(),info[i-1].end(),info[i-1].begin(),::toupper);
            if(info[i-1]=="DFF" || info[i-1]=="NOT" || info[i-1]=="SDFF")
                info[i] = info[i].substr(info[i].find('(')+1, info[i].find(')')-info[i].find('(')-1);
            else
                info[i] = info[i].substr(info[i].find('(')+1, info[i].find(',')-info[i].find('(')-1);
        }else if(i==info.size()-1) info[i] = info[i].substr(0, info[i].size()-1);
        else if (i > 2) info[i] = info[i].substr(0, info[i].find(','));
    }
    return info;
}


/**Function*************************************************************
    read the bench file into a netlist
***********************************************************************/
void circuit::Read_Bench_File(const string& file_name){
    this->circuit_name = file_name;
    string line;
    node* node_read;
    node* node_read_unode;
    string nodeName;

    circuit_infile.open(file_name, ios::in);
    if (!circuit_infile.is_open()) {
        std::cout << "Error opening file\n";
        return;
    }

    while(!circuit_infile.eof()) {
        getline(circuit_infile, line);
        if (strncmp(line.c_str(), "INPUT", 5) == 0) {
            nodeName = line.substr(line.find('(')+1, line.find(')') - line.find('(')-1);
            construct_PI(nodeName);
        } else if (strncmp(line.c_str(), "OUTPUT", 5) == 0){
            nodeName = line.substr(line.find('(')+1, line.find(')') - line.find('(')-1);
            node_read = construct_Node(nodeName);
            Poutput.push_back(node_read);
        }else{
            vector<string> info;
            if (line[0]!='#')   info = Read_Line_Helper(line);                // 0: out_node, 1: gate, 2-4, input gate;
            if (info.empty()) continue;
            // logic conversion:    NAND->AND, NOR->OR, XNOR->XOR
            if(info[1]=="NOR" || info[1]=="NAND" || info[1]=="XNOR"){
                int new_gate_type = gateToInt(info[1])-1;
                // output node does not exist, add output node into the circuit
                if (nameToNode.find(info[0]) == nameToNode.end()) {
                    string inter_node_name = "I";                              // set internode name = I+node_name
                    inter_node_name.append(info[0]);
                    node_read_unode = construct_Node(inter_node_name, new_gate_type);
                    // set read node to NOT
                    node_read = construct_Node(info[0], gateToInt("NOT"));
                }
                else {
                    string inter_node_name = "I";
                    inter_node_name.append(info[0]);                        // set internode = I+node_name
                    node_read_unode = construct_Node(inter_node_name, new_gate_type);
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt("NOT");
                }
                connectNodes(node_read, node_read_unode);
                node_read = node_read_unode;
            }
            else {
                if (nameToNode.find(info[0]) == nameToNode.end()) {         // add output node into the circuit
                    node_read = construct_Node(info[0], gateToInt(info[1]));
                } else {
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt(info[1]);
                    if(info[1] == "DFF")    node_read->initial_node = true;
                }
            }
            // connect read node with its upstream nodes
            for(int i = 2;  i < info.size(); i++){
                if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                    node_read_unode = new node(info[i]);        // create unode if it not in the map
                    Node_list.push_back(node_read_unode);
                    nameToNode[info[i]] = node_read_unode;
                } else{
                    node_read_unode = nameToNode[info[i]];
                }
                connectNodes(node_read,node_read_unode);
            }
        }
    }
    cout << "Circuit file read is done" << endl;
    circuit_infile.close();
}


/**Function*************************************************************
    Levelization by initialize all the PI and DFF to level 1
***********************************************************************/
void circuit::Levelization() {
    int leveled_node = 0;
    // initialize all the node's level and set all the PI or DFF to 1;
    for(auto& node : Node_list) {
        node->level = -1;
        node->initial_node = false;
        if(node->gtype == gateToInt("PI") || node->gtype == gateToInt("DFF")){
            node->level = 1;
            node->initial_node = true;
            leveled_node++;
        }
    }

    while(leveled_node<Node_list.size()){
        for(auto& node : Node_list){
//            cout << "node name: " << node->node_name << " level: " << node->level << endl;
            if (node->level > 0) continue;                     // skip the leveled nodes
            int max_fin_lev = 0;                                // record the maximum level of upstream nodes
            int num_fin_lev = 0;                                // count number of upstream nodes already leveled
            if(node->gtype >= gateToInt("XOR") && node->gtype != gateToInt("DFF")){
                for(auto& up_node : node->unodes){
                    max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
                    if(up_node->level != -1)    num_fin_lev++;
                }
                if(num_fin_lev == node->unodes.size()){
                    node->level = max_fin_lev+1;
                    max_lvl = (max_lvl>node->level) ? max_lvl : node->level;
                    leveled_node++;
                }
            } else if(node->gtype == gateToInt("SP")){            // branch
                for(auto& up_node : node->unodes){
                    max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
                    if(up_node->level != -1)    num_fin_lev++;
                }
                if(num_fin_lev == node->unodes.size()){
                    node->level = max_fin_lev;
                    leveled_node++;
                }
            }
            if(leveled_node >= Node_list.size()) break;
        }
    }

    // update level and gate infos
    levelList.clear();
    gateCounter.clear();
    totalGateNum = 0;
    max_lvl = INT_MIN;
    max_fin = 0;
    max_fout = 0;
    for(auto& cur_node : Node_list) {
        levelList[cur_node->level].push_back(cur_node->node_name);
        if(cur_node->gtype != gateToInt("PI")){
            gateCounter[intToGate(cur_node->gtype)] += 1;
            totalGateNum++;
            if(cur_node->level > max_lvl)           max_lvl = cur_node->level;
            if(cur_node->unodes.size() > max_fin)   max_fin = int (cur_node->unodes.size());
        }
        if(cur_node->dnodes.size() > max_fout)  max_fout = int (cur_node->dnodes.size());
    }

    this->level_flag = 1;
    cout << "Levelization is done" << endl;
}


/**Function*************************************************************
    Levelization base on the PI's level
***********************************************************************/
void circuit::Levelization_By_PI(){
    max_lvl = 0;
    unsigned long leveled_node = 0;

    // initialize all the node's level
    for(auto& node : Node_list) {
        if(node->gtype == gateToInt("PI")){
            leveled_node++;
            node->initial_node = true;
        }
        else{
            if(node->gtype == gateToInt("DFF")){
                node->level = 1;                                    // set DFF node to 1;
                leveled_node++;
                node->initial_node = true;
            }
            else{
                node->level = -1;
                node->initial_node = false;
            }
        }
    }

    while(leveled_node < Node_list.size()){
        for(auto& node : Node_list){
//            cout << "node name: " << node->node_name << " level: " << node->level << endl;
        if (node->level > 0) continue;                             // skip the leveled nodes
            int max_fin_lev = 0;                                    // record the maximum level of upstream nodes
            int num_fin_lev = 0;                                    // count number of upstream nodes already leveled
            if(node->gtype >= gateToInt("XOR") && node->gtype != gateToInt("DFF")){
                for(auto& up_node : node->unodes){
                    max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
                    if(up_node->level != -1)    num_fin_lev++;
                }
                if(num_fin_lev == node->unodes.size()){
                    node->level = max_fin_lev+1;
                    max_lvl = (max_lvl>node->level) ? max_lvl : node->level;
                    leveled_node++;
                }
            }
            else if(node->gtype != gateToInt("SP")){     // branch
                for(auto& up_node : node->unodes){
                    max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
                    if(up_node->level != -1)    num_fin_lev++;
                }
                if(num_fin_lev == node->unodes.size()){
                    node->level = max_fin_lev;
                    leveled_node++;
                }
            }
            if(leveled_node >= Node_list.size()) break;
        }
    }
    this->level_flag = 1;
//    cout << "Levelization is done based on PIs\n" << endl;
}


/**Function*************************************************************
    Balance the circuit depth
***********************************************************************/
void circuit::Depth_Balancing(){
    if(level_flag != 1) {
        cout << "circuit needs to be leveled first before balancing the depth" << endl;
        return;
    }
    node* added_sDFF;
    node* upNode_db;
    node* downNode_db;
    node* cur_node;
    int size = Node_list.size();
    for(int i = 0; i < size; i++){
        cur_node = Node_list[i];
        if(cur_node->gtype >= gateToInt("XOR") && cur_node->gtype < gateToInt("NOT")){
            int max_fin_lev = 0;                                         // record the maximum level of upstream nodes
            for(auto& up_node : cur_node->unodes){                       // find the maximum level of upstream nodes
                max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
            }
            for(auto& up_node : cur_node->unodes){
                if (up_node->gtype == gateToInt("SDFF"))
                    continue;
                upNode_db = up_node;
                downNode_db = cur_node;
                int numSDFF = max_fin_lev - up_node->level;
                for(int i = 0; i< numSDFF; i++){   // iterate to add SDFF
                    string name = "D" + to_string(SDFFNameCounter);
                    added_sDFF = construct_Node(name, gateToInt("SDFF"));
                    SDFFNameCounter++;
                    disconnectNodes(downNode_db,upNode_db);
                    connectNodes(added_sDFF, upNode_db);
                    connectNodes(downNode_db,added_sDFF);
                    upNode_db = added_sDFF;
                }
            }
        }
    }
    cout << "depth balancing done" << endl;
}


//TODO: remove?
void circuit::cut_Depth_balancing(){
    if(level_flag != 1) cout << "circuit needs to be leveled first" << endl;
    node* added_sDFF;
    node* upNode_db;
    node* downNode_db;
    for(auto& cur_node : Node_list){
        if(cur_node->node_name[0] == 'D' && cur_node->level ==0) continue;
        int max_fin_lev = 0;                                // record the maximum level of upstream nodes
        if(cur_node->gtype >= 2 && cur_node->gtype <= 10){
            // find the maximum level of upstream nodes
            for(const auto& up_node : cur_node->unodes)
                max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
            for(auto& up_node : cur_node->unodes){
                if(up_node->node_name[0] == 'D') continue;
                upNode_db = up_node;
                downNode_db = cur_node;
                int numSDFF = max_fin_lev - up_node->level;
                for(int i = 0; i< numSDFF; i++){            // iterate to add SDFF
                    string name = "D" + to_string(SDFFNameCounter);
                    added_sDFF = construct_Node(name, gateToInt("SDFF"));
                    SDFFNameCounter++;
                    disconnectNodes(downNode_db,upNode_db);
                    connectNodes(added_sDFF, upNode_db);
                    connectNodes(downNode_db,added_sDFF);
                    upNode_db = added_sDFF;
                }
            }
        }
    }
    cout << "depth balancing done" << endl;
}


/**Function*************************************************************
    Insert the Splitter binary tree into the circuit
***********************************************************************/
void circuit::Splitter_Binary_Tree_Insertion() {
    if(level_flag != 1) {
        cout << "circuit needs to be leveled first before inserting the splitter binary trees" << endl;
        return;
    }
    node *added_sta;
    node *upNode_sta;
    node *downNode_sta;
    node* cur_node;
    int nodesize = Node_list.size();
    for(int i = 0; i < nodesize; i++){
        cur_node = Node_list[i];
        if (cur_node->dnodes.size() > 1 && cur_node->gtype != gateToInt("SP")) {
            upNode_sta = cur_node;
            unsigned long size = cur_node->dnodes.size()-1;
            for (int d_idx = 0; d_idx < size; d_idx++) {
                downNode_sta = cur_node->dnodes[0];
                string name = "s" + to_string(splitterNameCounter);                         // create node sp1
                splitterNameCounter++;
                added_sta = construct_Node(name, gateToInt("SP"));
                disconnectNodes(downNode_sta, cur_node);
                connectNodes(added_sta,upNode_sta);
                connectNodes(downNode_sta,added_sta);
                if(d_idx == size-1){
                    downNode_sta = cur_node->dnodes[0];
                    disconnectNodes(downNode_sta,cur_node);
                    connectNodes(downNode_sta,added_sta);
                }
                upNode_sta = added_sta;
            }
        }
    }
    cout << "Splitter binary trees are inserted" << endl;
}


//TODO: move DFF and inverters from upstream to downstream
void circuit::DFF_optimization(){
    vector<string> reduceNot;
    vector<string> reduceDFF;
    for(auto cur_node : Node_list){
        if(cur_node->gtype == gateToInt("AND") || cur_node->gtype == gateToInt("OR")){
            int upnodeNOTCount = 0;
            int upnodeDFFCount = 0;
            for(auto up_node : cur_node->unodes){
                if(up_node->gtype == gateToInt("SDFF") || up_node->gtype == gateToInt("DFF"))
                    upnodeDFFCount++;
                if(up_node->gtype == gateToInt("NOT"))
                    upnodeNOTCount++;
            }
            if(upnodeNOTCount == cur_node->unodes.size())
                reduceNot.push_back(cur_node->node_name);
            if(upnodeDFFCount == cur_node->unodes.size())
                reduceDFF.push_back(cur_node->node_name);
        }
    }
}