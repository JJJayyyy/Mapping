//
// Created by Jay on 4/25/21.
//
vector<vector<string>> K_Feasible_Cut(node* root_node, int k);
void Construct_Cut_Circuit_Helper(circuit* new_circuit, vector<string>& cutPI, node* cur_node, node* new_node);
circuit* Construct_Cut_Circuit(circuit* bench, vector<string>& cutPI, node* rootNode, string& name);

/**Function*************************************************************
    Do the K-feasible cut for current node
***********************************************************************/
vector<vector<string>> K_Feasible_Cut(node* root_node, int k){
    vector<node*> cutPI;
    vector<vector<node*>> cutPIList;
    vector<vector<string>> cutPINameList;
    vector<string> rootUnodeNameList;
    for(auto& up_node : root_node->unodes)
        rootUnodeNameList.push_back(up_node->node_name);

    cutPI.push_back(root_node);
    node* cur_node;
    node *commonNode = nullptr;

    int idx = 0;
    while(cutPI.size()>idx){
        bool flag = false;
        bool jump_flag = false;
        bool specialFlag = false;

        cur_node = cutPI[idx];
        unsigned long size = cutPI.size() - 1 + cur_node->unodes.size();
//        cout << "idx: " << idx << " currentNode: " << cur_node->node_name << endl;

        /*if a cutPI node has a same up stream node with other node's unode in the CutPI
        e.g cutPI = {a b c}, a's unode = {d e}; b's unode = {d f}; =>  cutPI = {c d e f}*/
        int commonCount = 0;
        int i_idx = 0, j_idx = 0;
        if(cutPI.size() > 1){
            for(int i = idx; i < cutPI.size(); i++){
                for(int j = i+1; j < cutPI.size(); j++){
                    for(auto& i_up : cutPI[i]->unodes){
                        for(auto& j_up : cutPI[j]->unodes){
                            if(i_up->node_name == j_up->node_name){
                                commonNode = i_up;
                                commonCount++;
                                i_idx = i;
                                j_idx = j;
                            }
                        }
                    }
                }
            }
            if(commonCount==1){
                int special_size = idx + cutPI[i_idx]->unodes.size() + cutPI[j_idx]->unodes.size() - 1;
                if(k >= special_size)  specialFlag = true;
            }
        }


        if(specialFlag){
            cutPI.push_back(commonNode);
            for(auto i_up : cutPI[i_idx]->unodes){
                if(i_up->node_name != commonNode->node_name)    cutPI.push_back(i_up);
            }
            for(auto j_up : cutPI[j_idx]->unodes){
                if(j_up->node_name != commonNode->node_name)    cutPI.push_back(j_up);
            }
            cutPI.erase(remove(cutPI.begin(), cutPI.end(), cutPI[j_idx]), cutPI.end());
            cutPI.erase(remove(cutPI.begin(), cutPI.end(), cutPI[i_idx]), cutPI.end());
            flag = true;
        } else{
            /*if a cutPI node's unodes has a same node which is already in the CutPI: jump_flag => true
        e.g cutPI = {a b c}, a's unode = {c d};*/
            for(const auto& i : cur_node->unodes)
                for(const auto& j : cutPI)
                    if(j->node_name == i->node_name)
                        jump_flag = true;

            /*if a cutPI node's unodes only has a same node which is already in the CutPI, remove this node
             e.g cutPI = {a b c}, a's unode = {c}   =>     {b, c} is also a feasible cut*/
            if(jump_flag && cur_node->unodes.size() == 1){
                cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node), cutPI.end());
                flag = true;
            }

            /*if a cutPI
             * 1)   not PI;
             * 2)   its upstream nodes does have same nodes in current cutPI;
             * 3)   size value less than k
             * 4)   cur_node gate type is not a branch
             e.g cutPI = {a b}, a's upstream nodes = {c, d}   =>     {b, c, d} is a feasible cut*/
            if(!jump_flag && !cur_node->unodes.empty() && size <= k
               && cur_node->gtype != circuit::gateToInt("PI")
               && cur_node->gtype != circuit::gateToInt("DFF")
               && cur_node->gtype != circuit::gateToInt("SP")){
                for(auto& up_node : cur_node->unodes)    cutPI.push_back(up_node);
                if (size >= 2)    flag = true;      // only consider the cut has 2 more input situation
                cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node), cutPI.end());
            }
            else{                                       // else move to next node in cutPI
                idx++;
            }
        }


        if(flag)    cutPIList.push_back(cutPI);     // add current cutPI to cutPI list
