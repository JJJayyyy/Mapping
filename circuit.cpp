//
// Created by Jay on 4/22/21.
//
#include "circuit.h"

/**Function*************************************************************
    circuit constructor
***********************************************************************/
circuit::circuit(const string& file_name, bool readFile){
    if(readFile){
        cout << "##########################" << endl;
        if(Read_Bench_File(file_name)){
            Splitter_Insertion();
            Levelization();
            cout << "##########################" << endl;
        } else{
            delete this;
        }
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
            if(info[i-1]=="DFF" || info[i-1]=="NOT" || info[i-1]=="SDFF" || info[i-1]=="BUFF")
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
bool circuit::Read_Bench_File(const string& file_name){
    this->circuit_name = file_name;
    string line;
    node* node_read;
    node* node_read_unode;
    string nodeName;

    circuit_infile.open(file_name, ios::in);
    if (!circuit_infile.is_open()) {
        std::cout << "Error opening file\n";
        return false;
    }

    while(!circuit_infile.eof()) {
        getline(circuit_infile, line);
        if (strncmp(line.c_str(), "INPUT", 5) == 0) {
            nodeName = line.substr(line.find('(')+1, line.find(')') - line.find('(')-1);
            Construct_PI(nodeName);
        } else if (strncmp(line.c_str(), "OUTPUT", 5) == 0){
            nodeName = line.substr(line.find('(')+1, line.find(')') - line.find('(')-1);
            node_read = Construct_Node(nodeName);
            Poutput.push_back(node_read);
        }else{
            vector<string> info;
            if (line[0]!='#')   info = Read_Line_Helper(line);                // 0: out_node, 1: gate, 2-4, input gate;
            if (info.empty()) continue;
            // logic conversion:    NAND->AND, NOR->OR, XNOR->XOR
            if(info[1]=="XNOR" || info[1] == "BUFF"){
                int new_gate_type = gateToInt(info[1])-1;
                // output node does not exist, add output node into the circuit
                if (nameToNode.find(info[0]) == nameToNode.end()) {
                    string inter_node_name = "I";                              // set internode name = I+node_name
                    inter_node_name.append(info[0]);
                    node_read_unode = Construct_Node(inter_node_name, new_gate_type);
                    // set read node to NOT
                    node_read = Construct_Node(info[0], gateToInt("NOT"));
                }
                else {
                    string inter_node_name = "I";
                    inter_node_name.append(info[0]);                        // set internode = I+node_name
                    node_read_unode = Construct_Node(inter_node_name, new_gate_type);
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt("NOT");
                }
                Connect_Nodes(node_read, node_read_unode);
                node_read = node_read_unode;

                // connect read node with its upstream nodes
                for(int i = 2;  i < info.size(); i++){
                    if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                        node_read_unode = Construct_Node(info[i]);  // create unode if it not in the map
                    } else{
                        node_read_unode = nameToNode[info[i]];
                    }
                    Connect_Nodes(node_read, node_read_unode);
                }
            }else if(info[1]=="NOR" || info[1]=="NAND"){
                assert(info.size()>=4);
                int new_gate_type = gateToInt(info[1])-1;
                // output node does not exist, add output node into the circuit
                if (nameToNode.find(info[0]) == nameToNode.end()) {
                    string inter_node_name = "I";                              // set internode name = I+node_name
                    inter_node_name.append(info[0]);
                    node_read_unode = Construct_Node(inter_node_name, new_gate_type);
                    // set read node to NOT
                    node_read = Construct_Node(info[0], gateToInt("NOT"));
                }
                else {
                    string inter_node_name = "I";
                    inter_node_name.append(info[0]);                        // set internode = I+node_name
                    node_read_unode = Construct_Node(inter_node_name, new_gate_type);
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt("NOT");
                }
                Connect_Nodes(node_read, node_read_unode);
                node_read = node_read_unode;

                // connect read node with its upstream nodes
                int subnodeNum = 0;
                int Size = int(info.size() - 2);
                if(Size > 4){
                    if(int(Size%4) != 0)
                        subnodeNum = Size/4+1;
                    else
                        subnodeNum = (Size-Size%4)/4;
                    // connect read node with its upstream nodes
                    for(int j = 0;  j < subnodeNum; j++){
                        int subSize = (Size > 4) ? 4 : Size;
//                        cout << "subSize: " << subSize << endl;
                        Size -= subSize;
                        if(subSize > 1){
                            string name = "m";
                            name.append(to_string(newNodeNameCounter));
                            newNodeNameCounter++;
                            node* subNode = Construct_Node(name, gateToInt(info[1])-1);
                            Connect_Nodes(node_read, subNode);
                            for(int i = 2+j*4;  i < subSize+2+j*4; i++){
                                if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                                    node_read_unode = Construct_Node(info[i]);
                                } else{
                                    node_read_unode = nameToNode[info[i]];
                                }
                                Connect_Nodes(subNode, node_read_unode);
                            }
                        }else if(subSize == 1){
                            if(nameToNode.find(info[info.size()-1])==nameToNode.end()){    //setup relationship between out and in
                                node_read_unode = Construct_Node(info[info.size()-1]);
                            } else{
                                node_read_unode = nameToNode[info[info.size()-1]];
                            }
                            Connect_Nodes(node_read, node_read_unode);
                        } else{
                            string errorMessage = "NAND/OR fan-in problem";  // Exception
                            throw runtime_error(errorMessage);
                        }
                    }
                }else{
                    for(int i = 2;  i < info.size(); i++){
                        if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                            node_read_unode = Construct_Node(info[i]);
                        } else{
                            node_read_unode = nameToNode[info[i]];
                        }
                        Connect_Nodes(node_read, node_read_unode);
                    }
                }
            }else if(info[1]=="XOR" || info[1] == "DFF" || info[1] == "NOT"){
                if (nameToNode.find(info[0]) == nameToNode.end()) {         // add output node into the circuit
                    node_read = Construct_Node(info[0], gateToInt(info[1]));
                } else {
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt(info[1]);
                    if(info[1] == "DFF")    node_read->initial_node = true;
                }

                // connect read node with its upstream nodes
                for(int i = 2;  i < info.size(); i++){
                    if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                        node_read_unode = Construct_Node(info[i]);       // create unode if it not in the map
                    } else{
                        node_read_unode = nameToNode[info[i]];
                    }
                    Connect_Nodes(node_read, node_read_unode);
                }

            }else if(info[1]=="AND" || info[1]=="OR"){
                assert(info.size()>=4);
                if (nameToNode.find(info[0]) == nameToNode.end()) {         // add output node into the circuit
                    node_read = Construct_Node(info[0], gateToInt(info[1]));
                } else {
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt(info[1]);
                }

                int subnodeNum = 0;
                int Size = int(info.size() - 2);
                if(Size > 4){
                    if(int(Size%4) != 0)
                        subnodeNum = Size/4+1;
                    else
                        subnodeNum = (Size-Size%4)/4;
                    // connect read node with its upstream nodes
                    for(int j = 0;  j < subnodeNum; j++){
                        int subSize = (Size > 4) ? 4 : Size;
//                        cout << "subSize: " << subSize << endl;
                        Size -= subSize;
                        if(subSize > 1){
                            string name = "m";
                            name.append(to_string(newNodeNameCounter));
                            newNodeNameCounter++;
                            node* subNode = Construct_Node(name, gateToInt(info[1]));
                            Connect_Nodes(node_read, subNode);
                            for(int i = 2+j*4;  i < subSize+2+j*4; i++){
                                if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                                    node_read_unode = Construct_Node(info[i]);
                                } else{
                                    node_read_unode = nameToNode[info[i]];
                                }
                                Connect_Nodes(subNode, node_read_unode);
                            }
                        }else if(subSize == 1){
                            if(nameToNode.find(info[info.size()-1])==nameToNode.end()){    //setup relationship between out and in
                                node_read_unode = Construct_Node(info[info.size()-1]);
                            } else{
                                node_read_unode = nameToNode[info[info.size()-1]];
                            }
                            Connect_Nodes(node_read, node_read_unode);
                        } else{
                            string errorMessage = "AND/OR fan-in problem";  // Exception
                            throw runtime_error(errorMessage);
                        }
                    }
                }else{
                    for(int i = 2;  i < info.size(); i++){
                        if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                            node_read_unode = Construct_Node(info[i]);
                        } else{
                            node_read_unode = nameToNode[info[i]];
                        }
                        Connect_Nodes(node_read, node_read_unode);
                    }
                }
            }else{
                cout << "**************************\n"
                     << "Circuit format has problem\n"
                     << "**************************\n";
                cout << "check node:" << info[0];
                circuit_infile.close();
                return false;
            }
        }
    }
    cout << "Circuit file read is done" << endl;
    circuit_infile.close();
    return true;
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
//            cout << "leveled: " << leveled_node << "\tNode_list size: " << Node_list.size() << endl;
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
    string gate;
    for(auto& cur_node : Node_list) {
        levelList[cur_node->level].push_back(cur_node->node_name);
        if(cur_node->gtype != gateToInt("PI")){
            if(intToGate(cur_node->gtype) != "AND" && intToGate(cur_node->gtype) != "OR"){
                gateCounter[intToGate(cur_node->gtype)] += 1;
                totalGateNum++;
            }else{
                gate = intToGate(cur_node->gtype);
                gate.append(to_string(cur_node->unodes.size()));
                gateCounter[gate] += 1;
                totalGateNum++;
            }
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
    int leveled_node = 0;
    // initialize all the node's level and set all the PI or DFF to 1;
    for(auto& node : Node_list) {
        if(node->gtype == gateToInt("PI") || node->gtype == gateToInt("DFF")){
            assert(node->level >= 1);
            node->initial_node = true;
            leveled_node++;
        }else{
            node->level = -1;
            node->initial_node = false;
        }
    }

    while(leveled_node<Node_list.size()){
        for(auto& node : Node_list){
//            cout << "node name: " << node->node_name << " level: " << node->level << endl;
//            cout << "leveled: " << leveled_node << "\tNode_list size: " << Node_list.size() << endl;
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
    string gate;
    for(auto& cur_node : Node_list) {
        levelList[cur_node->level].push_back(cur_node->node_name);
        if(cur_node->gtype != gateToInt("PI")){
            if(intToGate(cur_node->gtype) != "AND" && intToGate(cur_node->gtype) != "OR"){
                gateCounter[intToGate(cur_node->gtype)] += 1;
                totalGateNum++;
            }else{
                gate = intToGate(cur_node->gtype);
                gate.append(to_string(cur_node->unodes.size()));
                gateCounter[gate] += 1;
                totalGateNum++;
            }
            if(cur_node->level > max_lvl)           max_lvl = cur_node->level;
            if(cur_node->unodes.size() > max_fin)   max_fin = int (cur_node->unodes.size());
        }
        if(cur_node->dnodes.size() > max_fout)  max_fout = int (cur_node->dnodes.size());
    }

    this->level_flag = 1;
//    cout << "Levelization is done based on PIs" << endl;
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
    queue<string> nodeNameSet;
    for(auto cur : Node_list){
        nodeNameSet.push(cur->node_name);
    }
    while(!nodeNameSet.empty()){
        if(nameToNode[nodeNameSet.front()]!= nullptr){
            cur_node = nameToNode[nodeNameSet.front()];
        } else{
            nodeNameSet.pop();
            continue;
        }
        nodeNameSet.pop();
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
                for(int j = 0; j < numSDFF; j++){   // iterate to add SDFF
                    string name = "D" + to_string(SDFFNameCounter);
                    added_sDFF = Construct_Node(name, gateToInt("SDFF"));
                    SDFFNameCounter++;
                    Disconnect_Nodes(downNode_db, upNode_db);
                    Connect_Nodes(added_sDFF, upNode_db);
                    Connect_Nodes(downNode_db, added_sDFF);
                    upNode_db = added_sDFF;
                }
            }
        }
    }
//    cout << "depth balancing done" << endl;
}



//TODO: remove?
/**Function*************************************************************
    Balance the circuit depth
***********************************************************************/
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
                    added_sDFF = Construct_Node(name, gateToInt("SDFF"));
                    SDFFNameCounter++;
                    Disconnect_Nodes(downNode_db, upNode_db);
                    Connect_Nodes(added_sDFF, upNode_db);
                    Connect_Nodes(downNode_db, added_sDFF);
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
                added_sta = Construct_Node(name, gateToInt("SP"));
                Disconnect_Nodes(downNode_sta, cur_node);
                Connect_Nodes(added_sta, upNode_sta);
                Connect_Nodes(downNode_sta, added_sta);
                if(d_idx == size-1){
                    downNode_sta = cur_node->dnodes[0];
                    Disconnect_Nodes(downNode_sta, cur_node);
                    Connect_Nodes(downNode_sta, added_sta);
                }
                upNode_sta = added_sta;
            }
        }
    }
    cout << "Splitter binary trees are inserted" << endl;
}


