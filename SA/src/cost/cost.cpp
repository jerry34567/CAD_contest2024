#include "cost.h"
#include "gvAbcSuper.h"
#include "abc_util.h"

CostMgr* costMgr;
extern AbcSuperMgr* abcSuperMgr;
extern AbcMgr* abcMgr;

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

double cost_diff(double orginal_cost, double after_cost, double init_cost) {
    double cost = cost_func(after_cost, init_cost) - cost_func(orginal_cost, init_cost);
    // cout << cost << endl;
    return 50000*cost;
}



double CostMgr::cost_cal(bool use_output, bool buf_flag) {
    abccmd("backup");
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 4 -O 2");
    // abccmd("mfs3 -ae -I 4 -O 2");
    if (buf_flag) {
        abccmd("topo");
        abccmd("buffer -N 2");
    }
    // abccmd("fraig_sweep");
    if (_not_penalty) {
        replace_not_with_nand();
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

double CostMgr::cost_cal_use_turtle(bool use_output, bool buf_flag, bool use_temp_lib) {
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

    int result = system(command.c_str());

    if (_not_penalty) {
        string read_file = "read -m ";
        read_file += verilog_file;
        abccmd(read_file);
        replace_not_with_nand();
        string write_file = "write_verilog ";
        write_file += verilog_file;
        abccmd(write_file);
    }
    if (buf_flag) {
        string read_file = "read -m ";
        read_file += verilog_file;
        abccmd(read_file);
        abccmd("topo");
        abccmd("buffer -N 2");
        string write_file = "write_verilog ";
        write_file += verilog_file;
        abccmd(write_file);
    }

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
