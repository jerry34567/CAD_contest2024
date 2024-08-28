#include "lib_sa.h"
#include "util.h"


extern my_RandomNumGen  Rand;
extern CostMgr* costMgr;
extern MioMgr* mioMgr;
extern SclMgr* sclMgr;

ACTION get_action(bool buf_flag){
    int result;
    result = Rand() % 32;
    return  static_cast<ACTION>(result);
}

void lib_simulated_annealing(double& low_effort_best_cost, double& best_cost, map<string, pair<string, double>>& gate_cost_dic, map<string, vector<double>>& gate_timing_dic, bool buf_flag){
    double init_cost = costMgr->cost_cal(0);
    map<string, pair<string, double>> temp_dic = gate_cost_dic;
    map<string, vector<double>> timing_dic = gate_timing_dic;
    map<string, pair<string, double>> record;
    map<string, vector<double>> record2;
    double orig_cost = init_cost;
    cout << "orig: " << orig_cost << endl;

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    while(T > T_low){
        record = temp_dic;
        record2 = timing_dic;
        int round = 1 + (Rand() % 4);
        for (int j = 0; j < round; j++) {
            double number = (double(Rand() / double(INT32_MAX))) / 2;
            ACTION action = get_action(buf_flag);
            switch(action){
                case ACTION::AND_ADD:
                    temp_dic["and"].second *= 1.1;
                break;
                case ACTION::AND_SUB:
                    temp_dic["and"].second *= 0.9;
                break;
                case ACTION::NAND_ADD:
                    temp_dic["nand"].second *= 1.1;
                break;
                case ACTION::NAND_SUB:
                    temp_dic["nand"].second *= 0.9;
                break;
                case ACTION::OR_ADD:
                    temp_dic["or"].second *= 1.1;
                break;
                case ACTION::OR_SUB:
                    temp_dic["or"].second *= 0.9;
                break;
                case ACTION::NOR_ADD:
                    temp_dic["nor"].second *= 1.1;
                break;
                case ACTION::NOR_SUB:
                    temp_dic["nor"].second *= 0.9;
                break;
                case ACTION::XOR_ADD:
                    temp_dic["xor"].second *= 1.1;
                break;
                case ACTION::XOR_SUB:
                    temp_dic["xor"].second *= 0.9;
                break;
                case ACTION::XNOR_ADD:
                    temp_dic["xnor"].second *= 1.1;
                break;
                case ACTION::XNOR_SUB:
                    temp_dic["xnor"].second *= 0.9;
                break;
                case ACTION::BUF_ADD:
                    temp_dic["buf"].second *= 1.1;
                break;
                case ACTION::BUF_SUB:
                    temp_dic["buf"].second *= 0.9;
                break;
                case ACTION::NOT_ADD:
                    temp_dic["not"].second *= 1.1;
                break;
                case ACTION::NOT_SUB:
                    temp_dic["not"].second *= 0.9;
                break;
                case ACTION::AND_ADD_FT:
                    timing_dic["and"][0] += number;
                break;
                case ACTION::AND_SUB_FT:
                    timing_dic["and"][0] -= number;
                    if (timing_dic["and"][0] < 0.01) timing_dic["and"][0] = 0.01;
                break;
                case ACTION::NAND_ADD_FT:
                    timing_dic["nand"][0] += number;
                break;
                case ACTION::NAND_SUB_FT:
                    timing_dic["nand"][0] -= number;
                    if (timing_dic["nand"][0] < 0.01) timing_dic["nand"][0] = 0.01;
                break;
                case ACTION::OR_ADD_FT:
                    timing_dic["or"][0] += number;
                break;
                case ACTION::OR_SUB_FT:
                    timing_dic["or"][0] -= number;
                    if (timing_dic["or"][0] < 0.01) timing_dic["or"][0] = 0.01;
                break;
                case ACTION::NOR_ADD_FT:
                    timing_dic["nor"][0] += number;
                break;
                case ACTION::NOR_SUB_FT:
                    timing_dic["nor"][0] -= number;
                    if (timing_dic["nor"][0] < 0.01) timing_dic["nor"][0] = 0.01;
                break;
                case ACTION::XOR_ADD_FT:
                    timing_dic["xor"][0] += number;
                break;
                case ACTION::XOR_SUB_FT:
                    timing_dic["xor"][0] -= number;
                    if (timing_dic["xor"][0] < 0.01) timing_dic["xor"][0] = 0.01;
                break;
                case ACTION::XNOR_ADD_FT:
                    timing_dic["xnor"][0] += number;
                break;
                case ACTION::XNOR_SUB_FT:
                    timing_dic["xnor"][0] -= number;
                    if (timing_dic["xnor"][0] < 0.01) timing_dic["xnor"][0] = 0.01;
                break;
                case ACTION::BUF_ADD_FT:
                    timing_dic["buf"][0] += number;
                break;
                case ACTION::BUF_SUB_FT:
                    timing_dic["buf"][0] -= number;
                    if (timing_dic["buf"][0] < 0.01) timing_dic["buf"][0] = 0.01;
                break;
                case ACTION::NOT_ADD_FT:
                    timing_dic["not"][0] += number;
                break;
                case ACTION::NOT_SUB_FT:
                    timing_dic["not"][0] -= number;
                    if (timing_dic["not"][0] < 0.01) timing_dic["not"][0] = 0.01;
                break;
                // case ACTION::AND_ADD_FT:
                //     timing_dic["and"][0] += 0.1;
                // break;
                // case ACTION::AND_SUB_FT:
                //     timing_dic["and"][0] -= 0.1;
                // break;
                // case ACTION::NAND_ADD_FT:
                //     timing_dic["nand"][0] += 0.1;
                // break;
                // case ACTION::NAND_SUB_FT:
                //     timing_dic["nand"][0] -= 0.1;
                // break;
                // case ACTION::OR_ADD_FT:
                //     timing_dic["or"][0] += 0.1;
                // break;
                // case ACTION::OR_SUB_FT:
                //     timing_dic["or"][0] -= 0.1;
                // break;
                // case ACTION::NOR_ADD_FT:
                //     timing_dic["nor"][0] += 0.1;
                // break;
                // case ACTION::NOR_SUB_FT:
                //     timing_dic["nor"][0] -= 0.1;
                // break;
                // case ACTION::XOR_ADD_FT:
                //     timing_dic["xor"][0] += 0.1;
                // break;
                // case ACTION::XOR_SUB_FT:
                //     timing_dic["xor"][0] -= 0.1;
                // break;
                // case ACTION::XNOR_ADD_FT:
                //     timing_dic["xnor"][0] += 0.1;
                // break;
                // case ACTION::XNOR_SUB_FT:
                //     timing_dic["xnor"][0] -= 0.1;
                // break;
                // case ACTION::BUF_ADD_FT:
                //     timing_dic["buf"][0] += 0.1;
                // break;
                // case ACTION::BUF_SUB_FT:
                //     timing_dic["buf"][0] -= 0.1;
                // break;
                // case ACTION::NOT_ADD_FT:
                //     timing_dic["not"][0] += 0.1;
                // break;
                // case ACTION::NOT_SUB_FT:
                //     timing_dic["not"][0] -= 0.1;
                // break;
            }
        }
        // bool temp = false;
        // write_genlib("temp.genlib", temp_dic, timing_dic,0, temp);
        // write_liberty("temp_liberty.lib", temp_dic);
        // if (buf_flag)
        //     abccmd("read ./temp_liberty.lib");
        // else
        //     abccmd("read ./temp.genlib");
        if (buf_flag)
            sclMgr->revise_scllib(timing_dic, temp_dic);
        else
            mioMgr->revise_genlib(timing_dic, temp_dic);
        double after_cost = costMgr->cost_cal(0);
        
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        // cout << "orig: " << orig_cost << "  after: " << after_cost << endl;
        double rand = (double(rnGen(INT32_MAX)) / double(INT32_MAX));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << "  " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < low_effort_best_cost) {
                // costMgr->cost_cal(1, buf_flag);
                // write_genlib("./contest.genlib", temp_dic, timing_dic, 0);
                // write_liberty("./contest_liberty.lib", temp_dic);
                gate_cost_dic = temp_dic;
                gate_timing_dic = timing_dic;
                // cout << "genlib replace!!!!!" << endl;
                low_effort_best_cost = orig_cost;
            }
            if (low_effort_best_cost < best_cost) {
                costMgr->change_name();
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                best_cost = low_effort_best_cost;
            }
        }
        else {
            temp_dic = record;
            timing_dic = record2;
        }
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    // if (buf_flag)
    //     abccmd("read ./contest_liberty.lib");
    // else
    //     abccmd("read ./contest.genlib");
    if (buf_flag)
        sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
    else
        mioMgr->revise_genlib(gate_timing_dic, gate_cost_dic);
}


