#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include "gvAbcMgr.h"
#include "base/abc/abc.h"
#include "gvAbcNtk.h"
#include <string>
#include <array>
#include <sstream>
#include <vector>
#include "parser.h"
#include "cmd_sa.h"
#include "lib_sa.h"
#include "cost.h"
#include "dc.h"
#include <chrono>
#include "lib_greedy.h"
#include "gradient.h"
#include "gvAbcSuper.h"
#include "super_greedy.h"
#include "cmd_sa_new.h"

using namespace std;

extern AbcMgr* abcMgr;
extern CostMgr* costMgr;

void write_module_inform() {
    ofstream file("./temp.txt");
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
    file << Abc_NtkName(pNtk) << endl;
    Abc_Obj_t* pPi;
    Abc_Obj_t* pPo;
    int i;
    Abc_NtkForEachPi(pNtk, pPi, i) {
        file << Abc_ObjName(pPi) << " ";
    }
    file << endl;
    Abc_NtkForEachPo(pNtk, pPo, i) {
        file << Abc_ObjName(pPo) << " ";
    }
    file.close();
}

bool dc_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost, bool high_effort, bool use_same_lib) {
    dc_gen(use_same_lib);

    ifstream temp_file("syn_design.v");
    if (!temp_file.is_open()) return 0;
    else temp_file.close();

    abccmd("read -m syn_design.v");

    if (high_effort) {
        abccmd("mfs3 -ae -I 5 -O 5 -R 4 -E 1000");
        abccmd("mfs3 -aer -I 5 -O 5 -R 4 -E 1000");
    }
    else {
        abccmd("mfs3 -ae -I 4 -O 2");
        // abccmd("mfs3 -aer -I 4 -O 2");
    }
    
    
    abccmd("write_verilog dc_syn.v");

    string args = "-library " + library + " -netlist " + "dc_syn.v" + " -output temp.out";
    string temp = exec(cost_function, args);
    double dc_cost = extractCost(temp);
    if (dc_cost < best_cost) {
        abccmd(write_verilog);
        best_cost = dc_cost;
    }
    cout << "design_compiler: " << dc_cost << endl;
    return 1;
}

