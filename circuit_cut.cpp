//
// Created by Jay on 4/25/21.
//
vector<vector<string>> K_Feasible_Cut(node* root_node, int k);
void Construct_Cut_Circuit_Helper(circuit* new_circuit, vector<string>& cutPI, node* cur_node, node* new_node);
circuit* Construct_Cut_Circuit(circuit* bench, vector<string>& cutPI, node* rootNode, string& name, int faninSize);
circuit* Construct_LUT_Circuit(vector<string>& implicantSet, vector<string>& PI, vector<int>& PI_levels, string& PO_name, string& name, int faninSize);


void AddNodeIntoSet(unordered_set<string>& target_set, node* cur_node){
    if(target_set.find(cur_node->node_name) == target_set.end()){
        target_set.insert(cur_node->node_name);
    }else{
        string errorMessage = cur_node->node_name + " already in the Set";  // Exception
        throw runtime_error(errorMessage);
    }
}

/**Function*************************************************************
    Do the K-feasible cut for current node
***********************************************************************/
vector<vector<string>> K_Feasible_Cut(node* root_node, int k){
    vector<node*> cutPI;
    vector<vector<node*>> cutPIList;
    vector<vector<string>> cutPINameList;
    vector<string> rootUnodeNameList;
    unordered_set<string> includeNode;

    for(auto& up_node : root_node->unodes)
        rootUnodeNameList.push_back(up_node->node_name);

    cutPI.push_back(root_node);
    node* cur_node;
    int idx = 0;
//    cout << "\nroot: " << root_node->node_name << endl;
    while(cutPI.size()>idx){
        bool flag = false;
        bool jump_flag = false;
        bool specialFlag = false;

        cur_node = cutPI[idx];
        unsigned long size = cutPI.size() - 1 + cur_node->unodes.size();
//        cout << "idx: " << idx << " currentNode: " << cur_node->node_name << endl;
//        cout << "Included set: ";
//        for(const auto& a : includeNode)
//            cout << a << "\t";
//        cout << endl;

        /*if a cutPI node has a same up stream node with other node's unode in the CutPI
        e.g cutPI = {a b c}, a's unode = {d e}; b's unode = {d f}; =>  cutPI = {c d e f}*/
        int commonCount = 0;
        unordered_set<string> cutPI_UpStream_Set;
        vector<node*> commonNodeSet;
        if(cutPI.size() > 1){
            for(auto &cur : cutPI){
                for(auto &up_cur : cur->unodes){
                    if(cutPI_UpStream_Set.find(up_cur->node_name) == cutPI_UpStream_Set.end()){
                        AddNodeIntoSet(cutPI_UpStream_Set, up_cur);
                    }else{
                        commonNodeSet.push_back(up_cur);
                        commonCount++;
                    }
                }
            }
            if(commonCount > 0){
                if(k >= cutPI_UpStream_Set.size())
                    specialFlag = true;
            }
        }


        if(specialFlag){
            vector<string> commonNodeNameSet;
            for(auto &common : commonNodeSet){
                commonNodeNameSet.push_back(common->node_name);
                cutPI.push_back(common);
//                cout << "add common: " <<  common->node_name << endl;
            }
            vector<string> nodeHaveCommonUpNameSet;
            vector<node*> nodeHaveCommonUpSet;
            for(auto &cur : cutPI){
                for(auto &up_cur : cur->unodes) {
                    for(auto &common : commonNodeSet){
                        if(common->node_name == up_cur->node_name){
                            if(find(nodeHaveCommonUpNameSet.begin(), nodeHaveCommonUpNameSet.end(), cur->node_name)==nodeHaveCommonUpNameSet.end()){
                                nodeHaveCommonUpSet.push_back(cur);
                                nodeHaveCommonUpNameSet.push_back(cur->node_name);
                            }
                        }
                    }
                }
            }

            for(auto &cur : nodeHaveCommonUpSet){
                for(auto &up_cur : cur->unodes){
                    if(find(commonNodeNameSet.begin(), commonNodeNameSet.end(), up_cur->node_name)==commonNodeNameSet.end()){
                        cutPI.push_back(up_cur);
//                        cout << "add else: " <<  up_cur->node_name << endl;
                    }
                }
            }
            for(auto &cur : nodeHaveCommonUpSet){
                AddNodeIntoSet(includeNode, cur);
                cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur), cutPI.end());
            }
            flag = true;
            commonNodeSet.clear();
            nodeHaveCommonUpSet.clear();
            commonNodeNameSet.clear();
            nodeHaveCommonUpNameSet.clear();
        } else{
            /*if a cutPI node's unodes has a same node which is already in the CutPI: jump_flag => true
        e.g cutPI = {a b c}, a's unode = {c d};*/
            for(const auto& i : cur_node->unodes){
                for(const auto& j : cutPI){
                    if(j->node_name == i->node_name)
                        jump_flag = true;
                }
            }

            /*if a cutPI node's unodes has a same node which is already in the CutPI: jump_flag => true
        e.g cutPI = {a b c}, a's unode = {e e};*/
            for (int l = 0; l < cur_node->unodes.size(); l++) {
                for (int i = l+1; i < cur_node->unodes.size(); i++) {
                    if(cur_node->unodes[i] == cur_node->unodes[l])
                        jump_flag = true;
                }
            }

            /*if a cutPI node's unodes only has a same node which is already in the CutPI, remove this node
             e.g cutPI = {a b c}, a's unode = {c}   =>     {b, c} is also a feasible cut*/
            if(jump_flag && cur_node->unodes.size() == 1){
                AddNodeIntoSet(includeNode, cur_node);
                cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node), cutPI.end());
                jump_flag = true;
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
                for(auto& up_node : cur_node->unodes){
                    cutPI.push_back(up_node);
                }
                if (size >= 2)    flag = true;      // only consider the cut has 2 more input situation
                AddNodeIntoSet(includeNode, cur_node);
                cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node), cutPI.end());
            }else if (!jump_flag && !cur_node->unodes.empty() && size <= k
                      && cur_node->gtype == circuit::gateToInt("SP")){
                bool SP_add_flag = true;
                // exit the sp dnode not in includeSet
                for(auto& down_node : cur_node->dnodes){
                    if(includeNode.find(down_node->node_name) == includeNode.end()){
                        SP_add_flag = false;
                    }
                }
                if(SP_add_flag){
                    for(auto& up_node : cur_node->unodes){
                        cutPI.push_back(up_node);
                    }
                    if (size >= 2)    flag = true;      // only consider the cut has 2 more input situation
                    AddNodeIntoSet(includeNode, cur_node);
                    cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node), cutPI.end());
                }else{
                    idx++;
                }
            }
            else{                                       // else move to next node in cutPI
                idx++;
            }
        }

