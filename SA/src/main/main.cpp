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
#include "libMgr.h"
#include "abc_util.h"
#include "cmd_init_greedy.h"
#include "init_cmd_sa.h"



using namespace std;

extern AbcMgr* abcMgr;
extern CostMgr* costMgr;
extern MioMgr* mioMgr;
extern SclMgr* sclMgr;
extern my_RandomNumGen Rand;


void try_add_buf(bool high_effort, int mode, double& best_cost, double& effort_cost) {
    map<string, pair<string, double>> temp_best_dic;
    if (high_effort) {
        if (mode == 0) {
            temp_best_dic = costMgr->get_high_best_dic();
        }
        else {
            temp_best_dic = costMgr->get_turtle_high_best_dic();
        }
    }
    else {
        if (mode == 0) {
            temp_best_dic = costMgr->get_low_best_dic();
        }
        else {
            temp_best_dic = costMgr->get_turtle_low_best_dic();
        }
    }
    write_liberty("temp_liberty.lib", temp_best_dic);
    abccmd("read temp_liberty.lib");
    string read_output;
    if (high_effort) {
        if (mode == 0) {
            read_output = "read -m high_effort.v";
        }
        else {
            read_output = "read -m turtle_high_effort.v";
        }
    }
    else {
        if (mode == 0) {
            read_output = "read -m low_effort.v";
        }
        else {
            read_output = "read -m turtle_low_effort.v";
        }
    }
    for(int buf_num = 2; buf_num <= 10; buf_num++) {
        abccmd(read_output);
        abccmd("unbuffer");
        string buffer = "buffer -N " + to_string(buf_num);
        abccmd(buffer);
        abccmd("write_verilog temp.v");
        string argument_buf = "-library " + costMgr->get_lib() + " -netlist " + "temp.v" + " -output temp.out";
        string score_out_buf = exec(costMgr->get_cost_exe(), argument_buf);
        float score_buf = extractCost(score_out_buf);
        if (best_cost > score_buf) {
            best_cost = score_buf;
            costMgr->change_name();
            if (costMgr->get_add_buf())
                abccmd("write_lib best_liberty.lib");
            else 
                abccmd("write_genlib best.genlib");
            // costMgr->set_best_dic(gate_cost_dic);
            // cout << "addbuf " << buf_num << ": " << score_buf << endl;
        }
    }
}

void try_phase_map(bool high_effort, int mode, double& best_cost, double& effort_cost) {
    if (high_effort) {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read high_best_liberty.lib");
            else 
                abccmd("read high_best.genlib");
            abccmd("read -m high_effort.v");
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_high_best_liberty.lib");
            else 
                abccmd("read turtle_high_best.genlib");
            abccmd("read -m turtle_high_effort.v");
        }
    }
    else {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read low_best_liberty.lib");
            else 
                abccmd("read low_best.genlib");
            abccmd("read -m low_effort.v");
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_low_best_liberty.lib");
            else 
                abccmd("read turtle_low_best.genlib");
            abccmd("read -m turtle_low_effort.v");
        }
    }
    
    abccmd("phase_map -v");
    abccmd("write_verilog temp.v");
    string phase_map_argument = "-library " + costMgr->get_lib() + " -netlist " + "temp.v" + " -output temp.out";
    string phase_map_score_out = exec(costMgr->get_cost_exe(), phase_map_argument);
    float phase_map_score = extractCost(phase_map_score_out);
    if (effort_cost > phase_map_score) {
        effort_cost = phase_map_score;
        if (mode == 0) {
            if (high_effort) {
                costMgr->change_high_effort_name();
            }
            else {
                costMgr->change_low_effort_name();
            }
        }
        else {
            if (high_effort) {
                costMgr->change_turtle_high_effort_name();
            }
            else {
                costMgr->change_turtle_low_effort_name();
            }
        }
    }
    if (best_cost > phase_map_score) {
        best_cost = phase_map_score;
        if (costMgr->get_add_buf())
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        // costMgr->set_best_dic(gate_cost_dic);
        costMgr->change_name();
        // cout << "phase_map: " << phase_map_score << endl;
    }
}

void try_add_pi(bool high_effort, int mode, double& best_cost, double& effort_cost) {
    if (high_effort) {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read high_best_liberty.lib");
            else 
                abccmd("read high_best.genlib");
            abccmd("read -m high_effort.v");
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_high_best_liberty.lib");
            else 
                abccmd("read turtle_high_best.genlib");
            abccmd("read -m turtle_high_effort.v");
        }
    }
    else {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read low_best_liberty.lib");
            else 
                abccmd("read low_best.genlib");
            abccmd("read -m low_effort.v");
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_low_best_liberty.lib");
            else 
                abccmd("read turtle_low_best.genlib");
            abccmd("read -m turtle_low_effort.v");
        }
    }

    add_const_on_pi();
    abccmd("write_verilog temp.v");
    string argument_add_const_on_pi = "-library " + costMgr->get_lib() + " -netlist " + "temp.v" + " -output temp.out";
    string score_out_pi = exec(costMgr->get_cost_exe(), argument_add_const_on_pi);
    float score_pi = extractCost(score_out_pi);
    if (effort_cost > score_pi) {
        effort_cost = score_pi;
        if (mode == 0) {
            if (high_effort) {
                costMgr->change_high_effort_name();
            }
            else {
                costMgr->change_low_effort_name();
            }
        }
        else {
            if (high_effort) {
                costMgr->change_turtle_high_effort_name();
            }
            else {
                costMgr->change_turtle_low_effort_name();
            }
        }
    }
    if (best_cost > score_pi) {
        best_cost = score_pi;
        if (costMgr->get_add_buf())
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        // costMgr->set_best_dic(gate_cost_dic);
        costMgr->change_name();
        // cout << "add const on pi: " << score_pi << endl;
    }
}