void circuit::Splitter_Insertion() {
    node *SP_node;
    node *downNode_sta;
    node* cur_node;
    queue<string> nodeNameSet;
    for(auto cur : Node_list){
        nodeNameSet.push(cur->node_name);
    }
    while(!nodeNameSet.empty()){
        if(nameToNode[nodeNameSet.front()]!= nullptr){
            cur_node = nameToNode[nodeNameSet.front()];
        } else{
            nodeNameSet.pop();
            continue;
        }
        nodeNameSet.pop();
        if (cur_node->dnodes.size() > 1 && cur_node->gtype != gateToInt("SP")) {
            unsigned long size = cur_node->dnodes.size();                           //there is not a "-1"
            string name = "s" + to_string(splitterNameCounter);                         // create node sp1
            splitterNameCounter++;
            SP_node = Construct_Node(name, gateToInt("SP"));
            for (int d_idx = 0; d_idx < size; d_idx++) {
                downNode_sta = cur_node->dnodes[0];                 // must be '0'
//                cout << cur_node->node_name << " " << downNode_sta->node_name << endl;
                Disconnect_Nodes(downNode_sta, cur_node);
                Connect_Nodes(downNode_sta, SP_node);
            }
            Connect_Nodes(SP_node, cur_node);
        }
    }
//    cout << "Splitters are inserted" << endl;
}


