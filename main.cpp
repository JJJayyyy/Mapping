#include <iostream>
#include "circuit.h"
#include "circuit_cut.cpp"
#include "LUT_generation.cpp"

using namespace std;
circuit*  Test_Cut_Replacement(circuit* bench, int k, int faninSize);
circuit*  Test_K_feasible_Cut(circuit* bench, int k, int faninSize);
circuit* SFQmap_Alg(circuit* bench, int k, int faninSize);
void LUT_To_NewCut(int fan_ins, int faninSize);
void LUT_To_NewCut_Debugging(int fan_ins, int faninSize);


int main(int argc, char *argv[]) {
    circuit *bench = nullptr, *baseline = nullptr, *SFQmap = nullptr;
    int faninSize = 4;
    bool benchOperation = false;
    bool baselineOperation = false;
    bool SFQmapOperation = false;

//    SFQmap = new circuit("../circuit/sim_test.bench", true);

//    bench = new circuit("../circuit/sim_test.bench", true);
//    bench = new circuit("../circuit/FA.bench", true);
//    bench = new circuit("../circuit/c880.bench", true);
//    bench = new circuit("../circuit/c17.bench", true);   //
//    bench = new circuit("../circuit/s27.bench", true); //6.28->9.4

//    baseline = new circuit("../circuit/b04.bench", true);   //
//    SFQmap = new circuit("../circuit/b04.bench", true);   //
    bench = new circuit("../circuit/b04.bench", true);  //

//    baseline = new circuit("../circuit/b11.bench", true);   //
//    SFQmap = new circuit("../circuit/b11.bench", true);   //
//    bench = new circuit("../circuit/b11.bench", true);  //

//    baseline = new circuit("../circuit/b14.bench", true);   //
//    SFQmap = new circuit("../circuit/b14.bench", true);   //
//    bench = new circuit("../circuit/b14.bench", true);  //

//    baseline = new circuit("../circuit/c880.bench", true);  //
//    SFQmap = new circuit("../circuit/c880.bench", true);  //
//    bench = new circuit("../circuit/c880.bench", true);  //

//    baseline = new circuit("../circuit/c1355.bench", true); //
//    SFQmap = new circuit("../circuit/c1355.bench", true); //
//    bench = new circuit("../circuit/c1355.bench", true); //

//    baseline = new circuit("../circuit/c1908.bench", true); //
//    SFQmap = new circuit("../circuit/c1908.bench", true); //
//    bench = new circuit("../circuit/c1908.bench", true); //

//    baseline = new circuit("../circuit/c2670.bench", true); //
//    SFQmap = new circuit("../circuit/c2670.bench", true); //
//    bench = new circuit("../circuit/c2670.bench", true); //

//    baseline = new circuit("../circuit/c3540.bench", true); //
//    SFQmap = new circuit("../circuit/c3540.bench", true); //
//    bench = new circuit("../circuit/c3540.bench", true); //

//    baseline = new circuit("../circuit/c5315.bench", true); //
//    SFQmap = new circuit("../circuit/c5315.bench", true); //
//    bench = new circuit("../circuit/c5315.bench", true); //

//    baseline = new circuit("../circuit/c6288.bench", true); //
//    SFQmap = new circuit("../circuit/c6288.bench", true); //
//    bench = new circuit("../circuit/c6288.bench", true); //

//    baseline = new circuit("../circuit/c7552.bench", true); //
//    SFQmap = new circuit("../circuit/c7552.bench", true); //
//    bench = new circuit("../circuit/c7552.bench", true); //

//    baseline = new circuit("../circuit/s1238.bench", true); //
//    SFQmap = new circuit("../circuit/s1238.bench", true); //
//    bench = new circuit("../circuit/s1238.bench", true); //

//    baseline = new circuit("../circuit/s1423.bench", true); //
//    SFQmap = new circuit("../circuit/s1423.bench", true); //
//    bench = new circuit("../circuit/s1423.bench", true); //

//    baseline = new circuit("../circuit/s1494.bench", true); //
//    SFQmap = new circuit("../circuit/s1494.bench", true); //
//    bench = new circuit("../circuit/s1494.bench", true); //

//    baseline = new circuit("../circuit/s5378.bench", true); //
//    SFQmap = new circuit("../circuit/s5378.bench", true); //
//    bench = new circuit("../circuit/s5378.bench", true); //

//    baseline = new circuit("../circuit/s9234.bench", true); //
//    SFQmap = new circuit("../circuit/s9234.bench", true); //
//    bench = new circuit("../circuit/s9234.bench", true); //

//    baseline = new circuit("../circuit/s13207.bench", true); //
//    SFQmap = new circuit("../circuit/s13207.bench", true); //
//    bench = new circuit("../circuit/s13207.bench", true); //

//    baseline = new circuit("../circuit/s35932.bench", true);
//    SFQmap = new circuit("../circuit/s35932.bench", true);
//    bench = new circuit("../circuit/s35932.bench", true);   //

//    baseline = new circuit("../circuit/s38584.bench", true); //
//    SFQmap = new circuit("../circuit/s38584.bench", true); //
//    bench = new circuit("../circuit/s38584.bench", true); //


    if(baseline != nullptr){
        baseline->Highfanin_To_Low(faninSize);
        baseline->Levelization();
        baseline->Depth_Balancing();
        baseline->Levelization();
        baselineOperation = true;
        cout << "base Done" << endl;
    }

    if(SFQmap != nullptr){
        SFQmap = SFQmap_Alg(SFQmap, 3, faninSize);
        SFQmap->Highfanin_To_Low(faninSize);
        SFQmap->Levelization();
        SFQmap->Depth_Balancing();
        SFQmap->Levelization();
        SFQmapOperation = true;
        cout << "SFQmap Done" << endl;
    }

    if(bench != nullptr){
        //    bench = Test_K_feasible_Cut(bench, 3);
        bench = Test_Cut_Replacement(bench, 3, faninSize);
        bench->Splitter_Insertion();
        bench->Levelization();
        bench->Depth_Balancing();
        bench->Levelization();
        bench->Post_Optimization();
        benchOperation = true;
        cout << "bench Done" << endl;
    }


    if(baselineOperation)
        baseline->pcGateOnly();

    if(SFQmapOperation)
        SFQmap->pcGateOnly();

    if(benchOperation)
        bench->pcGateOnly();

    //LUT test
//    LUT_To_NewCut(4, faninSize);
//    LUT_To_NewCut_Debugging(3, faninSize);

    delete bench;
    delete baseline;
    delete SFQmap;
    return 0;
}