void try_replace_not_function(int& not_mode, bool high_effort, int mode, double& best_cost, bool use_nand, double& effort_cost) {
    if (high_effort) {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read high_best_liberty.lib");
            else 
                abccmd("read high_best.genlib");
            abccmd("read -m high_effort.v");
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_high_best_liberty.lib");
            else 
                abccmd("read turtle_high_best.genlib");
            abccmd("read -m turtle_high_effort.v");
        }
    }
    else {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read low_best_liberty.lib");
            else 
                abccmd("read low_best.genlib");
            abccmd("read -m low_effort.v");
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_low_best_liberty.lib");
            else 
                abccmd("read turtle_low_best.genlib");
            abccmd("read -m turtle_low_effort.v");
        }
    }
    if (use_nand) {
        replace_not_with_nand();
    }
    else {
        replace_not_with_nor();
    }
    string write_verilog = "write_verilog ";
    string verilog_file = "temp.v";
    write_verilog += verilog_file;
    abccmd(write_verilog);
    string argument = "-library " + costMgr->get_lib() + " -netlist " + verilog_file + " -output temp.out";
    string score_out = exec(costMgr->get_cost_exe(), argument);
    float score = extractCost(score_out);
    if (effort_cost > score) {
        effort_cost = score;
        if (mode == 0) {
            if (high_effort) {
                costMgr->change_high_effort_name();
            }
            else {
                costMgr->change_low_effort_name();
            }
        }
        else {
            if (high_effort) {
                costMgr->change_turtle_high_effort_name();
            }
            else {
                costMgr->change_turtle_low_effort_name();
            }
        }
    }
    if (best_cost > score) {
        if (use_nand) {
            not_mode = 1;
            // cout << "replace not with nand: " << score << endl;
        }
        else {
            not_mode = 2;
            // cout << "replace not with nor: " << score << endl;
        }
        best_cost = score;
        if (costMgr->get_add_buf())
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        // costMgr->set_best_dic(gate_cost_dic);
        costMgr->change_name();
    }
}

void try_to_replace_each_not(bool high_effort, int not_mode, int mode, double& best_cost, double& effort_cost) {
    int switch_mode;
    if (high_effort) {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read high_best_liberty.lib");
            else 
                abccmd("read high_best.genlib");
            abccmd("read -m high_effort.v");
            switch_mode = 0;
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_high_best_liberty.lib");
            else 
                abccmd("read turtle_high_best.genlib");
            abccmd("read -m turtle_high_effort.v");
            switch_mode = 1;
        }
    }
    else {
        if (mode == 0) {
            if (costMgr->get_add_buf())
                abccmd("read low_best_liberty.lib");
            else 
                abccmd("read low_best.genlib");
            abccmd("read -m low_effort.v");
            switch_mode = 2;
        }
        else {
            if (costMgr->get_add_buf())
                abccmd("read turtle_low_best_liberty.lib");
            else 
                abccmd("read turtle_low_best.genlib");
            abccmd("read -m turtle_low_effort.v");
            switch_mode = 3;
        }
    }

    replace_not_func(300, not_mode, best_cost, effort_cost, switch_mode);
}

bool extract_low_delay_gate(map<string, pair<string, double>>& fast_timing_dic, unordered_map<string, unordered_set<string>> all_gates,string lib_file, string cost_exe) {
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
    int level = Abc_NtkLevel(pNtk);
    // cout << "level: " << level << endl;
    int no_timing = 0;
    for (auto& gate_type : all_gates) {
        for (auto& gate : gate_type.second) {
            int temp_level = 1;
            double first_cost;
            double cost_distance;
            double last_cost;
            while (temp_level < level * 3) {
                ofstream write_file("temp_file.v");
                write_file << "module " << Abc_NtkName(pNtk) << " (a, b, o);\n";
                write_file << "input a, b;\n";
                write_file << "output o;\n";
                for (int i = 0; i < temp_level; i++) {
                    if (gate_type.first != "buf" && gate_type.first != "not") {
                        if (i == 0)
                            write_file << gate << "(a,b,y1);\n";
                        else if (i == temp_level-1) {
                            write_file << gate << "(y" << to_string(i) << ",t" << to_string(i) << ",o);\n";
                        }
                        else {
                            write_file << gate << "(y" << to_string(i) << ",t" << to_string(i) << ",y" << to_string(i+1) << ");\n";
                        }
                    }
                    else {
                        if (i == 0)
                            write_file << gate << "(a,y1);\n";
                        else if (i == temp_level-1) {
                            write_file << gate << "(y" << to_string(i) << ",o);\n";
                        }
                        else {
                            write_file << gate << "(y" << to_string(i) << ",y" << to_string(i+1) << ");\n";
                        }
                    }
                }
                write_file << "endmodule\n";
                write_file.close();
                string args = "-library " + lib_file + " -netlist temp_file.v -output temp.out";
                string output = exec(cost_exe, args);
                float cost = extractCost(output);
                // cout << gate << ": " << temp_level << "  cost: " << cost << endl;
                if (temp_level == 1) {
                    first_cost = cost;
                    last_cost = cost;
                }
                else if (temp_level == 2) {
                    cost_distance = cost - first_cost;
                    last_cost = cost;
                }
                else {
                    double after_cost_distance = cost - last_cost;
                    if (cost_distance * 3 < after_cost_distance) {
                        if (!fast_timing_dic.count(gate_type.first)) {
                            fast_timing_dic[gate_type.first] = make_pair(gate, (5.0 / (temp_level-1)));
                        }
                        else if (fast_timing_dic[gate_type.first].second > (5.0 / (temp_level-1))) {
                            fast_timing_dic[gate_type.first] = make_pair(gate, (5.0 / (temp_level-1)));
                        }
                        break;
                    }
                    cost_distance = after_cost_distance;
                    last_cost = cost;
                }
                temp_level++;
            }
            if (temp_level == level * 3) {
                no_timing++;
            }
        }
    }
    if (no_timing > 16) return false;
    else return true;
}

