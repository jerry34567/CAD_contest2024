#ifndef LIB_GREEDY_H
#define LIB_GREEDY_H

#include <map>
#include "base/abc/abc.h"
#include "gvAbcMgr.h"
#include "libMgr.h"
#include "parser.h"

using namespace std;

double do_action(int, map<string, pair<string, double>>&, map<string, vector<double>>&, bool);
double do_action_use_turtle(int, map<string, pair<string, double>>&, map<string, vector<double>>&, bool);
void lib_greedy(double&, double&, map<string, pair<string, double>>&, map<string, vector<double>>&, bool);
void lib_greedy_using_turtle(double&, double&, map<string, pair<string, double>>&, map<string, vector<double>>&, bool);

#endif