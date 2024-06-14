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
#include "gradient.h"

using namespace std;

extern AbcMgr* abcMgr;

void dc_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost, bool high_effort) {
    dc_gen();

    ifstream temp_file("syn_design.v");
    if (!temp_file.is_open()) return;
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
    abccmd("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
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
    rnGen(2);

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
    ///// for abc script /////////
    script_exe(write_verilog, library, output, cost_function, best_cost, 1, buf_flag);
    script_exe(write_verilog, library, output, cost_function, best_cost, 0, buf_flag);
    //////////////////////////////
    
    ///// for design compiler ///////
    dc_exe(write_verilog, library, output, cost_function, best_cost, 0);
    /////////////////////////////////
    abccmd("read -m dc_syn.v");
    double dc_then_abcmap = cost_cal(library, cost_function, "temp.v", buf_flag);
    if (dc_then_abcmap <= best_cost) {
        read_design = "read -m dc_syn.v";
        best_cost = dc_then_abcmap;
        cout << best_cost << endl;
    }

    abccmd(read_design);

    if (buf_flag) {
        abccmd("read ./contest_liberty.lib");
    }
    else {
        abccmd("read ./contest.genlib");
    }

    double record = best_cost;
    vector<string> best_actions;
    best_actions.push_back("strash");
    best_actions.push_back("b -l; resub -K 6 -l; rewrite -l; resub -K 6 -N 2 -l; refactor -l; resub -K 8 -l; b -l; resub -K 8 -N 2 -l; rewrite -l; resub -K 10 -l; rewrite -z -l; resub -K 10 -N 2 -l; b -l; resub -K 12 -l; refactor -z -l; resub -K 12 -N 2 -l; rewrite -z -l; b -l");
    best_actions.push_back("orchestrate -N 3");
    vector<string> actions;
    bool SA_flag = false;


    //// cmd_SA /////
    for (int i = 0; i < 10; i++) {
        cout << "i: " << i << endl;
        actions = cmd_simulated_annealing(library, cost_function, best_cost, output, buf_flag);

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
    for (int i = 0; i < best_actions.size(); i++) {
        abccmd(best_actions[i]);
    }
    ///////////////////////

    ///// lib_SA //////
    for (int i = 0; i < 10; i++) {
        cout << "i: " << i << endl;
        lib_simulated_annealing(library, cost_function, best_cost, output, gate_cost_dic, gate_timing_dic, buf_flag);

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
    lib_greedy(library, cost_function, best_cost, output, gate_cost_dic, gate_timing_dic, buf_flag);
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
    args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    temp = exec(cost_function, args);
    double high_effort_cost = extractCost(temp);
    cout << "high_effort_cost: " << high_effort_cost << endl;
    if (high_effort_cost < best_cost) {
        abccmd(write_verilog);
        best_cost = high_effort_cost;
    }

    ///// dc after revising lib /////
    dc_exe(write_verilog, library, output, cost_function, best_cost, 1);


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




