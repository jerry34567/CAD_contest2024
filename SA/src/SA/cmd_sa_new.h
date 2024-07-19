#ifndef CMD_SA_NEW_H
#define CMD_SA_NEW_H

#include <cmath>
#include <ctime>
#include <random>
#include <utility>
#include "base/abc/abc.h"
#include "cost.h"
#include "util.h"
#include "gvAbcMgr.h"

enum class INST2{
    add_action      = 0,
    delete_action   = 1,
    swap_action     = 2,
    move_action     = 3    
};

enum class ACTION2{
    REWRITE      = 0,
    REFACTOR     = 1,
    RESUB        = 2,
    ORCHESTRATE  = 3,
    IFRAIG       = 4,
    DC2          = 5,
    BALANCE      = 6,
    SHARE        = 7
};

INST2 get_inst2();
vector<string> new_cmd_simulated_annealing(double&, bool, vector<string>&);
vector<string> new_cmd_simulated_annealing_using_map(double&, bool, vector<string>&);
vector<string> new_cmd_simulated_annealing_using_turtle(double&, bool, vector<string>&);
#endif