circuit* Test_K_feasible_Cut(circuit* bench, int k, int faninSize){
    vector<string> benchNodeNameList;
    string  root_node_name;

    for(const auto & n : bench->Node_list)
        benchNodeNameList.push_back(n->node_name);

    for(int i = int(benchNodeNameList.size())-1; i >= 0; i--){
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
        vector<vector<string>>rootNodeAllCutList = K_Feasible_Cut(root_node, k);
        if(rootNodeAllCutList.empty()) continue;
        int idx = 0;
        cout << "///////////////////" << endl;
        for(auto& currentCutPINames : rootNodeAllCutList) {
            unordered_map<string, int> PI_PO_Level;
            string name = root_node->node_name;
            string newCutName = root_node->node_name;
            newCutName.append("_cutNew_");
            newCutName.append(to_string(idx));
            name.append("_cut_");
            name.append(to_string(idx++));

            //generate cut circuits {C}
            circuit *cut = Construct_Cut_Circuit(bench, currentCutPINames, root_node, name, faninSize);

            //convert {C} to simplified boolean functions {F}
            vector<string> LUT = cut->LUT_TO_QM();
            if (LUT.empty()) {
                cout << name << " LUT is always 0, ignore it" << endl;
                continue;
            }
            cout << "Optimized BF : ";
            for (const auto &implicants : LUT) cout << implicants << " ";
            cout << endl;
        }
        cout << "///////////////////" << endl;
    }
    return bench;
}