// bool extract_high_driven_gate(map<string, pair<string, double>>& fast_timing_dic, unordered_map<string, unordered_set<string>> all_gates,string lib_file, string cost_exe) {
//     Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
//     int no_timing = 0;
//     string test_gate = *all_gates["nor"].begin();
//     for (auto& gate_type : all_gates) {
//         for (auto& gate : gate_type.second) {
//             int temp_level = 2;
//             double first_cost;
//             double cost_distance;
//             double last_cost;
//             while (temp_level < 150) {
//                 ofstream write_file("temp_file.v");
//                 write_file << "module " << Abc_NtkName(pNtk) << " (a, b, o);\n";
//                 write_file << "input a, b;\n";
//                 write_file << "output o;\n";
//                 for (int i = 0; i < temp_level; i++) {
//                     if (gate_type.first != "buf" && gate_type.first != "not") {
//                         if (i == 0) {
//                             write_file << gate << "(a,b,y1);\n";
//                         }
//                         else if (i == temp_level-1) {
//                             write_file << test_gate << "(y1" << ",t" << to_string(i) << ",o);\n";
//                         }
//                         else {
//                             write_file << test_gate << "(y1" << ",t" << to_string(i) << ",y" << to_string(i+1) << ");\n";
//                         }
//                     }
//                     else {
//                         if (i == 0)
//                             write_file << gate << "(a,y1);\n";
//                         else if (i == temp_level-1) {
//                             write_file << test_gate << "(y1" << ",t" << to_string(i) << ",o);\n";
//                         }
//                         else {
//                             write_file << test_gate << "(y1" << ",t" << to_string(i) << ",y" << to_string(i+1) << ");\n";
//                         }
//                     }
//                 }
//                 write_file << "endmodule\n";
//                 write_file.close();
//                 string args = "-library " + lib_file + " -netlist temp_file.v -output temp.out";
//                 string output = exec(cost_exe, args);
//                 float cost = extractCost(output);
//                 // cout << gate << ": " << temp_level << "  cost: " << cost << endl;
//                 if (temp_level == 1) {
//                     first_cost = cost;
//                     last_cost = cost;
//                 }
//                 else if (temp_level == 2) {
//                     cost_distance = cost - first_cost;
//                     last_cost = cost;
//                 }
//                 else {
//                     double after_cost_distance = cost - last_cost;
//                     if (cost_distance * 3 < after_cost_distance) {
//                         if (!fast_timing_dic.count(gate_type.first)) {
//                             fast_timing_dic[gate_type.first] = make_pair(gate, ((temp_level-1) / 10.0));
//                         }
//                         else if (fast_timing_dic[gate_type.first].second < ((temp_level-1) / 10.0)) {
//                             fast_timing_dic[gate_type.first] = make_pair(gate, ((temp_level-1) / 10.0));
//                         }
//                         break;
//                     }
//                     cost_distance = after_cost_distance;
//                     last_cost = cost;
//                 }
//                 temp_level++;
//             }
//             if (temp_level == 150) {
//                 no_timing++;
//             }
//         }
//     }
//     if (no_timing > 16) return false;
//     else return true;
// }

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

// bool dc_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost, bool high_effort, bool use_same_lib) {
//     dc_gen(use_same_lib);

//     ifstream temp_file("syn_design.v");
//     if (!temp_file.is_open()) return 0;
//     else temp_file.close();

//     abccmd("read -m syn_design.v");

//     if (high_effort) {
//         abccmd("mfs3 -ae -I 5 -O 5 -R 4 -E 1000");
//         abccmd("mfs3 -aer -I 5 -O 5 -R 4 -E 1000");
//     }
//     else {
//         abccmd("mfs3 -ae -I 4 -O 2");
//         // abccmd("mfs3 -aer -I 4 -O 2");
//     }
    
    
//     abccmd("write_verilog dc_syn.v");

//     string args = "-library " + library + " -netlist " + "dc_syn.v" + " -output temp.out";
//     string temp = exec(cost_function, args);
//     double dc_cost = extractCost(temp);
//     if (dc_cost < best_cost) {
//         abccmd(write_verilog);
//         best_cost = dc_cost;
//     }
//     cout << "design_compiler: " << dc_cost << endl;
//     return 1;
// }

void script_exe(const string& write_verilog, const string& library, const string& output, const string& cost_function, double& best_cost, bool buf_flag) {
    abccmd("backup");
    abccmd("strash");
    abccmd("dc2 -b");
    abccmd("dc2 -b -l");
    abccmd("dc2");
    abccmd("dc2");
    abccmd("resub -l -K 4 -N 1");
    abccmd("backup");
    if (abccmd("multi -m -f; sop; fx; st")) {
        abccmd("restore");
    }
    abccmd("ifraig");
    abccmd("orchestrate -K 8 -N 2");
    abccmd("rewrite -z");
    abccmd("dc2 -b");
    abccmd("balance");
    abccmd("orchestrate -K 8 -N 3");
    abccmd("ifraig");
    abccmd("resub -K 16 -N 1");
    abccmd("orchestrate -K 4 -N 2");
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd("mfs3 -aer -I 4 -O 2");

    if (buf_flag) {
        abccmd("topo");
        abccmd("buffer -N 2");
    }

    abccmd("write_verilog temp.v");

    string args = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    string temp = exec(cost_function, args);
    double temp_cost = extractCost(temp);
    // cout << "script: " << temp_cost << endl;
    if (temp_cost < best_cost) {
        buf_flag = false;
        if (buf_flag)
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        costMgr->change_name();
        best_cost = temp_cost;
    }
    abccmd("restore");
}

