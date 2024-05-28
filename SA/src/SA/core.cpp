#include "core.h"
#include "util.h"


extern RandomNumGen  rnGen;

INST get_action(){
    int result = rnGen(9); // number of action
    return  static_cast<INST>(result);
}

double error_func(double rate, float er_threshold) {
    // return 2*error_rate;
    // smoothened piecewise linear function with slope from 1 -> 5
    return 2*(log(exp(-2*rate+2*er_threshold)+1)+2*rate-2*er_threshold)+rate;
}

double cost_func(int cur_cost, int orig_cost){
    return 0.5*pow((cur_cost - orig_cost) / orig_cost, 4);
}

double cost_diff(double orginal_cost, double after_cost, double init_cost) {
    //cost function z = 0.1x^2 + ITE(y<5, y , 3y-10), x: "total" area reduction rate, y: error rate
    double cost = cost_func(after_cost, init_cost) - cost_func(orginal_cost, init_cost);
    return 20000*cost;
}

double cost_cal(const string& lib_file, const string& cost_exe, const string& output_file) {
    abccmd("backup");
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 4 -O 2");
    string write_verilog = "write_verilog ";
    write_verilog += output_file;
    abccmd(write_verilog);
    abccmd("restore");
    string args = "-library " + lib_file + " -netlist " + output_file + " -output temp.out";
    string output = exec(cost_exe, args);
    return extractCost(output);
}

vector<string> simulated_annealing(const string& lib_file, const string& cost_exe, double& best_cost, const string& output){
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
        INST action = get_action();
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

        if (diff < 0 || (rnGen(10000) / 10000.0) < exp(-diff / T)) {
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