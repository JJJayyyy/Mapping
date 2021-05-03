#include <iostream>
#include "circuit.h"
#include "circuit_cut.cpp"

using namespace std;
void mapping(circuit* bench);
void Test_K_feasible_Cut(circuit* bench);
void Test_CutLUT_To_Circuit(circuit* bench);


int main(int argc, char *argv[]) {
//    circuit *bench = new circuit("../circuit/s9234.bench", true);
//    circuit *bench = new circuit("../circuit/sim_test.bench", true);
//    circuit *bench = new circuit("../circuit/c17.bench", true);
    circuit *bench = new circuit("../circuit/s27.bench", true);

    float circuit_EDP = bench->errorRateCal()*(float)bench->Poutput[0]->level;;
    cout << "bench_EDP: " << circuit_EDP << endl;
//    bench->pc();
    Test_K_feasible_Cut(bench);
//    mapping(bench);
//    bench->Levelization();
//    bench->depth_balancing();
//    circuit_EDP = bench->errorRateCal()*(float)bench->Poutput[0]->level;;
//    cout << "bench_EDP: " << circuit_EDP << endl;

    return 0;
}


void Test_K_feasible_Cut(circuit* bench){
    for(auto& root_node : bench->Node_list){
        if(root_node->node_name[0] == 'M' || root_node->gtype == circuit::gateToInt("PI")
            || root_node->gtype == circuit::gateToInt("DFF"))
            continue;
//        node* root_node = bench->nameToNode["G9"];
        vector<vector<string>>rootNodeAllCutList = K_Feasible_Cut(root_node, 4);
        if(rootNodeAllCutList.empty()) continue;
        int idx = 0;
        for(auto& current_cut_PIs : rootNodeAllCutList){
            string name = root_node->node_name;
            name.append("_cut_");
            name.append(to_string(idx++));
            circuit* cut = Construct_Cut_Circuit(bench, current_cut_PIs, root_node, name);
            vector<string> LUT = cut->LUTtoQM();
            cout << "Input_LUT: ";
            for(const auto& i : LUT) cout << i << " ";    cout << endl;
            cout <<  endl;
        }
    }
}


void Test_CutLUT_To_Circuit(circuit* bench){
    for(auto n : bench->Node_list){
        if(n->node_name[0] == 'M' || n->gtype == circuit::gateToInt("PI")) continue;
        node* cur = n;
//        node* cur = bench->Poutput[0];
        cout << "Current node: "<< cur->node_name << endl;
        int idx = 0;
        vector<vector<string>>cutList = K_Feasible_Cut(cur, 4);
        float minEDP = INT_MAX;
        for(auto & cutItem : cutList){
            float curMinCut_EDP;
            string name = cur->node_name;
            name.append("_cut_");
            name.append(to_string(idx));
            idx++;
            vector<string> namelist;
            namelist.reserve(cutItem.size());
            for(auto piCut : cutItem) namelist.push_back(piCut);
            cout << "\ncut name: " << name << "\ncutPI: ";
            for(const auto& i : namelist) cout << i << " "; cout << endl;
            circuit* cut = Construct_Cut_Circuit(bench, namelist, cur, name);
            float cur_EDP = cut->errorRateCal()* (float)cut->Poutput[0]->level;
            cout << "cut EDP: " << cur_EDP << endl;
            vector<string> LUT = cut->LUTtoQM();
            unordered_map<string, int> PI_PO_Level;
            vector<string> cutPI;
            for(auto pi : cut->Pinput){
                PI_PO_Level[pi->node_name] = pi->level;
                cutPI.push_back(pi->node_name);
            }
            PI_PO_Level[cur->node_name] = cut->nameToNode[cur->node_name]->level;
            circuit* LUTToCut = Construct_LUT_Circuit(LUT, PI_PO_Level, cutPI, cur->node_name, name);
            float LUTToCut_EDP = cut->errorRateCal()*(float)LUTToCut->Poutput[0]->level;;
            cout << "LUTToCut_EDP: " << LUTToCut_EDP << endl;
            if(LUTToCut_EDP<cur_EDP){
                curMinCut_EDP = LUTToCut_EDP;
                if(minEDP > curMinCut_EDP){
                    cout << "assign LUTToCut & delete cut" << endl;
                    minEDP = curMinCut_EDP;
                }
                delete cut;
            } else {
                curMinCut_EDP = cur_EDP;
                if(minEDP > curMinCut_EDP){
                    cout << "assign cut & delete LUTToCut" << endl;
                    minEDP = curMinCut_EDP;
                }
                delete LUTToCut;
            }
        }
    }
}



