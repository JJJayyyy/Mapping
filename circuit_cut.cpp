//
// Created by Jay on 4/25/21.
//

#include "circuit.h"

vector<vector<node*>> K_feasible_Cut(node* cur_node, int k){
    queue<node *> nodeQueue;
    vector<queue<node *>> cutPIQueueList;
    vector<node*> cutPI;
    vector<vector<node*>> cutPIList;
    nodeQueue.push(cur_node);
    bool flag = false;
    while(!nodeQueue.empty()){
        flag = false;
        cur_node = nodeQueue.front();                                          // iterate by bench_circuit
        cout << "currentNode: " << cur_node->node_name << endl;
        if(nodeQueue.size()-1+cur_node->unodes.size() <= k){
            for(auto up_node : cur_node->unodes)    nodeQueue.push(up_node);
            if (nodeQueue.size()-1+cur_node->unodes.size() >= 2)    flag = true;
        } else continue;
        nodeQueue.pop();
        if(flag) cutPIQueueList.push_back(nodeQueue);
    }

    cout << "output all cuts: ";
    for(auto& q : cutPIQueueList){
        for(int i = 0; i < q.size(); i++) {
            cout << q.front() << " ";
            cutPI.push_back(q.front());
            q.pop();
        }
        cout << endl;
        cutPIList.push_back(cutPI);
    }
    return cutPIList;
}

//void helper(circuit* cut_circuit, node* root_node, int k, vector<string>& array){
//    int idx = 0;
//    string name = cur_node->node_name.append("_cut_");
//    vector<circuit*> cut_circuit_list;
//    circuit* K_feasible_cut = new circuit(name.append(to_string(idx)), false);      // create cut circuit
//    node* currentNode;
//    node* new_node;
////    node* new_node = new node(cur_node->node_name, cur_node->gtype);         // create a new node cut's circuit;
////    K_feasible_cut->Node_list.push_back(new_node);
////    K_feasible_cut->nameToNode[new_node->node_name] = new_node;
////    vector<string> cur_NodeList;
//    return *array;
//    node* new_up_node;
//    for(auto up_node : cur_node->unodes){
//        new_up_node = new node(up_node->node_name, up_node->gtype);
//        K_feasible_cut->Node_list.push_back(new_up_node);
//        K_feasible_cut->nameToNode[new_up_node->node_name] = new_up_node;
//        k--;
//    }
//}