circuit* Test_Cut_Replacement(circuit* bench, int k, int faninSize){
    vector<string> benchNodeNameList;
    string  root_node_name;

    for(const auto & n : bench->Node_list)
        benchNodeNameList.push_back(n->node_name);

    for(int i = int(benchNodeNameList.size())-1; i >= 0; i--){
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

        vector<vector<string>>rootNodeAllCutList = K_Feasible_Cut(root_node, k);
        if(rootNodeAllCutList.empty())
            continue;
        int idx = 0;
//        double EDPImprovement = 0;
        int EDPImprovement = 0;
        circuit * ImprovementCut = nullptr;
        cout << "///////////////////" << endl;

        int DFFcounter = bench->SDFFNameCounter;
        int SPcounter = bench->splitterNameCounter;

        for(auto& currentCutPINames : rootNodeAllCutList){
            string name = root_node->node_name;
            string newCutName = root_node->node_name;
            newCutName.append("_cutNew_");
            newCutName.append(to_string(idx));
            name.append("_cut_");
            name.append(to_string(idx++));

            //generate cut circuits {C}
            circuit* cut = Construct_Cut_Circuit(bench, currentCutPINames, root_node, name, faninSize);

            //convert {C} to simplified boolean functions {F}
            vector<string> LUT = cut->LUT_TO_QM();
            if(LUT.empty())
                continue;
//            assert(!LUT.empty());
            vector<int> pi_levels;
            for(auto& pi : cut->Pinput){
                pi_levels.push_back(pi->level);
//                cout << pi->node_name << " " << pi->level << endl;
            }
            assert(currentCutPINames.size() == pi_levels.size());

            //generate a new cut circuit {C_new} based on {F}
            circuit* LUTToCut = Construct_LUT_Circuit(LUT, currentCutPINames, pi_levels, root_node->node_name, newCutName, faninSize);

            //generate copy circuit of {C} and {C_new}
            circuit* Copycut = Construct_Cut_Circuit(bench, currentCutPINames, root_node, name, faninSize);
            circuit* CopyLUTToCut = Construct_LUT_Circuit(LUT, currentCutPINames, pi_levels, root_node->node_name, newCutName, faninSize);

            Copycut->splitterNameCounter = SPcounter;
            Copycut->SDFFNameCounter = DFFcounter;
            Copycut->Splitter_Insertion();
            Copycut->Levelization_By_PI();
            Copycut->Depth_Balancing();
            Copycut->Levelization_By_PI();
            cout << "cut " << cut->circuit_name << " copy constructed" << endl;

            CopyLUTToCut->splitterNameCounter = SPcounter;
            CopyLUTToCut->SDFFNameCounter = DFFcounter;
            CopyLUTToCut->Splitter_Insertion();
            CopyLUTToCut->Levelization_By_PI();
            CopyLUTToCut->Depth_Balancing();
            CopyLUTToCut->Levelization_By_PI();
            cout << "LUTcut " << LUTToCut->circuit_name << " copy constructed" << endl;


            //compare the PND between {C} and {C_new}
            int cut_EDP = Copycut->JJ_Calculation() * Copycut->max_lvl;
            int LUTToCut_EDP = CopyLUTToCut->JJ_Calculation() * CopyLUTToCut->max_lvl;

            cout << name << " PND: " << cut_EDP << "\t\t" << newCutName << " PND: " << LUTToCut_EDP << endl;
            if((cut_EDP - LUTToCut_EDP) > EDPImprovement){
                delete ImprovementCut;
                cout << "!!!!!!\nImprove: " << cut_EDP - LUTToCut_EDP << "\n!!!!!!" << endl;
//                Copycut->pc();
//                CopyLUTToCut->pc();
                EDPImprovement = cut_EDP - LUTToCut_EDP;
                ImprovementCut = LUTToCut;
            }else {
                delete LUTToCut;
            }
            if(idx != rootNodeAllCutList.size()) cout << endl;
            delete cut;
            delete Copycut;
            delete CopyLUTToCut;
        }
        if(ImprovementCut != nullptr) cout << "improvement circuit: " << ImprovementCut->circuit_name << endl;
        cout << "///////////////////" << endl;
        if(ImprovementCut != nullptr){
            bench->Remove_Old_Node(ImprovementCut->Pinput, root_node);
            bench->Fill_Cut_To_Circuit(ImprovementCut);
        }
    }
    bench->Levelization();
    return bench;
}




