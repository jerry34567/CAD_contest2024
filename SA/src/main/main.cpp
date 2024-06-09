#include <iostream>
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
#include "dc.h"
#include <chrono>
#include "lib_greedy.h"

using namespace std;

extern AbcMgr* abcMgr;

void dc_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost) {
    dc_gen();

    ifstream temp_file("syn_design.v");
    if (!temp_file.is_open()) return;
    abccmd("read -m syn_design.v");
    abccmd("mfs3 -ae -I 4 -O 2");
    abccmd("mfs3 -ae -I 4 -O 2");
    abccmd("write_verilog temp.v");

    string args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    string temp = exec(cost_function, args);
    double dc_cost = extractCost(temp);
    if (dc_cost < best_cost) {
        abccmd(write_verilog);
        best_cost = dc_cost;
    }
    cout << "design_compiler: " << dc_cost << endl;
}

void script_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost) {
    abccmd("backup");
    abccmd("strash");
    abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    abccmd("orchestrate -N 3");
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 4 -O 2");
    abccmd("mfs3 -ae -I 4 -O 2");
    abccmd("write_verilog temp.v");

    string args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    string temp = exec(cost_function, args);
    double temp_cost = extractCost(temp);
    if (temp_cost < best_cost) {
        abccmd(write_verilog);
        best_cost = temp_cost;
    }
    abccmd("restore");
    cout << "script: " << temp_cost << endl;
}

int main(int argc, char** argv) {
    auto start = std::chrono::high_resolution_clock::now();
    rnGen(2);

    abcMgr = new AbcMgr;
    string cost_function;
    string library;
    string netlist;
    string output;
    string temp;
    string args;

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

    // get cost of each gate
    double best_cost = MAXFLOAT;
    map<string, pair<string, float>> gate_cost_dic;
    map<string, vector<float>> gate_timing_dic;
    parser(library, netlist, cost_function, gate_cost_dic, gate_timing_dic);

    string read_lib = "read ./contest.genlib";
    abccmd(read_lib);
    string write_verilog = "write_verilog ";
    write_verilog += output; 

    ///// for design compiler ///////
    dc_exe(write_verilog, library, output, cost_function, best_cost);
    /////////////////////////////////

    string read_design = "read ";
    read_design += netlist;
    abccmd(read_design);

    ///// for abc script /////////
    script_exe(write_verilog, library, output, cost_function, best_cost);
    //////////////////////////////

    
    double record = best_cost;
    vector<string> best_actions;
    vector<string> actions;
    bool SA_flag = false;

    ///// lib_SA //////
    // abccmd("strash");
    // abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    // abccmd("orchestrate -N 3");
    // for (int i = 0; i < 10; i++) {
    //     cout << "i: " << i << endl;
    //     lib_simulated_annealing(library, cost_function, best_cost, output, gate_cost_dic, gate_timing_dic);

    //     if (record > best_cost) {
    //         record = best_cost;
    //         cout << best_cost << endl;
    //     }
    //     else {
    //         best_cost = record;
    //     }
    // }
    ////////////////////

    ////// lib_greedy /////
    abccmd("strash");
    abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    abccmd("orchestrate -N 3");
    lib_greedy(library, cost_function, best_cost, output, gate_cost_dic, gate_timing_dic);
    if (record > best_cost) {
        record = best_cost;
        cout << best_cost << endl;
    }
    else {
        best_cost = record;
    }
    ////////////////////

    //// cmd_SA /////
    for (int i = 0; i < 10; i++) {
        cout << "i: " << i << endl;
        actions = cmd_simulated_annealing(library, cost_function, best_cost, output);

        if (record > best_cost) {
            SA_flag = true;
            best_actions = actions;
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
    // if (SA_flag) {
    //     abccmd("strash");
    //     for (int i = 0; i < best_actions.size(); i++) {
    //         abccmd(best_actions[i]);
    //     }
    // }
    // else {
    //     abccmd("strash");
    //     abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    //     abccmd("orchestrate -N 3");
    // }
    ///////////////////////


    ///// dc after revising genlib /////
    // dc_exe(write_verilog, library, output, cost_function, best_cost);


    cout << "best_cost: " << best_cost << endl;
    for (int i = 0; i < actions.size(); i++) {
        cout << best_actions[i] << endl;
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
    return 0;
}




