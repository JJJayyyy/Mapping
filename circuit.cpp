//
// Created by Jay on 4/22/21.
//
#include "circuit.h"


circuit::circuit(const string& file_name, bool readFile){
    circuit();
    if(readFile){
        read(file_name);
        this->read_flag = 1;
        levelization();
    }else{
        circuit_name = file_name;
    }
}


//circuit::circuit(circuit* to_copy) {
//    copy(to_copy->Node_list.begin(), to_copy->Node_list.end(), Node_list.begin());
//    copy(to_copy->Pinput.begin(), to_copy->Pinput.end(), Pinput.begin());
//    copy(to_copy->Poutput.begin(), to_copy->Poutput.end(), Poutput.begin());
//    copy(to_copy->nameToNode.begin(), to_copy->nameToNode.end(), nameToNode.begin());
//}

circuit::~circuit() {
    for(auto i: Node_list) delete i;
    for(auto i: Poutput) delete i;
    for(auto i: Pinput) delete i;
    for(const auto& i: nameToNode) delete i.second;
}

circuit::circuit(){
    gateCounter = { {"PI",0},
                    {"SP", 0},
                    {"XOR",0},
                    {"XNOR",0},
                    {"OR",0},
                    {"NOR",0},
                    {"AND",0},
                    {"NAND",0},
                    {"DFF",0},
                    {"NOT",0},
                    {"SDFF",0},
                    {"NONE",0}};


    nameToNode.clear();
    levelList.clear();
    Node_list.clear();
    Pinput.clear();
    Poutput.clear();
}

int circuit::read_done(){
    return this->read_flag;
}


vector<string> readLineHelper(string& line){
    istringstream linestream (line);
    vector<string> info;
    string buff;
    while(linestream){
        linestream >> buff;
        info.push_back(buff);
    }
    info.pop_back();
    for(int i=0; i<info.size(); i++){
        if (i==2) {
            info[i-1] = info[i].substr(0, info[i].find('('));
            transform(info[i-1].begin(),info[i-1].end(),info[i-1].begin(),::toupper);
            if(info[i-1]=="DFF" || info[i-1]=="NOT" || info[i-1]=="SDFF") info[i] = info[i].substr(info[i].find('(')+1, info[i].find(')')-info[i].find('(')-1);
            else info[i] = info[i].substr(info[i].find('(')+1, info[i].find(',')-info[i].find('(')-1);
        }else if(i==info.size()-1) info[i] = info[i].substr(0, info[i].size()-1);
        else if (i > 2) info[i] = info[i].substr(0, info[i].find(','));
    }
//    for(const auto& i: info) cout << i << " ";
//    cout << endl;
    return info;
}


void circuit::read(const string& file_name){
    this->circuit_name = file_name;
    string line;
    circuit_infile.open(file_name, ios::in);
    if (!circuit_infile.is_open()) {
        std::cout << "Error opening file\n";
        return;
    }
    node* node_read;
    node* node_read_unode;
    string nodeName;

    while(!circuit_infile.eof()) {
        getline(circuit_infile, line);
        if (strncmp(line.c_str(), "INPUT", 5) == 0) {
            nodeName = line.substr(line.find('(')+1, line.find(')') - line.find('(')-1);
            node_read = new node(nodeName, gateToInt("PI"));
            Node_list.push_back(node_read);
            nameToNode[nodeName] = node_read;
            Pinput.push_back(node_read);
            gateCounter["PI"]+=1;                                    // update gate counter
        } else if (strncmp(line.c_str(), "OUTPUT", 5 ) == 0){
            nodeName = line.substr(line.find('(')+1, line.find(')') - line.find('(')-1);
            node_read = new node(nodeName);
            Node_list.push_back(node_read);
            nameToNode[nodeName] = node_read;
            Poutput.push_back(node_read);
        }else{
            vector<string> info;
            if (line[0]!='#'){
                info = readLineHelper(line);                        // 0: out_node, 1: gate, 2-4, input gate;
            }
            if (info.empty()) continue;
            if(info[1]=="NOR" || info[1]=="NAND" || info[1]=="XNOR"){          // convert NAND->AND, NOR->OR, XNOR->XOR
                int new_gate_type = gateToInt(info[1])-1;
                if (nameToNode.find(info[0]) == nameToNode.end()) {         // add output node into the circuit
                    string inter_node_name = "I";                               // set internode = I+node_name
                    inter_node_name.append(info[0]);
                    node_read_unode = new node(inter_node_name, new_gate_type);
                    Node_list.push_back(node_read_unode);
                    nameToNode[inter_node_name] = node_read_unode;

                    // set read node to NOT
                    node_read = new node(info[0], gateToInt("NOT"));
                    Node_list.push_back(node_read);
                    nameToNode[info[0]] = node_read;
                } else {
                    string inter_node_name = "I";
                    inter_node_name.append(info[0]);                        // set internode = I+node_name
                    node_read_unode = new node(inter_node_name, new_gate_type);
                    Node_list.push_back(node_read_unode);
                    nameToNode[inter_node_name] = node_read_unode;

                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt("NOT");
                }
                connectNodes(node_read, node_read_unode);
                gateCounter["NOT"] += 1;                                // update gate counter
                gateCounter[intToGate(new_gate_type)] += 1;
                node_read = node_read_unode;
            } else {
                if (nameToNode.find(info[0]) == nameToNode.end()) {         // add output node into the circuit
                    node_read = new node(info[0], gateToInt(info[1]));
                    Node_list.push_back(node_read);
                    nameToNode[info[0]] = node_read;
                } else {
                    node_read = nameToNode[info[0]];
                    node_read->gtype = gateToInt(info[1]);
                }
                gateCounter[info[1]] += 1;                                    // update gate counter
            }
            // connect read node with upnodes
            for(int i = 2;  i < info.size(); i++){
                if(nameToNode.find(info[i])==nameToNode.end()){    //setup relationship between out and in
                    node_read_unode = new node(info[i]);        // create unode if it not in the map
                    Node_list.push_back(node_read_unode);
                    nameToNode[info[i]] = node_read_unode;
                } else{
                    node_read_unode = nameToNode[info[i]];
                }
                connectNodes(node_read,node_read_unode);
                if(node_read->unodes.size()>max_fin)            max_fin = node_read->unodes.size();
                if(node_read_unode->dnodes.size()>max_fout)    max_fout= node_read_unode->dnodes.size();
            }
        }
    }
    cout << "Circuit file read is done\n" << endl;
    circuit_infile.close();
    depth_balancing();
    spliterTreeAdder();
}


