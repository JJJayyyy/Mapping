#include <iostream>
#include "circuit.h"
#include "circuit_cut.cpp"

using namespace std;
circuit*  Test_Cut_Replacement(circuit* bench, int k);
circuit*  Test_K_feasible_Cut(circuit* bench, int k);


int main(int argc, char *argv[]) {
    circuit *bench, *baseline;
    bench = new circuit("../circuit/sim_test.bench", true);
//    bench = new circuit("../circuit/c17.bench", true);   //
    baseline = new circuit("../circuit/c17.bench", true);   //
//    bench = new circuit("../circuit/c499.bench", true);  //
//    bench = new circuit("../circuit/c880.bench", true);  //
//    bench = new circuit("../circuit/c1196.bench", true); //
//    bench = new circuit("../circuit/c1355.bench", true); //
//    bench = new circuit("../circuit/c1908.bench", true); //
//    bench = new circuit("../circuit/c2670.bench", true); //
//    bench = new circuit("../circuit/c3540.bench", true); //
//    bench = new circuit("../circuit/c5315.bench", true); //
//    bench = new circuit("../circuit/c6288.bench", true); //
//    bench = new circuit("../circuit/c7552.bench", true); //

//    bench = new circuit("../circuit/s27.bench", true);
//    bench = new circuit("../circuit/s1238.bench", true);
//    bench = new circuit("../circuit/s1423.bench", true);
//    bench = new circuit("../circuit/s1494.bench", true);
//    bench = new circuit("../circuit/s5378.bench", true); //
//    bench = new circuit("../circuit/s9234.bench", true); //

//    bench = new circuit("../circuit/s13207.bench", true); //
//    bench = new circuit("../circuit/s35932.bench", true);
//    bench = new circuit("../circuit/s38584.bench", true); //

    if(bench->Node_list.empty() || baseline->Node_list.empty())
        return 0;
    baseline->Depth_Balancing();
    float circuit_EDP = baseline->Error_Rate_Calculation() * (float)bench->max_lvl;
    cout << "Baseline bench_EDP: " << circuit_EDP << endl;
//    bench->pc();
//    Test_K_feasible_Cut(bench);
    bench = Test_Cut_Replacement(bench, 4);
//    bench->pc();

    bench->Levelization();
    bench->Splitter_Insertion();
    bench->Depth_Balancing();
    circuit_EDP = bench->Error_Rate_Calculation() * (float)bench->max_lvl;;
    cout << "\nFinal bench_EDP: " << circuit_EDP << endl;
    delete bench;
    return 0;
}


circuit* Test_K_feasible_Cut(circuit* bench){
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
            vector<string> LUT = cut->LUT_TO_QM();
            cout << "Input_LUT: ";
            for(const auto& i : LUT) cout << i << " ";    cout << endl;
            cout <<  endl;
        }
    }
    return bench;
}


circuit* Test_Cut_Replacement(circuit* bench, int k){
    vector<string> benchNodeNameList;
    string  root_node_name;

    for(const auto & n : bench->Node_list)
        benchNodeNameList.push_back(n->node_name);


    for(int i = benchNodeNameList.size()-1; i >= 0; i--){
        root_node_name = benchNodeNameList[i];

        // check whether current root node exists in the bench circuit
        if(bench->nameToNode.find(root_node_name) == bench->nameToNode.end()){
            cout << "\n***********" << endl;
            cout << root_node_name << " has been removed in previous update" << endl;
            cout << "***********" << endl;
            continue;
        }

        node* root_node = bench->nameToNode[root_node_name];
        if(root_node->gtype == circuit::gateToInt("PI") || root_node->gtype == circuit::gateToInt("DFF"))
            continue;

//        debug = bench->nameToNode["I841"];
//        for(auto& xxx : debug->unodes)
//            cout << "aa: " << xxx->node_name << "\t";
//        cout <<  endl;
//        if(root_node_name == "880")
//            bench->pc();
//        node* root_node = bench->nameToNode["442"];

        vector<vector<string>>rootNodeAllCutList = K_Feasible_Cut(root_node, k);
        if(rootNodeAllCutList.empty()) continue;
        int idx = 0;
        float EDPImprovement = 0;
        circuit * ImprovementCut = nullptr;
        cout << "///////////////////" << endl;
        for(auto& currentCutPINames : rootNodeAllCutList){
            unordered_map<string, int> PI_PO_Level;
            string name = root_node->node_name;
            string newCutName = root_node->node_name;
            newCutName.append("_cutNew_");
            newCutName.append(to_string(idx));
            name.append("_cut_");
            name.append(to_string(idx++));

            //generate cut circuits {C}
            circuit* cut = Construct_Cut_Circuit(bench, currentCutPINames, root_node, name);

            //convert {C} to simplified boolean functions {F}
            vector<string> LUT = cut->LUT_TO_QM();

            if(LUT.empty()){
                cout << name << " LUT is always 0, ignore it" << endl;
                continue;
            }


            //generate a new cut circuit {C_new} based on {F}
            circuit* LUTToCut = Construct_LUT_Circuit(LUT, PI_PO_Level, currentCutPINames, root_node->node_name, newCutName);

            //compare the error rates between {C} and {C_new}
            float cut_EDP = cut->Error_Rate_Calculation()* (float)cut->max_lvl;
            float LUTToCut_EDP = LUTToCut->Error_Rate_Calculation() * (float)LUTToCut->max_lvl;;
            cout << name << " EDP: " << cut_EDP << "\t\t" << newCutName << " EDP: " << LUTToCut_EDP << endl;
            if((cut_EDP - LUTToCut_EDP) > EDPImprovement){
                cout << "!!!!!!\nImprove: " << cut_EDP - LUTToCut_EDP - EDPImprovement << "\n!!!!!!" << endl;
                EDPImprovement = cut_EDP - LUTToCut_EDP;
                ImprovementCut = LUTToCut;
            }else {
                delete LUTToCut;
            }
            if(idx != rootNodeAllCutList.size()) cout << endl;
            delete cut;
        }
        if(ImprovementCut != nullptr) cout << "improvement circuit: " << ImprovementCut->circuit_name << endl;
        cout << "///////////////////" << endl;
        if(ImprovementCut != nullptr){
            bench->Remove_Old_Node(ImprovementCut->Pinput, root_node);
//            cout <<"sss" << endl;
            bench->Fill_Cut_To_Circuit(ImprovementCut);
//            bench->pc();
        }
    }
    bench->Levelization();
    return bench;
}


//int main(int argc, char *argv[]) {
//    circuit *bench;
//    bench = new circuit("../circuit/sim_test.bench", true);
//
//}