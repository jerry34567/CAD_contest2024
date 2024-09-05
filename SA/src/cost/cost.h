#ifndef COST_H
#define COST_H

#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "base/abc/abc.h"
#include "gvAbcMgr.h"
#include "libMgr.h"

using namespace std;

class CostMgr;
extern CostMgr* costMgr;
string exec(const string& cmd, const string& args);
float extractCost(const string& output);
double cost_func(double, double);
double cost_diff(double, double, double);
void choose_best_map(bool);
void choose_best_map();
// void turtle_choose_best_map();

class CostMgr {
public:
    CostMgr(string lib_file, string cost_exe, string output_file, string temp_file, string module_name) {
        _lib_file = lib_file;
        _cost_exe = cost_exe;
        _output_file = output_file;
        _temp_file = temp_file;
        _module_name = module_name;
    }
    ~CostMgr() {}
    double cost_cal(bool use_output);
    double cost_cal_use_map(bool use_output, bool buf_flag);
    double cost_cal_use_turtle(bool use_output, bool use_temp_lib);
    double cost_cal_use_turtle_high_effort(bool use_output, bool use_temp_lib);
    double cost_cal_high_effort(bool use_output);
    void set_module_name(string module_name) { _module_name = module_name; }
    string get_module_name() { return _module_name; }
    void add_name_to_type(string name, string type) {
        name_to_type[name] = type;
    }
    void change_name();
    void change_low_effort_name();
    void change_turtle_low_effort_name();
    void change_high_effort_name();
    void change_turtle_high_effort_name();
    void set_not_penalty(bool not_penalty) { _not_penalty = not_penalty; }
    void set_add_const(bool add_const ) { _add_const = add_const; }
    string get_type(string name) { return name_to_type[name]; }
    string get_lib() { return _lib_file; }
    string get_cost_exe() { return _cost_exe; }
    string get_low_effort_file() { return _low_effort_file_name; }
    string get_turtle_low_effort_file() { return _turtle_low_effort_file_name; }
    string get_high_effort_file() { return _high_effort_file_name; }
    string get_turtle_high_effort_file() { return _turtle_high_effort_file_name; }
    map<string, pair<string, double>> get_best_dic() { return best_cost_dic; }
    map<string, pair<string, double>> get_low_best_dic() { return low_best_cost_dic; }
    map<string, pair<string, double>> get_high_best_dic() { return high_best_cost_dic; }
    map<string, pair<string, double>> get_turtle_low_best_dic() { return turtle_low_best_cost_dic; }
    map<string, pair<string, double>> get_turtle_high_best_dic() { return turtle_high_best_cost_dic; }
    void set_pDch(bool flag) { pDch = flag; }
    bool get_pDch() { return pDch; }
    void set_fDch(bool flag) { fDch = flag; }
    bool get_fDch() { return fDch; }
    void set_add_buf(bool flag) { add_buf = flag; }
    bool get_add_buf() { return add_buf; }
    void set_buf_num(int buf_num) { _buf_num = buf_num; }
    void set_aMfs3(bool flag) { aMfs3 = flag; }
    bool get_aMfs3() { return aMfs3; }
    void set_pNf(bool flag) { pNf = flag; }
    bool get_pNf() { return pNf; }
    void set_FNf(int number) { FNf = number; }
    int  get_FNf() { return FNf; }
    void set_ENf(int number) { ENf = number; }
    int  get_ENf() { return ENf; }
    void set_QNf(int number) { QNf = number; }
    int  get_QNf() { return QNf; }
    void set_RNf(int number) { RNf = number; }
    int  get_RNf() { return RNf; }
    void set_has_timing(bool flag) { has_timing = flag; }
    // void set_super(bool flag) { use_super = flag; }
    void gate_sizing(unordered_map<string, unordered_set<string>>& special_dic, map<string, pair<string, double>>& temp_dic, map<string, vector<double>>& timing_dic);
    void gate_sizing_using_pre_compute_fast_gate(map<string, pair<string, double>>& fast_gate_dic, double& best_cost);
    void set_best_dic (map<string, pair<string, double>>& temp_dic) { best_cost_dic = temp_dic; }
    void set_low_best_dic (map<string, pair<string, double>>& temp_dic) { low_best_cost_dic = temp_dic; }
    void set_high_best_dic (map<string, pair<string, double>>& temp_dic) { high_best_cost_dic = temp_dic; }
    void set_turtle_low_best_dic (map<string, pair<string, double>>& temp_dic) { turtle_low_best_cost_dic = temp_dic; }
    void set_turtle_high_best_dic (map<string, pair<string, double>>& temp_dic) { turtle_high_best_cost_dic = temp_dic; }


private:
    string _lib_file;
    string _cost_exe;
    string _output_file;
    string _temp_file;
    string _module_name;
    string _low_effort_file_name = "low_effort.v";
    string _turtle_low_effort_file_name = "turtle_low_effort.v";
    string _high_effort_file_name = "high_effort.v";
    string _turtle_high_effort_file_name = "turtle_high_effort.v";
    bool _not_penalty = false;
    bool _add_const = false;
    map<string, string> name_to_type;
    bool pDch = false;
    bool fDch = true;
    bool add_buf = false;
    int _buf_num = 3;
    bool aMfs3 = true;
    bool pNf = true;
    int FNf = 10;
    int ENf = 100; // 0 -> 100
    int QNf = 100; // internal parameter impacting area of the mapping [default = 0]
    int RNf = 1000; // the delay relaxation ratio (num >= 0) [default = 0]
    // bool use_super = true;
    //// above is for area oriented
    map<string, pair<string, double>> best_cost_dic;
    map<string, pair<string, double>> low_best_cost_dic;
    map<string, pair<string, double>> high_best_cost_dic;
    map<string, pair<string, double>> turtle_low_best_cost_dic;
    map<string, pair<string, double>> turtle_high_best_cost_dic;
    bool has_timing = false;
};

#endif