void circuit::connectNodes(node *down_node, node *up_node) {
    up_node->dnodes.push_back(down_node);           // connect read_node with internode
    down_node->unodes.push_back(up_node);
}

void circuit::disconnectNodes(node *down_node, node *up_node){
    up_node->dnodes.erase(remove(up_node->dnodes.begin(), up_node->dnodes.end(), down_node),
                           up_node->dnodes.end()); // remove old connection
    down_node->unodes.erase(remove(down_node->unodes.begin(), down_node->unodes.end(), up_node),
                            down_node->unodes.end());
}

void circuit:: pc(){
    levelization();
    node* np_pc;
    printf(" Node   Type \t    Out    \t\t      In       \n");
    printf("------ ------\t-----------\t\t---------------\n");
    for(int i = 0; i< Node_list.size(); i++) {
        np_pc = Node_list[i];
        printf(" %-4s   %-4s\t", np_pc->node_name.c_str(), intToGate(np_pc->gtype).c_str());
        for(int j = 0; j < max_fout; j++){
            if(np_pc->dnodes.size()>j) printf("%-4s ", np_pc->dnodes[j]->node_name.c_str());
            else printf("%-4s ", " ");
        }
        cout <<"\t\t";
        for(int j = 0; j < max_fin; j++) {
            if(np_pc->unodes.size()>j) printf("%-4s ", np_pc->unodes[j]->node_name.c_str());
            else printf("%-4s ", " ");
        }
        printf("\n");
    }
    for(int i=0; i<25;i++) cout << "--";
    printf("\nPrimary inputs:  ");
    for(auto & i : Pinput) printf("%s ",i->node_name.c_str());
    printf("\n");
    printf("Primary outputs: ");
    for(auto & i : Poutput) printf("%s ",i->node_name.c_str());
    printf("\n\n");
    printf("Number of nodes = %lu\n", Node_list.size());
    for(auto& i : gateCounter) if(i.second>0) printf("%-4s: %-4d\n", i.first.c_str(), i.second);
    printf("Maximum level = %d\n", max_lvl);
    for(const auto& lvl : levelList){
        printf("level %-2d:", lvl.first);
        for(const auto& nodes_name : lvl.second)
        printf("%-4s ", nodes_name.c_str());
        cout << endl;
    }
    cout << endl;
}