void mapping(circuit* bench){
    for(auto n : bench->Node_list){
        if(n->node_name[0] == 'M' || n->gtype == circuit::gateToInt("PI")) continue;
        node* cur = n;
//        node* cur = bench->Poutput[0];
        cout << "Current node: "<< cur->node_name << endl;
        int idx = 0;
        vector<vector<string>>cutList = K_Feasible_Cut(cur, 4);
        circuit *minEDP_cut = nullptr;
        float minEDP = INT_MAX;
        for(auto & cutItem : cutList){
            circuit* curMinCut;
            float curMinCut_EDP;
            string name = cur->node_name;
            name.append("_cut_");
            name.append(to_string(idx));
            idx++;
            vector<string> namelist;
            namelist.reserve(cutItem.size());
            for(auto piCut : cutItem) namelist.push_back(piCut);
            cout << "\ncut name: " << name << "\ncutPI: ";
            for(const auto& i : namelist) cout << i << " "; cout << endl;
            circuit* cut = Construct_Cut_Circuit(bench, namelist, cur, name);
            float cur_EDP = cut->errorRateCal()* (float)cut->Poutput[0]->level;
            cout << "cut EDP: " << cur_EDP << endl;
            vector<string> LUT = cut->LUTtoQM();
            unordered_map<string, int> PI_PO_Level;
            vector<string> cutPI;
            for(auto pi : cut->Pinput){
                PI_PO_Level[pi->node_name] = pi->level;
                cutPI.push_back(pi->node_name);
            }
            PI_PO_Level[cur->node_name] = cut->nameToNode[cur->node_name]->level;
            circuit* LUTToCut = Construct_LUT_Circuit(LUT, PI_PO_Level, cutPI, cur->node_name, name);
            float LUTToCut_EDP = cut->errorRateCal()*(float)LUTToCut->Poutput[0]->level;;
            cout << "LUTToCut_EDP: " << LUTToCut_EDP << endl;
            if(LUTToCut_EDP<cur_EDP){
                curMinCut = LUTToCut;
                curMinCut_EDP = LUTToCut_EDP;
                if(minEDP > curMinCut_EDP){
                    cout << "assign LUTToCut & delete cut" << endl;
                    minEDP = curMinCut_EDP;
                    minEDP_cut = curMinCut;
                }
                delete cut;
            } else {
                curMinCut = cut;
                curMinCut_EDP = cur_EDP;
                if(minEDP > curMinCut_EDP){
                    cout << "assign cut & delete LUTToCut" << endl;
                    minEDP = curMinCut_EDP;
                    minEDP_cut = curMinCut;
                }
                delete LUTToCut;
            }
        }
        bench->removeOldNode(minEDP_cut->Pinput, cur);
        bench->fillCutToCircuit(minEDP_cut);
    }
}



//float cur_EDP = cut->errorRateCal()* (float)cut->Poutput[0]->level;
//cout << name << " cut EDP: " << cur_EDP << endl;
//    bench->pc();
//    vector<node*> removePi;
//    removePi.push_back(bench->nameToNode["s2"]);
//    removePi.push_back(bench->nameToNode["10"]);
//    bench->removeOldNode(removePi, bench->nameToNode["22"]);
//    bench->pc();

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