//TODO: move DFF and inverters from upstream to downstream
/**Function*************************************************************
    DFF optimization
***********************************************************************/
void circuit::Post_Optimization(){
    DFF_Optimization();
    INV_Optimization();
    Levelization();
}


void circuit::DFF_Optimization(){
    vector<string> reduceNot;
    vector<string> reduceDFF;
    int total = 0;
    for(auto& cur_node : Node_list){
        if(cur_node->gtype == gateToInt("SDFF") || cur_node->gtype == gateToInt("NOT"))
            continue;
        if(cur_node->unodes.empty())
            continue;
            // change DFF to NOT
//        cout << "cur: " << cur_node->node_name << endl;
        for(auto up_node : cur_node->unodes){
            int DFFCount = 0;
            if(up_node->gtype == gateToInt("SDFF") || up_node->gtype == gateToInt("NOT")){
                DFF_Optimization_Helper(DFFCount, up_node);
//                cout << DFFCount << endl;
                if(DFFCount%2 == 1)
                    DFFCount-=1;
                total+=DFFCount;
                if(DFFCount>0)
                    DFF_To_INV(DFFCount, up_node);
            }
        }
    }
    cout << "DFF optimization changes " << total << " DFFs" << endl;
}

void circuit::DFF_Optimization_Helper(int& count, node* cur_node){
//    cout << cur_node->node_name <<" " << intToGate(cur_node->gtype) << endl;
    if(cur_node->gtype == gateToInt("SDFF"))
        count++;
    if(cur_node->unodes[0]->gtype == gateToInt("SDFF")|| cur_node->unodes[0]->gtype == gateToInt("NOT") ) {
        DFF_Optimization_Helper(count, cur_node->unodes[0]);
    }
}

