#include "parser.h"
#include "cost.h"
#include "gvAbcSuper.h"
#include "gvAbcMgr.h"
#include <unordered_set>

extern CostMgr* costMgr;
extern AbcSuperMgr* abcSuperMgr;
extern AbcMgr* abcMgr;

unordered_map<string, string> dictionary;
unordered_map<string, string> dictionary2;
unordered_map<string, string> dictionary3;

std::string generateOutput();

void try_to_change_gate(string gate_type, unordered_map<string, unordered_set<string>>& special_dic, map<string, pair<string, double>>& temp_dic, map<string, vector<double>>& timing_dic, string lib_file, string cost_exe) {
    string temp_best;
    double temp_best_cost = MAXFLOAT;
    for (auto& it : special_dic[gate_type]) {
        bool gar = false;
        temp_dic[gate_type].first = it;
        write_liberty("./contest_liberty.lib", temp_dic);
        abccmd("read contest_liberty.lib");
        // write_genlib("contest.genlib", temp_dic, timing_dic, 0, gar);
        // abccmd("read contest.genlib");
        // cout << cell_data["cell_name"] << " " << cell_data["cell_type"] << endl;
        abccmd("backup");
        abccmd("&get -n");
        abccmd("&nf -p -a -F 10 -A 10 -E 100 -Q 100 -C 32 -R 1000");
        abccmd("&put");
        abccmd("buffer -N 2");
        abccmd("write_verilog temp.v");
        abccmd("restore");
        string args1 = "-library " + lib_file + " -netlist " + "temp.v" + " -output temp.out";
        string temp1 = exec(cost_exe, args1);
        double temp_cost = extractCost(temp1);
        cout << it << " " << temp_cost << endl;
        if (temp_cost < temp_best_cost) {
            temp_best = it;
            temp_best_cost = temp_cost;
        }
    }
    temp_dic[gate_type].first = temp_best;
    // temp_dic[gate_type].second = temp_best_cost;
}

std::string extractModuleName(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return "";
    }

    std::string line;
    std::regex moduleRegex(R"(module\s+(\w+))");
    std::smatch match;

    while (std::getline(file, line)) {
        // Skip comment lines
        std::string trimmedLine = std::regex_replace(line, std::regex("^\\s+"), "");
        if (trimmedLine.rfind("//", 0) == 0) {
            continue;
        }

        if (std::regex_search(line, match, moduleRegex)) {
            return match[1];
        }
    }

    return "";
}

void initDictionary() {
    dictionary["not"] = "Y=!A;";
    dictionary["buf"] = "Y=A;";
    dictionary["and"] = "Y=A*B;";
    dictionary["nand"] = "Y=!(A*B);";
    dictionary["or"] = "Y=A+B;";
    dictionary["nor"] = "Y=!(A+B);";
    dictionary["xor"] = "Y=A*!B+!A*B;";
    dictionary["xnor"] = "Y=A*B+!A*!B;";
}
void initDictionary2() {
    dictionary2["not"] = "INV";
    dictionary2["buf"] = "NONINV";
    dictionary2["and"] = "NONINV";
    dictionary2["nand"] = "INV";
    dictionary2["or"] = "NONINV";
    dictionary2["nor"] = "INV";
    dictionary2["xor"] = "UNKNOWN";
    dictionary2["xnor"] = "UNKNOWN";
}
void initDictionary3() {
    dictionary3["not"] = "!A";
    dictionary3["buf"] = "A";
    dictionary3["and"] = "A & B";
    dictionary3["nand"] = "!(A & B)";
    dictionary3["or"] = "A | B";
    dictionary3["nor"] = "!(A | B)";
    dictionary3["xor"] = "(A & !B) | (!A & B)";
    dictionary3["xnor"] = "(A & B) | (!A & !B)";
}

