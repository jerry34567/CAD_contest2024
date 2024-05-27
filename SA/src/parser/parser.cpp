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
    getline(read_file, module_name);
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
    dictionary["not"] = "O=!a;";
    dictionary["buf"] = "O=a;";
    dictionary["and"] = "O=a*b;";
    dictionary["nand"] = "O=!(a*b);";
    dictionary["or"] = "O=a+b;";
    dictionary["nor"] = "O=!(a+b);";
    dictionary["xor"] = "O=a*!b+!a*b;";
    dictionary["xnor"] = "O=a*b+!a*!b;";

    unordered_map<string, string> dictionary2;
    dictionary2["not"] = "INV";
    dictionary2["buf"] = "NONINV";
    dictionary2["and"] = "NONINV";
    dictionary2["nand"] = "INV";
    dictionary2["or"] = "NONINV";
    dictionary2["nor"] = "INV";
    dictionary2["xor"] = "UNKNOWN";
    dictionary2["xnor"] = "UNKNOWN";

    map<string, pair<string, float>> temp_dic;

    // Analyze
    for (auto& cell_data : j["cells"]) {
        ofstream write_file("temp_file.v");
        write_file << module_name << " (a, b, o);\n";
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
        writefile << "PIN * " << dictionary2[cell_data.first] << " 1 999 0 0 0 0\n";
    }
    writefile << "GATE zero\t0\tO=CONST0;\nGATE one\t0\tO=CONST1;\n";
    writefile.close();

    return 0;
}

// int main() {
//     parser("./release/lib/lib1.json", "./release/netlists/design6.v", "./release/cost_estimators/cost_estimator_1");
//     return 0;
// }
