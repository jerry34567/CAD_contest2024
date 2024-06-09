#ifndef LIB_GREEDY_H
#define LIB_GREEDY_H

#include <map>
#include "base/abc/abc.h"
#include "gvAbcMgr.h"
#include "parser.h"

using namespace std;

double do_action(int, const string&, const string&, map<string, pair<string, float>>&, map<string, vector<float>>&);
void lib_greedy(const string&, const string&, double&, const string&, const map<string, pair<string, float>>&, const map<string, vector<float>>&);

#endif