void initTimingDictionary(map<string, vector<double>>& timing_dic) {
    timing_dic["not"] = {1, 0};
    timing_dic["buf"] = {1, 0};
    timing_dic["and"] = {1, 0};
    timing_dic["nand"] = {1, 0};
    timing_dic["or"] = {1, 0};
    timing_dic["nor"] = {1, 0};
    timing_dic["xor"] = {1, 0};
    timing_dic["xnor"] = {1, 0};
}

void initTimingDictionary_using_cost(map<string, vector<double>>& timing_dic, map<string, pair<string, double>>& temp_dic, string lib_file, string cost_exe) {
    Abc_Ntk_t* pNtk = Abc_FrameReadNtk(abcMgr->get_Abc_Frame_t());
    int level = Abc_NtkLevel(pNtk);
    int no_timing = 0;
    for (auto& cell : temp_dic) {
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
                if (cell.first != "buf" && cell.first != "not") {
                    if (i == 0)
                        write_file << cell.second.first << "(a,b,y1);\n";
                    else if (i == temp_level-1) {
                        write_file << cell.second.first << "(y" << to_string(i) << ",t" << to_string(i) << ",o);\n";
                    }
                    else {
                        write_file << cell.second.first << "(y" << to_string(i) << ",t" << to_string(i) << ",y" << to_string(i+1) << ");\n";
                    }
                }
                else {
                    if (i == 0)
                        write_file << cell.second.first << "(a,y1);\n";
                    else if (i == temp_level-1) {
                        write_file << cell.second.first << "(y" << to_string(i) << ",o);\n";
                    }
                    else {
                        write_file << cell.second.first << "(y" << to_string(i) << ",y" << to_string(i+1) << ");\n";
                    }
                }
            }
            write_file << "endmodule\n";
            write_file.close();
            string args = "-library " + lib_file + " -netlist temp_file.v -output temp.out";
            string output = exec(cost_exe, args);
            float cost = extractCost(output);
            // cout << cell.second.first << ": " << temp_level << "  cost: " << cost << endl;
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
                if (cost_distance * 5 < after_cost_distance) {
                    timing_dic[cell.first] = {(5.0 / (temp_level-1)), 0};
                    break;
                }
                cost_distance = after_cost_distance;
                last_cost = cost;
            }
            temp_level++;
        }
        if (temp_level == level * 3) {
            timing_dic[cell.first] = {1, 0};

        }
    }
}




// void write_syntcl(const string& module_name, const string& verilog_file) {
//     ofstream write_syn("syn.tcl");
//     if (!write_syn.is_open()) {
//         cerr << "can NOT open file!" << endl;
//     }

//     write_syn << "set design_name \"" + module_name + "\"\n" +
//                  "set verilog_file \"" + verilog_file + "\"\n" +
//                  "set library_file \"contest_liberty.db\"\n" +
//                  "set target_library  \"contest_liberty.db\"\n" +
//                  "set link_library   \"* $target_library\"\n" +
//                  "analyze -format verilog $verilog_file\n" +
//                  "elaborate $design_name\n" +
//                  "current_design $design_name\n" +
//                  "set_max_area 0\n" +
//                  "uniquify\n" +
//                  "compile_ultra -area -no_design_rule\n" +
//                  "compile_ultra -incremental\n" +
//                  "optimize_netlist -area\n" +
//                  "optimize_netlist -area\n" +
//                  "write -f verilog -hierarchy -output syn_design.v\n" +
//                  "quit\n";
//     write_syn.close();
// }

