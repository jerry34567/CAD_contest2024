#include "cmd_sa_new.h"
#include "util.h"
#include "cost.h"

// extern RandomNumGen  rnGen;
extern my_RandomNumGen Rand;
extern CostMgr* costMgr;

INST2 get_inst2(){
    int result = Rand() % 4; // number of action
    return  static_cast<INST2>(result);
}

ACTION2 get_action2(){
    int result = Rand() % 8; // number of action
    return  static_cast<ACTION2>(result);
}

vector<string> new_cmd_simulated_annealing(double& best_cost, bool buf_flag, vector<string>& init_actions){
    Abc_Ntk_t* pRecord;
    vector<string> temp_action = init_actions;
    vector<string> cur_action = init_actions;
    abccmd("strash");
    pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
    for (int i = 0; i < init_actions.size(); i++) {
        abccmd(init_actions[i]);
    }

    double init_cost = costMgr->cost_cal(0, buf_flag);
    cout << "init: " << init_cost << endl;

    Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
    double orig_cost = init_cost;
    vector<string> best_action;
    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        temp_action = cur_action;
        int length = cur_action.size();
        INST2 action = get_inst2();
        string act;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST2::add_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                int lRw = Rand() & 1; 
                int zRw = Rand() & 1; 
                int lRf = Rand() & 1; 
                int zRf = Rand() & 1; 
                int lRs = Rand() & 1; 
                int zRs = Rand() & 1; 
                int kRs = ((Rand() & 3) + 1) << 2;
                int nRs = Rand() & 3;
                int lOrch = Rand() & 1; 
                int zOrch = Rand() & 1; 
                int kOrch = ((Rand() & 3) + 1) << 2;
                int nOrch = Rand() & 3;
                int bDc2 = Rand() & 1; 
                int lDc2 = Rand() & 1; 
                int lB = Rand() & 1; 
                ACTION2 action2 = get_action2();
                switch(action2){
                    case ACTION2::REWRITE: {
                        act = "rewrite";
                        act += lRw ? " -l" : "";
                        act += zRw ? " -z" : "";
                        break;
                    }
                    case ACTION2::REFACTOR: {
                        act = "refactor";
                        act += lRf ? " -l" : "";
                        act += zRf ? " -z" : "";
                        break;
                    }
                    case ACTION2::RESUB: {
                        act = "resub";
                        act += lRs ? " -l" : "";
                        act += zRs ? " -z" : "";
                        act += " -K " + to_string(kRs);
                        act += " -N " + to_string(nRs);
                        break;
                    }
                    case ACTION2::ORCHESTRATE: {
                        act = "orchestrate";
                        act += lOrch ? " -l" : "";
                        act += zOrch ? " -z" : "";
                        act += " -K " + to_string(kOrch);
                        act += " -N " + to_string(nOrch);
                        break;
                    }
                    case ACTION2::IFRAIG: {
                        act = "ifraig";
                        break;
                    }
                    case ACTION2::DC2: {
                        act = "dc2";
                        act += bDc2 ? " -b" : "";
                        act += lDc2 ? " -l" : "";
                        break;
                    }
                    case ACTION2::BALANCE: {
                        act = "balance";
                        act += lB ? " -l" : "";
                        break;
                    }
                    case ACTION2::SHARE: {
                        act = "multi -m; sop; fx; st;";
                        break;
                    }
                }
                cur_action.insert(it, act);
                break;
            }
            case INST2::delete_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                cur_action.erase(it);
                break;
            }
            case INST2::swap_action: {
                string temp;
                int pos1 = Rand() % length;
                int pos2 = Rand() % length;
                temp = cur_action[pos1];
                cur_action[pos1] = cur_action[pos2];
                cur_action[pos2] = temp;
                break;
            }
            case INST2::move_action: {
                string temp;
                int pos1 = Rand() % length;
                temp = cur_action[pos1];
                vector<string>::iterator it = cur_action.begin() + pos1;
                cur_action.erase(it);
                it = cur_action.begin() + (Rand() % length);
                cur_action.insert(it, temp);
                break;
            }
        }
        for (int i = 0; i < cur_action.size(); i++) {
            abccmd(cur_action[i]);
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal(0, buf_flag);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < best_cost) {
                costMgr->cost_cal(1, buf_flag);
                best_cost = orig_cost;
                best_action = cur_action;
                vector<string>::iterator it = best_action.begin();
                best_action.insert(it, "strash");
            }

        }
        else {
            cur_action = temp_action;
        }
        Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}


