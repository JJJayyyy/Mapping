//
// Created by Jay on 4/28/21.
//
#include "circuit.h"


/**Function*************************************************************
    Delete a Node
***********************************************************************/

void circuit::deleteNode(node* cur_node){
    for(auto up_node : cur_node->unodes){           // remove the connection with upnodes
        if (find (up_node->dnodes.begin(), up_node->dnodes.end(), cur_node) != up_node->dnodes.end()){
            disconnectNodes(cur_node, up_node);
        }
    }
    for(auto down_node : cur_node->unodes){         // remove the connection with downnodes
        if (find (down_node->dnodes.begin(), down_node->dnodes.end(), cur_node) != down_node->dnodes.end()){
            circuit::disconnectNodes(down_node, cur_node);
        }
    }

    if (find (Pinput.begin(), Pinput.end(), cur_node)!= Pinput.end()){
        Pinput.erase(remove(Pinput.begin(), Pinput.end(), cur_node),Pinput.end());
    }
    if (find (Poutput.begin(), Poutput.end(), cur_node)!= Poutput.end()){
        Poutput.erase(remove(Poutput.begin(), Poutput.end(), cur_node),Poutput.end());
    }
    if (find (Node_list.begin(), Node_list.end(), cur_node)!= Node_list.end()){
        Node_list.erase(remove(Node_list.begin(), Node_list.end(), cur_node),Node_list.end());
    }

    nameToNode.erase(cur_node->node_name);
    cout << "Node: " << cur_node->node_name << " are removed" << endl;
    delete cur_node;
}


/**Function*************************************************************
    Remove the intermediate nodes between input list and root node
***********************************************************************/

void circuit :: removeOldNode(vector<node*>& cutPI, node* root_node) {
    unordered_set<string> cutPINameset;
    node* cur_node;
    for(auto pi : cutPI)    cutPINameset.insert(pi->node_name);
    queue<string> removeList;
    for(auto up_node : root_node->unodes){
        // can not find the pi in the current up node
        if(cutPINameset.find(up_node->node_name) == cutPINameset.end()){
            removeList.push(up_node->node_name);
        }
    }
    while(!removeList.empty()){
        cur_node = nameToNode[removeList.front()];
        for(auto up_node : cur_node->unodes){
            if(cutPINameset.find(up_node->node_name) == cutPINameset.end()){
                removeList.push(up_node->node_name);
            }
        }
        deleteNode(cur_node);
        removeList.pop();
    }
}


/**Function*************************************************************
    generate cut circuit based on LUTToCut
***********************************************************************/

void circuit::fillCutToCircuit(circuit * LUTToCut){
    node* new_Node;
    for(auto cur_node : LUTToCut->Node_list){
        if(nameToNode.find(cur_node->node_name) ==  nameToNode.end()){
            new_Node = construct_Node(cur_node->node_name, cur_node->gtype);
            cout << "build: " << new_Node->node_name << "\t" << intToGate(new_Node->gtype) << endl;
        }
    }
    for(auto cur_node : LUTToCut->Node_list){
        if(cur_node->gtype != gateToInt("PI")){
            for(auto up_node : cur_node->unodes){
                circuit::connectNodes(nameToNode[cur_node->node_name], nameToNode[up_node->node_name]);
                cout << "connect: " << cur_node->node_name <<"\t" << up_node->node_name << endl;
            }
        }
    }
    for(auto po : LUTToCut->Poutput){
        if(nameToNode.find(po->node_name) !=  nameToNode.end())
            nameToNode[po->node_name]->gtype = po->gtype;
    }
    cout << "circuit refill is done" << endl;
}

//    // assign the gate level to new circuit
//    for(auto pi : new_circuit->Pinput) pi->level = bench->nameToNode[pi->node_name]->level;
//    for(auto pout : new_circuit->Poutput) pout->level = bench->nameToNode[pout->node_name]->level;
//    new_circuit->levelization_by_PI();
//    new_circuit->depth_balancing();



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
//    splitterTreeAdder();
//}