//
// Created by Jay on 4/28/21.
//
#include "circuit.h"



/**Function*************************************************************
    Delete a Node
***********************************************************************/
void circuit::Delete_Node(node* cur_node){
    if(cur_node == nullptr){
        cout << "this node already deleted" << endl;
        return;
    }

    int sizeUP = cur_node->unodes.size();
    int sizeDown = cur_node->dnodes.size();
    for(int i = 0 ; i < sizeUP; i++){
        if (find (cur_node->unodes[0]->dnodes.begin(), cur_node->unodes[0]->dnodes.end(), cur_node) != cur_node->unodes[0]->dnodes.end()){
            Disconnect_Nodes(cur_node, cur_node->unodes[0]);
        }
    }

    for(int i = 0 ; i < sizeDown; i++){
        if (find (cur_node->dnodes[0]->unodes.begin(), cur_node->dnodes[0]->unodes.end(), cur_node) != cur_node->dnodes[0]->unodes.end()){
            circuit::Disconnect_Nodes(cur_node->dnodes[0], cur_node);
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
//    cout << "Node: " << cur_node->node_name << " are removed" << endl;
    delete cur_node;
}



/**Function*************************************************************
    Remove the intermediate nodes between input list and root node
***********************************************************************/
void circuit :: Remove_Old_Node(vector<node*>& cutPI, node* root_node) {
    unordered_set<string> cutPINameset;
    unordered_set<string> allRemovedNodeList;
    queue<string> removeList;
    node* cur_node;

    for(auto& pi : cutPI)
        cutPINameset.insert(pi->node_name);

    for(auto& up_node : root_node->unodes){
        // can not find the pi in the current up node
        if(cutPINameset.find(up_node->node_name) == cutPINameset.end()){
            removeList.push(up_node->node_name);
            allRemovedNodeList.insert(up_node->node_name);
        }
    }
    root_node->unodes.clear();

    while(!removeList.empty()){
        cur_node = nameToNode[removeList.front()];
        for(auto& up_node : cur_node->unodes){
//            cout << "cur_node:" << cur_node->node_name << "\tup_node:" << up_node->node_name << endl;
            if(cutPINameset.find(up_node->node_name) == cutPINameset.end()){
                if(allRemovedNodeList.find(up_node->node_name) == allRemovedNodeList.end()){
                    removeList.push(up_node->node_name);
                    allRemovedNodeList.insert(up_node->node_name);
                }
            }
        }
        Delete_Node(cur_node);
        removeList.pop();
    }
    cutPINameset.clear();
}


/**Function*************************************************************
    generate cut circuit based on LUTToCut
***********************************************************************/
void circuit::Fill_Cut_To_Circuit(circuit * LUTToCut){
    node* new_Node;
    unordered_map<string, string> CNNTBNN;  //current cut node name to bench node name
    for(auto& pi : LUTToCut->Pinput){
        CNNTBNN[pi->node_name] = pi->node_name;
//        cout << pi->node_name << "\t" << pi->node_name << endl;
    }
    for(auto& pout : LUTToCut->Poutput){
        CNNTBNN[pout->node_name] = pout->node_name;
//        cout << pout->node_name << "\t" << pout->node_name << endl;
    }

    for(auto& cur_node : LUTToCut->Node_list){
//        cout << cur_node->node_name << "!!!!" << endl;
        if(CNNTBNN.find(cur_node->node_name) ==  CNNTBNN.end()){
//            cout << cur_node->node_name << endl;
            string name = "m" + to_string(newNodeNameCounter++);
            CNNTBNN[cur_node->node_name] = name;
            new_Node = Construct_Node(name, cur_node->gtype);
//            cout << "build: " << new_Node->node_name << "\t" << intToGate(new_Node->gtype) << endl;
        }
    }
    for(auto& cur_node : LUTToCut->Node_list){
        if(cur_node->gtype != gateToInt("PI")){
            for(auto& up_node : cur_node->unodes){
                circuit::Connect_Nodes(nameToNode[CNNTBNN[cur_node->node_name]],
                        nameToNode[CNNTBNN[up_node->node_name]]);
//                cout << "connect: " << cur_node->node_name <<"\t" << up_node->node_name << " -> bench: "
//                << CNNTBNN[cur_node->node_name] <<"\t" << CNNTBNN[up_node->node_name] << endl;
            }
        }
    }
    for(auto& PO : LUTToCut->Poutput){
        if(nameToNode.find(CNNTBNN[PO->node_name]) != nameToNode.end())
            nameToNode[CNNTBNN[PO->node_name]]->gtype = PO->gtype;
    }
    cout << "circuit refill is done" << endl;
    CNNTBNN.clear();

    /* specific node check
    if(nameToNode.find("G11") !=  nameToNode.end()){
        new_Node = nameToNode["G11"];
        cout << "G11 unode size: " << new_Node->unodes.size() << endl;
        for(auto i : new_Node->unodes) cout << i->node_name << "\t" << intToGate(i->gtype) << endl;
    }
     */
}