//        cout << "size: " << cutPI.size() << "\tflag: " << flag << endl;
//        cout << "cutPI: ";
//        for (auto s : cutPI)
//            cout << s->node_name <<"\t";
//        cout << endl;

        if(cutPI.size() > k){
            for(auto x : cutPI) cout << x->node_name << "\t";
            break;
//            string errorMessage = "cutPI size is larger than K";  // Exception
//            throw runtime_error(errorMessage);
        }
        if(flag)
            cutPIList.push_back(cutPI);     // add current cutPI to cutPI list
    }

    // remove the cut which is just the upstream node of root_node
    if(!cutPIList.empty()){
        for(auto & cutpi : cutPIList){
            vector<string>cutPIName;
            int PICounter = 0;
            for(auto & j : cutpi){
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
        cout << "\n\nCurrent node: "<< root_node->node_name << endl;
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
    cutPI.clear();
    cutPIList.clear();
    rootUnodeNameList.clear();
    includeNode.clear();
    return cutPINameList;
}



/**Function*************************************************************
    form the cut circuit base on the cut Inputs
***********************************************************************/
circuit* Construct_Cut_Circuit(circuit* bench, vector<string>& cutPI, node* rootNode, string& name, int faninSize){
    auto* newCutCircuit = new circuit(name, false);
    node* new_node = new node(rootNode->node_name, rootNode->gtype);
    newCutCircuit->Node_list.push_back(new_node);
    newCutCircuit->Poutput.push_back(new_node);
    newCutCircuit->nameToNode[new_node->node_name] = new_node;

    for(auto& pi_name : cutPI){
        newCutCircuit->Construct_PI(pi_name);
    }

    for(auto& cur_node : rootNode->unodes){
        Construct_Cut_Circuit_Helper(newCutCircuit, cutPI, cur_node, new_node);
    }
    cout << "Cut circuit {" << name << "} is generated" << endl;

    int minimum_level = INT_MAX;
    for(auto pi : newCutCircuit->Pinput) {
        minimum_level = (bench->nameToNode[pi->node_name]->level<minimum_level) ? bench->nameToNode[pi->node_name]->level : minimum_level;
    }
    for(auto pi : newCutCircuit->Pinput) pi->level = bench->nameToNode[pi->node_name]->level - minimum_level + 1;

//    newCutCircuit->Levelization();
    newCutCircuit->Highfanin_To_Low(faninSize);
    newCutCircuit->Levelization_By_PI();
//    newCutCircuit->pc();
    return newCutCircuit;
}



/**Function*************************************************************
    recursive method to form nodes in the cutCircuit method
***********************************************************************/
void Construct_Cut_Circuit_Helper(circuit* new_circuit, vector<string>& cutPI, node* cur_node, node* new_node){
    auto pi_find = find( cutPI.begin( ), cutPI.end( ), cur_node->node_name);
    node* new_up_node;
    if (pi_find == cutPI.end())
        if(new_circuit->nameToNode.find(cur_node->node_name) == new_circuit->nameToNode.end()){
            new_up_node = new_circuit->Construct_Node(cur_node->node_name, cur_node->gtype);
        } else{
            new_up_node = new_circuit->nameToNode[cur_node->node_name];
        }
    else{
        new_up_node = new_circuit->nameToNode[cur_node->node_name];
    }

    circuit::Connect_Nodes(new_node, new_up_node);

    if(pi_find == cutPI.end()){
        for(auto& up_cur_node : cur_node->unodes){
            Construct_Cut_Circuit_Helper(new_circuit, cutPI, up_cur_node, new_up_node);
        }
    }
}



/**Function*************************************************************
    Construct the cut Circuit based on implicantSet
***********************************************************************/
circuit* Construct_LUT_Circuit(vector<string>& implicantSet, vector<string>& PI, vector<int>& PI_levels, string& PO_name, string& name, int faninSize){
    auto* new_circuit = new circuit(name, false);
    node* new_node;
    node* cur_node;
    node* up1Node;
    node* up2Node;
    node* up3Node;
    node* up4Node;
    node* PO;
    int idx = 0;

    // construct PI;
    for(auto& PI_name : PI)
        new_circuit->Construct_PI(PI_name);

    //  construct NOT node for PI
    if(implicantSet.size() > 1){      //  PO is OR gate
        PO = new_circuit->Construct_Node(PO_name, circuit::gateToInt("OR"));
        new_circuit->Poutput.push_back(PO);

        // create inv gate for PI
        for(const auto& implicant : implicantSet){
            for(int i = 0; i < implicant.size(); i++){
                cur_node = new_circuit->nameToNode[PI[i]];
                if((implicant[i] == '0' || implicant[i] == 'm') && cur_node->dnodes.empty()){
                    string midNodeName = "V";
                    midNodeName.append(to_string(idx));
                    idx++;
                    new_node = new_circuit->Construct_Node(midNodeName, circuit::gateToInt("NOT"));
                    circuit::Connect_Nodes(new_node, cur_node);
                }
            }
        }

        for(const auto& imp : implicantSet) {
            string newName = "V";
            unordered_map<char, vector<int>> piHash = {{'0', {}}, {'1', {}},
                                                       {'x', {}}, {'X', {}},
                                                       {'m', {}}, {'M', {}},
                                                       {'-', {}}};

            for (int i = 0; i < imp.size(); i++){
                piHash[imp[i]].push_back(i);                                        // store each idx for each char
            }

            if (piHash['-'].size() == PI.size()-1) {                                // only one PI in sub_PO
                if (!piHash['1'].empty()) {
                    up1Node = new_circuit->nameToNode[PI[piHash['1'][0]]];          // piHash['1'][0]: get PI idx
                    circuit::Connect_Nodes(PO, up1Node);
                }
                else if (!piHash['0'].empty()) {
                    up1Node = new_circuit->nameToNode[PI[piHash['0'][0]]]->dnodes[0];  // get this PI's inverse
                    circuit::Connect_Nodes(PO, up1Node);
                }
                else {
                    string errorMessage = "Exist other chars, check implicantSet";  // Exception
                    throw runtime_error(errorMessage);
                }
            }
                // PO up lv1 is XNOR
            else if ((piHash['-'].size()+piHash['x'].size() == PI.size()) && (!piHash['x'].empty())) {
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("NOT"));
                circuit::Connect_Nodes(PO, up1Node);

                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up2Node = new_circuit->Construct_Node(newName, circuit::gateToInt("XOR"));
                circuit::Connect_Nodes(up1Node, up2Node);

                for (auto i : piHash['x']) {
                    up3Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up2Node, up3Node);
                }
            }
                // PO up lv1 is XOR
            else if ((piHash['-'].size()+piHash['X'].size() == PI.size()) && (!piHash['X'].empty())) {
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("XOR"));
                circuit::Connect_Nodes(PO, up1Node);

                for (auto i : piHash['X']) {
                    up2Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
            }
                // PO up lv1 is MAJ
            else if ((piHash['-'].size()+piHash['M'].size()+piHash['m'].size() == PI.size()) &&
                                    (!piHash['M'].empty()||!piHash['m'].empty())) {
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("MAJ"));
                circuit::Connect_Nodes(PO, up1Node);
                for (auto i : piHash['M']) {
                    up2Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
                for (auto i : piHash['m']) {
                    up2Node = new_circuit->nameToNode[PI[i]]->dnodes[0];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
            }
                // PO up lv1 is AND
            else {
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("AND"));
                circuit::Connect_Nodes(PO, up1Node);

                if (!piHash['X'].empty()) {                              // PO up lv2 is XOR
                    string sub_subPOName = "V";
                    sub_subPOName.append(to_string(idx));
                    idx++;
                    up2Node = new_circuit->Construct_Node(sub_subPOName, circuit::gateToInt("XOR"));
                    circuit::Connect_Nodes(up1Node, up2Node);

                    for (auto i : piHash['X']) {
                        up3Node = new_circuit->nameToNode[PI[i]];
                        circuit::Connect_Nodes(up2Node, up3Node);
                    }
                } else if(!piHash['x'].empty()){                         // PO up lv2 is XNOR
                    string sub_subPOName = "V";
                    sub_subPOName.append(to_string(idx));
                    idx++;
                    up2Node = new_circuit->Construct_Node(sub_subPOName, circuit::gateToInt("NOT"));
                    circuit::Connect_Nodes(up1Node, up2Node);

                    sub_subPOName = "V";
                    sub_subPOName.append(to_string(idx));
                    idx++;
                    up3Node = new_circuit->Construct_Node(sub_subPOName, circuit::gateToInt("XOR"));
                    circuit::Connect_Nodes(up2Node, up3Node);

                    for (auto i : piHash['x']) {
                        up4Node = new_circuit->nameToNode[PI[i]];
                        circuit::Connect_Nodes(up3Node, up4Node);
                    }
                } else if(!piHash['M'].empty() || !piHash['m'].empty()){                         // PO up lv2 is MAJ
                    string sub_subPOName = "V";
                    sub_subPOName.append(to_string(idx));
                    idx++;
                    up2Node = new_circuit->Construct_Node(sub_subPOName, circuit::gateToInt("MAJ"));
                    circuit::Connect_Nodes(up1Node, up2Node);

                    for (auto i : piHash['M']) {
                        up3Node = new_circuit->nameToNode[PI[i]];
                        circuit::Connect_Nodes(up2Node, up3Node);
                    }
                    for (auto i : piHash['m']) {
                        up3Node = new_circuit->nameToNode[PI[i]]->dnodes[0];
                        circuit::Connect_Nodes(up2Node, up3Node);
                    }
                }

                for (auto i : piHash['0']) {
                    up2Node = new_circuit->nameToNode[PI[i]]->dnodes[0];                  // get this PI is inverse
                    circuit::Connect_Nodes(up1Node, up2Node);
                }

                for (auto i : piHash['1']) {
                    up2Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
            }
        }
    }
    else if(implicantSet.size() == 1){
        PO = new_circuit->Construct_Node(PO_name);      // construct PO, PO gate type is not determined
        new_circuit->Poutput.push_back(PO);

        string implicant = implicantSet[0];
        string newName = "V";

        for(int i = 0; i < implicant.length(); i++){
            cur_node = new_circuit->nameToNode[PI[i]];
            if(implicant[i] == '0' || implicant[i] == 'm'){
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                new_node = new_circuit->Construct_Node(newName, circuit::gateToInt("NOT"));
                circuit::Connect_Nodes(new_node, cur_node);
            }
        }

        unordered_map<char, vector<int>> piHash = {{'0', {}}, {'1', {}},
                                                   {'x', {}}, {'X', {}},
                                                   {'m', {}}, {'M', {}},
                                                   {'-', {}}};

        for (int i = 0; i < implicant.length(); i++) {
            piHash[implicant[i]].push_back(i);
        }

        if (piHash['-'].size() == PI.size()-1) {                                // only one PI in sub_PO
            if (!piHash['1'].empty()) {
                up1Node = new_circuit->nameToNode[PI[piHash['1'][0]]];          // piHash['1'][0]: get PI idx
                circuit::Connect_Nodes(PO, up1Node);
                PO->gtype = circuit::gateToInt("SDFF");              // 1---, PO->SDFF
            }
            else if (!piHash['0'].empty()) {
                up1Node = new_circuit->nameToNode[PI[piHash['0'][0]]];          // get this PI's' inverse
                circuit::Connect_Nodes(PO, up1Node);
                PO->gtype = circuit::gateToInt("NOT");               // 1---, PO->NOT
            }
            else {
                string errorMessage = "Exist other chars, check implicantSet";  // Exception
                throw runtime_error(errorMessage);
            }
        }
            // PO is XNOR
        else if ((piHash['-'].size()+piHash['x'].size() == PI.size()) && (!piHash['x'].empty())){
            PO->gtype = circuit::gateToInt("NOT");
            newName = "V";
            newName.append(to_string(idx));
            idx++;
            up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("XOR"));
            circuit::Connect_Nodes(PO, up1Node);
            for (auto i : piHash['x']) {
                up2Node = new_circuit->nameToNode[PI[i]];
                circuit::Connect_Nodes(up1Node, up2Node);
            }
        }
            // PO is XOR
        else if ((piHash['-'].size()+piHash['X'].size() == PI.size()) && (!piHash['X'].empty())){
            PO->gtype = circuit::gateToInt("XOR");
            for (auto i : piHash['X']) {
                up1Node = new_circuit->nameToNode[PI[i]];
                circuit::Connect_Nodes(PO, up1Node);
            }
        }
            // PO is MAJ
        else if ((piHash['-'].size()+piHash['M'].size()+piHash['m'].size() == PI.size()) &&
                 (!piHash['M'].empty()||!piHash['m'].empty())) {
            PO->gtype = circuit::gateToInt("MAJ");
            for (auto i : piHash['M']) {
                up1Node = new_circuit->nameToNode[PI[i]];
                circuit::Connect_Nodes(PO, up1Node);
            }
            for (auto i : piHash['m']) {
                up1Node = new_circuit->nameToNode[PI[i]]->dnodes[0];
                circuit::Connect_Nodes(PO, up1Node);
            }
        }
            // sub_PO is AND
        else {
            PO->gtype = circuit::gateToInt("AND");

            if (!piHash['X'].empty()) {                             // PO up lv1 is XOR
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("XOR"));
                circuit::Connect_Nodes(PO, up1Node);
                for (auto i : piHash['X']) {
                    up2Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
            } else if (!piHash['x'].empty()) {                      // PO up lv1 is XNOR
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("NOT"));
                circuit::Connect_Nodes(PO, up1Node);

                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up2Node = new_circuit->Construct_Node(newName, circuit::gateToInt("XOR"));
                circuit::Connect_Nodes(up1Node, up2Node);

                for (auto i : piHash['x']) {
                    up3Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up2Node, up3Node);
                }
            } else if (!piHash['M'].empty() || !piHash['m'].empty()) {   // PO up lv1 is MAJ
                newName = "V";
                newName.append(to_string(idx));
                idx++;
                up1Node = new_circuit->Construct_Node(newName, circuit::gateToInt("MAJ"));
                circuit::Connect_Nodes(PO, up1Node);
                for (auto i : piHash['M']) {
                    up2Node = new_circuit->nameToNode[PI[i]];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
                for (auto i : piHash['m']) {
                    up2Node = new_circuit->nameToNode[PI[i]]->dnodes[0];
                    circuit::Connect_Nodes(up1Node, up2Node);
                }
            }

            for (auto i : piHash['0']) {
                up1Node = new_circuit->nameToNode[PI[i]]->dnodes[0];  // get this PI is inverse
                circuit::Connect_Nodes(PO, up1Node);
            }
            for (auto i : piHash['1']) {
                up1Node = new_circuit->nameToNode[PI[i]];
                circuit::Connect_Nodes(PO, up1Node);
            }
        }
    }

    for(int i = 0; i < new_circuit->Pinput.size(); i++) {
        new_circuit->Pinput[i]->level = PI_levels[i];
    }

    new_circuit->Highfanin_To_Low(faninSize);
    new_circuit->Levelization_By_PI();
    cout << "Cut circuit is generated from implicantSet" << endl;