vector<string> new_cmd_simulated_annealing_using_map(double& best_cost, bool buf_flag, vector<string>& init_actions){
    Abc_Ntk_t* pRecord;
    vector<string> temp_action = init_actions;
    vector<string> cur_action = init_actions;
    abccmd("strash");
    pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
    for (int i = 0; i < init_actions.size(); i++) {
        abccmd(init_actions[i]);
    }

    double init_cost = costMgr->cost_cal_use_map(0, buf_flag);
    cout << "init: " << init_cost << endl;

    Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
    double orig_cost = init_cost;
    vector<string> best_action;
    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        temp_action = cur_action;
        int length = cur_action.size();
        INST2 action = get_inst2();
        string act;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST2::add_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                int lRw = Rand() & 1; 
                int zRw = Rand() & 1; 
                int lRf = Rand() & 1; 
                int zRf = Rand() & 1; 
                int lRs = Rand() & 1; 
                int zRs = Rand() & 1; 
                int kRs = ((Rand() & 3) + 1) << 2;
                int nRs = Rand() & 3;
                int lOrch = Rand() & 1; 
                int zOrch = Rand() & 1; 
                int kOrch = ((Rand() & 3) + 1) << 2;
                int nOrch = Rand() & 3;
                int bDc2 = Rand() & 1; 
                int lDc2 = Rand() & 1; 
                int lB = Rand() & 1; 
                ACTION2 action2 = get_action2();
                switch(action2){
                    case ACTION2::REWRITE: {
                        act = "rewrite";
                        act += lRw ? " -l" : "";
                        act += zRw ? " -z" : "";
                        break;
                    }
                    case ACTION2::REFACTOR: {
                        act = "refactor";
                        act += lRf ? " -l" : "";
                        act += zRf ? " -z" : "";
                        break;
                    }
                    case ACTION2::RESUB: {
                        act = "resub";
                        act += lRs ? " -l" : "";
                        act += zRs ? " -z" : "";
                        act += " -K " + to_string(kRs);
                        act += " -N " + to_string(nRs);
                        break;
                    }
                    case ACTION2::ORCHESTRATE: {
                        act = "orchestrate";
                        act += lOrch ? " -l" : "";
                        act += zOrch ? " -z" : "";
                        act += " -K " + to_string(kOrch);
                        act += " -N " + to_string(nOrch);
                        break;
                    }
                    case ACTION2::IFRAIG: {
                        act = "ifraig";
                        break;
                    }
                    case ACTION2::DC2: {
                        act = "dc2";
                        act += bDc2 ? " -b" : "";
                        act += lDc2 ? " -l" : "";
                        break;
                    }
                    case ACTION2::BALANCE: {
                        act = "balance";
                        act += lB ? " -l" : "";
                        break;
                    }
                    case ACTION2::SHARE: {
                        act = "multi -m; sop; fx; st;";
                        break;
                    }
                }
                cur_action.insert(it, act);
                break;
            }
            case INST2::delete_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                cur_action.erase(it);
                break;
            }
            case INST2::swap_action: {
                string temp;
                int pos1 = Rand() % length;
                int pos2 = Rand() % length;
                temp = cur_action[pos1];
                cur_action[pos1] = cur_action[pos2];
                cur_action[pos2] = temp;
                break;
            }
            case INST2::move_action: {
                string temp;
                int pos1 = Rand() % length;
                temp = cur_action[pos1];
                vector<string>::iterator it = cur_action.begin() + pos1;
                cur_action.erase(it);
                it = cur_action.begin() + (Rand() % length);
                cur_action.insert(it, temp);
                break;
            }
        }
        for (int i = 0; i < cur_action.size(); i++) {
            abccmd(cur_action[i]);
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_map(0, buf_flag);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < best_cost) {
                costMgr->cost_cal_use_map(1, buf_flag);
                best_cost = orig_cost;
                best_action = cur_action;
                vector<string>::iterator it = best_action.begin();
                best_action.insert(it, "strash");
            }

        }
        else {
            cur_action = temp_action;
        }
        Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}