void write_genlib(const string& output_file_name, map<string, pair<string, double>>& temp_dic, map<string, vector<double>>& timing_dic, bool is_super, bool& not_penalty) {
    ofstream writefile(output_file_name);
    if (!writefile.is_open()) {
        cerr << "can NOT open file!" << endl;
    }

    double max_area = 0;
    for (auto& cell_data : temp_dic) {
        if (cell_data.second.second > max_area && cell_data.second.second < 100000) max_area = cell_data.second.second;
        if (cell_data.second.second > 1) cell_data.second.second -= 1;
    }
    for (auto& cell_data : temp_dic) {
        writefile << "GATE " << cell_data.second.first << "\t";
        if (cell_data.second.second > 100000) {
            if (cell_data.first == "not") {
                writefile << temp_dic["nand"].second << "\t";
                cell_data.second.second = temp_dic["nand"].second;
                not_penalty = true;
            }
            else
                writefile << 8*max_area << "\t";
        }
        else
            writefile << cell_data.second.second << "\t";
        writefile << dictionary[cell_data.first] << "\t";
        writefile << "PIN * " << dictionary2[cell_data.first] << " 1 999 ";
        writefile << timing_dic[cell_data.first][0] << " " << timing_dic[cell_data.first][1] << " " << timing_dic[cell_data.first][0] << " " << timing_dic[cell_data.first][1] << "\n";
    }
    writefile << "GATE zero\t" << max_area << "\tY=CONST0;\nGATE one\t" << max_area << "\tY=CONST1;\n";
    
    if (is_super)
        for (auto& cell_data : abcSuperMgr->get_name_to_super()) {
            writefile << "GATE " << cell_data.first << "\t";
            writefile << cell_data.second->get_area() << "\t";
            writefile << "Y=" << cell_data.second->get_function() << ";" << "\t";
            writefile << "PIN * UNKNOWN 1 999 1 0 1 0\n";
        }
    writefile.close();
}



