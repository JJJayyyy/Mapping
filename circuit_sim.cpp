//
// Created by Jay on 4/25/21.
//
#include "circuit.h"
void Node_Simulation(node* cur_node);



/**Function*************************************************************
    Interface between LUT and QM
***********************************************************************/
vector<string>circuit::LUT_TO_QM(){
    vector<int>LUT_output_high;
    vector<string> boolExpression;
    vector<vector<int>>LUT;

    LUT = LUT_Generation();

    for(int i=0; i<LUT.size(); i++){                        // k inputs, 1 outputs
        if(LUT[i][0]==1)   LUT_output_high.push_back(i);
    }

    cout << "LUT: ";
    for(auto i : LUT_output_high) {
        cout << i << " ";
    }
    cout << endl;

    if(!LUT_output_high.empty()){
        boolExpression = circuit::QM(LUT_output_high, Pinput);
    }
    LUT.clear();
    LUT_output_high.clear();
    return boolExpression;
}



/**Function*************************************************************
    Generate the LUT
***********************************************************************/
vector<vector<int>>circuit::LUT_Generation() {
    vector<vector<int>> PO_results;
    int LUT_row_size = pow(2,Pinput.size());

    if(level_flag != 1) {
        cout << "circuit needs to be leveled first before inserting the splitter binary trees" << endl;
        return PO_results;
    }

    for(int row = 0; row < LUT_row_size; row++){
        vector<int> cur_row_PO_results;
        int ini_value = row;
        for(auto& node : Node_list){
            node->val_sim = -1;
        }
        for(int i = int(Pinput.size())-1; i >= 0; i--){             // set the Pis according to the LUT row
            node* pi = Pinput[i];
            if(ini_value > 1){
                pi->val_sim = ini_value % 2;
                ini_value = ini_value/2;
            } else{
                pi->val_sim =  ini_value;
                if(ini_value == 1)
                    ini_value-=1;
            }
        }
        for(int lvl=0; lvl<=max_lvl; lvl++){
            for(const auto& cur_lvl_node_name : levelList[lvl]){
                Node_Simulation(nameToNode[cur_lvl_node_name]);
                if(lvl == max_lvl) cur_row_PO_results.push_back(nameToNode[cur_lvl_node_name]->val_sim);
//                cout << "node " << nameToNode[cur_lvl_node_name]->node_name
//                << "\tvalue:" << nameToNode[cur_lvl_node_name]->val_sim << endl;
            }
        }
        PO_results.push_back(cur_row_PO_results);
    }
    return PO_results;
}



/**Function*************************************************************
    Do the logic simulation
***********************************************************************/
void Node_Simulation(node* cur_node){
    int sum;
    switch(cur_node->gtype){
        case (0): break;

        case (1):
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            break;

        case (2):       // XOR gate
            sum = 0;
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1){
                    sum++;
                }
            }
            if(sum == 0 || sum == 2)
                cur_node->val_sim = 0;
            else if (sum == 1){
                cur_node->val_sim = 1;
            } else{
                string errorMessage = "XOR fan-ins error";  // Exception
                throw runtime_error(errorMessage);
            }
            break;

        case (3):       // XNOR gate
            sum = 0;
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1){
                    sum++;
                }
            }
            if(sum == 0 || sum == 2)
                cur_node->val_sim = 1;
            else if (sum == 1){
                cur_node->val_sim = 0;
            } else{
                string errorMessage = "XNOR fan-ins error";  // Exception
                throw runtime_error(errorMessage);
            }
            break;

        case (4):       // OR gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1){
                    cur_node->val_sim = 1;
                    break;
                }
                else
                    cur_node->val_sim = 0;
            }
            break;

        case (5):       // NOR gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1){
                    cur_node->val_sim = 0;
                    break;
                }
                else
                    cur_node->val_sim = 1;
            }
            break;

        case  (6):       // AND gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 0){
                    cur_node->val_sim = 0;
                    break;
                }
                else
                    cur_node->val_sim = 1;
            }
            break;

        case (7):       // NAND gate
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 0){
                    cur_node->val_sim = 1;
                    break;
                }
                else
                    cur_node->val_sim = 0;
            }
            break;

        case (8):       // NOT gate
            cur_node->val_sim = !(cur_node->unodes[0]->val_sim);
            break;

        case (9):       // BUFF gate
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            break;

        case (10):      // MAJ gate
            sum = 0;
            for (auto & unode : cur_node->unodes){
                if(unode->val_sim == 1) {
                    sum++;
                }
                if(sum >= (cur_node->unodes.size()+1)/2){
                    cur_node->val_sim = 1;
                    break;
                }
                else{
                    cur_node->val_sim = 0;
                }
            }
            break;

        case (11):       // DFF gate
            // TODO: DFF still needs further update
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            cout<<"DFF! not allowed!"<<endl;
            break;

        case (12):      // SDFF gate
            cur_node->val_sim = cur_node->unodes[0]->val_sim;
            break;

        default:
            cout<<"It's not valid type !"<<endl;
            break;
    }
}