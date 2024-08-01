#ifndef LIB_SA_H
#define LIB_SA_H

#include <cmath>
#include <ctime>
#include <random>
#include <utility>
#include "cost.h"
#include "util.h"
#include "base/abc/abc.h"
#include "gvAbcMgr.h"
#include "libMgr.h"
#include "parser.h"

enum class ACTION{
    AND_ADD      = 0,
    AND_SUB      = 1,
    NAND_ADD     = 2,
    NAND_SUB     = 3,
    NOR_ADD      = 4,
    NOR_SUB      = 5,
    OR_ADD       = 6,
    OR_SUB       = 7,
    NOT_ADD      = 8,
    NOT_SUB      = 9,
    XOR_ADD      = 10,
    XOR_SUB      = 11,
    XNOR_ADD     = 12,
    XNOR_SUB     = 13,
    BUF_ADD      = 14,
    BUF_SUB      = 15,
    AND_ADD_FT   = 16,
    AND_SUB_FT   = 17,
    NAND_ADD_FT  = 18,
    NAND_SUB_FT  = 19,
    NOR_ADD_FT   = 20,
    NOR_SUB_FT   = 21,
    OR_ADD_FT    = 22,
    OR_SUB_FT    = 23,
    NOT_ADD_FT   = 24,
    NOT_SUB_FT   = 25,
    XOR_ADD_FT   = 26,
    XOR_SUB_FT   = 27,
    XNOR_ADD_FT  = 28,
    XNOR_SUB_FT  = 29,
    BUF_ADD_FT   = 30,
    BUF_SUB_FT   = 31,
};

ACTION get_action(bool);
void lib_simulated_annealing(double&, double&, map<string, pair<string, double>>&, map<string, vector<double>>&, bool);
void lib_simulated_annealing_using_turtle(double&, double&, map<string, pair<string, double>>&, map<string, vector<double>>&, bool);
#endif