//    new_circuit->pc();
    return new_circuit;
}



//circuit* Construct_LUT_Circuit(vector<string>& LUT, vector<string> PI, string& PO_name, string& name){
//    auto* new_circuit = new circuit(name, false);
//    node* new_node;
//    node* cur_node;
//    node* up_1_Node;
//    node* up_2_Node;
//    node* up_3_Node;
//    node* PO;
//    int idx = 0;
//
//    // construct PI;
//    for(auto& PI_name : PI) new_circuit->Construct_PI(PI_name);
//
//    //  construct NOT node for PI
//    if(LUT.size()>1){      //  PO is OR gate
//        PO = new_circuit->Construct_Node(PO_name, circuit::gateToInt("OR"));
//        new_circuit->Poutput.push_back(PO);
//        for(const auto& logic : LUT){
//            for(int i = 0; i < logic.size(); i++){
//                cur_node = new_circuit->nameToNode[PI[i]];
//                if(logic[i] == '0' && cur_node->dnodes.empty()){
//                    string midNodeName = "M";
//                    midNodeName.append(to_string(idx));
//                    idx++;
//                    new_node = new_circuit->Construct_Node(midNodeName, circuit::gateToInt("NOT"));
//                    circuit::Connect_Nodes(new_node, cur_node);
//                }
//            }
//        }
//
//        for(const auto& logic : LUT) {
//            unordered_map<char, vector<int>> piHash = {{'0', {}}, {'1', {}}, {'-', {}}, {'^', {}}};
//            for (int i = 0; i < logic.size(); i++) piHash[logic[i]].push_back(i);
//            if (piHash['-'].size() == PI.size()-1) {                                                // only one PI in sub_PO
//                if (!piHash['1'].empty()) {
//                    up_1_Node = new_circuit->nameToNode[PI[piHash['1'][0]]];             // piHash['1'][0]: get PI idx
//                    circuit::Connect_Nodes(PO, up_1_Node);
//                }
//                else if (!piHash['0'].empty()) {
//                    up_1_Node = new_circuit->nameToNode[PI[piHash['0'][0]]]->dnodes[0];  // get this PI is inverse
//                    circuit::Connect_Nodes(PO, up_1_Node);
//                }
//                else {
//                    cout << "there is only one ^ char, check the LUT" << endl;
//                }
//            }
//            else if (piHash['-'].size() == 2 && (!piHash['^'].empty())) {                  // sub_PO is XOR
//                string subPOName = "M";
//                subPOName.append(to_string(idx));
//                idx++;
//                up_1_Node = new_circuit->Construct_Node(subPOName, circuit::gateToInt("XOR"));
//                circuit::Connect_Nodes(PO, up_1_Node);
//                for (auto i : piHash['^']) {
//                    up_2_Node = new_circuit->nameToNode[PI[i]];
//                    circuit::Connect_Nodes(up_1_Node, up_2_Node);
//                }
//            }
//            else {                                                                          // sub_PO is AND
//                string subPOName = "M";
//                subPOName.append(to_string(idx));
//                idx++;
//                up_1_Node = new_circuit->Construct_Node(subPOName, circuit::gateToInt("AND"));
//                circuit::Connect_Nodes(PO, up_1_Node);
//                if (!piHash['^'].empty()) {                                                 // sub_sub_PO is XOR
//                    string sub_subPOName = "M";
//                    sub_subPOName.append(to_string(idx));
//                    idx++;
//                    up_2_Node = new_circuit->Construct_Node(sub_subPOName, circuit::gateToInt("XOR"));
//                    circuit::Connect_Nodes(up_1_Node, up_2_Node);
//                    for (auto i : piHash['^']) {
//                        up_3_Node = new_circuit->nameToNode[PI[i]];
//                        circuit::Connect_Nodes(up_2_Node, up_3_Node);
//                    }
//                }
//                for (auto i : piHash['0']) {
//                    up_2_Node = new_circuit->nameToNode[PI[i]]->dnodes[0];                  // get this PI is inverse
//                    circuit::Connect_Nodes(up_1_Node, up_2_Node);
//                }
//                for (auto i : piHash['1']) {
//                    up_2_Node = new_circuit->nameToNode[PI[i]];
//                    circuit::Connect_Nodes(up_1_Node, up_2_Node);
//                }
//            }
//        }
//    }
//    else if(LUT.size() == 1){
//        string logic = LUT[0];
//        unordered_map<char, vector<int>> piHash = {{'0', {}}, {'1', {}}, {'-', {}}, {'^', {}}};
//        for (int i = 0; i < logic.length(); i++) piHash[logic[i]].push_back(i);
//        PO = new_circuit->Construct_Node(PO_name);
//        new_circuit->Poutput.push_back(PO);
//        if (piHash['-'].size() == PI.size()-1) {                                                // only one PI in sub_PO
//            if (!piHash['1'].empty()) {
//                up_1_Node = new_circuit->nameToNode[PI[piHash['1'][0]]];             // piHash['1'][0]: get PI idx
//                circuit::Connect_Nodes(PO, up_1_Node);
//                PO->gtype = circuit::gateToInt("SDFF");
//            }
//            else if (!piHash['0'].empty()) {
//                up_1_Node = new_circuit->nameToNode[PI[piHash['0'][0]]];  // get this PI is inverse
//                circuit::Connect_Nodes(PO, up_1_Node);
//                PO->gtype = circuit::gateToInt("NOT");
//            }
//            else {
//                cout << "there is only one ^ char, check the LUT" << endl;
//            }
//        }
//        else if (piHash['-'].size() == 2 && (!piHash['^'].empty())) {                  // sub_PO is XOR
//            for (auto i : piHash['^']) {
//                up_1_Node = new_circuit->nameToNode[PI[i]];
//                circuit::Connect_Nodes(PO, up_1_Node);
//                PO->gtype = circuit::gateToInt("XOR");
//            }
//        }
//        else {                                                                     // sub_PO is AND
//            for(int i = 0; i < logic.length(); i++){
//                cur_node = new_circuit->nameToNode[PI[i]];
//                if(logic[i] == '0'){
//                    string midNodeName = "M";
//                    midNodeName.append(to_string(idx));
//                    idx++;
//                    new_node = new_circuit->Construct_Node(midNodeName, circuit::gateToInt("NOT"));
//                    circuit::Connect_Nodes(new_node, cur_node);
//                }
//            }
//            PO->gtype = circuit::gateToInt("AND");
//            if (!piHash['^'].empty()) {                                               // sub_sub_PO is XOR
//                string subPOName = "M";
//                subPOName.append(to_string(idx));
//                idx++;
//                up_1_Node = new_circuit->Construct_Node(subPOName, circuit::gateToInt("XOR"));
//                circuit::Connect_Nodes(PO, up_1_Node);
//                for (auto i : piHash['^']) {
//                    up_2_Node = new_circuit->nameToNode[PI[i]];
//                    circuit::Connect_Nodes(up_1_Node, up_2_Node);
//                }
//            }
//            for (auto i : piHash['0']) {
//                up_1_Node = new_circuit->nameToNode[PI[i]]->dnodes[0];  // get this PI is inverse
//                circuit::Connect_Nodes(PO, up_1_Node);
//            }
//            for (auto i : piHash['1']) {
//                up_1_Node = new_circuit->nameToNode[PI[i]];
//                circuit::Connect_Nodes(PO, up_1_Node);
//            }
//        }
//    }
//
//    cout << "Cut circuit is generated from LUT" << endl;
//
//
////    new_circuit->levelization_by_PI();
//    new_circuit->Levelization();
////    /** cut circuit check
////    new_circuit->cut_Depth_balancing();
////    new_circuit->Splitter_Binary_Tree_Insertion();
////    new_circuit->Levelization();
////    new_circuit->levelization_by_PI();
////    new_circuit->pc();
//    return new_circuit;
//}