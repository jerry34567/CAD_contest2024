#ifndef CMD_SA_H
#define CMD_SA_H

#include <cmath>
#include <ctime>
#include <random>
#include <utility>
#include "base/abc/abc.h"
#include "cost.h"
#include "util.h"
#include "gvAbcMgr.h"

enum class INST{
    REWRITE      = 0,
    REFACTOR     = 1,
    RESUB        = 2,
    ORCHESTRATE  = 3,
    IFRAIG       = 4,
    DC2          = 5,
    BALANCE      = 6,
    SHARE        = 7
};

static const std::string inst_strings[] = {"rw", "rf", "rs", "orchestrate", "ifraig", "dc2", "balance"};

INST get_inst();
vector<string> cmd_simulated_annealing(double&, bool);
vector<string> cmd_simulated_annealing_using_map(double&, bool);
vector<string> cmd_simulated_annealing_using_turtle(double&, bool);
#endif