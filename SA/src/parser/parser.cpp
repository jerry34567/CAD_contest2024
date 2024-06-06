#include "parser.h"

int parser(const string& lib_file, const string& verilog_file, const string& cost_exe) {
    string input_file_name = lib_file;
    string output_file_name = "./contest.genlib";
    string verilog_file_name = verilog_file;
    string module_name;

    // Read verilog file
    ifstream read_file(verilog_file_name);
    if (!read_file.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }
    read_file >> module_name;
    read_file >> module_name;
    read_file.close();

    // Read JSON file
    ifstream readfile(input_file_name);
    if (!readfile.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }
    json j;
    readfile >> j;
    readfile.close();

    // Dictionary
    unordered_map<string, string> dictionary;
    dictionary["not"] = "Y=!A;";
    dictionary["buf"] = "Y=A;";
    dictionary["and"] = "Y=A*B;";
    dictionary["nand"] = "Y=!(A*B);";
    dictionary["or"] = "Y=A+B;";
    dictionary["nor"] = "Y=!(A+B);";
    dictionary["xor"] = "Y=A*!B+!A*B;";
    dictionary["xnor"] = "Y=A*B+!A*!B;";

    unordered_map<string, string> dictionary2;
    dictionary2["not"] = "INV";
    dictionary2["buf"] = "NONINV";
    dictionary2["and"] = "NONINV";
    dictionary2["nand"] = "INV";
    dictionary2["or"] = "NONINV";
    dictionary2["nor"] = "INV";
    dictionary2["xor"] = "UNKNOWN";
    dictionary2["xnor"] = "UNKNOWN";

    unordered_map<string, string> dictionary3;
    dictionary3["not"] = "!A";
    dictionary3["buf"] = "A";
    dictionary3["and"] = "A & B";
    dictionary3["nand"] = "!(A & B)";
    dictionary3["or"] = "A | B";
    dictionary3["nor"] = "!(A | B)";
    dictionary3["xor"] = "(A & !B) | (!A & B)";
    dictionary3["xnor"] = "(A & B) | (!A & !B)";

    map<string, pair<string, float>> temp_dic;

    // Analyze
    for (auto& cell_data : j["cells"]) {
        ofstream write_file("temp_file.v");
        write_file << "module " << module_name << " (a, b, o);\n";
        write_file << "input a, b;\n";
        write_file << "output o;\n";
        write_file << cell_data["cell_name"].get<string>();
        if (cell_data["cell_type"] == "not" || cell_data["cell_type"] == "buf") {
            write_file << " g0 (a, o);\n";
        } else {
            write_file << " g0 (a, b, o);\n";
        }
        write_file << "endmodule\n";
        write_file.close();

        string args = "-library " + input_file_name + " -netlist temp_file.v -output temp.out";
        string output = exec(cost_exe, args);
        float cost = extractCost(output);

        auto& value = temp_dic[cell_data["cell_type"]];
        if (!value.first.empty()) {
            if (value.second > cost) {
                value = {cell_data["cell_name"], cost};
            }
        } else {
            value = {cell_data["cell_name"], cost};
        }
    }

    // Write Output file
    ofstream writefile(output_file_name);
    if (!writefile.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }

    for (auto& cell_data : temp_dic) {
        writefile << "GATE " << cell_data.second.first << "\t" << cell_data.second.second << "\t";
        writefile << dictionary[cell_data.first] << "\t";
        writefile << "PIN * " << dictionary2[cell_data.first] << " 1 999 1.5 0.5 1.5 0.5\n";
    }
    writefile << "GATE zero\t0\tO=CONST0;\nGATE one\t0\tO=CONST1;\n";
    writefile.close();


    // write syn.tcl
    ofstream write_syn("syn.tcl");
    if (!write_syn.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }

    write_syn << "set design_name \"" + module_name + "\"\n" +
                 "set verilog_file \"" + verilog_file + "\"\n" +
                 "set library_file \"contest_liberty.db\"\n" +
                 "set target_library  \"contest_liberty.db\"\n" +
                 "set link_library   \"* $target_library\"\n" +
                 "analyze -format verilog $verilog_file\n" +
                 "elaborate $design_name\n" +
                 "current_design $design_name\n" +
                 "set_max_area 0\n" +
                 "uniquify\n" +
                 "compile_ultra -area -no_design_rule\n" +
                 "compile_ultra -incremental\n" +
                 "optimize_netlist -area\n" +
                 "optimize_netlist -area\n" +
                 "write -f verilog -hierarchy -output syn_design.v\n" +
                 "quit\n";
    write_syn.close();


    // write lc.sh
    ofstream write_lc("lc.sh");
    if (!write_lc.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }

    write_lc << "read_lib contest_liberty.lib\nwrite_lib -format db my_lib -output contest_liberty.db\nquit\n";
    write_lc.close();


    // write contest_liberty.lib
    ofstream write_lib("contest_liberty.lib");
    if (!write_lib.is_open()) {
        cerr << "can NOT open file!" << endl;
        return 1;
    }

    write_lib << "library(my_lib) {\n";
    for (auto& cell_data : temp_dic) {
        write_lib << "\tcell(" << cell_data.second.first << ") {\n"
                  << "\t\tarea : " << cell_data.second.second << ";\n";
        if (cell_data.first == "not" || cell_data.first == "buf") {
            write_lib << "\t\tpin(A) {\n\t\t\tdirection : input;\n\t\t}\n\t\tpin(Y) {\n\t\t\tdirection : output;\n\t\t\tfunction : \""
                      << dictionary3[cell_data.first] << "\";\n\t\t\ttiming() {\n \t\t\t\trelated_pin : \"A\";\n\t\t\t}\n\t\t}\n\t}\n\n";
        }
        else {
            write_lib << "\t\tpin(A) {\n\t\t\tdirection : input;\n\t\t}\n\t\tpin(B) {\n\t\t\tdirection : input;\n\t\t}\n\t\tpin(Y) {\n\t\t\tdirection : output;\n\t\t\tfunction : \""
                      << dictionary3[cell_data.first] << "\";\n\t\t\ttiming() {\n \t\t\t\trelated_pin : \"A\";\n\t\t\t}timing() {\n \t\t\t\trelated_pin : \"B\";\n\t\t\t}\n\t\t}\n\t}\n\n";
        }
    }
    write_lib << "}\n";

    return 0;
}

// int main() {
//     parser("./release/lib/lib1.json", "./release/netlists/design6.v", "./release/cost_estimators/cost_estimator_1");
//     return 0;
// }
