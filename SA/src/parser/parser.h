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

int parser(const string&, const string&, const string&);

#endif