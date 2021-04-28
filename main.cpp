#include <iostream>
#include "circuit.h"
//#include "circuit_cut.cpp"
using namespace std;
vector<vector<node*>> K_feasible_Cut(node* cur_node, int k);
void node_constructor(circuit* new_circuit, vector<string>& cutPI, node* cur_node, node* new_node);
circuit* cutCircuit(circuit* bench, vector<string>& cutPI, node* rootNode, string& name);


int main(int argc, char *argv[]) {
//    circuit *bench = new circuit("../circuit/s9234.bench", true);
//    circuit *bench = new circuit("../circuit/sim_test.bench", true);
    circuit *bench = new circuit("../circuit/c17.bench", true);
//    circuit *bench = new circuit("../circuit/s27.bench", true);

    bench->pc();
    float x = bench->errorRateCal();
    cout << "EDP : "  << bench->max_lvl*x;

    for(auto cur : bench->Poutput){
        cout << cur->node_name << endl;
//        if(cur->level > 2){
            int idx = 0;
            vector<vector<node*>>cutList = K_feasible_Cut(cur,4);
            circuit *minEDP_cut = nullptr;
            float minEDP = 1;
            for(int j=0; j<1; j++){
                string name = cur->node_name;
                name.append("_cut_");
                name.append(to_string(idx));
                idx++;
                vector<string> namelist;
                for(auto i : cutList[j]) namelist.push_back(i->node_name);
//                cout <<endl << name << endl;
//                cout << "\ncut circuit PI: " << endl;
//                for(auto i : namelist) cout << i << " ";
//                cout << endl;
                circuit* cut = cutCircuit(bench, namelist, cur, name);
                cut->levelization();
                minEDP = (minEDP < cut->errorRateCal()) ? minEDP*cut->Poutput[0]->level : cut->errorRateCal()* cut->Poutput[0]->level;
                minEDP_cut = (minEDP < cut->errorRateCal()) ? cut : minEDP_cut;
            }
//             cout << "cut circuit: " << minEDP_cut->circuit_name << "EDP: " << minEDP << endl;
//            vector<string> s = minEDP_cut->LUTtoQM();
        cout << "start replacing" << endl;
        bench->replaceCutwithQM(cutList[3], cur);
        bench->levelization();
        }


//    }
    bench->pc();
    x = bench->errorRateCal();
    cout << "EDP : "  << bench->max_lvl*x;





    //    for(auto i : cut->Node_list) cout << i->node_name << " ";
//    cout << endl;
//    for(auto pi : cutList[idx]) cut->deleteOldCut(n, namelist);
//    cout << "delete" << endl;
//    for(auto i : cut->Node_list) cout << i->node_name << " ";
//    cout << endl;
//    K_feasible_Cut(bench->nameToNode["g4"],3);

    //QM test
//    vector<int> a = {0, 1, 2};
//    vector<string> result = bench->LUTtoQM();
//    vector<string>result = bench->QM(a, bench->Pinput);
    return 0;
}


vector<vector<node*>> K_feasible_Cut(node* cur_node, int k){
    vector<node*> cutPI;
    vector<vector<node*>> cutPIList;
    cutPI.push_back(cur_node);
    int idx = 0;
    while(cutPI.size()>=idx){
        bool flag = false;
        cur_node = cutPI[idx];
//        cout << "idx: " << idx << " currentNode: " << cur_node->node_name << endl;
        int size = cutPI.size()-1+cur_node->unodes.size();      // all PI size if add cur_node's upnode
        bool jump_flag = false;                                 // if has same node remain this node
        for(auto i : cur_node->unodes) for(auto j : cutPI) if(j->node_name == i->node_name)  jump_flag = true;
        if(jump_flag && cur_node->unodes.size()==1){
            cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node),cutPI.end());
            flag = true;
        }
        if(!jump_flag && cur_node->unodes.size()>0 && size<=k){
            for(auto up_node : cur_node->unodes)    cutPI.push_back(up_node);
            if (size >= 2)    flag = true;
            cutPI.erase(remove(cutPI.begin(), cutPI.end(), cur_node),cutPI.end());
        }else{
            idx++;
        }
//        for(auto j : cutPI) cout << j->node_name << " ";
//        cout << endl;
        if(flag)    cutPIList.push_back(cutPI);                         // if cutPI > 2 add it into list
    }
//    for(const auto& i : cutPIList){
//        cout << "cuts: ";
//        for(auto j : i) cout << j->node_name << " ";
//        cout << endl;
//    }
    return cutPIList;
}


circuit* cutCircuit(circuit* bench, vector<string>& cutPI, node* rootNode, string& name){
//    int min_lvl = 0;
//    for(const auto& pi : cutPI){
//        min_lvl = (bench->nameToNode[pi]->level < min_lvl) ? bench->nameToNode[pi]->level : min_lvl;
//    }

    circuit* new_circuit = new circuit(name, false);
    node* new_node = new node(rootNode->node_name, rootNode->gtype);
    new_circuit->Node_list.push_back(new_node);
    new_circuit->Poutput.push_back(new_node);
    new_circuit->nameToNode[new_node->node_name] = new_node;

    for(auto cur_node : rootNode->unodes){
        node_constructor(new_circuit, cutPI, cur_node, new_node);
    }
    cout << "\nGenerate a cut circuit\n" << endl;
    return new_circuit;
}


void node_constructor(circuit* new_circuit, vector<string>& cutPI, node* cur_node, node* new_node){
    vector<string>::iterator pi_find = find( cutPI.begin( ), cutPI.end( ), cur_node->node_name);
    node* new_up_node;
    if ( pi_find == cutPI.end() )
        new_up_node = new node(cur_node->node_name, cur_node->gtype);
    else{
        new_up_node = new node(cur_node->node_name, circuit::gateToInt("PI"));
        new_circuit->Pinput.push_back(new_up_node);
    }
    new_circuit->Node_list.push_back(new_up_node);
    new_circuit->nameToNode[new_up_node->node_name] = new_up_node;
    new_circuit->connectNodes(new_node, new_up_node);

    if(pi_find == cutPI.end())
        for(auto up_cur_node : cur_node->unodes) node_constructor(new_circuit, cutPI, up_cur_node, new_up_node);
}