void circuit::DFF_To_INV(int& count, node* cur_node){
    if(cur_node->gtype == gateToInt("SDFF")){
        cur_node->gtype = gateToInt("NOT");
        count--;
    }
//    cout << cur_node->node_name <<" " << intToGate(cur_node->gtype) << " " << count << endl;
    if(cur_node->gtype != gateToInt("SDFF") && cur_node->gtype != gateToInt("NOT")){
        cout << "!!!Check: "<< cur_node->node_name <<" " << intToGate(cur_node->gtype) << " " << count << endl;
        string errorMessage = "DFF optimization problem";  // Exception
        throw runtime_error(errorMessage);
    }
    if(count!=0){
        DFF_To_INV(count, cur_node->unodes[0]);
    }
}


void circuit::INV_Optimization(){
    vector<string> reduceNot;
    vector<string> reduceDFF;
    queue<string> nodeNameSet;
    unordered_map<string, string> OUTDFFSwitch;
    node* cur_node;
    int total = 0;
    for(auto& cur : Node_list){
        nodeNameSet.push(cur->node_name);
    }
    while(!nodeNameSet.empty()){
        if(nameToNode.find(nodeNameSet.front()) != nameToNode.end()){
            cur_node = nameToNode[nodeNameSet.front()];
//            cout << "Front name : " << cur_node->node_name << endl;
        } else{
            nodeNameSet.pop();
            continue;
        }
        if(cur_node->unodes.size() < 2){
            nodeNameSet.pop();
            continue;
        }

        nodeNameSet.pop();
        // change DFF to NOT
        int minimumINVNum = INT_MAX;
        for(auto up_node : cur_node->unodes){
            int INVCount = 0;
            if(up_node->gtype == gateToInt("NOT")){
//                cout << "cur: " << cur_node->node_name <<  " \t " << up_node->node_name << " "<< intToGate(up_node->gtype) << endl;
                INV_Optimization_Helper(INVCount, up_node);
            }
            minimumINVNum = minimumINVNum < INVCount ? minimumINVNum : INVCount;
        }
        if(minimumINVNum > 0){
            total += minimumINVNum;
            Remove_Up_INV(minimumINVNum, cur_node);                     // remove the upstream INV
            string DFFname = Add_Down_INV(minimumINVNum, cur_node);     // add INV to downstream to balanced logic
            auto iter = find(Poutput.begin(), Poutput.end(), cur_node);
            if(iter != Poutput.end()){
                OUTDFFSwitch[cur_node->node_name] = DFFname;
            }
        }
    }

    if(!OUTDFFSwitch.empty()){
        cout << "some OUTPUT need to change with DFF" << endl;
        for(auto& out : Poutput){
            if(OUTDFFSwitch.find(out->node_name) != OUTDFFSwitch.end()){
                node* a =  nameToNode[OUTDFFSwitch[out->node_name]];
                node* b =  a->unodes[0];
                while(b->node_name != out->node_name){
                    Delete_Node(a);
                    a = b;
                    b = a->unodes[0];
                }
            }
        }
    }
    cout << "INV optimization move " << total << " INVs" << endl;
}


