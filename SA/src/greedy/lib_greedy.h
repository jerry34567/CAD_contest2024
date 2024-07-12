#ifndef LIB_GREEDY_H
#define LIB_GREEDY_H

#include <map>
#include "base/abc/abc.h"
#include "gvAbcMgr.h"
#include "parser.h"

using namespace std;

double do_action(int, map<string, pair<string, float>>&, map<string, vector<float>>&, bool);
double do_action_use_turtle(int, map<string, pair<string, float>>&, map<string, vector<float>>&, bool);
void lib_greedy(double&, const map<string, pair<string, float>>&, const map<string, vector<float>>&, bool);
void lib_greedy_using_turtle(double&, const map<string, pair<string, float>>&, const map<string, vector<float>>&, bool);

#endif