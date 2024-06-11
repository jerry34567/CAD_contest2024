#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <array>
#include <sstream>
#include <regex>
#include "cost.h"


using json = nlohmann::json;
using namespace std;

class Cell {
public:
    string cell_name;
    string cell_type;
    vector<int> integer_attributes;
    vector<double> float_attributes;
};

extern unordered_map<string, string> dictionary;
extern unordered_map<string, string> dictionary2;
extern unordered_map<string, string> dictionary3;

void initDictionary();
void initDictionary2();
void initDictionary3();
void write_genlib(const string&, map<string, pair<string, float>>&, map<string, vector<float>>&);
void write_syntcl(const string&, const string&);
void initTimingDictionary(map<string, vector<float>>&);
void write_liberty(const string&, map<string, pair<string, float>>&);
int parser(const string&, const string&, const string&, map<string, pair<string, float>>&, map<string, vector<float>>&);



#endif