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
#include <map>

enum class INST2{
    add_action      = 0,
    delete_action   = 1,
    swap_action     = 2,
    move_action     = 3,
    change_action   = 4
};

enum class ACTION2{
    REWRITE      = 0,
    REFACTOR     = 1,
    RESUB        = 2,
    ORCHESTRATE  = 3,
    IFRAIG       = 4,
    DC2          = 5,
    BALANCE      = 6,
    SHARE        = 7,
    ABC9BALANCE  = 8,  //  &b -d -a
    ABC9BLUT     = 9,  //  &blut -m -r -a     performs AIG balancing for the given LUT size
    ABC9DC2      = 10, //  &dc2 -l   performs heavy rewriting of the AIG
    ABC9DSDB     = 11, //  &dsdb     performs DSD balancing
    ABC9SOPB     = 12, //  &sopb     performs SOP balancing
    DRWSAT       = 13, //  drwsat -b
    ABC9DEEPSYN  = 14, //  "&dch%s; &if -a -K %d; &mfs -dae -W 20 -L 0-100; &st; &put;"
    ABC9JF       = 15, //  &dch -fpx; &if -aph -K %d; &st; &put;
    RESYN        = 16,
    RESYN2       = 17,
    RESYN2a      = 18,
    RESYN3       = 19,
    COMPRESS     = 20,
    COMPRESS2    = 21,
    RWSAT        = 22,
    DRWSAT2      = 23,
    DC3          = 24,
    DC4          = 25,
    SRCRW        = 26,
    SRCRS        = 27,
    SRCRWS       = 28,
    R2RS         = 29,
    C2RS         = 30
};
    // phase_map

INST2 get_inst2();
ACTION2 get_action2();
vector<string> new_cmd_simulated_annealing(double&, double&, bool, vector<string>&, map<string, pair<string, double>>&);
vector<string> new_cmd_simulated_annealing_using_map(double&, bool, vector<string>&);
vector<string> new_cmd_simulated_annealing_using_turtle(double&, double&, bool, vector<string>&, map<string, pair<string, double>>&);
string get_command(ACTION2);

#endif