void script_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost, bool is_buffer, bool& buf_flag) {
    if (is_buffer) {
        abccmd("read ./contest_liberty.lib");
    }
    else {
        abccmd("read ./contest.genlib");
    }
    abccmd("backup");
    abccmd("strash");
    abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l;");
    abccmd("refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l;");
    abccmd("rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l;");
    abccmd("b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    abccmd("orchestrate -N 3");
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd("mfs3 -aer -I 4 -O 2");

    if (is_buffer) {
        abccmd("topo");
        abccmd("buffer -N 2");
    }

    abccmd("write_verilog temp.v");

    string args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    string temp = exec(cost_function, args);
    double temp_cost = extractCost(temp);
    if (temp_cost < best_cost) {
        if (is_buffer) {
            buf_flag = true;
        }
        else {
            buf_flag = false;
        }
        abccmd(write_verilog);
        best_cost = temp_cost;
    }
    abccmd("restore");
    cout << "script: " << temp_cost << endl;
}

int main(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();

    abcMgr = new AbcMgr;
    string cost_function;
    string library;
    string netlist;
    string output;
    string temp;
    string args;
    bool buf_flag;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-library" && i + 1 < argc) {
            library = argv[++i]; 
        }
        if (arg == "-cost_function" && i + 1 < argc) {
            cost_function = argv[++i]; 
        }
        if (arg == "-netlist" && i + 1 < argc) {
            netlist = argv[++i]; 
        }
        if (arg == "-output" && i + 1 < argc) {
            output = argv[++i]; 
        }
    }
    string module_name = extractModuleName(netlist);
    costMgr = new CostMgr(library, cost_function, output, "temp.v", module_name);
    // get cost of each gate
    double best_cost = MAXFLOAT;
    map<string, pair<string, float>> gate_cost_dic;
    map<string, vector<float>> gate_timing_dic;
    parser(library, netlist, cost_function, gate_cost_dic, gate_timing_dic);

    string read_lib = "read ./contest.genlib";
    abccmd(read_lib);
    string write_verilog = "write_verilog ";
    write_verilog += output; 
    


    string read_design = "read ";
    read_design += netlist;

    abccmd(read_design);
    write_module_inform();
    ///// for abc script /////////
    // script_exe(write_verilog, library, output, cost_function, best_cost, 1, buf_flag);
    // script_exe(write_verilog, library, output, cost_function, best_cost, 0, buf_flag);
    //////////////////////////////
    
    ///// for design compiler ///////
    bool success = dc_exe(write_verilog, library, output, cost_function, best_cost, 0, 0);
    /////////////////////////////////
    if (success) {
        abccmd("read -m dc_syn.v");
        double dc_then_abcmap = costMgr->cost_cal(0, buf_flag);
        if (dc_then_abcmap <= best_cost) {
            read_design = "read -m dc_syn.v";
            best_cost = dc_then_abcmap;
            cout << best_cost << endl;
        }
    }

    

    abccmd(read_design);

    if (buf_flag) {
        abccmd("read ./contest_liberty.lib");
    }
    else {
        abccmd("read ./contest.genlib");
        abccmd("super -I 5 -L 2 ./contest.genlib");
    }

    double record = best_cost;
    vector<string> best_actions;
    best_actions.push_back("strash");
    best_actions.push_back("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l;");
    best_actions.push_back("refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l;");
    best_actions.push_back("rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l;");
    best_actions.push_back("resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    best_actions.push_back("orchestrate -N 3");
    vector<string> actions;

    ///// new cmd_SA need 1 ///
    vector<string> cur_action;
    cur_action.push_back("dc2 -b");
    cur_action.push_back("dc2 -b -l");
    cur_action.push_back("dc2");
    cur_action.push_back("dc2");
    cur_action.push_back("resub -l -K 4 -N 1");
    cur_action.push_back("multi -m; sop; fx; st");
    cur_action.push_back("ifraig");
    cur_action.push_back("orchestrate -K 8 -N 2");
    cur_action.push_back("rewrite -z");
    cur_action.push_back("dc2 -b");
    cur_action.push_back("balance");
    cur_action.push_back("orchestrate -K 8 -N 3");
    cur_action.push_back("ifraig");
    cur_action.push_back("resub -K 16 -N 1");
    cur_action.push_back("orchestrate -K 4 -N 2");
    //////////////////////////

    // //// cmd_SA with mockturtle /////
    // for (int i = 0; i < 16; i++) {
    //     cout << "i: " << i << endl;
    //     actions = cmd_simulated_annealing_using_turtle(best_cost, buf_flag);

    //     if (record > best_cost) {
    //         best_actions = actions;
    //         record = best_cost;
    //         cout << best_cost << endl;
    //     }
    //     else {
    //         best_cost = record;
    //     }
    //     abccmd(read_design);
    // }
    // //////////////////

    // abccmd("strash");
    // for (int i = 0; i < cur_action.size(); i++) {
    //     abccmd(cur_action[i]);
    // }

    // ///// lib_SA with mockturtle //////
    // for (int i = 0; i < 16; i++) {
    //     cout << "i: " << i << endl;
    //     lib_simulated_annealing_using_turtle(best_cost, gate_cost_dic, gate_timing_dic, buf_flag);

    //     if (record > best_cost) {
    //         record = best_cost;
    //         cout << best_cost << endl;
    //     }
    //     else {
    //         best_cost = record;
    //     }
    // }
    // ////////////////////

    // ////// lib_greedy with mockturtle /////
    // lib_greedy_using_turtle(best_cost, gate_cost_dic, gate_timing_dic, buf_flag);
    // if (record > best_cost) {
    //     record = best_cost;
    //     cout << best_cost << endl;
    // }
    // else {
    //     best_cost = record;
    // }
    // ////////////////////

    // abccmd(read_design);
    // //// new_cmd_SA with mockturtle /////
    // for (int i = 0; i < 16; i++) {
    //     cout << "i: " << i << endl;
    //     actions = new_cmd_simulated_annealing_using_turtle(best_cost, buf_flag, cur_action);

    //     if (record > best_cost) {
    //         best_actions = actions;
    //         cur_action = actions;
    //         vector<string>::iterator it = cur_action.begin();
    //         cur_action.erase(it);
    //         record = best_cost;
    //         cout << best_cost << endl;
    //     }
    //     else {
    //         best_cost = record;
    //     }
    //     abccmd(read_design);
    // }
    //////////////////


    // cur_action.push_back("resub -K 16 -N 1");
    // cur_action.push_back("resub -l -K 16 -N 1");
    // cur_action.push_back("dc2 -b");
    // cur_action.push_back("orchestrate -K 4 -N 2");
    // cur_action.push_back("rewrite");
    // cur_action.push_back("dc2 -b");
    // cur_action.push_back("dc2 -b");
    // cur_action.push_back("rewrite -z");
    // cur_action.push_back("orchestrate -K 8 -N 3");
    // cur_action.push_back("balance");
    // cur_action.push_back("dc2 -b -l");
    // cur_action.push_back("dc2");
    // cur_action.push_back("dc2");
    // cur_action.push_back("orchestrate -l -z -K 12 -N 0");
    // cur_action.push_back("dc2 -b");
    // cur_action.push_back("refactor -l -z");
    // cur_action.push_back("rewrite -l -z");
    // cur_action.push_back("balance -l");
    // cur_action.push_back("ifraig");
    // cur_action.push_back("refactor -l -z");
    // cur_action.push_back("resub -K 8 -N 3");
    // cur_action.push_back("dc2 -b");
    // cur_action.push_back("orchestrate -l -K 8 -N 2");
    // cur_action.push_back("multi -m; sop; fx; st;");
    // cur_action.push_back("refactor");
    // cur_action.push_back("balance -l");
    // cur_action.push_back("ifraig");
    // cur_action.push_back("orchestrate -z -K 16 -N 1");
    // cur_action.push_back("orchestrate -l -K 8 -N 1");
    // cur_action.push_back("resub -K 4 -N 2");
    // cur_action.push_back("balance");
    // cur_action.push_back("multi -m; sop; fx; st;");
    // cur_action.push_back("ifraig");

    //// new cmd_SA /////
    for (int i = 0; i < 8; i++) {
        cout << "i: " << i << endl;
        actions = new_cmd_simulated_annealing(best_cost, buf_flag, cur_action);

        if (record > best_cost) {
            best_actions = actions;
            cur_action = actions;
            vector<string>::iterator it = cur_action.begin();
            cur_action.erase(it);
            record = best_cost;
            cout << best_cost << endl;
        }
        else {
            best_cost = record;
        }
        abccmd(read_design);
    }
    //////////////////

    // if put the lib greedy or lib sa after cmd_sa, should execute the code below first ///////////
    for (int i = 0; i < best_actions.size(); i++) {
        abccmd(best_actions[i]);
    }
    ///////////////////////

    ///// lib_SA //////
    for (int i = 0; i < 16; i++) {
        cout << "i: " << i << endl;
        lib_simulated_annealing(best_cost, gate_cost_dic, gate_timing_dic, buf_flag);

        if (record > best_cost) {
            record = best_cost;
            cout << best_cost << endl;
        }
        else {
            best_cost = record;
        }
    }
    ////////////////////

    ////// lib_greedy /////
    lib_greedy(best_cost, gate_cost_dic, gate_timing_dic, buf_flag);
    if (record > best_cost) {
        record = best_cost;
        cout << best_cost << endl;
    }
    else {
        best_cost = record;
    }
    ////////////////////

    /////// lib gradient /////
    // abccmd("strash");
    // abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    // abccmd("orchestrate -N 3");
    // double learning_rate = 0.1; // 學習率
    // int iterations = 100;
    // gradientDescent(learning_rate, iterations, gate_cost_dic, gate_timing_dic, library, cost_function, output, best_cost);
    //////////////////////////

    //////// super greedy ////
    // for (int i = 0; i < best_actions.size(); i++) {
    //     abccmd(best_actions[i]);
    // }
    // abccmd("super -I 5 -L 2 ./contest.genlib");
    // abccmd("read_super ./contest.super");
    // Abc_Super_Precompute();
    // reset_super_area();

    // super_greedy(best_cost, buf_flag);
    // super_greedy(best_cost, buf_flag);
    // cout << "super: " << best_cost << endl;
    // if (record > best_cost) {
    //     record = best_cost;
    //     cout << best_cost << endl;
    // }
    // else {
    //     best_cost = record;
    // }
    //////////////////////////


    ////// test /////////////
    // abccmd("super -I 5 -L 2 ./contest.genlib");
    // abccmd("read_super ./contest.super");
    // Abc_Super_Precompute();
    // reset_super_area();
    // for (int i = 0; i < 10; i++) {
    //     cout << "i: " << i << endl;
    //     for (int k = 0; k < 3; k++) {
    //         actions = cmd_simulated_annealing_using_map(best_cost, buf_flag);

    //         if (record > best_cost) {
    //             for (int m = 0; m < actions.size(); m++) {
    //                 best_actions.push_back(actions[m]);
    //                 // cout << "action: " << actions[m] << endl;
    //             }
    //             record = best_cost;
    //             cout << best_cost << endl;
    //         }
    //         else {
    //             best_cost = record;
    //         }
    //         abccmd(read_design);
    //         for (int j = 0; j < best_actions.size(); j++) {
    //             abccmd(best_actions[j]);
    //             // cout << best_actions[j] << endl;
    //         }
    //     }

    //     super_greedy(best_cost, buf_flag);
    //     if (record > best_cost) {
    //         record = best_cost;
    //         cout << best_cost << endl;
    //     }
    //     else {
    //         best_cost = record;
    //     }
    // }




    ////// high effort //////
    abccmd(read_design);
    for (int i = 0; i < best_actions.size(); i++) {
        abccmd(best_actions[i]);
    }
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 5 -O 5 -R 4 -E 1000");
    abccmd("mfs3 -aer -I 5 -O 5 -R 4 -E 1000");

    if (buf_flag) {
        abccmd("topo");
        abccmd("buffer -N 2");
    }

    abccmd("write_verilog temp.v");
    // Abc_replace_super("tmep.v");
    args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    temp = exec(cost_function, args);
    double high_effort_cost = extractCost(temp);
    cout << "high_effort_cost: " << high_effort_cost << endl;
    if (high_effort_cost < best_cost) {
        abccmd(write_verilog);
        // Abc_replace_super(output);
        best_cost = high_effort_cost;
    }

    ///// dc after revising lib /////
    // dc_exe(write_verilog, library, output, cost_function, best_cost, 1, 1);
    // dc_exe(write_verilog, library, output, cost_function, best_cost, 1, 0);


    cout << "best_cost: " << best_cost << endl;
    for (int i = 0; i < best_actions.size(); i++) {
        cout << best_actions[i] << endl;
    }

    // verify correctness
    read_design = "read ";
    read_design += netlist;
    abccmd(read_design);
    abccmd("strash");
    abccmd("write_aiger temp.aig");
    read_design = "read -m ";
    read_design += output;
    abccmd("write_aiger temp2.aig");
    abccmd("cec temp.aig temp2.aig");
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
    return 0;
}




