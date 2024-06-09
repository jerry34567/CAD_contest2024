#ifndef COST_H
#define COST_H

#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>
#include "base/abc/abc.h"
#include "gvAbcMgr.h"

using namespace std;

string exec(const string& cmd, const string& args);
float extractCost(const string& output);
double cost_func(double, double);
double cost_diff(double, double, double);
double cost_cal(const string&, const string&, const string&);

#endif