int main(int argc, char** argv) {
    myUsage.reset();
    pid_t pid = getpid(); // 獲取當前進程的 PID
    std::cout << "Process ID (PID): " << pid << std::endl;
    auto start = std::chrono::steady_clock::now();

    generate_abcrc();
    abcMgr = new AbcMgr;
    abccmd("abcrc");

    string cost_function;
    string library;
    string netlist;
    string output;
    string temp;
    string args;
    bool buf_flag = false;
    bool add_const_on_pi_flag = false;
    bool not_penalty = false;
    bool consider_timing = false;
    bool use_deepsyn = false;
    int mode = 0;

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
    string read_design = "read ";
    read_design += netlist;
    string read_output = "read -m ";
    read_output += output;

    abccmd(read_design);
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
    string module_name = Abc_NtkName(pNtk);
    costMgr = new CostMgr(library, cost_function, output, "temp.v", module_name);
    // get cost of each gate
    double best_cost = MAXFLOAT;
    map<string, pair<string, double>> gate_cost_dic;
    map<string, vector<double>> gate_timing_dic;
    map<string, pair<string, double>> orig_gate_cost_dic;
    map<string, vector<double>> orig_gate_timing_dic;
    map<string, pair<string, double>> fast_gate_dic;


    unordered_map<string, unordered_set<string>> all_gate = parser(library, netlist, cost_function, gate_cost_dic, gate_timing_dic, not_penalty);
    int count_no_timing = 0;
    for (auto it : gate_timing_dic) {
        if (it.second[0] == 1)
            count_no_timing++;
    }
    if (count_no_timing > 4) {
        consider_timing = false;
        // cout << "no consider timing!!!" << endl;
    }
    else {
        consider_timing = true;
        costMgr->set_has_timing(consider_timing);
        // cout << "consider timing!!!" << endl;
    }
    // consider_timing = extract_low_delay_gate(fast_gate_dic, all_gate, library, cost_function);
    orig_gate_cost_dic = gate_cost_dic;
    orig_gate_timing_dic = gate_timing_dic;
    costMgr->set_low_best_dic(gate_cost_dic);
    costMgr->set_high_best_dic(gate_cost_dic);
    costMgr->set_turtle_low_best_dic(gate_cost_dic);
    costMgr->set_turtle_high_best_dic(gate_cost_dic);
    
    // consider_timing = extract_high_driven_gate(fast_gate_dic, all_gate, library, cost_function);
    
    costMgr->set_not_penalty(not_penalty);
    string read_lib = "read ./contest.genlib";
    string read_liberty = "read ./contest_liberty.lib";
    abccmd(read_lib);
    string write_verilog = "write_verilog ";
    write_verilog += output; 
    
    abccmd(read_design);
    write_module_inform();

    //// check add buf ////
    abccmd("st");
    abccmd("backup");
    abccmd("&get -n");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("write_verilog temp.v");
    string args1 = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
    string temp1 = exec(cost_function, args1);
    double init_cost = extractCost(temp1);
    // cout << "origin circuit: " << init_cost << endl;
    if (init_cost < best_cost) {
        costMgr->set_add_buf(false);
        costMgr->change_name();
        if (buf_flag)
            abccmd("write_lib best_liberty.lib");
        else 
            abccmd("write_genlib best.genlib");
        costMgr->set_best_dic(gate_cost_dic);
        best_cost = init_cost;
    }

    if (!not_penalty) {
        for (int i = 2; i < 9; i++) {
            abccmd(read_design);
            abccmd(read_liberty);
            sclMgr = new SclMgr;
            sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
            abccmd("st");
            abccmd("backup");
            abccmd("&get -n");
            abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
            abccmd("&put");
            abccmd("buffer -N " + to_string(i));
            abccmd("write_verilog temp.v");
            args1 = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
            temp1 = exec(cost_function, args1);
            double temp_cost = extractCost(temp1);
            // cout << "add buffer " << i << " cost: " << temp_cost << endl;
            if (temp_cost < (best_cost * 0.95)) {
                costMgr->set_add_buf(true);
                buf_flag = true;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
                best_cost = temp_cost;
            }
        }
    }
    ////////////////////////////////

    if (buf_flag) {
        abccmd("read ./contest_liberty.lib");
    }
    else {
        abccmd("read ./contest.genlib");
        abccmd("super -I 4 -L 2 ./contest.genlib");
    }

    ///// for abc script /////////
    script_exe(write_verilog, library, output, cost_function, best_cost, buf_flag);
    //////////////////////////////

    double record = MAXFLOAT;
    double record2 = MAXFLOAT;
    double low_effort_best_cost = MAXFLOAT;
    double turtle_low_effort_best_cost = MAXFLOAT;
    double high_effort_best_cost = MAXFLOAT;
    double turtle_high_effort_best_cost = MAXFLOAT;

    ///// for design compiler ///////
    // bool success = dc_exe(write_verilog, library, output, cost_function, best_cost, 0, 0);
    /////////////////////////////////
    // if (success) {
    //     abccmd("read -m dc_syn.v");
    //     double dc_then_abcmap = costMgr->cost_cal(0);
    //     if (dc_then_abcmap < low_effort_best_cost) {
    //         read_design = "read -m dc_syn.v";
    //         low_effort_best_cost = dc_then_abcmap;
    //         // cout << "dc_then_abc: " << low_effort_best_cost << endl;
    //     }
    //     if (low_effort_best_cost < best_cost) {
    //         if (buf_flag)
    //             abccmd("write_lib best_liberty.lib");
    //         else 
    //             abccmd("write_genlib best.genlib");
    //         costMgr->set_best_dic(gate_cost_dic);
    //         costMgr->change_name();
    //         best_cost = low_effort_best_cost;
    //     }
    // }

    abccmd(read_liberty);
    
    mioMgr = new MioMgr;
    sclMgr = new SclMgr;
    sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);

    //// try add const on pi when no timing constraint ///
    if (!consider_timing && !not_penalty) {
        // cout << "try add pi " << endl;
        abccmd(read_design);
        abccmd("st");
        abccmd("backup");
        abccmd("&get -n");
        abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
        abccmd("&put");
        add_const_on_pi();
        abccmd("write_verilog temp.v");
        string argument_add_const_on_pi = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
        string score_out_pi = exec(cost_function, argument_add_const_on_pi);
        float score_pi = extractCost(score_out_pi);
        if (best_cost > score_pi) {
            best_cost = score_pi;
            costMgr->set_add_const(true);
            if (buf_flag)
                abccmd("write_lib best_liberty.lib");
            else 
                abccmd("write_genlib best.genlib");
            costMgr->set_best_dic(gate_cost_dic);
            costMgr->change_name();
            // cout << "add const on pi: " << score_pi << endl;
        }
    }

    abccmd(read_design);
    
    
    vector<string> best_actions;
    vector<string> actions;
    vector<string> cur_action;

    //////// first use deepsyn to get usually good circuit ///////
    
    abccmd("strash");
    abccmd("backup");
    if (abccmd("multi -m -f; sop; fx;")) {
        abccmd("restore");
    }
    double before_deepsyn = costMgr->cost_cal(0);
    if (before_deepsyn < init_cost * 0.8) { // init_cost * 0.8
        // cout << "in deepsyn!!" << endl;
        abccmd("&get -n");
        abccmd("&deepsyn -T 300 -v");
        abccmd("&put");
        double after_deepsyn = costMgr->cost_cal(0);
        if (after_deepsyn < low_effort_best_cost) {
            costMgr->change_low_effort_name();
            costMgr->set_low_best_dic(gate_cost_dic);
            if (buf_flag)
                abccmd("write_lib low_best_liberty.lib");
            else 
                abccmd("write_genlib low_best.genlib");
            low_effort_best_cost = after_deepsyn;
            // cout << "deepsyn: " << low_effort_best_cost << endl;
            abccmd("write_blif deepsyn.blif");
            read_design = "read deepsyn.blif";
            use_deepsyn = true;
        }
        if (low_effort_best_cost < best_cost) {
            if (buf_flag)
                abccmd("write_lib best_liberty.lib");
            else 
                abccmd("write_genlib best.genlib");
            costMgr->set_best_dic(gate_cost_dic);
            costMgr->change_name();
            best_cost = low_effort_best_cost;
        }
    }

    abccmd(read_design);
    // if (consider_timing) {
    /////// choose a best map ////
    // choose_best_map(consider_timing);
    choose_best_map();


    /////// greedy choose five command as initial command /////
    // cur_action = init_cmd_greedy();
    cur_action = init_cmd_simulated_annealing(low_effort_best_cost, best_cost, buf_flag, gate_cost_dic);
    // }

    auto temp_current_time = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds_temp = temp_current_time - start;
    // cout << "time: " << elapsed_seconds_temp.count() << endl;
    std::chrono::duration<double> elapsed_seconds2 = temp_current_time - temp_current_time;

    best_actions = cur_action;

    int abc_no_change = 0;
    int turtle_no_change = 0;
    double this_run_record = best_cost;
    double turtle_this_run_record = best_cost;
    double check_best = 0;
    double has_turtle_flag = false;

    double one_round_time;
    while(true) {
        auto current_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = current_time - start;
        if (elapsed_seconds.count() > 10600) break;
        else if (elapsed_seconds.count() >= 7000) {
        // else if (!consider_timing) {
            if (mode == 0) {
                check_best = best_cost;
                has_turtle_flag = true;
                Rand.reset(914591);
                bool gar = false;
                turtle_no_change = 0;
                gate_cost_dic = orig_gate_cost_dic;
                gate_timing_dic = orig_gate_timing_dic;
                write_genlib("./contest.genlib", gate_cost_dic, gate_timing_dic, 0, gar);
                write_liberty("contest_liberty.lib", gate_cost_dic);
                if (use_deepsyn) {
                    read_design = "read deepsyn.blif";
                }
                else {
                    read_design = "read ";
                    read_design += netlist;
                }
                abccmd("super -I 4 -L 2 ./contest.genlib");
                if (buf_flag) {
                    abccmd("read contest_liberty.lib");
                    sclMgr = new SclMgr;
                    sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
                }
                else {
                    abccmd("read ./contest.genlib");
                    mioMgr = new MioMgr;
                } 
                abccmd(read_design);
                double turtle_init_cost = costMgr->cost_cal_use_turtle(0,0);
                // cout << "turtle init: " << turtle_init_cost << endl;
                if (turtle_init_cost < turtle_low_effort_best_cost) {
                    turtle_low_effort_best_cost = turtle_init_cost;
                    costMgr->change_turtle_low_effort_name();
                    costMgr->set_turtle_low_best_dic(gate_cost_dic);
                    if (buf_flag)
                        abccmd("write_lib turtle_low_best_liberty.lib");
                    else 
                        abccmd("write_genlib turtle_low_best.genlib");
                }
                if (turtle_low_effort_best_cost < best_cost) {
                    if (buf_flag)
                        abccmd("write_lib best_liberty.lib");
                    else 
                        abccmd("write_genlib best.genlib");
                    costMgr->set_best_dic(gate_cost_dic);
                    costMgr->change_name();
                    best_cost = turtle_low_effort_best_cost;
                }

                // cur_action = init_cmd_greedy_turtle();
                cur_action = init_cmd_simulated_annealing_using_turtle(turtle_low_effort_best_cost, best_cost, buf_flag, gate_cost_dic);
            }
            mode = 1;
            // cout << "switch mode" << endl;
        }

        if (this_run_record == low_effort_best_cost) {
            abc_no_change++;
        }
        else {
            abc_no_change = 0;
        }

        if (turtle_this_run_record == turtle_low_effort_best_cost) {
            turtle_no_change++;
        }
        else {
            turtle_no_change = 0;
        }

        if (mode == 0 && abc_no_change > 2) {
            // cout << "no progress, try to jump local minimum" << endl;
            abccmd(read_design);
            // ACTION2 action2 = get_action2();
            // string act = get_command(action2);
            // // cout << act << endl;
            // if (buf_flag) {
            //     abccmd("st");
            //     abccmd("backup");
            //     if (abccmd(act)) {
            //         abccmd("restore");
            //     }
            // }
            // else {
            //     abccmd("&get -n; &deepsyn -J 10; &put");
            // }
            // abccmd("&get -n; &deepsyn -T 60; &put");
            int seed = Rand() % 100;
            string action = "&get -n; &deepsyn -dv -J 10 -S " + to_string(seed) + "; &put";
            // string action = "&get -n; &deepsyn -dv -J 3 -S " + to_string(seed) + "; &put";
            // cout << action << endl;
            abccmd(action);
            // cout << "out of deepsyn" << endl;
            abccmd("write_blif temp_struct.blif");
            low_effort_best_cost = costMgr->cost_cal(0);
            high_effort_best_cost = low_effort_best_cost;
            cur_action = init_cmd_simulated_annealing(low_effort_best_cost, best_cost, buf_flag, gate_cost_dic);

            // cout << low_effort_best_cost << endl;
            read_design = "read temp_struct.blif";
        }

        if (mode == 1 && turtle_no_change > 2) {
            abccmd("super -I 4 -L 2 ./contest.genlib");
            abccmd(read_design);
            if (buf_flag) {
                abccmd("read contest_liberty.lib");
                sclMgr = new SclMgr;
                sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
            }
            else {
                abccmd("read ./contest.genlib");
                mioMgr = new MioMgr;
            } 
            // ACTION2 action2 = get_action2();
            // string act = get_command(action2);
            // // cout << act << endl;
            // cout << "no progress, try to jump local minimum" << endl;
            // if (buf_flag) {
            //     abccmd("st");
            //     abccmd("backup");
            //     if (abccmd(act)) {
            //         abccmd("restore");
            //     }
            // }
            // else {
            //     abccmd("&get -n; &deepsyn -J 10; &put");
            // }
            int seed = Rand() % 100;
            string action = "&get -n; &deepsyn -dv -J 10 -S " + to_string(seed) + "; &put";
            // string action = "&get -n; &deepsyn -dv -J 3 -S " + to_string(seed) + "; &put";
            // cout << action << endl;
            abccmd(action);
            // cout << "out of deepsyn" << endl;
            abccmd("write_blif temp_struct.blif");
            turtle_low_effort_best_cost = costMgr->cost_cal_use_turtle(0,0);
            turtle_high_effort_best_cost = turtle_low_effort_best_cost;
            // cout << turtle_low_effort_best_cost << endl;
            cur_action = init_cmd_simulated_annealing_using_turtle(turtle_low_effort_best_cost, best_cost, buf_flag, gate_cost_dic);
            read_design = "read temp_struct.blif";
        }

        record = low_effort_best_cost;
        record2 = turtle_low_effort_best_cost;
        this_run_record = record;
        turtle_this_run_record = record2;
        

        if (mode == 0) {
            abccmd(read_design);
            //// new cmd_SA  /////
            for (int i = 0; i < 2; i++) {
                // cout << "cmd_i: " << i << endl;
                actions = new_cmd_simulated_annealing(record, best_cost, buf_flag, cur_action, gate_cost_dic);

                if (low_effort_best_cost > record) {
                    best_actions = actions;
                    cur_action = actions;
                    vector<string>::iterator it = cur_action.begin();
                    cur_action.erase(it);
                    low_effort_best_cost = record;
                    // cout << low_effort_best_cost << endl;
                    for (int j = 0; j < best_actions.size(); j++) {
                        // cout << best_actions[j] << endl;
                    }
                }
                else {
                    record = low_effort_best_cost;
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
            for (int i = 0; i < 2; i++) {
                // cout << "i: " << i << endl;
                lib_simulated_annealing(record, best_cost, gate_cost_dic, gate_timing_dic, buf_flag);

                if (low_effort_best_cost > record) {
                    low_effort_best_cost = record;
                    // cout << low_effort_best_cost << endl;
                }
                else {
                    record = low_effort_best_cost;
                }
            }
            ////////////////////

            ////// lib_greedy /////
            // lib_greedy(record, best_cost, gate_cost_dic, gate_timing_dic, buf_flag);
            // if (low_effort_best_cost > record) {
            //     low_effort_best_cost = record;
            //     // cout << low_effort_best_cost << endl;
            // }
            // else {
            //     record = low_effort_best_cost;
            // }
            ///////////////////////

            ////// high effort //////
            abccmd(read_design);
            // abccmd("write_genlib debug.genlib");
            for (int i = 0; i < best_actions.size(); i++) {
                abccmd(best_actions[i]);
            }
            double high_effort_cost = costMgr->cost_cal_high_effort(0);
            // cout << "high_effort_cost: " << high_effort_cost << endl;
            if (high_effort_cost < high_effort_best_cost) {
                costMgr->change_high_effort_name();
                costMgr->set_high_best_dic(gate_cost_dic);
                high_effort_best_cost = high_effort_cost;
                if (buf_flag)
                    abccmd("write_lib high_best_liberty.lib");
                else 
                    abccmd("write_genlib high_best.genlib");
            }
            if (high_effort_best_cost < best_cost) {
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                // abccmd(write_verilog);
                costMgr->change_name();
                // Abc_replace_super(output);
                best_cost = high_effort_best_cost;
            }
            /////////////////////////
        }

        else if (mode == 1) {
            abccmd("super -I 4 -L 2 ./contest.genlib");
            if (buf_flag) {
                abccmd("read contest_liberty.lib");
                sclMgr = new SclMgr;
                sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
            }
            else {
                abccmd("read ./contest.genlib");
                mioMgr = new MioMgr;
            } 
                
            abccmd(read_design);
            //// new cmd_SA using turtle /////
            for (int i = 0; i < 2; i++) {
                // cout << "cmd_i: " << i << endl;
                // actions = cmd_simulated_annealing_using_turtle(record, buf_flag);
                actions = new_cmd_simulated_annealing_using_turtle(record2, best_cost, buf_flag, cur_action, gate_cost_dic);

                if (turtle_low_effort_best_cost > record2) {
                    best_actions = actions;
                    cur_action = actions;
                    vector<string>::iterator it = cur_action.begin();
                    cur_action.erase(it);
                    turtle_low_effort_best_cost = record2;
                    // cout << turtle_low_effort_best_cost << endl;
                    for (int j = 0; j < best_actions.size(); j++) {
                        // cout << best_actions[j] << endl;
                    }
                }
                else {
                    record2 = turtle_low_effort_best_cost;
                }
                abccmd(read_design);
            }
            //////////////////

            // if put the lib greedy or lib sa after cmd_sa, should execute the code below first ///////////
            for (int i = 0; i < best_actions.size(); i++) {
                abccmd(best_actions[i]);
            }
            ///////////////////////

            ///// lib_SA using turtle //////
            for (int i = 0; i < 2; i++) {
                // cout << "i: " << i << endl;
                lib_simulated_annealing_using_turtle(record2, best_cost, gate_cost_dic, gate_timing_dic, buf_flag);

                if (turtle_low_effort_best_cost > record2) {
                    turtle_low_effort_best_cost = record2;
                    // cout << turtle_low_effort_best_cost << endl;
                }
                else {
                    record2 = turtle_low_effort_best_cost;
                }
            }
            ////////////////////

            ////// lib_greedy /////
            // lib_greedy_using_turtle(record2, best_cost, gate_cost_dic, gate_timing_dic, buf_flag);
            // if (turtle_low_effort_best_cost > record2) {
            //     turtle_low_effort_best_cost = record2;
            //     // cout << turtle_low_effort_best_cost << endl;
            // }
            // else {
            //     record2 = turtle_low_effort_best_cost;
            // }
            ///////////////////////

            ////// high effort using turtle //////
            abccmd(read_design);

            for (int i = 0; i < best_actions.size(); i++) {
                abccmd(best_actions[i]);
            }

            double high_effort_cost = costMgr->cost_cal_use_turtle_high_effort(0, 0);
            
            // cout << "high_effort_cost: " << high_effort_cost << endl;
            if (high_effort_cost < turtle_high_effort_best_cost) {
                costMgr->change_turtle_high_effort_name();
                costMgr->set_turtle_high_best_dic(gate_cost_dic);
                turtle_high_effort_best_cost = high_effort_cost;
                if (buf_flag)
                    abccmd("write_lib turtle_high_best_liberty.lib");
                else 
                    abccmd("write_genlib turtle_high_best.genlib");
            }
            if (turtle_high_effort_best_cost < best_cost) {
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                // abccmd(write_verilog);
                costMgr->change_name();
                // Abc_replace_super(output);
                best_cost = turtle_high_effort_best_cost;
            }
            //////////////////
        }
        
        double record_low_effort = low_effort_best_cost;
        double record_high_effort = high_effort_best_cost;
        double record_turtle_low_effort = turtle_low_effort_best_cost;
        double record_turtle_high_effort = turtle_high_effort_best_cost;

        // cout << "low: " << low_effort_best_cost << endl;
        //// try to add const on pi /////
        // cout << "try add pi " << endl;
        if (mode == 0) {
            try_add_pi(0, 0, best_cost, record_low_effort);
            try_add_pi(1, 0, best_cost, record_high_effort);
        }
        else {
            try_add_pi(0, 1, best_cost, record_turtle_low_effort);
            try_add_pi(1, 1, best_cost, record_turtle_high_effort);
        }
        
        /////////////////////////////////

        int low_not_mode = 0;
        int high_not_mode = 0;
        /// try to replace not with nand /////
        // cout << "try to replace global not function" << endl;
        if (mode == 0) {
            try_replace_not_function(low_not_mode, 0, 0, best_cost, 0, record_low_effort);
            try_replace_not_function(high_not_mode, 1, 0, best_cost, 1, record_high_effort);
        }
        else {
            try_replace_not_function(low_not_mode, 0, 1, best_cost, 0, record_turtle_low_effort);
            try_replace_not_function(high_not_mode, 1, 1, best_cost, 1, record_turtle_high_effort);
        }
        ////////////////////////////////////

        /// try to change each gate (nor, nand, not) /////
        // cout << "try replace each not function" << endl;
        
        if (mode == 0) {
            if (record_low_effort < record_high_effort) {
                try_to_replace_each_not(300, low_not_mode, 0, best_cost, record_low_effort);
            }
            else {
                try_to_replace_each_not(300, high_not_mode, 0, best_cost, record_high_effort);
            }
        }
        else {
            if (record_turtle_low_effort < record_turtle_high_effort) {
                try_to_replace_each_not(300, low_not_mode, 1, best_cost, record_turtle_low_effort);
            }
            else {
                try_to_replace_each_not(300, high_not_mode, 1, best_cost, record_turtle_high_effort);
            }
        }

        // abccmd("dcec");
        /////////////////////////////////

        /// try to use phase map /////
        // cout << "try use phase map" << endl;
        if (mode == 0) {
            try_phase_map(0, 0, best_cost, record_low_effort);
            try_phase_map(1, 0, best_cost, record_high_effort);
        }
        else {
            try_phase_map(0, 1, best_cost, record_turtle_low_effort);
            try_phase_map(1, 1, best_cost, record_turtle_high_effort);
        }
        
        ////////////////////////////////////

        ///// try to add buffer /////
        // cout << "try buf" << endl;
        if (mode == 0) {
            try_add_buf(0, 0, best_cost, record_low_effort);
            try_add_buf(1, 0, best_cost, record_high_effort);
        }
        else {
            try_add_buf(0, 1, best_cost, record_turtle_low_effort);
            try_add_buf(1, 1, best_cost, record_turtle_high_effort);
        }

        /////// try to replace buffer with nodedup /////
        // int max_fanout_num = 9;
        // bool success_nodedup = true;
        // while (success_nodedup) {
        //     // cout << "try nodedup" << endl;
        //     max_fanout_num++;
        //     if (buf_flag)
        //         abccmd("read best_liberty.lib");
        //     else 
        //         abccmd("read best.genlib");
        //     abccmd(read_output);
        //     abccmd("unbuffer");
        //     string nodedup = "nodedup -N " + to_string(max_fanout_num);
        //     abccmd(nodedup);
        //     abccmd("write_verilog temp.v");
        //     string argument_nodedup = "-library " + library + " -netlist " + "temp.v" + " -output temp.out";
        //     string score_out_nodedup = exec(cost_function, argument_nodedup);
        //     float score_nodedup = extractCost(score_out_nodedup);
        //     if (best_cost > score_nodedup) {
        //         best_cost = score_nodedup;
        //         costMgr->change_name();
        //         if (buf_flag)
        //             abccmd("write_lib best_liberty.lib");
        //         else 
        //             abccmd("write_genlib best.genlib");
        //         costMgr->set_best_dic(gate_cost_dic);
        //         // cout << "nodedup " << max_fanout_num << ": " << score_nodedup << endl;
        //     }
        //     else {
        //         success_nodedup = false;
        //     }
        // }

        // if (consider_timing) {
        //     if (buf_flag) {
        //         abccmd("read best_liberty.lib");
        //         sclMgr = new SclMgr;
        //         mioMgr = new MioMgr;
        //     }
        //     else {
        //         abccmd("read best.genlib");
        //         mioMgr = new MioMgr;
        //     }
        //     abccmd(read_output);
        //     costMgr->gate_sizing_using_pre_compute_fast_gate(fast_gate_dic, best_cost);
        // }

        auto current_time_start = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration2 = current_time_start - start;
        // cout << "expect time: " << (elapsed_seconds2 + duration2).count() << endl;

        
        if (mode == 0 && (elapsed_seconds2 + duration2).count() < 7000) {
            abccmd(read_design);
            for (int i = 0; i < best_actions.size(); i++) {
                abccmd(best_actions[i]);
            }
            // cout << "low_effort_best: " << low_effort_best_cost << endl;

            //// gate sizing /////
            // cout << "try gate sizing" << endl;
            costMgr->gate_sizing(all_gate, gate_cost_dic, gate_timing_dic);
            double gate_sizing_cost = costMgr->cost_cal(0);
            // cout << "debug_gate_sizing: " << gate_sizing_cost << endl;
            if (best_cost > gate_sizing_cost) {
                best_cost = gate_sizing_cost;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
                // cout << "gate_sizing: " << gate_sizing_cost << endl;
            }

            ///// after every round, choose best map setting again ///
            // cout << "try choose best map" << endl;
            // choose_best_map(false);
            choose_best_map();
            double choose_map_cost = costMgr->cost_cal(0);
            // cout << "debug_choose map: " << choose_map_cost << endl; 
            if (best_cost > choose_map_cost) {
                best_cost = choose_map_cost;
                if (buf_flag)
                    abccmd("write_lib best_liberty.lib");
                else 
                    abccmd("write_genlib best.genlib");
                costMgr->set_best_dic(gate_cost_dic);
                costMgr->change_name();
                // cout << "choose map: " << choose_map_cost << endl;
            }
        }
        else {
            if (buf_flag) {
                abccmd("read contest_liberty.lib");
                sclMgr = new SclMgr;
                sclMgr->revise_scllib(gate_timing_dic, gate_cost_dic);
            }
            else {
                abccmd("read ./contest.genlib");
                mioMgr = new MioMgr;
            } 
        }
        auto current_time_end = std::chrono::steady_clock::now();
        elapsed_seconds2 = current_time_end - current_time_start;
        // cout << "this time run time: " << elapsed_seconds2.count() << endl;
        auto one_run_end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = one_run_end_time - current_time;
        one_round_time = duration.count();
        // cout << one_round_time << endl;
    }

    

    
    ////////////////////
    ///// dc after revising lib /////
    // dc_exe(write_verilog, library, output, cost_function, best_cost, 1, 1);
    // if (success) {
    //     abccmd("read -m syn_design.v");
    //     abccmd("mfs3 -ae -I 5 -O 5 -R 4 -E 1000");
    //     abccmd("mfs3 -aer -I 5 -O 5 -R 4 -E 1000");
    //     abccmd("write_verilog dc_syn.v");

    //     args = "-library " + library + " -netlist " + "dc_syn.v" + " -output temp.out";
    //     temp = exec(cost_function, args);
    //     double dc_cost = extractCost(temp);
    //     if (dc_cost < best_cost) {
    //         abccmd(write_verilog);
    //         if (buf_flag)
    //             abccmd("write_lib best_liberty.lib");
    //         else 
    //             abccmd("write_genlib best.genlib");
    //         costMgr->set_best_dic(gate_cost_dic);
    //         best_cost = dc_cost;
    //         cout << "dc_high_effort: " << best_cost << endl;
    //     }
    // }

    // write_liberty("contest_liberty.lib", gate_cost_dic);
    // dc_exe(write_verilog, library, output, cost_function, best_cost, 1, 0);
    //////////////////////////////////////////

    if (check_best == best_cost && has_turtle_flag) {
        cout << "abc win!!!" << endl;
    }
    else if (has_turtle_flag) {
        cout << "turtle win!!!" << endl;
    }
    else {
        cout << "no turtle!!!" << endl;
    }

    cout << "best_cost: " << best_cost << endl;
    for (int i = 0; i < best_actions.size(); i++) {
        cout << best_actions[i] << endl;
    }

    // verify correctness
    if (buf_flag)
        abccmd("read_lib best_liberty.lib");
    else 
        abccmd("read_genlib best.genlib");
    read_design = "read ";
    read_design += netlist;
    abccmd(read_design);
    abccmd("strash");
    abccmd("write_aiger temp.aig");
    read_design = "read -m ";
    read_design += output;
    abccmd(read_design);
    abccmd("strash");
    abccmd("write_aiger temp2.aig");
    abccmd("dcec temp.aig temp2.aig");
    abccmd("write_genlib final.genlib");
    myUsage.report(0,1);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;
    return 0;
}