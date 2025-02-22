#include "cmd_sa.h"
#include "util.h"
#include "cost.h"

// extern RandomNumGen  rnGen;
extern my_RandomNumGen Rand;
extern CostMgr* costMgr;

INST get_inst(){
    int result = Rand() % 8; // number of action
    return  static_cast<INST>(result);
}

vector<string> cmd_simulated_annealing(double& best_cost, bool buf_flag){
    Abc_Ntk_t* pRecord;

    abccmd("strash");
    double init_cost = costMgr->cost_cal(0);
    // cout << "init: " << init_cost << endl;

    double orig_cost = init_cost;
    vector<string> best_action;
    vector<string> cur_action;
    cur_action.push_back("strash");

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        INST action = get_inst();
        string act;
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
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST::REWRITE: {
                act = "rewrite";
                act += lRw ? " -l" : "";
                act += zRw ? " -z" : "";
                break;
            }
            case INST::REFACTOR: {
                act = "refactor";
                act += lRf ? " -l" : "";
                act += zRf ? " -z" : "";
                break;
            }
            case INST::RESUB: {
                act = "resub";
                act += lRs ? " -l" : "";
                act += zRs ? " -z" : "";
                act += " -K " + to_string(kRs);
                act += " -N " + to_string(nRs);
                break;
            }
            case INST::ORCHESTRATE: {
                act = "orchestrate";
                act += lOrch ? " -l" : "";
                act += zOrch ? " -z" : "";
                act += " -K " + to_string(kOrch);
                act += " -N " + to_string(nOrch);
                break;
            }
            case INST::IFRAIG: {
                act = "ifraig";
                break;
            }
            case INST::DC2: {
                act = "dc2";
                act += bDc2 ? " -b" : "";
                act += lDc2 ? " -l" : "";
                break;
            }
            case INST::BALANCE: {
                act = "balance";
                act += lB ? " -l" : "";
                break;
            }
            case INST::SHARE: {
                act = "multi -m; sop; fx; st;";
                break;
            }
        }
        abccmd(act);
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal(0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            cur_action.push_back(act);
            if (orig_cost < best_cost) {
                costMgr->cost_cal(1);
                best_cost = orig_cost;
                best_action = cur_action;
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


vector<string> cmd_simulated_annealing_using_map(double& best_cost, bool buf_flag){
    Abc_Ntk_t* pRecord;

    abccmd("strash");
    double init_cost = costMgr->cost_cal_use_map(0, buf_flag);
    // cout << "init: " << init_cost << endl;

    double orig_cost = init_cost;
    vector<string> best_action;
    vector<string> cur_action;
    cur_action.push_back("strash");

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        INST action = get_inst();
        string act;
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
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST::REWRITE: {
                act = "rewrite";
                act += lRw ? " -l" : "";
                act += zRw ? " -z" : "";
                break;
            }
            case INST::REFACTOR: {
                act = "refactor";
                act += lRf ? " -l" : "";
                act += zRf ? " -z" : "";
                break;
            }
            case INST::RESUB: {
                act = "resub";
                act += lRs ? " -l" : "";
                act += zRs ? " -z" : "";
                act += " -K " + to_string(kRs);
                act += " -N " + to_string(nRs);
                break;
            }
            case INST::ORCHESTRATE: {
                act = "orchestrate";
                act += lOrch ? " -l" : "";
                act += zOrch ? " -z" : "";
                act += " -K " + to_string(kOrch);
                act += " -N " + to_string(nOrch);
                break;
            }
            case INST::IFRAIG: {
                act = "ifraig";
                break;
            }
            case INST::DC2: {
                act = "dc2";
                act += bDc2 ? " -b" : "";
                act += lDc2 ? " -l" : "";
                break;
            }
            case INST::BALANCE: {
                act = "balance";
                act += lB ? " -l" : "";
                break;
            }
            case INST::SHARE: {
                act = "multi -m; sop; fx; st;";
                break;
            }
        }
        abccmd(act);
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_map(0, buf_flag);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            cur_action.push_back(act);
            if (orig_cost < best_cost) {
                costMgr->cost_cal_use_map(1, buf_flag);
                best_cost = orig_cost;
                best_action = cur_action;
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

vector<string>cmd_simulated_annealing_using_turtle(double& best_cost, bool buf_flag){
    Abc_Ntk_t* pRecord;

    abccmd("strash");
    double init_cost = costMgr->cost_cal_use_turtle(0, 0);
    // cout << "init: " << init_cost << endl;

    double orig_cost = init_cost;
    vector<string> best_action;
    vector<string> cur_action;
    cur_action.push_back("strash");

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        INST action = get_inst();
        string act;
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
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST::REWRITE: {
                act = "rewrite";
                act += lRw ? " -l" : "";
                act += zRw ? " -z" : "";
                break;
            }
            case INST::REFACTOR: {
                act = "refactor";
                act += lRf ? " -l" : "";
                act += zRf ? " -z" : "";
                break;
            }
            case INST::RESUB: {
                act = "resub";
                act += lRs ? " -l" : "";
                act += zRs ? " -z" : "";
                act += " -K " + to_string(kRs);
                act += " -N " + to_string(nRs);
                break;
            }
            case INST::ORCHESTRATE: {
                act = "orchestrate";
                act += lOrch ? " -l" : "";
                act += zOrch ? " -z" : "";
                act += " -K " + to_string(kOrch);
                act += " -N " + to_string(nOrch);
                break;
            }
            case INST::IFRAIG: {
                act = "ifraig";
                break;
            }
            case INST::DC2: {
                act = "dc2";
                act += bDc2 ? " -b" : "";
                act += lDc2 ? " -l" : "";
                break;
            }
            case INST::BALANCE: {
                act = "balance";
                act += lB ? " -l" : "";
                break;
            }
            case INST::SHARE: {
                act = "multi -m; sop; fx; st;";
                break;
            }
        }
        abccmd(act);
        // cout << "action: " << act << endl;
        double after_cost = costMgr->cost_cal_use_turtle(0, 0);
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(Rand() / double(INT32_MAX)));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            cur_action.push_back(act);
            if (orig_cost < best_cost) {
                costMgr->cost_cal_use_turtle(1, 0);
                best_cost = orig_cost;
                best_action = cur_action;
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