vector<string> new_cmd_simulated_annealing_using_turtle(double& best_cost, bool buf_flag, vector<string>& init_actions){
    Abc_Ntk_t* pRecord;
    vector<string> temp_action = init_actions;
    vector<string> cur_action = init_actions;
    abccmd("strash");
    pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
    for (int i = 0; i < init_actions.size(); i++) {
        abccmd(init_actions[i]);
    }

    double init_cost = costMgr->cost_cal_use_turtle(0, buf_flag, 0);
    cout << "init: " << init_cost << endl;

    Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
    double orig_cost = init_cost;
    vector<string> best_action;
    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        temp_action = cur_action;
        int length = cur_action.size();
        INST2 action = get_inst2();
        string act;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST2::add_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                int lRw = Rand() & 1; 
                int zRw = Rand() & 1; 
                int lRf = Rand() & 1; 
                int zRf = Rand() & 1; 
                int lRs = Rand() & 1; 
                int zRs = Rand() & 1; 
                int kRs = ((Rand() & 3) + 1) << 2;
                int nRs = Rand() & 3;
                int lOrch = Rand() & 1; 
                int zOrch = Rand() & 1; 
                int kOrch = ((Rand() & 3) + 1) << 2;
                int nOrch = Rand() & 3;
                int bDc2 = Rand() & 1; 
                int lDc2 = Rand() & 1; 
                int lB = Rand() & 1; 
                ACTION2 action2 = get_action2();
                switch(action2){
                    case ACTION2::REWRITE: {
                        act = "rewrite";
                        act += lRw ? " -l" : "";
                        act += zRw ? " -z" : "";
                        break;
                    }
                    case ACTION2::REFACTOR: {
                        act = "refactor";
                        act += lRf ? " -l" : "";
                        act += zRf ? " -z" : "";
                        break;
                    }
                    case ACTION2::RESUB: {
                        act = "resub";
                        act += lRs ? " -l" : "";
                        act += zRs ? " -z" : "";
                        act += " -K " + to_string(kRs);
                        act += " -N " + to_string(nRs);
                        break;
                    }
                    case ACTION2::ORCHESTRATE: {
                        act = "orchestrate";
                        act += lOrch ? " -l" : "";
                        act += zOrch ? " -z" : "";
                        act += " -K " + to_string(kOrch);
                        act += " -N " + to_string(nOrch);
                        break;
                    }
                    case ACTION2::IFRAIG: {
                        act = "ifraig";
                        break;
                    }
                    case ACTION2::DC2: {
                        act = "dc2";
                        act += bDc2 ? " -b" : "";
                        act += lDc2 ? " -l" : "";
                        break;
                    }
                    case ACTION2::BALANCE: {
                        act = "balance";
                        act += lB ? " -l" : "";
                        break;
                    }
                    case ACTION2::SHARE: {
                        act = "multi -m; sop; fx; st;";
                        break;
                    }
                }
                cur_action.insert(it, act);
                break;
            }
            case INST2::delete_action: {
                vector<string>::iterator it = cur_action.begin() + (Rand() % length);
                cur_action.erase(it);
                break;
            }
            case INST2::swap_action: {
                string temp;
                int pos1 = Rand() % length;
                int pos2 = Rand() % length;
                temp = cur_action[pos1];
                cur_action[pos1] = cur_action[pos2];
                cur_action[pos2] = temp;
                break;
            }
            case INST2::move_action: {
                string temp;
                int pos1 = Rand() % length;
                temp = cur_action[pos1];
                vector<string>::iterator it = cur_action.begin() + pos1;
                cur_action.erase(it);
                it = cur_action.begin() + (Rand() % length);
                cur_action.insert(it, temp);
                break;
            }
        }
        for (int i = 0; i < cur_action.size(); i++) {
            abccmd(cur_action[i]);
        }
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_turtle(0, buf_flag, 0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            if (orig_cost < best_cost) {
                costMgr->cost_cal_use_turtle(1, buf_flag, 0);
                best_cost = orig_cost;
                best_action = cur_action;
                vector<string>::iterator it = best_action.begin();
                best_action.insert(it, "strash");
            }

        }
        else {
            cur_action = temp_action;
        }
        Abc_FrameReplaceCurrentNetwork(abcMgr->get_Abc_Frame_t(), pRecord);
        T = r*T;    
        if(T>200 && r<=0.95) r += 0.01;
        else if(r>=0.85) r -= 0.01;
    }
    return best_action;
}