void circuit::INV_Optimization_Helper(int& count, node* cur_node){
//    cout <<"INV_Helper: " << cur_node->node_name <<"\t" << intToGate(cur_node->gtype)  << "\t" << count << endl;
    if(cur_node->gtype == gateToInt("NOT")){
        count++;
        INV_Optimization_Helper(count, cur_node->unodes[0]);
    }
}


void circuit::Remove_Up_INV(int miniNum, node* cur_node){
    node* up_node;
    node* temp_UP;
    queue<string> upNodeName;
    queue<string> upNodeName2;
    // collect all the down node of cur_node, as cur_node will change later;
    for(auto uN : cur_node->unodes){
        upNodeName.push(uN->node_name);
    }

    while(!upNodeName.empty()) {
        if (nameToNode.find(upNodeName.front()) != nameToNode.end()) {
            up_node = nameToNode[upNodeName.front()];
            assert(up_node->node_name == upNodeName.front());
        } else {
            upNodeName.pop();
            continue;
        }
        upNodeName.pop();
//        cout << "upnode: "<< up_node->node_name <<" " << intToGate(up_node->gtype) << endl;
        for(int j = 0; j < miniNum; j++){
            if(up_node->gtype != gateToInt("NOT")){
                cout << "!!!Check: "<< up_node->node_name <<" " << intToGate(up_node->gtype) << endl;
                string errorMessage = "INV optimization problem";  // Exception
                throw runtime_error(errorMessage);
            }
            temp_UP = up_node->unodes[0];
            Delete_Node(up_node);
            up_node = temp_UP;
        }
        upNodeName2.push(up_node->node_name);
    }

    while(!upNodeName2.empty()) {
        if (nameToNode.find(upNodeName2.front()) != nameToNode.end()) {
            temp_UP = nameToNode[upNodeName2.front()];
        } else {
            upNodeName2.pop();
            continue;
        }
        upNodeName2.pop();
        Connect_Nodes(cur_node, temp_UP);
//        cout << "connect: " << temp_UP->node_name << "->" << cur_node->node_name << endl;
    }
}


