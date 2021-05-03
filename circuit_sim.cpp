//
// Created by Jay on 4/25/21.
//
#include "circuit.h"
void simulation(node* cur_node);

/**Function*************************************************************
    Interface between LUT and QM
***********************************************************************/
vector<string>circuit::LUTtoQM(){
    vector<vector<int>>LUT = LUT_generation();
    cout << "LUT generation is done" << endl;
    vector<int>LUT_output_1;
    for(int i=0; i<LUT.size(); i++){        // k inputs, 1 outputs
        if(LUT[i][0]==1)   LUT_output_1.push_back(i);
    }
//    for(auto i : LUT_output_1) cout << i << " ";
//    cout << endl;
    return circuit::QM(LUT_output_1, Pinput);
}


/**Function*************************************************************
    Generate the LUT
***********************************************************************/
vector<vector<int>>circuit::LUT_generation() {

    vector<vector<int>> PO_results;
    if(level_flag != 1) {
        cout << "circuit needs to be leveled first before inserting the splitter binary trees" << endl;
        return PO_results;
    }
    int LUT_row_size = pow(2,Pinput.size());
    for(int row = 0; row <LUT_row_size; row++){
        vector<int> cur_row_PO_results;
        int ini_value = row;
        for(auto& node : Node_list)  node->val_sim = -1;
        for(auto& pi : Pinput){            // set the Pis according to the LUT row
            if(ini_value > 1){
                pi->val_sim = ini_value % 2;
                ini_value = ini_value/2;
            } else{
                pi->val_sim =  ini_value;
                if(ini_value == 1) ini_value-=1;
            }
//            cout << "Primary num and val is " << pi->node_name << "," << pi->val_sim << endl;
        }
        for(int lvl=0; lvl<=max_lvl; lvl++){
            for(const auto& cur_lvl_node_name : levelList[lvl]){
                simulation(nameToNode[cur_lvl_node_name]);
                if(lvl == max_lvl) cur_row_PO_results.push_back(nameToNode[cur_lvl_node_name]->val_sim);
            }
        }
        PO_results.push_back(cur_row_PO_results);
//        cout << row << ": ";
//        for(auto x : cur_row_PO_results) cout << x << " ";    cout << endl;
    }
    return PO_results;
}

/**Function*************************************************************
    Do the logic simulation
***********************************************************************/

void simulation(node* cur_node){
    int sum;
    switch(cur_node->gtype){
        case (0): break;

        case (1):
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            break;

        case (2):       // XOR gate
            sum = 0;
            for (auto & unode : cur_node->unodes)   sum = sum^unode->val_sim;
            cur_node->val_sim = sum;
            break;

        case (3):       // XNOR gate
            sum = 1;
            for (auto & unode : cur_node->unodes)   sum = sum^unode->val_sim;
            cur_node->val_sim = sum;
            break;

        case (4):       // OR gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1){
                    cur_node->val_sim = 1;
                    break;
                }
                else cur_node->val_sim = 0;
            }
            break;

        case (5):       // NOR gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1){
                    cur_node->val_sim = 0;
                    break;
                }
                else cur_node->val_sim = 1;
            }
            break;

        case  (6):       // AND gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 0){
                    cur_node->val_sim = 0;
                    break;
                }
                else cur_node->val_sim = 1;
            }
            break;

        case (7):       // NAND gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 0){
                    cur_node->val_sim = 1;
                    break;
                }
                else cur_node->val_sim = 0;
            }
            break;

        case (8):       // NOT gate
            cur_node->val_sim = !(cur_node->unodes[0]->val_sim);
            break;

        case (9):       // DFF gate
            // TODO: DFF still needs further update
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            cout<<"DFF! not complete!"<<endl;
            break;

        case (10):      // SDFF gate
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            break;

        default:
            cout<<"it's not valid type !"<<endl;
            break;
    }
}


float circuit::errorRateCal(){
    float result = 1;
    for(auto cur_node : Node_list){
        if(cur_node->gtype > 0) {
            result = result*(1-errorRateMap[intToGate(cur_node->gtype)]);
            /** ER calculation check
            cout << "current gate: " << intToGate(cur_node->gtype) \
            << "\tER: " << errorRateMap[intToGate(cur_node->gtype)] << endl;
            cout <<  "Correct rate: " << result << endl;
             **/
        }
    }
//    cout << "Total error rate of the circuit is: " << 1-result << endl;
    return 1-result;
}