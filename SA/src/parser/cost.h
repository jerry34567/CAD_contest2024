#ifndef COST_H
#define COST_H

#include <cstdlib>
#include <memory>
#include <iostream>
#include <sstream>

using namespace std;

string exec(const string& cmd, const string& args);
float extractCost(const string& output);

#endif