string circuit::Add_Down_INV(int miniNum, node* cur_node){
    node* temp_Down;
    string name;
    queue<string> downNodeName;
    queue<string> downNodeName2;
    // collect all the down node of cur_node, as cur_node will change later;
    for(auto dN : cur_node->dnodes){
        downNodeName.push(dN->node_name);
        downNodeName2.push(dN->node_name);
    }
    while(!downNodeName.empty()) {
        if (nameToNode.find(downNodeName.front()) != nameToNode.end()) {
            temp_Down = nameToNode[downNodeName.front()];
        } else {
            downNodeName.pop();
            continue;
        }
        downNodeName.pop();
        Disconnect_Nodes(temp_Down, cur_node);
    }

    for(int i = 0; i < miniNum; i++){
        name = "D";
        name.append(to_string(SDFFNameCounter));
        SDFFNameCounter++;
        temp_Down = Construct_Node(name, gateToInt("NOT"));
        Connect_Nodes(temp_Down, cur_node);
        cur_node = temp_Down;
    }
    while(!downNodeName2.empty()) {
        if (nameToNode.find(downNodeName2.front()) != nameToNode.end()) {
            temp_Down = nameToNode[downNodeName2.front()];
        } else {
            downNodeName2.pop();
            continue;
        }
        downNodeName2.pop();
        Connect_Nodes(temp_Down, cur_node);
    }
    return cur_node->node_name;
}


void circuit::Highfanin_To_Low(int faninSize){
    node* cur_node;
    vector<string> highFaninList;
    bool needHighFaninToLow = false;

    // collect all the down node of cur_node, as cur_node will change later;
    for(auto& cur : Node_list){
        if(intToGate(cur->gtype) == "AND" || intToGate(cur->gtype)=="OR"){
            if(cur->unodes.size()>faninSize){
                highFaninList.push_back(cur->node_name);
                needHighFaninToLow = true;
            }
        }
    }

//    for(const auto& x : highFaninList)
//        cout << x << " ";
//    cout << endl;
    if(!needHighFaninToLow){
        return;
    }

    for(auto& cur : highFaninList){
        cur_node = nameToNode[cur];
        queue<string> subNodeList;
//        cout << "cur: " << cur_node->node_name << endl;
        for(auto up_node : cur_node->unodes){
            subNodeList.push(up_node->node_name);
//            cout << "up: " << up_node->node_name << endl;
        }
        int iterNum = 0;
        int Size = int(cur_node->unodes.size());
        if(Size%faninSize != 0)
            iterNum = Size / faninSize + 1;
        else
            iterNum = (Size - Size % faninSize) / faninSize;
        // connect read node with its upstream nodes
//        cout << "iter: " << iterNum << endl;
        for(int j = 0; j < iterNum; j++){
            int subSize = (Size > faninSize) ? faninSize : Size;
//                        cout << "subSize: " << subSize << endl;
            Size -= subSize;
            if(subSize > 1){
                string name = "m";
                name.append(to_string(newNodeNameCounter));
                newNodeNameCounter++;
                node* subNode = Construct_Node(name, cur_node->gtype);
                Connect_Nodes(cur_node, subNode);
                for(int i = 0;  i < faninSize; i++){
                    node* upnode = nameToNode[subNodeList.front()];
                    subNodeList.pop();
                    Disconnect_Nodes(cur_node, upnode);
                    Connect_Nodes(subNode, upnode);
                }
            }else if(subSize == 1){
                subNodeList.pop();
            } else{
                string errorMessage = "AND/OR fanin convertion problem";  // Exception
                throw runtime_error(errorMessage);
            }
        }
    }
    cout << circuit_name << ": fanin H->L" << endl;
}