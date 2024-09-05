#include "cost.h"
#include "gvAbcSuper.h"
#include "abc_util.h"
#include "util.h"
#include "parser.h"
#include <filesystem>
#include <csignal>
#include <csetjmp> 

CostMgr* costMgr;
extern AbcSuperMgr* abcSuperMgr;
extern AbcMgr* abcMgr;
extern MioMgr* mioMgr;
extern SclMgr* sclMgr;
extern my_RandomNumGen Rand;

std::jmp_buf jump_buffer;

void signalHandler(int signum) {
    std::cerr << "Caught signal " << signum << " (abort signal)." << std::endl;
    // 如果希望程式不退出，則不調用 exit()
    std::longjmp(jump_buffer, 1);
}

string exec(const string& cmd, const string& args) {
    array<char, 128> buffer;
    string result;
    string fullCommand = cmd + " " + args; // Combine command and arguments
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(fullCommand.c_str(), "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

float extractCost(const string& output) {
    istringstream iss(output);
    string token;
    while (iss >> token) {
        if (token == "=") {
            if (iss >> token) {
                return stof(token);
            }
        }
    }
    return __FLT_MAX__;
    throw runtime_error("Cost not found in output");
}

double cost_func(double cur_cost, double orig_cost){
    if (orig_cost != 0)
        return (cur_cost - orig_cost) / orig_cost;
    return 0.1;
}
// double cost_func(double cur_cost, double orig_cost) {
//     if (orig_cost != 0) {
//         if ((cur_cost - orig_cost) < 0)
//             return -pow((cur_cost - orig_cost) / orig_cost, 2);
//         else 
//             return pow((cur_cost - orig_cost) / orig_cost, 2);
//     }
//     return 0.1; 
// }


double cost_diff(double orginal_cost, double after_cost, double init_cost) {
    double cost = cost_func(after_cost, init_cost) - cost_func(orginal_cost, init_cost);
    // cout << cost << endl;
    return 50000*cost;
}

void choose_best_map(bool has_timing) {
    bool record_pDch;
    bool record_fDch;
    bool record_pNf;
    bool record_aMfs3;
    int  record_FNf;
    int  record_ENf;
    int  record_QNf;
    int  record_RNf;
    double map_best_cost = MAXFLOAT;
    int temp = has_timing ? 1 : 2;

    for (int a = 0; a < 2; a++) {
        for (int b = 0; b < 2; b++) {
            for (int d = 0; d < 2; d++) {
                for (int f = 0; f < temp; f++) {
                    bool temp_record_pDch;
                    bool temp_record_fDch;
                    bool temp_record_xDch;
                    bool temp_record_pNf;
                    bool temp_record_aMfs3;
                    int  temp_record_FNf;
                    int  temp_record_ENf;
                    int  temp_record_QNf;
                    int  temp_record_RNf;

                    if (a == 0) {
                        temp_record_pDch = false;
                        costMgr->set_pDch(false);
                    }
                    else {
                        temp_record_pDch = true;
                        costMgr->set_pDch(true);
                    }
                    if (b == 0) {
                        temp_record_fDch = false;
                        costMgr->set_fDch(false);
                    }
                    else {
                        temp_record_fDch = true;
                        costMgr->set_fDch(true);
                    }
                    if (d == 0) {
                        temp_record_pNf = false;
                        costMgr->set_pNf(false);
                    }
                    else {
                        temp_record_pNf = true;
                        costMgr->set_pNf(true);
                    }
                    
                    if (f == 0) {
                        costMgr->set_FNf(3);
                        costMgr->set_ENf(10);
                        costMgr->set_QNf(10);
                        costMgr->set_RNf(0);
                        costMgr->set_aMfs3(false);
                        temp_record_FNf = 3;
                        temp_record_ENf = 10;
                        temp_record_QNf = 10;
                        temp_record_RNf = 0;
                        temp_record_aMfs3 = false;
                    }
                    else {
                        costMgr->set_FNf(10);
                        costMgr->set_ENf(100);
                        costMgr->set_QNf(100);
                        costMgr->set_RNf(1000);
                        costMgr->set_aMfs3(true);
                        temp_record_FNf = 10;
                        temp_record_ENf = 100;
                        temp_record_QNf = 100;
                        temp_record_RNf = 1000;
                        temp_record_aMfs3 = true;
                    }
                    double temp_map_cost = costMgr->cost_cal(0);
                    if (temp_map_cost < map_best_cost) {
                        map_best_cost = temp_map_cost;
                        record_pDch = temp_record_pDch;
                        record_fDch = temp_record_fDch;
                        record_pNf = temp_record_pNf;
                        record_aMfs3 = temp_record_aMfs3;
                        record_FNf = temp_record_FNf;
                        record_ENf = temp_record_ENf;
                        record_QNf = temp_record_QNf;
                        record_RNf = temp_record_RNf;
                    }
                }
            }
        }
    }
    costMgr->set_pDch(record_pDch);
    costMgr->set_fDch(record_fDch);
    costMgr->set_pNf(record_pNf);
    costMgr->set_FNf(record_FNf);
    costMgr->set_ENf(record_ENf);
    costMgr->set_QNf(record_QNf);
    costMgr->set_RNf(record_RNf);
    costMgr->set_aMfs3(record_aMfs3);
    // cout << "pDch: " << record_pDch << endl;
    // cout << "fDch: " << record_fDch << endl;
    // cout << "pNf: " << record_pNf << endl;
    // cout << "aMfs3: " << record_aMfs3 << endl;
    // cout << "FNf: " << record_FNf << endl;
    // cout << "ENf: " << record_ENf << endl;
    // cout << "QNf: " << record_QNf << endl;
    // cout << "RNf: " << record_RNf << endl;
}

void choose_best_map() {
    bool record_pDch;
    bool record_fDch;
    bool record_pNf;
    bool record_aMfs3;
    int  record_FNf;
    int  record_ENf;
    int  record_QNf;
    int  record_RNf;
    double map_best_cost = MAXFLOAT;

    for(int i = 0; i < 40; i++) {
        bool temp_record_pDch = costMgr->get_pDch();
        bool temp_record_fDch = costMgr->get_fDch();
        bool temp_record_pNf = costMgr->get_pNf();
        bool temp_record_aMfs3 = costMgr->get_aMfs3();
        int  temp_record_FNf = costMgr->get_FNf();
        int  temp_record_ENf = costMgr->get_ENf();
        int  temp_record_QNf = costMgr->get_QNf();
        int  temp_record_RNf = costMgr->get_RNf();

        if (i == 0) {
            temp_record_pDch = costMgr->get_pDch();
            temp_record_fDch = costMgr->get_fDch();
            temp_record_pNf = costMgr->get_pNf();
            temp_record_aMfs3 = costMgr->get_aMfs3();
            temp_record_FNf = costMgr->get_FNf();
            temp_record_ENf = costMgr->get_ENf();
            temp_record_QNf = costMgr->get_QNf();
            temp_record_RNf = costMgr->get_RNf();
        }
        else {
            temp_record_pDch = Rand() % 1;
            temp_record_fDch = Rand() % 1;
            temp_record_pNf = Rand() % 1;
            temp_record_aMfs3 = Rand() % 1;
            temp_record_FNf = (Rand() % 10) + 2;
            temp_record_ENf = Rand() % 101;
            temp_record_QNf = Rand() % 101;
            temp_record_RNf = Rand() % 1000;
        }

        costMgr->set_pDch(temp_record_pDch);
        costMgr->set_fDch(temp_record_fDch);
        costMgr->set_pNf(temp_record_pNf);
        
        costMgr->set_FNf(temp_record_FNf);
        costMgr->set_ENf(temp_record_ENf);
        costMgr->set_QNf(temp_record_QNf);
        costMgr->set_RNf(temp_record_RNf);
        costMgr->set_aMfs3(temp_record_aMfs3);
        double temp_map_cost = costMgr->cost_cal(0);
        if (temp_map_cost < map_best_cost) {
            map_best_cost = temp_map_cost;
            record_pDch = temp_record_pDch;
            record_fDch = temp_record_fDch;
            record_pNf = temp_record_pNf;
            record_aMfs3 = temp_record_aMfs3;
            record_FNf = temp_record_FNf;
            record_ENf = temp_record_ENf;
            record_QNf = temp_record_QNf;
            record_RNf = temp_record_RNf;
        }
    }
    costMgr->set_pDch(record_pDch);
    costMgr->set_fDch(record_fDch);
    costMgr->set_pNf(record_pNf);
    costMgr->set_FNf(record_FNf);
    costMgr->set_ENf(record_ENf);
    costMgr->set_QNf(record_QNf);
    costMgr->set_RNf(record_RNf);
    costMgr->set_aMfs3(record_aMfs3);
    // cout << "pDch: " << record_pDch << endl;
    // cout << "fDch: " << record_fDch << endl;
    // cout << "pNf: " << record_pNf << endl;
    // cout << "aMfs3: " << record_aMfs3 << endl;
    // cout << "FNf: " << record_FNf << endl;
    // cout << "ENf: " << record_ENf << endl;
    // cout << "QNf: " << record_QNf << endl;
    // cout << "RNf: " << record_RNf << endl;
}

// void turtle_choose_best_map() {
//     costMgr->set_super(true);
//     double temp_cost_use_super = costMgr->cost_cal_use_turtle(0,0);
//     costMgr->set_super(false);
//     double temp_cost_not_use_super = costMgr->cost_cal_use_turtle(0,0);
//     cout << "use_super: " << temp_cost_use_super << endl;
//     cout << "not_use_super: " << temp_cost_not_use_super << endl;
//     if (temp_cost_not_use_super > temp_cost_use_super) 
//         costMgr->set_super(true);
// }


void gate_sizing_util(string gate_type, unordered_map<string, unordered_set<string>>& all_gate, map<string, pair<string, double>>& temp_dic, map<string, vector<double>>& timing_dic, string lib_file, string cost_exe, bool add_buf, bool _not_penalty) {
    string temp_best;
    double temp_best_cost = MAXFLOAT;
    
    // cout << "gate size: " << gate_type << endl;
    double temp_timing = timing_dic[gate_type][0];
    // cout << temp_timing << endl;
    for (auto& it : all_gate[gate_type]) {
        // cout << gate_type << endl;
        bool gar = false;
        temp_dic[gate_type].first = it;
        if (add_buf) {
            write_liberty("contest_liberty.lib", temp_dic);
            abccmd("read contest_liberty.lib");
            sclMgr = new SclMgr;
            sclMgr->revise_scllib(timing_dic, temp_dic);
        }
        else {
            write_genlib("./contest.genlib", temp_dic, timing_dic, 0, _not_penalty);
            abccmd("read ./contest.genlib");
            mioMgr = new MioMgr;
        }
        
        double temp_cost = costMgr->cost_cal(0);
        // cout << it << " " << temp_cost << endl;
        if (temp_cost < temp_best_cost) {
            temp_best = it;
            temp_best_cost = temp_cost;
        }
    }
    temp_dic[gate_type].first = temp_best;
    timing_dic[gate_type] = {temp_timing, 0};
    if (add_buf) {
        write_liberty("contest_liberty.lib", temp_dic);
        write_genlib("./contest.genlib", temp_dic, timing_dic, 0, _not_penalty);
        abccmd("read contest_liberty.lib");
        sclMgr = new SclMgr;
        sclMgr->revise_scllib(timing_dic, temp_dic);
    }
    else {
        write_genlib("./contest.genlib", temp_dic, timing_dic, 0, _not_penalty);
        abccmd("read ./contest.genlib");
        mioMgr = new MioMgr;
    }
}

void CostMgr::gate_sizing(unordered_map<string, unordered_set<string>>& all_gate, map<string, pair<string, double>>& temp_dic, map<string, vector<double>>& timing_dic) {
    gate_sizing_util("and", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    gate_sizing_util("nand", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    gate_sizing_util("or", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    gate_sizing_util("nor", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    gate_sizing_util("not", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    if (add_buf)
        gate_sizing_util("buf", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    gate_sizing_util("xor", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
    gate_sizing_util("xnor", all_gate, temp_dic, timing_dic, _lib_file, _cost_exe, add_buf, _not_penalty);
}

void CostMgr::gate_sizing_using_pre_compute_fast_gate(map<string, pair<string, double>>& fast_gate_dic, double& best_cost) {
    for (auto& it : fast_gate_dic) {
        strcpy((mioMgr->find_gate_with_name(it.first)->pName), it.second.first.c_str());
        abccmd("write_verilog temp.v");
        string args = "-library " + _lib_file + " -netlist " + "temp.v" + " -output temp.out";
        string output = exec(_cost_exe, args);
        double cost = extractCost(output);
        // cout << "fast_gate cost: " << cost << endl;
        if (best_cost > cost) {
            best_cost = cost;
            costMgr->change_name();
            abccmd("write_lib best_liberty.lib");
            abccmd("write_genlib best.genlib");
        }
    }
}


double CostMgr::cost_cal(bool use_output) {
    abccmd("backup");
    abccmd("&get -n");
    
    // abccmd("&dch -f");
    string act = "&dch ";
    act += pDch ? "-p " : "";
    act += fDch ? "-f " : "";
    // cout << act << endl;
    abccmd(act);

    // abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    act = "&nf ";
    act += pNf ? "-p " : "";
    act += "-F " + to_string(FNf) + " ";
    act += "-E " + to_string(ENf) + " ";
    act += "-Q " + to_string(QNf) + " ";
    act += "-R " + to_string(RNf) + " ";
    act += "-C 32";
    // cout << act << endl;
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd(act);
    }
    else {
        abccmd("restore");
        return __FLT_MAX__;
    }

    abccmd("&put");
    act = "mfs3 -e ";
    act += aMfs3 ? "-a " : "";
    act += "-I 4 -O 2";
    // cout << act << endl;
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd(act);
    }
    else {
        abccmd("restore");
        return __FLT_MAX__;
    }
    // abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd("mfs3 -ae -I 4 -O 2");
    if (add_buf) {
        abccmd("topo");
        abccmd("buffer -N " + to_string(_buf_num));
    }
    // abccmd("fraig_sweep");
    if (_not_penalty) {
        replace_not_with_nand();
    }
    if (_add_const) {
        add_const_on_pi();
    }
    string write_verilog = "write_verilog ";
    string verilog_file;
    if (use_output) {
        verilog_file = _output_file;
    }
    else {
        verilog_file = _temp_file;
    }
    write_verilog += verilog_file;
    abccmd(write_verilog);
    abccmd("restore");
    // Abc_replace_super(verilog_file);
    string args = "-library " + _lib_file + " -netlist " + verilog_file + " -output temp.out";
    string output = exec(_cost_exe, args);
    return extractCost(output);
}


double CostMgr::cost_cal_use_map(bool use_output, bool buf_flag) {
    abccmd("backup");
    abccmd("dch -f");
    abccmd("map -asf");
    abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd("mfs3 -ae -I 4 -O 2");
    if (buf_flag) {
        abccmd("topo");
        abccmd("buffer -N 2");
    }
    if (_add_const) {
        add_const_on_pi();
    }
    string write_verilog = "write_verilog ";
    string verilog_file;
    if (use_output) {
        verilog_file = _output_file;
    }
    else {
        verilog_file = _temp_file;
    }
    write_verilog += verilog_file;
    abccmd(write_verilog);
    abccmd("restore");
    // Abc_replace_super(verilog_file);
    string args = "-library " + _lib_file + " -netlist " + verilog_file + " -output temp.out";
    string output = exec(_cost_exe, args);
    return extractCost(output);
}

double CostMgr::cost_cal_use_turtle(bool use_output, bool use_temp_lib) {
    abccmd("backup");
    abccmd("strash");
    abccmd("write_aiger temp.aig");
    string verilog_file;
    if (use_output) {
        verilog_file = _output_file;
    }
    else {
        verilog_file = _temp_file;
    }

    string command;
    if (use_temp_lib)
        command = "./test " + verilog_file + " ./temp.genlib ./temp.super"; 
    else
        command = "./test " + verilog_file + " ./contest.genlib ./contest.super"; 

    command += has_timing ? " has_timing" : " has_no_timing";
    int result = system(command.c_str());

    string read_file = "read -m ";
    read_file += verilog_file;
    abccmd(read_file);
    string write_file = "write_verilog ";
    write_file += verilog_file;
    if (_not_penalty) {
        replace_not_with_nand();
    }
    if (add_buf) {
        abccmd("topo");
        abccmd("buffer -N " + to_string(_buf_num));
    }
    if (_add_const) {
        add_const_on_pi();
    }
    abccmd(write_file);

    abccmd("restore");

    string args = "-library " + _lib_file + " -netlist " + verilog_file + " -output temp.out";
    string output = exec(_cost_exe, args);
    float temp = extractCost(output);
    if (temp == 0) return 10000;
    return temp;
}

double CostMgr::cost_cal_use_turtle_high_effort(bool use_output, bool use_temp_lib) {
    abccmd("backup");
    abccmd("strash");
    abccmd("write_aiger temp.aig");
    string verilog_file;
    if (use_output) {
        verilog_file = _output_file;
    }
    else {
        verilog_file = _temp_file;
    }

    string command;
    if (use_temp_lib)
        command = "./test " + verilog_file + " ./temp.genlib ./temp.super"; 
    else
        command = "./test " + verilog_file + " ./contest.genlib ./contest.super"; 

    command += has_timing ? " has_timing" : "";
    int result = system(command.c_str());

    abccmd("read -m temp.v");
    abccmd("backup");
    // if (abccmd("mfs3 -ae -I 5 -O 5 -R 4 -E 1000")) {
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd("mfs3 -ae -M 2500 -R 4 -E 1000");
    }
    else {
        abccmd("restore");
    }
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd("mfs3 -aer -M 2500 -R 4 -E 1000");
    }
    else {
        abccmd("restore");
    }

    if (add_buf) {
        abccmd("topo");
        abccmd("buffer -N " + to_string(_buf_num));
    }
    if (_not_penalty) {
        replace_not_with_nand();
    }

    if (_add_const) {
        add_const_on_pi();
    }

    abccmd("write_verilog temp.v");

    abccmd("restore");

    string args = "-library " + _lib_file + " -netlist " + verilog_file + " -output temp.out";
    string output = exec(_cost_exe, args);
    float temp = extractCost(output);
    if (temp == 0) return 10000;
    return temp;
}

void CostMgr::change_name() {
    std::remove(_output_file.c_str());

    // 重新命名（移動）文件
    if (std::rename(_temp_file.c_str(), _output_file.c_str()) != 0) {
        std::perror("Error renaming file");
    } else {
        std::cout << "File renamed successfully" << std::endl;
    }
}

void CostMgr::change_low_effort_name() {
    std::remove(_low_effort_file_name.c_str());

    try {
        std::filesystem::copy(_temp_file, _low_effort_file_name, std::filesystem::copy_options::overwrite_existing);
        std::cout << "low_effort_temp file successfully copied" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
}

void CostMgr::change_turtle_low_effort_name() {
    std::remove(_turtle_low_effort_file_name.c_str());

    try {
        std::filesystem::copy(_temp_file, _turtle_low_effort_file_name, std::filesystem::copy_options::overwrite_existing);
        std::cout << "turtle low_effort_temp file successfully copied" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
}

void CostMgr::change_high_effort_name() {
    std::remove(_high_effort_file_name.c_str());

    try {
        std::filesystem::copy(_temp_file, _high_effort_file_name, std::filesystem::copy_options::overwrite_existing);
        std::cout << "high_effort_temp file successfully copied" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
}

void CostMgr::change_turtle_high_effort_name() {
    std::remove(_turtle_high_effort_file_name.c_str());

    try {
        std::filesystem::copy(_temp_file, _turtle_high_effort_file_name, std::filesystem::copy_options::overwrite_existing);
        std::cout << "turtle high_effort_temp file successfully copied" << std::endl;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error copying file: " << e.what() << std::endl;
    }
}

double CostMgr::cost_cal_high_effort(bool use_output) {
    abccmd("backup");
    abccmd("&get -n");
    
    // abccmd("&dch -f");
    string act = "&dch ";
    act += pDch ? "-p " : "";
    act += fDch ? "-f " : "";
    // cout << act << endl;
    abccmd(act);

    // abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    act = "&nf ";
    act += pNf ? "-p " : "";
    act += "-F " + to_string(FNf) + " ";
    act += "-E " + to_string(ENf) + " ";
    act += "-Q " + to_string(QNf) + " ";
    act += "-R " + to_string(RNf) + " ";
    act += "-C 32";
    // cout << act << endl;
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd(act);
    }
    else {
        abccmd("restore");
        return __FLT_MAX__;
    }

    abccmd("&put");
    act = "mfs3 -e ";
    act += aMfs3 ? "-a " : "";
    // act += "-I 5 -O 5 -R 4 -E 1000";
    act += "-M 2500 -R 4 -E 1000";
    // cout << act << endl;
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd(act);
    }
    else {
        abccmd("restore");
        return __FLT_MAX__;
    }
    act = "mfs3 -er ";
    act += aMfs3 ? "-a " : "";
    // act += "-I 5 -O 5 -R 4 -E 1000";
    act += "-M 2500 -R 4 -E 1000";
    // cout << act << endl;
    if (setjmp(jump_buffer) == 0) {
        std::signal(SIGABRT, signalHandler);
        abccmd(act);
    }
    else {
        abccmd("restore");
        return __FLT_MAX__;
    }

    // abccmd("&get -n");
    // abccmd("&dch -f");
    // abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    // abccmd("&put");
    // abccmd("mfs3 -ae -I 5 -O 5 -R 4 -E 1000");
    // abccmd("mfs3 -aer -I 5 -O 5 -R 4 -E 1000");

    if (add_buf) {
        abccmd("topo");
        abccmd("buffer -N " + to_string(_buf_num));
    }
    if (_not_penalty) {
        replace_not_with_nand();
    }

    if (_add_const) {
        add_const_on_pi();
    }

    abccmd("write_verilog temp.v");
    // Abc_replace_super("tmep.v");
    string args = "-library " + _lib_file + " -netlist " + "temp.v" + " -output temp.out";
    string temp = exec(_cost_exe, args);
    double high_effort_cost = extractCost(temp);
    return high_effort_cost;
}
