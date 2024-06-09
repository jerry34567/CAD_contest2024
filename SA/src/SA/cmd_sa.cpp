#include "cmd_sa.h"
#include "util.h"

extern RandomNumGen  rnGen;

INST get_inst(){
    int result = rnGen(9); // number of action
    return  static_cast<INST>(result);
}

vector<string> cmd_simulated_annealing(const string& lib_file, const string& cost_exe, double& best_cost, const string& output){
    Abc_Ntk_t* pRecord;
    
    abccmd("strash");
    double init_cost = cost_cal(lib_file, cost_exe, "temp.v");

    double orig_cost = init_cost;
    vector<string> best_action;
    vector<string> cur_action;

    // parameters
    float T = 500;
    float T_low = 0.5;
    float r = 0.85;
    int iters = 40;
    while(T > T_low){
        INST action = get_inst();
        string act;
        pRecord = Abc_NtkDup(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t()));
        switch(action){
            case INST::REWRITE:
                act = "rewrite";
            break;
            case INST::REWRITEZ:
                act = "rewrite -z";
            break;
            case INST::REFACTOR:
                act = "refactor";
            break;
            case INST::REFACTORZ:
                act = "refactor -z";
            break;
            case INST::ORCHESTRATE:
                act = "orchestrate -N 3";
            case INST::IFRAIG:
                act = "ifraig";
            case INST::DC2:
                act = "dc2";
            case INST::BALANCE:
                act = "balance";
            break;
        }
        abccmd(act);
        double after_cost = cost_cal(lib_file, cost_exe, "temp.v");
        double diff = cost_diff(orig_cost, after_cost, init_cost);
        // std::cout << "cost prob:" << exp(-diff / T)  << "\n"; 
        double rand = (double(rnGen(INT32_MAX)) / double(INT32_MAX));
        // cout << "rand: " << rand << endl;
        if (diff < 0 || rand < exp(-diff / T)) {
            // std::cout << "Replace! cost (orig/after): " << orig_cost << " " << after_cost << "\n"; 
            orig_cost = after_cost;
            cur_action.push_back(act);
            if (orig_cost < best_cost) {
                cost_cal(lib_file, cost_exe, output);
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