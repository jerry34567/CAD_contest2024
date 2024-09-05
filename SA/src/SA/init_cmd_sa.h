#ifndef INIT_CMD_SA_H
#define INIT_CMD_SA_H

#include "cmd_sa_new.h"

vector<string> init_cmd_simulated_annealing(double&, double&, bool, map<string, pair<string, double>>&);
vector<string> init_cmd_simulated_annealing_using_turtle(double&, double&, bool, map<string, pair<string, double>>&);

#endif