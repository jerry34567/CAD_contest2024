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
    REWRITEZ     = 1,
    REFACTOR     = 2,
    REFACTORZ    = 3,
    RESUB        = 4,
    ORCHESTRATE  = 5,
    IFRAIG       = 6,
    DC2          = 7,
    BALANCE      = 8,
};

static const std::string inst_strings[] = {"rw", "rwz", "rf", "rfz", "rs",
 "orchestrate", "ifraig", "dc2", "balance"};

INST get_inst();
vector<string> cmd_simulated_annealing(const string&, const string&, double&, const string&);
#endif