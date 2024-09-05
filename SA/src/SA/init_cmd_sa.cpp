#include "init_cmd_sa.h"
#include "cost.h"

extern my_RandomNumGen Rand;
extern CostMgr* costMgr;

vector<string> init_cmd_simulated_annealing(double& low_effort_best_cost, double& best_cost, bool buf_flag, map<string, pair<string, double>>& gate_cost_dic) {
    Abc_Ntk_t* pRecord;

    abccmd("strash");
    double init_cost = costMgr->cost_cal(0);

    // cout << "init: " << init_cost << endl;

    double orig_cost = init_cost;
    vector<string> best_action;
    vector<string> cur_action;

    if (init_cost < low_effort_best_cost) {
        costMgr->change_low_effort_name();
        costMgr->set_low_best_dic(gate_cost_dic);
        if (buf_flag)
            abccmd("write_lib low_best_liberty.lib");
        else 
            abccmd("write_genlib low_best.genlib");
        low_effort_best_cost = init_cost;
    }
    if (low_effort_best_cost < best_cost) {
        best_cost = low_effort_best_cost;
        costMgr->change_name();
        if (buf_flag)
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        costMgr->set_best_dic(gate_cost_dic);
    }

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    while(T > T_low){
        ACTION2 action = get_action2();
        string act = get_command(action);
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        bool success = true;
        if (abccmd(act)) {
            // cout << "error!";
            success = false;
            break;
        }
        if (!success) {
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
            continue;
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal(0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX))) / 2;;
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            cur_action.push_back(act);
            if (orig_cost < low_effort_best_cost) {
                costMgr->change_low_effort_name();
                costMgr->set_low_best_dic(gate_cost_dic);
                if (buf_flag)
                    abccmd("write_lib low_best_liberty.lib");
                else 
                    abccmd("write_genlib low_best.genlib");
                low_effort_best_cost = orig_cost;
                best_action = cur_action;
            }
            if (low_effort_best_cost < best_cost) {
                best_cost = low_effort_best_cost;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
                // cout << low_effort_best_cost << endl;
            }
        }
        else {
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        }
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}


vector<string> init_cmd_simulated_annealing_using_turtle(double& low_effort_best_cost, double& best_cost, bool buf_flag, map<string, pair<string, double>>& gate_cost_dic) {
    Abc_Ntk_t* pRecord;

    abccmd("strash");
    double init_cost = costMgr->cost_cal_use_turtle(0,0);
    // cout << "init: " << init_cost << endl;

    double orig_cost = init_cost;
    vector<string> best_action;
    vector<string> cur_action;

    if (init_cost < low_effort_best_cost) {
        costMgr->change_turtle_low_effort_name();
        costMgr->set_turtle_low_best_dic(gate_cost_dic);
        if (buf_flag)
            abccmd("write_lib turtle_low_best_liberty.lib");
        else 
            abccmd("write_genlib turtle_low_best.genlib");
        low_effort_best_cost = init_cost;
    }
    if (low_effort_best_cost < best_cost) {
        best_cost = low_effort_best_cost;
        costMgr->change_name();
        if (buf_flag)
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        costMgr->set_best_dic(gate_cost_dic);
    }

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    while(T > T_low){
        ACTION2 action = get_action2();
        string act = get_command(action);
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        bool success = true;
        if (abccmd(act)) {
            // cout << "error!";
            success = false;
            break;
        }
        if (!success) {
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
            continue;
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_turtle(0,0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX))) / 2;;
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            cur_action.push_back(act);
            if (orig_cost < low_effort_best_cost) {
                costMgr->change_turtle_low_effort_name();
                costMgr->set_turtle_low_best_dic(gate_cost_dic);
                if (buf_flag)
                    abccmd("write_lib turtle_low_best_liberty.lib");
                else 
                    abccmd("write_genlib turtle_low_best.genlib");
                low_effort_best_cost = orig_cost;
                best_action = cur_action;
            }
            if (low_effort_best_cost < best_cost) {
                best_cost = low_effort_best_cost;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
                // cout << low_effort_best_cost << endl;
            }
        }
        else {
            Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        }
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}