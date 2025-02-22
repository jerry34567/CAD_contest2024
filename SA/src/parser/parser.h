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
#include <unordered_set>


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

string extractModuleName(const string& filePath);
void initDictionary();
void initDictionary2();
void initDictionary3();
void write_genlib(const string&, map<string, pair<string, double>>&, map<string, vector<double>>&, bool, bool&);
void write_syntcl(const string&, const string&);
void initTimingDictionary(map<string, vector<double>>&);
void write_liberty(const string&, map<string, pair<string, double>>&);
unordered_map<string, unordered_set<string>> parser(const string&, const string&, const string&, map<string, pair<string, double>>&, map<string, vector<double>>&, bool&);



#endif