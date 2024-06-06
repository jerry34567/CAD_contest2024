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
#include "core.h"
#include "dc.h"

using namespace std;

extern AbcMgr* abcMgr;

int main(int argc, char** argv) {
    rnGen(2);

    abcMgr = new AbcMgr;
    string cost_function;
    string library;
    string netlist;
    string output;

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
    parser(library, netlist, cost_function);
    string read_lib = "read ./contest.genlib";
    abccmd(read_lib);
    string write_verilog = "write_verilog ";
    write_verilog += output; 

    ///// for design compiler ///////
    // dc_gen();

    // abccmd("read -m syn_design.v");
    // abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd(write_verilog);
    /////////////////////////////////

    string args = "-library " + library + " -netlist " + output + " -output temp.out";

    string temp = exec(cost_function, args);
    double best_cost = extractCost(temp);
    cout << "design_compiler: " << best_cost << endl;

    string read_design = "read ";
    read_design += netlist;
    abccmd(read_design);

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
    args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    temp = exec(cost_function, args);
    double temp_cost = extractCost(temp);
    if (temp_cost < best_cost) {
        abccmd(write_verilog);
        best_cost = temp_cost;
    }
    cout << "script: " << temp_cost << endl;
    double record = best_cost;
    abccmd("restore");
    vector<string> best_actions;

    vector<string> actions;
    for (int i = 0; i < 50; i++) {
        cout << "i: " << i << endl;
        actions = simulated_annealing(library, cost_function, best_cost, output);
        if (record > best_cost) {
            best_actions = actions;
            record = best_cost;
            cout << best_cost << endl;
        }
        else {
            best_cost = record;
        }
        abccmd(read_design);
    }
    

    cout << "best_cost: " << best_cost << endl;
    for (int i = 0; i < actions.size(); i++) {
        cout << best_actions[i] << endl;
    }
    // // std::cout << "node_num (orig/after): " << node_nums_orig << " " << node_nums_after << "\n"; 
    // cout << "best_area: " << Abc_NtkGetMappedArea(Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t())) << " error: " << float(error_record) *100 << "%" << endl;
    return 0;
}