circuit* SFQmap_Alg(circuit* bench, int k, int faninSize){
    vector<string> benchNodeNameList;
    string  root_node_name;

    for(const auto & n : bench->Node_list)
        benchNodeNameList.push_back(n->node_name);

    for(int i = int(benchNodeNameList.size())-1; i >= 0; i--){
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

        vector<vector<string>>rootNodeAllCutList = K_Feasible_Cut(root_node, k);
        if(rootNodeAllCutList.empty())
            continue;
        int idx = 0;
        int DFF_improvement = 0;
        circuit * ImprovementCut = nullptr;
        cout << "///////////////////" << endl;

        int DFFcounter = bench->SDFFNameCounter;
        int SPcounter = bench->splitterNameCounter;
        int newCounter = bench->newNodeNameCounter;

        for(auto& currentCutPINames : rootNodeAllCutList){
            string name = root_node->node_name;
            string newCutName = root_node->node_name;
            newCutName.append("_cutNew_");
            newCutName.append(to_string(idx));
            name.append("_cut_");
            name.append(to_string(idx++));
            //generate cut circuits {C}

            circuit* cut = Construct_Cut_Circuit(bench, currentCutPINames, root_node, name, faninSize);

            //convert {C} to simplified boolean functions {F}
            vector<string> LUT = cut->LUT_TO_QM();
            if(LUT.empty())
                continue;
//            assert(!LUT.empty());
            vector<int> pi_levels;
            for(auto& pi : cut->Pinput){
                pi_levels.push_back(1);
//                pi_levels.push_back(pi->level);
            }
            assert(currentCutPINames.size() == pi_levels.size());
            //generate a new cut circuit {C_new} based on {F}
            circuit* LUTToCut = Construct_LUT_Circuit(LUT, currentCutPINames, pi_levels, root_node->node_name, newCutName, faninSize);

//            circuit* cut1 = Construct_Cut_Circuit(bench, currentCutPINames, root_node, name);
//            circuit* LUTToCut1 = Construct_LUT_Circuit(LUT, currentCutPINames, root_node->node_name, newCutName);
            //compare the error rates between {C} and {C_new}
//            cut1->splitterNameCounter = SPcounter;
//            cut1->SDFFNameCounter = DFFcounter;
//            cut1->Splitter_Insertion();
//            cut1->Depth_Balancing();
//            cut1->Levelization();
//
//            LUTToCut1->splitterNameCounter = SPcounter;
//            LUTToCut1->SDFFNameCounter = DFFcounter;
//            LUTToCut1->Splitter_Insertion();
//            LUTToCut1->Depth_Balancing();
//            LUTToCut1->Levelization();

//            int cut_DFF_Num = cut1->gateCounter["SDFF"];
//            int LUTcut_DFF_Num = LUTToCut1->gateCounter["SDFF"];
            int cut_DFF_Num = cut->max_lvl;
            int LUTcut_DFF_Num = LUTToCut->max_lvl;

            cout << name << " DFFs: " << cut_DFF_Num << "\t\t" << newCutName << " DFFs: " << LUTcut_DFF_Num << endl;
            if((cut_DFF_Num - LUTcut_DFF_Num) > DFF_improvement){
                cout << "!!!!!!\nImprove: " << cut_DFF_Num - LUTcut_DFF_Num << "\n!!!!!!" << endl;
                DFF_improvement = cut_DFF_Num - LUTcut_DFF_Num;
                ImprovementCut = LUTToCut;
            }else {
                delete LUTToCut;
            }
            if(idx != rootNodeAllCutList.size()) cout << endl;
            delete cut;
//            delete cut1;
//            delete LUTToCut1;
        }
        if(ImprovementCut != nullptr) cout << "improvement circuit: " << ImprovementCut->circuit_name << endl;
        cout << "///////////////////" << endl;
        if(ImprovementCut != nullptr){
            bench->Remove_Old_Node(ImprovementCut->Pinput, root_node);
            bench->Fill_Cut_To_Circuit(ImprovementCut);
//            bench->pc();
        }

    }
    bench->Levelization();
    return bench;
}




