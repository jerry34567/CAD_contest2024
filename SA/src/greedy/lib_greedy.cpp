#include "lib_greedy.h"
#include "util.h"
#include <vector>
#include <algorithm>

double do_action(int i, const string& lib_file, const string& cost_exe, map<string, pair<string, float>>& temp_dic, map<string, vector<float>>& timing_dic) {
    switch(i){
        case 0:
            temp_dic["and"].second *= 1.1;
        break;
        case 1:
            temp_dic["and"].second *= 0.9;
        break;
        case 2:
            temp_dic["nand"].second *= 1.1;
        break;
        case 3:
            temp_dic["nand"].second *= 0.9;
        break;
        case 4:
            temp_dic["or"].second *= 1.1;
        break;
        case 5:
            temp_dic["or"].second *= 0.9;
        break;
        case 6:
            temp_dic["nor"].second *= 1.1;
        break;
        case 7:
            temp_dic["nor"].second *= 0.9;
        break;
        case 8:
            temp_dic["xor"].second *= 1.1;
        break;
        case 9:
            temp_dic["xor"].second *= 0.9;
        break;
        case 10:
            temp_dic["xnor"].second *= 1.1;
        break;
        case 11:
            temp_dic["xnor"].second *= 0.9;
        break;
        case 12:
            temp_dic["buf"].second *= 1.1;
        break;
        case 13:
            temp_dic["buf"].second *= 0.9;
        break;
        case 14:
            temp_dic["not"].second *= 1.1;
        break;
        case 15:
            temp_dic["not"].second *= 0.9;
        break;
        case 16:
            timing_dic["and"][0] += 0.1;
        break;
        case 17:
            timing_dic["and"][0] -= 0.1;
        break;
        case 18:
            timing_dic["nand"][0] += 0.1;
        break;
        case 19:
            timing_dic["nand"][0] -= 0.1;
        break;
        case 20:
            timing_dic["or"][0] += 0.1;
        break;
        case 21:
            timing_dic["or"][0] -= 0.1;
        break;
        case 22:
            timing_dic["nor"][0] += 0.1;
        break;
        case 23:
            timing_dic["nor"][0] -= 0.1;
        break;
        case 24:
            timing_dic["xor"][0] += 0.1;
        break;
        case 25:
            timing_dic["xor"][0] -= 0.1;
        break;
        case 26:
            timing_dic["xnor"][0] += 0.1;
        break;
        case 27:
            timing_dic["xnor"][0] -= 0.1;
        break;
        case 28:
            timing_dic["buf"][0] += 0.1;
        break;
        case 29:
            timing_dic["buf"][0] -= 0.1;
        break;
        case 30:
            timing_dic["not"][0] += 0.1;
        break;
        case 31:
            timing_dic["not"][0] -= 0.1;
        break;
        case 32:
            timing_dic["and"][1] += 0.1;
        break;
        case 33:
            timing_dic["and"][1] -= 0.1;
        break;
        case 34:
            timing_dic["nand"][1] += 0.1;
        break;
        case 35:
            timing_dic["nand"][1] -= 0.1;
        break;
        case 36:
            timing_dic["or"][1] += 0.1;
        break;
        case 37:
            timing_dic["or"][1] -= 0.1;
        break;
        case 38:
            timing_dic["nor"][1] += 0.1;
        break;
        case 39:
            timing_dic["nor"][1] -= 0.1;
        break;
        case 40:
            timing_dic["xor"][1] += 0.1;
        break;
        case 41:
            timing_dic["xor"][1] -= 0.1;
        break;
        case 42:
            timing_dic["xnor"][1] += 0.1;
        break;
        case 43:
            timing_dic["xnor"][1] -= 0.1;
        break;
        case 44:
            timing_dic["buf"][1] += 0.1;
        break;
        case 45:
            timing_dic["buf"][1] -= 0.1;
        break;
        case 46:
            timing_dic["not"][1] += 0.1;
        break;
        case 47:
            timing_dic["not"][1] -= 0.1;
        break;
    }
    write_genlib("temp.genlib", temp_dic, timing_dic);
    abccmd("read ./temp.genlib");
    return cost_cal(lib_file, cost_exe, "temp.v");
    
}

vector<int> myArray;

void lib_greedy(const string& lib_file, const string& cost_exe, double& best_cost, const string& output, const map<string, pair<string, float>>& gate_cost_dic, const map<string, vector<float>>& gate_timing_dic){
    double orig_cost = cost_cal(lib_file, cost_exe, "temp.v");
    map<string, pair<string, float>> temp_dic = gate_cost_dic;
    map<string, vector<float>> timing_dic = gate_timing_dic;
    map<string, pair<string, float>> record = temp_dic;
    map<string, vector<float>> record2 = timing_dic;
    double cur_best_cost = orig_cost;
    
    while(true){
        int best_action;
        bool has_better = false;
        for (int i = 0; i < 48; i++) {
            if (find(myArray.begin(), myArray.end(), i) != myArray.end()) {
                cout << i << " is in the array." << std::endl;
                continue;
            }
            cout << "I: " << i << endl;
            double after_cost = do_action(i, lib_file, cost_exe, temp_dic, timing_dic);
            temp_dic = record;
            timing_dic = record2;
            if (after_cost < cur_best_cost) {
                cur_best_cost = after_cost;
                best_action = i;
                has_better = true;
            }
        }
        if (!has_better) break;
        else {
            if (best_action%2==0) myArray.push_back(best_action+1);
            else myArray.push_back(best_action-1);
            cout << "best action: " << best_action << endl;
            cout << do_action(best_action, lib_file, cost_exe, record, record2) << endl;
            write_genlib("contest.genlib", record, record2);
            cost_cal(lib_file, cost_exe, output);
            write_liberty(record);
        }
    }
    best_cost = cur_best_cost;
    abccmd("read ./contest.genlib");
}