void lib_simulated_annealing_using_turtle(double& low_effort_best_cost, double& best_cost, map<string, pair<string, double>>& gate_cost_dic, map<string, vector<double>>& gate_timing_dic, bool buf_flag){
    double init_cost = costMgr->cost_cal_use_turtle(0, 0);
    map<string, pair<string, double>> temp_dic = gate_cost_dic;
    map<string, vector<double>> timing_dic = gate_timing_dic;
    map<string, pair<string, double>> record;
    map<string, vector<double>> record2;
    double orig_cost = init_cost;
    cout << "orig: " << orig_cost << endl;

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    while(T > T_low){
        record = temp_dic;
        record2 = timing_dic;
        double number = (double(Rand() / double(INT32_MAX))) / 2;
        ACTION action = get_action(buf_flag);
        switch(action){
            case ACTION::AND_ADD:
                temp_dic["and"].second *= 1.1;
            break;
            case ACTION::AND_SUB:
                temp_dic["and"].second *= 0.9;
            break;
            case ACTION::NAND_ADD:
                temp_dic["nand"].second *= 1.1;
            break;
            case ACTION::NAND_SUB:
                temp_dic["nand"].second *= 0.9;
            break;
            case ACTION::OR_ADD:
                temp_dic["or"].second *= 1.1;
            break;
            case ACTION::OR_SUB:
                temp_dic["or"].second *= 0.9;
            break;
            case ACTION::NOR_ADD:
                temp_dic["nor"].second *= 1.1;
            break;
            case ACTION::NOR_SUB:
                temp_dic["nor"].second *= 0.9;
            break;
            case ACTION::XOR_ADD:
                temp_dic["xor"].second *= 1.1;
            break;
            case ACTION::XOR_SUB:
                temp_dic["xor"].second *= 0.9;
            break;
            case ACTION::XNOR_ADD:
                temp_dic["xnor"].second *= 1.1;
            break;
            case ACTION::XNOR_SUB:
                temp_dic["xnor"].second *= 0.9;
            break;
            case ACTION::BUF_ADD:
                temp_dic["buf"].second *= 1.1;
            break;
            case ACTION::BUF_SUB:
                temp_dic["buf"].second *= 0.9;
            break;
            case ACTION::NOT_ADD:
                temp_dic["not"].second *= 1.1;
            break;
            case ACTION::NOT_SUB:
                temp_dic["not"].second *= 0.9;
            break;
            case ACTION::AND_ADD_FT:
                timing_dic["and"][0] += number;
            break;
            case ACTION::AND_SUB_FT:
                timing_dic["and"][0] -= number;
                if (timing_dic["and"][0] < 0) timing_dic["and"][0] = 0.01;
            break;
            case ACTION::NAND_ADD_FT:
                timing_dic["nand"][0] += number;
            break;
            case ACTION::NAND_SUB_FT:
                timing_dic["nand"][0] -= number;
                if (timing_dic["nand"][0] < 0) timing_dic["nand"][0] = 0.01;
            break;
            case ACTION::OR_ADD_FT:
                timing_dic["or"][0] += number;
            break;
            case ACTION::OR_SUB_FT:
                timing_dic["or"][0] -= number;
                if (timing_dic["or"][0] < 0) timing_dic["or"][0] = 0.01;
            break;
            case ACTION::NOR_ADD_FT:
                timing_dic["nor"][0] += number;
            break;
            case ACTION::NOR_SUB_FT:
                timing_dic["nor"][0] -= number;
                if (timing_dic["nor"][0] < 0) timing_dic["nor"][0] = 0.01;
            break;
            case ACTION::XOR_ADD_FT:
                timing_dic["xor"][0] += number;
            break;
            case ACTION::XOR_SUB_FT:
                timing_dic["xor"][0] -= number;
                if (timing_dic["xor"][0] < 0) timing_dic["xor"][0] = 0.01;
            break;
            case ACTION::XNOR_ADD_FT:
                timing_dic["xnor"][0] += number;
            break;
            case ACTION::XNOR_SUB_FT:
                timing_dic["xnor"][0] -= number;
                if (timing_dic["xnor"][0] < 0) timing_dic["xnor"][0] = 0.01;
            break;
            case ACTION::BUF_ADD_FT:
                timing_dic["buf"][0] += number;
            break;
            case ACTION::BUF_SUB_FT:
                timing_dic["buf"][0] -= number;
                if (timing_dic["buf"][0] < 0) timing_dic["buf"][0] = 0.01;
            break;
            case ACTION::NOT_ADD_FT:
                timing_dic["not"][0] += number;
            break;
            case ACTION::NOT_SUB_FT:
                timing_dic["not"][0] -= number;
                if (timing_dic["not"][0] < 0) timing_dic["not"][0] = 0.01;
            break;
            // case ACTION::AND_ADD_FT:
            //     timing_dic["and"][0] += 0.1;
            // break;
            // case ACTION::AND_SUB_FT:
            //     timing_dic["and"][0] -= 0.1;
            // break;
            // case ACTION::NAND_ADD_FT:
            //     timing_dic["nand"][0] += 0.1;
            // break;
            // case ACTION::NAND_SUB_FT:
            //     timing_dic["nand"][0] -= 0.1;
            // break;
            // case ACTION::OR_ADD_FT:
            //     timing_dic["or"][0] += 0.1;
            // break;
            // case ACTION::OR_SUB_FT:
            //     timing_dic["or"][0] -= 0.1;
            // break;
            // case ACTION::NOR_ADD_FT:
            //     timing_dic["nor"][0] += 0.1;
            // break;
            // case ACTION::NOR_SUB_FT:
            //     timing_dic["nor"][0] -= 0.1;
            // break;
            // case ACTION::XOR_ADD_FT:
            //     timing_dic["xor"][0] += 0.1;
            // break;
            // case ACTION::XOR_SUB_FT:
            //     timing_dic["xor"][0] -= 0.1;
            // break;
            // case ACTION::XNOR_ADD_FT:
            //     timing_dic["xnor"][0] += 0.1;
            // break;
            // case ACTION::XNOR_SUB_FT:
            //     timing_dic["xnor"][0] -= 0.1;
            // break;
            // case ACTION::BUF_ADD_FT:
            //     timing_dic["buf"][0] += 0.1;
            // break;
            // case ACTION::BUF_SUB_FT:
            //     timing_dic["buf"][0] -= 0.1;
            // break;
            // case ACTION::NOT_ADD_FT:
            //     timing_dic["not"][0] += 0.1;
            // break;
            // case ACTION::NOT_SUB_FT:
            //     timing_dic["not"][0] -= 0.1;
            // break;
        }
        bool temp;
        // if (buf_flag)
        //     sclMgr->revise_scllib(timing_dic, temp_dic);
        // else
        //     mioMgr->revise_genlib(timing_dic, temp_dic);
        // abccmd("write_genlib ./temp.genlib");
        write_genlib("./temp.genlib", temp_dic, timing_dic, 0, temp);
        // write_liberty("temp_liberty.lib", temp_dic);
        if (abccmd("super -I 4 -L 2 ./temp.genlib")) {
            cout << "super bug!!!" << endl;
            temp_dic = record;
            timing_dic = record2;
            continue;
        }
        // abccmd("super ./temp.genlib");
        double after_cost = costMgr->cost_cal_use_turtle(0, 1);
        // cout << after_cost << endl;
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(rnGen(INT32_MAX)) / double(INT32_MAX));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << "  " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < low_effort_best_cost) {
                write_genlib("./contest.genlib", temp_dic, timing_dic, 0, temp);
                // abccmd("write_genlib ./contest.genlib");
                abccmd("super -I 4 -L 2 ./contest.genlib");
                // abccmd("super ./contest.genlib");
                // costMgr->cost_cal_use_turtle(1, buf_flag, 0);

                // write_liberty("./contest_liberty.lib", temp_dic);
                gate_cost_dic = temp_dic;
                gate_timing_dic = timing_dic;
                // cout << "genlib replace!!!!!" << endl;
                low_effort_best_cost = orig_cost;
            }
            if (low_effort_best_cost < best_cost) {
                costMgr->change_name();
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                best_cost = low_effort_best_cost;
            }
        }
        else {
            temp_dic = record;
            timing_dic = record2;
        }
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    // if (buf_flag)
    //     abccmd("read ./contest_liberty.lib");
    // else
        // abccmd("read ./contest.genlib");
    if (buf_flag)
        sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
    else
        mioMgr->revise_genlib(gate_timing_dic, gate_cost_dic);
}