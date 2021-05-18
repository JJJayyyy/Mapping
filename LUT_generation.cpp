//
// Created by Jay on 5/12/21.
//

void uploadResult(vector<int>& vecInt, vector<vector<int>>& final){
    vector<int> base;
    for(int i = 0; i < vecInt.size(); ++i)  if(vecInt[i] == 1)  base.push_back(i);
    if(!base.empty())
        final.push_back(base);
}

bool compare(int a, int b){
    return a > b;
}

void combination(int c, int total, vector<vector<int>> &final) {
    //initial first combination like:1,1,0,0,0
    vector<int> vecInt(total,0);
    for(int i = 0; i < c; ++i)  vecInt[i] = 1;
    uploadResult(vecInt, final);
    for(int i = 0; i < total - 1; ++i){
        if(vecInt[i] == 1 && vecInt[i+1] == 0){
            //1. first exchange 1 and 0 to 0 1
            swap(vecInt[i], vecInt[i+1]);
            //2.move all 1 before vecInt[i] to left
            sort(vecInt.begin(),vecInt.begin() + i, compare);
            //after step 1 and 2, a new combination is exist
            uploadResult(vecInt, final);
            //try do step 1 and 2 from front
            i = -1;
        }
    }
}
