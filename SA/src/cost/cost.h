#ifndef COST_H
#define COST_H

#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "base/abc/abc.h"
#include "gvAbcMgr.h"

using namespace std;

class CostMgr;
extern CostMgr* costMgr;
string exec(const string& cmd, const string& args);
float extractCost(const string& output);
double cost_func(double, double);
double cost_diff(double, double, double);

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
    double cost_cal(bool use_output, bool buf_flag);
    double cost_cal_use_map(bool use_output, bool buf_flag);
    double cost_cal_use_turtle(bool use_output, bool buf_flag, bool use_temp_lib);
    void set_module_name(string module_name) { _module_name = module_name; }
    string get_module_name() { return _module_name; }
    void add_name_to_type(string name, string type) {
        name_to_type[name] = type;
    }
    string get_type(string name) { return name_to_type[name]; }
    string get_lib() { return _lib_file; }
    string get_cost_exe() { return _cost_exe; }

private:
    string _lib_file;
    string _cost_exe;
    string _output_file;
    string _temp_file;
    string _module_name;
    map<string, string> name_to_type;
};

#endif