//        for(const auto& j : cutPI) cout << j->node_name << " ";    cout << endl;
//        cout << "idx: " << idx << endl;
    }

    // remove the cut which is just the upstream node of root_node


    if(!cutPIList.empty()){
        for(auto & i : cutPIList){
            vector<string>cutPIName;
            int PICounter = 0;
            for(auto & j : i){
                cutPIName.push_back(j->node_name);
                auto pi_find = find(rootUnodeNameList.begin( ), rootUnodeNameList.end( ), j->node_name);
                if(pi_find != rootUnodeNameList.end())  PICounter++;
            }
            if(PICounter < rootUnodeNameList.size()){
                cutPINameList.push_back(cutPIName);
            }
        }
    }


    if(!cutPINameList.empty()){
        cout << "\nCurrent node: "<< root_node->node_name << endl;
        cout << "cuts: ";
        for(int i = 0; i < cutPINameList.size(); i++){
            cout << "{";
            for(int j = 0; j < cutPINameList[i].size(); j++){
                if(j != cutPINameList[i].size()-1)          cout << cutPINameList[i][j] << " ";
                else    cout << cutPINameList[i][j];
            }
            if(i == cutPINameList.size()-1) cout << "}";
            else    cout << "}, ";
        }
        cout << endl;
    }
    return cutPINameList;
}


/**Function*************************************************************
    form the cut circuit base on the cut Inputs
***********************************************************************/
circuit* Construct_Cut_Circuit(circuit* bench, vector<string>& cutPI, node* rootNode, string& name){
    circuit* new_circuit = new circuit(name, false);
    node* new_node = new node(rootNode->node_name, rootNode->gtype);
    new_circuit->Node_list.push_back(new_node);
    new_circuit->Poutput.push_back(new_node);
    new_circuit->nameToNode[new_node->node_name] = new_node;

    for(auto& pi_name : cutPI){
        new_circuit->construct_PI(pi_name);
    }

    for(auto& cur_node : rootNode->unodes){
        Construct_Cut_Circuit_Helper(new_circuit, cutPI, cur_node, new_node);
    }
    cout << "Cut circuit {" << name << "} is generated" << endl;

//    int minimum_level = INT_MAX;
//    for(auto pi : new_circuit->Pinput) {
//        minimum_level = (bench->nameToNode[pi->node_name]->level<minimum_level) ? bench->nameToNode[pi->node_name]->level : minimum_level;
//    }
//    for(auto pi : new_circuit->Pinput) pi->level = bench->nameToNode[pi->node_name]->level-minimum_level+1;
//    for(auto pout : new_circuit->Poutput) pout->level = bench->nameToNode[pout->node_name]->level-minimum_level+1;
    new_circuit->Levelization();
//    new_circuit->Levelization_By_PI();
//    new_circuit->pc();
//    new_circuit->Depth_Balancing();
//    new_circuit->Splitter_Binary_Tree_Insertion();
//    new_circuit->Levelization_By_PI();
//    new_circuit->pc();
    return new_circuit;
}


/**Function*************************************************************
    recursive method to form nodes in the cutCircuit method
***********************************************************************/
void Construct_Cut_Circuit_Helper(circuit* new_circuit, vector<string>& cutPI, node* cur_node, node* new_node){
    auto pi_find = find( cutPI.begin( ), cutPI.end( ), cur_node->node_name);
    node* new_up_node;
    if (pi_find == cutPI.end())
        new_up_node = new_circuit->construct_Node(cur_node->node_name, cur_node->gtype);
    else{
        new_up_node = new_circuit->nameToNode[cur_node->node_name];
    }
    circuit::connectNodes(new_node, new_up_node);
    if(pi_find == cutPI.end())
        for(auto& up_cur_node : cur_node->unodes)
            Construct_Cut_Circuit_Helper(new_circuit, cutPI, up_cur_node, new_up_node);
}



