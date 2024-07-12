#include "lib_greedy.h"
#include "util.h"

extern CostMgr* costMgr;

void print_temp(const std::map<std::string, std::pair<std::string, float>>& temp_dic) {
    // 打印地圖內容
    for (const auto& item : temp_dic) {
        std::cout << std::left << std::setw(15) << item.first
                  << std::left << std::setw(15) << item.second.first
                  << std::left << std::setw(15) << item.second.second << std::endl;
    }
}

void print_timing(const std::map<std::string, std::vector<float>>& timing_dic) {

    // 打印地圖內容
    for (const auto& item : timing_dic) {
        std::cout << std::left << std::setw(15) << item.first;
        
        // 打印對應的浮點數向量
        for (const auto& val : item.second) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}

double do_action(int i, map<string, pair<string, float>>& temp_dic, map<string, vector<float>>& timing_dic, bool buf_flag) {
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
    write_genlib("./temp.genlib", temp_dic, timing_dic, 0);
    write_liberty("temp_liberty.lib", temp_dic);
    if (buf_flag) {
        abccmd("read ./temp_liberty.lib");
    }
    else {
        abccmd("read ./temp.genlib");
    }
    return costMgr->cost_cal(0, buf_flag);
    
}


double do_action_use_turtle(int i, map<string, pair<string, float>>& temp_dic, map<string, vector<float>>& timing_dic, bool buf_flag) {
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
    write_genlib("./temp.genlib", temp_dic, timing_dic, 0);
    write_liberty("temp_liberty.lib", temp_dic);
    abccmd("super -I 4 -L 2 ./temp.genlib");
    return costMgr->cost_cal_use_turtle(0, buf_flag, 1);
    
}

bool sortbysec(const pair<int,double> &a, const pair<int,double> &b) {
    return (a.second < b.second);
}

void lib_greedy(double& best_cost, const map<string, pair<string, float>>& gate_cost_dic, const map<string, vector<float>>& gate_timing_dic, bool buf_flag){
    double orig_cost = costMgr->cost_cal(0, buf_flag);
    map<string, pair<string, float>> temp_dic = gate_cost_dic;
    map<string, vector<float>> timing_dic = gate_timing_dic;
    map<string, pair<string, float>> record = temp_dic;
    map<string, vector<float>> record2 = timing_dic;
    double cur_best_cost = orig_cost;
    
    while(true){
        int best_action;
        bool has_better = false;
        vector<pair<int, double>> improve_actions;
        double record_cost = cur_best_cost;
        for (int i = 0; i < 48; i++) {
            cout << "I: " << i << endl;
            double after_cost = do_action(i, temp_dic, timing_dic, buf_flag);
            cout << " cur_best: " << cur_best_cost << " this: " << after_cost << endl;
            // print_temp(temp_dic);
            // print_timing(timing_dic);
            // cout << endl;
            temp_dic = record;
            timing_dic = record2;
            if (after_cost < record_cost) {
                improve_actions.push_back(make_pair(i, after_cost));
            }
            if (after_cost < cur_best_cost) {
                cur_best_cost = after_cost;
                best_action = i;
                has_better = true;
            }
        }
        if (!has_better) break;
        else {
            // cout << do_action(best_action, record, record2, buf_flag) << endl;
            sort(improve_actions.begin(), improve_actions.end(), sortbysec);
            double temp_best_cost = record_cost;
            double times = 0;
            for (int i = 0; i < improve_actions.size() && i < 3; i++) {
                cout << improve_actions[i].first << endl;
                double cost = do_action(improve_actions[i].first, temp_dic, timing_dic, buf_flag);
                if (cost < temp_best_cost) {
                    temp_best_cost = cost;
                    times = i;
                }
                if (cost < cur_best_cost) {
                    cur_best_cost = cost;
                }
            }
            temp_dic = record;
            timing_dic = record2;
            for (int i = 0; i <= times; i++) {
                cout << do_action(improve_actions[i].first, record, record2, buf_flag) << endl;
            }
            write_genlib("contest.genlib", record, record2, 0);
            write_liberty("contest_liberty.lib", record);
            costMgr->cost_cal(1, buf_flag);
            temp_dic = record;
            timing_dic = record2;
        }
    }
    best_cost = cur_best_cost;
    if (buf_flag) {
        abccmd("read ./contest_liberty.lib");
    }
    else {
        abccmd("read ./contest.genlib");
    }
}

void lib_greedy_using_turtle(double& best_cost, const map<string, pair<string, float>>& gate_cost_dic, const map<string, vector<float>>& gate_timing_dic, bool buf_flag){
    double orig_cost = costMgr->cost_cal_use_turtle(0, buf_flag, 0);
    map<string, pair<string, float>> temp_dic = gate_cost_dic;
    map<string, vector<float>> timing_dic = gate_timing_dic;
    map<string, pair<string, float>> record = temp_dic;
    map<string, vector<float>> record2 = timing_dic;
    double cur_best_cost = orig_cost;
    
    while(true){
        int best_action;
        bool has_better = false;
        vector<pair<int, double>> improve_actions;
        double record_cost = cur_best_cost;
        for (int i = 0; i < 48; i++) {
            cout << "I: " << i << endl;
            double after_cost = do_action_use_turtle(i, temp_dic, timing_dic, buf_flag);
            cout << " cur_best: " << cur_best_cost << " this: " << after_cost << endl;
            // print_temp(temp_dic);
            // print_timing(timing_dic);
            // cout << endl;
            temp_dic = record;
            timing_dic = record2;
            if (after_cost < record_cost) {
                improve_actions.push_back(make_pair(i, after_cost));
            }
            if (after_cost < cur_best_cost) {
                cur_best_cost = after_cost;
                best_action = i;
                has_better = true;
            }
        }
        if (!has_better) break;
        else {
            // cout << do_action(best_action, record, record2, buf_flag) << endl;
            sort(improve_actions.begin(), improve_actions.end(), sortbysec);
            double temp_best_cost = record_cost;
            double times = 0;
            for (int i = 0; i < improve_actions.size() && i < 3; i++) {
                cout << improve_actions[i].first << endl;
                double cost = do_action_use_turtle(improve_actions[i].first, temp_dic, timing_dic, buf_flag);
                if (cost < temp_best_cost) {
                    temp_best_cost = cost;
                    times = i;
                }
                if (cost < cur_best_cost) {
                    cur_best_cost = cost;
                }
            }
            temp_dic = record;
            timing_dic = record2;
            for (int i = 0; i <= times; i++) {
                cout << do_action_use_turtle(improve_actions[i].first, record, record2, buf_flag) << endl;
            }
            write_genlib("./contest.genlib", record, record2, 0);
            abccmd("super -I 4 -L 2 ./contest.genlib");
            write_liberty("contest_liberty.lib", record);
            costMgr->cost_cal_use_turtle(1, buf_flag, 0);
            temp_dic = record;
            timing_dic = record2;
        }
    }
    best_cost = cur_best_cost;
    if (buf_flag) {
        abccmd("read ./contest_liberty.lib");
    }
    else {
        abccmd("read ./contest.genlib");
    }
}