unordered_map<string, unordered_set<string>> parser(const string& lib_file, const string& verilog_file, const string& cost_exe, map<string, pair<string, double>>& temp_dic, map<string, vector<double>>& timing_dic, bool& not_penalty) {
    string input_file_name = lib_file;
    string genlib_file_name = "./contest.genlib";
    string verilog_file_name = verilog_file;
    string module_name = costMgr->get_module_name();

    // Read JSON file
    ifstream readfile(input_file_name);
    if (!readfile.is_open()) {
        cerr << "can NOT open file!" << endl;
    }
    json j;
    readfile >> j;
    readfile.close();

    // Dictionary
    initDictionary();
    initDictionary2();
    initDictionary3();
    // initTimingDictionary(timing_dic);

    // Analyze
    for (auto& cell_data : j["cells"]) {
        ofstream write_file("temp_file.v");
        costMgr->add_name_to_type(cell_data["cell_name"], cell_data["cell_type"]);
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

    for (auto& cell_data : temp_dic) {
        if (cell_data.second.second > 1) cell_data.second.second -= 1;
    }

    float check_same = temp_dic["and"].second;
    unordered_map<string, unordered_set<string>> special_dic;
    for (auto& cell_data : j["cells"]) {
        special_dic[cell_data["cell_type"]].insert(cell_data["cell_name"]);
    }

    if (temp_dic["nand"].second == check_same && temp_dic["nor"].second == check_same && temp_dic["or"].second == check_same && temp_dic["not"].second == check_same && temp_dic["buf"].second == check_same && temp_dic["xor"].second == check_same && temp_dic["xnor"].second == check_same) {
        try_to_change_gate("and", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("nand", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("nor", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("or", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("not", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("xor", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("xnor", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
        try_to_change_gate("buf", special_dic, temp_dic, timing_dic, lib_file, cost_exe);
    }
    initTimingDictionary_using_cost(timing_dic, temp_dic, lib_file, cost_exe);
    // Write genlib file
    write_genlib(genlib_file_name, temp_dic, timing_dic, 0, not_penalty);

    // write syn.tcl
    // write_syntcl(module_name, verilog_file_name);

    // write lc.sh
    // ofstream write_lc("lc.sh");
    // if (!write_lc.is_open()) {
    //     cerr << "can NOT open file!" << endl;
    // }

    // write_lc << "read_lib contest_liberty.lib\nwrite_lib -format db my_lib -output contest_liberty.db\nquit\n";
    // write_lc.close();


    // write contest_liberty.lib
    write_liberty("./contest_liberty.lib", temp_dic);

    return special_dic;
}

void write_liberty(const string& output_file_name, map<string, pair<string, double>>& temp_dic) {
    ofstream write_lib(output_file_name);
    if (!write_lib.is_open()) {
        cerr << "can NOT open file!" << endl;
    }

    write_lib << "/*\n";
    write_lib << "* ******************************************************************************\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* *                   Copyright (C) 2004-2011, Nangate Inc.                    *\n";
    write_lib << "* *                           All rights reserved.                             *\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* * Nangate and the Nangate logo are trademarks of Nangate Inc.                *\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* * All trademarks, logos, software marks, and trade names (collectively the   *\n";
    write_lib << "* * \"Marks\") in this program are proprietary to Nangate or other respective    *\n";
    write_lib << "* * owners that have granted Nangate the right and license to use such Marks.  *\n";
    write_lib << "* * You are not permitted to use the Marks without the prior written consent   *\n";
    write_lib << "* * of Nangate or such third party that may own the Marks.                     *\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* * This file has been provided pursuant to a License Agreement containing     *\n";
    write_lib << "* * restrictions on its use. This file contains valuable trade secrets and     *\n";
    write_lib << "* * proprietary information of Nangate Inc., and is protected by U.S. and      *\n";
    write_lib << "* * international laws and/or treaties.                                        *\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* * The copyright notice(s) in this file does not indicate actual or intended  *\n";
    write_lib << "* * publication of this file.                                                  *\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* *   NGLibraryCharacterizer, v2011.01-HR04-2011-01-19 - build 201102050200    *\n";
    write_lib << "* *                                                                            *\n";
    write_lib << "* ******************************************************************************\n";
    write_lib << "*\n";
    write_lib << "* Spice engine            : Nanspice v2011.01-HR04-2011-01-19-1102050200\n";
    write_lib << "* Liberty export type     : conditional\n";
    write_lib << "*\n";
    write_lib << "* Characterization Corner : fast\n";
    write_lib << "* Process                 : FastFast\n";
    write_lib << "* Temperature             : 0C\n";
    write_lib << "* Voltage                 : 1.25V\n";
    write_lib << "*\n";
    write_lib << "****************************************************************************/\n";
    write_lib << "\n";
    write_lib << "library (my_lib) {\n";
    write_lib << "\n";
    write_lib << "  /* Documentation Attributes */\n";
    write_lib << "  date                          : \"Thu 10 Feb 2011, 18:11:32\";\n";
    write_lib << "  revision                      : \"revision 1.0\";\n";
    write_lib << "  comment                       : \"Copyright (c) 2004-2011 Nangate Inc. All Rights Reserved.\";\n";
    write_lib << "\n";
    write_lib << "  /* General Attributes */\n";
    write_lib << "  technology                    (cmos);\n";
    write_lib << "  delay_model                   : table_lookup;\n";
    write_lib << "  in_place_swap_mode            : match_footprint;\n";
    write_lib << "\n";
    write_lib << "  /* Units Attributes */\n";
    write_lib << "  time_unit                     : \"1ns\";\n";
    write_lib << "  leakage_power_unit            : \"1nW\";\n";
    write_lib << "  voltage_unit                  : \"1V\";\n";
    write_lib << "  current_unit                  : \"1mA\";\n";
    write_lib << "  pulling_resistance_unit       : \"1kohm\";\n";
    write_lib << "  capacitive_load_unit          (1,ff);\n";
    write_lib << "\n";
    write_lib << "  /* Operation Conditions */\n";
    write_lib << "\n";
    write_lib << "  /* Threshold Definitions */\n";
    write_lib << "  slew_lower_threshold_pct_fall  : 30.00 ;\n";
    write_lib << "  slew_lower_threshold_pct_rise  : 30.00 ;\n";
    write_lib << "  slew_upper_threshold_pct_fall  : 70.00 ;\n";
    write_lib << "  slew_upper_threshold_pct_rise  : 70.00 ;\n";
    write_lib << "  slew_derate_from_library       : 1.00 ;\n";
    write_lib << "  input_threshold_pct_fall       : 50.00 ;\n";
    write_lib << "  input_threshold_pct_rise       : 50.00 ;\n";
    write_lib << "  output_threshold_pct_fall      : 50.00 ;\n";
    write_lib << "  output_threshold_pct_rise      : 50.00 ;\n";
    write_lib << "  default_leakage_power_density  : 0.00 ;\n";
    write_lib << "  default_cell_leakage_power     : 0.00 ;\n";
    write_lib << "\n";
    write_lib << "  /* Default Pin Attributes */\n";
    write_lib << "  default_inout_pin_cap          : 1.000000;\n";
    write_lib << "  default_input_pin_cap          : 1.000000;\n";
    write_lib << "  default_output_pin_cap         : 0.000000;\n";
    write_lib << "  default_fanout_load            : 1.000000;\n";
    write_lib << "  default_max_transition         : 0.146240;\n";
    write_lib << "\n";
    write_lib << "  define(drive_strength, cell, float);\n";
    write_lib << "\n";
    write_lib << "  /* Wire load tables */\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"1K_hvratio_1_4\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.3207 );\n";
    write_lib << "    fanout_length( 2, 2.9813 );\n";
    write_lib << "    fanout_length( 3, 5.1135 );\n";
    write_lib << "    fanout_length( 4, 7.6639 );\n";
    write_lib << "    fanout_length( 5, 10.0334 );\n";
    write_lib << "    fanout_length( 6, 12.2296 );\n";
    write_lib << "    fanout_length( 8, 19.3185 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"1K_hvratio_1_2\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.3216 );\n";
    write_lib << "    fanout_length( 2, 2.8855 );\n";
    write_lib << "    fanout_length( 3, 4.6810 );\n";
    write_lib << "    fanout_length( 4, 6.7976 );\n";
    write_lib << "    fanout_length( 5, 9.4037 );\n";
    write_lib << "    fanout_length( 6, 13.0170 );\n";
    write_lib << "    fanout_length( 8, 24.1720 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"1K_hvratio_1_1\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 6.283688;\n";
    write_lib << "    fanout_length( 1, 1.3446 );\n";
    write_lib << "    fanout_length( 2, 2.8263 );\n";
    write_lib << "    fanout_length( 3, 4.7581 );\n";
    write_lib << "    fanout_length( 4, 7.4080 );\n";
    write_lib << "    fanout_length( 5, 10.9381 );\n";
    write_lib << "    fanout_length( 6, 15.7314 );\n";
    write_lib << "    fanout_length( 8, 29.7891 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"3K_hvratio_1_4\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.8234 );\n";
    write_lib << "    fanout_length( 2, 4.5256 );\n";
    write_lib << "    fanout_length( 3, 7.5342 );\n";
    write_lib << "    fanout_length( 4, 10.6237 );\n";
    write_lib << "    fanout_length( 5, 13.5401 );\n";
    write_lib << "    fanout_length( 6, 16.3750 );\n";
    write_lib << "    fanout_length( 7, 18.6686 );\n";
    write_lib << "    fanout_length( 8, 19.4348 );\n";
    write_lib << "    fanout_length( 10, 20.9672 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"3K_hvratio_1_2\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.6615 );\n";
    write_lib << "    fanout_length( 2, 3.9827 );\n";
    write_lib << "    fanout_length( 3, 6.6386 );\n";
    write_lib << "    fanout_length( 4, 9.6287 );\n";
    write_lib << "    fanout_length( 5, 12.8485 );\n";
    write_lib << "    fanout_length( 6, 16.4145 );\n";
    write_lib << "    fanout_length( 7, 20.0747 );\n";
    write_lib << "    fanout_length( 8, 22.6325 );\n";
    write_lib << "    fanout_length( 10, 21.7173 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"3K_hvratio_1_1\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.5771 );\n";
    write_lib << "    fanout_length( 2, 3.9330 );\n";
    write_lib << "    fanout_length( 3, 6.6217 );\n";
    write_lib << "    fanout_length( 4, 9.7638 );\n";
    write_lib << "    fanout_length( 5, 13.5526 );\n";
    write_lib << "    fanout_length( 6, 18.1322 );\n";
    write_lib << "    fanout_length( 7, 22.5871 );\n";
    write_lib << "    fanout_length( 8, 25.1074 );\n";
    write_lib << "    fanout_length( 10, 30.1480 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"5K_hvratio_1_4\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 2.0449 );\n";
    write_lib << "    fanout_length( 2, 4.4094 );\n";
    write_lib << "    fanout_length( 3, 7.2134 );\n";
    write_lib << "    fanout_length( 4, 10.4927 );\n";
    write_lib << "    fanout_length( 5, 13.9420 );\n";
    write_lib << "    fanout_length( 6, 18.0039 );\n";
    write_lib << "    fanout_length( 7, 23.9278 );\n";
    write_lib << "    fanout_length( 8, 30.8475 );\n";
    write_lib << "    fanout_length( 9, 34.9441 );\n";
    write_lib << "    fanout_length( 11, 43.1373 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"5K_hvratio_1_2\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.6706 );\n";
    write_lib << "    fanout_length( 2, 3.7951 );\n";
    write_lib << "    fanout_length( 3, 6.2856 );\n";
    write_lib << "    fanout_length( 4, 9.1309 );\n";
    write_lib << "    fanout_length( 5, 12.1420 );\n";
    write_lib << "    fanout_length( 6, 15.6918 );\n";
    write_lib << "    fanout_length( 7, 20.1043 );\n";
    write_lib << "    fanout_length( 8, 24.2827 );\n";
    write_lib << "    fanout_length( 9, 27.3445 );\n";
    write_lib << "    fanout_length( 11, 35.3421 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  wire_load(\"5K_hvratio_1_1\") {\n";
    write_lib << "    capacitance : 1.774000e-01;\n";
    write_lib << "    resistance : 3.571429e-03;\n";
    write_lib << "    slope : 5.000000;\n";
    write_lib << "    fanout_length( 1, 1.7460 );\n";
    write_lib << "    fanout_length( 2, 3.9394 );\n";
    write_lib << "    fanout_length( 3, 6.4626 );\n";
    write_lib << "    fanout_length( 4, 9.2201 );\n";
    write_lib << "    fanout_length( 5, 11.9123 );\n";
    write_lib << "    fanout_length( 6, 14.8358 );\n";
    write_lib << "    fanout_length( 7, 18.6155 );\n";
    write_lib << "    fanout_length( 8, 22.6727 );\n";
    write_lib << "    fanout_length( 9, 25.4842 );\n";
    write_lib << "    fanout_length( 11, 27.0320 );\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "  default_wire_load : \"5K_hvratio_1_1\" ;\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  power_lut_template (Hidden_power_7) {\n";
    write_lib << "    variable_1 : input_transition_time;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030,0.0040,0.0050,0.0060,0.0070\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Hold_3_3) {\n";
    write_lib << "    variable_1 : constrained_pin_transition;\n";
    write_lib << "    variable_2 : related_pin_transition;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "    index_2 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  power_lut_template (Power_7_7) {\n";
    write_lib << "    variable_1 : input_transition_time;\n";
    write_lib << "    variable_2 : total_output_net_capacitance;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030,0.0040,0.0050,0.0060,0.0070\");\n";
    write_lib << "    index_2 (\"0.0010,0.0020,0.0030,0.0040,0.0050,0.0060,0.0070\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Pulse_width_3) {\n";
    write_lib << "    variable_1 : related_pin_transition;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Recovery_3_3) {\n";
    write_lib << "    variable_1 : constrained_pin_transition;\n";
    write_lib << "    variable_2 : related_pin_transition;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "    index_2 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Removal_3_3) {\n";
    write_lib << "    variable_1 : constrained_pin_transition;\n";
    write_lib << "    variable_2 : related_pin_transition;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "    index_2 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Setup_3_3) {\n";
    write_lib << "    variable_1 : constrained_pin_transition;\n";
    write_lib << "    variable_2 : related_pin_transition;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "    index_2 (\"0.0010,0.0020,0.0030\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Timing_7_7) {\n";
    write_lib << "    variable_1 : input_net_transition;\n";
    write_lib << "    variable_2 : total_output_net_capacitance;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030,0.0040,0.0050,0.0060,0.0070\");\n";
    write_lib << "    index_2 (\"0.0010,0.0020,0.0030,0.0040,0.0050,0.0060,0.0070\");\n";
    write_lib << "  }\n";
    write_lib << "\n";
    write_lib << "\n";
    write_lib << "  lu_table_template (Tristate_disable_7) {\n";
    write_lib << "    variable_1 : input_net_transition;\n";
    write_lib << "    index_1 (\"0.0010,0.0020,0.0030,0.0040,0.0050,0.0060,0.0070\");\n";
    write_lib << "  }\n\n";

    // map<string, string> temp_timing_dic = {
    //     "and" : 
    // }


    for (auto& cell_data : temp_dic) {
        write_lib << "\tcell(" << cell_data.second.first << ") {\n"
                  << "\t\tarea : " << cell_data.second.second << ";\n";
        if (cell_data.first == "not" || cell_data.first == "buf") {
            write_lib << "\t\tpin(A) {\n\t\t\tdirection : input;\n\t\t}\n\t\tpin(Y) {\n\t\t\tdirection : output;\n\t\t\tfunction : \""
                      << dictionary3[cell_data.first] << "\";\n\t\t\ttiming() {\n \t\t\t\trelated_pin : \"A\";\n"
                      << generateOutput()
                      << "\t\t\t}\n\t\t}\n\t}\n\n";
        }
        else {
            write_lib << "\t\tpin(A) {\n\t\t\tdirection : input;\n\t\t}\n\t\tpin(B) {\n\t\t\tdirection : input;\n\t\t}\n\t\tpin(Y) {\n\t\t\tdirection : output;\n\t\t\tfunction : \""
                      << dictionary3[cell_data.first] << "\";\n\t\t\ttiming() {\n \t\t\t\trelated_pin : \"A\";\n"
                      << generateOutput()
                      << "\t\t\t}\n\t\t\ttiming() {\n \t\t\t\trelated_pin : \"B\";\n"
                      << generateOutput()
                      << "\t\t\t}\n\t\t}\n\t}\n\n";
        }
    }
    write_lib << "}\n";
}


std::string generateOutput() {
    std::ostringstream file;
    file << "cell_fall(Timing_7_7) {\n";
    file << "    index_1 (\"0.000932129,0.00354597,0.0127211,0.0302424,0.0575396,0.0958408,0.146240\");\n";
    file << "    index_2 (\"0.365616,3.776560,7.553130,15.106300,30.212500,60.425000,120.850000\");\n";
    file << "    values (\"0.0117055,0.0142627,0.0166263,0.0207942,0.0284388,0.0432601,0.0727947\", \\\n";
    file << "            \"0.0126428,0.0151923,0.0175537,0.0217218,0.0293681,0.0441914,0.0737271\", \\\n";
    file << "            \"0.0165428,0.0190507,0.0213856,0.0255348,0.0331654,0.0479752,0.0775016\", \\\n";
    file << "            \"0.0215059,0.0242901,0.0268105,0.0311556,0.0388987,0.0536596,0.0831139\", \\\n";
    file << "            \"0.0268261,0.0299226,0.0327013,0.0373944,0.0454343,0.0602911,0.0896921\", \\\n";
    file << "            \"0.0322980,0.0357250,0.0388172,0.0439916,0.0526341,0.0679170,0.0973737\", \\\n";
    file << "            \"0.0378108,0.0415588,0.0449791,0.0506864,0.0601014,0.0761881,0.106216\");\n";
    file << "}\n\n";

    file << "cell_rise(Timing_7_7) {\n";
    file << "    index_1 (\"0.000932129,0.00354597,0.0127211,0.0302424,0.0575396,0.0958408,0.146240\");\n";
    file << "    index_2 (\"0.365616,3.776560,7.553130,15.106300,30.212500,60.425000,120.850000\");\n";
    file << "    values (\"0.0145284,0.0176673,0.0207117,0.0262585,0.0367181,0.0572697,0.0982957\", \\\n";
    file << "            \"0.0151301,0.0182671,0.0213094,0.0268535,0.0373123,0.0578650,0.0988918\", \\\n";
    file << "            \"0.0180831,0.0211719,0.0241731,0.0296611,0.0400622,0.0605765,0.101588\", \\\n";
    file << "            \"0.0218692,0.0250128,0.0280348,0.0335219,0.0438922,0.0642793,0.105190\", \\\n";
    file << "            \"0.0254830,0.0288330,0.0320027,0.0375987,0.0479461,0.0682833,0.109064\", \\\n";
    file << "            \"0.0285786,0.0322066,0.0356129,0.0415591,0.0521721,0.0726495,0.113370\", \\\n";
    file << "            \"0.0309863,0.0349113,0.0385776,0.0449776,0.0560572,0.0769542,0.118034\");\n";
    file << "}\n\n";

    file << "fall_transition(Timing_7_7) {\n";
    file << "    index_1 (\"0.000932129,0.00354597,0.0127211,0.0302424,0.0575396,0.0958408,0.146240\");\n";
    file << "    index_2 (\"0.365616,3.776560,7.553130,15.106300,30.212500,60.425000,120.850000\");\n";
    file << "    values (\"0.00267921,0.00399612,0.00541962,0.00829245,0.0142954,0.0268614,0.0523948\", \\\n";
    file << "            \"0.00268043,0.00399842,0.00542234,0.00829326,0.0142955,0.0268621,0.0523942\", \\\n";
    file << "            \"0.00297387,0.00418311,0.00555232,0.00836791,0.0143220,0.0268649,0.0523941\", \\\n";
    file << "            \"0.00408644,0.00523952,0.00648423,0.00905735,0.0146666,0.0269220,0.0523964\", \\\n";
    file << "            \"0.00547425,0.00665449,0.00785507,0.0102596,0.0154565,0.0272834,0.0524698\", \\\n";
    file << "            \"0.00719530,0.00845395,0.00969401,0.0120619,0.0169801,0.0281402,0.0527866\", \\\n";
    file << "            \"0.00922011,0.0105719,0.0118940,0.0143208,0.0191404,0.0298482,0.0537331\");\n";
    file << "}\n\n";

    file << "rise_transition(Timing_7_7) {\n";
    file << "    index_1 (\"0.000932129,0.00354597,0.0127211,0.0302424,0.0575396,0.0958408,0.146240\");\n";
    file << "    index_2 (\"0.365616,3.776560,7.553130,15.106300,30.212500,60.425000,120.850000\");\n";
    file << "    values (\"0.00335682,0.00544345,0.00764471,0.0120616,0.0212582,0.0402725,0.0785986\", \\\n";
    file << "            \"0.00335773,0.00544328,0.00764543,0.0120628,0.0212578,0.0402723,0.0785985\", \\\n";
    file << "            \"0.00344936,0.00550719,0.00769869,0.0120962,0.0212741,0.0402693,0.0785953\", \\\n";
    file << "            \"0.00418550,0.00604123,0.00813856,0.0124408,0.0214520,0.0402904,0.0786015\", \\\n";
    file << "            \"0.00537258,0.00706263,0.00903232,0.0130647,0.0218285,0.0405307,0.0786378\", \\\n";
    file << "            \"0.00695935,0.00854255,0.0104666,0.0143502,0.0227960,0.0411074,0.0788905\", \\\n";
    file << "            \"0.00889756,0.0104152,0.0122868,0.0161419,0.0242679,0.0424310,0.0796599\");\n";
    file << "}\n";

    return file.str();
}