void circuit::levelization() {
    int leveled_node = 0;
    for(auto node : Node_list) {
        node->level = -1;                           // initialize all the node's level
        if(node->gtype == 0){
            node->level = 1;    // set all the PI to 1;
            leveled_node++;
        }
        if(node->gtype == 9){
            node->level = 0;    // set DFF node to 0;
            leveled_node++;
        }
    }
    while(leveled_node<Node_list.size()){
        for(auto node : Node_list){
//            cout << "node name: " << node->node_name << " level: " << node->level << endl;
            if (node->level >= 0) continue;                     // skip the leveled nodes
            int max_fin_lev = 0;                                // record the maximum level of upstream nodes
            int num_fin_lev = 0;                                // count number of upstream nodes already leveled
            if(node->gtype >= 2 && node->gtype != 9){
                for(auto up_node : node->unodes){
                    max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
                    if(up_node->level != -1)    num_fin_lev++;
                }
                if(num_fin_lev == node->unodes.size()){
                    node->level = max_fin_lev+1;
                    leveled_node++;
                }
            } else if(node->gtype == 1){            // branch
                for(auto up_node : node->unodes){
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
    levelList.clear();
    for(auto node : Node_list) levelList[node->level].push_back(node->node_name);
    max_lvl = Poutput[0]->level;
    this->level_flag = 1;
    cout << "\nLevelization is done\n" << endl;
}


void circuit::depth_balancing(){
    levelization();
    node* added_sDFF;
    node* upNode_db;
    node* downNode_db;
    int idx_SDFF = 0;
    for(auto cur_node : Node_list){
        int max_fin_lev = 0;                                // record the maximum level of upstream nodes
        if(cur_node->gtype >= 2 && cur_node->gtype <= 8){
            for(auto up_node : cur_node->unodes){               // find the maximum level of upstream nodes
                max_fin_lev = (max_fin_lev>up_node->level) ? max_fin_lev : up_node->level;
            }
            for(auto up_node : cur_node->unodes){
                if (up_node->gtype == gateToInt("DFF") || up_node->gtype == gateToInt("SDFF")) continue;
                upNode_db = up_node;
                downNode_db = cur_node;
                for(int i = 0; i< max_fin_lev - up_node->level; i++){            // iterate to add SDFF
                    string name = "D" + to_string(idx_SDFF);
                    added_sDFF = new node(name, gateToInt("SDFF"));
                    idx_SDFF++;
                    Node_list.push_back(added_sDFF);
                    nameToNode[name] = added_sDFF;
                    gateCounter["SDFF"]+=1;
                    disconnectNodes(downNode_db,upNode_db);
                    connectNodes(added_sDFF, upNode_db);
                    connectNodes(downNode_db,added_sDFF);
                    upNode_db = added_sDFF;
                }
            }
        }
    }
    cout << "\ndepth balancing done\n" << endl;
}


void circuit::spliterTreeAdder() {
    node *added_sta;
    node *upNode_sta;
    node *downNode_sta;
    int idx_spliter = 0;
    for (auto cur_node : Node_list) {
        if (cur_node->dnodes.size()>1 && cur_node->gtype!=1) {
            upNode_sta = cur_node;
            int size = cur_node->dnodes.size()-1;
            for (int d_idx = 0; d_idx < size; d_idx++) {
                downNode_sta = cur_node->dnodes[0];
                string name = "s" + to_string(idx_spliter);                         // create node sp1
                added_sta = new node(name, gateToInt("SP"));
                idx_spliter++;
                Node_list.push_back(added_sta);
                nameToNode[name] = added_sta;
                gateCounter["SP"] += 1;
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
    cout << "\nSpliter trees are inserted\n" << endl;
}


//void circuit::deleteOldCut(node* cur_node, vector<string> node_name){
//    for(auto d_n : cur_node->dnodes){
//        if(d_n->node_name != node_name){
//            deleteOldCut(d_n, node_name);
//            cout << cur_node->node_name << endl;
//            delete cur_node;
//        }
//    }
//}

//TODO: \'
//void circuit::replaceCutwithQM(vector<string> boolEqn, node* root_node){
//    string node_n;
//    string temp;
//    node* cur_node;
//    node* up_node;
//    int idx = 0;
//    if(boolEqn.size() > 1)  root_node->gtype = gateToInt("OR");
//    else if (boolEqn[0].size()<5) root_node->gtype = gateToInt("AND");
//    for(const auto& eqn : boolEqn){
//        string name = "N";
//        cur_node = new node(name.append(to_string(idx)), gateToInt("AND"));
//        nameToNode[cur_node->node_name] = cur_node;
//        Node_list.push_back(cur_node);
//        connectNodes(root_node, cur_node);
//        temp = eqn;
//        if(eqn.find('*') != 0){
//            while (temp.find('*') != 0){
//                node_n = eqn.substr(0, eqn.find('*'));
//                temp = eqn.substr(eqn.find('*')+1);
//                //TODO: \'
////            if(node_n.find('\'')==0){
//                up_node = nameToNode[node_n];
////            }else{cur_node = nameToNode[node_n.substr(0,node_n.length()-1)];}
//                connectNodes(cur_node, up_node);
//            }
//        }else{
//            up_node = nameToNode[node_n];
//            connectNodes(cur_node, up_node);
//        }
//    }
//    levelization();
//    depth_balancing();
//    spliterTreeAdder();
//}

void circuit :: replaceCutwithQM(vector<node*> boolEqn, node* root_node) {
    root_node->unodes[0]->unodes.clear();
    root_node->unodes.clear();
    for (auto x : boolEqn){
        connectNodes(root_node, x);
    }
    root_node->gtype = 6;
}

int circuit::gateToInt(const string &gate_string) {
    int num;
    if(gate_string=="PI")           num = 0;
    else if(gate_string=="SP")    num = 1;
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


string circuit:: intToGate(int gate_type){
    switch(gate_type) {
        case 0: return("PI");
        case 1: return("SPLIT");
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


int circuit :: getGateTotalNum (){
    int num = 0;
    for(const auto& i : this->gateCounter) num+=i.second;
    return num;
}


int circuit :: countGate (){
    gateCounter.clear();
    for(auto cur_node : Node_list){
        gateCounter[intToGate(cur_node->gtype)]+=1;
    }
}

