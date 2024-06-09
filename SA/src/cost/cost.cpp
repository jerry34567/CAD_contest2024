#include "cost.h"

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
    return 40000*cost;
}

double cost_cal(const string& lib_file, const string& cost_exe, const string& output_file) {
    abccmd("backup");
    abccmd("&get -n");
    abccmd("&dch -f");
    abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
    abccmd("&put");
    abccmd("mfs3 -ae -I 4 -O 2");
    abccmd("mfs3 -ae -I 4 -O 2");
    string write_verilog = "write_verilog ";
    write_verilog += output_file;
    abccmd(write_verilog);
    abccmd("restore");
    string args = "-library " + lib_file + " -netlist " + output_file + " -output temp.out";
    string output = exec(cost_exe, args);
    return extractCost(output);
}