/**Function*************************************************************
    Construct the cut Circuit based on LUT
***********************************************************************/
circuit* Construct_LUT_Circuit(vector<string>& LUT, unordered_map<string, int>& PI_PO_Level, vector<string> PI, string& PO_name, string& name){
    cout << "from LUT to circuit" << endl;
    circuit* new_circuit = new circuit(name, false);
    node* new_node;
    node* cur_node;
    node* up_1_Node;
    node* up_2_Node;
    node* up_3_Node;
    node* PO;
    int idx = 0;

    // construct PI;
    for(auto& PI_name : PI)   new_circuit->construct_PI(PI_name);

    //  construct NOT node for PI
    if(LUT.size()>1){      //  PO is OR gate
        PO = new_circuit->construct_Node(PO_name, circuit::gateToInt("OR"));
        new_circuit->Poutput.push_back(PO);
        for(const auto& logic : LUT){
            for(int i = 0; i < logic.size(); i++){
                cur_node = new_circuit->nameToNode[PI[i]];
                if(logic[i] == '0' && cur_node->dnodes.empty()){
                    string midNodeName = "M";
                    midNodeName.append(to_string(idx));
                    idx++;
                    new_node = new_circuit->construct_Node(midNodeName, circuit::gateToInt("NOT"));
                    circuit::connectNodes(new_node, cur_node);
                }
            }
        }

        for(const auto& logic : LUT) {
            unordered_map<char, vector<int>> piHash = {{'0', {}}, {'1', {}}, {'-', {}}, {'^', {}}};
            for (int i = 0; i < logic.size(); i++) piHash[logic[i]].push_back(i);
            if (piHash['-'].size() == PI.size()-1) {                                                // only one PI in sub_PO
                if (!piHash['1'].empty()) {
                    up_1_Node = new_circuit->nameToNode[PI[piHash['1'][0]]];             // piHash['1'][0]: get PI idx
                    circuit::connectNodes(PO, up_1_Node);
                }
                else if (!piHash['0'].empty()) {
                    up_1_Node = new_circuit->nameToNode[PI[piHash['0'][0]]]->dnodes[0];  // get this PI is inverse
                    circuit::connectNodes(PO, up_1_Node);
                }
                else {
                    cout << "there is only one ^ char, check the LUT" << endl;
                }
            }
            else if (piHash['-'].size() == 2 && (!piHash['^'].empty())) {                  // sub_PO is XOR
                string subPOName = "M";
                subPOName.append(to_string(idx));
                idx++;
                up_1_Node = new_circuit->construct_Node(subPOName, circuit::gateToInt("XOR"));
                circuit::connectNodes(PO, up_1_Node);
                for (auto i : piHash['^']) {
                    up_2_Node = new_circuit->nameToNode[PI[i]];
                    circuit::connectNodes(up_1_Node, up_2_Node);
                }
            }
            else {                                                                          // sub_PO is AND
                string subPOName = "M";
                subPOName.append(to_string(idx));
                idx++;
                up_1_Node = new_circuit->construct_Node(subPOName, circuit::gateToInt("AND"));
                circuit::connectNodes(PO, up_1_Node);
                if (!piHash['^'].empty()) {                                                 // sub_sub_PO is XOR
                    string sub_subPOName = "M";
                    sub_subPOName.append(to_string(idx));
                    idx++;
                    up_2_Node = new_circuit->construct_Node(sub_subPOName, circuit::gateToInt("XOR"));
                    circuit::connectNodes(up_1_Node, up_2_Node);
                    for (auto i : piHash['^']) {
                        up_3_Node = new_circuit->nameToNode[PI[i]];
                        circuit::connectNodes(up_2_Node, up_3_Node);
                    }
                }
                for (auto i : piHash['0']) {
                    up_2_Node = new_circuit->nameToNode[PI[i]]->dnodes[0];                  // get this PI is inverse
                    circuit::connectNodes(up_1_Node, up_2_Node);
                }
                for (auto i : piHash['1']) {
                    up_2_Node = new_circuit->nameToNode[PI[i]];
                    circuit::connectNodes(up_1_Node, up_2_Node);
                }
            }
        }
    }
    else if(LUT.size() == 1){
        string logic = LUT[0];
        unordered_map<char, vector<int>> piHash = {{'0', {}}, {'1', {}}, {'-', {}}, {'^', {}}};
        for (int i = 0; i < logic.length(); i++) piHash[logic[i]].push_back(i);
        PO = new_circuit->construct_Node(PO_name);
        new_circuit->Poutput.push_back(PO);
        if (piHash['-'].size() == PI.size()-1) {                                                // only one PI in sub_PO
            if (!piHash['1'].empty()) {
                up_1_Node = new_circuit->nameToNode[PI[piHash['1'][0]]];             // piHash['1'][0]: get PI idx
                circuit::connectNodes(PO, up_1_Node);
                PO->gtype = circuit::gateToInt("SDFF");
            }
            else if (!piHash['0'].empty()) {
                up_1_Node = new_circuit->nameToNode[PI[piHash['0'][0]]];  // get this PI is inverse
                circuit::connectNodes(PO, up_1_Node);
                PO->gtype = circuit::gateToInt("NOT");
            }
            else {
                cout << "there is only one ^ char, check the LUT" << endl;
            }
        }
        else if (piHash['-'].size() == 2 && (!piHash['^'].empty())) {                  // sub_PO is XOR
            for (auto i : piHash['^']) {
                up_1_Node = new_circuit->nameToNode[PI[i]];
                circuit::connectNodes(PO, up_1_Node);
                PO->gtype = circuit::gateToInt("XOR");
            }
        }
        else {                                                                     // sub_PO is AND
            for(int i = 0; i < logic.length(); i++){
                cur_node = new_circuit->nameToNode[PI[i]];
                if(logic[i] == '0'){
                    string midNodeName = "M";
                    midNodeName.append(to_string(idx));
                    idx++;
                    new_node = new_circuit->construct_Node(midNodeName, circuit::gateToInt("NOT"));
                    circuit::connectNodes(new_node, cur_node);
                }
            }
            PO->gtype = circuit::gateToInt("AND");
            if (!piHash['^'].empty()) {                                               // sub_sub_PO is XOR
                string subPOName = "M";
                subPOName.append(to_string(idx));
                idx++;
                up_1_Node = new_circuit->construct_Node(subPOName, circuit::gateToInt("XOR"));
                circuit::connectNodes(PO, up_1_Node);
                for (auto i : piHash['^']) {
                    up_2_Node = new_circuit->nameToNode[PI[i]];
                    circuit::connectNodes(up_1_Node, up_2_Node);
                }
            }
            for (auto i : piHash['0']) {
                up_1_Node = new_circuit->nameToNode[PI[i]]->dnodes[0];  // get this PI is inverse
                circuit::connectNodes(PO, up_1_Node);
            }
            for (auto i : piHash['1']) {
                up_1_Node = new_circuit->nameToNode[PI[i]];
                circuit::connectNodes(PO, up_1_Node);
            }
        }
    }

    cout << "LUT Cut circuit is generated" << endl;

    int minimum_level = INT_MAX;
    for(auto pi : new_circuit->Pinput) {
        minimum_level = (PI_PO_Level[pi->node_name]<minimum_level) ? PI_PO_Level[pi->node_name] : minimum_level;
    }
    for(auto pi : new_circuit->Pinput) pi->level = PI_PO_Level[pi->node_name]-minimum_level+1;
    for(auto pout : new_circuit->Poutput) pout->level = PI_PO_Level[pout->node_name]-minimum_level+1;

//    new_circuit->levelization_by_PI();
    new_circuit->Levelization();
    // update the max_lvl and levelList
    new_circuit->max_lvl = new_circuit->Poutput[0]->level;
    for(auto node : new_circuit->Node_list) {
        new_circuit->levelList[node->level].push_back(node->node_name);
        new_circuit->max_fin = (new_circuit->max_fin > node->unodes.size()) ? new_circuit->max_fin : node->unodes.size();
        new_circuit->max_fout = (new_circuit->max_fout > node->dnodes.size()) ? new_circuit->max_fout : node->dnodes.size();
    }
//    /** cut circuit check
    new_circuit->cut_Depth_balancing();
    new_circuit->Splitter_Binary_Tree_Insertion();
    new_circuit->Levelization();
//    new_circuit->levelization_by_PI();
    new_circuit->pc();
//     cout << "node list: ";
//     for(auto n : new_circuit->Node_list) cout << n->node_name << " ";
//     cout << endl;
////     cout << "\nPI list:" << endl;
////     for(auto n : new_circuit->Pinput) cout << n->node_name << "\tlevel: " << n->level << endl;
////     cout << "PO list: " << endl;
////     for(auto n : new_circuit->Poutput) cout << n->node_name << "\tlevel: " << n->level << endl;
////     **/
    cout << endl;
    return new_circuit;
}