void LUT_To_NewCut(int fan_ins, int faninSize){
    int N = pow(2, fan_ins);
    string name;
    vector<string> LUT;
    vector<string> LUT_newCut;
    vector<node*> fin_node;
    vector<string> fin_node_names;
    vector<vector<int>> total;
    node* fin;

    for(int i = 0; i < fan_ins; i++) {
        name = "a";
        name.append(to_string(i));
        fin = new node(name);
        fin_node.push_back(fin);
        fin_node_names.push_back(name);
    }

    for(int i = 2; i < N; i++){
        combination(i, N, total);
    }

    string out = "out", lut_circuit_name = "x";

    for(auto& item : total){
        LUT.clear();
        LUT_newCut.clear();
        cout << "LUT : ";
        for (const auto &implicants : item) cout << implicants << " ";
        cout << endl;
        LUT = circuit::QM(item, fin_node);
        cout << "Optimized BF : ";
        for (const auto &implicants : LUT) cout << implicants << " ";
        cout << endl;

        vector<int> pi_levels;
        pi_levels.reserve(fin_node_names.size());
        for(auto& pi : fin_node_names){
            pi_levels.push_back(1);
        }

        circuit* LUTToCut = Construct_LUT_Circuit(LUT, fin_node_names, pi_levels, out, lut_circuit_name, faninSize);
        LUT_newCut = LUTToCut->LUT_TO_QM();
        cout << "New cut BF : ";
        for (const auto &implicants : LUT_newCut) cout << implicants << " ";
        cout << endl;
        cout << "/////////////" << endl;
        assert(LUT.size() == LUT_newCut.size());
        for(int i = 0; i < LUT.size(); i++){
            assert(LUT[i] == LUT_newCut[i]);
        }
        delete LUTToCut;
    }

    for(auto x : fin_node) delete x;
}


void LUT_To_NewCut_Debugging(int fan_ins, int faninSize){
    string name;
    vector<int> LUT;
    vector<string> LUT_string;
    vector<string> LUT_newCut;
    vector<node*> fin_node;
    vector<string> fin_node_names;
    node* fin;

    for(int i = 0; i < fan_ins; i++) {
        name = "a";
        name.append(to_string(i));
        fin = new node(name);
        fin_node.push_back(fin);
        fin_node_names.push_back(name);
    }

    string out = "out", lut_circuit_name = "x";

    LUT = {0, 1, 2, 3, 4, 5, 6, 7};
    LUT_string = circuit::QM(LUT, fin_node);
    cout << "F : ";
    for (const auto &implicants : LUT_string) cout << implicants << " ";
    cout << endl;

    vector<int> pi_levels;
    pi_levels.reserve(fin_node_names.size());
    for(auto& pi : fin_node_names){
        pi_levels.push_back(1);
    }
    circuit* LUTToCut = Construct_LUT_Circuit(LUT_string, fin_node_names, pi_levels, out, lut_circuit_name, faninSize);
    LUT_newCut = LUTToCut->LUT_TO_QM();
    cout << "New BF : ";
    for (const auto &implicants : LUT_newCut) cout << implicants << " ";
    cout << endl;
    LUTToCut->pc();

    delete LUTToCut;
    for(auto x : fin_node) delete x;
}

