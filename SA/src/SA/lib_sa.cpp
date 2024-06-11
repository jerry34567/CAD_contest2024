#include "lib_sa.h"
#include "util.h"


extern my_RandomNumGen  Rand;

ACTION get_action(bool buf_flag){
    int result;
    if (buf_flag)
        result = Rand() % 16;
    else
        result = Rand() & 63;
    return  static_cast<ACTION>(result % 48);
}

void lib_simulated_annealing(const string& lib_file, const string& cost_exe, double& best_cost, const string& output, map<string, pair<string, float>>& gate_cost_dic, map<string, vector<float>>& gate_timing_dic, bool buf_flag){
    double init_cost = cost_cal(lib_file, cost_exe, "temp.v", buf_flag);
    map<string, pair<string, float>> temp_dic = gate_cost_dic;
    map<string, vector<float>> timing_dic = gate_timing_dic;
    map<string, pair<string, float>> record;
    map<string, vector<float>> record2;

    double orig_cost = init_cost;

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    while(T > T_low){
        record = temp_dic;
        record2 = timing_dic;
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
                timing_dic["and"][0] += 0.1;
            break;
            case ACTION::AND_SUB_FT:
                timing_dic["and"][0] -= 0.1;
            break;
            case ACTION::NAND_ADD_FT:
                timing_dic["nand"][0] += 0.1;
            break;
            case ACTION::NAND_SUB_FT:
                timing_dic["nand"][0] -= 0.1;
            break;
            case ACTION::OR_ADD_FT:
                timing_dic["or"][0] += 0.1;
            break;
            case ACTION::OR_SUB_FT:
                timing_dic["or"][0] -= 0.1;
            break;
            case ACTION::NOR_ADD_FT:
                timing_dic["nor"][0] += 0.1;
            break;
            case ACTION::NOR_SUB_FT:
                timing_dic["nor"][0] -= 0.1;
            break;
            case ACTION::XOR_ADD_FT:
                timing_dic["xor"][0] += 0.1;
            break;
            case ACTION::XOR_SUB_FT:
                timing_dic["xor"][0] -= 0.1;
            break;
            case ACTION::XNOR_ADD_FT:
                timing_dic["xnor"][0] += 0.1;
            break;
            case ACTION::XNOR_SUB_FT:
                timing_dic["xnor"][0] -= 0.1;
            break;
            case ACTION::BUF_ADD_FT:
                timing_dic["buf"][0] += 0.1;
            break;
            case ACTION::BUF_SUB_FT:
                timing_dic["buf"][0] -= 0.1;
            break;
            case ACTION::NOT_ADD_FT:
                timing_dic["not"][0] += 0.1;
            break;
            case ACTION::NOT_SUB_FT:
                timing_dic["not"][0] -= 0.1;
            break;
            case ACTION::AND_ADD_ST:
                timing_dic["and"][1] += 0.1;
            break;
            case ACTION::AND_SUB_ST:
                timing_dic["and"][1] -= 0.1;
            break;
            case ACTION::NAND_ADD_ST:
                timing_dic["nand"][1] += 0.1;
            break;
            case ACTION::NAND_SUB_ST:
                timing_dic["nand"][1] -= 0.1;
            break;
            case ACTION::OR_ADD_ST:
                timing_dic["or"][1] += 0.1;
            break;
            case ACTION::OR_SUB_ST:
                timing_dic["or"][1] -= 0.1;
            break;
            case ACTION::NOR_ADD_ST:
                timing_dic["nor"][1] += 0.1;
            break;
            case ACTION::NOR_SUB_ST:
                timing_dic["nor"][1] -= 0.1;
            break;
            case ACTION::XOR_ADD_ST:
                timing_dic["xor"][1] += 0.1;
            break;
            case ACTION::XOR_SUB_ST:
                timing_dic["xor"][1] -= 0.1;
            break;
            case ACTION::XNOR_ADD_ST:
                timing_dic["xnor"][1] += 0.1;
            break;
            case ACTION::XNOR_SUB_ST:
                timing_dic["xnor"][1] -= 0.1;
            break;
            case ACTION::BUF_ADD_ST:
                timing_dic["buf"][1] += 0.1;
            break;
            case ACTION::BUF_SUB_ST:
                timing_dic["buf"][1] -= 0.1;
            break;
            case ACTION::NOT_ADD_ST:
                timing_dic["not"][1] += 0.1;
            break;
            case ACTION::NOT_SUB_ST:
                timing_dic["not"][1] -= 0.1;
            break;
        }
        write_genlib("temp.genlib", temp_dic, timing_dic);
        write_liberty("temp_liberty.lib", temp_dic);
        if (buf_flag)
            abccmd("read ./temp_liberty.lib");
        else
            abccmd("read ./temp.genlib");
        double after_cost = cost_cal(lib_file, cost_exe, "temp.v", buf_flag);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(rnGen(INT32_MAX)) / double(INT32_MAX));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << "  " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < best_cost) {
                cost_cal(lib_file, cost_exe, output, buf_flag);
                write_genlib("./contest.genlib", temp_dic, timing_dic);
                write_liberty("./contest_liberty.lib", temp_dic);
                gate_cost_dic = temp_dic;
                gate_timing_dic = timing_dic;
                // cout << "genlib replace!!!!!" << endl;
                best_cost = orig_cost;
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
    if (buf_flag)
        abccmd("read ./contest_liberty.lib");
    else
        abccmd("read ./contest.genlib");
}